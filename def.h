
#ifndef DEF_H_
#define DEF_H_

#include <avr/io.h>
#include <stdint.h>
#include <stdint.h>

#define F_CPU	16000000
#define SSD1306_USE_NO_BUF
#define POWER_UP_WITH_USER_APP

/*
 This ports does not exist in the original micro controller.
 This ports are used by boot loader, but can be used even by the application.
 */
#define PINA						_SFR_IO8(0x00)
#define DDRA						_SFR_IO8(0x01)
#define PORTA						_SFR_IO8(0x02)

#define FLASH_APP_EXPLORER_START_ADDR	0x100000
#define FLASH_APP_USER_START_ADDR		0x140000

#define FLASH_APP_ROM_OFFSET				0x00000
#define FLASH_APP_RAM_OFFSET				0x20000
#define FLASH_APP_EEP_OFFSET				0x40000
#define FLASH_APP_PATH_OFFSET				0x42000
#define FLASH_APP_MEMORY_SIZES_OFFSET		0x43000
#define FLASH_APP_MEMORY_SIZES_PGM_OFFSET	0x4
#define FLASH_APP_MEMORY_SIZES_RAM_OFFSET	0x8
#define FLASH_APP_MEMORY_SIZES_EEP_OFFSET	0xC

/* These IO's are placed in SRAM data space, we need to subtract 0x20 value to address them correctly. */
#define F_CNT_L						_SFR_IO8(0xDB)
#define F_CNT_H						_SFR_IO8(0xDC)
#define F_DATA_L					_SFR_IO8(0xDD)
#define F_DATA_H					_SFR_IO8(0xDE)
#define BOOT_STAT					_SFR_IO8(0xDF)

#define BOOT_STAT_FLASH_APP_NR		(1 << 0)
#define BOOT_STAT_EEP_EDITED		(1 << 1)
#define BOOT_STAT_USR_APP_RUNNING	(1 << 2)
#define BOOT_STAT_APP_PGM_WR_EN		(1 << 3)
#define BOOT_STAT_IO_RST			(1 << 4)

#define KBD_DIR						DDRA
#define KBD_PORT					PORTA
#define KBD_IN						PINA
#define KBD_U_PIN					(1<<7)
#define KBD_D_PIN					(1<<6)
#define KBD_B_PIN					(1<<5)
#define KBD_K_PIN					(1<<4)
#define KBD_INT_PIN					(1<<3)

#define LED_B						0b00100000
#define LED_R						0b01000000
#define LED_G						0b10000000

/*******************************************/

#define SPI_SCK_DIR					DDRB
#define SPI_SCK_PORT				PORTB
#define SPI_SCK_PIN					(1<<1)

#define SPI_MISO_DIR				DDRB
#define SPI_MISO_PORT				PORTB
#define SPI_MISO_PIN				(1<<3)

#define SPI_MOSI_DIR				DDRB
#define SPI_MOSI_PORT				PORTB
#define SPI_MOSI_PIN				(1<<2)

#define SPI_CS_1_DIR				DDRD
#define SPI_CS_1_PORT				PORTD
#define SPI_CS_1_PIN				(1<<6)
#define SPI_SSD1331_CS_ASSERT()		SPI_CS_1_PORT &= ~SPI_CS_1_PIN
#define SPI_SSD1331_CS_DEASSERT()	SPI_CS_1_PORT |= SPI_CS_1_PIN
#define SPI_SSD1306_CS_ASSERT()		SPI_CS_1_PORT &= ~SPI_CS_1_PIN
#define SPI_SSD1306_CS_DEASSERT()	SPI_CS_1_PORT |= SPI_CS_1_PIN

#define SPI_CS_2_DIR				DDRD
#define SPI_CS_2_PORT				PORTD
#define SPI_CS_2_PIN				(1<<2)
#define SPI_uSD_CS_ASSERT()			SPI_CS_2_PORT &= ~SPI_CS_2_PIN
#define SPI_uSD_CS_DEASSERT()		SPI_CS_2_PORT |= SPI_CS_2_PIN

#define SPI_CS_3_DIR				DDRB
#define SPI_CS_3_PORT				PORTB
#define SPI_CS_3_PIN				(1<<3)
#define SPI_ADC_CS_ASSERT()			SPI_CS_3_PORT &= ~SPI_CS_3_PIN
#define SPI_ADC_CS_DEASSERT()		SPI_CS_3_PORT |= SPI_CS_3_PIN

#define SPI_CS_4_DIR				DDRA
#define SPI_CS_4_PORT				PORTA
#define SPI_CS_4_PIN				(1<<0)
#define SPI_DES_CS_ASSERT()			SPI_CS_4_PORT &= ~SPI_CS_4_PIN
#define SPI_DES_CS_DEASSERT()		SPI_CS_4_PORT |= SPI_CS_4_PIN

#define SPI_CS_5_DIR				DDRA
#define SPI_CS_5_PORT				PORTA
#define SPI_CS_5_PIN				(1<<1)
#define SPI_APP_CS_ASSERT()			SPI_CS_5_PORT &= ~SPI_CS_5_PIN
#define SPI_APP_CS_DEASSERT()		SPI_CS_5_PORT |= SPI_CS_5_PIN

#define SPI_CS_6_DIR				DDRB
#define SPI_CS_6_PORT				PORTB
#define SPI_CS_6_PIN				(1<<0)
#define SPI_xCS_CS_ASSERT()			SPI_CS_6_PORT &= ~SPI_CS_6_PIN
#define SPI_xCS_CS_DEASSERT()		SPI_CS_6_PORT |= SPI_CS_6_PIN

#define SPI_CS_7_DIR				DDRD
#define SPI_CS_7_PORT				PORTD
#define SPI_CS_7_PIN				(1<<5)
#define SPI_xDCS_CS_ASSERT()		SPI_CS_7_PORT &= ~SPI_CS_7_PIN
#define SPI_xDCS_CS_DEASSERT()		SPI_CS_7_PORT |= SPI_CS_7_PIN

/*******************************************/

#define SSD1306_RST_DIR				DDRD
#define SSD1306_RST_PORT			PORTD
#define SSD1306_RST_PIN				(1<<7)

#define SSD1306_DC_DIR				DDRD
#define SSD1306_DC_PORT				PORTD
#define SSD1306_DC_PIN				(1<<4)

/*******************************************/

#define VS10xx_RST_DIR				DDRD
#define VS10xx_RST_PORT				PORTD
#define VS10xx_RST_PIN				(1<<0)

#define VS10xx_DREQ_DIR				DDRF
#define VS10xx_DREQ_PORT			PORTF
#define VS10xx_DREQ_PIN				(1<<0)

/*******************************************/

#define uSD_DC_DIR					DDRD
#define uSD_DC_PORT					PORTD
#define uSD_DC_PIN					(1<<1)

/*******************************************/
typedef struct {
	int16_t x_min;
	int16_t x_max;
	int16_t y_min;
	int16_t y_max;
}box_t;
/*******************************************/
/* Platform dependent definitions */

typedef uint8_t pio_t;

/*******************************************/
#endif /* DEF_H_ */
