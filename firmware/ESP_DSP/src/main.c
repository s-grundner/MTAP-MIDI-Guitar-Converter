#define NFFT 8192
#define F_ABT 44100

#include <stdio.h>
#include <stdlib.h>
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/adc.h"
#include "esp_adc_cal.h"
#include <math.h>
#include "fft.h"
#include "processed-data.h" //enthält test_buffer

float fft_buffer[NFFT];
float magnitude[NFFT / 2];
float frequency[NFFT / 2];
float keyNR[NFFT / 2];
float ratio = (float)F_ABT / (float)NFFT;
char file_buffer[NFFT];

float getMaxMag();
float max;

void app_main(void)
{
    fft_config_t *real_fft_plan = fft_init(NFFT, FFT_REAL, FFT_FORWARD, test_buffer, fft_buffer);

    fft_execute(real_fft_plan);

    // printf("ratio: %f\n", ratio);
    // printf("DC component : %f\n", fft_buffer[0] / NFFT); // DC is at [0]

    for (int k = 1; k < NFFT / 2; k++)
    {
        magnitude[k] = 2 * sqrt(pow(fft_buffer[2 * k], 2) + pow(fft_buffer[2 * k + 1], 2)) / NFFT;
        frequency[k] = k * ratio;
        keyNR[k] = log2(frequency[k] / 440) * 12 + 49;
    }
    
    max = getMaxMag();

    for (int k = 1; k < NFFT / 2; k++)
    {
        // printf("%d-th freq : %f+(%f)j\n", k, fft_buffer[2*k], fft_buffer[2*k+1]);
        // printf("%f\t(%f)j\n", fft_buffer[2*k], fft_buffer[2*k+1]);

        if (magnitude[k] > max*0.5)
        {
            printf("%d-th magnitude: %f => corresponds to %f Hz\n", k, magnitude[k], frequency[k]);
            printf("keyNR: %d\n", (int)round(keyNR[k]));
        }
        // printf("%f\n", magnitude[k]);
    }
    // printf("Middle component : %f\n", fft_buffer[1]); // N/2 is real and stored at [1]

    fft_destroy(real_fft_plan);
}

float getMaxMag()
{
    float max = 0;
    for (int i = 0; i < NFFT / 2; i++)
    {
        if (magnitude[i] > max)
        {
            max = magnitude[i];
        }
    }
    return max;
}