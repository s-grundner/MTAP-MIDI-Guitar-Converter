/**
 * @file midi.h
 * @author @s-grundner
 * @brief
 * @version 0.1
 * @date 2022-12-23
 *
 * @copyright Copyright (c) 2022
 *
 */

#pragma once

#include <stdio.h>
#include "driver/gpio.h"
#include "esp_log.h"
#include "driver/uart.h"

#define MIDI_UART_DATA_SIZE 3
typedef enum
{
	MIDI_STATUS_NOTE_OFF = 0x80,
	MIDI_STATUS_NOTE_ON = 0x90,
	MIDI_STATUS_POLYPHONIC_KEY_PRESSURE = 0xA0,
	MIDI_STATUS_CONTROL_CHANGE = 0xB0,
	MIDI_STATUS_PROGRAM_CHANGE = 0xC0,
	MIDI_STATUS_CHANNEL_PRESSURE = 0xD0,
	MIDI_STATUS_PITCH_BEND = 0xE0,
} midi_status_t;

/**
 * @brief MIDI Message
 * @typedef midi_message_t
 * @param status 0x80-0xE0
 * @param channel 0-15
 * @param note_num 0-127
 * @param velocity 0-127
 */
typedef struct
{
	midi_status_t status;
	uint8_t channel;
	uint8_t note_num;
	uint8_t velocity;
} midi_message_t;

/**
 * @brief Initialize MIDI UART driver
 *
 * @param uart_port Number of UART port
 * @param baudrate Baudrate of UART port
 * @param rx_pin RX Pin
 * @param tx_pin TX Pin
 * @return esp_err_t
 */
esp_err_t midi_init(uart_port_t uart_port, int baudrate, gpio_num_t rx_pin, gpio_num_t tx_pin);

/**
 * @brief Send MIDI Message
 *
 * @param uart_port Number of UART port
 * @param msg midi message to send
 * @return esp_err_t
 */
esp_err_t midi_send(uart_port_t uart_port, midi_message_t *msg);

/**
 * @brief Exit MIDI UART driver
 *
 * @param uart_port Number of UART port
 * @retval ESP_OK On success
 * @retval ESP_FAIL On parameter error
 */
esp_err_t midi_exit(uart_port_t uart_port);
