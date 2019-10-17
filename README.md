# OTA Firmware update for Arduino
The libraries provide an option for updating the firmware over-the-air using four different components:
- generate_firmware - program to read  a HEX file which contains  firmware data and perform a decryption on each record data.
- control_box - application to transfer the generated file to the Arduino using RS232 connection 
- arduino/Firmware - library to handle the received messages from the control box and stores the record data on SPI flash. Performs a checksum check at the end of each transfer to verify the integrity of the received data
- bootloader - a custom version that reads data from SPI flash and decrypts it on the fly before flushing the internal memory.

The library is tested and fully functional on Arduino Leonardo board and should be compatible with other baords with a small modification on the pins for SPI flash chip.

The encryption algorithm is  based on the [tinyAES](https://github.com/kokke/tiny-AES-c) implementaion.
  
# Bootloader
The bootloader version is using the original Caterina make file with removed LUFA drivers and have a complete new custom logic. The added code will result in a compiledsize of just under 2KB (Originally Caterina takes 4KB). 

## To burn on Arduino board

    Connect your Arduino to an ISP programmer [[Installing]]
    Run the shell script from the bootloader folder to compile and upload the bootloader 
    The bootloader will flush the chip if a firmware is stored on the SPI Flash and flag UPDATE is set. Otherwise it jumps to the application sector.
    NOTE: If nothing is stored on the application sector the process will enter endless loop.


