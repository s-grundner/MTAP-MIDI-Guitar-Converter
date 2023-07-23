/**
 * @file i2s_sampler.h
 * @author @s-grundner
 * @brief I2S Sampler Driver for ESP32
 * @version 0.1
 * @date 2023-03-26
 *
 * @copyright Copyright (c) 2023
 *
 */
#ifndef SAMPLING_H
#define SAMPLING_H

#include <stdint.h>
#include "driver/i2s.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "freertos/timers.h"
#include "esp_log.h"

/**
 * @brief Sampler Configuration
 * @param dma_queue Samples are sent to this queue by the DMA
 * @param dsp_queue Sampling result is sent to this queue
 * @param buffer Buffer to store samples in
 * @param buffer_pos Current position in buffer
 * @param buffer_size Size of the buffer in samples
 */
typedef struct
{
	QueueHandle_t dma_queue;
	QueueHandle_t dsp_queue;
	size_t *buffer;
	size_t buffer_pos;
	size_t buffer_size;
} i2s_sampler_t;

/**
 * @brief Starts a sampler Task that samples from the given ADC1 Channel and sends the samples to the given Queue
 *
 * @param adc1_channel ADC1 Channel to use (Only ADC1 Channels are supported)
 * @param recv_queue Queue to send samples to
 * @param buffer_size Size of the buffer in samples
 * @param f_sample Sample rate
 * @return i2s_sampler_t* Sampler context or NULL if failed
 */
i2s_sampler_t *i2s_sampler_start(adc_channel_t adc1_channel, QueueHandle_t recv_queue, size_t buffer_size, size_t f_sample);

/**
 * @brief Deletes sampler Task, frees memory and uninstalls adc as well as i2s driver
 * @param sampler Sampler to stop
 * @return ESP_OK if successful
 */
void i2s_sampler_stop(i2s_sampler_t *sampler);

#endif // SAMPLING_H