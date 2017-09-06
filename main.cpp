#include "packet.h"
#include <stdio.h>

int main()
{
	CPacket p( 0x1382 );

	p.WriteInt( 0x12345678 );
	p.Flush();

	for(int i=0; i < p.GetLength(); ++i)
	{
		printf("%02X ", (unsigned char)p.GetBuffer()[i] );
	}

	printf("\n");
	printf( "packet length : %d \n", p.GetLength() );
	printf( "int value : %X \n", p.ReadInt() );
	printf( "op : %X \n", p.GetOp() );

	return 0;
}
