#include "drivers/QnPacket.h"
#include "helper/helper.h"

#include <iostream>
#include <string>
#include <stdint.h>
#include <string.h>	



uint16_t crc16(uint8_t *p_data, unsigned length)
{
	return hoi::crc16_ccitt_generic(CRC16_POLY, p_data, length);
}

unsigned QnPacket::decode_datalen(uint8_t *p_buf)
{
	unsigned datalen = 0x00;

	datalen = (((unsigned)p_buf[0]) & 0x0000FF);
	datalen |= ((((unsigned)p_buf[1]) << 8) & 0x00FF00);
	datalen |= ((((unsigned)p_buf[2]) << 16) & 0xFF0000);

	return datalen;
}

int QnPacket::build_packet(uint8_t cmd, uint8_t *data, unsigned data_len)
{
	if (data == nullptr)
		data_len = 0;

	_pkt_len = TOTAL_PKT_LEN(data_len);
	_pkt = new uint8_t[TOTAL_PKT_LEN(data_len)];

	_pkt[HEAD_CODE_OFFSET_IN_PKT] = HEAD_CODE;
	_pkt[CMD_OFFSET_IN_PKT] = (cmd & 0xFF);

	// Endianess should matter. Let the compiler handles conversion for us!
	_pkt[DATA_LEN_OFFSET_IN_PKT] = (data_len & 0xFF);
	_pkt[DATA_LEN_OFFSET_IN_PKT + 1] = ((data_len >> 8) & 0xFF);
	_pkt[DATA_LEN_OFFSET_IN_PKT + 2] = ((data_len >> 16) & 0xFF);


	if (data_len)
		memcpy(_pkt + DATA_OFFSET_IN_PKT, data, data_len);

	uint16_t crc = crc16(_pkt + CMD_OFFSET_IN_PKT, CRC_CALCULATION_LEN_FOR(data_len));

	_pkt[CRC_OFFSET_IN_PKT(data_len)] = (crc & 0xFF);
	_pkt[CRC_OFFSET_IN_PKT(data_len) + 1] = ((crc >> 8) & 0xFF);

	return 0;
}


QnPacketFields QnPacket::decode(uint8_t *p)
{
	QnPacketFields f;

	uint32_t datalen;
	uint16_t crc_recv;

	if (p == nullptr)
		return f;

	if (p[HEAD_CODE_OFFSET_IN_PKT] != HEAD_CODE)
		return f;

	f._cmd = p[CMD_OFFSET_IN_PKT];

	datalen = (((unsigned)p[DATA_LEN_OFFSET_IN_PKT]) & 0x0000FF);
	datalen |= ((((unsigned)p[DATA_LEN_OFFSET_IN_PKT + 1]) << 8) & 0x00FF00);
	datalen |= ((((unsigned)p[DATA_LEN_OFFSET_IN_PKT + 2]) << 16) & 0xFF0000);

	f._datalen = datalen;
	if (datalen > 0)
		f._data = (p + DATA_OFFSET_IN_PKT);

	f._calculated_crc = crc16(p + CMD_OFFSET_IN_PKT, CRC_CALCULATION_LEN_FOR(datalen));
	
	crc_recv = (((unsigned)p[CRC_OFFSET_IN_PKT(datalen)]) & 0x00FF);
	crc_recv |= ((((unsigned)p[CRC_OFFSET_IN_PKT(datalen) + 1]) << 8) & 0xFF00);

	f._recv_crc = crc_recv;

	return f;
}


void QnPacket::print()
{
	if (_pkt == nullptr)
		return;
	if (_pkt_len > MAX_QN_PKT_LEN)
		return;

	unsigned i;
	for (i=0; i<_pkt_len; i++)
		std :: cout << "0x" << std :: hex << (int)_pkt[i] << " ";
	std :: cout << std :: dec << std :: endl;
}

