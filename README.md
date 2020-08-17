# arduFPGA-mega-first-stage-boot-loader
 arduFPGA first stage bootloader project for mega core

This bootloader act like a service with its own interrupt function that the running application does not even know that is running, in fact is running in the user application stack place :), but with its own 512Bytes RAM memory that is independent from the 2560Bytes of the user application RAM .

Into this bootloader are four entry vectors place at the beginning of the bootloader image, these are as fallow:

```
1f800:	_init
1f802:	_int
1f804:	main
1f806:	_set_serv_addr
1f808:	_flash_write
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
* If the "BOOT_STAT_DEBUG_EN" bit in "BOOT_STAT" PORT is set and 'DEBUG_ENABLE' is defined, will listen to the UART interface for commands, more detail in DEBUG section.
* if the INT button is press between 100mS and 500mS ( short push ) will interrupt the running application, load the GUI boot-loader that save the EEPROM content to the uSD memory card.
* If the INT button is press more than 500mS, keyboard will have other functions as described below:
1): INT + B button change the LED colour B,G,R.
2): INT + A button turn ON/OFF the flashlight (RGB led becomes flashlight).
3): INT + UP increase the game volume in four steps, from mute to maximum.
4): INT + DN decrease the game volume in four steps, from maximum to mute.

###### When INT button is press, the keyboard is disconnected for the running application, will not receive any key press until INT button is released.

###### The arduboy sound IO's are pass thru two 4 bit PWM modulators that controls the volume, bit[3:2] of the PWM's inputs are connected to '0' logic, bit[1:0] of the PWM inputs are connected to PIO[3:2] of the PORTA, the maximum volume is 1/4 of total power to avoid blowing up the ears of the listener when listen to headphones.


## main
In main function will do the fallowing actions in order:

* reset all IO's, because in the design is a dedicated reset line for the IO's, this line is asserted after power up and by software.
* deassert all chip select pins on the SPI BUS to avoid conflict's on the BUS.
* initialize the SPI interface to its maximum speed.
* load the GUI boot-loader or user application from onboard FLASH to internal program memory, if the first four bytes of the user application are 0xFF, loads the GUI boot-loader.
* enable global interrupts and jump to address 0x0000 where the loaded application is located.

## _set_serv_addr
Is the vector of function that sets the service address to be call by the "int" service function and need to have the fallowing format: void _set_serv_addr(uint16_t service_addr);.

## _flash_write
This function can be used by user application to change its own ROM content, the design does not allow the user application to directly edit the ROM content when running from the same ROM.

This function being located in the first stage boot-loader will allow user application to change the ROM content.

This function is useful for cases where the application use more data that can fit into the ROM memory and load it from another external memory as needed, or the application is to big, and is split in a bunch of library's that can be load in the ROM memory as needed.

The caller need to call this function as *** void _flash_write(uint32_t a, uint16_t *buf, uint16_t len) ***

* To pay attention that the data buffer is a buffer of uint16_t words and the given length need to be the number of words and not the number of bytes.

## DEBUG
A soft debug using the UART interface at 115200b/s that can be check by hand using a terminal like PuTTy.

#### Usage:
Type by hand but no faster than 1000 characters per second :), in fact you type a character and wait for the terminal to respond with the typed character, will take in consideration the backspace and respond with backspace.

The rule of 1000 characters/second is due to the fact that the check of UART receiving a new character is done by the service function that tick every 1mS and check for a received character, the function that check's for the character is a nonblocking function to avoid slowing down the user application.
When the terminal is sending the debug information, the user application will freeze, the debug function is suspended when GUI boot-loader is in execution because the global interrupts are disabled.

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
