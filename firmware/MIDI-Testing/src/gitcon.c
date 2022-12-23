#include "gitcon.h"

esp_err_t gitcon_init(gitcon_config_t **out_handle)
{
	gitcon_config_t *cfg = (gitcon_config_t *)malloc(sizeof(gitcon_config_t));
	if (!cfg)
		return ESP_ERR_NO_MEM;
	// ------------------------------------------------------------
	// SPI
	// ------------------------------------------------------------
	spi_bus_config_t bus_cfg = {
		.miso_io_num = SPI_MISO,
		.mosi_io_num = SPI_MOSI,
		.sclk_io_num = SPI_SCLK,
		.max_transfer_sz = 32,
	};
	spi_bus_initialize(SPI_DEV, &bus_cfg, DMA_CHAN);

	// ------------------------------------------------------------
	// MCP3201 (ADC)
	// ------------------------------------------------------------
	mcp3201_config_t adc_cfg = {
		.host = SPI_DEV,
		.cs_io = SPI_CS,
		.miso_io = SPI_MISO,
		.mosi_io = SPI_MOSI};

	mcp3201_handle_t adc_handle;
	ESP_ERROR_CHECK(mcp3201_init(&adc_handle, &adc_cfg));

	// ------------------------------------------------------------
	// INITIAL GITCON CONFIG
	// ------------------------------------------------------------

	*cfg = (gitcon_config_t){
		.mcp3201 = adc_handle,
	};

	*out_handle = cfg;
	return ESP_OK;
}

esp_err_t gitcon_exit(gitcon_handle_t handle)
{
	ESP_ERROR_CHECK(mcp3201_exit(handle->mcp3201));
	free(handle);
	return ESP_OK;
}