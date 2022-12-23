#pragma once

#include <stdio.h>
#include "driver/gpio.h"
#include "esp_log.h"
#include "driver/uart.h"

typedef struct
{
	uint8_t status;	 // 4bit
	uint8_t channel; // 4bit
	uint8_t note_num;
	uint8_t velocity;
} midi_message_t;

typedef struct
{
	uart_port_t uart;
} midi_config_t;
typedef midi_context_t *midi_handle_t;

esp_err_t midi_init(midi_handle_t *out_handle, const midi_config_t *cfg);
esp_err_t midi_exit(midi_handle_t handle);
