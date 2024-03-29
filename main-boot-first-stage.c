/*
 * Main function for first stage BOOT-LOADRER of ARDUFPGA soft core design.
 * 
 * Copyright (C) 2020  Iulian Gheorghiu (morgoth@devboard.tech)
 * 
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 */

#include <avr/interrupt.h>
#include <avr/pgmspace.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <avr/io.h>
#include <avr/eeprom.h>
#include <stdlib.h>
#include "def.h"
#include "driver/spi.h"
#include "device/25flash.h"
#include "driver/uart.h"
#include "unions.h"
#include "util.h"

#include "arduinoBoot.h"

void (*service_Ptr)(void) = NULL;
volatile void *sp_value = 0;
volatile int16_t cnt_int_key_k;
volatile uint8_t pushed;
volatile bool after_power_up = true;
uint8_t flash_buf[FLASH_BUFF_LEN];
volatile uint8_t led_color = 0;
volatile uint8_t debug_char_in_cnt = 0;
volatile uint8_t volume;
volatile bool usb_function_ntsc_out = false;
volatile bool aud_function_ntsc_out = false;
volatile uint8_t debug_char_buf[16] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
	
spi_t spi;
uint8_t screen_buf[1];
_25flash_t flash_des;
//_25flash_t flash_app;

void init(void) __attribute__ ((naked)) __attribute__ ((section (".init0")));
int main(void) __attribute__ ((naked)) __attribute__ ((section (".init5")));
//void end(void) __attribute__ ((naked)) __attribute__ ((section (".init6")));

/* In order to suspend the user application we need to backup all registers SREG and special registers. */
void enter_sleep() {// FOR FUTURE USE
/*
 * Save all registers, special registers and SREG.
 */
	__asm__ __volatile__
	(
	"push r0 \n\t"
	"push r1 \n\t"
	"push r2 \n\t"
	"push r3 \n\t"
	"push r4 \n\t"
	"push r5 \n\t"
	"push r6 \n\t"
	"push r7 \n\t"
	"push r8 \n\t"
	"push r9 \n\t"
	"push r10 \n\t"
	"push r11 \n\t"
	"push r12 \n\t"
	"push r13 \n\t"
	"push r14 \n\t"
	"push r15 \n\t"
	"push r16 \n\t"
	"push r17 \n\t"
	"push r18 \n\t"
	"push r19 \n\t"
	"push r20 \n\t"
	"push r21 \n\t"
	"push r22 \n\t"
	"push r23 \n\t"
	"push r24 \n\t"
	"push r25 \n\t"
	"push r26 \n\t"
	"push r27 \n\t"
	"push r28 \n\t"
	"push r29 \n\t"
	"push r30 \n\t"
	"push r31 \n\t"
	"in r16, %0 \n\t"
	"push r16 \n\t"
	:
	: "I" (_SFR_IO_ADDR(SREG))
	);
/*
* Save STAK pointer.
*/
	__asm__ __volatile__
	(
	"in r16, %0 \n\t"
	"in r17, %1 \n\t"
	"sts (sp_value), R16 \n\t"
	"sts (sp_value + 1), R17 \n\t"
	:
	: "I" (_SFR_IO_ADDR(SPL)),
		"I" (_SFR_IO_ADDR(SPH))
	);
/*
* Save the RAM application state.
*/
	//cnt++;
	//PORTB = cnt << 5;
				
/*
* Restore STAK pointer.
*/
	__asm__ __volatile__
	(
	"lds R17, (sp_value + 1) \n\t"
	"lds R16, (sp_value) \n\t"
	"out %1, R17 \n\t"
	"out %0, R16 \n\t"
	:
	: "I" (_SFR_IO_ADDR(SPL)),
		"I" (_SFR_IO_ADDR(SPH))
	);

/*
* Restore all registers, special registers and SREG.
*/
	__asm__ __volatile__
	(
	"pop r16 \n\t"
	"out %0, r16 \n\t"
	"pop r31 \n\t"
	"pop r30 \n\t"
	"pop r29 \n\t"
	"pop r28 \n\t"
	"pop r27 \n\t"
	"pop r26 \n\t"
	"pop r25 \n\t"
	"pop r24 \n\t"
	"pop r23 \n\t"
	"pop r22 \n\t"
	"pop r21 \n\t"
	"pop r20 \n\t"
	"pop r19 \n\t"
	"pop r18 \n\t"
	"pop r17 \n\t"
	"pop r16 \n\t"
	"pop r15 \n\t"
	"pop r14 \n\t"
	"pop r13 \n\t"
	"pop r12 \n\t"
	"pop r11 \n\t"
	"pop r10 \n\t"
	"pop r9 \n\t"
	"pop r8 \n\t"
	"pop r7 \n\t"
	"pop r6 \n\t"
	"pop r5 \n\t"
	"pop r4 \n\t"
	"pop r3 \n\t"
	"pop r2 \n\t"
	"pop r1 \n\t"
	"pop r0 \n\t"
	:
	: "I" (_SFR_IO_ADDR(SREG))
	);
}

