/**
 * @file config.h
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
#include <stdlib.h>
#include <math.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "driver/gpio.h"
#include "driver/spi_master.h"
#include "driver/uart.h"
#include "driver/adc.h"
#include "driver/i2s.h"
#include "esp_adc_cal.h"

#include "esp_log.h"
#include "fft.h"

#define SPI_MOSI (GPIO_NUM_23)
#define SPI_MISO (GPIO_NUM_19)
#define SPI_SCLK (GPIO_NUM_18)
#define SPI_CS (GPIO_NUM_5)
#define SPI_DEV (VSPI_HOST)

#define INTERNAL_ADC_UNIT (ADC_UNIT_2)
#define INTERNAL_ADC (ADC2_CHANNEL_0)
#define INTERNAL_ADC_IO (GPIO_NUM_4)

#define MIDI_UART (UART_NUM_1)
#define MIDI_BAUD (115200)
#define MIDI_TX (GPIO_NUM_17)
#define MIDI_RX (GPIO_NUM_16)

#define DMA_CHAN 1

#define FFT_SIZE 8192	  // Amount of samples to take for FFT
#define F_SAMPLE_HZ 44100 // Sample rate of FFT

// #define USE_MCP3201
// #define USE_INTERNAL_ADC

#include "mcp3201.h"
#include "midi.h"
