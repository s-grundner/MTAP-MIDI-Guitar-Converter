#include "gitcon.h"

esp_err_t inti_gitcon(gitcon_config_t **out_handle)
{
	esp_err_t err = ESP_OK;
	gitcon_config_t *cfg = (gitcon_config_t *)malloc(sizeof(gitcon_config_t));
	if (cfg == NULL)
	{
		err = ESP_ERR_NO_MEM;
		goto cleanup;
	}
	// initialize SPI
	spi_bus_config_t bus_cfg = {
		.miso_io_num = SPI_MISO,
		.mosi_io_num = SPI_MOSI,
		.sclk_io_num = SPI_SCLK,
		.max_transfer_sz = 32,
	};
	spi_bus_initialize(SPI_DEV, &bus_cfg, DMA_CHAN);

	mcp3201_config_t adc_cfg = {
		.host = SPI_DEV,
		.cs_io = SPI_CS,
		.miso_io = SPI_MISO,
		.mosi_io = SPI_MOSI};

	mcp3201_handle_t adc_handle;
	init_mcp3201(&adc_handle, &adc_cfg);

	*cfg = (gitcon_config_t){
		.mcp3201 = adc_handle,
	};

	*out_handle = cfg;

cleanup:
	return err;
}