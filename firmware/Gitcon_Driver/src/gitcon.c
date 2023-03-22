/**
 * @file gitcon.c
 * @author @s-grundner @Laurenz03
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

static TaskHandle_t midi_task_handle;
static TaskHandle_t dsp_task_handle;

#define FLOAT_TO_UINT16(x) ((uint16_t)((x)*32767.0f))
#define UINT16_TO_FLOAT(x) ((float)(x) / 32767.0f)

// ------------------------------------------------------------
// static functions
// ------------------------------------------------------------

/**
 * @brief Task to handle Digital Signal Processing
 * @param arg gitcon_handle_t, context handler
 */
static void dsp_task(void *arg)
{
	// parameter handler
	gitcon_handle_t gitcon_handle = (gitcon_handle_t)arg;
	uint16_t *audio_buffer = NULL;

	// fft variables
	float fft_buffer[FFT_SIZE];
	float magnitude[FFT_SIZE / 2];
	float frequency[FFT_SIZE / 2];
	unsigned char keyNR[FFT_SIZE / 2];
	float ratio = (float)F_SAMPLE_HZ / (float)FFT_SIZE;

	// active notes resulting from fft
	midi_message_t *active_notes = (midi_message_t *)malloc(128 * sizeof(midi_message_t));
	if (active_notes == NULL)
	{
		ESP_LOGE(TAG, "Could not allocate memory for active_notes");
		gitcon_exit(gitcon_handle);
		return;
	}

	// initialize active_notes
	for (int i = 0; i < 128; i++)
	{
		active_notes[i].channel = 0;
		active_notes[i].status = MIDI_STATUS_NOTE_OFF;
		active_notes[i].param1 = i;
		active_notes[i].param2 = 0;
	}

	for (;;)
	{
		/// @note velocity of the note is determined by the initial amplitude of a transient frequency
		// ------------------------------------------------------------
		// DSP STEPS
		// ------------------------------------------------------------

		///@note 1. read ADC to DMA buffer
		if (xQueueReceive(gitcon_handle->sampler->dsp_queue, &audio_buffer, portMAX_DELAY) == pdTRUE)
		{
			// do stuff with audio_buffer
			// this is where the audio buffer is available and the FFT is executed
			// convert a uint16_t array to a float array
			float audio_buffer_float[AUDIO_BUFFER_SIZE];
			for (int i = 0; i < AUDIO_BUFFER_SIZE; i++)
				audio_buffer_float[i] = UINT16_TO_FLOAT(audio_buffer[i]);

			///@note  2. analyze audio data (FFT, etc.)
			fft_config_t *real_fft_plan = fft_init(FFT_SIZE, FFT_REAL, FFT_FORWARD, audio_buffer_float, fft_buffer);
			if (real_fft_plan == NULL)
			{
				ESP_LOGE(TAG, "FFT plan could not be created");
				vTaskDelay(1000 / portTICK_PERIOD_MS);
				continue;
			}
			fft_execute(real_fft_plan);

			for (int k = 1; k < FFT_SIZE / 2; k++)
			{
				///@note  3. detect fundamental frequencies and convert to note number on piano roll
				keyNR[k] = (unsigned char)round(log2(frequency[k] / 440) * 12 + 69) % 128;
				magnitude[k] = 2 * sqrt(pow(fft_buffer[2 * k], 2) + pow(fft_buffer[2 * k + 1], 2)) / FFT_SIZE;
				frequency[k] = k * ratio;
			}

			float max = 0;
			for (int i = 0; i < FFT_SIZE / 2; i++)
				max = (magnitude[i] > max) ? magnitude[i] : max;

			if (max < 0.0005)
				max = 100;

			for (int k = 1; k < FFT_SIZE / 2; k++)
			{
				///@note  4. check if fundamental frequencies are above a certain threshold
				///@note  4.1 save note on transient
				///@note  5. check if already on notes are below a certain threshold and delete saved note

				if (magnitude[k] < max * 0.5)
				{
					active_notes[keyNR[k]].status = MIDI_STATUS_NOTE_OFF;
					continue;
				}
				active_notes[keyNR[k]].status = MIDI_STATUS_NOTE_ON;
				active_notes[keyNR[k]].param2 = (uint8_t)(magnitude[k] / max * 127);
			}
			///@note  6. send saved notes to MIDI queue
			xQueueSend(gitcon_handle->midi_queue, &active_notes, portMAX_DELAY);
			fft_destroy(real_fft_plan);
		}
		vTaskDelay(100 / portTICK_PERIOD_MS);
	} // for(;;)
} // dsp_task

static void midi_task(void *arg)
{
	gitcon_handle_t gitcon_handle = (gitcon_handle_t)arg;
	midi_message_t *active_notes = NULL;
	midi_status_t previous_states[128] = {0};

	for (;;)
	{
		if (xQueueReceive(gitcon_handle->midi_queue, &active_notes, portMAX_DELAY) == pdTRUE)
		{
			for (size_t i = 0; i < 128; i++)
			{
				if (active_notes[i].status == previous_states[i])
					continue;

				// send message to MIDI UART
				ESP_ERROR_CHECK(midi_write(gitcon_handle->midi_handle, &active_notes[i]));
				previous_states[i] = active_notes[i].status;
			}
		}
		vTaskDelay(10 / portTICK_PERIOD_MS);
	} // for(;;)
} // midi_task

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
	gitcon_cfg->midi_queue = xQueueCreate(5, sizeof(midi_handle_t *));
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