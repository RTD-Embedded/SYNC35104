#include "serial_port.h"
#include "ubxproto.h"
#include "ubxutil.h"

#include <inttypes.h>
#include <time.h>
#include <termios.h>

#include <unistd.h>

int detect_ubx_baud_rate(port_t port)
{
	// Baud rates to try

	static int const baud_rates[] = { 9600, 19200, 38400, 57600, 115200 };
	// Timeout for receiving messages (Note: timeout seems to be very important)
	static long const timeout = 500000;

	ubx_protocol_type_t proto;

	ubx_ring_buffer_t ubx_buf;
	unsigned int offset, length;

	struct timeval then, now;
	long elapsed = 0;
	int success = 0;

	int baud_rate;					// Current baud rate
	
	for (int i = 0; i < sizeof(baud_rates) / sizeof(baud_rates[0]); ++i) {
		baud_rate = baud_rates[i];

		int retval = set_serial_port_rate(port, baud_rate);
		if (!retval) {
			fprintf(stderr, "Could not set serial port rate");
			return -1;
		}

		// Try to read from port
		ubx_buf_init(&ubx_buf);

		// Mark start time
		gettimeofday(&then, NULL);

		do {
			// Read from port
			int received = ubx_buf_read_port(port, &ubx_buf);
			if (received < 0)
				return -1;

			if (received == 0) {
				continue;
			}

			// Read all the received messages
			do {
				proto = ubx_buf_find_packet(&ubx_buf, &offset, &length);
				ubx_buf_consume(&ubx_buf, offset);

				// Consume if protocol recongized			
				if (proto != UBX_PROTO_NONE) {
					// Protocol has been found, we are OK
					ubx_buf_consume(&ubx_buf, length);
					success = 1;
					break;
				}
			} while (proto != UBX_PROTO_NONE);

			if (success)
				break;

			gettimeofday(&now, NULL);

			// Compute elapsed time
			elapsed = (now.tv_sec - then.tv_sec) * 1000000l +
				(now.tv_usec - then.tv_usec);
			
		} while (elapsed < timeout);

		// Close the opened port
		
		if (success)
			break;
	}

	if (success)
		return baud_rate;
	else
		return -1;
}

/**
 * @brief Read commands from a text file into a buffer
 * @return Number of bytes created in the buffer
 *
 * @details
 * 
 */
size_t read_ubx_command(FILE * fp, uint8_t* buf, size_t size)
{
	// Fill synchronization characters
	buf[0] = UBX_SYNC_CHAR1;
	buf[1] = UBX_SYNC_CHAR2;

	uint8_t chk_a = 0, chk_b = 0;

	for (int i = 0; i < 4; ++i) {
		if (fscanf(fp, "%02"SCNx8, &buf[i + 2]) != 1) {
			return -1;
		}

		chk_a = chk_a + buf[i + 2];
		chk_b = chk_b + chk_a;
	}

	// Caclulate length
	uint16_t len = ((uint16_t)buf[4]) + (((uint16_t)buf[5]) << 8);

	// Read the payload
	for (int i = 0; i < len; ++i) {
		if (fscanf(fp, "%02"SCNx8, &buf[i + 6]) != 1) {
			return -1;
		}

		chk_a = chk_a + buf[i + 6];
		chk_b = chk_b + chk_a;
	}

	// Store the checksum
	buf[len + 6] = chk_a;
	buf[len + 7] = chk_b;

	return len + UBX_OVERHEAD_SIZE;
}

