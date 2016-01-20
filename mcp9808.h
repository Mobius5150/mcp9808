/*
 * mcp9808.h
 *
 * Created: 2014-12-20 3:20:01 PM
 *  Author: michaelblouin
 */ 

#ifndef MCP9808_H_
#define MCP9808_H_

#include "temperatures.h"
#include "Defines.h"
#include <avr/builtins.h>

#define MCP9808_REG_CONFIG          0x01
#define MCP9808_REG_TUPPER          0x02
#define MCP9808_REG_TLOWER          0x03
#define MCP9808_REG_TCRIT           0x04
#define MCP9808_REG_TA              0x05
#define MCP9808_REG_MANUFACTURER_ID 0x06
#define MCP9808_REG_DEVICE_ID       0x07
#define MCP9808_REG_RESOLUTION      0x07

#define MCP9808_TEMP_REG_MASK       0x1FFF
#define MCP9808_TEMP_REG_MASK_UPPER (MCP9808_TEMP_REG_MASK & 0xFF00)
#define MCP9808_TEMP_REG_MASK_LOWER (MCP9808_TEMP_REG_MASK & 0x00FF)

#define MCP9808_TEMP_LIM_REG_MASK       0x1FFC
#define MCP9808_TEMP_LIM_REG_MASK_UPPER (MCP9808_TEMP_LIM_REG_MASK & 0xFF00)
#define MCP9808_TEMP_LIM_REG_MASK_LOWER (MCP9808_TEMP_LIM_REG_MASK & 0x00FF)

#define MCP9808_CONF_REG_MASK       0x01FF
#define MCP9808_CONF_REG_MASK_UPPER (MCP9808_CONF_REG_MASK & 0xFF00)
#define MCP9808_CONF_REG_MASK_LOWER (MCP9808_CONF_REG_MASK & 0x00FF)

#define MCP9808_MANUFACTURER_ID     0x0054
#define MCP9808_MIN_DEVICE_ID       0x0400

#define MCP9808_RESOLUTION_0P5      0b00
#define MCP9808_RESOLUTION_0P25     0b01
#define MCP9808_RESOLUTION_0P125    0b10
#define MCP9808_RESOLUTION_0P0625   0b11

#define MCP9808_CONFIG_ALERT_MODE      0
#define MCP9808_CONFIG_ALERT_POLARITY  1
#define MCP9808_CONFIG_ALERT_SELECT    2
#define MCP9808_CONFIG_ALERT_CONTROL   3
#define MCP9808_CONFIG_ALERT_STATUS    4
#define MCP9808_CONFIG_INT_CLR         5
#define MCP9808_CONFIG_WIN_LOCK        6
#define MCP9808_CONFIG_CRIT_LOCK       7
#define MCP9808_CONFIG_SHDN            8
#define MCP9808_CONFIG_THYST           9

#define MCP9808_ALERT_COMPARATOR 0
#define MCP9808_ALERT_INTERRUPT  1

#define MCP9808_ALERT_ALL_SOURCES 0
#define MCP9808_ALERT_TCRIT_ONLY  1

#define MCP9808_OUTPUT_ACTIVE_LOW  0
#define MCP9808_OUTPUT_ACTIVE_HIGH 1

#define MCP9808_ALERT_ENABLE  1
#define MCP9808_ALERT_DISABLE 0

#define MCP9808_ALERT_STATUS_NOT_ASSERTED 0
#define MCP9808_ALERT_STATUS_ASSERTED     1

#define MCP9808_OK 0

typedef enum {
	THYST_0C,
	THYST_1C5,
	THYST_3C,
	THYST_6C,
} MCP9808_THYST;

typedef enum {
	MCP9808_RUNNING = 0,
	MCP9808_SHUTDOWN = 1,
} MCP9808_SHUTDOWN_STATE;

typedef struct {
	int8_t Address;
	int16_t ManufacturerId;
	int16_t DeviceId;
	uint16_t Configuration;
	int16_t TUpper;
	int16_t TLower;
	int16_t TCrit;
	uint8_t Resolution;
	int16_t Temperature;
} MCP9808_DEVICE;

int16_t mcp9808_get_temp_int(int16_t read_value);
float   mcp9808_get_temp_float(int16_t read_value);
MCP9808_DEVICE mcp9808_load_device(int16_t address);
void mcp9808_reset_interrupts(MCP9808_DEVICE *device);
int16_t mcp9808_read_temperature(MCP9808_DEVICE *device);
int8_t mcp9808_write_and_verify_int16(MCP9808_DEVICE *device, int8_t reg, uint16_t value);
int8_t mcp9808_write_and_verify_int8(MCP9808_DEVICE *device, int8_t reg, uint8_t value);
int8_t mcp9808_apply_temperature_configuration(MCP9808_DEVICE *device);
void mcp9808_set_temperature(int16_t *tempVar, int8_t tempC);
int8_t mcp9808_apply_configuration(MCP9808_DEVICE *device);
void mcp9808_set_is_shutdown(MCP9808_DEVICE *device, MCP9808_SHUTDOWN_STATE is_shutdown);

#endif /* MCP9808_H_ */