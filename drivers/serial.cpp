#include "helper/serial.h"
#include "helper/print.h"

#include <iostream>
#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <stdlib.h>

// Feature Test Macro
#define _BSD_SOURCE
#define _SVID_SOURCE

#include <unistd.h>
#include <termios.h>
#include <fcntl.h>

#include <sys/signal.h>
#include <sys/ioctl.h>
#include <sys/types.h>


/*
 * Read more about Termios: http://man7.org/linux/man-pages/man3/termios.3.html
 */

int Serial::map_to_termios_buad(unsigned baud_rate)
{
	if (baud_rate == 0)
		return B0;
	else if (baud_rate == 50)
		return B50;
	else if (baud_rate == 75)
		return B75;
	else if (baud_rate == 110)
		return B110;
	else if (baud_rate == 134)
		return B134;
	else if (baud_rate == 150)
		return B150;
	else if (baud_rate == 200)
		return B200;
	else if (baud_rate == 300)
		return B300;
	else if (baud_rate == 600)
		return B600;
	else if (baud_rate == 1200)
		return B1200;
	else if (baud_rate == 1800)
		return B1800;
	else if (baud_rate == 2400)
		return B2400;
	else if (baud_rate == 4800)
		return B4800;
	else if (baud_rate == 9600)
		return B9600;
	else if (baud_rate == 19200)
		return B19200;
	else if (baud_rate == 38400)
		return B38400;
	else if (baud_rate == 57600)
		return B57600;
	else if (baud_rate == 115200)
		return B115200;
	else if (baud_rate == 230400)
		return B230400;
	else 
		return B115200;
}

unsigned long Serial::map_to_c_cflag(uint8_t cfg, bool flow_control)
{
	// Parity
	unsigned long c_cflag = CLOCAL | CREAD;

	uint8_t parity = (cfg >> 4) & 0b11;
	uint8_t stop_bits = (cfg >> 3) & 0b1;
	uint8_t char_size = (cfg >> 1) & 0b11;

	if (parity == 0b10) // Even Parity
		c_cflag |= PARENB;
	else if (parity == 0b11)
		c_cflag |= PARENB | PARODD;

	if (stop_bits == 1)
		c_cflag |= CSTOPB;

	if (char_size == 0b00)
		c_cflag |= CS5;
	else if (char_size == 0b01)
		c_cflag |= CS6;
	else if (char_size == 0b10)
		c_cflag |= CS7;
	else if (char_size == 0b11)
		c_cflag |= CS8;
	else	// Default Case
		c_cflag |= CS8;

	if (flow_control)
		c_cflag |= CRTSCTS;

	return c_cflag;
}


Serial::Serial(const char *dev_path) : Print()
{
	fd = ::open(dev_path, O_RDWR | O_NOCTTY | O_SYNC);
}

int Serial::open(const char *dev_path)
{
	fd = ::open(dev_path, O_RDWR | O_NOCTTY);
	if (fd < 0)
		return -1;
	if (!isatty(fd)) {
		::close(fd);
		fd=-1;
		return -1;
	}
	return 0;
}

void Serial::end()
{
	if (fd < 0)
		return;
	::close(fd);
	fd=-1; // Make it invalid File descriptor
}


int Serial::begin(unsigned baud_rate)
{
    struct termios setting, verify_setting;
	if (fd < 0)
		return -1;

	speed_t termios_buad = map_to_termios_buad(baud_rate);
        
    memset(&setting, 0, sizeof(setting));
    memset(&verify_setting, 0, sizeof(verify_setting));
    setting.c_cflag = CS8 | CLOCAL | CREAD;
    setting.c_iflag = IGNBRK | IGNPAR;
    setting.c_oflag = 0;
    setting.c_lflag = 0;      // Noncanonical mode
         
    setting.c_cc[VTIME] = 0;  // Timeout in deciseconds for noncanonical read
    setting.c_cc[VMIN] = 0;   // Minimum number of characters for noncanonical read

    if (cfsetospeed(&setting, termios_buad))
    	return -1;
 	if (cfsetispeed(&setting, termios_buad))
 		return -1;

    if (tcflush(fd, TCIOFLUSH))			  // Flush non-tramsmitted and unread data!
    	return -1;
    if (tcsetattr(fd, TCSANOW, &setting)) // Set attribute now!
    	return -1;
    if (tcgetattr(fd, &verify_setting))	   // Get attribute now!
    	return -1;
    if (memcmp(&setting, &verify_setting, sizeof(setting)))
    	return -1;
    return 0;
}

