/*
 * mcp9808.c
 *
 * Created: 2014-12-20 3:30:09 PM
 *  Author: michaelblouin
 */ 

#include "mbtwi.h"

#include "mcp9808.h"

/************************************************************************/
/* Converts the value read from the TA register on the MCP9808 to an int*/
/* representation that is 16x larger than the actual value.             */
/************************************************************************/
inline int16_t mcp9808_get_temp_int(int16_t read_value) {
	if (read_value & 0x1000) {
		// Negative temperature. Extend sign bit and shift right by 4
		return (int16_t)(0xF000 | read_value) + T_16xKELVIN;
	} else {
		// Positive temperature. Extend sign bit and shift right by 4
		return (int16_t)(0x0FFF & read_value) + T_16xKELVIN;
	}
}

/************************************************************************/
/* Converts the value read from the TA register on the MCP9808 to float */
/************************************************************************/
inline float   mcp9808_get_temp_float(int16_t read_value) {
	return ((float)mcp9808_get_temp_int(read_value)) / 16.0;
}

/************************************************************************/
/* Retrieves the information for the specified MCP9808 Device.          */
/************************************************************************/
MCP9808_DEVICE mcp9808_load_device(int16_t address) {
	MCP9808_DEVICE device;
	
	device.Address = address;
	
	// Read the manufacturer id
	send_twi_byte(device.Address, MCP9808_REG_MANUFACTURER_ID, 0);
	device.ManufacturerId = read_twi_int16(device.Address);

	// Read the device id
	send_twi_byte(device.Address, MCP9808_REG_DEVICE_ID, 0);
	device.DeviceId = read_twi_int16(device.Address);

	// Read the configuration
	send_twi_byte(device.Address, MCP9808_REG_CONFIG, 0);
	device.Configuration = read_twi_int16(device.Address);
	
	// Read the configuration
	send_twi_byte(device.Address, MCP9808_REG_RESOLUTION, 0);
	device.Resolution = read_twi_int16(device.Address);

	return device;
}

void mcp9808_reset_interrupts(MCP9808_DEVICE *device) {
	// Read the config
	send_twi_byte(device->Address, MCP9808_REG_CONFIG, 0);
	read_twi_int16(device->Address);
}

int8_t mcp9808_apply_configuration(MCP9808_DEVICE *device) {
	int8_t result = MCP9808_OK;
	
	// Read the config
	send_twi_byte(device->Address, MCP9808_REG_CONFIG, 0);
	int16_t config = read_twi_int16(device->Address);
	
	if (config & _BV(MCP9808_CONFIG_WIN_LOCK) || config & _BV(MCP9808_CONFIG_CRIT_LOCK)) {
		result |= 0b0001;
	}
	
	// Write the config
	if (mcp9808_write_and_verify_int16(device, MCP9808_REG_CONFIG, device->Configuration & MCP9808_CONF_REG_MASK)) {
		result |= 0b0010;
	}
	
	// Write the config
	if (mcp9808_write_and_verify_int8(device, MCP9808_REG_RESOLUTION, device->Resolution)) {
		result |= 0b0100;
	}
	
	return result;
}

void mcp9808_set_temperature(int16_t *tempVar, int8_t tempC) {
	if (tempC & 0x80) {
		*tempVar = (0x1000 | (tempC << 4)) & MCP9808_TEMP_LIM_REG_MASK;
	} else {
		*tempVar = (tempC << 4) & MCP9808_TEMP_LIM_REG_MASK;
	}
}

int8_t mcp9808_apply_temperature_configuration(MCP9808_DEVICE *device) {
	int8_t result = MCP9808_OK;
	
	// Write the config
	if (mcp9808_write_and_verify_int16(device, MCP9808_REG_TUPPER, (uint16_t)device->TUpper & MCP9808_TEMP_REG_MASK)) {
		result |= 0b0001;
	}
	
	if (mcp9808_write_and_verify_int16(device, MCP9808_REG_TLOWER, (uint16_t)device->TLower & MCP9808_TEMP_REG_MASK)) {
		result |= 0b0010;
	}
	
	if (mcp9808_write_and_verify_int16(device, MCP9808_REG_TCRIT, (uint16_t)device->TCrit & MCP9808_TEMP_REG_MASK)) {
		result |= 0b0100;
	}
	
	return result;
}

int8_t mcp9808_write_and_verify_int16(MCP9808_DEVICE *device, int8_t reg, uint16_t value) {
	int8_t buff[3] = { reg, (value & 0x7F00) >> 8, value & 0x00FF };
	send_twi_bin_data(device->Address, 3, buff);
	
	// Verify config
	send_twi_byte(device->Address, reg, 0);
	int16_t result = read_twi_int16(device->Address);
	if (value != result) {
		return 1;
	}
	
	return MCP9808_OK;
}

int8_t mcp9808_write_and_verify_int8(MCP9808_DEVICE *device, int8_t reg, uint8_t value) {
	int8_t buff[2] = { reg, value };
	send_twi_bin_data(device->Address, 2, buff);
	
	// Verify config
	send_twi_byte(device->Address, reg, 0);
	if (value != read_twi_int8(device->Address)) {
		return 1;
	}
	
	return MCP9808_OK;
}

int16_t mcp9808_read_temperature(MCP9808_DEVICE *device) {
	send_twi_byte(device->Address, MCP9808_REG_TA, 0);
	device->Temperature = read_twi_int16(device->Address);
	return device->Temperature;
}

void mcp9808_set_is_shutdown(MCP9808_DEVICE *device, MCP9808_SHUTDOWN_STATE is_shutdown) {
	if (is_shutdown) {
		device->Configuration |= (1 << MCP9808_CONFIG_SHDN);
	} else {
		device->Configuration &= ~(1 << MCP9808_CONFIG_SHDN);
	}
}