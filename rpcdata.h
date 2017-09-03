#pragma once
#include "localmacro.h"
#include "packet.h"
#include <stdexcept>
class C2S_PlayerLogin
{
public :
	void Encode(CPacket& p)
	{
		{ // encode string : userName
			int length = strlen(userName); 
			if ( length >= MAX_STRING_LENGTH) 
				length = MAX_STRING_LENGTH -1 ; 
			p.WriteInt( length );
			p.WriteByteArray( userName, length );
		}
		{ // encode string : password
			int length = strlen(password); 
			if ( length >= MAX_STRING_LENGTH) 
				length = MAX_STRING_LENGTH -1 ; 
			p.WriteInt( length );
			p.WriteByteArray( password, length );
		}
		p.WriteInt(randomInt);
		p.Flush();
	}
	void Decode(CPacket& p)
	{
			{ // decode string : userName
				int length = p.ReadInt();
				if ( length < 0 || length >= MAX_STRING_LENGTH) 
					throw std::runtime_error("string 'userName' is too long ");
				memset(userName, 0, MAX_STRING_LENGTH);
				p.ReadByteArray(userName, length);
			}
			{ // decode string : password
				int length = p.ReadInt();
				if ( length < 0 || length >= MAX_STRING_LENGTH) 
					throw std::runtime_error("string 'password' is too long ");
				memset(password, 0, MAX_STRING_LENGTH);
				p.ReadByteArray(password, length);
			}
			randomInt = p.ReadInt();
	}
public :
	char userName[ MAX_STRING_LENGTH ] ; // 用户名
	char password[ MAX_STRING_LENGTH ] ; // 密码
	int randomInt ; // 随机数
};
