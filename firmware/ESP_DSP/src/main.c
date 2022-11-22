#define NFFT 64

#include <stdio.h>
#include <stdlib.h>
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/adc.h"
#include "esp_adc_cal.h"
#include <math.h>
#include "fft.h"

static const char *TAG = "ADC EXAMPLE";
static esp_adc_cal_characteristics_t adc1_chars;

float test_buffer[64];
float fft_buffer[64];

void init_test_buffer()
{
    for (int i = 0; i < 64; i+=2)
    {
        test_buffer[i] = 200 * sin(2 * M_PI * i / 32);
        test_buffer[i+1] = 0;
    }
}

void app_main(void)
{   

    fft_config_t *real_fft_plan = fft_init(NFFT, FFT_REAL, FFT_FORWARD, test_buffer, fft_buffer);
    init_test_buffer();
    uint32_t voltage;
    esp_adc_cal_characterize(ADC_UNIT_1, ADC_ATTEN_DB_11, ADC_WIDTH_BIT_DEFAULT, 0, &adc1_chars);
    ESP_ERROR_CHECK(adc1_config_width(ADC_WIDTH_BIT_DEFAULT));
    ESP_ERROR_CHECK(adc1_config_channel_atten(ADC1_CHANNEL_6, ADC_ATTEN_DB_11));//ADC1_CHANNEL_6 is GPIO34
    
    fft_execute(real_fft_plan);

    printf("DC component : %f\n", real_fft_plan->output[0]);  // DC is at [0]
    for (int k = 1; k < real_fft_plan->size / 2 ; k++){
        //printf("%d-th freq : %f+j%f\n", k, real_fft_plan->output[2*k], real_fft_plan->output[2*k+1]);
        printf("%d-th magnitude : %f\n", k, sqrt(real_fft_plan->output[2*k]*real_fft_plan->output[2*k] + real_fft_plan->output[2*k+1]*real_fft_plan->output[2*k+1]));
        //printf("%d-th phase : %f\n", k, atan2(real_fft_plan->output[2*k+1], real_fft_plan->output[2*k]));
    }
    printf("Middle component : %f\n", real_fft_plan->output[1]);  // N/2 is real and stored at [1]
    
    // Don't forget to clean up at the end to free all the memory that was allocated
    fft_destroy(real_fft_plan);

}