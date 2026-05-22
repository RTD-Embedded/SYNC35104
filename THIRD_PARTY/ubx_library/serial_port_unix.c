#include "serial_port_unix.h"

#include <errno.h>
#include <fcntl.h> 
#include <stdio.h>
#include <string.h>
#include <termios.h>
#include <unistd.h>

#define error_message printf

int set_serial_port_rate(port_t port, int speed)
{
        speed_t ttyspd = B9600;
        switch (speed)
        {
        case 9600:
                ttyspd = B9600;
                break;

        case 19200:
                ttyspd = B19200;
                break;

        case 38400:
                ttyspd = B38400;
                break;

        case 57600:
                ttyspd = B57600;
                break;

        case 115200:
                ttyspd = B115200;
                break;

        case 230400:
                ttyspd = B230400;
                break;

        case 460800:
                ttyspd = B460800;
                break;

        default:
                ttyspd = B9600;
                break;
        }

	int fd  = port;
        struct termios tty;
        if (tcgetattr (fd, &tty) != 0)
        {
                error_message ("error %d from tcgetattr", errno);
                return -1;
        }

        cfsetospeed (&tty, ttyspd);
        cfsetispeed (&tty, ttyspd);
        cfmakeraw(&tty);

        tty.c_cflag &= ~(CSTOPB);
        tty.c_cflag &= ~(CSIZE);
        tty.c_cflag |= CS8; // 8bit
        tty.c_cflag |= CLOCAL; // ignore modem controls,
        tty.c_cflag |= CREAD; // enable reading
        tty.c_cflag &= ~(PARENB); // shut off parity
        tty.c_cflag &= ~(CRTSCTS); // disable flow control
		
        if (tcsetattr (fd, TCSANOW, &tty) != 0)
        {
                error_message ("error %d from tcsetattr", errno);
                return -1;
        }
        return 0;
}

void set_blocking (int fd, int should_block)
{
        struct termios tty;
        if (tcgetattr (fd, &tty) != 0)
        {
                error_message ("error %d from tggetattr", errno);
                return;
        }

        tty.c_cc[VMIN]  = should_block ? 1 : 0;
        tty.c_cc[VTIME] = 1; // 0.1 seconds read timeout

        if (tcsetattr (fd, TCSANOW, &tty) != 0)
                error_message ("error %d setting term attributes", errno);
}


port_t open_serial_port(char const * device, uint32_t baud_rate)
{
	port_t fd = open(device, O_RDWR | O_NOCTTY | O_SYNC);
	if (fd < 0) {
        error_message ("error %d opening %s: %s", errno, device, strerror (errno));
        return fd;
	}

	set_serial_port_rate(fd, baud_rate);
	set_blocking (fd, 0);
	/* Clear stale messages */
	tcflush(fd, TCIOFLUSH);
	return fd;
}

size_t read_port(port_t port, uint8_t * buffer, size_t size)
{
	return read(port, buffer, size);
}

size_t write_port(port_t port, uint8_t * buffer, size_t size)
{
	return write(port, buffer, size);
}

void close_serial_port(port_t port)
{
	close(port);
}
