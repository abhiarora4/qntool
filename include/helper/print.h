#ifndef _PRINT_H_
#define _PRINT_H_

#include <stdint.h>

#define MAX_PRINTABLE_LEN				120

#define base_fmt base_fmt_e

class Print {

public:

	enum base_fmt_e {
		BINARY=2,
		OCTAL=8,
		DECIMAL=10,
		HEXADECIMAL=16
	};

	enum line_ending_e {
		UNIX_STYLE_LINE_ENDING=0,
		WINDOWS_STYLE_LINE_ENDING
	};

private:
	int printULong(unsigned long l, bool terminate, base_fmt_e b);
	int printLong(long l, bool terminate, base_fmt_e b);

	int printDouble(double d, bool terminate, base_fmt_e b);

	line_ending_e line_ending;

	static const char *window_style_terminator;
	static const char *unix_style_terminator;

public:

	Print() : line_ending(UNIX_STYLE_LINE_ENDING)
	{

	}

	void set_line_ending(line_ending_e l)
	{
		line_ending = l;
	}

	virtual int write(uint8_t c) = 0; // Pure Virtual Function
	virtual int write(const uint8_t *buf, unsigned len);
	int write(const char *cstr, unsigned len);
	int write(const char *cstr);

	int print(const char *cstr);
	int print(unsigned i, base_fmt_e b=DECIMAL);
	int print(int i, base_fmt_e b=DECIMAL);
	int print(char c, base_fmt_e b=DECIMAL);
	int print(unsigned char c, base_fmt_e b=DECIMAL);
	int print(float f, base_fmt_e b=DECIMAL);
	int print(double f, base_fmt_e b=DECIMAL);
	int print(long l, base_fmt_e b=DECIMAL);
	int print(unsigned long l, base_fmt_e b=DECIMAL);

	int println(const char *cstr);
	int println(unsigned i, base_fmt_e b=DECIMAL);
	int println(int i, base_fmt_e b=DECIMAL);
	int println(char c, base_fmt_e b=DECIMAL);
	int println(unsigned char c, base_fmt_e b=DECIMAL);
	int println(float f, base_fmt_e b=DECIMAL);
	int println(double f, base_fmt_e b=DECIMAL);
	int println(long l, base_fmt_e b=DECIMAL);
	int println(unsigned long l, base_fmt_e b=DECIMAL);
	int println(void);
};



#endif