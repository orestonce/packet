#include "packet.h"
#include "rpcdata.h"
#include <stdio.h>
#include <assert.h>
#include <stdlib.h>
#include <string.h>

#define __STDC_LIMIT_MACROS
#include <stdint.h>

class CPacketTest
{
public :
	CPacketTest();
	void BeginTest();
	bool CanReport();
	int GetTimePassed();

	void TestOp();
	void TestBuffer_Byte();
	void TestBuffer_Short();
	void TestBuffer_Int();
	void TestBuffer_Long();
	void TestBuffer_ByteArray();
private:
	time_t lastReport;
	time_t beginTime;
};

int main()
{
	CPacketTest test;
	
	test.BeginTest();

	test.TestOp();
	test.TestBuffer_Byte();
	test.TestBuffer_Short();
	test.TestBuffer_Int();
	test.TestBuffer_Long();
	test.TestBuffer_ByteArray();

	return 0;
}

CPacketTest::CPacketTest()
{
	beginTime	= 0;
	lastReport= 0;
}

void CPacketTest::BeginTest()
{
	beginTime = time(NULL);
	lastReport= beginTime;
}

bool CPacketTest::CanReport()
{
	time_t now = time(NULL);

	if ( now - lastReport > 5)
	{
		lastReport =now;
		return true;
	}

	return false;
}

int CPacketTest::GetTimePassed()
{
	time_t now = time(NULL);
	
	return now - beginTime;
}

void CPacketTest::TestOp()
{
	for (short i=INT16_MIN; i <= INT16_MAX; ++i)
	{
		CPacket p1( i );
		p1.Flush();

		CPacket p2;

		p2.SetBuffer(p1.GetBuffer(), p1.GetLength());

		short op1 = p1.GetOp();
		short op2 = p2.GetOp();

		assert(op1 == op2);
		assert(op1 == i);

		p1.SetOp( i );

		assert( i == p1.GetOp() );

		if ( CanReport() )
			printf("%d\top=%d\n", GetTimePassed(), i);

		if ( i == INT16_MAX )
			break;
	}
}

void CPacketTest::TestBuffer_Byte()
{
	CPacket p;

	assert(p.GetLength() == CPacket::PACKET_HEAD_SIZE);
	
	int seed = time(NULL);

	srand( seed );

	int cnt = (CPacket::PACKET_MAX_SIZE - CPacket::PACKET_HEAD_SIZE) / 1;

	for(int i=0; i< cnt; ++i)
	{
	 	p.WriteByte( char( rand() ) );
	}
	
	p.Flush();
	
	CPacket p2;

	p2.SetBuffer(p.GetBuffer(), p.GetLength());
	
	srand( seed );
	for(int i=0; i< cnt; ++i)
	{
		assert( char(rand()) == p2.ReadByte() );
	}
}

void CPacketTest::TestBuffer_Short()
{
	CPacket p;

	assert(p.GetLength() == CPacket::PACKET_HEAD_SIZE);
	
	int seed = time(NULL);

	srand( seed );
	
	int cnt = (CPacket::PACKET_MAX_SIZE - CPacket::PACKET_HEAD_SIZE) / 2;

	for(int i=0; i< cnt; ++i)
	{
	 	p.WriteShort( short( rand() ) );
	}
	
	p.Flush();
	
	CPacket p2;

	p2.SetBuffer(p.GetBuffer(), p.GetLength());
	
	srand( seed );
	for(int i=0; i< cnt; ++i)
	{
		assert( short(rand()) == p2.ReadShort() );
	}
}

void CPacketTest::TestBuffer_Int()
{
	CPacket p;

	assert(p.GetLength() == CPacket::PACKET_HEAD_SIZE);
	
	int seed = time(NULL);

	srand( seed );
	
	int cnt = (CPacket::PACKET_MAX_SIZE - CPacket::PACKET_HEAD_SIZE) / 4;

	for(int i=0; i< cnt; ++i)
	{
	 	p.WriteInt( int( rand() ) );
	}
	
	p.Flush();
	
	CPacket p2;

	p2.SetBuffer(p.GetBuffer(), p.GetLength());
	
	srand( seed );
	for(int i=0; i< cnt; ++i)
	{
		assert( int(rand()) == p2.ReadInt() );
	}
}

void CPacketTest::TestBuffer_Long()
{
	CPacket p;

	assert(p.GetLength() == CPacket::PACKET_HEAD_SIZE);
	
	int seed = time(NULL);

	srand( seed );
	
	int cnt = (CPacket::PACKET_MAX_SIZE - CPacket::PACKET_HEAD_SIZE) / 8;

	for(int i=0; i< cnt; ++i)
	{
	 	p.WriteLong( 1L * rand() * rand() );
	}
	
	p.Flush();
	
	CPacket p2;

	p2.SetBuffer(p.GetBuffer(), p.GetLength());
	
	srand( seed );
	for(int i=0; i< cnt; ++i)
	{
		assert( 1L * rand() * rand() == p2.ReadLong() );
	}
}

void CPacketTest::TestBuffer_ByteArray()
{
	CPacket p;

	assert(p.GetLength() == CPacket::PACKET_HEAD_SIZE);
	
	int seed = time(NULL);

	srand( seed );

	int cnt = (CPacket::PACKET_MAX_SIZE - CPacket::PACKET_HEAD_SIZE) / 1;

	for(int i=0; i< cnt; ++i)
	{
	 	p.WriteByte( char( rand() ) );
	}
	
	p.Flush();
	
	CPacket p2;

	int writeLength = p.GetLength() - CPacket::PACKET_HEAD_SIZE;

	p2.WriteByteArray(p.GetBuffer() + CPacket::PACKET_HEAD_SIZE, writeLength);
	p2.Flush();
	
	assert( p.GetLength() == p2.GetLength() );
	assert( memcmp(p.GetBuffer(), p2.GetBuffer(), p.GetLength()) == 0 );
}

