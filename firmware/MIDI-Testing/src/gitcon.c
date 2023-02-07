/**
 * @file gitcon.c
 * @author @s-grundner @l-hoelzl
 * @brief
 * @version 0.1
 * @date 2022-12-23
 *
 * @copyright Copyright (c) 2022
 *
 */

#include "gitcon.h"
#include "processed-data.h"

static const char *TAG = "gitcon";

// ------------------------------------------------------------
// ISR and static functions
// ------------------------------------------------------------
static void dma_task(void *arg)
{
	for (;;)
	{
		// Do DMA here
		vTaskDelay(1000 / portTICK_PERIOD_MS);
	}
}

static void dsp_task(void *arg)
{
	gitcon_handle_t gitcon_handle = (gitcon_handle_t)arg;
	midi_status_t test_status = MIDI_STATUS_NOTE_OFF;
	float fft_buffer[FFT_SIZE];
	float magnitude[FFT_SIZE / 2];
	float frequency[FFT_SIZE / 2];
	float keyNR[FFT_SIZE / 2];
	float ratio = (float)F_SAMPLE_HZ / (float)FFT_SIZE;
	char file_buffer[FFT_SIZE];

	for (;;)
	{
		// Do DSP here

		// ------------------------------------------------------------
		// DSP STEPS
		// ------------------------------------------------------------

		fft_config_t *real_fft_plan = fft_init(FFT_SIZE, FFT_REAL, FFT_FORWARD, test_buffer, fft_buffer);

		fft_execute(real_fft_plan);

		for (int k = 1; k < FFT_SIZE / 2; k++)
		{
			magnitude[k] = 2 * sqrt(pow(fft_buffer[2 * k], 2) + pow(fft_buffer[2 * k + 1], 2)) / FFT_SIZE;
			frequency[k] = k * ratio;
			keyNR[k] = log2(frequency[k] / 440) * 12 + 49;

			if (magnitude[k] >= 0.5)
			{
				ESP_LOGI(TAG, "%d-th magnitude: %f => corresponds to %f Hz\n", k, magnitude[k], frequency[k]);
				ESP_LOGI(TAG, "keyNR: %d\n", (int)round(keyNR[k]));
			}
		}

		// 1. read ADC to DMA buffer
		// 2. analyze audio data (FFT, etc.)
		// 3. detect fundamental frequencies and convert to note number on piano roll
		// 4. detect if frequency is transient
		// 4.1 save note on transient ()
		// 5. check if already on notes are below a certain threshold
		// 5.1 delete saved note
		// 6. send saved notes to MIDI queue

		// at a later point, the message should be created from the DSP result
		// eventually, the message should be created in the MIDI task and not in the DSP task
		// instead, the DSP task should send the rawest possible data to the MIDI task
		// the MIDI task should then create the MIDI message from the raw data
		// the raw data could be the a buffer in which, currently on/off notes are stored

		// (!note) velocity of the note is determined by the initial amplitude of a transient frequency

		ESP_LOGI(TAG, "Sending MIDI message from DSP task");

		// send dummy message
		midi_message_t msg = {
			.status = test_status,
			.channel = 0,
			.param1 = 0x3C, // C4
			.param2 = 0x7F};
		// toggle note on/off for testing purposes
		test_status = (test_status == MIDI_STATUS_NOTE_OFF) ? MIDI_STATUS_NOTE_ON : MIDI_STATUS_NOTE_OFF;

		xQueueSend(gitcon_handle->midi_queue, &msg, portMAX_DELAY);
		vTaskDelay(1000 / portTICK_PERIOD_MS);
	}
}

static void midi_task(void *arg)
{
	gitcon_handle_t gitcon_handle = (gitcon_handle_t)arg;
	midi_message_t msg;
	for (;;)
	{
		if (xQueueReceive(gitcon_handle->midi_queue, &msg, portMAX_DELAY) == pdTRUE)
		{
			// send message to MIDI UART
			ESP_ERROR_CHECK(midi_write(gitcon_handle->midi_handle, &msg));
		}
		vTaskDelay(1000 / portTICK_PERIOD_MS);
	}
}

// ------------------------------------------------------------
// non-static functions
// ------------------------------------------------------------

esp_err_t gitcon_init(gitcon_context_t **out_handle)
{
	gitcon_context_t *gitcon_cfg = (gitcon_context_t *)malloc(sizeof(gitcon_context_t));
	if (!gitcon_cfg)
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
	// setup ADC
	mcp3201_handle_t adc_handle;
	mcp3201_config_t adc_cfg = {
		.host = SPI_DEV,
		.cs_io = SPI_CS,
		.miso_io = SPI_MISO,
		.mosi_io = SPI_MOSI};
	// initialize ADC and store in gitcon handle
	ESP_ERROR_CHECK(mcp3201_init(&adc_handle, &adc_cfg));
	cfg->mcp3201 = adc_handle;
#endif

#ifdef USE_INTERNAL_ADC
	// ------------------------------------------------------------
	// SETUP INTERNAL I2S ADC
	// ------------------------------------------------------------

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

	// Configure a handle parameter for MIDI
	midi_handle_t midi_handle;
	midi_config_t midi_cfg = {
		.uart_num = MIDI_UART,
		.baudrate = MIDI_BAUD,
		.rx_io = MIDI_RX,
		.tx_io = MIDI_TX};
	// Initialize MIDI and store in gitcon handle
	ESP_ERROR_CHECK(midi_init(&midi_handle, &midi_cfg));
	gitcon_cfg->midi_handle = midi_handle;

	// ------------------------------------------------------------
	// INIT RTOS
	// ------------------------------------------------------------

	gitcon_cfg->midi_queue = xQueueCreate(10, sizeof(midi_message_t *));
	if (!gitcon_cfg->midi_queue)
		return ESP_ERR_NO_MEM;

	// DMA task: reads audio data from ADC and sends it to DSP task
	if (xTaskCreatePinnedToCore(dma_task, "dma_task", 2048, gitcon_cfg, 5, NULL, 0) == pdFALSE)
		return ESP_ERR_NO_MEM;
	// DSP task: receives audio data from DMA task and sends midi messages to midi task
	if (xTaskCreatePinnedToCore(dsp_task, "dsp_task", 65536, gitcon_cfg, 5, NULL, 1) == pdFALSE)
		return ESP_ERR_NO_MEM;
	// MIDI task: receives midi messages from DSP task and sends them to MIDI UART
	if (xTaskCreatePinnedToCore(midi_task, "midi_task", 2048, gitcon_cfg, 5, NULL, 0) == pdFALSE)
		return ESP_ERR_NO_MEM;

	// Pass final configuration to outer parameters
	*out_handle = gitcon_cfg;
	return ESP_OK;
}

esp_err_t gitcon_exit(gitcon_handle_t handle)
{
	ESP_ERROR_CHECK(midi_exit(handle->midi_handle));

#ifdef USE_MCP3201
	ESP_ERROR_CHECK(mcp3201_exit(handle->mcp3201));
#endif

#ifdef USE_INTERNAL_ADC
	ESP_ERROR_CHECK(i2s_driver_uninstall(I2S_NUM_0));
#endif
	free(handle);
	return ESP_OK;
}