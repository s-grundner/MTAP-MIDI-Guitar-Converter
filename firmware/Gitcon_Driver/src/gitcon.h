/**
 * @file gitcon.h
 * @author @s-grundner @Laurenz03
 * @brief Gitcon Driver Header
 * @version 0.1
 * @date 2022-12-23
 *
 * @copyright Copyright (c) 2022
 *
 */

#ifndef GITCON_H
#define GITCON_H

#include "config.h"
#include <i2s_sampler.h>
#include <midi.h>
#include <fft.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#define GITCON_LOG_LEVEL ESP_LOG_ERROR

// handle for external opaque reference
typedef void *gitcon_handle_t;

/**
 * @brief initializes gitcon device and installs peripheral drivers
 *
 * @param[out] out_handle gitcon context handler
 * @return esp_err_t ESP_OK on success, ESP_ERR_NO_MEM on memory allocation error
 */
esp_err_t gitcon_init(gitcon_handle_t *out_handle);

/**
 * @brief frees all resources
 *
 * @param handle gitcon context handler
 * @return ESP_OK on success
 */
esp_err_t gitcon_exit(gitcon_handle_t handle);

#endif // GITCON_H