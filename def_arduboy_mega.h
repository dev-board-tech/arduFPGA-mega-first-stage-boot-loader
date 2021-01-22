/*
 * def_arduboy_mega.h
 *
 * For anyone to change it as he wish.
 *  Author: MorgothCreator
 */ 


#ifndef DEF_ARDUBOY_MEGA_H_
#define DEF_ARDUBOY_MEGA_H_

#define F_CPU								16000000
#define FLASH_SIZE							(0x10000 - 0x1000)
#define EEP_SIZE							0x400
#define RAM_SIZE							0xA00


/*
 This ports does not exist in the original micro controller.
 This ports are used by boot loader, but can be used even by the application.
 */
#define PINA								_SFR_IO8(0x00)
#define DDRA								_SFR_IO8(0x01)
#define PORTA								_SFR_IO8(0x02)

#define FLASH_APP_EXPLORER_START_ADDR		0x100000
#define FLASH_APP_USER_START_ADDR			0x140000

#define FLASH_APP_ROM_OFFSET				0x00000
#define FLASH_APP_ROM_ZIZE					0x20000
#define FLASH_APP_RAM_OFFSET				0x20000
#define FLASH_APP_RAM_ZIZE					0x20000
#define FLASH_APP_EEP_OFFSET				0x40000
#define FLASH_APP_EEP_SIZE					0x00400
#define FLASH_APP_PATH_OFFSET				0x42000
#define FLASH_APP_MEMORY_SIZES_OFFSET		0x43000
#define FLASH_APP_MEMORY_SIZES_PGM_OFFSET	0x4
#define FLASH_APP_MEMORY_SIZES_RAM_OFFSET	0x8
#define FLASH_APP_MEMORY_SIZES_EEP_OFFSET	0xC

/* These IO's are placed in SRAM data space, we need to subtract 0x20 value to address them correctly. */
#define SEC_REG_ADDR						_SFR_IO8(0xD8)
#define F_CNT_L								_SFR_IO8(0xDB)
#define F_CNT_H								_SFR_IO8(0xDC)
#define F_DATA_L							_SFR_IO8(0xDD)
#define F_DATA_H							_SFR_IO8(0xDE)
#define BOOT_STAT							_SFR_IO8(0xDF)

#define BOOT_STAT_FLASH_APP_NR				(1 << 0)
#define BOOT_STAT_EEP_EDITED				(1 << 1)
#define BOOT_STAT_NMI_INT_ENABLE			(1 << 2)
#define BOOT_STAT_APP_PGM_WR_EN				(1 << 3)
#define BOOT_STAT_IO_RST					(1 << 4)
#define BOOT_STAT_DEBUG_EN					(1 << 7)

#define BOOT_VECTOR_MAIN					(0x7802)
#define BOOT_VECTOR_SET_SERVICE_VECT		(0x7803)
#define BOOT_VECTOR_FLASH_WRITE				(0x7804)
#define BOOT_VECTOR_FLASH_DES_ERASE			(0x7805)
#define BOOT_VECTOR_FLASH_DES_WRITE			(0x7806)

#define KBD_DIR						DDRA
#define KBD_PORT					PORTA
#define KBD_IN						PINA

#define KBD_U_DIR					DDRA
#define KBD_U_PORT					PORTA
#define KBD_U_IN					PINA
#define KBD_U_PIN					(1<<7)

#define KBD_D_DIR					DDRA
#define KBD_D_PORT					PORTA
#define KBD_D_IN					PINA
#define KBD_D_PIN					(1<<6)

#define KBD_B_DIR					DDRA
#define KBD_B_PORT					PORTA
#define KBD_B_IN					PINA
#define KBD_B_PIN					(1<<5)

#define KBD_A_DIR					DDRA
#define KBD_A_PORT					PORTA
#define KBD_A_IN					PINA
#define KBD_A_PIN					(1<<4)

#define KBD_INT_DIR					DDRA
#define KBD_INT_PORT				PORTA
#define KBD_INT_IN					PINA
#define KBD_INT_PIN					(1<<3)

#define KBD_L_DIR					DDRA
#define KBD_L_PORT					PORTA
#define KBD_L_IN					PINA
#define KBD_L_PIN					(1<<2)

#define KBD_R_DIR					DDRA
#define KBD_R_PORT					PORTA
#define KBD_R_IN					PINA
#define KBD_R_PIN					(1<<1)

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
#define SPI_ST7735_CS_ASSERT()		SPI_CS_1_PORT &= ~SPI_CS_1_PIN
#define SPI_ST7735_CS_DEASSERT()	SPI_CS_1_PORT |= SPI_CS_1_PIN

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
#define VS10xx_xCS_ASSERT()			SPI_xCS_CS_ASSERT()
#define VS10xx_xCS_DEASSERT()		SPI_xCS_CS_DEASSERT()

