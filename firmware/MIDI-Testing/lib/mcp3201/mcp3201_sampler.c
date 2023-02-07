#include "mcp3201.h"

#define RESAMPLE_DIVIDER 2
#define READER_TIMEOUT_MS 10
#define READER_TIMEOUT_TICKS (READER_TIMEOUT_MS / portTICK_PERIOD_MS)

static const char *TAG = "MCP3201_SAMPLER";
struct mcp3201_context_t
{
	mcp3201_config_t cfg;
	spi_device_handle_t spi;
	spi_transaction_t *ongoing_transaction;
};
typedef struct mcp3201_context_t mcp3201_context_t;

static TaskHandle_t sampler_task_handle;
static TaskHandle_t reader_task_handle;

static void IRAM_ATTR sampler_task(void *arg)
{
	mcp3201_sampler_t *sampler = (mcp3201_sampler_t *)arg;
	for (;;)
	{
		spi_event_t evt;
		if (xQueueReceive(sampler->dma_queue, &evt, portMAX_DELAY) == pdTRUE)
		{
			if (evt == SPI_EV_SEND_DMA_READY)
			{
				size_t bytes_read = 0;
				do
				{
					// fill audio buffer
					size_t bytes_to_read = RESAMPLE_DIVIDER * (sampler->buffer_size - sampler->buffer_pos);
					void *buffer_position = (void *)(sampler->buffer + sampler->buffer_pos);

					// read data from spi
					spi_transaction_t t = {
						.user = (void *)sampler->mcp_handle,
						.length = bytes_to_read,
						.rx_buffer = buffer_position,
					};
					sampler->mcp_handle->ongoing_transaction = &t;
					spi_device_get_trans_result(sampler->mcp_handle->spi, &t, READER_TIMEOUT_TICKS);
					bytes_read = t.rxlength;

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

static void IRAM_ATTR reader_task(void *arg)
{
	mcp3201_sampler_t *sampler = (mcp3201_sampler_t *)arg;
	for (;;)
	{
		spi_device_queue_trans(sampler->mcp_handle->spi, sampler->mcp_handle->ongoing_transaction, portMAX_DELAY);
		spi_event_t evt = SPI_EV_SEND_DMA_READY;
		xQueueSendFromISR(sampler->dma_queue, &evt, NULL);
	}
}

mcp3201_sampler_t *mcp3201_sampler_start(mcp3201_handle_t mcp_handle, QueueHandle_t recv_queue, const size_t buffer_size, const size_t f_sample)
{
	if (mcp_handle == NULL || recv_queue == NULL || buffer_size == 0 || f_sample == 0)
	{
		ESP_LOGE(TAG, "Invalid arguments");
		return NULL;
	}

	mcp3201_sampler_t *sampler = (mcp3201_sampler_t *)malloc(sizeof(mcp3201_sampler_t));
	*sampler = (mcp3201_sampler_t){
		.mcp_handle = mcp_handle,
		.buffer_size = buffer_size,
		.buffer = (size_t *)malloc(sizeof(size_t) * buffer_size),
		.buffer_pos = 0,
		.dma_queue = xQueueCreate(4, sizeof(spi_event_t)),
		.dsp_queue = recv_queue};

	while (spi_device_acquire_bus(mcp_handle->spi, portMAX_DELAY) != ESP_OK)
		ESP_LOGE(TAG, "Failed to acquire bus\n retrying...");

	xTaskCreatePinnedToCore(sampler_task, "sampler_task", RESAMPLE_DIVIDER * buffer_size, sampler, 5, &sampler_task_handle, 0);
	xTaskCreatePinnedToCore(reader_task, "reader_task", 2048, sampler, 5, &reader_task_handle, 0);

	return sampler;
}

void mcp3201_sampler_stop(mcp3201_sampler_t *sampler)
{
	vQueueDelete(sampler->dma_queue);
	vQueueDelete(sampler->dsp_queue);

	spi_device_release_bus(sampler->mcp_handle->spi);

	vTaskDelete(sampler_task_handle);
	vTaskDelete(reader_task_handle);

	free(sampler->buffer);
	free(sampler);
}