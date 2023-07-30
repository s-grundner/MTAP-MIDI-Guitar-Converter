/**
 * @file gitcon.c
 * @author @s-grundner @Laurenz03
 * @brief Gitcon Driver Source
 * @version 0.1
 * @date 2022-12-23
 *
 * @copyright Copyright (c) 2022
 *
 */

#include "gitcon.h"
#include "processed-data.h"

static const char *TAG = "gitcon";

static TaskHandle_t task_handle_midi;
static TaskHandle_t task_handle_dsp;

#define FLOAT_TO_UINT16(x) ((uint16_t)((x)*32767.0f))
#define UINT16_TO_FLOAT(x) ((float)(x) / 32767.0f)
#define SENSITIVITY 0.9f
#define MIDI_LOWEST_NOTE 21
#define MIDI_HIGHEST_NOTE 108
#define MIDI_KEY_BOUNDARY(x) ((x) < MIDI_LOWEST_NOTE || (x) > MIDI_HIGHEST_NOTE)
#define CONCERT_A 440.0f
#define CONCERT_A_NOTE 69
#define GAIN 1.0f

// uncomment to enable debug output for better_serial_plotter software
// #define DEBUG_BETTER_SERIAL_PLOTTER

// uncomment to enable debug output for dsp_task
// #define DEBUG_DSP

/**
 * @brief Gitcon Configuration
 * @param sampler Sampler Handler (MCP3201 or I2S)
 * @param midi_handle MIDI Driver Context (MIDI over UART)
 * @param midi_queue MIDI Queue Handler
 */
typedef struct gitcon_data_s
{
#ifdef USE_MCP3201
	mcp3201_sampler_t *sampler;
#else
	i2s_sampler_handle_t sampler;
#endif
	midi_handle_t midi_handle;
	QueueHandle_t midi_queue;
} gitcon_data_t;

// ------------------------------------------------------------
// static functions
// ------------------------------------------------------------

/**
 * @brief Task to handle Digital Signal Processing
 * @param arg gitcon_handle_t, context handler
 */
static void dsp_task(void *arg)
{
	// gitcon driver context handler
	gitcon_handle_t g_handle = (gitcon_handle_t)arg;

	// audio buffer variables
	uint16_t *audio_buffer = NULL;
	float *audio_buffer_float = (float *)malloc(FFT_SIZE * sizeof(float));
	if (audio_buffer_float == NULL)
	{
		ESP_LOGE(TAG, "Could not allocate memory for audio_buffer_float");
		gitcon_exit(g_handle);
		return;
	}

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
		gitcon_exit(g_handle);
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
		// delay to avoid overflow and to allow other tasks to run
		vTaskDelay(10 / portTICK_PERIOD_MS);

		// get audio buffer from sampler
		if (xQueueReceive(i2s_sampler_get_queue(g_handle->sampler), &audio_buffer, portMAX_DELAY) == pdFALSE)
			continue; // skip iteration if queue is empty

		///@note this is where the audio buffer is available and the FFT is executed
		///@note append the audio_buffer to audio_buffer_float

		for (int i = FFT_WINDOW_SIZE - 1; i > 0; i--)
		{
			memcpy(audio_buffer_float + (i * AUDIO_BUFFER_SIZE),
				   audio_buffer_float + ((i - 1) * AUDIO_BUFFER_SIZE),
				   AUDIO_BUFFER_SIZE * sizeof(float));
		}

		// starting in a new window and fill the buffer with the new data
		for (int i = 0; i < AUDIO_BUFFER_SIZE; i++)
			audio_buffer_float[i] = GAIN * UINT16_TO_FLOAT(audio_buffer[AUDIO_BUFFER_SIZE - 1 - i]);

#ifdef DEBUG_BETTER_SERIAL_PLOTTER
		for (int i = 0; i < FFT_SIZE; i++)
			printf("%f\n", audio_buffer_float[i]); // for debugging in BetterSerialPlotter
#endif
		///@TODO: high pass f_g ~ 30Hz

		///@note analyze audio data (FFT, etc.)
		fft_config_t *fft_plan = fft_init(FFT_SIZE, FFT_REAL, FFT_FORWARD, audio_buffer_float, fft_buffer);
		if (fft_plan == NULL)
		{
			ESP_LOGE(TAG, "FFT plan could not be created");
			vTaskDelay(1000 / portTICK_PERIOD_MS);
			continue;
		}
		fft_execute(fft_plan);

		for (int k = 1; k < FFT_SIZE / 2; k++)
		{
			// detect fundamental frequencies
			frequency[k] = k * ratio;
			// convert to note number on piano roll
			keyNR[k] = (unsigned char)round(log2(frequency[k] / CONCERT_A) * 12 + CONCERT_A_NOTE) % 128;
			// calculate magnitude (absolute value of complex number)
			magnitude[k] = GAIN * 2 * sqrt(pow(fft_buffer[2 * k], 2) + pow(fft_buffer[2 * k + 1], 2)) / FFT_SIZE;
		}

		// calculate max magnitude for threshholding
		float max = 0;
		for (int i = 0; i < FFT_SIZE / 2; i++)
			max = (magnitude[i] > max) ? magnitude[i] : max;

		///@note if average is too small (noise or no audio), set it to a high value
		///@note this is to avoid the threshholding to be too sensitive
		/// TODO: find a better way to do this
		if (max < 0.0005 * GAIN)
			max = 100;

		// check if magnitudes pass a certain threshold
		for (int k = 1; k < FFT_SIZE / 2; k++)
		{
			// switch off notes that are not active
			if ((magnitude[k] < max * SENSITIVITY) || MIDI_KEY_BOUNDARY(keyNR[k]))
			{
				active_notes[keyNR[k]].status = MIDI_STATUS_NOTE_OFF;
				active_notes[keyNR[k]].param2 = 0; // set velocity to 0
				continue;						   // skip to next iteration threshold is not passed
			}
#ifdef DEBUG_DSP
			ESP_LOGI(TAG, "keyNR: %d, magnitude: %f, frequency: %f", keyNR[k], magnitude[k], frequency[k]);
#endif
			active_notes[keyNR[k]].status = MIDI_STATUS_NOTE_ON;
			active_notes[keyNR[k]].param2 = (uint8_t)(magnitude[k] / max * 127);
		}
		// send saved notes to MIDI queue
		xQueueSend(g_handle->midi_queue, &active_notes, portMAX_DELAY);
		fft_destroy(fft_plan);
	} // for(;;)
} // dsp_task

