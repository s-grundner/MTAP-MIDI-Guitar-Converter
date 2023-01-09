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
#include "driver/uart.h"
#include "esp_log.h"

#define MIDI_LOG_LEVEL ESP_LOG_ERROR

#define MIDI_BYTE_SIZE_DEFAULT 3
#define MIDI_BYTE_SIZE_SHORT 2

/**
 * @brief
 * @enum midi_status_t
 * @param MIDI_STATUS_NOTE_OFF 0x80 requires param2
 * @param MIDI_STATUS_NOTE_ON 0x90 requires param2
 * @param MIDI_STATUS_POLYPHONIC_KEY_PRESSURE 0xA0 param2 is not used
 * @param MIDI_STATUS_CONTROL_CHANGE 0xB0 requires param2
 * @param MIDI_STATUS_PROGRAM_CHANGE 0xC0 param2 is is not used
 * @param MIDI_STATUS_CHANNEL_PRESSURE 0xD0 param2 is is not used
 * @param MIDI_STATUS_PITCH_BEND 0xE0 requires param2
 */
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

typedef struct
{
	midi_status_t status;
	uint8_t channel;
	uint8_t param1;
	uint8_t param2;
} midi_message_t;

typedef struct
{
	uart_port_t uart_num;
	uint baudrate;
	gpio_num_t rx_io;
	gpio_num_t tx_io;
} midi_config_t;
typedef struct midi_context_t *midi_handle_t;

esp_err_t midi_init(midi_handle_t *out_handle, midi_config_t *cfg);
esp_err_t midi_exit(midi_handle_t midi_handle);

esp_err_t midi_send(midi_handle_t midi_handle, midi_message_t *msg);