void _flash_des_write(uint32_t addr, uint8_t *buff, uint16_t size) {
	_25flash_write(&flash_des, addr, (void *)buff, size);
}

void _flash_des_erase(uint32_t addr) {
	_25flash_erase(&flash_des, addr);
}

void ram_restore() {
	/* Load the RAM size. */
	_25flash_read(&flash_des, FLASH_APP_USER_START_ADDR + FLASH_APP_MEMORY_SIZES_OFFSET + FLASH_APP_MEMORY_SIZES_RAM_OFFSET, flash_buf, 4);
	uint32_t ram_size = (uint32_t)flash_buf[3] << 24 | (uint32_t)flash_buf[2] << 16 | (uint32_t)flash_buf[1] << 8 | (uint32_t)flash_buf[0];
	/* Protect against empty FLASH */
	if(ram_size > 0x2000) {
		ram_size = 0xA00;
	}
	/* Load the RAM from user application section. */
	for (uint16_t cnt = 0x100; cnt < ram_size + 0x100; cnt += 0x100) {
		_25flash_read(&flash_des, FLASH_APP_USER_START_ADDR + FLASH_APP_RAM_OFFSET + cnt, (void *)cnt, 0x100);
	}
}
/* The back-up of RAM content to the flash need to happen here, in first-stage-bootloader, before launch on GUI explorer, because GUI explorer use the same RAM as the user application */
void ram_backup() {
	/* Load the RAM size. */
	_25flash_read(&flash_des, FLASH_APP_USER_START_ADDR + FLASH_APP_MEMORY_SIZES_OFFSET + FLASH_APP_MEMORY_SIZES_RAM_OFFSET, flash_buf, 4);
	uint32_t ram_size = (uint32_t)flash_buf[3] << 24 | (uint32_t)flash_buf[2] << 16 | (uint32_t)flash_buf[1] << 8 | (uint32_t)flash_buf[0];
	/* Protect against empty FLASH */
	if(ram_size > 0x2000) {
		ram_size = 0xA00;
	}
	/* Erase FLASH user area RAM section. */
	for (uint16_t cnt = 0x100; cnt < 0x20000 + 0x100; cnt += 0x1000) {
		_flash_des_erase(FLASH_APP_USER_START_ADDR + FLASH_APP_RAM_OFFSET + cnt);
	}
	/* Back-up the RAM content to FLASH user area RAM section. */
	for (uint16_t cnt = 0x100; cnt < ram_size + 0x100; cnt += 0x100) {
		_flash_des_write(FLASH_APP_USER_START_ADDR + FLASH_APP_RAM_OFFSET + cnt, (void *)cnt, 0x100);
	}
}

void eep_load () {
	/* Load the EEPROM size. */
	//_25flash_read(&flash_des, FLASH_APP_USER_START_ADDR + FLASH_APP_MEMORY_SIZES_OFFSET + FLASH_APP_EEP_OFFSET, flash_buf, 4);
	//uint32_t eep_size = (uint32_t)flash_buf[3] << 24 | (uint32_t)flash_buf[2] << 16 | (uint32_t)flash_buf[1] << 8 | (uint32_t)flash_buf[0];
	/* Protect against empty FLASH */
	/*if(eep_size > FLASH_APP_EEP_SIZE) {
		eep_size = EEP_SIZE;
	}*/
	for (uint16_t cnt = 0; cnt < EEP_SIZE; cnt += FLASH_BUFF_LEN) {
		_25flash_read(&flash_des, FLASH_APP_USER_START_ADDR + FLASH_APP_EEP_OFFSET + cnt, flash_buf, FLASH_BUFF_LEN);
		eeprom_write_block(flash_buf, (void *)cnt, FLASH_BUFF_LEN);
	}
}