static void midi_task(void *arg)
{
	// gitcon driver context handler
	gitcon_handle_t g_handle = (gitcon_handle_t)arg;
	midi_message_t *active_notes = NULL;
	midi_status_t previous_states[128] = {0};
	for (;;)
	{
		///@note delay to avoid a watchdog timeout
		vTaskDelay(10 / portTICK_PERIOD_MS);

		if (xQueueReceive(g_handle->midi_queue, &active_notes, portMAX_DELAY) == pdFALSE)
			continue; // skip iteration if queue is empty

		///@note active_notes is a pointer to an array of MIDI messages and should not be NULL
		// send MIDI messages to UART
		for (size_t i = 0; i < 128; i++)
		{
			if (active_notes[i].status == previous_states[i])
				continue; // continue if note has not changed
			// send message to MIDI UART
			ESP_ERROR_CHECK(midi_write(g_handle->midi_handle, &active_notes[i]));
			previous_states[i] = active_notes[i].status;
		}
	} // for(;;)
} // midi_task

// ------------------------------------------------------------
// non-static functions
// ------------------------------------------------------------

esp_err_t gitcon_init(gitcon_handle_t *out_data)
{
	gitcon_data_t *g_data = (gitcon_data_t *)malloc(sizeof(gitcon_data_t));
	if (!g_data)
		return ESP_ERR_NO_MEM;

	// create queue for audio data (passed into sampler)
	QueueHandle_t dsp_queue = xQueueCreate(10, sizeof(size_t *));

	// create queue for midi messages
	g_data->midi_queue = xQueueCreate(5, sizeof(midi_handle_t *));
	if (!g_data->midi_queue)
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
	g_data->sampler = i2s_sampler_start(INTERNAL_ADC_CHANNEL, dsp_queue, AUDIO_BUFFER_SIZE, F_SAMPLE_HZ);
#endif

	// ------------------------------------------------------------
	// MIDI
	// ------------------------------------------------------------

	// Initialize MIDI and store in gitcon handle
	ESP_ERROR_CHECK(midi_init(&g_data->midi_handle, MIDI_UART, MIDI_BAUD, MIDI_RX, MIDI_TX));

	// ------------------------------------------------------------
	// INIT RTOS
	// ------------------------------------------------------------

	ESP_LOGI(TAG, "Creating RTOS tasks...");
	///@note DSP task: receives audio data from DMA task and sends audio data to MIDI task
	if (xTaskCreatePinnedToCore(dsp_task, "dsp_task", 1 << 16, g_data, 5, &task_handle_dsp, 1) == pdFALSE)
		return ESP_ERR_NO_MEM;

	///@note MIDI task: receives midi messages from DSP task and sends them to MIDI UART
	if (xTaskCreatePinnedToCore(midi_task, "midi_task", 2048, g_data, 5, &task_handle_midi, 0) == pdFALSE)
		return ESP_ERR_NO_MEM;

	// Pass final configuration to outer parameters
	*out_data = g_data;
	return ESP_OK;
}

esp_err_t gitcon_exit(gitcon_handle_t handle)
{
	ESP_ERROR_CHECK(midi_exit(handle->midi_handle));

	// stop tasks
	vTaskDelete(task_handle_dsp);
	vTaskDelete(task_handle_midi);

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