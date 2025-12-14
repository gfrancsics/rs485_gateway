# RS485 Gateway

## Goal
The goal of this project is to create an interface between the Solax inverter and DDSU44 power meter. Unfortunatelly the Solax inverter requires a special settings of the Modbus communication from the power meter.
The problem with this setting is that it is out of the standard value and it is impossible to set on the device. The advantages of the custom device is that any value is possible to set. 

## Structure
The device has 2 serial port interfaces which are used for Modbus communication. It behaves once as a master and other times as a slave. 

