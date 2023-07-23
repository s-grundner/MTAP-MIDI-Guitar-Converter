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

#include "freertos/task.h"
#include "freertos/queue.h"

#include <config.h>
#include <midi.h>
#include <i2s_sampler.h>
#include <fft.h>

#define GITCON_LOG_LEVEL ESP_LOG_ERROR

/**
 * @brief Gitcon Configuration
 * @param sampler Sampler Handler (MCP3201 or I2S)
 * @param midi_handle MIDI Driver Context (MIDI over UART)
 * @param midi_queue MIDI Queue Handler
 */
typedef struct
{
#ifdef USE_MCP3201
  mcp3201_sampler_t *sampler;
#else
  i2s_sampler_t *sampler;
#endif
  midi_handle_t midi_handle;
  QueueHandle_t midi_queue;
} gitcon_context_t;

/// @typedef gitcon_context_t *gitcon_handle_t Gitcon Context Handler pointer
typedef gitcon_context_t *gitcon_handle_t;

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