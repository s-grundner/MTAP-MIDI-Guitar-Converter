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

typedef struct i2s_sampler_s *i2s_sampler_handle_t;

/**
 * @brief Starts a sampler Task that samples from the given ADC1 Channel and sends the samples to the given Queue
 *
 * @param adc1_channel ADC1 Channel to use (Only ADC1 Channels are supported)
 * @param recv_queue Queue to send samples to
 * @param buffer_size Size of the buffer in samples
 * @param f_sample Sample rate
 * @return i2s_sampler_t* Sampler handle or NULL if failed
 */
i2s_sampler_handle_t i2s_sampler_start(adc_channel_t adc1_channel, QueueHandle_t recv_queue, size_t buffer_size, size_t f_sample);

QueueHandle_t i2s_sampler_get_queue(i2s_sampler_handle_t sampler);

/**
 * @brief Deletes sampler Task, frees memory and uninstalls adc as well as i2s driver
 * @param sampler Sampler to stop
 * @return ESP_OK if successful
 */
void i2s_sampler_stop(i2s_sampler_handle_t sampler);

#endif // SAMPLING_H