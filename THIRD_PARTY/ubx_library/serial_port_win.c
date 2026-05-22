#include "serial_port_win.h"

#include <stdio.h>

void print_error(const char * context)
{
	DWORD error_code = GetLastError();
	char buffer[256];
	DWORD size = FormatMessageA(
		FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_MAX_WIDTH_MASK,
		NULL, error_code, MAKELANGID(LANG_ENGLISH, SUBLANG_ENGLISH_US),
		buffer, sizeof(buffer), NULL);
	if (size == 0) { buffer[0] = 0; }
	fprintf(stderr, "%s: %s\n", context, buffer);
}

// Opens the specified serial port, configures its timeouts, and sets its
// baud rate.  Returns a handle on success, or INVALID_HANDLE_VALUE on failure.
port_t open_serial_port(const char * device, uint32_t baud_rate)
{
	HANDLE port = CreateFileA(device, GENERIC_READ | GENERIC_WRITE, 0, NULL,
		OPEN_EXISTING, /*FILE_ATTRIBUTE_NORMAL*/ 0, NULL);
	if (port == INVALID_HANDLE_VALUE)
	{
		print_error(device);
		return INVALID_HANDLE_VALUE;
	}

	// Flush away any bytes previously read or written.
	BOOL success = FlushFileBuffers(port);
	if (!success)
	{
		print_error("Failed to flush serial port");
		CloseHandle(port);
		return INVALID_HANDLE_VALUE;
	}

	// Configure read and write operations to time out after 100 ms.
	COMMTIMEOUTS timeouts = { 0 };
	timeouts.ReadIntervalTimeout = 1;
	timeouts.ReadTotalTimeoutConstant = 100;
	timeouts.ReadTotalTimeoutMultiplier = 1;
	timeouts.WriteTotalTimeoutConstant = 100;
	timeouts.WriteTotalTimeoutMultiplier = 1;

	success = SetCommTimeouts(port, &timeouts);
	if (!success)
	{
		print_error("Failed to set serial timeouts");
		CloseHandle(port);
		return INVALID_HANDLE_VALUE;
	}

	success = set_serial_port_rate(port, baud_rate);

	if (!success) {
		print_error("Failed to set serial port baud rate");
		CloseHandle(port);
		return INVALID_HANDLE_VALUE;
	}

	return port;
}

int set_serial_port_rate(port_t port, int baud_rate)
{
	BOOL success;
	DCB state;
	state.DCBlength = sizeof(DCB);

	PurgeComm(port, 0xFF);

	success = GetCommState(port, &state);
	if (!success)
	{
		print_error("Failed to get serial settings");
		CloseHandle(port);
		return INVALID_HANDLE_VALUE;
	}

	state.BaudRate = baud_rate;
	state.ByteSize = 8;         // Setting ByteSize = 8
	state.StopBits = ONESTOPBIT;// Setting StopBits = 1
	state.Parity   = PARITY_NONE;  // Setting Parity = None
	
	success = SetCommState(port, &state);
	if (!success)
	{
		print_error("Failed to set serial settings");
		CloseHandle(port);
		return INVALID_HANDLE_VALUE;
	}
	
	COMSTAT stat;
	DWORD error;

	ClearCommBreak(port);
	ClearCommError(port, &error, &stat);

	return success;
}

void close_serial_port(port_t port)
{
	CloseHandle(port);
}

size_t write_port(port_t port, uint8_t * buffer, size_t size)
{
	DWORD written;
	BOOL success = WriteFile(port, buffer, size, &written, NULL);
	if (!success) {
		return -1;
	}
	return written;
}

// Reads bytes from the serial port.
// Returns after all the desired bytes have been read, or if there is a
// timeout or other error.
// Returns the number of bytes successfully read into the buffer, or -1 if
// there was an error reading.
size_t read_port(port_t port, uint8_t * buffer, size_t size)
{
	DWORD received;
	BOOL success = ReadFile(port, buffer, size, &received, NULL);
	if (!success)
	{
		print_error("Failed to read from port");
		return -1;
	}
	return received;
}