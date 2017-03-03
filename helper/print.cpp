#include "helper/print.h"

#include <string.h>
#include <iostream>
#include <fstream>

const char *Print :: window_style_terminator = "\r\n";
const char *Print :: unix_style_terminator = "\n\0";

int Print :: write(const uint8_t *buf, unsigned len)
{
	unsigned i;
	for (i=0; i<len; i++) {
		if (write(buf[i])==0)
			break;
	}
	return i;
}

int Print :: write(const char *buf, unsigned len)
{
	return write(reinterpret_cast<const uint8_t *>(buf), len);
}

int Print :: write(const char *cstr)
{
	return write(cstr, strnlen(cstr, MAX_PRINTABLE_LEN));
}

int Print :: print(const char *cstr)
{
	if (cstr == NULL)
		return 0;
	return write(cstr, strnlen(cstr, MAX_PRINTABLE_LEN));
}

int Print :: print(unsigned i, base_fmt_e b)
{
	return printULong(static_cast<unsigned long>(i), false, b);
}

int Print :: print(int i, base_fmt_e b)
{
	return printLong(static_cast<long>(i), false, b);
}

int Print :: print(unsigned char c, base_fmt_e b)
{
	char buf[8*(sizeof(c)) + 1];
	snprintf(buf, (8*(sizeof(c)) + 1), "%c", c);
	return write(reinterpret_cast<const char *>(buf));
}

int Print :: print(char c, base_fmt_e b)
{
	char buf[8*(sizeof(c)) + 1];
	snprintf(buf, (8*(sizeof(c)) + 1), "%c", c);
	return write(reinterpret_cast<const char *>(buf));
}

int Print :: print(unsigned long l, base_fmt_e b)
{
	return printULong(l, false, b);
}

int Print :: print(long l, base_fmt_e b)
{
	return printLong(l, false, b);
}

int Print :: print(float f, base_fmt_e b)
{
	return printDouble(static_cast<double>(f), false, b);
}

int Print :: print(double d, base_fmt_e b)
{
	return printDouble(d, false, b);
}

int Print :: println(const char *cstr)
{
	if (cstr == NULL)
		return 0;
	unsigned len = strnlen(cstr, MAX_PRINTABLE_LEN);
	char buf[len + 3];
	memcpy(buf, cstr, len);
	buf[len++] = '\r';
	buf[len++] = '\n';
	buf[len] = '\0';
	return write(buf, len);
}

int Print :: println(unsigned i, base_fmt_e b)
{
	return printULong(static_cast<unsigned long>(i), true, b);
}

int Print :: println(int i, base_fmt_e b)
{
	return printLong(static_cast<long>(i), true, b);
}

int Print :: println(unsigned char c, base_fmt_e b)
{
	char buf[8*(sizeof(c)) + 3];
	snprintf(buf, (8*(sizeof(c)) + 1), "%c\r\n", c);
	return write(reinterpret_cast<const char *>(buf));
}

int Print :: println(char c, base_fmt_e b)
{
	char buf[8*(sizeof(c)) + 3];
	snprintf(buf, (8*(sizeof(c)) + 1), "%c\r\n", c);
	return write(reinterpret_cast<const char *>(buf));
}

int Print :: println(unsigned long l, base_fmt_e b)
{
	return printULong(l, true, b);
}

int Print :: println(long l, base_fmt_e b)
{
	return printLong(l, true, b);
}

int Print :: println(float f, base_fmt_e b)
{
	return printDouble(static_cast<double>(f), true, b);
}

int Print :: println(double d, base_fmt_e b)
{
	return printDouble(d, true, b);
}

int Print :: println()
{
	return write((line_ending == UNIX_STYLE_LINE_ENDING) ? 
				unix_style_terminator : window_style_terminator);
}

int Print :: printULong(unsigned long l, bool terminate, base_fmt_e base)
{
	char buf[8*(sizeof(l)) + 3]; // +3

	char *str = &(buf[sizeof(buf) - 1]);
	*str = '\0';

	if (terminate) {
		str -= (2 + 1);
		memcpy(str, (line_ending == UNIX_STYLE_LINE_ENDING) ? 
							unix_style_terminator : window_style_terminator, 2);
	}

	if (base < 2)
		base = DECIMAL;

	do {
		char c = l % base;
		l /= base;
		str--;
		*str = c < 10 ? (c + '0') : (c + 'A' - 10);
	} while (l);

	return write(str);
}

int Print :: printLong(long l, bool terminate, base_fmt_e base)
{
	bool positive = (l >= 0);

	char buf[8*(sizeof(l)) + 4]; // +4

	char *str = &(buf[sizeof(buf) - 1]);
	*str = '\0';

	if (terminate) {
		str -= (2 + 1);
		memcpy(str, (line_ending == UNIX_STYLE_LINE_ENDING) ? 
							unix_style_terminator : window_style_terminator, 2);
	}

	if (base < 2)
		base = DECIMAL;

	if ((l < 0) && (base == DECIMAL))
		l = -l;
	do {
		char c = l % base;
		l /= base;
		str--;
		*str = c < 10 ? (c + '0') : (c + 'A' - 10);
	} while (l);

	if ((!positive) && (base == DECIMAL))
		*--str = '-';
	return write(str);
}

int Print :: printDouble(double d, bool terminate, base_fmt_e base)
{
	char buf[8*(sizeof(d)) + 3];
	if (terminate)
		snprintf(buf, (8*(sizeof(d)) + 3), "%lf%s", d, (line_ending == UNIX_STYLE_LINE_ENDING) ? 
				unix_style_terminator : window_style_terminator);
	else
		snprintf(buf, (8*(sizeof(d)) + 3), "%lf", d);
	return write(reinterpret_cast<const char *>(buf));
}










//#define __MAIN__

#ifdef __MAIN__

using namespace std;

class file : public Print {
public:
	ofstream stream;

	file(const char *f)
	{
		stream.open(f);
	}

	int write(uint8_t c)
	{
		stream.put(c);
		if (stream.fail())
			return 0;
		return 1;
	}

	void close()
	{
		stream.close();
	}
};

int main()
{
	file f("hello");
	f.set_line_ending(Print :: 	UNIX_STYLE_LINE_ENDING
);
	f.println("Hello World");
	f.println(123, Print :: BINARY);
	f.println();
	f.println(1.23);
	f.println('a');
	//f.println();
	f.println('b');
	f.close();
	return  0;
}

#endif

