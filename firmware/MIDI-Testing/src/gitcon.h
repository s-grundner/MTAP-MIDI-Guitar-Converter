/**
 * @file gitcon.h
 * @author @s-grundner
 * @brief
 * @version 0.1
 * @date 2022-12-23
 *
 * @copyright Copyright (c) 2022
 *
 */

#pragma once
#include <config.h>

/**
 * @brief Gitcon Configuration
 * @param mcp3201 MCP3201 ADC
 */
typedef struct
{
#ifdef USE_MCP3201
	mcp3201_handle_t mcp3201;
#endif
	QueueHandle_t midi_queue;
} gitcon_context_t;
typedef gitcon_context_t *gitcon_handle_t;

/**
 * @brief initializes gitcon
 *
 * @param out_handle
 * @return esp_err_t
 */
esp_err_t gitcon_init(gitcon_handle_t *out_handle);

/**
 * @brief frees all resources
 *
 * @param handle
 * @return esp_err_t
 */
esp_err_t gitcon_exit(gitcon_handle_t handle);