#ifndef _BB_GPIO_H_
#define _BB_GPIO_H_


#define HIGH		true
#define LOW			false

enum pin_mode_e {
    INPUT=0,
    OUTPUT
};

void pinMode(unsigned pin, pin_mode_e mode);
void digitalWrite(unsigned pin, bool value);
bool digitalRead(unsigned pin);

#endif