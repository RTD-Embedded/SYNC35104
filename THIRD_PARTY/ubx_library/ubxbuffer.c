#include "ubxbuffer.h"

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static inline uint8_t hex2dec(uint8_t d)
{
	if (d >= '0' && d <= '9') return (d - '0');
	if (d >= 'A' && d <= 'F') return (d - 'A' + 10);
	if (d >= 'a' && d <= 'f') return (d - 'a' + 10);
	return 0xFF;
}

static inline uint8_t nmea_chksum(uint8_t hi, uint8_t lo)
{
	return hex2dec(hi) * 16 + hex2dec(lo);
}

#ifndef min
static inline long int min(long int a, long int b)
{
	return a < b ? a : b;
}
#endif

int ubx_buf_init(ubx_ring_buffer_t *buf)
{
	buf->r_ptr = 0;
	buf->w_ptr = 0;
	return 0;
}

unsigned int ubx_buf_consume(ubx_ring_buffer_t *buf, unsigned int n)
{
	unsigned int avail = ubx_buf_get_free(buf);
	n = min(n, avail);
	buf->r_ptr += n;
	return n;
}

size_t ubx_buf_copy(ubx_ring_buffer_t *buf, uint8_t *dest, size_t size)
{
	unsigned int avail = ubx_buf_get_used(buf);
	size = min(size, avail);

	if (size < UBX_BUFFER_SIZE - (buf->r_ptr & UBX_BUFFER_MASK)) {
		memcpy(dest, buf->data + (buf->r_ptr & UBX_BUFFER_MASK), size);
	} else {
		memcpy(dest, buf->data + (buf->r_ptr & UBX_BUFFER_MASK),
			UBX_BUFFER_SIZE - (buf->r_ptr & UBX_BUFFER_MASK));
		memcpy(dest + UBX_BUFFER_SIZE - (buf->r_ptr & UBX_BUFFER_MASK),
			buf->data, size - (UBX_BUFFER_SIZE - (buf->r_ptr & UBX_BUFFER_MASK)));
	}

	return size;
}

int ubx_buf_read_file(FILE * fp, ubx_ring_buffer_t *buf, size_t n)
{
	size_t to_read;
	size_t received;
	int bytes_read = 0;

	if ((buf->r_ptr - 1) % UBX_BUFFER_SIZE > buf->w_ptr % UBX_BUFFER_SIZE) {
		// In this case we only need to read once
		to_read = min((buf->r_ptr - 1) % UBX_BUFFER_SIZE - buf->w_ptr % UBX_BUFFER_SIZE, n);
		received = fread(buf->data + buf->w_ptr % UBX_BUFFER_SIZE, sizeof(uint8_t), to_read, fp);

		buf->w_ptr += received;
		bytes_read = received;
	} else {
		// In this case we may need to read twice
		to_read = min(UBX_BUFFER_SIZE - (buf->w_ptr % UBX_BUFFER_SIZE), n);
		received = fread(buf->data + buf->w_ptr % UBX_BUFFER_SIZE, sizeof(uint8_t), to_read, fp);

		buf->w_ptr += received;
		bytes_read = received;
		// Need to read twice if there is more data available
		if (received == to_read) {
			to_read = min((buf->r_ptr - 1) % UBX_BUFFER_SIZE, n - received);
			received = fread(buf->data, sizeof(uint8_t), to_read, fp);

			buf->w_ptr += received;
			bytes_read += received;
		}

	}
	// Return the number of bytes received	
	return bytes_read;
}

