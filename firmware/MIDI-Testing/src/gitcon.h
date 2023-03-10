/**
 * @file gitcon.h
 * @author @s-grundner @Laurenz03
 * @brief Gitcon Device Driver
 * @version 0.1
 * @date 2022-12-23
 *
 * @copyright Copyright (c) 2022
 *
 */

#ifndef GITCON_H
#define GITCON_H

#include "config.h"

#define GITCON_LOG_LEVEL ESP_LOG_ERROR

/**
 * @brief Gitcon Configuration
 * @param mcp3201 MCP3201 ADC
 * @param midi_handle MIDI Context Handler
 * @param midi_queue MIDI Queue
 */
typedef struct
{
#ifdef USE_MCP3201
  mcp3201_sampler_t *sampler;
#else
  i2s_sampler_t *sampler;
#endif
  midi_handle_t midi_handle;
  QueueHandle_t midi_queue; // TODO: merge with midi_handle
} gitcon_context_t;
typedef gitcon_context_t *gitcon_handle_t;

/**
 * @brief initializes gitcon device
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