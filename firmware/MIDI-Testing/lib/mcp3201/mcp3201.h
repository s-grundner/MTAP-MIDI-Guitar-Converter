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

#ifndef MCP3201_H
#define MCP3201_H

#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "driver/gpio.h"
#include "driver/spi_master.h"
#include "hal/spi_types.h"
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
typedef struct mcp3201_context_t* mcp3201_handle_t;

/**
 * @brief MCP3201 Sampler. Samples continuously and puts the samples into a queue.
 * @param mcp_handle MCP3201 Handle
 * @param dma_queue DMA Queue. Will be created by the sampler.
 * @param[out] dsp_queue DSP Queue. Has to be created by the user. 
 * @param buffer Buffer
 * @param buffer_pos Buffer Position
 * @param buffer_size Buffer Size
 * @typedef mcp3201_sampler_t
 */
typedef struct
{
	mcp3201_handle_t mcp_handle;
	QueueHandle_t dma_queue;
	QueueHandle_t dsp_queue;
	size_t* buffer;
	size_t buffer_pos;
	size_t buffer_size;
} mcp3201_sampler_t;

esp_err_t mcp3201_init(mcp3201_handle_t* out_handle, const mcp3201_config_t* cfg);
esp_err_t mcp3201_exit(mcp3201_handle_t mcp_handle);
esp_err_t mcp3201_read(mcp3201_handle_t handle, uint16_t* out_value);

mcp3201_sampler_t* mcp3201_sampler_start(mcp3201_handle_t mcp_handle, QueueHandle_t recv_queue, const size_t buffer_size, const size_t f_sample);
void mcp3201_sampler_stop(mcp3201_sampler_t* sampler);

#endif // MCP3201_H