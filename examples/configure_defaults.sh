#!/bin/sh

# This will return a failure becuase the baud rate is changing and the acknowledge signal isn't responded to.
echo "Setting UBX Baud Rate..."
sudo ./ubx_set_baudrate --port /dev/ttyUSB0 --baud_rate 9600 --target_baud 115200 > configure.log

if [ "$?" -eq '1' ]
then
	echo "Failed to Set UBX Baud rate Check the configure.log for details"
fi

# Double check the register should return a successful acknowledge this time
echo "Checking UBX Baud Rate..."
sudo ./ubx_set_baudrate --port /dev/ttyUSB0 --baud_rate 115200 --target_baud 115200 >> configure.log

if [ "$?" -eq '1' ]
then
	echo "Failed to Set UBX Baud rate Check the configure.log for details"
fi

# Configure the GPS module with the default config file.
echo "Uploading UBX Configuration..."
sudo ./ubx_config --port /dev/ttyUSB0 --baud_rate 115200 --file cfg/gps_config_115200.txt >> configure.log

if [ "$?" -eq '1' ]
then
	echo "Failed to upload UBX configureation Check the configure.log for details"
fi


# si5395 program frequency outputs
echo "Uploading Si5395 Configuration..."
sudo ./si5395_prog cfg/Si5395-RevA-S35104-Registers.txt >> configure.log

if [ "$?" -eq '1' ]
then
	echo "Failed to set up si5395 frequency outputs Check the configure.log for details"
fi


# program syncbus enable lines
echo "Configuring Syncbus..."
sudo ./syncbus_prog 0 >> configure.log

if [ "$?" -eq '1' ]
then
	echo "Failed to Set Syncbus Outputs Check the configure.log for details"
fi

echo "Configuration completed. Check configure.log for details."
