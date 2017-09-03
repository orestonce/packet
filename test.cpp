#include "rpcdata.h"
#include "packet.h"
#include <string.h>

int main()
{
	C2S_PlayerLogin data;

	strncpy(data.userName, "user1", MAX_STRING_LENGTH);
	strncpy(data.password, "password1", MAX_STRING_LENGTH);
	data.randomInt = 100;
	
	CPacket p;

	data.Encode(p);

	C2S_PlayerLogin sdata;
	sdata.Decode(p);
}
