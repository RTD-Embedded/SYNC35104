/**
    @file

    @brief
        This example program sets the baud rate to the LEA-M8F Ublocks Chip.
		This is required for the usage of any serial port programs to communicate
		with the UBlox chip.

    @verbatim

		This program demonstrates how to change a si5395 register using a single 
		command rather than reprogrammign the entire configuration. It reads and 
		writes to register 0x0121 and enables and disables all frequency outputs
		from the si5395.
		
		This program requries the usage of the si5395_reg library and the FT4222H 
		library. This this program requires super user privledges to execute by 
		default.
		
		Example Usage:
			./sycx5104_toggle_si5395_outputs
		

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

    $Id: syncx5104_toggle_si5395_outputs.c 154532 2026-05-20 15:56:55Z Nsmith $
*/
#include <stdio.h>
#include "si5395_reg.h"

int main(void)
{
	FT_HANDLE fthandle;
	if (syncx5104_open(&fthandle)) {
		printf("Successfully opened ft4222 device\n");
		uint8_t val = 0;
		uint8_t newval = 0xD;	// disable outputs
		//Read Register 0x0121 (Disable all Outputs Register, Anyfreq connector)
		if (!syncx5104_clock_synthesizer_read_register
		    (0x0121, &val, &fthandle)) {
			printf("Couldn't read register!\n");
			syncx5104_close(fthandle);
			return 0;
		}
		printf("SYNC35104 register 0x0121 is 0x%X\n", val);
		if (val == 0xD)	// all outputs are disabled turn on instead
		{
			newval = 0xF;	// turn on outputs
		}
		//Turn off Output all outputs
		if (!syncx5104_clock_synthesizer_write_register
		    (0x0121, newval, &fthandle)) {
			printf("Couldn't write register!\n");
		} else		// Write was successful
		{
			printf("SYNC35104 register 0x0121 is now 0x%X\n",
			       newval);
			if (newval == 0xD) {
				printf("The outputs are Disabled\n");
			} else if (newval == 0xF) {
				printf("The outputs are Enabled\n");
			}
		}

		syncx5104_close(fthandle);
	}
	return 0;
}
