#include "config.h"

void app_main(void)
{
	// initialize SPI
	spi_bus_config_t bus_cfg = {
		.miso_io_num = SPI_MISO,
		.mosi_io_num = SPI_MOSI,
		.sclk_io_num = SPI_SCLK,
		.quadwp_io_num = -1,
		.quadhd_io_num = -1};
	spi_bus_initialize(SPI_DEV, &bus_cfg, DMA_CHAN);

	mcp3201_config_t adc_cfg = {
		.host = SPI_DEV,
		.cs_io = SPI_CS,
		.miso_io = SPI_MISO,
		.mosi_io = SPI_MOSI};

	mcp3201_handle_t adc_handle;
	init_mcp3201(&adc_handle, &adc_cfg);
}
