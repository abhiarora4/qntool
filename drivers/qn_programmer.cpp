#include "drivers/qn_programmer.h"
#include "drivers/QnPacket.h"
#include "helper/timing.h"
#include "helper/Exception.h"
#include "drivers/bb_gpio.h"


#include <iostream>
#include <string>
#include <string.h>
#include <stdint.h>

// Bootloader of qn902x works in two modes:
// 1. ISP Mode
// 2. Load Mode

/*
 * Bootloader Packet Format
 * HEADCODE(1 byte) 	COMMAND(1 byte)		DATA_LENGTH(3 bytes)		DATA(n bytes) 	CRC(2 bytes)
 *
 *
 */


void convertToBuf(uint32_t val, uint8_t *p_buf)
{
	p_buf[0] = (val & 0xFF);
	p_buf[1] = ((val >> 8) & 0xFF);
	p_buf[2] = ((val >> 16) & 0xFF);
	p_buf[3] = ((val >> 24) & 0xFF);
}
	
uint32_t QnProgrammer::calculate_uart_register(unsigned clk, unsigned baudrate)
{
	unsigned long tmp = 16 * baudrate;
    unsigned long inter_div = clk / tmp;
    unsigned long frac_div = (((clk - (inter_div * tmp)) * 64) + tmp / 2) / tmp;
    return (inter_div << 8) + frac_div;
}

unsigned QnProgrammer::write(uint8_t *buf, uint8_t len)
{
	unsigned i=0;
	while (i++ < len)
		_serial.write(*buf++);
	return i;
}

unsigned QnProgrammer::block_read(uint8_t *buf, unsigned len, unsigned timeout)
{
	uint64_t ms = millis();
	unsigned i;
	for (i=0; i<len; i++) {
		while (!_serial.available()) {
			if ((millis() - ms) > timeout)
				throw Exception("Timeout in reading");
		}
		buf[i] = _serial.read();
	}
	return i;
}

int QnProgrammer::send_cmd(unsigned cmd, uint8_t *data, unsigned len)
{
	// Log Any information over here!
	QnPacket pkt;
	if (pkt.build_packet(cmd, data, len))
		throw Exception("Packet Format Error");
	// pkt.print();
	_serial.flush();
	return write(pkt.get_pkt(), pkt.get_pktlen());
}


bool QnProgrammer::read_pkt(bool only_confirm, uint8_t *buf, unsigned len)
{
	uint8_t sync, start_byte;
	block_read(&sync, 1);

	if (sync != CONFIRM_ACK)
		throw Exception("Invalid Response Byte");

	if (only_confirm)
		return true;
	
	block_read(&start_byte, 1);

	if (start_byte == RESULT_FAILED)
		return false;
	if (start_byte == RESULT_SUCCESSFULL)
		return true;
	
	if (start_byte != HEAD_CODE)
		throw Exception("Invalid Received Packet Format");

	QnPacket p;
	uint8_t cmd;
	uint8_t data_len_buf[DATA_LEN_FIELD_SIZE_IN_PKT];
	// uint16_t crc;

	block_read(&cmd, 1);
	block_read(data_len_buf, DATA_LEN_FIELD_SIZE_IN_PKT);

	unsigned datalen = QnPacket::decode_datalen(data_len_buf);

	uint8_t pkt[TOTAL_PKT_LEN(datalen)];

	pkt[CMD_OFFSET_IN_PKT] = cmd;
	pkt[DATA_LEN_OFFSET_IN_PKT] = data_len_buf[0];
	pkt[DATA_LEN_OFFSET_IN_PKT + 1] = data_len_buf[1];
	pkt[DATA_LEN_OFFSET_IN_PKT + 2] = data_len_buf[2];

	block_read(pkt + DATA_OFFSET_IN_PKT, datalen);
	block_read(pkt + CRC_OFFSET_IN_PKT(datalen), CRC_FIELD_LEN_SIZE_IN_PKT);

	if (!p.decode(pkt).isCRCMatched())
		throw Exception("Invalid Received Packet CRC");

	if (len < datalen)
		throw Exception("Invalid Arguments");

	if (buf == nullptr)
		return true;

	memcpy(buf, pkt, datalen);
	return true;
}


int QnProgrammer::setup(unsigned baud_rate, unsigned clock)
{
	_baud_rate = baud_rate;
	_clock = clock;
	return 0; 
}

void QnProgrammer::connect(unsigned timeout)
{
	if (_serial.begin(9600)) {
		std :: cerr << "BuadRate Failed" << std :: endl;
		// throw Exception("Baudrate Failed");
	}
	while (_serial.read() != -1);
	while (true) {
		uint64_t ms = millis();
		while (_serial.available() == 0) {
			if ((millis() - ms) > timeout)
				throw Exception("Timeout in Connecting");
			_serial.write(B_C_CMD);
			delay_ms(5);
		}
		int c = _serial.read();
		if (c == CONFIRM_ACK) // Connected!
			break;
		if (c == CONFIRM_NACK)
			throw Exception("Qn rejected connection with bootloader");

		// Hard Fix!
		digitalWrite(111, HIGH);
		delay_ms(100);
		digitalWrite(111, LOW);
		delay_ms(10);
	}

	// Time to switch Buadrate!
	uint32_t uart_reg = calculate_uart_register(_clock, _baud_rate);

	uint8_t data[4];
	convertToBuf(uart_reg, data);

	send_cmd(SET_BR_CMD, data, sizeof(data));

	read_pkt(true);
		
	if (_serial.begin(_baud_rate)) {
		std :: cerr << "BuadRate Failed" << std :: endl;
		// throw Exception("Baudrate Failed");
	}
	delay_ms(100);

	send_cmd(SET_BR_CMD, data, sizeof(data));

	read_pkt(true);	
}

