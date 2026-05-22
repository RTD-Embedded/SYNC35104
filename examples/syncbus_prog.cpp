/**
    @file

    @brief
        Set the Enable pins on the SYNCBUS

    @verbatim

		This program demonstrates the setting of the SYNCBUS ENABLE lines. Each bit 
		represents an individual Enable line.
		
		This program requries the usage of the FT4222H library. This this program 
		requires super user privledges to execute by default.
		
		Example Usage:
			./syncbus_prog [enable pin out mask]
			./syncbus_prog 0xF
		

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

    $Id: syncbus_prog.cpp 154542 2026-05-20 18:26:06Z Nsmith $
*/  
     
#if defined _WIN32 || defined _WIN64
#include <windows.h>
#endif /*  */
    
#if defined __unix__
#include <unistd.h>
int Sleep(int msec)  { return usleep(1000 * msec);
}

 
#endif /*  */
    
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <vector>
#include <string>
    
//------------------------------------------------------------------------------
// include FTDI libraries
//
#include "ftd2xx.h"
#include "libft4222.h"
    std::vector < FT_DEVICE_LIST_INFO_NODE > g_FT4222DevList;
 
//------------------------------------------------------------------------------
    inline std::string DeviceFlagToString(DWORD flags)  { std::string msg;
	msg += (flags & 0x1) ? "DEVICE_OPEN" : "DEVICE_CLOSED";
	msg += ", ";
	msg += (flags & 0x2) ? "High-speed USB" : "Full-speed USB";
	return msg;
}
 void ListFtUsbDevices()  { FT_STATUS ftStatus = 0;
	 DWORD numOfDevices = 0;
	ftStatus = FT_CreateDeviceInfoList(&numOfDevices);
	 for (DWORD iDev = 0; iDev < numOfDevices; ++iDev)
		 {
		FT_DEVICE_LIST_INFO_NODE devInfo;
		memset(&devInfo, 0, sizeof(devInfo));
		 ftStatus =
		    FT_GetDeviceInfoDetail(iDev, &devInfo.Flags, &devInfo.Type,
					   &devInfo.ID, &devInfo.LocId,
					   devInfo.SerialNumber,
					   devInfo.Description,
					   &devInfo.ftHandle);
		 if (FT_OK == ftStatus)
			 {
			printf("Dev %d:\n", iDev);
			printf("  Flags= 0x%x, (%s)\n", devInfo.Flags,
				DeviceFlagToString(devInfo.Flags).c_str());
			printf("  Type= 0x%x\n", devInfo.Type);
			printf("  ID= 0x%x\n", devInfo.ID);
			printf("  LocId= 0x%x\n", devInfo.LocId);
			printf("  SerialNumber= %s\n", devInfo.SerialNumber);
			printf("  Description= %s\n", devInfo.Description);
			printf("  ftHandle= 0x%p\n", devInfo.ftHandle);
			 const std::string desc = devInfo.Description;
			if (desc == "FT4222" || desc == "FT4222 B")
				 {
				g_FT4222DevList.push_back(devInfo);
				}
			}
		}
}

 
//------------------------------------------------------------------------------
// main
//------------------------------------------------------------------------------
int main(int argc, char const *argv[])  { int gpo = 0;
	if (argc > 1) {
		gpo = strtoul(argv[1], NULL, 0) & 0x0f;
	}
	printf("Setting Syncbus enable to 0x%x\n", gpo);
	 ListFtUsbDevices();
	 if (g_FT4222DevList.empty()) {
		printf("No FT4222 device is found!\n");
		return 0;
	}
	 FT_HANDLE ftHandle = NULL;
	 FT_STATUS ftStatus;
	ftStatus =
	    FT_OpenEx((PVOID) g_FT4222DevList[0].LocId, FT_OPEN_BY_LOCATION,
		      &ftHandle);
	if (FT_OK != ftStatus) {
		printf("Open a FT4222 device failed!\n");
		return 0;
	}
	 
	    // Set all pins to output
	GPIO_Dir my_gpio_dirs[4] =
	    { GPIO_OUTPUT, GPIO_OUTPUT, GPIO_OUTPUT, GPIO_OUTPUT };
	 
	    // Initialize GPIO
	    printf("Initializing GPIO\n");
	ftStatus = FT4222_GPIO_Init(ftHandle, my_gpio_dirs);
	if (FT_OK != ftStatus) {
		printf("Failed to initialize GPIO\n");
		return -1;
	}
	 
	    // Enable GPIO2
	    ftStatus = FT4222_SetSuspendOut(ftHandle, false);
	if (FT_OK != ftStatus) {
		printf("Failed to disable suspend out\n");
		return -1;
	}
	 
	    // Enable GPIO3
	    ftStatus = FT4222_SetWakeUpInterrupt(ftHandle, false);
	if (FT_OK != ftStatus) {
		printf("Failed to disable wakeup interrupt\n");
		return -1;
	}
	 
	    // Set the Enable pins
	    printf("Setting Syncbus Enable pins ...\n");
	for (int j = 0; j < 4; ++j) {
		ftStatus =
		    FT4222_GPIO_Write(ftHandle, (GPIO_Port) j, gpo & (1 << j));
		if (FT_OK != ftStatus) {
			printf("Failed to write to GPIO pin %d\n", j);
			return -1;
		}
	}
	  printf("UnInitialize FT4222\n");
	FT4222_UnInitialize(ftHandle);
	 printf("Close FT device\n");
	FT_Close(ftHandle);
	return 0;
}


