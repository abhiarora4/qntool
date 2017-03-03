#ifndef _QN_PACKET_H_
#define _QN_PACKET_H_

#include <iostream>
#include <stdint.h>
#include <string.h>

#define HEAD_CODE					0x71

#define HEAD_CODE_OFFSET_IN_PKT		0
#define CMD_OFFSET_IN_PKT			1
#define DATA_LEN_OFFSET_IN_PKT		2
#define DATA_OFFSET_IN_PKT 			5
#define CRC_OFFSET_IN_PKT(n)		(DATA_OFFSET_IN_PKT + (n))

#define TOTAL_PKT_LEN(n)			(7 + (n))
#define CRC_CALCULATION_LEN_FOR(n)	(4 + (n))

#define DATA_LEN_FIELD_SIZE_IN_PKT	(3)
#define CRC_FIELD_LEN_SIZE_IN_PKT	2


#define MAX_QN_PKT_LEN				1024


#define POLY 						(0x1021) // 0b10001000000100001
#define CRC16_POLY					POLY

struct QnPacketFields {
	uint8_t _cmd;
	uint32_t _datalen;
	uint8_t *_data;
	uint16_t _recv_crc;
	uint16_t _calculated_crc;

public:

	QnPacketFields() : _cmd(0), _datalen(0), _data(nullptr), _recv_crc(0), _calculated_crc(0) { }

	uint8_t get_cmd(void) { return _cmd; }
	uint8_t get_datalen(void) { return _datalen; }
	uint8_t *get_data(void) { return _data; }
	uint16_t get_recv_crc(void) { return _recv_crc; }
	uint16_t get_calc_crc(void) { return _calculated_crc; }

	bool isCRCMatched() { return (_calculated_crc == _recv_crc); }

};


class QnPacket {
private:
	uint8_t *_pkt;
	unsigned _pkt_len;

	friend struct QnPacketFields;

public:
	QnPacket(uint8_t cmd, uint8_t *data, unsigned datalen)
	{
		build_packet(cmd, data, datalen);
	}

	QnPacket() : _pkt(nullptr), _pkt_len(0) { }

	~QnPacket()
	{
		if (_pkt and (_pkt_len > 0))
			delete [] _pkt;
		_pkt = nullptr;
		_pkt_len = 0x00;
	}

	QnPacket(const QnPacket &p) // Copy Constructor
	{
		_pkt = nullptr;
		_pkt_len = p._pkt_len;
		if (p._pkt) {
			_pkt = new uint8_t[_pkt_len];
			memcpy(_pkt, p._pkt, _pkt_len);
		}
	}

	QnPacket(QnPacket &&p) // Move Constructor
	{
		_pkt = p._pkt;
		_pkt_len = p._pkt_len;

		p._pkt = nullptr;
		p._pkt_len = 0;
	}

	QnPacket& operator=(QnPacket r) // Copy and Swap Idiom of C++!
	{
		std :: swap(*this, r);
		return *this;
	}

	int build_packet(uint8_t cmd, uint8_t *data, unsigned data_len);
	void print(void);

	uint8_t *get_pkt(void) { return _pkt; }
	unsigned get_pktlen(void) { return _pkt_len; }

	static unsigned decode_datalen(uint8_t *p_buf);

	QnPacketFields decode(uint8_t *p);

};




#endif