/**
 * @file mcp3201.c
 * @author @s-grundner
 * @brief
 * @version 0.1
 * @date 2022-12-24
 *
 * @copyright Copyright (c) 2022
 *
 */

#include "mcp3201_internal.h"

static const char *TAG = "mcp3201";

static void cs_low(spi_transaction_t *t)
{
	mcp3201_handle_t mcp_handle = (mcp3201_handle_t)(t->user);
	gpio_set_level(mcp_handle->mcp_cfg.cs_io, 0);
}

static void cs_high(spi_transaction_t *t)
{
	mcp3201_handle_t mcp_handle = (mcp3201_handle_t)(t->user);
	gpio_set_level(mcp_handle->mcp_cfg.cs_io, 1);
}

esp_err_t mcp3201_init(mcp3201_data_t **out_data, const mcp3201_config_t *mcp_cfg)
{
	mcp3201_data_t *data = (mcp3201_data_t *)malloc(sizeof(mcp3201_data_t));
	esp_err_t err = ESP_OK;
	if (data == NULL)
	{
		ESP_LOGE(TAG, "Failed to allocate memory for mcp3201 context");
		return ESP_ERR_NO_MEM;
	}

	*data = (mcp3201_data_t){
		.mcp_cfg = *mcp_cfg};

	spi_device_interface_config_t dev_cfg = {
		.clock_speed_hz = ADC_CLK,
		.mode = 0,
		.spics_io_num = data->mcp_cfg.cs_io,
		.queue_size = 1,
		.pre_cb = cs_low,
		.post_cb = cs_high,
		.command_bits = 0,
		.address_bits = 0,
		.dummy_bits = 0,
		.flags = 0,
	};

	err = spi_bus_add_device(data->mcp_cfg.spi_host, &dev_cfg, &data->spi_handle);
	if (err == ESP_OK)
	{
		*out_data = data;
		return err;
	}

	ESP_LOGE(TAG, "Failed to add device to spi bus");
	if (data->spi_handle)
	{
		spi_bus_remove_device(data->spi_handle);
		data->spi_handle = NULL;
	}
	free(data);
	return err;
}

esp_err_t mcp3201_read(mcp3201_data_t *data, uint16_t *out_value)
{
	esp_err_t err = ESP_OK;
	spi_transaction_t t = {
		.user = (void *)data,
		.length = 16,
		.rx_buffer = out_value,
	};
	err = spi_device_polling_transmit(data->spi_handle, &t);
	if (err != ESP_OK)
		ESP_LOGE(TAG, "Failed to transmit to mcp3201");

	return err;
}

esp_err_t mcp3201_exit(mcp3201_handle_t mcp_handle)
{
	if (mcp_handle->spi_handle != NULL)
	{
		free(mcp_handle);
		return ESP_OK;
	}

	esp_err_t err = spi_bus_remove_device(mcp_handle->spi_handle);
	if (err != ESP_OK)
	{
		ESP_LOGE(TAG, "Failed to remove device from spi bus");
		return err;
	}
	mcp_handle->spi_handle = NULL;
	free(mcp_handle);

	return err;
}