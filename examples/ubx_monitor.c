/**
    @file

    @brief
        This example program views NMEA data and more streamed from the 
		LEA-M8F Ublox chip. 

    @verbatim

		This program requries that the UBX chip is configured with a UCenter
		configuration prior and a matching baud rate. Use both ubx_config and
		ubx_set_baudrate prior to this program. This program demonstrates the 
		use of the UBX third party library function calls. This program requires
		super user privledges to execute by default.
		
		Example Usage:
			./ubx_monitor [serial port] [baud rate]
			./ubx_monitor /dev/ttyUSB0 115200
		

    @endverbatim

    @verbatim
    --------------------------------------------------------------------------
    This file and its contents are copyright (C) RTD Embedded Technologies,
    Inc.  All Rights Reserved.

    This software is licensed as described in the RTD End-User Software License
    Agreement.  For a copy of this agreement, refer to the file LICENSE.TXT
    (which should be included with this software) or contact RTD Embedded
    Technologies, Inc.
    --------------------------------------------------------------------------
    @endverbatim

    $Id: ubx_monitor.c 154532 2026-05-20 15:56:55Z Nsmith $
*/  
    
#include <inttypes.h>
#define _USE_MATH_DEFINES
#include <math.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
    
#include "serial_port.h"
#include "ubxbuffer.h"
#include "ubxproto.h"
#include "ubxutil.h"
int main(int argc, char *argv[])  { ubx_protocol_type_t proto;
	ubx_ring_buffer_t ubx_buffer;
	 ubx_buf_init(&ubx_buffer);
	 ubx_msg_t ubx_msg;
	 int baud_rate = 0;
	char *endptr;
	 if (argc > 2)
		baud_rate = strtol(argv[2], &endptr, 10);
	 
	    // Auto-detect the baud rate
	    port_t port = open_serial_port(argv[1], baud_rate);
	printf("Opening serial port: %s ... ", argv[1]);
	if (port < 0) {
		printf("Could not open serial port\n");
		return 1;
	}
	printf("Success.\n");
	 if (baud_rate == 0) {
		printf("Detecting baud rate for port: %s\n", argv[1]);
		baud_rate = detect_ubx_baud_rate(port);
		if (baud_rate < 0) {
			fprintf(stderr, "Could not detect baud rate\n");
			return 1;
		} else {
			printf("Detected baud rate: %d\n", baud_rate);
		}
	} else {
		printf("Using baud rate: %d\n", baud_rate);
	}
	 uint8_t recv_buf[4096];
	 int total = 0;
	for (;;) {
		int received = ubx_buf_read_port(port, &ubx_buffer);
		 total += received;
		
		    //printf("Read %d bytes, total: %d, cycle: %d\n", received, total, i);
		    //ubx_buf_dump_dbg(&ubx_buffer, received);
		    
		    // Look for packet
		unsigned int offset;
		unsigned int length;
		
		do {
			proto =
			    ubx_buf_find_packet(&ubx_buffer, &offset, &length);
			
			    //printf("Consuming %u bytes\n", offset);
			    ubx_buf_consume(&ubx_buffer, offset);
			switch (proto) {
			case UBX_PROTO_UBX:
				printf("UBX  / ");
				 ubx_buf_extract_ubx(&ubx_buffer, &ubx_msg);
				ubx_msg_print_dbg(stdout, &ubx_msg);
				 ubx_buf_consume(&ubx_buffer, length);
				break;
			case UBX_PROTO_NMEA:
				printf("NMEA / ");
				 ubx_buf_copy(&ubx_buffer, recv_buf, length);
				fwrite(recv_buf, 1, length - 2, stdout);
				printf("\n");
				 ubx_buf_consume(&ubx_buffer, length);
				break;
			case UBX_PROTO_NONE:
			default:
				break;
			}
		} while (proto != UBX_PROTO_NONE);
	}
	 printf("Closing serial port ... ");
	close_serial_port(port);
	printf("Success.\n");
	 return 0;
}


