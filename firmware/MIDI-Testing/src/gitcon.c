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

static const char *TAG = "gitcon";

static TaskHandle_t midi_task_handle;
static TaskHandle_t dsp_task_handle;

// ------------------------------------------------------------
// ISR and static functions
// ------------------------------------------------------------

static void IRAM_ATTR dsp_task(void *arg)
{
	gitcon_handle_t handle = (gitcon_handle_t)arg;
	uint16_t *audio_buffer = NULL;

	float float_audio[FFT_SIZE];
	float fft_buffer[FFT_SIZE];
	float magnitude[FFT_SIZE / 2];
	float frequency[FFT_SIZE / 2];
	float keyNR[FFT_SIZE / 2];
	float ratio = (float)F_SAMPLE_HZ / (float)FFT_SIZE;
	unsigned char active_notes[127] = {0};

	for (;;)
	{
		// ------------------------------------------------------------
		// DSP STEPS
		// ------------------------------------------------------------

		// 1. read ADC to DMA buffer
		if (xQueueReceive(handle->sampler->dsp_queue, &audio_buffer, portMAX_DELAY) == pdTRUE)
		{

			magnitude[k] = 2 * sqrt(pow(fft_buffer[2 * k], 2) + pow(fft_buffer[2 * k + 1], 2)) / FFT_SIZE;
			frequency[k] = k * ratio;
			keyNR[k] = log2(frequency[k] / 440) * 12 + 49;

			if (magnitude[k] >= 0.5)
			{
				ESP_LOGI(TAG, "%d-th magnitude: %f => corresponds to %f Hz\n", k, magnitude[k], frequency[k]);
				ESP_LOGI(TAG, "keyNR: %d\n", (int)round(keyNR[k]));
				active_notes[k] = 1;
				xQueueSend(gitcon_handle->midi_queue, &active_notes, portMAX_DELAY);
			}
			else
			{
				active_notes[k] = 0;
			}
		}

		fft_destroy(real_fft_plan);

		// 1. read ADC to DMA buffer

		// 2. analyze audio data (FFT, etc.)
		// 3. detect fundamental frequencies and convert to note number on piano roll
		// 4. detect if frequency is transient
		// 4.1 save note on transient
		// 5. check if already on notes are below a certain threshold
		// 5.1 delete saved note
		// 6. send saved notes to MIDI queue

		// (!note) velocity of the note is determined by the initial amplitude of a transient frequency

		// at a later point, the message should be created from the DSP result
		// eventually, the message should be created in the MIDI task and not in the DSP task
		// instead, the DSP task should send the rawest possible data to the MIDI task
		// the MIDI task should then create the MIDI message from the raw data
		// the raw data could be the a buffer in which, currently on/off notes are stored
		// (!note) velocity of the note is determined by the initial amplitude of a transient frequency

		ESP_LOGI(TAG, "Sending MIDI message from DSP task");

		// send dummy message
		/*midi_message_t msg = {
			.status = test_status,
			.channel = 0,
			.param1 = 0x3C, // C4
			.param2 = 0x7F};
		// toggle note on/off for testing purposes
		test_status = (test_status == MIDI_STATUS_NOTE_OFF) ? MIDI_STATUS_NOTE_ON : MIDI_STATUS_NOTE_OFF;
		*/
		vTaskDelay(1000 / portTICK_PERIOD_MS);
	}
}

static void midi_task(void *arg) // TODO: notesending with bool array
{
	gitcon_handle_t gitcon_handle = (gitcon_handle_t)arg;
	unsigned char active_notes[127] = {0};
	unsigned char previous_notes[127] = {0};

	for (;;)
	{
		*previous_notes = *active_notes;
		if (xQueueReceive(gitcon_handle->midi_queue, &active_notes, portMAX_DELAY) == pdTRUE)
		{
			// send message to MIDI UART
			for (size_t i = 0; i < 127; i++)
			{
				if (active_notes[i] != previous_notes[i])
				{
					if (active_notes[i] == 1)
					{
						midi_message_t msg = {
							.status = MIDI_STATUS_NOTE_ON,
							.channel = 0,
							.param1 = i,
							.param2 = 0x7F};
					}
					else
					{
						midi_message_t msg = {
							.status = MIDI_STATUS_NOTE_OFF,
							.channel = 0,
							.param1 = i,
							.param2 = 0x7F};
					}
				}
				ESP_ERROR_CHECK(midi_write(gitcon_handle->midi_handle, &msg));
				*previous_notes = *active_notes;
			}
		}
		vTaskDelay(10 / portTICK_PERIOD_MS);
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

	// creat queue for audio data (passed into sampler)
	QueueHandle_t dsp_queue = xQueueCreate(10, sizeof(size_t *));

	// create queue for midi messages
	gitcon_cfg->midi_queue = xQueueCreate(5, sizeof(midi_message_t));
	if (!gitcon_cfg->midi_queue)
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
	mcp3201_handle_t mcp_handle;
	mcp3201_config_t mcp_cfg = {
		.host = SPI_DEV,
		.cs_io = SPI_CS,
		.miso_io = SPI_MISO,
		.mosi_io = SPI_MOSI};
	// initialize ADC and store in gitcon handle
	ESP_ERROR_CHECK(mcp3201_init(&mcp_handle, &mcp_cfg));
	gitcon_cfg->sampler = mcp3201_sampler_start(mcp_handle, dsp_queue, AUDIO_BUFFER_SIZE, F_SAMPLE_HZ);
#else
	gitcon_cfg->sampler = i2s_sampler_start(INTERNAL_ADC_CHANNEL, dsp_queue, AUDIO_BUFFER_SIZE, F_SAMPLE_HZ);
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

	ESP_LOGI(TAG, "Creating RTOS tasks...");

	gitcon_cfg->midi_queue = xQueueCreate(10, sizeof(unsigned char *));
	if (!gitcon_cfg->midi_queue)
		return ESP_ERR_NO_MEM;

	// DSP task: receives audio data from DMA task and sends midi messages to midi task
	if (xTaskCreatePinnedToCore(dsp_task, "dsp_task", 1 << 16, gitcon_cfg, 5, &dsp_task_handle, 1) == pdFALSE)
		return ESP_ERR_NO_MEM;

	// MIDI task: receives midi messages from DSP task and sends them to MIDI UART
	if (xTaskCreatePinnedToCore(midi_task, "midi_task", 2048, gitcon_cfg, 5, &midi_task_handle, 0) == pdFALSE)
		return ESP_ERR_NO_MEM;

	// Pass final configuration to outer parameters
	*out_handle = gitcon_cfg;
	return ESP_OK;
}

esp_err_t gitcon_exit(gitcon_handle_t handle)
{
	ESP_ERROR_CHECK(midi_exit(handle->midi_handle));

	// stop tasks
	vTaskDelete(dsp_task_handle);
	vTaskDelete(midi_task_handle);

	// stop sampler
#ifdef USE_MCP3201
	mcp3201_sampler_stop(handle->sampler->mcp_handle);
	ESP_ERROR_CHECK(mcp3201_exit(handle->sampler->mcp_handle));
#else
	i2s_sampler_stop(handle->sampler);
#endif

	free(handle);
	return ESP_OK;
}