#ifndef SI5395_TYPES_H_INCLUDED
#define SI5395_TYPES_H_INCLUDED

#include <stdint.h>
typedef uint16_t si5395_reva_address_t;
typedef uint8_t si5395_reva_value_t;
 typedef uint8_t si5395_reva_page_t;
typedef uint8_t si5395_reva_offset_t;
 typedef struct {
	si5395_reva_address_t address;	/* 16-bit register address */
	si5395_reva_value_t value;	/* 8-bit register data */
} si5395_reva_register_t;
 typedef enum { 
	/** @brief Set address */ 
	    SI5395_SPI_COMMAND_SET_ADDR = 0x00, 
	/** @brief Write data */ 
	    SI5395_SPI_COMMAND_WRITE_DATA = 0x40, 
	/** @brief Read data */ 
	    SI5395_SPI_COMMAND_READ_DATA = 0x80, 
	/** @brief Write data and increment address */ 
	    SI5395_SPI_COMMAND_WRITE_INC = 0x60, 
	/** @brief Read data and increment address */ 
	    SI5395_SPI_COMMAND_READ_INC = 0xA0, 
	/** @brief Burst write data */ 
	SI5395_SPI_COMMAND_WRITE_BURST = 0xE0, 
} si5395_reva_spi_commands_t;
 typedef enum { SI5395_PAGE_0 = 0x00, SI5395_PAGE_1 = 0x01,  
} si5395_reav_pages_t;
 typedef enum { 
	/** 
 	 * @brief Page register address 
	 * @details
	 * Selects one of 256 possible pages
	 */ 
	    SI5395_ADDR_PAGE_REGISTER = 0x01, 
	/** 
	 * @brief Part number low register address 
	 * @details
	 * 
	 */ 
	    SI5395_ADDR_PN_BASE_LOW = 0x02, 
	/** @brief Part number high register address */ 
	    SI5395_ADDR_PN_BASE_HIGH = 0x03, 
	/**
	 * @brief Device grade register address 
	 * @details
	 * Number, sym
	 */ 
	    SI5395_ADDR_DEVICE_GRADE = 0x04, SI5395_ADDR_DEVICE_REV = 0x05, 
	/**
	 * @brief Device temperature grade
	 */ 
	    SI5395_ADDR_TEMP_GRADE = 0x09, 
	/**
	 * @brief Package ID
	 * @details
	 * Can take the following values:
	 *	0: 9x9 mm 64 QFN
	 */ 
	    SI5395_ADDR_PKG_ID = 0x0A, SI5395_ADDR_I2CADDR = 0x0B, 
	/**
	 * @brief Internal status of the device (read only)
	 * @details
	 *	Bit 0: 1 if the device is calibrating
	 */ 
	    SI5395_ADDR_INTERNAL_STATUS = 0x0C, 
	/**
	 * @brief Out-of-frequency and loss-of-signal alarms (read only)
	 */ 
	    SI5395_ADDR_LOSOOF = 0x0D, 
	/**
	 * @brief Holdover and Loss-of-lock status (read only)
	 * @details
	 *	Bit 1: LOL
	 *	Bit 5: HOLD (holdover or free run)
	 */ 
	    SI5395_ADDR_LOLHOLD = 0x0E, 
	/**
	 * @brief Calibration status (read only)
	 * @details
	 */ 
	    SI5395_ADDR_CAL_PLL = 0x0F, 
	/**
	 * @brief Internale error flags (read/write)
	 * @details
	 *	Bit 0: SYSINCAL_FLG
	 *	Bit 1: LOSXAXB_FLG
	 *	Bit 3: XAXB_ERR_FLG
	 *	Bit 5: SMBUS_TIMEOUT_FLG
	 */ 
	SI5395_ADDR_DEVICE_READY = 0xFE, 
} si5395_reva_page0_addresses_t;
 
#endif /*  */
