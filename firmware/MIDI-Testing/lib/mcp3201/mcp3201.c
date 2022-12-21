#include "mcp3201.h"
#define SPI_CS 5

static const char *TAG = "mcp3201";

typedef struct
{
	mcp3201_config_t cfg;
	spi_device_handle_t spi;
} mcp3201_context_t;

static void cs_low(spi_transaction_t *t)
{
	gpio_set_level(SPI_CS, 0);
}
static void cs_high()
{
	gpio_set_level(SPI_CS, 1);
}

esp_err_t init_mcp3201(mcp3201_context_t **out_ctx, const mcp3201_config_t *cfg)
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
		.spics_io_num = SPI_CS,
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
	return ESP_OK;

cleanup:
	if (ctx->spi)
	{
		spi_bus_remove_device(ctx->spi);
		ctx->spi = NULL;
	}
	free(ctx);
	return err;
}