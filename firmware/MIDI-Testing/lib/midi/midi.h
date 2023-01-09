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

typedef enum
{
	MIDI_BYTE_SIZE_DEFAULT = 3,
	MIDI_BYTE_SIZE_SHORT = 2
} midi_byte_size_t;

/**
 * @brief MIDI Message
 * @typedef midi_message_t
 * @param status 0x80-0xE0
 * @param channel 0-15
 * @param param1 0-127
 * @param param2 0-127
 */
typedef struct
{
	midi_status_t status;
	uint8_t channel;
	uint8_t param1;
	uint8_t param2;
} midi_message_t;

typedef void (*init_fn_t)(void *user);
typedef void (*exit_fn_t)(void *user);
typedef void (*send_fn_t)(void *user, midi_message_t *message, midi_byte_size_t byte_size);

typedef struct
{
	init_fn_t user_init;
	exit_fn_t user_exit;
	send_fn_t user_send;

	const void *user;
} midi_config_t;

typedef struct midi_context_t *midi_handle_t;

char midi_init(midi_handle_t *out_handle, midi_config_t *cfg);

void midi_exit(midi_handle_t midi_handle);

void midi_note_off(midi_handle_t handle, uint8_t, uint8_t);
void midi_note_on(midi_handle_t handle, uint8_t, uint8_t);
void midi_poly_key_pressure(midi_handle_t handle, uint8_t, uint8_t);
void midi_ctrl_change(midi_handle_t handle, uint8_t, uint8_t);
void midi_prg_change(midi_handle_t handle, uint8_t, uint8_t);
void midi_channel_pressure(midi_handle_t handle, uint8_t, uint8_t);
void midi_pitch_bend(midi_handle_t midi_handle, uint8_t channel, uint16_t bend);