bool QnProgrammer::reboot()
{
	send_cmd(REBOOT_CMD);
	return read_pkt(true);
}

bool QnProgrammer::set_load_target(uint32_t target)
{
	uint8_t data[4];
	convertToBuf(target, data);
	send_cmd(SET_APP_LOC_CMD, data, sizeof(data));
	return read_pkt(true);
}

bool QnProgrammer::get_bootloader_version()
{
	send_cmd(RD_BL_VER_CMD);
	return read_pkt();
}

bool QnProgrammer::get_chip_id()
{
	send_cmd(RD_CHIP_ID_CMD);
	return read_pkt();
}

bool QnProgrammer::get_flash_id()
{
	send_cmd(RD_FLASH_ID_CMD);
	return read_pkt();
}

bool QnProgrammer::program(uint8_t *data, unsigned len)
{
	send_cmd(PROGRAM_CMD, data, len);
	return read_pkt(false, nullptr, 0);
}

bool QnProgrammer::set_program_address(uint32_t address)
{
	uint8_t data[4];
	convertToBuf(address, data);
	send_cmd(SET_ST_ADDR_CMD, data, 4);
	return read_pkt(true);
}

bool QnProgrammer::sector_erase(uint32_t sector_count)
{
	uint8_t data[4];
	convertToBuf(sector_count, data);
	send_cmd(SE_FLASH_CMD, data, 4);
	return read_pkt(false, nullptr, 0);
}

bool QnProgrammer::set_app_ram_addr(uint32_t address)
{
	uint8_t data[4];
	convertToBuf(address, data);
	send_cmd(SET_APP_IN_RAM_ADDR_CMD, data, 4);
	return read_pkt(false, nullptr, 0);
}

bool QnProgrammer::set_app_reset_addr(uint32_t address)
{
	uint8_t data[4];
	convertToBuf(address, data);
	send_cmd(SET_APP_RESET_ADDR_CMD, data, 4);
	return read_pkt(false, nullptr, 0);
}

// #define __MAIN__


#ifdef __MAIN__
int main(int argc, char *argv[])
{
	if (argc < 2) {
		std :: cerr << "Usage: [PORT]" << std :: endl;
		return -1;	
	}

	if (clock_init()) {
		std :: cerr << "Error" << std :: endl;
		return -1;
	}

    QnProgrammer prog(argv[1]);
    prog.setup();
    if (prog.connect(5000)) {
    	std :: cerr << "Failed to connect" << std :: endl;
    	return -1;
    }

    if (prog.set_program_address(0x1000)) {
    	std :: cerr << "Error in Setting program address" << std :: endl;
    	return -1;
    }
    
    if (prog.sector_erase(0x0f)) {
    	std :: cerr << "Can't Erase Sector" << std :: endl;
    	return -1;
    }
    if (prog.set_program_address(0x1100)) {
    	std :: cerr << "Error in Setting program address" << std :: endl;
    	return -1;
    }


    if (prog.set_app_ram_addr(0x10000000)) {
    	std :: cerr << "Error in Setting app ram address" << std :: endl;
    	return -1;
    }

    if (prog.set_app_reset_addr(0x100000d4)) {
    	std :: cerr << "Error in Setting app ram address" << std :: endl;
    	return -1;
    }

    std :: cout << "Programming..." << std :: endl;

    std :: fstream bin_file;

    bin_file.open(argv[2], std::fstream::in);

    if (!bin_file.is_open()) {
    	std :: cerr << "Binary File can't be open" << std :: endl;
    	return -1;
    }

    char chunk[BIN_FILE_CHUNK_SIZE];

    bin_file.seekg(0, std::fstream::end);
    unsigned long bin_file_size = bin_file.tellg();
    unsigned long bin_file_chunk_num = bin_file_size / BIN_FILE_CHUNK_SIZE;

     bin_file.seekg(0, std::fstream::beg);

    unsigned long i;
    for (i=0; (i<bin_file_chunk_num) and bin_file.good(); i++) {
    	delay_ms(10);
    	bin_file.read(chunk, BIN_FILE_CHUNK_SIZE);
    	if (prog.program(reinterpret_cast<uint8_t *>(chunk), BIN_FILE_CHUNK_SIZE)) {
    		std :: cerr << "Error in programming" << std :: endl;
    		return -1;
    	}
    }
    if (!bin_file.good()) {
    	std :: cerr << "Error reading binary file" << std :: endl;
    	return -1;
    }

    bin_file.read(chunk, bin_file_size % BIN_FILE_CHUNK_SIZE);
    prog.program(reinterpret_cast<uint8_t *>(chunk), bin_file_size % BIN_FILE_CHUNK_SIZE);
    std :: cout << "Correctly set program address" << std :: endl;

    prog.reboot();
    return 0;
}

#endif