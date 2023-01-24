#include "midi.h"

// ------------------------------------------------------------
// MIDI UTILS
// ------------------------------------------------------------

midi_message_t note_off(uint8_t channel, uint8_t key_num, uint8_t velocity)
{
	midi_message_t msg = {
		.status = MIDI_STATUS_NOTE_OFF,
		.channel = channel,
		.param1 = key_num,
		.param2 = velocity};
	return msg;
}

midi_message_t note_on(uint8_t channel, uint8_t key_num, uint8_t velocity)
{
	midi_message_t msg = {
		.status = MIDI_STATUS_NOTE_ON,
		.channel = channel,
		.param1 = key_num,
		.param2 = velocity};
	return msg;
}

midi_message_t poly_key_pressure(uint8_t channel, uint8_t key_num, uint8_t value)
{
	midi_message_t msg = {
		.status = MIDI_STATUS_POLYPHONIC_KEY_PRESSURE,
		.channel = channel,
		.param1 = key_num,
		.param2 = value};
	return msg;
}

midi_message_t ctrl_change(uint8_t channel, uint8_t controller_num, uint8_t value)
{
	midi_message_t msg = {
		.status = MIDI_STATUS_CONTROL_CHANGE,
		.channel = channel,
		.param1 = controller_num,
		.param2 = value};
	return msg;
}

midi_message_t prg_change(uint8_t channel, uint8_t program)
{
	midi_message_t msg = {
		.status = MIDI_STATUS_PROGRAM_CHANGE,
		.channel = channel,
		.param1 = program,
		.param2 = 0};
	return msg;
}

midi_message_t channel_pressure(uint8_t channel, uint8_t value)
{
	midi_message_t msg = {
		.status = MIDI_STATUS_CHANNEL_PRESSURE,
		.channel = channel,
		.param1 = value,
		.param2 = 0};
	return msg;
}

midi_message_t pitch_bend(uint8_t channel, uint16_t value)
{
	midi_message_t msg = {
		.status = MIDI_STATUS_PITCH_BEND,
		.channel = channel,
		.param1 = value & 0x7F,
		.param2 = (value >> 7) & 0x7F};
	return msg;
}