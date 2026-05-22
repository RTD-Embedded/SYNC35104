/**
 * @file serial_port.h
 * @brief Platform independent interface for serial port
 */

#ifndef SERIAL_PORT_H_INCLUDED
#define SERIAL_PORT_H_INCLUDED

#if defined _WIN32 || defined _WIN64
#include "serial_port_win.h"
#elif defined __unix
#include "serial_port_unix.h"
#else
#error "Either windows or unix must be defined!"
#endif

#endif /* SERIAL_PORT_H_INCLUDED */
