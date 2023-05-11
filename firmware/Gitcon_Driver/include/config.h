/**
 * @file config.h
 * @author @s-grundner
 * @brief Gitcon Driver Configuration and Pin-Out File and dependencies
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

#define MIDI_UART (UART_NUM_1)
#define MIDI_BAUD (115200)
#define MIDI_TX (GPIO_NUM_26)
#define MIDI_RX (GPIO_NUM_27)

#define DMA_CHAN 1
#define ADC_RES_BITS 12
#define ADC_RES (1 << ADC_RES_BITS)
#define INTERNAL_ADC_UNIT (ADC_UNIT_1)
#define INTERNAL_ADC_CHANNEL (ADC_CHANNEL_5)
#define INTERNAL_ADC_IO (GPIO_NUM_33)

#define AUDIO_BUFFER_SIZE 1024						   // Size of buffer for FFT and sampler
#define F_SAMPLE_HZ 44100							   // Sample rate of FFT and sampler
#define FFT_WINDOW_SIZE 4							   // Number of buffers to take for FFT
#define FFT_SIZE (AUDIO_BUFFER_SIZE * FFT_WINDOW_SIZE) // Number of samples to take for FFT

// leave this commented out to use internal ADC
// #define USE_MCP3201

#endif // CONFIG_H
