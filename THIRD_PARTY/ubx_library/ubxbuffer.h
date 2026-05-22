#ifndef UBXBUFFER_H_INCLUDED
#define UBXBUFFER_H_INCLUDED

#include "serial_port.h"
#include "ubxproto.h"

#include <stdio.h>

/**
 * @brief Size of the UBX ring buffer
 * @note Must be 2^N
 */
#define UBX_BUFFER_SIZE (1 << 14)

/**
 * @brief UBX ring buffer size mask
 */
#define UBX_BUFFER_MASK (UBX_BUFFER_SIZE-1)

/**
 * @brief Maximum payload size we can receive
 */
#define UBX_MAX_PAYLOAD_SIZE (UBX_BUFFER_SIZE - 1 - UBX_OVERHEAD_SIZE)

/**
 * @brief Ring buffer for receiving U-Blox messages
 * @details
 * Lock-free ring buffer implementation
 */
typedef struct {
	/** @brief Read pointer */
	unsigned int r_ptr;
	/** @brief Write pointer */
	unsigned int w_ptr;
	/** @brief Data */
	uint8_t data[UBX_BUFFER_SIZE];
} ubx_ring_buffer_t;

#ifdef __cplusplus
extern "C" {
#endif 

/**
 * @brief Initialize U-Blox ring buffer
 */
int ubx_buf_init(ubx_ring_buffer_t *buf);

static inline uint8_t ubx_buf_at(ubx_ring_buffer_t *buf, unsigned int idx)
{
	return buf->data[(buf->r_ptr + idx) & UBX_BUFFER_MASK];
}

/**
 * @brief Get number of free bytes in U-Blox ring buffer
 * @param[in] buf Pointer to U-blox ring buffer
 * @return Number of free bytes
 */
static inline unsigned int ubx_buf_get_free(ubx_ring_buffer_t *buf)
{
	return UBX_BUFFER_SIZE - 1 - (buf->w_ptr - buf->r_ptr) & UBX_BUFFER_MASK;
}

/**
 * @brief Get number of used bytes in U-Blox ring buffer
 * @param[in] buf Pointer to U-blox ring buffer
 * @return Number of used bytes
 */
static inline unsigned int ubx_buf_get_used(ubx_ring_buffer_t *buf)
{
	return (buf->w_ptr - buf->r_ptr) & UBX_BUFFER_MASK;
}

/**
 * @brief Consume bytes from UBX ring buffer
 * @param [in] buf UBX ring buffer
 * @param [in] n Number of bytes to consume
 * @return Number of bytes consumed
 */
unsigned int ubx_buf_consume(ubx_ring_buffer_t *buf, unsigned int n);

/**
 * @brief Copy from the U-Blox ring buffer
 * @param[in] buf Pointer to U-Blox ring buffer
 * @param[in] dest Destination address
 * @param[in] size Maximum number of bytes to copy
 * @return Number of bytes copied
 */
size_t ubx_buf_copy(ubx_ring_buffer_t *buf, uint8_t *dest, size_t size);


/**
 * @brief Read from file into U-blox ring buffer
 * @param[in] fp File pointer
 * @param[in] buf Pointer to ring buffer
 * @return Number of bytes read
 */
int ubx_buf_read_file(FILE * fp, ubx_ring_buffer_t *buf, size_t n);

/**
 * @brief Read from serial port into U-Blox ring buffer
 * @param[in] port Port to read from
 * @param[in] buf Pointer to U-Blox ring buffer
 * @return Number of bytes read
 */
size_t ubx_buf_read_port(port_t port, ubx_ring_buffer_t *buf);

/**
 * @brief Find packet frame in u-blox ring buffer
 * @param[in] buf Pointer to ring buffer
 * @param[out] offset Byte offset to start of found frame
 * @param[out] length Length of the message (full length), zero if none found
 * @return Protocol type found (NONE / UBX / NMEA)

 * @details
 * Processes the available bytes in the ring buffer until a valid message
 * is found. The message can be of type UBX and NMEA.
 * This routine does not consume bytes from the buffer.
 */
ubx_protocol_type_t ubx_buf_find_packet(ubx_ring_buffer_t *buf, unsigned int *offset, unsigned int *length);

/**
 * @brief Extract UBX message from a ring buffer
 * @param[in] buf Pointer to U-Blox ring buffer
 * @param[out] pmsg Pointer to UBX message
 */
void ubx_buf_extract_ubx(ubx_ring_buffer_t * buf, ubx_msg_t * pmsg);

/**
 * @brief Extract NMEA message from U-Blox ring buffer
 * @param[in] buf Pointer to U-Blox ring buffer
 * @param[out] pmsg Pointer to NMEA message
 */
void ubx_buf_extract_nmea(ubx_ring_buffer_t * buf, nmea_msg_t * pmsg, size_t length);

// Debug functions

/**
 * @brief Print debug info of the ring buffer
 */
int ubx_buf_print_dbg(ubx_ring_buffer_t *buf);

/**
 * @brief Dump data from a buffer
 */
int ubx_buf_dump_dbg(ubx_ring_buffer_t *buf, unsigned int count);


#ifdef __cplusplus
}
#endif 

#endif /* UBXBUFFER_H_INCLUDED */
