# Firmware Update Over-The-Air via Bluetooth LE Project

Firmware Update Over-The-Air via Bluetooth LE for the STM32WBA52CG microcontroller.

The goal of this project is to update the user application of the microcontroller without physical access.

Inside the microcontroller, the FLASH memory is split into three parts:
- Bootloader
- User application slot A
- User application slot B

The application can be booted from either slot A or B. This allows the new version of the firmware to be placed in the other slot and enables a quick downgrade if needed, since the old version is kept in the FLASH.

The project is divided into two parts:
- **Bootloader:** Placed at the start of the FLASH memory, it launches the application from slot A or slot B, according to parameters written in the FLASH.
- **User application:** Compiled for both slots, it contains user logic alongside the BLE FUOTA logic, which handles the firmware download, writes to FLASH memory, and instructs the bootloader which slot to boot from at the next reboot.

Project works in the following environment:
- STM32CubeIDE 1.14.1
- STM32CubeMX 6.10.0
- WBA FW 1.2.0
- STM32CubeProgrammer 2.17.0 (not relevant probably)