size_t read_ubx_config_file(char const * fname, uint8_t *buf, size_t size, size_t *n_command)
{
	size_t offset = 0;
	size_t length = 0;

	char msg_name[32];
	*n_command = 0;
	
	FILE * fp = fopen(fname, "r");
	if (fp == NULL) {
		fprintf(stderr, "Could not open file: %s\n", fname);
		return -1;
	}

	while (!feof(fp)) {
		// Read the identifier
		if (fscanf(fp, "%32s - ", msg_name) != 1) {
			if (feof(fp)) {
				// Ready, reached end of file
				break;
			} else {
				fclose(fp);
				fprintf(stderr, "Error reading file!");
				return -1;
			}
		}

		// Read one command
		length = read_ubx_command(fp, buf + offset, size - offset);

		if (length == -1) {
			fclose(fp);
			fprintf(stderr, "Error reading file!");
			return -1;
		}

		if (buf[offset + UBX_HEADER_CLASS_OFFSET] == UBX_MSG_CLASS_CFG) {
			offset += length;
			(*n_command)++;
		}

	}

	fclose(fp);
	return offset;
}

ubx_config_result_t send_ubx_config(port_t port, uint8_t *buf, size_t size, long timeout)
{
	ubx_config_result_t result;
	ubx_protocol_type_t proto;

	ubx_ring_buffer_t ubx_buf;

	struct timeval then, now;
	long elapsed;
	unsigned int offset, length;

	ubx_buf_init(&ubx_buf);

	// Extract message id 
	uint8_t req_class = buf[UBX_HEADER_CLASS_OFFSET];
	uint8_t req_id = buf[UBX_HEADER_ID_OFFSET];

	// Send the message
	write_port(port, buf, size);

	gettimeofday(&then, NULL);

	result = UBX_CONFIG_RESULT_PENDING;
	do {
		// Read from port
		int received = ubx_buf_read_port(port, &ubx_buf);
		if (received < 0)
			return UBX_CONFIG_RESULT_ERROR;

		// Read all the received messages
		do {
			proto = ubx_buf_find_packet(&ubx_buf, &offset, &length);
			ubx_buf_consume(&ubx_buf, offset);

			// Process UBX message
			if (proto == UBX_PROTO_UBX) {
				// ACK message found
				if (ubx_buf_at(&ubx_buf, UBX_HEADER_CLASS_OFFSET) == UBX_MSG_CLASS_ACK &&
					((ubx_buf_at(&ubx_buf, UBX_HEADER_ID_OFFSET) == UBX_MSG_ACK_NAK) ||
					(ubx_buf_at(&ubx_buf, UBX_HEADER_ID_OFFSET) == UBX_MSG_ACK_ACK)))
				{

					// ACK to to our request
					if (ubx_buf_at(&ubx_buf, UBX_HEADER_SIZE + 0) == req_class &&
						ubx_buf_at(&ubx_buf, UBX_HEADER_SIZE + 1) == req_id)
					{
						// Not acknowledged
						if (ubx_buf_at(&ubx_buf, UBX_HEADER_ID_OFFSET) == UBX_MSG_ACK_NAK)
							result = UBX_CONFIG_RESULT_NAK;
						if (ubx_buf_at(&ubx_buf, UBX_HEADER_ID_OFFSET) == UBX_MSG_ACK_ACK)
							result = UBX_CONFIG_RESULT_ACK;
						// Ready
						break;
					}

				}
			}
			// Consume if protocol recongized			
			if (proto != UBX_PROTO_NONE)
				ubx_buf_consume(&ubx_buf, length);
		} while (proto != UBX_PROTO_NONE);

		gettimeofday(&now, NULL);

		// Compute elapsed time
		elapsed = (now.tv_sec - then.tv_sec) * 1000000l +
			(now.tv_usec - then.tv_usec);

		if (elapsed > timeout)
			result = UBX_CONFIG_RESULT_TIMEOUT;
	} while (result == UBX_CONFIG_RESULT_PENDING);

	return result;
}