void flash_load (uint32_t flash_app_addr) {
	F_CNT_L = 0;
	F_CNT_H = 0;

	PORTB = (PORTB & 0b00011111) | LED_R;
	/* Load the APP size. */
	//_25flash_read(&flash_des, flash_app_addr + FLASH_APP_MEMORY_SIZES_OFFSET + FLASH_APP_MEMORY_SIZES_PGM_OFFSET, flash_buf, 4);
	//uint32_t app_size = (uint32_t)flash_buf[3] << 24 | (uint32_t)flash_buf[2] << 16 | (uint32_t)flash_buf[1] << 8 | (uint32_t)flash_buf[0];
	/* Protect against empty FLASH */
	/*if(app_size > 0x1E000) {
		app_size = 0x8000;
	}*/
	/* Load the APP from selected flash section. */
	BOOT_STAT |= BOOT_STAT_APP_PGM_WR_EN;
	for (uint32_t cnt = flash_app_addr; cnt < flash_app_addr + FLASH_SIZE; cnt += 2) {
		if(!(cnt & (FLASH_BUFF_LEN - 1))) {
			_25flash_read(&flash_des, cnt, flash_buf, FLASH_BUFF_LEN);
		}
		F_DATA_L = flash_buf[(cnt & (FLASH_BUFF_LEN - 1)) + 0];
		F_DATA_H = flash_buf[(cnt & (FLASH_BUFF_LEN - 1)) + 1];
	}
	BOOT_STAT &= ~BOOT_STAT_APP_PGM_WR_EN;

	PORTB = (PORTB & 0b00011111);
}

void init() {
	__asm__ __volatile__ (
	"rjmp _init \n\t"
	"rjmp _int \n\t"
	"rjmp main \n\t"
	"rjmp _set_serv_addr \n\t"
	"rjmp _flash_write \n\t"
	//"rjmp _flash_des_erase \n\t"
	//"rjmp _flash_des_write \n\t"
	"_init: \n\t"
	"cli \n\t"
	"eor	r1, r1 \n\t"
	"out	0x3f, r1 \n\t"//	; 63
	"ldi	r28, 0xFF \n\t"//	; 255
	"ldi	r29, 0xFF \n\t"//	; 10
	"out	0x3e, r29 \n\t"//	; 62
	"out	0x3d, r28 \n\t");//	; 61
	uint16_t cnt = 0x100;
	for (; cnt < 0xFE00; cnt++) {
		*((uint8_t *)cnt) = 0;
	}
}

int main(void)
{
	BOOT_STAT &= ~BOOT_STAT_NMI_INT_ENABLE;
	service_Ptr = NULL;
	BOOT_STAT |= BOOT_STAT_IO_RST;
	asm("nop");
	BOOT_STAT &= ~BOOT_STAT_IO_RST;
	
	DDRB = 0b11100000;
    PORTB |= 0b11100000;
    
	SPI_SSD1306_CS_DEASSERT();
	SPI_uSD_CS_DEASSERT();
	SPI_ADC_CS_DEASSERT();
	SPI_DES_CS_DEASSERT();
	SPI_APP_CS_DEASSERT();
	SPI_xCS_CS_DEASSERT();
	SPI_xDCS_CS_DEASSERT();
	
	spi.spcr = &SPCR;
	spi.spsr = &SPSR;
	spi.spdr = &SPDR;
	spi_init(&spi);
	flash_des.spi = &spi;
	flash_des.cs_port_out = &SPI_CS_4_PORT;
	flash_des.pin_mask = SPI_CS_4_PIN;
	//flash_app.spi = &spi;
	//flash_app.cs_port_out = &SPI_CS_5_PORT;
	//flash_app.pin_mask = SPI_CS_5_PIN;
	
#ifdef USE_ARDUINO_BOOT_LOADER
	if(~KBD_IN & KBD_INT_PIN) {
		cli();
		enter_arduino_bootloader();
	}
#endif

	/* Choose the APP section in the FLASH to load. */
	uint32_t flash_app_addr;
	if(BOOT_STAT & BOOT_STAT_FLASH_APP_NR) {
		flash_app_addr = FLASH_APP_USER_START_ADDR;
	} else {
		flash_app_addr = FLASH_APP_EXPLORER_START_ADDR;
	}

#ifdef POWER_UP_WITH_USER_APP	
// If at power UP the INT button is pressed will skip loading the user application, will load the GUI boot-loader.
// If in the user application is something wrong and freezes the core, user has a way to avoid loading the APP and load the GUI boot-loader instead.
	if(KBD_IN & KBD_INT_PIN) {
// After power UP check if a user application is written in the FLASH, if not, proceed with launching the explorer.
		if(after_power_up) {
			after_power_up = false;
			_25flash_read(&flash_des, FLASH_APP_USER_START_ADDR, flash_buf, 4);
			if((flash_buf[0] != 0xFF || flash_buf[1] != 0xFF || flash_buf[2] != 0xFF || flash_buf[3] != 0xFF)) {
				BOOT_STAT |= BOOT_STAT_FLASH_APP_NR;
				flash_app_addr = FLASH_APP_USER_START_ADDR;
			}
		}
	}
#endif
	
	flash_load(flash_app_addr);
	
	if(BOOT_STAT & BOOT_STAT_FLASH_APP_NR) {
		eep_load();
		BOOT_STAT |= BOOT_STAT_NMI_INT_ENABLE;
	} else {
		BOOT_STAT &= ~BOOT_STAT_NMI_INT_ENABLE;
	}
	uart_init(115200);
	uart_put_s("arduFPGA iCE40UP5k (morgoth@devboard.tech)\n\r");
	if(BOOT_STAT & BOOT_STAT_FLASH_APP_NR) {
		sei();
	}
	asm("jmp 0x0000");
}

