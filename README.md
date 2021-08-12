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
Type by hand "DEBUG" word and press enter no faster than 1000 characters per second :), in fact you type a character and wait for the terminal to respond with the typed character, will take in consideration the backspace and respond with backspace, when enter is hit the boot-loader will respond with letter 'K'.

The rule of 1000 characters/second is due to the fact that the check of UART receiving a new character is done by the service function that tick every 1mS and check for a received character, the function that check's for the character is a nonblocking function to avoid slowing down the user application.
When the device is in debug mode the user application will freeze, this way we avoid the situation when write or read of the same address is happened at the same time resulting in a outdated memory read or memory corruption and for faster transfers.

### Commands description:

##### For reading memory content:

RF: Read FLASH memory content.
RE: Read EEPROM memory content.
RR: Read RAM memory content.

#### * "Send:" and "Receive:" are not commands, they describe the direction of the data flow from boot-loader perspective.

```
Receive: RR:0000-0200
```

Will dump first 512 bytes of the RAM BUS, that in fact is IO section and first 256 bytes of RAM memory, without the register values ( registers are not connected to the RAM BUS in this design ), the report looks as fallowing:

```
Send:
>0000: 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00
>0010: 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00
>0020: fe 7f 03 10 e1 01 00 00 00 00 f5 fd 40 00 00 f1
>0030: 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00
>0040: 00 ff 03 00 00 00 00 00 00 00 00 00 50 00 ff 00
>0050: 00 00 00 00 00 00 00 00 00 00 00 00 00 6f 7f 21
>0060: 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00
>0070: 00 00 00 00 00 00 00 00 5a 02 00 00 00 00 00 00
>0080: 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00
>0090: 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00
>00a0: 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00
>00b0: 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00
>00c0: 00 00 00 00 00 00 00 00 02 18 06 00 10 00 0d 00
>00d0: 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00
>00e0: 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00
>00f0: 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 07
>0100: 00 00 ee 07 20 00 00 53 44 31 00 22 2a 3a 3c 3e
>0110: 3f 7c 7f 00 2b 2c 3b 3d 5b 5d 00 01 03 05 07 09
>0120: 0e 10 12 14 16 18 1c 1e 7d 1d 01 00 63 2c 00 1e
>0130: 96 01 a0 1e 5a 01 00 1f 08 06 10 1f 06 06 20 1f
>0140: 08 06 30 1f 08 06 40 1f 06 06 51 1f 07 00 59 1f
>0150: 52 1f 5b 1f 54 1f 5d 1f 56 1f 5f 1f 60 1f 08 06
>0160: 70 1f 0e 00 ba 1f bb 1f c8 1f c9 1f ca 1f cb 1f
>0170: da 1f db 1f f8 1f f9 1f ea 1f eb 1f fa 1f fb 1f
>0180: 80 1f 08 06 90 1f 08 06 a0 1f 08 06 b0 1f 04 00
>0190: b8 1f b9 1f b2 1f bc 1f cc 1f 01 00 c3 1f d0 1f
>01a0: 02 06 e0 1f 02 06 e5 1f 01 00 ec 1f f2 1f 01 00
>01b0: fc 1f 4e 21 01 00 32 21 70 21 10 02 84 21 01 00
>01c0: 83 21 d0 24 1a 05 30 2c 2f 04 60 2c 02 01 67 2c
>01d0: 06 01 75 2c 02 01 80 2c 64 01 00 2d 26 08 41 ff
>01e0: 1a 03 00 00 61 00 1a 03 e0 00 17 03 f8 00 07 03
>01f0: ff 00 01 00 78 01 00 01 30 01 32 01 06 01 39 01
```
```
Receive: RF:0100-0200
```

Will dump bytes from 0x100 to 0x200 of FLASH memory, the report looks as fallowing:

