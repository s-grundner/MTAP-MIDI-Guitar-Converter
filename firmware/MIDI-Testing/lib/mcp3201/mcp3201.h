/**
 * @file mcp3201.h
 * @author @s-grundner
 * @brief
 * @version 0.1
 * @date 2022-12-24
 *
 * @copyright Copyright (c) 2022
 *
 */

#pragma once

#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include "driver/spi_master.h"
#include "esp_log.h"

#define ADC_CLK SPI_MASTER_FREQ_8M

/**
 * @brief MCP3201 Configuration
 *
 * @typedef mcp3201_config_t
 * @param host SPI Host
 * @param cs_io Chip Select IO
 * @param miso_io MISO IO
 * @param mosi_io MOSI IO
 * @param dma_chan DMA Channel
 */
typedef struct
{
	spi_host_device_t host;
	gpio_num_t cs_io;
	gpio_num_t miso_io;
	gpio_num_t mosi_io;
	int dma_chan;
} mcp3201_config_t;

typedef struct mcp3201_context_t *mcp3201_handle_t;

esp_err_t mcp3201_init(mcp3201_handle_t *out_handle, const mcp3201_config_t *cfg);
esp_err_t mcp3201_exit(mcp3201_handle_t mcp_handle);
esp_err_t mcp3201_read(mcp3201_handle_t handle, uint16_t *out_value);
