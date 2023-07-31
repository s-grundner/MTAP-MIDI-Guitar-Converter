/**
 * @file mcp3201_sampler.c
 * @author @s-grundner
 * @brief MCP3201 Sampler
 * @version 0.1
 * @date 2023-03-26
 *
 * @copyright Copyright (c) 2023
 *
 */
#include "mcp3201_internal.h"

#define RESAMPLE_DENOMINATOR 2
#define READER_TIMEOUT_MS 10
#define READER_TIMEOUT_TICKS (READER_TIMEOUT_MS / portTICK_PERIOD_MS)

static const char *TAG = "MCP3201_SAMPLER";

static TaskHandle_t task_handle_sampler;
static TaskHandle_t task_handle_reader;

/**
 * @brief MCP3201 Sampler. Samples continuously and puts the samples into a queue.
 * @param mcp_handle MCP3201 Handle
 * @param dma_queue DMA Queue. Will be created by the sampler.
 * @param[out] result_queue DSP Queue. Has to be created by the user.
 * @param buffer Buffer
 * @param buffer_pos Buffer Position
 * @param buffer_size Buffer Size
 */
typedef struct mcp3201_sampler_data_s
{
	mcp3201_handle_t mcp_handle;
	QueueHandle_t dma_queue;
	QueueHandle_t result_queue;
	size_t *buffer;
	size_t buffer_pos;
	size_t buffer_size;
} mcp3201_sampler_data_t;

static void IRAM_ATTR sampler_task(void *arg)
{
	mcp3201_sampler_handle_t sampler = (mcp3201_sampler_handle_t)arg;
	for (;;)
	{
		spi_event_t evt;
		if (xQueueReceive(sampler->dma_queue, &evt, portMAX_DELAY) == pdFALSE)
			continue;

		if (evt != SPI_EV_SEND_DMA_READY)
			continue;

		size_t bytes_read = 0;
		do
		{
			// fill audio buffer
			size_t bytes_to_read = RESAMPLE_DENOMINATOR * (sampler->buffer_size - sampler->buffer_pos);
			void *buffer_position = (void *)(sampler->buffer + sampler->buffer_pos);

			// read data from spi
			spi_transaction_t t = {
				.user = (void *)sampler->mcp_handle,
				.length = bytes_to_read,
				.rx_buffer = buffer_position,
			};
			sampler->mcp_handle->cur_transaction = &t;

			spi_device_get_trans_result(
				sampler->mcp_handle->spi_handle,
				(spi_transaction_t **)(sampler->mcp_handle->cur_transaction),
				READER_TIMEOUT_TICKS);

			bytes_read = t.rxlength;

			sampler->buffer_pos += bytes_read / RESAMPLE_DENOMINATOR;

			if (sampler->buffer_pos == sampler->buffer_size)
			{
				// send data to DSP queue
				sampler->buffer_pos = 0;
				xQueueSend(sampler->result_queue, &sampler->buffer, portMAX_DELAY);
			}
		} while (bytes_read > 0);
	}
}

static void IRAM_ATTR reader_task(void *arg)
{
	mcp3201_sampler_handle_t sampler = (mcp3201_sampler_handle_t)arg;
	for (;;)
	{
		spi_device_queue_trans(sampler->mcp_handle->spi_handle, sampler->mcp_handle->cur_transaction, portMAX_DELAY);
		spi_event_t evt = SPI_EV_SEND_DMA_READY;
		xQueueSendFromISR(sampler->dma_queue, &evt, NULL);
	}
}

mcp3201_sampler_handle_t mcp3201_sampler_start(mcp3201_handle_t mcp_handle, QueueHandle_t recv_queue, const size_t buffer_size, const size_t f_sample)
{
	if (mcp_handle == NULL || recv_queue == NULL || buffer_size == 0 || f_sample == 0)
	{
		ESP_LOGE(TAG, "Invalid arguments");
		return NULL;
	}

	mcp3201_sampler_data_t *sampler = (mcp3201_sampler_data_t *)malloc(sizeof(mcp3201_sampler_data_t));
	*sampler = (mcp3201_sampler_data_t){
		.mcp_handle = mcp_handle,
		.buffer_size = buffer_size,
		.buffer = (size_t *)malloc(sizeof(size_t) * buffer_size),
		.buffer_pos = 0,
		.dma_queue = xQueueCreate(4, sizeof(spi_event_t)),
		.result_queue = recv_queue};

	ESP_ERROR_CHECK(spi_device_acquire_bus(mcp_handle->spi_handle, portMAX_DELAY));

	xTaskCreatePinnedToCore(sampler_task, "sampler_task", RESAMPLE_DENOMINATOR * buffer_size, sampler, 5, &task_handle_sampler, 0);
	xTaskCreatePinnedToCore(reader_task, "reader_task", 2048, sampler, 5, &task_handle_reader, 0);

	return sampler;
}

void mcp3201_sampler_stop(mcp3201_sampler_handle_t sampler)
{
	vQueueDelete(sampler->dma_queue);
	vQueueDelete(sampler->result_queue);

	spi_device_release_bus(sampler->mcp_handle->spi_handle);

	vTaskDelete(task_handle_sampler);
	vTaskDelete(task_handle_reader);

	free(sampler->buffer);
	free(sampler);
}