size_t ubx_buf_read_port(port_t port, ubx_ring_buffer_t *buf)
{
	size_t bytes_read = 0;
	size_t to_read;
	size_t retval;

	if (buf->r_ptr % UBX_BUFFER_SIZE > buf->w_ptr % UBX_BUFFER_SIZE) {
		// In this case we only need to read once
		to_read = buf->r_ptr % UBX_BUFFER_SIZE - buf->w_ptr % UBX_BUFFER_SIZE;
		retval = read_port(port, buf->data + (buf->w_ptr & UBX_BUFFER_MASK), to_read);
		if (retval < 0) {
			return retval;
		}
		buf->w_ptr += retval;
		bytes_read = retval;
	} else {
		// In this case we may need to read twice
		to_read = UBX_BUFFER_SIZE - (buf->w_ptr % UBX_BUFFER_SIZE);
		retval = read_port(port, buf->data + (buf->w_ptr & UBX_BUFFER_MASK), to_read);
		if (retval < 0) {
			return retval;
		}
		buf->w_ptr += retval;
		bytes_read = retval;
		// Need to read twice if there is more data available
		if (retval == to_read) {
			to_read = (buf->r_ptr - 1) % UBX_BUFFER_SIZE;
			retval = read_port(port, buf->data, to_read);
			if (retval < 0) {
				return retval;
			}

			buf->w_ptr += retval;
			bytes_read += retval;
		}

	}
	// Return the number of bytes received	
	return bytes_read;
}

ubx_protocol_type_t ubx_buf_find_packet(ubx_ring_buffer_t *buf, unsigned int *offset, unsigned int *length)
{
	// These are the valid NMEA talker names
	static char const * const ubx_nmea_talker_names[NMEA_NUM_TALKERS] = { "GP", "GL", "GA", "GB", "GN" };

	// Find number of available bytes
	unsigned int avail = ubx_buf_get_used(buf);
	unsigned int idx = 0;

	*offset = 0;
	*length = 0;

	while (avail >= idx + UBX_OVERHEAD_SIZE) {
		// Check if UBX header
		if (ubx_buf_at(buf, idx) == 0xb5 && ubx_buf_at(buf, idx + 1) == 0x62)
		{
			// Possible ubx header found
			// Extract payload size
			uint16_t payload_size =
				buf->data[(buf->r_ptr + idx + 4) % UBX_BUFFER_SIZE] +
				(((uint16_t)buf->data[(buf->r_ptr + idx + 5) % UBX_BUFFER_SIZE]) << 8);
			// Check if payload size is possible
			if (payload_size > UBX_MAX_PAYLOAD_SIZE) {
				// Payload size is too large
				idx += 2;
				continue;
			}

			// This might be a valid package
			if (avail + UBX_OVERHEAD_SIZE < idx + payload_size) {
				// We don't have enough bytes to process the validity of the packet
				*offset = idx;
				return UBX_PROTO_NONE;
			}

			// Check the validity of the packet
			uint8_t chk_a = 0, chk_b = 0;
			// Calculate checksum
			for (unsigned int i = 0; i < payload_size + 4; ++i) {
				chk_a = chk_a + buf->data[(buf->r_ptr + idx + i + 2) % UBX_BUFFER_SIZE];
				chk_b = chk_b + chk_a;
			}
			// Validate
			if (buf->data[(buf->r_ptr + idx + payload_size + UBX_HEADER_SIZE + 0) % UBX_BUFFER_SIZE] == chk_a &&
				buf->data[(buf->r_ptr + idx + payload_size + UBX_HEADER_SIZE + 1) % UBX_BUFFER_SIZE] == chk_b)
			{
				// Checksum is valid, UBX packet found at offset idx
				*offset = idx;
				*length = payload_size + UBX_OVERHEAD_SIZE;
				return UBX_PROTO_UBX;
			} else {
				idx += 2;
				continue;
			}
		}

		// Check for NMEA header: header char is '$' (0x24)
		if (buf->data[(buf->r_ptr + idx) % UBX_BUFFER_SIZE] == 0x24) {
			char talker[2];
			talker[0] = buf->data[(buf->r_ptr + idx + 1) % UBX_BUFFER_SIZE];
			talker[1] = buf->data[(buf->r_ptr + idx + 2) % UBX_BUFFER_SIZE];
			// Check if valid talker
			int talker_idx = -1;
			for (int i = 0; i < NMEA_NUM_TALKERS; ++i) {
				if (strncmp(talker, ubx_nmea_talker_names[i], 2) == 0) {
					talker_idx = i;
					break;
				}
			}

			// If no valid talker found
			if (talker_idx < 0) {
				idx += 1;
				continue;
			}

			// If talker valid look for termination
			uint8_t chksum_exp = 0;
			// Look for checksum bytes
			unsigned int j = 1;
			while (buf->data[(buf->r_ptr + idx + j) % UBX_BUFFER_SIZE] != '*' && avail > idx + j && j < NMEA_MAX_LENGTH - 4) {
				// Calculate checksum
				chksum_exp ^= buf->data[(buf->r_ptr + idx + j) % UBX_BUFFER_SIZE];
				j++;
			}

			// Don't have enough bytes to process the message
			if (avail <= idx + j) {
				*offset = idx;
				return UBX_PROTO_NONE;
			}

			// Invalid length found
			if (j >= NMEA_MAX_LENGTH - 4) {
				idx += 1;
				continue;
			}

			// Check termination sequence
			if (!isxdigit(ubx_buf_at(buf, idx + j + 1)) || !isxdigit(ubx_buf_at(buf, idx + j + 2))) {
				// Checksum sequence is invalid
				idx += 1;
				continue;
			}
			
			// Validate checkusm
			uint8_t chksum_act = nmea_chksum(ubx_buf_at(buf, idx + j + 1), ubx_buf_at(buf, idx + j + 2));
			if (chksum_exp != chksum_act) {
				idx += 1;
				continue;
			}

			// Check last two bytes, must be CR LF
			if (ubx_buf_at(buf, idx + j + 3) != '\r' || ubx_buf_at(buf, idx + j + 4) != '\n') {
				idx += 1;
				continue;
			}

			// Valid NMEA message
			*offset = idx;
			*length = j + 5;
			return UBX_PROTO_NMEA;
		}

		// Continue with the next byte
		idx += 1;
		continue;
	}
	*offset = idx;
	return UBX_PROTO_NONE;
}

