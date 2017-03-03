#include "helper/helper.h"

#include <stdio.h>

#define MAX_IP_STR_LEN 		20

struct _ui_ipv4_s _temp = {0, 0, 0, 0};
char ip_cstr[MAX_IP_STR_LEN];

void convToCompactIP(struct ipv4_s *ip)
{
	ip->byte[0] = (uint8_t)_temp.ui_octet[0];
	ip->byte[1] = (uint8_t)_temp.ui_octet[1];
	ip->byte[2] = (uint8_t)_temp.ui_octet[2];
	ip->byte[3] = (uint8_t)_temp.ui_octet[3];
}

void cpyToTempIP(struct ipv4_s *ip)
{
	_temp.ui_octet[0] = ip->byte[0];
	_temp.ui_octet[1] = ip->byte[1];
	_temp.ui_octet[2] = ip->byte[2];
	_temp.ui_octet[3] = ip->byte[3];
}

// TODO move this somewhere nice
uint16_t hoi::crc16(void *buffer, uint16_t len)
{
	// http://www.mycal.net/?cpath=/Archive/&id=567&action=9/
	uint8_t *buff = (uint8_t *)buffer;
	uint16_t crcvalue = 0xFFFF; // Initial crc
	uint16_t data = 0;
	int i;
	for (i=0; i<len; i++) {
		data = (crcvalue ^ buff[i]) & 0xFF;
		data = (data ^ (data << 4)) & 0xFF;
		data = (data << 8) ^ (data << 3) ^ (data >> 4);
		crcvalue=((crcvalue >> 8) ^ data);
	}
	return crcvalue;
}

// https://www.ghsi.de/CRC/index.php?Polynom=10001000000100001&Message=33040000
// https://www.lammertbies.nl/comm/info/crc-calculation.html
uint16_t hoi::crc16_ccitt_generic(uint16_t poly, uint8_t *p_data, unsigned length)
{
	unsigned i;
  	uint8_t data;
  	uint16_t crc = 0; // 0xffff;

  	while (length-- > 0) {
    	for (i=0, data = *p_data; i < 8; i++, (data <<= 1)) {
      		if ((crc >> 15) ^ (data >> 7))
        		crc = (crc << 1) ^ poly;
      		else
        		crc <<= 1;
    	}
    	p_data++;
  	}
  	return crc;
}

char* hoi::ipToCStr(char *dest, struct ipv4_s *ip)
{
	sprintf(dest, "%d.%d.%d.%d", (int)ip->byte[0], (int)ip->byte[1], (int)ip->byte[2], (int)ip->byte[3]);
	return dest;
}

void hoi::cstrToIP(struct ipv4_s *ip, const char *src)
{
	sscanf(src, "%u.%u.%u.%u", &_temp.ui_octet[0], &_temp.ui_octet[1], &_temp.ui_octet[2], &_temp.ui_octet[3]);
	convToCompactIP(ip);
	return;
}

int hoi::printIP(struct ipv4_s *ip)
{
	cpyToTempIP(ip);
	return printf("%u.%u.%u.%u", _temp.ui_octet[0], _temp.ui_octet[1], _temp.ui_octet[2], _temp.ui_octet[3]);
}

void hoi::setIP(struct ipv4_s *ip, uint8_t byte3, uint8_t byte2, uint8_t byte1, uint8_t byte0)
{
	ip->byte[0] = byte3;
	ip->byte[1] = byte2;
	ip->byte[2] = byte1;
	ip->byte[3] = byte0;
}

uint32_t flip_byte_order_ui32(uint32_t ui32)
{
	uint32_t _ui32R = (ui32 >> 24) & 0x000000FF;
	_ui32R |= (ui32 >> 8) & 0x0000FF00;
	_ui32R |= (ui32 << 8) & 0x00FF0000;
	_ui32R |= (ui32 << 24) & 0xFF000000;
	return _ui32R;
}

uint16_t flip_byte_order_ui16(uint16_t ui16)
{
	uint16_t _ui16R = (ui16 >> 8) & 0x00FF;
	_ui16R |= (ui16 << 8) & 0xFF00;
	return _ui16R;
}


int hoi::inet_aton(const char *cp, struct in_addr_s *inp) 
{
	setIP(inp, cp[0], cp[1], cp[2], cp[3]);
	return 1;
}

char* hoi::inet_ntoa(struct in_addr_s in) 
{
	ipToCStr(ip_cstr, &in);
	return ip_cstr;
}

in_addr_t hoi::inet_network(const char *cp)
{
	struct ipv4_s ip;
	cstrToIP(&ip, cp);
	ip.s_addr = ntoh_ui32(ip.s_addr);
	return ip.s_addr;
}

uint16_t hton_ui16(uint16_t ui16_host)
{
	return flip_byte_order_ui16(ui16_host);
}

uint16_t ntoh_ui16(uint16_t ui16_net)
{
	return flip_byte_order_ui16(ui16_net);
}

uint32_t hton_ui32(uint32_t ui32_host)
{
	return flip_byte_order_ui32(ui32_host);
}


uint32_t ntoh_ui32(uint32_t ui32_net)
{
	return flip_byte_order_ui32(ui32_net);
}

int firstSetBit(uint32_t d)
{
	int i;
	for (i=0; !(d & (0x01)); i++, d >>= 1);
	return i;
}

uint32_t maskTime(uint8_t ui8, const uint32_t mask)
{
	int shift = firstSetBit(mask);
	return ((ui8 << shift) & mask);
}

