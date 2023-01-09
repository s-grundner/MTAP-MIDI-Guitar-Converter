/**
 * @file gitcon.c
 * @author @s-grundner
 * @brief
 * @version 0.1
 * @date 2022-12-23
 *
 * @copyright Copyright (c) 2022
 *
 */

#include "gitcon.h"

static const char *TAG = "gitcon";

// ------------------------------------------------------------
// ISR and static functions
// ------------------------------------------------------------

static void IRAM_ATTR dma_task(void *arg)
{
	gitcon_handle_t handle = (gitcon_handle_t)arg;
	for (;;)
	{
		// Do DMA here

		vTaskDelay(1000 / portTICK_PERIOD_MS);
	}
}

static void IRAM_ATTR dsp_task(void *arg)
{
	midi_status_t status = MIDI_STATUS_NOTE_OFF;

	fft_config_t *fft_config = fft_init(FFT_SIZE, FFT_REAL, FFT_FORWARD, NULL, NULL);
	gitcon_handle_t handle = (gitcon_handle_t)arg;
	for (;;)
	{
		// Do DSP here

		fft_execute(fft_config);

		// ------------------------------------------------------------
		// DSP STEPS
		// ------------------------------------------------------------

		// 1. read ADC to DMA buffer
		// 2. analyze audio data (FFT, etc.)
		// 3. detect fundamental frequencies and convert to note number on piano roll
		// 4. detect if frequency is transient
		// 4.1 save note on transient
		// 5. check if already on notes are below a certain threshold
		// 5.1 delete saved note
		// 6. send saved notes to MIDI queue

		fft_destroy(fft_config);
		vTaskDelay(1000 / portTICK_PERIOD_MS);

		// toggle note on/off for testing purposes
		status = (status == MIDI_STATUS_NOTE_ON) ? MIDI_STATUS_NOTE_OFF : MIDI_STATUS_NOTE_ON;

		// send processed audio data to MIDI queue
		ESP_LOGI(TAG, "Sending MIDI message from DSP task");

		// send dummy message
		midi_message_t message = {
			.status = status,
			.channel = 0,
			.param1 = 0x3C, // C4
			.param2 = 0x7F};

		xQueueSend(handle->midi_queue, &message, portMAX_DELAY);
	}
}

static void midi_task(void *arg)
{
	gitcon_handle_t handle = (gitcon_handle_t)arg;
	midi_message_t message;
	for (;;)
	{
		if (xQueueReceive(handle->midi_queue, &message, portMAX_DELAY) == pdTRUE)
		{
			// Send MIDI here
			ESP_LOGI(TAG, "Receiving MIDI message and sending to UART");
			midi_send(MIDI_UART, &message);
		}
		vTaskDelay(1000 / portTICK_PERIOD_MS);
	}
}

typedef struct
{
	uart_port_t uart_num;
	gpio_num_t rx_io;
	gpio_num_t tx_io;
	uint baudrate;
} midi_user_args_t;

static void midi_user_init(void *arg)
{
	midi_user_args_t *user = (midi_user_args_t *)arg;
	// ------------------------------------------------------------
	// INIT UART DRIVER
	// ------------------------------------------------------------

	gpio_config_t rx_pin_config = {
		.pin_bit_mask = (1ULL << user->rx_io),
		.mode = GPIO_MODE_INPUT,
		.pull_up_en = GPIO_PULLUP_DISABLE,
		.pull_down_en = GPIO_PULLDOWN_DISABLE,
		.intr_type = GPIO_INTR_DISABLE};

	gpio_config_t tx_pin_config = {
		.pin_bit_mask = (1ULL << user->tx_io),
		.mode = GPIO_MODE_OUTPUT,
		.pull_up_en = GPIO_PULLUP_DISABLE,
		.pull_down_en = GPIO_PULLDOWN_DISABLE,
		.intr_type = GPIO_INTR_DISABLE};

	ESP_ERROR_CHECK(gpio_config(&rx_pin_config));
	ESP_ERROR_CHECK(gpio_config(&tx_pin_config));

	uart_config_t uart_config = {
		.baud_rate = user->baudrate,
		.data_bits = UART_DATA_8_BITS,
		.parity = UART_PARITY_DISABLE,
		.stop_bits = UART_STOP_BITS_1,
		.flow_ctrl = UART_HW_FLOWCTRL_DISABLE,
		.source_clk = UART_SCLK_APB,
	}; // Configure UART parameters

	ESP_ERROR_CHECK(uart_param_config(user->uart_num, &uart_config));
	ESP_ERROR_CHECK(uart_set_pin(user->uart_num, user->tx_io, user->rx_io, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE));
	ESP_ERROR_CHECK(uart_driver_install(user->uart_num, 1024 * 2, 1024 * 2, 0, NULL, 0));
}

static void midi_user_exit(void *arg)
{
	midi_user_args_t *user = (midi_user_args_t *)arg;
	uart_driver_delete(user->uart_num);
}

