# arduFPGA-mega-first-stage-boot-loader
 arduFPGA first stage bootloader project for mega core

This bootloader act like a service with its own interrupt function that the running application does not even know that is running, in fact is running in the user application stack place :), but with its own 512Bytes RAM memory that is independent from the 2560Bytes of the user application RAM .

Into this bootloader are four entry vectors place at the beginning of the bootloader image, these are as fallow:

```
1f800:	_init
1f802:	_int
1f804:	main
1f806:	_set_serv_addr
```

* ##### "_init" is the start vector ( reset vector ) of the bootloader, here the uC land after hardware reset.
* ##### "_int" is NMI interrupt vector, for a 1mS tick.
* ##### "main" is the vector where the GUI explorer boot-loader jumps after load the application to be run and after "_init" function do his stuff.
* ##### "_set_serv_addr" is the vector where the user application can set a service function address to be call every 1mS and share the same RTC tick without the need to implement a separate timer for this in the design.

## _init

After power up the uC jump to 0x1f800, from there jumps to the "_init", "_init" function disable the global interrupts, initiate the stack pointer with the 0x0CFF address value, the end of the 512Bytes of RAM dedicated to the first stage bootloader, initiate the RAM static variables and clear the used RAM.

## int
"int" function is the service function that is call by NMI line in interrupt controller every 1mS.

The "int" function do the fallowing actions in order:
* Check if the user application service function vector address is set up, if is different than 0x0000 will call the function, the header of the function need to be: void service_Ptr(void);.
* If the "BOOT_STAT_DEBUG_EN" bit in "BOOT_STAT" PORT is set, will listen to the UART interface for commands, more detail in DEBUG section.
* If the INTERRUPT button is pressed at last two seconds will do the fallowing:
1) If no other key is press, will load and launch the GUI boot-loader, that will save the EEPROM content to the uSD.
2) If UP button is press, will change the colour of the LED, at first INTERRUPT press to RED, at second press to GREEN, at third to BLUE and at fourth will power off the LED and continue from beginning, all these actions without interrupting the user application.
3) If DOWN button is press, will turn all three colours ON and OFF and act like a flashlight, all these actions without interrupting the user application.

## main
In main function will do the fallowing actions in order:

* reset all IO's, because in the design is a dedicated reset line for the IO's, this line is asserted after power up and by software.
* deassert all chip select pins on the SPI BUS to avoid conflict's on the BUS.
* initialize the SPI interface to its maximum speed.
* load the GUI boot-loader or user application from onboard FLASH to internal program memory, if the first four bytes of the user application are 0xFF, loads the GUI boot-loader.
* enable global interrupts and jump to address 0x0000 where the loaded application is located.

## _set_serv_addr
Is the vector of function that sets the service address to be call by the "int" service function and need to have the fallowing format: void _set_serv_addr(uint16_t service_addr);.

## DEBUG (NOT FUNCTIONAL YET, one of the reasons is some issues in the RX section of the UART IP)
In development, a soft debug using the UART interface and can be check by hand using a terminal.

#### Usage:
Type by hand but no faster than 1000 characters per second :), in fact you type a character and wait for the terminal to respond with the typed character, will take in consideration the backspace and respond with backspace.

The rule of 1000 characters/second is due to the fact that the check of UART receiving a new character is done by the service function that tick every 1mS and check for a received character, the function that check's for the character is a nonblocking function to avoid slowing down the user application.
When the terminal is sending the debug information, the user application will freeze.

```
DUMP0
```

Will dump the first 256 bytes of the RAM BUS, that in fact is the IO section, without the register values, the report is as fallowing:

```
ADDR: 0x0000  HEX VAL: 0x00  BIN VAL: 0b00000000  ASCII VAL: \n\r
ADDR: 0x0001  HEX VAL: 0x00  BIN VAL: 0b00000000  ASCII VAL: \n\r
ADDR: 0x0002  HEX VAL: 0x00  BIN VAL: 0b00000000  ASCII VAL: \n\r
...
```
```
DUMP1
```

Will dump the second 256 bytes of the RAM BUS, that in fact is the first 256Bytes of the internal SRAM:

```
ADDR: 0x0100  HEX VAL: 0x00  BIN VAL: 0b00000000  ASCII VAL: \n\r
ADDR: 0x0101  HEX VAL: 0x00  BIN VAL: 0b00000000  ASCII VAL: \n\r
ADDR: 0x0102  HEX VAL: 0x00  BIN VAL: 0b00000000  ASCII VAL: \n\r
...
```
The value after DUMP can be up to 256 in decimal format, optionally you can uncomment the "DEBUG_BINARY" definition line and will return 256Bytes representing the memory values in binary format, this way will cut ~200Bytes of program memory of the boot-loader.
