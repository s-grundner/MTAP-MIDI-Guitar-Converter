/**
 * @file midi.c
 * @author @s-grundner
 * @brief MIDI driver for ESP32
 * @version 0.1
 * @date 2022-12-23
 *
 * @copyright Copyright (c) 2022
 *
 */
#include "midi.h"

#define MIDI_UART_DATA_SIZE 3
#define MIDI_UART_SHORT_DATA_SIZE 2
static const char *TAG = "midi";

typedef struct midi_context_t
{
	midi_config_t cfg;
} midi_context_t;

char midi_init(midi_context_t **out_ctx, midi_config_t *cfg)
{
	midi_context_t *ctx = (midi_context_t *)malloc(sizeof(midi_context_t));

	if (!ctx)
		return -1;

	cfg->user_init(cfg->user);

	*ctx = (midi_context_t){
		.cfg = *cfg};

	return 0;
}

void midi_pitch_bend(midi_handle_t midi_handle, uint8_t channel, uint16_t value)
{
	// ------------------------------------------------------------
	// SEND MIDI PITCH BEND
	// ------------------------------------------------------------

	midi_message_t message = {
		.status = MIDI_STATUS_PITCH_BEND,
		.channel = channel,
		.param1 = value & 0x7F,
		.param2 = (value >> 7) & 0x7F};

	midi_handle->cfg.user_send(midi_handle->cfg.user, &message, MIDI_BYTE_SIZE_DEFAULT);
}

void midi_exit(midi_handle_t midi_handle)
{
	midi_handle->cfg.user_exit(midi_handle->cfg.user);
	free(midi_handle);
}