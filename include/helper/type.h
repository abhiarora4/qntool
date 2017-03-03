#ifndef _TYPE_H_
#define _TYPE_H_

#include <stdint.h>

/* Big Endian or Network Byte Order */
/* bytes[0] stores most-significant byte of ip version 4 address */	

#define in_addr_s ipv4_s
typedef uint32_t in_addr_t;

struct ipv4_s {
	union {
		uint8_t byte[4];
		in_addr_t s_addr;
	};
}__attribute__((packed));

struct _ui_ipv4_s {
	unsigned ui_octet[4];
};

	

#endif