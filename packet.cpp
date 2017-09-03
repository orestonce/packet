#include "packet.h"
#include <string.h>
#include <arpa/inet.h>
#include <stdexcept>

#define PACKET_HEAD_SIZE 6

int CPacket::GetPackMinLength()
{
	return PACKET_HEAD_SIZE;
}

CPacket::CPacket(short op)
{
	memset(_buffer, 0, sizeof(_buffer) );
	_rpos = _wpos = PACKET_HEAD_SIZE;

	SetOp(op);
}

void CPacket::SetOp(short op)
{
	if ( op < 0 || op > 0xFFFF )
		throw std::runtime_error("Unexpected packet op !");
	
	int oldw = _wpos;
	_wpos = 0;
	WriteShort( op );
	_wpos = oldw;
}

short CPacket::GetOp()
{
	int oldr = _rpos;

	_rpos = 0;
	short op = ReadShort();
	_rpos = oldr;

	return op;
}

char* CPacket::GetBuffer()
{
	return _buffer;
}

int CPacket::GetLength()
{
	return _wpos;
}

#define CHECK_WPOS(sz) \
	if ( _wpos + sz > sizeof(_buffer) ) \
		throw std::runtime_error("Too many write data !")

void CPacket::WriteInt(int data)
{
	CHECK_WPOS( 4 );
	
	*(int*)( _buffer + _wpos) = htonl( data );

	_wpos += 4;
}

void CPacket::WriteShort(short data)
{
	CHECK_WPOS( 2 );

	*(short*)(_buffer + _wpos) = htons( data );
	_wpos += 2;
}

void CPacket::WriteByte(char data)
{
	CHECK_WPOS( 1 );

	*(char*)(_buffer + _wpos) = data;
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
	int oldw = _wpos;
	_wpos = 2;
	WriteInt( oldw );
	_wpos = oldw;
}

#define CHECK_RPOS(sz) \
	if ( _rpos + sz > _wpos ) \
		throw std::runtime_error("Has no data to read !")

int CPacket::ReadInt()
{
	CHECK_RPOS(4);
	
	int data = ntohl( *(int*)(_buffer + _rpos) );
	_rpos += 4;

	return data;
}

short CPacket::ReadShort()
{
	CHECK_RPOS(2);

	short data = ntohs( *(short*)(_buffer + _rpos) );
	_rpos += 2;
	
	return data;
}

char CPacket::ReadByte()
{
	CHECK_RPOS(1);

	char data = *(char *)( _buffer + _rpos );
	_rpos += 1;

	return data;
}

void CPacket::ReadByteArray(char *data, int length)
{
	CHECK_RPOS( length );
	
	memcpy(data, _buffer + _rpos, length);
	_rpos += length;
}

bool CPacket::Init(const char* data, int dataLength)
{
	if ( dataLength < PACKET_HEAD_SIZE || dataLength > sizeof(_buffer) )
		return false;
	
	memcpy(_buffer, data, dataLength);
	_rpos = _wpos = PACKET_HEAD_SIZE;

	return true;
}

bool CPacket::ParseLength(const char* data, int dataLength, int& packLength)
{
	if ( dataLength < PACKET_HEAD_SIZE || !data )
		return false;
	
	CPacket p;
	p.Init(data, dataLength);
	
	p._rpos = 2;
	packLength = p.ReadInt();

	return true;
}