int upload_ubx_config(port_t port, uint8_t *buf, size_t size, long timeout, int retries)
{
	size_t offset = 0;
	size_t length;
	int success = 0;
	ubx_config_result_t cfg_result;
	int i;
	int count = 0;

	while (offset + UBX_OVERHEAD_SIZE < size) {
		// Extract message length
		length = (uint16_t)(buf[offset + UBX_HEADER_LENGTH_OFFSET + 0]) +
			((uint16_t)(buf[offset + UBX_HEADER_LENGTH_OFFSET+ 1]) << 8) +
			UBX_OVERHEAD_SIZE;

		// Check if message fits
		if (offset + length > size)
			break;

		printf("Sending config message #%d ... ", count + 1);
		fflush(stdout);
		// Try to send message
		for (i = 0; i < retries + 1; ++i) {
			cfg_result = send_ubx_config(port, buf + offset, length, timeout);
			if (cfg_result != UBX_CONFIG_RESULT_TIMEOUT)
				break;
		}

		switch (cfg_result) {
		case UBX_CONFIG_RESULT_ACK:
			printf("Successfully completed.\n");
			success++;
			break;
		case UBX_CONFIG_RESULT_NAK:
			printf("Rejected.\n");
			success++;
			break;
		case UBX_CONFIG_RESULT_TIMEOUT:
			printf("Failed due to timeout.\n");
			break;
		case UBX_CONFIG_RESULT_ERROR:
			printf("Error occurred.\n");
			return -1;
			break;
		}

		offset += length;
		count++;
	}

	return success;
}

int change_ubx_baudrate(port_t port, int rate)
{
	uint8_t msg[] = {
		0xB5, 0x62, /* sync */
		0x06, 0x00, /* CFG-PRT */
		0x14, 0x00, /* 20 bytes */
		0x01,       /* port 1 uart */
		0x00,       /* rsvd */
		0x00, 0x00, /* txready disabled */
		0xC0, 0x08, 0x00, 0x00, /* mode 8n1 */
		0x00, 0xC2, 0x01, 0x00, /* baudrate 115200 */
		0x07, 0x00, /* inproto */
		0x23, 0x00, /* outproto */
		0x00, 0x00, /* flags */
		0x00, 0x00, /* rsvd */
		0xD0, 0x3E, /* ck_a ck_b */
	};

	if (rate != 4800 &&
		rate != 9600 &&
		rate != 19200 &&
		rate != 38400 &&
		rate != 57600 &&
		rate != 115200 &&
		rate != 230400 &&
		rate != 460800) {
		printf("Error: unsupported baurate:%d\n", rate);
		return -1;
	}

	/* setup the ubx message */
	*((uint32_t*)(msg + 14)) = rate;

	uint8_t ck_a = 0;
	uint8_t ck_b = 0;
	for (int i = 0; i < 24; i++) {
		ck_a = ck_a + msg[i + 2];
     	ck_b = ck_b + ck_a;
	}
	msg[26] = ck_a;
	msg[27] = ck_b;

#if 0
	printf("UBX message: ");
	for(int j = 0; j < 28;j++) {
		printf("%02X ", msg[j]);
	}
	printf("\n");
#endif

	/* send the ubx message */
	int s_commands = upload_ubx_config(port, msg, 28, 500000, 1);
	/* if the baud rate change is successful, the acknowledge can not be received */
	if (s_commands != 1) {
		printf("Failed to send CFG-PRT message %d \n", s_commands);
		printf("If the baudrate is changed, the ack can not be received\n");
	}

	/* set the port baud rate */
	printf("Setting serial port baud rate to %d\n", rate);
	int retval = set_serial_port_rate(port, rate);
	if (retval) {
		printf("Failed to set serial port baud rate %d\n", retval);
		return -3;
	}

	return 0;
}

int save_ubx_config(port_t port) {

	uint8_t msg[] = {
		0xB5, 0x62,             /* sync */
		0x06, 0x09,             /* CFG-CFG */
		0x0C, 0x00,             /* 12 bytes */
		0x00, 0x00, 0x00, 0x00, /* clearMask */
		0xFF, 0xFF, 0xFF, 0xFF, /* saveMask */
		0x00, 0x00, 0x00, 0x00, /* loadMask */
		0x17, 0x75,             /* ck_a ck_b */
	};

	/* send the ubx message */
	int s_commands = upload_ubx_config(port, msg, sizeof(msg), 500000, 1);
	if (s_commands != 1) {
		printf("Failed to save config %d \n", s_commands);
		return -1;
	}
	return 0;
}