static void midi_user_send(void *arg, midi_message_t *msg, midi_byte_size_t byte_size)
{
	midi_user_args_t *user = (midi_user_args_t *)arg;

	if (uart_write_bytes(user->uart_num, msg, byte_size) < 0)
	{
		ESP_LOGE("uart", "uart_write_bytes failed");
		ESP_ERROR_CHECK(ESP_FAIL);
	}
}

// ------------------------------------------------------------
// non-static functions
// ------------------------------------------------------------

esp_err_t gitcon_init(gitcon_context_t **out_handle)
{
	esp_log_level_set(TAG, GITCON_LOG_LEVEL);

	gitcon_context_t *cfg = (gitcon_context_t *)malloc(sizeof(gitcon_context_t));
	if (!cfg)
		return ESP_ERR_NO_MEM;

#ifdef USE_MCP3201
	// ------------------------------------------------------------
	// SPI
	// ------------------------------------------------------------
	spi_bus_config_t bus_cfg = {
		.miso_io_num = SPI_MISO,
		.mosi_io_num = SPI_MOSI,
		.sclk_io_num = SPI_SCLK,
		.max_transfer_sz = 32,
	};
	spi_bus_initialize(SPI_DEV, &bus_cfg, DMA_CHAN);

	// ------------------------------------------------------------
	// MCP3201 (ADC)
	// ------------------------------------------------------------

	mcp3201_config_t adc_cfg = {
		.host = SPI_DEV,
		.cs_io = SPI_CS,
		.miso_io = SPI_MISO,
		.mosi_io = SPI_MOSI};

	mcp3201_handle_t adc_handle;
	ESP_ERROR_CHECK(mcp3201_init(&adc_handle, &adc_cfg));
	cfg->mcp3201 = adc_handle;
#endif

	// ------------------------------------------------------------
	// SETUP INTERNAL I2S ADC
	// ------------------------------------------------------------

#ifdef USE_INTERNAL_ADC
	i2s_config_t i2s_cfg = {
		.mode = (i2s_mode_t)(I2S_MODE_MASTER | I2S_MODE_RX | I2S_MODE_ADC_BUILT_IN),
		.sample_rate = 40000,
		.bits_per_sample = I2S_BITS_PER_SAMPLE_16BIT,
		.channel_format = I2S_CHANNEL_FMT_ONLY_LEFT,
		.communication_format = I2S_COMM_FORMAT_STAND_I2S,
		.intr_alloc_flags = ESP_INTR_FLAG_LEVEL1,
		.dma_buf_count = 4,
		.dma_buf_len = 1024,
		.use_apll = false,
		.fixed_mclk = 0};

	ESP_ERROR_CHECK(i2s_driver_install(I2S_NUM_0, &i2s_cfg, 0, NULL));
	ESP_ERROR_CHECK(i2s_set_adc_mode((adc_unit_t)INTERNAL_ADC_UNIT, (adc1_channel_t)INTERNAL_ADC));
	ESP_ERROR_CHECK(i2s_adc_enable(I2S_NUM_0));
#endif

	// ------------------------------------------------------------
	// MIDI
	// ------------------------------------------------------------

	midi_handle_t midi_handle;
	midi_user_args_t midi_user = {
		.uart_num = MIDI_UART,
		.baudrate = MIDI_BAUD,
		.rx_io = MIDI_RX,
		.tx_io = MIDI_TX};

	midi_config_t midi_cfg = {
		.user = &midi_user,
		.user_init = midi_user_init,
		.user_exit = midi_user_exit,
		.user_send = midi_user_send};

	ESP_ERROR_CHECK((esp_err_t)midi_init(&midi_handle, &midi_cfg));

	// ------------------------------------------------------------
	// INIT RTOS
	// ------------------------------------------------------------

	// Create a queue to send midi messages whenever a note is detected
	if (!(cfg->midi_queue = xQueueCreate(10, sizeof(midi_message_t))))
		return ESP_ERR_NO_MEM;

	// Midi task: receives midi messages from DSP task and sends them to UART
	if (xTaskCreatePinnedToCore(midi_task, "midi_task", 2048, cfg, 5, NULL, 0) == pdFALSE)
		return ESP_ERR_NO_MEM;
	// DMA task: reads audio data from ADC and sends it to DSP task
	if (xTaskCreatePinnedToCore(dma_task, "dma_task", 2048, cfg, 5, NULL, 0) == pdFALSE)
		return ESP_ERR_NO_MEM;
	// DSP task: receives audio data from DMA task and sends midi messages to midi task
	if (xTaskCreatePinnedToCore(dsp_task, "dsp_task", 2048, cfg, 5, NULL, 1) == pdFALSE)
		return ESP_ERR_NO_MEM;

	*out_handle = cfg;
	return ESP_OK;
}

esp_err_t gitcon_exit(gitcon_handle_t handle)
{
	midi_exit(handle->midi_handle);

#ifdef USE_MCP3201
	ESP_ERROR_CHECK(mcp3201_exit(handle->mcp3201));
#endif

#ifdef USE_INTERNAL_ADC
	ESP_ERROR_CHECK(i2s_driver_uninstall(I2S_NUM_0));
#endif

	free(handle);
	return ESP_OK;
}