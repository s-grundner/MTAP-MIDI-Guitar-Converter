#ifndef SAMPLING_H
#define SAMPLING_H

#include <stdint.h>
#include "driver/i2s.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "freertos/timers.h"
#include "esp_log.h"


esp_err_t sampler_init(i2s_sampler_handle_t* out_handle);

#endif // SAMPLING_H