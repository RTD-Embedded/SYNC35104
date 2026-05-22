
#include <stdint.h>
#include <string.h>
#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>

#include "ftd2xx.h"
#include "libft4222.h"

#include "si5395_reg.h"
//#define DEBUG
uint8_t si5395_address2page(uint8_t address)
{
	return ((address >> 8) & 0xFF);
}

uint8_t si5395_address2offs(uint8_t address)
{
	return (address & 0xFF);
}

void print_buffer(uint8_t *buf, size_t len)
{
	for (size_t i = 0; i < len; ++i) {
		printf("%02x ", buf[i]);
		if (i % 16 == 15)
			printf("\n");
	}
	printf("\n");
}

FT_DEVICE_LIST_INFO_NODE *ListFtUsbDevices()
{
	FT_STATUS ftStatus = 0;

	DWORD numOfDevices = 0;
	ftStatus = FT_CreateDeviceInfoList(&numOfDevices);

	//malloc(0) is implementation defined, so we return null here...
	if (numOfDevices == 0) {
		printf("Num of devices is Zero\n");
		return NULL;
	}
	FT_DEVICE_LIST_INFO_NODE *list =
	    (FT_DEVICE_LIST_INFO_NODE *) malloc(sizeof(FT_DEVICE_LIST_INFO_NODE)
						* numOfDevices);

	for (DWORD iDev = 0; iDev < numOfDevices; ++iDev) {
		FT_DEVICE_LIST_INFO_NODE devInfo;
		memset(&devInfo, 0, sizeof(devInfo));

		ftStatus =
		    FT_GetDeviceInfoDetail(iDev, &devInfo.Flags, &devInfo.Type,
					   &devInfo.ID, &devInfo.LocId,
					   devInfo.SerialNumber,
					   devInfo.Description,
					   &devInfo.ftHandle);

		if (FT_OK == ftStatus) {
#ifdef DEBUG
			printf("Dev %d:\n", iDev);
			printf("  Type= 0x%x\n", devInfo.Type);
			printf("  Vendor ID= 0x%04x\n",
			       (devInfo.ID & 0xFFFF0000) >> 16);
			printf("  Device ID= 0x%04x\n",
			       (devInfo.ID & 0x0000FFFF) >> 0);
			printf("  LocId= 0x%x\n", devInfo.LocId);
			printf("  SerialNumber= %s\n", devInfo.SerialNumber);
			printf("  Description= %s\n", devInfo.Description);
			printf("  ftHandle= 0x%p\n", devInfo.ftHandle);
#endif

			if (strcmp(devInfo.Description, "FT4222") == 0
			    || strcmp(devInfo.Description, "FT4222 A") == 0) {
				list[0] = devInfo;	// storing as the first element in the list, we assume this in syncx5104_open(...)
				// TODO: check against other FT4222 A devices in the stack to make sure that we have the right one.
			}
		}
	}
	return list;
}

int syncx5104_open(FT_HANDLE *handle)
{
	FT_DEVICE_LIST_INFO_NODE *list = ListFtUsbDevices();
	if (list == NULL) {
		printf("No FT4222 device found!\n");
		return 0;
	}

	FT_HANDLE fthandle = NULL;
	FT_STATUS ftstatus;

	ftstatus =
	    FT_OpenEx((PVOID) list[0].LocId, FT_OPEN_BY_LOCATION, &fthandle);
	if (ftstatus != FT_OK) {
		printf("Opening the FT4222 device failed!\n");
		goto ERR;

	}

	ftstatus =
	    FT4222_SPIMaster_Init(fthandle, SPI_IO_SINGLE, CLK_DIV_256,
				  CLK_IDLE_LOW, CLK_LEADING, 0x01);
	if (ftstatus != FT_OK) {
		printf("Init FT4222 as SPI master device failed!\n");
		goto ERR;
	}

	*handle = fthandle;
	//At this point we should be connected

	return 1;
ERR:
	free(list);
	return 0;
}

int syncx5104_close(FT_HANDLE *handle)
{
	FT4222_UnInitialize(*handle);
	FT_Close(*handle);
	return 1;
}

int syncx5104_clock_synthesizer_write_register(int reg, int value,
					       FT_HANDLE *fthandle)
{
	uint8_t reg_offset;
	uint8_t page;

	page = si5395_address2page(reg);
	reg_offset = si5395_address2offs(reg);
	FT_STATUS ftstatus;
	uint16_t transferred = 0;

	uint8_t cmd[8] =
	    { SI5395_SPI_COMMAND_SET_ADDR, SI5395_ADDR_PAGE_REGISTER,
		SI5395_SPI_COMMAND_WRITE_INC, page, SI5395_SPI_COMMAND_SET_ADDR,
		reg_offset, SI5395_SPI_COMMAND_WRITE_INC, value
	};
	for (int i = 0; i < 8; i += 2) {
		ftstatus =
		    FT4222_SPIMaster_SingleWrite(*fthandle, cmd + i, 2,
						 &transferred, true);
		if (ftstatus != FT_OK)
			return 0;
	}
	return 1;
}

int syncx5104_clock_synthesizer_read_register(int reg, uint8_t *output,
					      FT_HANDLE *handle)
{
	//uint8_t val = 0;
	uint16_t read;
	uint8_t page = si5395_address2page(reg);
	uint8_t offset = si5395_address2offs(reg);

	uint8_t cmd[] =
	    { SI5395_SPI_COMMAND_SET_ADDR, SI5395_ADDR_PAGE_REGISTER,
		SI5395_SPI_COMMAND_WRITE_DATA, page,
		SI5395_SPI_COMMAND_SET_ADDR, offset,
		SI5395_SPI_COMMAND_READ_DATA, 0xFF
	};

	uint8_t commands_rb[sizeof(cmd) / sizeof(uint8_t)];
	memset(commands_rb, 0xFF, sizeof(cmd));
	uint8_t num_data = sizeof(cmd) / sizeof(uint8_t);

	for (int i = 0; i < num_data; i += 2) {
		FT_STATUS ftstatus =
		    FT4222_SPIMaster_SingleReadWrite(*handle, commands_rb + i,
						     cmd + i, 2, &read, true);
		if (ftstatus != FT_OK) {
			printf("Failed to write/read byte %d\n", i);
			return 0;
		}
		//printf("\r%4zd / %4zd ready, status: %d\n", i + 2, num_data, ftstatus);
	}
	*output = commands_rb[num_data - 1];
	return 1;
}
