#include "helper/Exception.h"
#include "drivers/bb_gpio.h"

#include <fstream>
#include <iostream>
#include <stdio.h>
#include <unistd.h>
#include <string.h>

#include <sys/stat.h>
#include <getopt.h>

#define MAX_GPIO_PATH_STRING_SIZE           128
 
const char gpio_export_path[] = "/sys/class/gpio/export";

// Read: http://falsinsoft.blogspot.in/2012/11/access-gpio-from-linux-user-space.html


bool isFileExist(const char *path)
{
    struct stat buf;
    return (stat(path, &buf) == 0);
}


void enable_file_exception(std::fstream &f)
{
    return;
    std::fstream::iostate exceptionMask = f.exceptions() | std::ios::failbit | std::fstream::badbit;
    f.exceptions(exceptionMask);
}

void export_gpio(unsigned pin)
{
    std::fstream export_gpio;
    enable_file_exception(export_gpio);
    export_gpio.open(gpio_export_path, std::fstream::binary | std::fstream::app | std::fstream::out);
    if (!export_gpio.is_open())
        throw Exception("Export failed");
    export_gpio << pin;
    export_gpio.flush();
    export_gpio.close();
}

void pinMode(unsigned pin, pin_mode_e mode)
{   
    char path[MAX_GPIO_PATH_STRING_SIZE + 1];
    sprintf(path, "/sys/class/gpio/gpio%u/direction", pin);

    if (!isFileExist(path))
        export_gpio(pin);

    std::fstream dir_file;
    enable_file_exception(dir_file);
    dir_file.open(path, std::fstream::binary | std::fstream::app | std::fstream::out);
    if (!dir_file.is_open())
        throw Exception("Direction failed");

    if (mode == INPUT)
        dir_file << "in";
    else
        dir_file << "out";

    dir_file.flush();
    dir_file.close();
}

void digitalWrite(unsigned pin, bool value)
{   
    char path[MAX_GPIO_PATH_STRING_SIZE + 1];
    sprintf(path, "/sys/class/gpio/gpio%u/value", pin);

    if (!isFileExist(path)) 
        pinMode(pin, OUTPUT);

    std::fstream value_file;
    enable_file_exception(value_file);
    value_file.open(path, std::fstream::binary | std::fstream::app | std::fstream::out);

    if (!value_file.is_open())
        throw Exception("Write Failed");

    if (value)
        value_file << 1;
    else
        value_file << 0;

    value_file.flush();
    value_file.close();
}

bool digitalRead(unsigned pin)
{   
    int value;
    char path[MAX_GPIO_PATH_STRING_SIZE + 1];
    sprintf(path, "/sys/class/gpio/gpio%u/value", pin);

    if (!isFileExist(path))
        pinMode(pin, INPUT);

    std::fstream value_file;
    enable_file_exception(value_file);
    value_file.open(path, std::fstream::binary | std::fstream::app | std::fstream::in);

    if (!value_file.is_open())
        throw Exception("Read Failed");

    value_file.seekg (0, value_file.beg);
    value_file >> value;

    return (value != 0);
}

