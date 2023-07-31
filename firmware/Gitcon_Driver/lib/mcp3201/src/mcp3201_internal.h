#include "../mcp3201.h"

typedef struct mcp3201_data_s
{
	mcp3201_config_t mcp_cfg;
	spi_device_handle_t spi_handle;
	spi_transaction_t *cur_transaction;
} mcp3201_data_t;
