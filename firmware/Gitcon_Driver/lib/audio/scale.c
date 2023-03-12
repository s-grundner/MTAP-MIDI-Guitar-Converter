#include "scale.h"
#include "esp_log.h"

static const char* TAG = "scale";

static char key_name[12][3] = { "G* ", "A  ", "A* ", "B  ", "C  ", "C* ", "D  ", "D* ", "E  ", "F  ", "F* ", "G  " };
static char modal_name[MODAL_COUNT][3] = { "MAJ", "MIN", "HMJ", "HMN" };

uint8_t get_key_num(double freq)
{
	if (freq <= 0)
		return 0;
	return log2(freq / CONCERT_PITCH) * 12.0 + (double)A0_NUM;
}

double get_pitch_hz(uint8_t key_num)
{
	return pow(2.0, ((double)key_num - (double)A0_NUM) / 12.0) * CONCERT_PITCH;
}

void print_key_name(uint8_t key_num)
{
	ESP_LOGI(TAG, "%c%c%d", key_name[key_num % 12][0], key_name[key_num % 12][1], 1 + ((key_num - 4) / 12));
}

char* get_key_name(uint8_t key_num)
{
	return key_name[key_num % 12];
}

char* get_modal_name(uint8_t modal_num)
{
	return modal_name[modal_num];
}

double adc_to_pitch(uint8_t adc_val, uint8_t oct_offset)
{
	return get_pitch_hz(adc_to_num(adc_val, oct_offset));
}

uint8_t adc_to_num(uint8_t adc_val, uint8_t oct_offset)
{
	return (12.0 * ((float)oct_offset + 2.0 * ((float)adc_val / ADC_MAX_VAL))) + 0.5;
}