/**
 * @brief Extract UBX message from a ring buffer
 */
void ubx_buf_extract_ubx(ubx_ring_buffer_t * buf, ubx_msg_t * pmsg)
{
	pmsg->sync_char[0] = buf->data[(buf->r_ptr + 0) & UBX_BUFFER_MASK];
	pmsg->sync_char[1] = buf->data[(buf->r_ptr + 1) & UBX_BUFFER_MASK];
	pmsg->msg_class = buf->data[(buf->r_ptr + UBX_HEADER_CLASS_OFFSET) % UBX_BUFFER_SIZE];
	pmsg->msg_id = buf->data[(buf->r_ptr + 3) % UBX_BUFFER_SIZE];
	pmsg->msg_len = buf->data[(buf->r_ptr + 4) % UBX_BUFFER_SIZE] +
		(((uint16_t)buf->data[(buf->r_ptr + 5) % UBX_BUFFER_SIZE]) << 8);
	pmsg->chksum[0] = buf->data[(buf->r_ptr + 6 + pmsg->msg_len) % UBX_BUFFER_SIZE];
	pmsg->chksum[1] = buf->data[(buf->r_ptr + 7 + pmsg->msg_len) % UBX_BUFFER_SIZE];
}

void ubx_buf_extract_nmea(ubx_ring_buffer_t * buf, nmea_msg_t * pmsg, size_t length)
{
	pmsg->sync_char = ubx_buf_at(buf, 0);
	pmsg->talker[0] = ubx_buf_at(buf, 1);
	pmsg->talker[1] = ubx_buf_at(buf, 2);
}

int ubx_buf_print_dbg(ubx_ring_buffer_t *buf)
{
	unsigned int avail = ubx_buf_get_free(buf);
	unsigned int size = UBX_BUFFER_SIZE;
	printf("Bytes available: %u / %u (%4.1f %%)\n", avail, size, (double)avail / (double)size * 100.0);
	return 0;
}

int ubx_buf_dump_dbg(ubx_ring_buffer_t *buf, unsigned int count)
{
	unsigned int avail = ubx_buf_get_free(buf);
	for (unsigned int i = 0; i < min(avail, count); ++i) {
		printf("%02x ", buf->data[(buf->r_ptr + i) % UBX_BUFFER_SIZE]);
		if (i % 16 == 7)
			printf("  ");
		if (i % 16 == 15)
			printf("\n");
	}
	printf("\n");
	return 0;
}