void _set_serv_addr(uint16_t service_addr) {
	uint8_t tmp = _SFR_IO8(0x3F);
	cli();
	service_Ptr = (void *)service_addr;
	_SFR_IO8(0x3F) = tmp;
	/*if(tmp & 0x80) {
		sei();
	}*/
	//asm("ret");
}

void _flash_write(uint32_t a, uint16_t *buf, uint16_t len) {
	asm("push r26");
	uint8_t tmp = _SFR_IO8(0x3F);
	cli();
	convert16to8 co;
	co.i16 = a;
	F_CNT_L = co.Byte0;
	F_CNT_H = co.Byte1;
	BOOT_STAT |= BOOT_STAT_APP_PGM_WR_EN;
	while (len--){
		co.i16 = *buf++;
		F_DATA_L = co.Byte0;
		F_DATA_H = co.Byte1;
	}
	BOOT_STAT &= ~BOOT_STAT_APP_PGM_WR_EN;
	_SFR_IO8(0x3F) = tmp;
	asm("pop r26");
}


void debug() {
	debug_char_in_cnt = 0;
	memset((char *)debug_char_buf, 0, sizeof(debug_char_buf));
	while (1) {
		uint8_t c;
		if(uart_get_c_nb(&c)) {
			if(c == 0x08) {
				uart_put_c(c);
				if(debug_char_in_cnt > 0) {
					debug_char_in_cnt--;
				}
				debug_char_buf[debug_char_in_cnt] = 0;
			} else {
				if(debug_char_in_cnt < sizeof(debug_char_buf)) {
					uart_put_c(c);
					debug_char_buf[debug_char_in_cnt] = c;
					debug_char_in_cnt++;
				}
				if(c == 'X') {
					return;
				}
				if(c == 0x0a || c == 0x0d) {
					if(debug_char_in_cnt >= 12 && (debug_char_buf[0] == 'W' || debug_char_buf[0] == 'R') && debug_char_buf[2] == ':' && debug_char_buf[7] == '-') { //WR:0000-0000
						uint8_t bin_buff_addr[2];
						uint8_t bin_buff_len[2];
						if(util_get_bin_from_hex_buf(bin_buff_addr, (char *)(debug_char_buf + 3), 2) == 2 && util_get_bin_from_hex_buf(bin_buff_len, (char *)(debug_char_buf + 8), 2) == 2) {
							uint32_t cnt = uni_8_to_16(bin_buff_addr[0], bin_buff_addr[1]);
							uint32_t end = uni_8_to_16(bin_buff_len[0], bin_buff_len[1]);
							if(end == 0) {
								end = 0x10000;
							}
							uint8_t cm0 = debug_char_buf[0];
							uint8_t cm1 = debug_char_buf[1];
							if(cm0 == 'R') {
								if(cnt & 0x0F) {
									uart_put_s("\n\r>");
									uart_print_hex_short(cnt);
									uart_put_s(": ");
								}
								for (; cnt < end; cnt++) {
									if((cnt & 0x0F) == 0) {
										uart_put_s("\n\r>");
										uart_print_hex_short(cnt);
										uart_put_s(": ");
									} else {
										uart_put_c(' ');
									}
									if(cm1 == 'F') {
										uart_print_hex_char(pgm_read_byte(cnt));
									} else if(cm1 == 'E') {
										uart_print_hex_char(eeprom_read_byte((uint8_t *)(uint16_t)cnt));
									} else if(cm1 == 'R') {
										uart_print_hex_char(*((uint8_t *)(uint16_t)cnt));
									}
								}
								uart_put_s("\n\r");
								debug_char_in_cnt = 0;
								memset((char *)debug_char_buf, 0, sizeof(debug_char_buf));
							} else if(cm0 == 'W') {
								if(cm1 == 'F') {
									uart_put_c('k');
									F_CNT_L = 0;
									F_CNT_H = 0;
									for (; cnt < end; cnt++) {
										uint8_t err = 0;
										uint8_t c0 = 0, c1 = 0, c2 = 0, c3 = 0;
										c0 = uart_get_c();
										if(util_is_hex(c0)) {
											c1 = uart_get_c();
											if(util_is_hex(c1)) {
												c2 = uart_get_c();
												if(util_is_hex(c2)) {
													c3 = uart_get_c();
													if(!util_is_hex(c3)) {
														err++;
														uart_put_c('e');
													}
												} else {
													err++;
													uart_put_c('e');
												}
											} else {
												err++;
												uart_put_c('e');
											}
										} else {
											err++;
											uart_put_c('e');
										}
										if(!err) {
											util_get_bin_from_hex_char(&c0, c0);
											util_get_bin_from_hex_char(&c1, c1);
											util_get_bin_from_hex_char(&c2, c2);
											util_get_bin_from_hex_char(&c3, c3);
											uart_put_c('k');
											BOOT_STAT |= BOOT_STAT_APP_PGM_WR_EN;
											F_DATA_L = (c2 << 4) | (c3  & 0x0F);
											F_DATA_H = (c0 << 4) | (c1  & 0x0F);
											BOOT_STAT &= ~BOOT_STAT_APP_PGM_WR_EN;
										} else if(err == 1) {
											if(c0 == 'L') {
												/*uint16_t cnt = 0;
												for (; cnt < 0xFE00; cnt++) {
													*((uint8_t *)cnt) = 0;
												}*/
												BOOT_STAT |= BOOT_STAT_FLASH_APP_NR;
												BOOT_STAT |= BOOT_STAT_NMI_INT_ENABLE;
												sei();
												asm("jmp 0x0000");
											} else if(cm0 == 'X') {
												return;
											}
										}
									}
									uart_put_c('K');
									debug_char_in_cnt = 0;
									memset((char *)debug_char_buf, 0, sizeof(debug_char_buf));
								} else if(cm1 == 'E' || cm1 == 'R') {
									uart_put_c('k');
									for (; cnt < end; cnt++) {
										uint8_t err = 0;
										uint8_t c0 = 0, c1 = 0;
										c0 = uart_get_c();
										if(util_is_hex(c0)) {
											c1 = uart_get_c();
											if(!util_is_hex(c1)) {
												err++;
												uart_put_c('e');
											}
										} else {
											err++;
											uart_put_c('e');
										}
										if(!err) {
											util_get_bin_from_hex_char(&c0, c0);
											util_get_bin_from_hex_char(&c1, c1);
											uart_put_c('k');
											if(cm1 == 'E') {
												eeprom_write_byte((uint8_t *)(uint16_t)cnt, (c0 << 4) | (c1  & 0x0F));
											} else if(cm1 == 'R') {
												*((uint8_t *)(uint16_t)cnt) = (c0 << 4) | (c1  & 0x0F);
											}
										} else if(err == 1) {
											if(c0 == 'L') {
												uart_put_c('K');
												BOOT_STAT |= BOOT_STAT_FLASH_APP_NR;
												BOOT_STAT |= BOOT_STAT_NMI_INT_ENABLE;
												sei();
												asm("jmp 0x0000");
											} else if(c0 == 'X') {
												return;
											}
										}
									}
									uart_put_c('K');
									debug_char_in_cnt = 0;
									memset((char *)debug_char_buf, 0, sizeof(debug_char_buf));
								} else {
									uart_put_c('e');
								}
							}
						} else {
							uart_put_s("ERR FORMAT\n\r");
						}
					} else {
						uart_put_s("ERR FORMAT\n\r");
					}
				}
			}
		}
	}
}

