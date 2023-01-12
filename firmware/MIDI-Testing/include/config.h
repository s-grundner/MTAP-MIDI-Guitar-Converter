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
#ifndef CONFIG_H
#define CONFIG_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "freertos/timers.h"

#include "driver/gpio.h"
#include "driver/spi_master.h"
#include "driver/uart.h"
#include "driver/adc.h"
#include "driver/i2s.h"
#include "esp_adc_cal.h"
#include "esp_log.h"

#include "fft.h"
#include "i2s_sampler.h"
#include "mcp3201.h"
#include "midi.h"

#define SPI_MOSI (GPIO_NUM_23)
#define SPI_MISO (GPIO_NUM_19)
#define SPI_SCLK (GPIO_NUM_18)
#define SPI_CS (GPIO_NUM_5)
#define SPI_DEV (VSPI_HOST)

#define ADC_RES_BITS 12
#define ADC_SAMPLES_COUNT 2*(1 << ADC_RES_BITS)
#define INTERNAL_ADC_UNIT (ADC_UNIT_1)
#define INTERNAL_ADC_CHANNEL (ADC_CHANNEL_0)
#define INTERNAL_ADC_IO (GPIO_NUM_4)
#define DMA_CHAN 1

#define MIDI_UART (UART_NUM_1)
#define MIDI_BAUD (115200)
#define MIDI_TX (GPIO_NUM_17)
#define MIDI_RX (GPIO_NUM_16)

#define FFT_SIZE 8192	  // Amount of samples to take for FFT
#define F_SAMPLE_HZ 44100 // Sample rate of FFT

 // #define USE_MCP3201
#define USE_INTERNAL_ADC

#endif // CONFIG_H