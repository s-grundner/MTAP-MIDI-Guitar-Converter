#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include "driver/spi_master.h"

#define SPI_MOSI 23
#define SPI_MISO 19
#define SPI_SCLK 18
#define SPI_CS 5

#define SPI_DEV VSPI_HOST
#define ADC_CLK 1000000

#define DMA_CHAN 1

void cs_low()
{
	gpio_set_level(SPI_CS, 0);
}
void cs_high()
{
	gpio_set_level(SPI_CS, 1);
}

void app_main(void)
{
	// initialize SPI
	spi_bus_config_t buscfg = {
		.miso_io_num = SPI_MISO,
		.mosi_io_num = SPI_MOSI,
		.sclk_io_num = SPI_SCLK,
		.quadwp_io_num = -1,
		.quadhd_io_num = -1};
	spi_bus_initialize(SPI_DEV, &buscfg, DMA_CHAN);
	spi_device_interface_config_t devcfg = {
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
	spi_device_handle_t spi = {
		.handle = 0,
		.host = SPI_DEV,
		.slot = 0,
		.config = &devcfg,
	};
}
