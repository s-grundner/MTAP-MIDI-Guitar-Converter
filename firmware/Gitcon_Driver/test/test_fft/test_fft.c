#include <processed-data.h>
#include <fft.h>
#include <unity.h>

#define AUDIO_BUFFER_SIZE 512 // Size of buffer for FFT and sampler
#define F_SAMPLE_HZ 44100	  // Sample rate of FFT and sampler
#define FFT_WINDOW_SIZE 2	  // Amount of buffers to take for FFT
#define FFT_SIZE 4096		  //(AUDIO_BUFFER_SIZE * FFT_WINDOW_SIZE)  Amount of samples to take for FFT

void setUp(void)
{
}

void tearDown(void)
{
}

void test_processed_data(void)
{
	// fft variables
	float fft_buffer[FFT_SIZE];
	float magnitude[FFT_SIZE / 2];
	float frequency[FFT_SIZE / 2];
	unsigned char keyNR[FFT_SIZE / 2];
	float ratio = (float)F_SAMPLE_HZ / (float)FFT_SIZE;

	fft_config_t *real_fft_plan = fft_init(FFT_SIZE, FFT_REAL, FFT_FORWARD, test_buffer, fft_buffer);
	TEST_ASSERT_NOT_NULL(real_fft_plan);
	fft_execute(real_fft_plan);

	for (int k = 1; k < FFT_SIZE / 2; k++)
	{
		magnitude[k] = 2 * sqrt(pow(fft_buffer[2 * k], 2) + pow(fft_buffer[2 * k + 1], 2)) / FFT_SIZE;
		frequency[k] = k * ratio;
		keyNR[k] = (unsigned char)round(log2(frequency[k] / 440) * 12 + 69) % 128;
	}

	float max = 0;
	for (int i = 0; i < FFT_SIZE / 2; i++)
		max = (magnitude[i] > max) ? magnitude[i] : max;

	for (int k = 1; k < FFT_SIZE / 2; k++)
	{
		if (magnitude[k] >= max * 0.5)
		{
			TEST_ASSERT_EQUAL(45, keyNR[k]);
		}
	}
	fft_destroy(real_fft_plan);
}

void app_main()
{
	UNITY_BEGIN();
	RUN_TEST(test_processed_data);
	UNITY_END();
}