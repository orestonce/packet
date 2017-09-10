#include "rpcdata-client.h"
#include "packet.h"

#include <stdexcept>

RDStringInfo::RDStringInfo()
{
	Clear();
}

void RDStringInfo::Clear()
{
	str.clear(); 
	msgId = 0; 
}

const char* RDStringInfo::GetName() const
{
	return "RDStringInfo";
}

void RDStringInfo::Encode(CPacket& p) const
{
	{
		int len = (int) str.size();
		p.WriteInt(len); 
		if (len > 0)
		{
			p.WriteByteArray(str.c_str(), len); 
		}
	}
	p.WriteLong(msgId); 
	p.Flush(); // 在包头部写入包大小信息
}

void RDStringInfo::Decode(CPacket& p)
{
	{
		int len = 0;
		len = p.ReadInt(); 
		if ( len < 0 || len > CPacket::PACKET_MAX_SIZE ) 
			throw std::runtime_error("length of str invalid !"); 
		if ( len > 0 )
		{
			str.resize(len); 
			p.ReadByteArray( &(str[0]), len); 
		}
	}
	msgId = p.ReadLong(); 
}

RDIntShortPair::RDIntShortPair()
{
	Clear();
}

void RDIntShortPair::Clear()
{
	id = 0; 
	count = 0; 
}

const char* RDIntShortPair::GetName() const
{
	return "RDIntShortPair";
}

void RDIntShortPair::Encode(CPacket& p) const
{
	p.WriteInt(id); 
	p.WriteShort(count); 
	p.Flush(); // 在包头部写入包大小信息
}

void RDIntShortPair::Decode(CPacket& p)
{
	id = p.ReadInt(); 
	count = p.ReadShort(); 
}

RDStringInfo2::RDStringInfo2()
{
	Clear();
}

void RDStringInfo2::Clear()
{
	str.clear(); 
	msgId = 0; 
}

const char* RDStringInfo2::GetName() const
{
	return "RDStringInfo2";
}

void RDStringInfo2::Encode(CPacket& p) const
{
	{
		int len = (int) str.size();
		p.WriteInt(len); 
		if (len > 0)
		{
			p.WriteByteArray(str.c_str(), len); 
		}
	}
	p.WriteLong(msgId); 
	p.Flush(); // 在包头部写入包大小信息
}

void RDStringInfo2::Decode(CPacket& p)
{
	{
		int len = 0;
		len = p.ReadInt(); 
		if ( len < 0 || len > CPacket::PACKET_MAX_SIZE ) 
			throw std::runtime_error("length of str invalid !"); 
		if ( len > 0 )
		{
			str.resize(len); 
			p.ReadByteArray( &(str[0]), len); 
		}
	}
	msgId = p.ReadLong(); 
}

RDStringInfo3::RDStringInfo3()
{
	Clear();
}

void RDStringInfo3::Clear()
{
	str.clear(); 
	msgId = 0; 
}

const char* RDStringInfo3::GetName() const
{
	return "RDStringInfo3";
}

void RDStringInfo3::Encode(CPacket& p) const
{
	{
		int len = (int) str.size();
		p.WriteInt(len); 
		if (len > 0)
		{
			p.WriteByteArray(str.c_str(), len); 
		}
	}
	p.WriteLong(msgId); 
	p.Flush(); // 在包头部写入包大小信息
}

void RDStringInfo3::Decode(CPacket& p)
{
	{
		int len = 0;
		len = p.ReadInt(); 
		if ( len < 0 || len > CPacket::PACKET_MAX_SIZE ) 
			throw std::runtime_error("length of str invalid !"); 
		if ( len > 0 )
		{
			str.resize(len); 
			p.ReadByteArray( &(str[0]), len); 
		}
	}
	msgId = p.ReadLong(); 
}

RDStringInfo4::RDStringInfo4()
{
	Clear();
}

void RDStringInfo4::Clear()
{
	str.clear(); 
	msgId = 0; 
}

const char* RDStringInfo4::GetName() const
{
	return "RDStringInfo4";
}

void RDStringInfo4::Encode(CPacket& p) const
{
	{
		int len = (int) str.size();
		p.WriteInt(len); 
		if (len > 0)
		{
			p.WriteByteArray(str.c_str(), len); 
		}
	}
	p.WriteLong(msgId); 
	p.Flush(); // 在包头部写入包大小信息
}

void RDStringInfo4::Decode(CPacket& p)
{
	{
		int len = 0;
		len = p.ReadInt(); 
		if ( len < 0 || len > CPacket::PACKET_MAX_SIZE ) 
			throw std::runtime_error("length of str invalid !"); 
		if ( len > 0 )
		{
			str.resize(len); 
			p.ReadByteArray( &(str[0]), len); 
		}
	}
	msgId = p.ReadLong(); 
}

