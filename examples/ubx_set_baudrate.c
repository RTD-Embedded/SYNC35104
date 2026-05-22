/**
    @file

    @brief
        This example program sets the baud rate to the LEA-M8F Ublocks Chip.
		This is required for the usage of any serial port programs to communicate
		with the UBlox chip.

    @verbatim

		This program demonstrates how to change the baud rate of the Ublox chip 
		using C code. It uses the Third party Ublox libraries to accomplish this.
		This this program requires super user privledges to execute by default.
		
		Example Usage:
			./ubx_set_baudrate [serial port] [current baud rate] [new baud rate]
			./ubx_set_baudrate --port /dev/ttyUSB0 --baud_rate 9600 --target_baud 115200
		

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

    $Id: ubx_set_baudrate.c 154532 2026-05-20 15:56:55Z Nsmith $
*/

#include "serial_port.h"
#include "ubxbuffer.h"
#include "ubxproto.h"
#include "ubxutil.h"

#include <stdlib.h>
#include <getopt.h>
#include <string.h>

int main(int argc, char *argv[])
{
	int baud_rate = 0;
	int target_baud_rate = 0;
	char port_name[100] = { 0 };
	int c;
	while (1) {

		static struct option long_options[] = {
			{ "port", required_argument, 0, 'p' },
			{ "baud_rate", required_argument, 0, 'b' },
			{ "target_baud", required_argument, 0, 't' },
			{ "help", no_argument, 0, 'h' },
			{ 0, 0, 0, 0 }
		};
		int options_index = 0;

		c = getopt_long(argc, argv, "pbth", long_options,
				&options_index);

		if (c == -1)
			break;

		switch (c) {
		case 'p':
			{
				strcpy(port_name, optarg);
				break;
			}
		case 'b':
			{
				baud_rate = strtol(optarg, NULL, 10);
				break;
			}
		case 't':
			{
				target_baud_rate = strtol(optarg, NULL, 10);
				break;
			}
		case 'h':
			{
				printf("\n%s\n", argv[0]);
				printf
				    ("\nUsage:%s [--help] [--port /dev/ttyUSB0] [--baud_rate 115200]\n",
				     argv[0]);
				printf("\t\t\t\t [--target_baud 115200]\n");
				printf
				    ("\n\t --help:\t Display usage information and exit.\n");
				printf
				    ("\n\t --port: \t Set the specified serial port as PORT.\n");
				printf
				    ("\t      PORT:\t\t Port number (/dev/ttyX).\n");
				printf
				    ("\t --baud_rate: \t Set the specified baud rate.\n");
				printf
				    ("\t      BAUD:\t\t Baud rate (9600<=x<=115200).\n");
				printf
				    ("\t --target_baud: \t Set the specified target baud rate.\n");
				printf
				    ("\t      BAUD:\t\t Baud rate (9600<=x<=115200).\n");
				printf("Help messages:\n");
				exit(1);
				break;
			}
		default:
			{
				exit(1);
				break;
			}
		}
	}

	if (baud_rate == 0)
		baud_rate = 115200;
	if (target_baud_rate == 0)
		target_baud_rate = 115200;
	if (port_name[0] == 0)
		strcpy(port_name, "/dev/ttyUSB0");
	// Auto-detect the baud rate
	port_t port = open_serial_port(port_name, baud_rate);
	printf("Opening serial port: %s ... ", port_name);
	if (port < 0) {
		printf("Could not open serial port\n");
		return 1;
	}
	printf("Success.\n");
	printf("Using baud rate: %d\n", baud_rate);

	printf("Changing baud rate to %d\n", target_baud_rate);
	return change_ubx_baudrate(port, target_baud_rate);
}
