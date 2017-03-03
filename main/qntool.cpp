#ifndef _GNU_SOURCE
	#define _GNU_SOURCE
#endif

#include "drivers/qn_programmer.h"
#include "drivers/bb_gpio.h"
#include "helper/timing.h"
#include "helper/Exception.h"

#include <fstream>
#include <iostream>
#include <stdio.h>
#include <string>
#include <unistd.h>
#include <getopt.h>

// https://learnbuildshare.wordpress.com/2013/05/29/beaglebone-black-digital-ouput/


static unsigned qn_enable_gpio = 66;
static unsigned qn_reset_gpio = 111;

int program_qn902x(std::string &file, std::string &port)
{
	try {
		QnProgrammer prog(port.c_str());
	    prog.setup();

	    pinMode(qn_enable_gpio, OUTPUT);
		pinMode(qn_reset_gpio, OUTPUT);

		digitalWrite(qn_enable_gpio, HIGH);

		digitalWrite(qn_reset_gpio, HIGH);
		delay_ms(100);
		digitalWrite(qn_reset_gpio, LOW);
		delay_ms(10);

	    std::cout << "Reseting qn902x..." << std::endl;
	    std::cout << "Connecting to Qn902x..." <<std::endl;
	    
	    prog.connect(5000);

	    std::cout << "Connected to qn902x" << std::endl;

	    prog.set_program_address(0x1000);
	    
	    if (!prog.sector_erase(0x0f))
	    	throw Exception("Error in erasing sector");
	    	
	    if (!prog.set_program_address(0x1100))
	    	throw Exception("Error in Setting program address");
	 
	    if (!prog.set_app_ram_addr(0x10000000))
	    	throw Exception("Error in Setting app ram address");

	    if (!prog.set_app_reset_addr(0x100000d4))
	    	throw Exception("Error in setting app reset address");

	    std :: cout << "Programming..." << std :: endl;

	    std :: fstream bin_file;

	    bin_file.open(file.c_str(), std::fstream::in);

	    if (!bin_file.is_open())
	    	throw Exception("Binary File can't be open");

	    char chunk[BIN_FILE_CHUNK_SIZE];

	    bin_file.seekg(0, std::fstream::end);
	    unsigned long bin_file_size = bin_file.tellg();
	    unsigned long bin_file_chunk_num = bin_file_size / BIN_FILE_CHUNK_SIZE;

	    bin_file.seekg(0, std::fstream::beg);

	    std::cout << "[ 0% ]";
	    std::cout.flush();

	    unsigned long i;
	    for (i=0; (i<bin_file_chunk_num) and (bin_file.good()); i++) {
	    	delay_ms(10);
	    	bin_file.read(chunk, BIN_FILE_CHUNK_SIZE);
	    	if (!prog.program(reinterpret_cast<uint8_t *>(chunk), BIN_FILE_CHUNK_SIZE))
	    		throw Exception("Error in programming");
	    	unsigned long percent = ((100*i)/bin_file_chunk_num);
	    	std::cout << '\r' << "[ " << percent << "% ]";
	    	std::cout.flush();

	    }

	    if (!bin_file.good())
	    	throw Exception("Binary File isn't good");

	    bin_file.read(chunk, bin_file_size % BIN_FILE_CHUNK_SIZE);

	    if (!prog.program(reinterpret_cast<uint8_t *>(chunk), bin_file_size % BIN_FILE_CHUNK_SIZE))
	    	throw Exception("Error in programming");

	    std::cout << '\r' << "[ 100% ]" << std::endl;
	    std::cout << "Programmed Successfully" << std::endl;
	    std::cout << "Rebooting the qn902x..." << std::endl;

	    prog.reboot();

	    std::cout << "DONE!!" << std::endl;
	}

	catch (Exception &e) {
		std::cerr << "EXCEPTION THROWN!: MESSAGE: ";
		std::cerr << e.what() << std::endl;
		return -1;
	}

    return 0;
}

struct option qn_prog_opt[] = {
	{ "file",  required_argument,  NULL,  'f'},
	{ "port",  required_argument,  NULL,  'p'},
	{  NULL,   required_argument,  NULL,   0 }
};

void usage()
{
	std::cout << "QnProgrammer Usage Guide" << std::endl;
	std::cout << "QnProgrammer [ --port PORT ] [ --file FILE ]" << std::endl;
	std::cout << "Use 'f' short option for \"file\" and 'p' short option for \"port\"" << std::endl;
}

int main(int argc, char *argv[])
{
	std::string filename;
	std::string portname;

	int opt=0;	
	while ((opt = getopt_long(argc, argv, "+:f:p:", qn_prog_opt, NULL)) >= 0) {
		switch (opt) {
		case 'f':
		{	
			filename = optarg;
			std::cout << "Binary File to be programmed: " << filename.c_str() << std::endl;
			break;
		}
		case 'p':
		{
			portname = optarg;
			std::cout << "Port of qn902x: " << portname.c_str() << std::endl;
			break;
		}
		// Executes when missing argument for an option is detected!
		case ':':
		{
			std::cerr << "Missing argument for required argument" << std::endl;
			usage();
			return -1;

			break;
		}
		// Executes when non-recognized option character is seen!
		case '?':
		default:
		{
			usage();
			return -1;

			break;
		}
		}
	}

	if (clock_init()) {
		std::cerr << "Error" << std::endl;
		return -1;
	}

	if ((portname.empty()) or (filename.empty())) {
		std::cerr << "Insufficient arguments" << std::endl;
		usage();
		return -1;
	}

	if (program_qn902x(filename, portname))
		return -1;

	return 0;
}