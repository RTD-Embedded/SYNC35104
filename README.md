# Linux Software (SYNC35104)

> SWP-700010183 rev A
>
> Version v01.00.00.154623

Copyright (C), RTD Embedded Technologies, Inc.  All Rights Reserved.

This software package is dual-licensed.  Source code that is compiled for
kernel mode execution is licensed under the GNU General Public License
version 2. For a copy of this license, refer to the file
LICENSE_GPLv2.TXT (which should be included with this software) or contact
the Free Software Foundation. Source code that is compiled for user mode
execution is licensed under the RTD End-User Software License Agreement.
For a copy of this license, refer to LICENSE.TXT or contact RTD Embedded
Technologies, Inc. Using this software indicates agreement with the
license terms listed above.

## Table of Contents

- [About This Software](#about-this-software)
- [Supported Hardware](#supported-hardware)
- [Support Kernel Versions](#supported-kernel-versions)
- [Supported CPU Architecture](#supported-cpu-architecture)
- [Required Libraries](#required-libraries)
- [Library Interface](#library-interface)
- [Required Configuration Files](#required-configuration-files)
- [Example Configuration Files and Usage](#example-configuration-files-and-usage)
- [Using the GPSd and Ntpd Configuration Files](#using-the-gpsd-and-ntpd-configuration-files)
- [Supported Compilers](#supported-compilers)
- [Header Files](#header-files)
- [Example Programs](#example-programs)
- [Known Limitations](#known-limitations)
- [Redistributables](#redistributables)
- [Getting Technical Support](#getting-technical-support)

## About This Software

This software package contains [libraries](#required-libraries) and [example programs](#example-programs). 
There are also [redistributables](#redistributables) provided under the 'Release' section of this repository. 
Please note, the provided libraries and example programs are only compatible with Linux operating systems. 
However, many of the redistributables are only compatible with Windows.

The library and examples include full source code. The library source code can be 
used as a starting point for your own applications.

## Supported Hardware

This software supports the following RTD boards:
- [SYNC25104/SYNC35104](https://www.rtd.com/PC104/UM/GPS/SYNC35104.htm)

The names SYNC25104 and SYNC35104 will be referred to as SYNCx5104 in the documentation and source code.

## Supported Kernel Versions

This software has been tested with the following Linux distributions and kernel
versions:

* Ubuntu 20.04 LTS (5.11.0 Kernel Version)
* Ubuntu 20.04 LTS (5.13.0 Kernel Version)

Note that due to kernel API differences between kernel versions, RTD cannot
guarantee compatibility with kernels and distributions not listed above.  It may
be necessary to modify the kernel code and/or Makefiles for your specific Linux
environment.

## Supported CPU Architecture

This software has been validated on the following CPU architectures.
 
* x86_64 (64-bit) multi-core

## Required Libraries

The SYNCx5104 board makes use of Third party libraries. 

The libft4222h library from Future Technology Devices International Ltd. (FTDI) 
is used to send serial commands through the FT4222H chip to the Si5395 Clock 
Synthesizer, the syncbus IO and the Ublox LEA-M8F GPS Diciplined Oscillator. 
Simply extract and follow the readme provided within to use. On Ubuntu 
the default location for installation is:

`/usr/local/lib`

This location may be different on other distrubutions of linux and may require
changes to the provided Makefiles in order to make use of the libft222h. This is
a dynamically linked library, so updates that replace this may alter the behaviour
of COMPILED programs. The validated and provided version for this library 
is 1.4.4.44.

The ubx_library redistributed in this software package is used for sending commands 
to the Ublox LEA-M8F. This is a static library and is required to be compiled before 
RTD's `./lib` and `./examples`. To build the library, issue the command `make` within 
`THIRD_PARTY/ubx_library` and verify the file outputs.

## Library Interface

The directory `lib/` contains RTD source code related to the user library.


The si5395_regs library is statically linked library.


The library contains functions that can be accessed by a user program 
to access the registers on the SYNCx5104 card. The library must be 
built before compiling the example programs or any application that 
uses it.

To build the library, issue the command `make` within `lib/`.

## Required Configuration Files

The SYNCx5104 requires the usage of configuration files for operation. The'default' 
configuration of our board is provided under `examples/cfg/`.

The configurations are stored in Non-volatile memory, meaning that after a power-loss
event, the configuration to the FT4222, the LEA-M8F, and the Si5395 will need to be 
re-uploaded. In a linux distrubition, this requires the usage of our example programs.

The configuration for the Ublox LEA-M8F was generated using UCenter a third party 
program that may be required for your use. This program is redistribued under the release
package on our GitHub.

The configuration of the Si5395 Clock Synthesizer is generated through a software called
ClockBuilderPro. This program is redistribued under the release package on our GitHub.

> Important:\
> UCenter and ClockBuilderPro do not run natively on Linux and will require 
> a Windows operating system to generate custom configuration files at this time.

For more information on how these configuration files are generated please consult the
respective manufactuerer's documentation.

## Example Configuration Files and Usage

Under the `examples/cfg/` directory are the default configurations for the SYNCx5104
chipset. 

### [gpsconfig_115200.txt](examples/cfg/gps_config_115200.txt)
Default configuration for the Ublox LEA-M8F chip used by ubx_config.c

### [gpsd](examples/cfg/gpsd)
Configuration for the GPSd daemon. Only required for setting up NTP servers
see usage below.

### [gpsdo-m8f-aocjy-10.00-ubx-nmea-115200.txt](examples/cfg/gpsdo-m8f-aocjy-10.00-ubx-nmea-115200.txt)
Ublox configuration when using GPSd/NTPd.

### [ntp.conf](examples/cfg/ntp.conf)
A Default NTPd configuration file. See usage below. Only required for
usage when setting up NTP servers see usage below.

### [Si5395-RevA-S35104-Registers.txt](examples/cfg/Si5395-RevA-S35104-Registers.txt)
Default configuration for the Si5395 Clock Synthesizer Chip.

## Using the GPSd and Ntpd Configuration Files

The ntpd program is an operating-system daemon that sets and maintains a computer system's system time in 
synchronization with Internet-standard time servers. These are the minimum required, but not the only way to configure
GPSd/NTPd. You may need to adjust the contents of these configuration files to suit your usage case.

1. Copy the configration files from examples/cfg to their respective locations overriding any original files:\
The gpsd config file should be copied to `/etc/default/gpsd`\
The ntp.conf file should be copied to `/etc/ntp.conf`
	
2. Launch the gpsd server:  
```sh
sudo systemctl start gpsd
```

3. Restart the ntp server:  
```sh
sudo systemctl restart ntp
```
	
Now you can issue the `ntpq -p` command and see that the ntp is synchronized using the 1 pps signal.
Note: For the ntp server to synchronize you first have to set the time using the date command 
(Set the date with at least a minute accuracy)

## Supported Compilers

The library software and example programs were compiled using the GNU gcc 
and the GNU g++ compiler.

## Header Files

The directory `include/` contains all header files needed by the example programs, library
and user applications.

There are additional third party headers under THIRD_PARTIES that are required
for compilation.

## Example Programs

The directory `examples/` contains source code related to the example programs,
which demonstrate how to use and configure features of the SYNCx5104 board.

To build the example utility programs, issue the command `make` within `examples/`.

The following files are provided in `examples/`:

### [Makefile](examples/Makefile)

Build file for GNU Make to compile example programs.

### [configure_defaults.sh](examples/configure_defaults.sh)

Uses the below example programs to upload the 'default' configuration to
the SYNCx5104 board. Note how the baud rate is changed before programming.
Outputs will be recorded in `configure.log`

```sh
./configure_defaults.sh
```

### [si5395_prog.cpp](examples/si5395_prog.cpp)

Accepts and uploads a given configuration to the Si5395 Clock 
Synthesizer on the SYNCx5104.

```sh
sudo ./si5395_prog [config.txt]
```

### [syncbus_prog.cpp](examples/syncbus_prog.cpp)

Programs the Syncbus IO ports for use with other RTD products that use the
syncbus.

```sh
sudo ./syncbus_prog
```

### [syncx5104_toggle_si5395_outputs.c](examples/syncx5104_toggle_si5395_outputs.c)

This shows an example of how to change the clock synthesizer outputs while
in operation. This is done by sending a single I2C command from the FT4222H
and demonstrates how to disable & enable ALL frequency outputs of the si5395.

```sh
sudo ./syncx5104_toggle_si5395_outputs
```

### [ubx_config.c](examples/ubx_config.c)

This uploads a UCenter configuration to the Ublox LEA-M8F GPS Disciplined
Oscillator.

```sh
./ubx_config [--help] [--port /dev/ttyUSB0] [--baud_rate 115200] [--file example_config.txt] [--save]
```

### [ubx_monitor.c](examples/ubx_monitor.c)

This shows an example of how to parse the NMEA data from the GPS in linux.

```sh
./ubx_monitor [/dev/ttyUSB0] [115200]
```

### [ubx_set_baudrate.c](examples/ubx_set_baudrate.c)

This example shows how to set the baudrate of the Ublox LEA-M8F chip. The 
chips power on default is 9600, and the maximum baud is 115200.

```sh
./ubx_set_baudrate [--help] [--port /dev/ttyUSB0] [--baud_rate 115200] [--target_baud 115200]
```

## Known Limitations

- All examples need to be executed with root privileges by default, as the 
programs are accessing and writing to the USB interface directly.

- The configuration files required for SYNCx5104 can only be generated using 
a Windows machine, which will require a seperate boot partition or computer
using that operating system.

## Redistributables

Under the 'Releases' section of this Github repository, you can find the below listed redistributables. 
Instructions on their use are provided within each redistributable, recorded in a README.txt.

Software Name       | Manufacturer      | Operating System              | Description
--------------------|-------------------|-------------------------------|---------------------------------------------------------------
ClockBuilder Pro    |  Silicon Labs	    | Compatible with Windows 11    | Used to build the configuration file for the SI5395A clock DSPLL.
U-Center            |  U-Blox			| Compatible with Windows 11    | Used to monitor the SYNCx5104's GPS output, and remotely change it's configuration.
Libft4222h 			|  FTDI				| Compatible with Linux         | Used on Linux to allow communication with the Si5395 Clock Synthesizer via serial port.


## Getting Technical Support

If you require additional support with this product, or any other products from
RTD Embedded Technologies, contact us using the information below:


RTD Embedded Technologies, Inc.\
103 Innovation Boulevard\
State College, PA 16803 USA


Telephone: (814) 234-8087\
Fax: (814) 234-5218\
Sales Information and Quotes: sales@rtd.com\
Technical Assistance: techsupport@rtd.com\
Web Site: [https://www.rtd.com](https://www.rtd.com)
