/**
 * @file mcp3201.h
 * @author @s-grundner
 * @brief MCP3201 12-Bit ADC Driver for ESP32
 * @version 0.1
 * @date 2022-12-24
 *
 * @copyright Copyright (c) 2022
 */

#ifndef MCP3201_DRIVER_H
#define MCP3201_DRIVER_H

#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "driver/gpio.h"
#include "hal/spi_types.h"
#include "driver/spi_master.h"
#include "esp_log.h"
#include "esp_err.h"

#define ADC_CLK SPI_MASTER_FREQ_8M

/**
 * @brief MCP3201 Configuration Structure
 * @param spi_host SPI Host Device
 * @param cs_io Chip Select IO (CS)
 * @param miso_io Master In Slave Out IO (MISO, SPIQ)
 * @param mosi_io Master Out Slave In IO (MOSI, SPID)
 * @param dma_chan DMA Channel
 */
typedef struct
{
	spi_host_device_t spi_host;
	gpio_num_t cs_io;
	gpio_num_t miso_io;
	gpio_num_t mosi_io;
	uint dma_chan;
} mcp3201_config_t;

typedef void *mcp3201_handle_t;
typedef void *mcp3201_sampler_handle_t;

/**
 * @brief Initializes the MCP3201 ADC device
 *
 * @param out_handle MCP3201 Handler to store initialization data
 * @param mcp_cfg MCP3201 Configuration
 * @return
 */
esp_err_t mcp3201_init(mcp3201_handle_t *out_handle, const mcp3201_config_t *mcp_cfg);

/**
 * @brief Exits the MCP3201 ADC device and frees all resources
 *
 * @param mcp_handle MCP3201 Device to exit
 * @return ESP_OK on success
 */
esp_err_t mcp3201_exit(mcp3201_handle_t mcp_handle);

/**
 * @brief Reads a single value from the MCP3201 ADC
 *
 * @param handle MCP3201 Device
 * @param[out] out_value Value
 * @return ESP_OK on success
 */
esp_err_t mcp3201_read(mcp3201_handle_t handle, uint16_t *out_value);

/**
 * @brief Starts the MCP3201 Sampler, which samples continuously and puts the samples into a queue.
 *
 * @param mcp_handle MCP3201 Device must be initialized first
 * @param recv_queue queue to send the samples into
 * @param buffer_size size of the audio buffer
 * @param f_sample Sample rate
 * @return mcp3201_sampler_t* Sampler Handler
 */
mcp3201_sampler_handle_t mcp3201_sampler_start(mcp3201_handle_t mcp_handle, QueueHandle_t recv_queue, const size_t buffer_size, const size_t f_sample);

/**
 * @brief Stops the MCP3201 Sampler
 * @param sampler Sampler Handler to stop
 */
void mcp3201_sampler_stop(mcp3201_sampler_handle_t sampler);

#endif // MCP3201_H