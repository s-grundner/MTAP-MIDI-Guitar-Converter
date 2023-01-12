#ifndef SAMPLING_H
#define SAMPLING_H

#include <stdint.h>
#include "driver/i2s.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "freertos/timers.h"
#include "esp_log.h"

typedef struct
{
	QueueHandle_t dma_queue;
	QueueHandle_t dsp_queue;
	size_t* buffer;
	size_t buffer_pos;
	size_t buffer_size;
} i2s_sampler_t;

i2s_sampler_t* sampler_start(adc_channel_t adc1_channel, size_t buffer_size);
esp_err_t sampler_stop(i2s_sampler_t* sampler);

#endif // SAMPLING_H