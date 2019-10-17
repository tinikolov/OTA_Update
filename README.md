# OTA Firmware update for Arduino
The libraries provide an option for updating the firmware over-the-air using four different components:
- generate_firmware - program to read  a HEX file which contains  firmware data and perform a decryption on each record data.
- control_box - application to transfer the generated file to the Arduino using RS232 connection 
- arduino/Firmware - library to handle the received messages from the control box and stores the record data on SPI flash. Performs a checksum check at the end of each transfer to verify the integrity of the received data
- bootloader - a custom version that reads data from SPI flash and decrypts it on the fly before flushing the internal memory.

The library is tested and fully functional on Arduino Leonardo board. The SPI flash library is tested and working on Atmega2560.

The encryption algorithm is  based on the [tinyAES](https://github.com/kokke/tiny-AES-c) implementaion.

# Bootloader
The bootloader  is using the original Caterina make file with removed LUFA drivers and have a complete new custom logic. The added code will result in a compiled size of just under 2KB (Originally Caterina takes 4KB). 

## To burn on Arduino board

    Connect your Arduino to an ISP programmer [[Installing]]
    Run the shell script from the bootloader folder to compile and upload the bootloader 
    The bootloader will flush the chip if a firmware is stored on the SPI Flash and flag UPDATE is set. Otherwise it jumps to the application sector.
    NOTE: If nothing is stored on the application sector the process will enter endless loop.

# Upload process
    1. Include the arduino/Firmware library to your project and add SetupFirmwareUpdate() in the setup(), and include the FirmwareUpdate() function to the loop()
    2. To genrated encrypted version of the firmware,copy firmware.hex into generate_firmware and then compile and run the application The outcome of the program is encrypted firmware version stored in firm_encrypted.hex.
    3. Compile the application from control_box on a device which have a RS232 connection to the Arduino board
    4. Transfer the file and start the application from control_box folder
    5. Burn the custom bootloader to the device
    6. Restart
    
