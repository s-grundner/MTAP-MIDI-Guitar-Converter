#include "midi.h"

static const char *TAG = "midi";

struct midi_context_t
{
	midi_config_t config;
};
typedef struct midi_context_t midi_context_t;

esp_err_t midi_init(midi_context_t **out_handle, const midi_config_t *cfg)
{
	midi_context_t *ctx = (midi_context_t *)malloc(sizeof(midi_context_t));
	esp_err_t err = ESP_OK;
	if (ctx == NULL)
	{
		ESP_LOGE(TAG, "Failed to allocate memory for midi context");
		return ESP_ERR_NO_MEM;
	}

	// ------------------------------------------------------------
	// INIT USART1 DRIVER
	// ------------------------------------------------------------

	*ctx = (midi_context_t){
		.config = *cfg};

	*out_handle = ctx;
	return ESP_OK;
}
