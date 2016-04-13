#ifndef __I2C_H_
	#define __I2C_H_
	/* I2C device addresses */
	#define EEPR_DEV_ADDR	0x00a0
	#define CLK_DEV_ADDR 0x00d0
	#define DA_DEV_ADDR 0x0098

	#define ADDR_CLK_REG 0x8000
	// RTC and RAM Address Map
	// 00h  seconds
	// 01h  minutes
	// 02h  hours
	// 03h  day
	// 04h  date
	// 05h  month
	// 06h  year
	// 07h  control
	// 08h-3fh  RAM 56 byte

	/*-------------------------------------------------------------------------*/
	// EEPROM
	#define READ		0
	#define WRITE		1
	#define VERIFY	2
	//#define EEPROM_24LC64
	#define EEPROM_24LC128
	//#define PAGE_SIZE 16	// EEPROM lap merete 16 word a 24LC64-en 4 k word
	#define PAGE_SIZE 32	// EEPROM lap merete 32 word a 24LC128-en 8 k word
	#define EEPROM_WRITE_ERR                   52
	/*-------------------------------------------------------------------------*/

	// vk_napok
	#define VK_NAPOK_NINCS                      0
	#define VK_NAPOK_MINDEN                     1
	#define VK_NAPOK_HETFOE                     2
	#define VK_NAPOK_KEDD                       3
	#define VK_NAPOK_SZERDA                     4
	#define VK_NAPOK_CSUTOERT                   5
	#define VK_NAPOK_PENTEK                     6
	#define VK_NAPOK_SZOMBAT                    7
	#define VK_NAPOK_VASARNAP                   8
	#define VK_NAPOK_HETKOZNAP                  9

void i2c_force_stop(void);

#endif
