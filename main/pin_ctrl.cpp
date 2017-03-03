#include "drivers/bb_gpio.h"
#include "helper/Exception.h"

#include <iostream>
#include <stdio.h>
#include <unistd.h>
#include <string.h>

#include <sys/stat.h>
#include <getopt.h>


struct option gpio_options[] = {
    { "pin",        required_argument,     NULL,   'p' },
    { "direction",  required_argument,     NULL,   'd' },
    { "value",      required_argument,     NULL,   'v' },
    { NULL,         0                ,     NULL,    0  }
};

void usage()
{
    std::cout << "OPENWRT GPIO Guide" << std::endl;
    std::cout << "[ --pin PIN_NUMBER ] [ --direction [O | I] ] [ --value [0 | 1] ]" << std::endl;
}

 
int main(int argc, char *argv[]) 
{
    unsigned pin;
    bool pin_specified=false;
    pin_mode_e default_mode = OUTPUT;
    bool default_value = false;

    int opt;

    while ((opt = getopt_long(argc, argv, "+:p:d:v:", gpio_options, NULL)) >= 0) {
        switch (opt) {
        case 'p':
        {
            if (sscanf(optarg, "%u", &pin) != 1) {
                usage();
                return -1;
            }
            pin_specified = true;
            break;
        }
        case 'd':
        {
            if ((*optarg == 'i') || (*optarg == 'I')) 
                default_mode = INPUT;
            break;
        }
        case 'v':
        {
            if ((*optarg == '1') || (*optarg == 'h') || (*optarg == 'H')) 
                default_value = true;
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
    if (!pin_specified) {
        std::cerr << "Pin number is required" << std::endl;
        usage();
        return -1;
    }

    try {
        std::cout << "Pin: " << pin << std::endl;
        pinMode(pin, default_mode);
        std::cout << "Direction: " << (default_mode == INPUT ? "INPUT" : "OUTPUT") << std::endl;
        if (default_mode == OUTPUT) {
            digitalWrite(pin, default_value);
            std::cout << "Value: " << (default_value ? "HIGH" : "LOW") << std::endl;
        } else {
            std::cout << "Value: " << (digitalRead(pin) ? "HIGH" : "LOW") << std::endl;
        }
    }
    catch (Exception &e) {
        std::cerr << "FAILURE EXCEPTION: " << e.what() << std::endl;

    }

    return 0;
}