#include "packet.h"
#include <string.h>
#include <stdexcept>
#include <stdint.h>

#define P_OP_POS	0
#define P_LENGTH_POS	2

bool CPacket::HostIsBigEndian()
{
	union {
		char	s[2];
		short	i;
	} data;

	data.i	= 1;

	return (data.s[0] == 0 );
}

long CPacket::HostToNetLong(long data)
{
	long ret = data;

	if ( !HostIsBigEndian() )
	{
		char *p1 = (char*)&ret;
		char *p2 = (char*)&data;

		p1[0]	= p2[7];
		p1[1]	= p2[6];
		p1[2]	= p2[5];
		p1[3]	= p2[4];
		p1[4]	= p2[3];
		p1[5]	= p2[2];
		p1[6]	= p2[1];
		p1[7]	= p2[0];
	}

	return ret;
}

int CPacket::HostToNetInt(int data)
{
	int ret = data;

	if ( !HostIsBigEndian() )
	{
		char *p1 = (char*)&ret;
		char *p2 = (char*)&data;

		p1[0]	= p2[3];
		p1[1]	= p2[2];
		p1[2]	= p2[1];
		p1[3]	= p2[0];
	}

	return ret;
}

short CPacket::HostToNetShort(short data)
{
	short ret = data;

	if ( !HostIsBigEndian() )
	{
		char *p1 = (char*)&ret;
		char *p2 = (char*)&data;

		p1[0]	= p2[1];
		p1[1]	= p2[0];
	}

	return ret;
}

CPacket::CPacket(short op)
{
	Reset();

	SetOp(op);
}

void CPacket::Reset()
{
	memset(_buffer, 0, sizeof(_buffer) );
	_rpos = _wpos = PACKET_HEAD_SIZE;
}

void CPacket::SetOp(short op)
{
	WriteShort(_buffer + P_OP_POS, op);
}

short CPacket::GetOp()
{
	return ReadShort(_buffer + P_OP_POS);
}

char* CPacket::GetBuffer()
{
	return _buffer;
}

int CPacket::GetLength()
{
	return _wpos;
}

void CPacket::SetBuffer(const char* data, int dataLength)
{
	if ( !IsValidLength(dataLength) )
		throw std::logic_error("dataLength is invalid !");

	int len = ReadInt(data + P_LENGTH_POS);
	if ( len != dataLength )
		throw std::runtime_error("Buffer length not equals dataLength !");

	memset(_buffer, 0, sizeof(_buffer) );
	memcpy(_buffer, data, dataLength);
	_rpos = PACKET_HEAD_SIZE;
	_wpos = dataLength;
}

#define CHECK_WPOS(sz) \
	if ( _wpos + sz > (int)sizeof(_buffer) ) \
		throw std::runtime_error("Too many write data !")

void CPacket::WriteLong(long data)
{
	CHECK_WPOS( 8 );
	
	WriteLong(_buffer + _wpos, data);

	_wpos += 8;
}

void CPacket::WriteInt(int data)
{
	CHECK_WPOS( 4 );

	WriteInt(_buffer + _wpos, data);
	
	_wpos += 4;
}

void CPacket::WriteShort(short data)
{
	CHECK_WPOS( 2 );

	WriteShort(_buffer + _wpos, data);

	_wpos += 2;
}

void CPacket::WriteByte(char data)
{
	CHECK_WPOS( 1 );

	WriteByte(_buffer + _wpos, data);

	_wpos += 1;
}

void CPacket::WriteByteArray(const char* data, int length)
{
	CHECK_WPOS(length);

	memcpy(_buffer + _wpos, data, length);

	_wpos += length;
}

void CPacket::Flush()
{
	WriteInt(_buffer + P_LENGTH_POS, _wpos );
}

#define CHECK_RPOS(sz) \
	if ( _rpos + sz > (int)_wpos ) \
		throw std::runtime_error("Has no data to read !")

long CPacket::ReadLong()
{
	CHECK_RPOS(8);

	long data = ReadLong(_buffer + _rpos);
	_rpos += 8;
	
	return data;
}

int CPacket::ReadInt()
{
	CHECK_RPOS(4);
	
	int data = ReadInt(_buffer + _rpos);
	_rpos += 4;

	return data;
}

short CPacket::ReadShort()
{
	CHECK_RPOS(2);

	short data = ReadShort(_buffer + _rpos);
	_rpos += 2;
	
	return data;
}

char CPacket::ReadByte()
{
	CHECK_RPOS(1);

	char data = ReadByte(_buffer + _rpos);
	_rpos += 1;

	return data;
}

void CPacket::ReadByteArray(char *data, int length)
{
	CHECK_RPOS( length );
	
	memcpy(data, _buffer + _rpos, length);
	_rpos += length;
}

long CPacket::ReadLong(const char* ptr)
{
	return HostToNetLong( *(long*)ptr );
}

int CPacket::ReadInt(const char* ptr)
{
	return HostToNetInt( *(int*)ptr );
}

short CPacket::ReadShort(const char* ptr)
{
	return HostToNetShort( *(short*)ptr );
}

char CPacket::ReadByte(const char* ptr)
{
	return *ptr;
}

void CPacket::WriteLong(char *ptr, long data)
{
	*(long*)ptr = HostToNetLong(data);
}

void CPacket::WriteInt(char *ptr, int data)
{
	*(int*)ptr = HostToNetInt(data);
}

void CPacket::WriteShort(char *ptr, short data)
{
	*(short*) ptr = HostToNetShort(data);
}

void CPacket::WriteByte(char *ptr, char data)
{
	*ptr = data;
}

bool CPacket::IsValidLength(int len)
{
	return PACKET_HEAD_SIZE <= len && len <= PACKET_MAX_SIZE;
}

int CPacket::ParseLength(const char *data)
{
	return ReadInt(data +  P_LENGTH_POS);
}

short CPacket::ParseOp(const char* data)
{
	return ReadShort(data + P_OP_POS);
}
