#include <config.h>
#include "mcp3201.h"

typedef struct
{
	mcp3201_handle_t mcp3201;
} gitcon_config_t;
typedef gitcon_config_t *gitcon_handle_t;

esp_err_t gitcon_init(gitcon_handle_t *out_handle);
esp_err_t gitcon_exit(gitcon_handle_t handle);