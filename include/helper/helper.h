#ifndef _HELPER_H_
#define _HELPER_H_

#include "errors.h"
#include "type.h"

#include <ctype.h>
#include <stdbool.h>

#define UNUSED_VARIABLE(V)          ((void)(V))

#define HIGH 						0x1
#define LOW  						0x0

#define PI 							3.1415926535897932384626433832795
#define HALF_PI 					1.5707963267948966192313216916398
#define TWO_PI 						6.283185307179586476925286766559

#define DEG_TO_RAD 					0.017453292519943295769236907684886
#define RAD_TO_DEG 					57.295779513082320876798154814105

#define EULER 						2.718281828459045235360287471352

/*
#define min(x,y) 					((x)>(y) ? (y) : (x))
#define max(x,y) 					((x)>(y) ? (x) : (y))
*/

#define absolute(x,y)				((x) >=0 ? (x) : -(x))

#define constrain(a,low,high)		(((a) > (high)) ? (high) : (((a) < (low)) ? (low) : (a)))

#define radians(deg) 				((deg)*DEG_TO_RAD)
#define degrees(rad) 				((rad)*RAD_TO_DEG)

#define lowByte(w) 					((uint8_t)((w) & 0xff))
#define highByte(w) 				((uint8_t)((w) >> 8))

#define bitRead(value,bit) 				(((value) >> (bit)) & 0x01)
#define bitSet(value,bit) 				((value) |= (1UL << (bit)))
#define bitClear(value,bit) 			(value) &= ~(1UL << (bit)))
#define bitWrite(value,bit,bitvalue) 	(bitvalue ? bitSet(value, bit) : bitClear(value, bit))

#define square(x) 						((x)*(x))

#define iseven(x) 					((x) & 0x01)
#define isodd(x)					(!iseven(x))

/* Double Negation Normalize Boolean values i {0,1} */
#define bit_at(a,i)					(!!((a)[(unsigned)(i) >> 3] & (1 << ((unsigned)(i) & 0x07))))
	
#define array_size(a)				(sizeof(a)/sizeof(a))

#define CR                  		('\r')   //Carriage Return Character
#define LF                  		('\n')   //Line Feed or New Line Character

/*
#define isblank(c) 					(((c) == ' ') || ((c) == '\t') || ((c) == '\v'))
#define isbacksplash(c) 			((c) == '/')
#define ishex(c)           			(isxdigit(c))
*/
#define isCR(c)						((c) == CR)
#define isLF(c)						((c) == LF)

namespace hoi {
	
	uint16_t crc16(void * buff, uint16_t len);
	uint16_t crc16_ccitt_generic(uint16_t poly, uint8_t *p_data, unsigned length);

	char * ipToCStr(char *dest, struct ipv4_s *ip);
	void cstrToIP(struct ipv4_s *ip, const char *src);

	int printIP(struct ipv4_s *ip);
	void setIP(struct ipv4_s *ip, uint8_t byte3, uint8_t byte2, uint8_t byte1, uint8_t byte0);

	int inet_aton(const char *cp, struct in_addr_s *inp);
	char *inet_ntoa(struct in_addr_s in);
	in_addr_t inet_network(const char *cp);

	struct in_addr inet_makeaddr(in_addr_t net, in_addr_t host);
	in_addr_t inet_lnaof(struct in_addr in);
	in_addr_t inet_netof(struct in_addr in);

}

uint16_t hton_ui16(uint16_t ui16_host);
uint16_t ntoh_ui16(uint16_t ui16_net);

uint32_t hton_ui32(uint32_t ui32_host);
uint32_t ntoh_ui32(uint32_t ui32_net);

int firstSetBit(uint32_t d);

uint32_t maskTime(uint8_t ui8, const uint32_t mask);

#endif