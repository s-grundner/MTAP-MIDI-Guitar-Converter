/**
 * @file midi.h
 * @author @s-grundner
 * @brief midi uart driver for esp32
 * @version 0.1
 * @date 2022-12-23
 *
 * @copyright Copyright (c) 2022
 *
 */

#ifndef MIDI_DRIVER_H
#define MIDI_DRIVER_H

#include <stdio.h>
#include <stdint.h>
#include "driver/gpio.h"
#include "driver/uart.h"
#include "esp_log.h"

#define MIDI_LOG_LEVEL ESP_LOG_ERROR

#define MIDI_BYTE_SIZE_DEFAULT 3
#define MIDI_BYTE_SIZE_SHORT 2

#define MIDI_PITCH_BEND_MIN (0)
#define MIDI_PITCH_BEND_MAX (16383)
#define MIDI_PITCH_BEND_CENTER (8192)

/**
 * @brief MIDI Status Bytes
 * @enum midi_status_t
 * @param MIDI_STATUS_NOTE_OFF 0x80, requires param2
 * @param MIDI_STATUS_NOTE_ON 0x90, requires param2
 * @param MIDI_STATUS_POLYPHONIC_KEY_PRESSURE 0xA0, param2 is not used
 * @param MIDI_STATUS_CONTROL_CHANGE 0xB0, requires param2
 * @param MIDI_STATUS_PROGRAM_CHANGE 0xC0, param2 is is not used
 * @param MIDI_STATUS_CHANNEL_PRESSURE 0xD0, param2 is is not used
 * @param MIDI_STATUS_PITCH_BEND 0xE0, requires param2
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

/**
 * @brief MIDI Message
 * @struct midi_message_t
 * @param status MIDI Status Byte
 * @param channel MIDI Channel
 * @param param1 MIDI Parameter 1
 * @param param2 MIDI Parameter 2
 */
typedef struct
{
	uint8_t param1;
	midi_status_t status;
	uint8_t channel;
	uint8_t param2;
} midi_message_t;

/**
 * @brief MIDI UART Configuration
 * @struct midi_config_t
 * @param uart_num UART Port
 * @param baudrate UART Baudrate
 * @param rx_io UART RX Pin
 * @param tx_io UART TX Pin
 */
typedef struct
{
	uart_port_t uart_num;
	uint baudrate;
	gpio_num_t rx_io;
	gpio_num_t tx_io;
} midi_config_t;

/// @typedef *midi_handle_t MIDI Context Handler
typedef struct midi_context_t *midi_handle_t;

/**
 * @brief initializes MIDI and allocates driver resources
 *
 * @param[out] out_handle MIDI Handle to be initialized
 * @param[out] out_cfg MIDI Configuration
 * @return esp_err_t
 */
esp_err_t midi_init(midi_handle_t *out_handle, midi_config_t *out_cfg);

/**
 * @brief Exits MIDI and frees all resources
 *
 * @param midi_handle MIDI Handle to be freed
 * @return esp_err_t
 */
esp_err_t midi_exit(midi_handle_t midi_handle);

/**
 * @brief Writes MIDI Message to UART
 *
 * @param midi_handle MIDI Handle to pass parameters
 * @param msg MIDI Message to be sent
 * @return esp_err_t
 */
esp_err_t midi_write(midi_handle_t midi_handle, midi_message_t *msg);

/**
 * @brief Reads MIDI Message from UART
 *
 * @param midi_handle MIDI Handle to pass parameters
 * @param msg MIDI Message to be read
 * @return esp_err_t
 */
esp_err_t midi_read(midi_handle_t midi_handle, midi_message_t *msg, TickType_t timeout);

// functions to configure midi messages
midi_message_t note_off(uint8_t channel, uint8_t key_num, uint8_t velocity);
midi_message_t note_on(uint8_t channel, uint8_t key_num, uint8_t velocity);
midi_message_t poly_key_pressure(uint8_t channel, uint8_t key_num, uint8_t value);
midi_message_t ctrl_change(uint8_t channel, uint8_t controller_num, uint8_t value);
midi_message_t prg_change(uint8_t channel, uint8_t program);
midi_message_t channel_pressure(uint8_t channel, uint8_t value);
midi_message_t pitch_bend(uint8_t channel, uint16_t value);

#endif // MIDI_DRIVER_H