// Waits for the transmission of outgoing serial data to complete.
void Serial::flush()
{
	fsync(fd);
	while (tcdrain(fd)); // Wait until all data previously written to the serial line indicated by fd has been sent.
}

int Serial::available()
{
	int in_bytes = 0;
	if (ioctl(fd, FIONREAD, &in_bytes))
		return 0;
	return in_bytes;
}


int Serial::read()
{
	uint8_t byte;
	if (::read(fd, &byte, sizeof(byte)) == 0)
		return -1;
	return byte;
}	

bool Serial::find(const char *target)
{
	char message[MAX_INPUT_QUEUE_LEN+1];
	int num_read=0;
	int target_len = strlen(target);

	while (true) {
		if (available() <= 0)
			continue;

		if (num_read >= MAX_INPUT_QUEUE_LEN)
			return false;

		message[num_read++] = read();
		message[num_read] = '\0';

		char *look_ptr = (message + num_read - target_len) < message ? message : (message + num_read - target_len);
		if (!strcmp(look_ptr, target))
			break;
	}
	return true;
}

int Serial::write(uint8_t c)
{
	return ::write(fd, &c, 1);
}




// #define __MAIN__

#ifdef __MAIN__

const char *str_ok = "OK";
#define MAX_MESSAGE_SIZE	1000

int readTillOK(Serial& serial)
{
	char message[MAX_MESSAGE_SIZE+1];
	int num_read=0;
	while (true) {
		if (serial.available() <= 0)
			continue;
		if (num_read >= MAX_MESSAGE_SIZE)
			return -1;
		message[num_read++] = serial.read();
		message[num_read] = '\0';
		char *potential_ok = (message + num_read - 2) < message ? message : (message + num_read - 2);
		if (!strcmp(potential_ok, str_ok))
			break;
	}
	std :: cout << "Message: " << message << std :: endl;
	return 0;
}

int main(int argc, char *argv[])
{
	if (argc < 2)
		return -1;

	Serial serial(argv[1]);
	serial.set_line_ending(Print :: WINDOWS_STYLE_LINE_ENDING);

	if (serial.begin(115200)) {
		std :: cerr << "Fail to connect" << std :: endl;
		perror("Failed");
		return -1;
	}

	serial.println("printmodulus 100 set drop");
	serial.find("OK"); //readTillOK(Serial);
	serial.println("printmask");
	serial.find("OK"); //readTillOK(Serial);
	serial.println("gyror_trigger gyrop_trigger or set drop");
	serial.find("OK"); //readTillOK(Serial);
	serial.println("printtrigger printmask set drop");
	serial.find("OK"); //readTillOK(Serial);
	//Serial.print("1 compass.p\r\n");

	//Serial.print("restart\r\n");
	//Serial.find("OK"); //readTillOK(Serial);

	//Serial.print("reset\r\n");
	//Serial.find("OK"); //readTillOK(Serial);
	//usleep(100000);

	//Serial.print("roll di.\r\n");
	//Serial.find("OK"); //readTillOK(Serial);

	//serial.println("yaw di.");
	//serial.find("OK"); //readTillOK(Serial);

	//Serial.find("OK"); //readTillOK(Serial);
	while (true) {
		if (serial.available() <= 0)
			continue;
		std :: cout << char(serial.read());
	}
	return 0;	
}

#endif