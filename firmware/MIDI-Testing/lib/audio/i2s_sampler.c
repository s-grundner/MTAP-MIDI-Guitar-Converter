#include "i2s_sampler.h"
static const char *TAG = "I2S_SAMPLER";

#define RESAMPLE_DIVIDER 2
#define READER_TIMEOUT_MS 10
#define READER_TIMEOUT_TICKS (READER_TIMEOUT_MS / portTICK_PERIOD_MS)

static TaskHandle_t sampler_task_handle;

static void IRAM_ATTR sampler_task(void *arg)
{
	i2s_sampler_t *sampler = (i2s_sampler_t *)arg;
	for (;;)
	{
		i2s_event_t evt;
		if (xQueueReceive(sampler->dma_queue, &evt, portMAX_DELAY) == pdTRUE)
		{
			if (evt.type == I2S_EVENT_RX_DONE)
			{
				size_t bytes_read = 0;
				do
				{
					// fill audio buffer
					size_t bytes_to_read = RESAMPLE_DIVIDER * (sampler->buffer_size - sampler->buffer_pos);
					void *buffer_position = (void *)(sampler->buffer + sampler->buffer_pos);

					// read data from i2s
					i2s_read(I2S_NUM_0, buffer_position, bytes_to_read, &bytes_read, READER_TIMEOUT_TICKS);
					sampler->buffer_pos += bytes_read / RESAMPLE_DIVIDER;

					if (sampler->buffer_pos == sampler->buffer_size)
					{
						// send data to DSP queue
						sampler->buffer_pos = 0;
						xQueueSend(sampler->dsp_queue, &sampler->buffer, portMAX_DELAY);
					}
				} while (bytes_read > 0);
			}
		}
	}
}

i2s_sampler_t *i2s_sampler_start(adc_channel_t adc1_channel, QueueHandle_t recv_queue, const size_t buffer_size, const size_t f_sample)
{
	ESP_LOGI(TAG, "Initializing I2S Sampler...");

	QueueHandle_t dma_queue;

	i2s_config_t i2s_cfg = {
		.mode = (i2s_mode_t)(I2S_MODE_MASTER | I2S_MODE_RX | I2S_MODE_ADC_BUILT_IN),
		.sample_rate = f_sample,
		.bits_per_sample = I2S_BITS_PER_SAMPLE_16BIT,
		.channel_format = I2S_CHANNEL_FMT_ONLY_LEFT,
		.communication_format = I2S_COMM_FORMAT_STAND_I2S,
		.intr_alloc_flags = ESP_INTR_FLAG_LEVEL1,
		.dma_buf_count = 4,
		.dma_buf_len = 1024,
		.use_apll = false,
		.fixed_mclk = 0};

	// Initialize ADC
	ESP_ERROR_CHECK(i2s_driver_install(I2S_NUM_0, &i2s_cfg, 4, &dma_queue));
	ESP_ERROR_CHECK(i2s_set_adc_mode(ADC_UNIT_1, adc1_channel));
	ESP_ERROR_CHECK(i2s_adc_enable(I2S_NUM_0));

	i2s_sampler_t *sampler = (i2s_sampler_t *)malloc(sizeof(i2s_sampler_t));

	*sampler = (i2s_sampler_t){
		.buffer = (size_t *)malloc(buffer_size * sizeof(size_t)),
		.buffer_pos = 0,
		.buffer_size = buffer_size,
		.dma_queue = dma_queue,
		.dsp_queue = recv_queue};

	// DMA task: receives audio data from ADC and sends it to DSP task
	if (xTaskCreatePinnedToCore(sampler_task, "sampler_task", 1 << 14, sampler, 5, &sampler_task_handle, 0) == pdFALSE)
		return NULL;

	return sampler;
}

esp_err_t i2s_sampler_stop(i2s_sampler_t *sampler)
{
	ESP_LOGI(TAG, "Stopping I2S Sampler...");

	// stop i2s
	i2s_adc_disable(I2S_NUM_0);
	i2s_driver_uninstall(I2S_NUM_0);

	// stop task
	vTaskDelete(sampler_task_handle);

	// free memory
	free(sampler->buffer);
	free(sampler);

	return ESP_OK;
}