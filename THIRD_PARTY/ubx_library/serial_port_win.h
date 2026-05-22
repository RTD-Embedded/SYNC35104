#ifndef SERIAL_PORT_WIN_H_INCLUDED
#define SERIAL_PORT_WIN_H_INCLUDED

#include <windows.h>

#include <stdint.h>

typedef HANDLE port_t;

void print_error(const char * context);

port_t open_serial_port(const char * device, uint32_t baud_rate);

/**
 * @brief Close serial port
 * @param[in] port Port to close
 */
void close_serial_port(port_t port);

int set_serial_port_rate(port_t port, int baud_rate);

size_t read_port(port_t port, uint8_t * buffer, size_t size);

size_t write_port(port_t port, uint8_t * buffer, size_t size);

#endif /* SERIAL_PORT_WIN_H_INCLUDED */