```
Send:
>0100: 08 2a 1c 2a 08 00 08 08 3e 08 08 00 50 30 00 00
>0110: 00 00 08 08 08 00 00 00 30 30 00 00 00 00 20 10
>0120: 08 04 02 00 3e 51 49 45 3e 00 42 7f 40 00 00 00
>0130: 42 61 51 49 46 00 21 41 45 4b 31 00 18 14 12 7f
>0140: 10 00 27 45 45 45 39 00 3c 4a 49 49 30 00 01 71
>0150: 09 05 03 00 36 49 49 49 36 00 06 49 49 29 1e 00
>0160: 36 00 00 00 00 00 56 36 00 00 00 00 08 14 22 41
>0170: 00 00 14 14 14 00 00 00 41 22 14 08 00 00 02 01
>0180: 51 09 06 00 32 49 79 41 3e 00 7e 11 11 7e 00 00
>0190: 7f 49 49 36 00 00 3e 41 41 22 00 00 7f 41 22 1c
>01a0: 00 00 7f 49 49 41 00 00 7f 09 09 01 00 00 3e 41
>01b0: 51 32 00 00 7f 08 08 7f 00 00 41 7f 41 00 00 00
>01c0: 20 40 41 3f 01 00 7f 08 14 22 41 00 7f 40 40 00
>01d0: 00 00 7f 02 04 02 7f 00 7f 04 08 10 7f 00 3e 41
>01e0: 41 3e 00 00 7f 09 09 06 00 00 3e 41 51 21 5e 00
>01f0: 7f 19 29 46 00 00 46 49 49 31 00 00 01 7f 01 00
```

##### For writing to memory:

WF: Write to FLASH memory, the addresses is in words ( 16bit/word ), because will write words not bytes.
WE: Write to EEPROM memory.
WR: Write to RAM memory.

Write 16 words beginning with address 0 to FLASH.
One word is composed of four hex digits, any time when is receiving the first digit in the group will check if a L or a X is received, for description of L and X command continue reading at the end of the page.

```
Receive: "WF:0000-0010\r"
Send: 'k'
Receive: "0000"
Send: 'k'
Receive: "0000"
Send: 'k'
Receive: "0000"
Send: 'k'
Receive: "0000"
Send: 'k'
Receive: "0000"
Send: 'k'
Receive: "0000"
Send: 'k'
Receive: "0000"
Send: 'k'
Receive: "0000"
Send: 'k'
Receive: "0000"
Send: 'k'
Receive: "0000"
Send: 'k'
Receive: "0000"
Send: 'k'
Receive: "0000"
Send: 'k'
Receive: "0000"
Send: 'k'
Receive: "0000"
Send: 'k'
Receive: "0000"
Send: 'k'
Receive: "0000"
Send: 'K'
```

To interrupt the writing loop replace:

```
Receive: "0000"
with
Receive: "X"

Will send a "K" and exit the loop.
```

To interrupt the writing loop and jump to execute the application replace:

```
Receive: "0000"
with
Receive: "L"

Will send a "K" exit the loop and jump to address 0 to execute the written application.
```

Write 16 bytes beginning with address 0 to EEPROM.
One word is composed of two hex digits, any time when is receiving the first digit in the group will check if a L or a X is received, for description of L and X command continue reading at the end of the page.

```
Receive: "WE:0000-0010\r"
Send: 'k'
Receive: "00"
Send: 'k'
Receive: "00"
Send: 'k'
Receive: "00"
Send: 'k'
Receive: "00"
Send: 'k'
Receive: "00"
Send: 'k'
Receive: "00"
Send: 'k'
Receive: "00"
Send: 'k'
Receive: "00"
Send: 'k'
Receive: "00"
Send: 'k'
Receive: "00"
Send: 'k'
Receive: "00"
Send: 'k'
Receive: "00"
Send: 'k'
Receive: "00"
Send: 'k'
Receive: "00"
Send: 'k'
Receive: "00"
Send: 'k'
Receive: "00"
Send: 'K'
```


Write 16 bytes beginning with address 0 to RAM.
One word is composed of two hex digits, any time when is receiving the first digit in the group will check if a L or a X is received, for description of L and X command continue reading at the end of the page.

```
"WR:0000-0010\r"
Receive: "WE:0000-0010\r"
Send: 'k'
Receive: "00"
Send: 'k'
Receive: "00"
Send: 'k'
Receive: "00"
Send: 'k'
Receive: "00"
Send: 'k'
Receive: "00"
Send: 'k'
Receive: "00"
Send: 'k'
Receive: "00"
Send: 'k'
Receive: "00"
Send: 'k'
Receive: "00"
Send: 'k'
Receive: "00"
Send: 'k'
Receive: "00"
Send: 'k'
Receive: "00"
Send: 'k'
Receive: "00"
Send: 'k'
Receive: "00"
Send: 'k'
Receive: "00"
Send: 'k'
Receive: "00"
Send: 'K'
```

To interrupt the writing loop replace:

```
Receive: "0000"
with
Receive: "X"

Will send a "K" and exit the loop.
```

To interrupt the writing loop and jump to execute the application replace:

```
Receive: "0000"
with
Receive: "L"

Will send a "K" exit the loop and jump to address 0 to execute the write application.
```

The read and write commands supports writing and reading from one byte to the whole memory fallowing the debug application to implement memory pooling and soft control of each IO on the BUS.