void _int(void) __attribute__ ((signal,__INTR_ATTRS));
void _int(void) {
// If a service function is registered, call it.
	if(service_Ptr) {
		service_Ptr();
	}
#ifdef DEBUG_ENABLE
	//if(BOOT_STAT & BOOT_STAT_DEBUG_EN) {
		uint8_t c;
		//uart_put_c('-');
		if(uart_get_c_nb(&c)) {
			if(debug_char_in_cnt < sizeof(debug_char_buf)) {
				debug_char_buf[debug_char_in_cnt++] = c;
			}
			if(c == 0x0a || c == 0x0d) {
				if(!memcmp((char *)debug_char_buf, "DEBUG", 5)) {
					uart_put_c('K');
					debug();
					uart_put_c('K');
				}
				debug_char_in_cnt = 0;
				memset((char *)debug_char_buf, 0, sizeof(debug_char_buf));
			} else {
				uart_put_c(c);
			}
		}
	//}
#endif
	if(KBD_IN & KBD_INT_PIN) {
/*
 * Load GUI boot-loader only if user APP is running, if INTERRUPT button is press between 100 and 500 mS.
 */
		if(cnt_int_key_k > 100 && cnt_int_key_k < 500 && BOOT_STAT & BOOT_STAT_FLASH_APP_NR) {
			cnt_int_key_k = 0;
			BOOT_STAT &= ~BOOT_STAT_FLASH_APP_NR;
			led_color = 0;
			main();
		}
		cnt_int_key_k = 0;
		//pushed &= ~KBD_INT_PIN;
		DISC_USR_KBD_PORT &= ~DISC_USR_KBD_PIN_bm;
	} else {
		DISC_USR_KBD_PORT |= DISC_USR_KBD_PIN_bm;
		if(cnt_int_key_k != 500) {
			cnt_int_key_k++;
		} else {
/*
 * If the INT button was pressed more than two seconds.
 */
			uint8_t tmp_kbd = KBD_IN;
			if (pushed ^ tmp_kbd) {
/*
 * Parse the reason of INT pressing.
 */
				if(~tmp_kbd & KBD_B_PIN) {
					pushed &= ~KBD_B_PIN;
/*
 * Change the color of the LED in order: B, G, R.
 */
					led_color = led_color << 1;
					if(!led_color) {
						led_color = LED_B;
					}
					PORTB = (PORTB & 0b00011111) | led_color;
				} else if(~tmp_kbd & KBD_A_PIN) {
					pushed &= ~KBD_A_PIN;
					led_color = 0;
					PORTB = (PORTB & 0b00011111) | (PORTB & (LED_R | LED_G | LED_B) ?  0 : LED_R | LED_G | LED_B);
				} else if(~tmp_kbd & KBD_U_PIN) {
/*
 * Volume UP.
 */
					pushed &= ~KBD_U_PIN;
					if(PWM_VOLUME_PORT & PWM_VOLIME_PIN_gm) {
						PWM_VOLUME_PORT = (PWM_VOLUME_PORT & ~PWM_VOLIME_PIN_gm) | ((PWM_VOLUME_PORT - (1 << PWM_VOLIME_PIN_gp)) & PWM_VOLIME_PIN_gm);
					}
				} else if(~tmp_kbd & KBD_D_PIN) {
/*
 * Volume DOWN.
 */
					pushed &= ~KBD_D_PIN;
					if((PWM_VOLUME_PORT & PWM_VOLIME_PIN_gm) != PWM_VOLIME_PIN_gm) {
						PWM_VOLUME_PORT = (PWM_VOLUME_PORT & ~PWM_VOLIME_PIN_gm) | ((PWM_VOLUME_PORT + (1 << PWM_VOLIME_PIN_gp)) & PWM_VOLIME_PIN_gm);
					}
				} else if(~tmp_kbd & KBD_L_PIN) {
/*
 * Select USB connector function.
 */
					pushed &= ~KBD_L_PIN;
					if(usb_function_ntsc_out) {
						USB_NTSC_EN_PORT &= ~USB_NTSC_EN_PORT_bm;
						usb_function_ntsc_out = false;
					} else {
						USB_NTSC_EN_PORT |= USB_NTSC_EN_PORT_bm;
						usb_function_ntsc_out = true;
					}
				} else if(~tmp_kbd & KBD_R_PIN) {
/*
 * Select Audio connector function.
 */
					pushed &= ~KBD_R_PIN;
					if(aud_function_ntsc_out) {
						AUD_NTSC_EN_PORT &= ~AUD_NTSC_EN_PORT_bm;
						aud_function_ntsc_out = false;
					} else {
						AUD_NTSC_EN_PORT |= AUD_NTSC_EN_PORT_bm;
						aud_function_ntsc_out = true;
					}
				}
			}
			pushed |= tmp_kbd;
		}
	}
}
