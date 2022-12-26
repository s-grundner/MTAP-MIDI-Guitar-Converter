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

#include "mcp3201.h"

static const char *TAG = "mcp3201";

struct mcp3201_context_t
{
	mcp3201_config_t cfg;
	spi_device_handle_t spi;
};
typedef struct mcp3201_context_t mcp3201_context_t;

static void cs_low(spi_transaction_t *t)
{
	mcp3201_handle_t mcp_handle = (mcp3201_handle_t)t->user;
	gpio_set_level(mcp_handle->cfg.cs_io, 0);
}

static void cs_high(spi_transaction_t *t)
{
	mcp3201_handle_t mcp_handle = (mcp3201_handle_t)t->user;
	gpio_set_level(mcp_handle->cfg.cs_io, 1);
}

esp_err_t mcp3201_init(mcp3201_context_t **out_ctx, const mcp3201_config_t *cfg)
{
	mcp3201_context_t *ctx = (mcp3201_context_t *)malloc(sizeof(mcp3201_context_t));
	esp_err_t err = ESP_OK;
	if (ctx == NULL)
	{
		ESP_LOGE(TAG, "Failed to allocate memory for mcp3201 context");
		return ESP_ERR_NO_MEM;
	}

	*ctx = (mcp3201_context_t){
		.cfg = *cfg};

	spi_device_interface_config_t dev_cfg = {
		.clock_speed_hz = ADC_CLK,
		.mode = 0,
		.spics_io_num = ctx->cfg.cs_io,
		.queue_size = 1,
		.pre_cb = cs_low,
		.post_cb = cs_high,
		.command_bits = 0,
		.address_bits = 0,
		.dummy_bits = 0,
		.flags = 0,
	};

	err = spi_bus_add_device(ctx->cfg.host, &dev_cfg, &ctx->spi);
	ESP_ERROR_CHECK(err);
	if (err != ESP_OK)
	{
		ESP_LOGE(TAG, "Failed to add device to spi bus");
		goto cleanup;
	}
	*out_ctx = ctx;
	return err;

cleanup:
	if (ctx->spi)
	{
		spi_bus_remove_device(ctx->spi);
		ctx->spi = NULL;
	}
	free(ctx);
	return err;
}

esp_err_t mcp3201_read(mcp3201_context_t *ctx, uint16_t *out_value)
{
	esp_err_t err = ESP_OK;
	spi_transaction_t t = {
		.user = (void *)ctx,
		.length = 16,
		.rx_buffer = out_value,
	};
	err = spi_device_polling_transmit(ctx->spi, &t);
	if (err != ESP_OK)
	{
		ESP_LOGE(TAG, "Failed to transmit to mcp3201");
		return err;
	}
	return err;
}

esp_err_t mcp3201_exit(mcp3201_handle_t mcp_handle)
{
	esp_err_t err = ESP_OK;

	if (mcp_handle->spi)
	{
		err = spi_bus_remove_device(mcp_handle->spi);
		if (err != ESP_OK)
		{
			ESP_LOGE(TAG, "Failed to remove device from spi bus");
			return err;
		}
		mcp_handle->spi = NULL;
	}

	free(mcp_handle);
	return err;
}