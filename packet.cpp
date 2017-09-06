#include "packet.h"
#include <string.h>
#include <arpa/inet.h>
#include <stdexcept>

#define P_OP_POS	0
#define P_LENGTH_POS	2

CPacket::CPacket(short op)
{
	memset(_buffer, 0, sizeof(_buffer) );
	_rpos = _wpos = PACKET_HEAD_SIZE;
	
	SetOp(op);
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
	if ( _wpos + sz > sizeof(_buffer) ) \
		throw std::runtime_error("Too many write data !")

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
	if ( _rpos + sz > _wpos ) \
		throw std::runtime_error("Has no data to read !")

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

int CPacket::ReadInt(const char* ptr)
{
	return ntohl( *(int*)ptr );
}

short CPacket::ReadShort(const char* ptr)
{
	return ntohs( *(int*)ptr );
}

char CPacket::ReadByte(const char* ptr)
{
	return *ptr;
}

void CPacket::WriteInt(char *ptr, int data)
{
	*(int*)ptr = htonl( data );
}

void CPacket::WriteShort(char *ptr, short data)
{
	*(short*) ptr = htons( data );
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
	return ReadInt(data +  P_OP_POS);
}

short CPacket::ParseOp(const char* data)
{
	return ReadShort(data);
}