#define SPI_CS_7_DIR				DDRD
#define SPI_CS_7_PORT				PORTD
#define SPI_CS_7_PIN				(1<<5)
#define SPI_xDCS_CS_ASSERT()		SPI_CS_7_PORT &= ~SPI_CS_7_PIN
#define SPI_xDCS_CS_DEASSERT()		SPI_CS_7_PORT |= SPI_CS_7_PIN
#define VS10xx_xDCS_ASSERT()		SPI_xDCS_CS_ASSERT()
#define VS10xx_xDCS_DEASSERT()		SPI_xDCS_CS_DEASSERT()

/*******************************************/

#define DISPLAY_RST_DIR				DDRD
#define DISPLAY_RST_PORT			PORTD
#define DISPLAY_RST_PIN				(1<<7)

#define DISPLAY_DC_DIR				DDRD
#define DISPLAY_DC_PORT				PORTD
#define DISPLAY_DC_PIN				(1<<4)

#define SSD1331_RST_DIR				DISPLAY_RST_DIR
#define SSD1331_RST_PORT			DISPLAY_RST_PORT
#define SSD1331_RST_PIN				DISPLAY_RST_PIN

#define SSD1331_DC_DIR				DISPLAY_DC_DIR
#define SSD1331_DC_PORT				DISPLAY_DC_PORT
#define SSD1331_DC_PIN				DISPLAY_DC_PIN

#define SSD1306_RST_DIR				DISPLAY_RST_DIR
#define SSD1306_RST_PORT			DISPLAY_RST_PORT
#define SSD1306_RST_PIN				DISPLAY_RST_PIN

#define SSD1306_DC_DIR				DISPLAY_DC_DIR
#define SSD1306_DC_PORT				DISPLAY_DC_PORT
#define SSD1306_DC_PIN				DISPLAY_DC_PIN

#define ST7735_RST_DIR				DISPLAY_RST_DIR
#define ST7735_RST_PORT				DISPLAY_RST_PORT
#define ST7735_RST_PIN				DISPLAY_RST_PIN

#define ST7735_DC_DIR				DISPLAY_DC_DIR
#define ST7735_DC_PORT				DISPLAY_DC_PORT
#define ST7735_DC_PIN				DISPLAY_DC_PIN

#define ST7789_RST_DIR				DISPLAY_RST_DIR
#define ST7789_RST_PORT				DISPLAY_RST_PORT
#define ST7789_RST_PIN				DISPLAY_RST_PIN

#define ST7789_DC_DIR				DISPLAY_DC_DIR
#define ST7789_DC_PORT				DISPLAY_DC_PORT
#define ST7789_DC_PIN				DISPLAY_DC_PIN

#define SPI_ST7735_BLK_ASSERT()		SPI_CS_5_PORT &= ~SPI_CS_5_PIN
#define SPI_ST7735_BLK_DEASSERT()	SPI_CS_5_PORT |= SPI_CS_5_PIN
/*******************************************/

#define VS10xx_RST_DIR				DDRD
#define VS10xx_RST_PORT				PORTD
#define VS10xx_RST_PIN				(1<<0)

#define VS10xx_DREQ_DIR				DDRF
#define VS10xx_DREQ_PORT			PORTF
#define VS10xx_DREQ_IN				PINF
#define VS10xx_DREQ_PIN				(1<<0)

/*******************************************/

#define uSD_CD_DIR					DDRD
#define uSD_CD_PORT					PORTD
#define uSD_CD_IN					PIND
#define uSD_CD_PIN					(1<<1)

/*******************************************/

#define PWM_VOLUME_PORT				PORTA
#define PWM_VOLIME_PIN_gp			(2)
#define PWM_VOLIME_PIN_gm			(0x03 << PWM_VOLIME_PIN_gp)

#define DISC_USR_KBD_PORT			PORTA
#define DISC_USR_KBD_PIN_bp			(4)
#define DISC_USR_KBD_PIN_bm			(1 << DISC_USR_KBD_PIN_bp)

#define USB_NTSC_EN_PORT			PORTA
#define USB_NTSC_EN_PORT_bp			(5)
#define USB_NTSC_EN_PORT_bm			(1 << USB_NTSC_EN_PORT_bp)

#define AUD_NTSC_EN_PORT			PORTA
#define AUD_NTSC_EN_PORT_bp			(6)
#define AUD_NTSC_EN_PORT_bm			(1 << AUD_NTSC_EN_PORT_bp)

/*******************************************/
/*******************************************/
/* Platform dependent definitions */

typedef uint8_t pio_t;

/*******************************************/


#endif /* DEF_ARDUBOY_MEGA_H_ */