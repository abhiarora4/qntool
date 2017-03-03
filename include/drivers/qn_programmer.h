#ifndef _QN_PROGRAMMER_H_
#define _QN_PROGRAMMER_H_

#include "helper/serial.h"
#include <stdint.h>


#define B_C_CMD						0x33
#define SET_BR_CMD					0x34
#define SET_FLASH_CLK_CMD			0x35
#define RD_BL_VER_CMD				0x36
#define RD_CHIP_ID_CMD				0x37
#define RD_FLASH_ID_CMD				0x38
#define SET_APP_LOC_CMD				0x39
#define SETUP_FLASH_CMD				0x3A
#define SET_ST_ADDR_CMD				0x3B
#define SET_APP_SIZE_CMD			0x3C
#define SET_APP_CRC_CMD				0x3E
#define SET_APP_IN_FLASH_ADDR_CMD	0x40
#define SE_FLASH_CMD 				0x42
#define BE_FLASH_CMD				0x43
#define CE_FLASH_CMD				0x44
#define PROGRAM_CMD					0x45
#define RD_CMD 						0x46
#define VERIFY_CMD 					0x47
#define PROTECT_CMD 				0x48
#define RUN_APP_CMD 				0x49
#define REBOOT_CMD 					0x4A
#define WR_RANDOM_DATA_CMD			0x4B
#define SET_APP_IN_RAM_ADDR_CMD 	0x4C
#define SET_APP_RESET_ADDR_CMD 		0x4D

#define CONFIRM_ACK					0x01
#define CONFIRM_NACK				0x02

#define RESULT_SUCCESSFULL			0x03
#define RESULT_FAILED				0x04


#define DEFAULT_CONNECTION_TIMEOUT	5000  // milli seconds
#define DEFAULT_BLOCK_READ_TIMEOUT	5000  // milli seconds


#define TARGET_RAM					0x00
#define TARGET_FLASH				0x01


// Important Macro!
#define BIN_FILE_CHUNK_SIZE			64


class QnProgrammer {
private:
	Serial _serial;
	unsigned _baud_rate;
	unsigned _clock;

	static uint32_t calculate_uart_register(unsigned clock, unsigned baudrate);

	unsigned write(uint8_t *buf, uint8_t len);
	unsigned block_read(uint8_t *buf, unsigned len, unsigned timeout=DEFAULT_BLOCK_READ_TIMEOUT);
	
public:

	QnProgrammer(const char *dev) : _serial(dev) { }
	~QnProgrammer() { }

	int setup(unsigned baud_rate=115200, unsigned clock=16000000);
	void connect(unsigned timeout=DEFAULT_CONNECTION_TIMEOUT);

	int send_cmd(unsigned cmd, uint8_t *data, unsigned len);
	int send_cmd(unsigned cmd) { return send_cmd(cmd, nullptr, 0); }

	bool read_pkt(bool only_confirm, uint8_t *buf, unsigned len);
	bool read_pkt(bool only_confirm=false) { return read_pkt(only_confirm, nullptr, 0); }

	bool reboot(void);
	bool set_load_target(uint32_t target);
	bool get_bootloader_version(void);
	bool get_chip_id(void);
	bool get_flash_id(void);
	
	bool set_program_address(uint32_t address);
	bool sector_erase(uint32_t sector_count);
	bool set_app_ram_addr(uint32_t address);
	bool set_app_reset_addr(uint32_t address);

	bool program(uint8_t *data, unsigned len);

};


#endif /* _QN_PROGRAMMER_H_ */