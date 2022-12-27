/**
 * @file 	scale.h
 * @author 	@h-ihninger
 * @author 	@s-grundner
 * @brief 	Basic Functions for Frequency calculation
 * @version 0.1
 * @date 	2022-05-05
 *
 * @copyright Copyright (c) 2022
 */

#ifndef SCALE_H_
#define SCALE_H_

#include "inttypes.h"
#include <math.h>
#include <stdio.h>

#define CONCERT_PITCH 440.0
#define ADC_MAX_VAL 255.0
#define A0_NUM 49

typedef enum
{
	KEY_A = 0,
	KEY_AS = 1,
	KEY_B = 2,
	KEY_C = 3,
	KEY_CS = 4,
	KEY_D = 5,
	KEY_DS = 6,
	KEY_E = 7,
	KEY_F = 8,
	KEY_FS = 9,
	KEY_G = 10,
	KEY_GS = 11,
} key_name_t;

#define MODAL_COUNT 4

/**
 * @brief Modal Names
 * @param MAJ Major
 * @param MIN Minor
 * @param HMJ Harmonic Major
 * @param HMN Harmonic Minor
 */
typedef enum
{
	MAJ = 0,
	MIN = 1,
	HMJ = 2,
	HMN = 3,
} modal_name_t;

/**
 * @brief Convert Keynumber to Frequency
 *
 * @param key_num Keynumber
 * @return double Frequency
 */
double get_pitch_hz(uint8_t key_num);

/**
 * @brief Convert Frequency to Keynumber
 *
 * @param freq Frequency
 * @return uint8_t Keynumber
 */
uint8_t get_key_num(double freq);

/**
 * @brief print Keyname
 *
 * @param key_num Keynumber
 */
void print_key_name(uint8_t key_num);

/**
 * @brief Convert ADC Value to Frequency
 *
 * @param adc_val ADC Value
 * @param oct_offset Octave Offset
 * @return double Frequency
 */
double adc_to_pitch(uint8_t adc_val, uint8_t oct_offset);

/**
 * @brief Convert ADC Value to Keynumber
 *
 * @param adc_val ADC Value
 * @param oct_offset Octave Offset
 * @return uint8_t Keynumber
 */
uint8_t adc_to_num(uint8_t adc_val, uint8_t oct_offset);

/**
 * @brief Get a key name
 *
 * @param key_num Key number
 * @return char* Keyname
 */
char *get_key_name(uint8_t key_num);

/**
 * @brief Get a modal name
 *
 * @param modal_num Number of the modal
 * @return char*
 */
char *get_modal_name(uint8_t modal_num);
#endif // SCALE_H_