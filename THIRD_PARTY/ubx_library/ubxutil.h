#ifndef UBXUTIL_H_INCLUDED
#define UBXUTIL_H_INCLUDED

#include "ubxbuffer.h"

#if defined _WIN32 || defined _WIN64
#include "ubxutil_win.h"
#elif defined __unix
#include <sys/time.h>
#endif

#include <stdint.h>
#include <stdio.h>

typedef enum {
	/** @brief Configuration command pending */
	UBX_CONFIG_RESULT_PENDING = -1,
	/** @brief Configuration command acknowledged */
	UBX_CONFIG_RESULT_ACK = 0,
	/** @brief Configuration command rejected */
	UBX_CONFIG_RESULT_NAK = 1,
	/** @brief Configuration command timeout */
	UBX_CONFIG_RESULT_TIMEOUT = 2,
	/** @brief Error occurred during sending configuration command */
	UBX_CONFIG_RESULT_ERROR = 3,
} ubx_config_result_t;

/**
 * @brief Detect the baud rate of a serial port
 * @param[in] port Serial port to check
 * @return The detected baud rate, -1 if could not detect
 *
 * @details
 * The detection procedure sets the baud rate and then tries to read from the 
 * port. If a UBX or NMEA message is successfully read, then the baud rate is
 * detected. Unsuccessful detection occurs if all the possible baud rates 
 * result in timeout.
 */
int detect_ubx_baud_rate(port_t port);


/**
 * @brief Read one U-Blox command from a file
 * @param[in] fp File pointer (file should be opened in text read mode)
 * @param[in] buf Buffer allocated for command bytes
 * @param[in] size Size of the allocated buffer in bytes
 * @return Number of bytes written to buffer
 * 
 * @details
 * The checksum of the command is calculated and is written into the
 * message buffer.
 */
size_t read_ubx_command(FILE *fp, uint8_t *buf, size_t size);

/**
 * @brief Read configuration commands from a text file into a buffer
 * @param[in] fname File name to read from
 * @param[in] buf Array allocated for reading the data
 * @param[out] n_command Number of commands read
 * @return Number of bytes created in the buffer
 *
 * @details
 * This routine support reading GPS module configuration files for
 * uploading the stored configuration into the GSP receiver.
 * The configuration file is a text file that can be created
 */
size_t read_ubx_config_file(char const * fname, uint8_t *buf, size_t size, size_t *n_command);


ubx_config_result_t send_ubx_config(port_t port, uint8_t *buf, size_t size, long timeout);

/**
 * @brief Upload configuration to U-blox module
 * @param[in] port Serial port to use
 * @param[in] buf Buffer storing the configuration commands
 * @param[in] size Number of useful bytes in the command buffer
 * @param[in] timeout Timeout 
 */
int upload_ubx_config(port_t port, uint8_t *buf, size_t size, long timeout, int retries);

/**
 * @brief Change port baudrate of the U-blox module, then change the serial port accordingly
 * @param[in] port Serial port to use
 * @param[in] rate Baudrate to change the port to
 */
int change_ubx_baudrate(port_t port, int rate);

/**
 * @brief Save current U-Blox configuration to nonvolatile memory
 * @param[in] port Serial port to use
 */
int save_ubx_config(port_t port);

#endif /* UBXUTIL_H_INCLUDED */
