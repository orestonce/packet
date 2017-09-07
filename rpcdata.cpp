#include "rpcdata.h"
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

CS2SS_LocalInfoData::CS2SS_LocalInfoData()
{
	Clear();
}

void CS2SS_LocalInfoData::Clear()
{
	type = 0; 
	id = 0; 
}

const char* CS2SS_LocalInfoData::GetName() const
{
	return "CS2SS_LocalInfoData";
}

void CS2SS_LocalInfoData::Encode(CPacket& p) const
{
	p.WriteByte(type); 
	p.WriteInt(id); 
	p.Flush(); // 在包头部写入包大小信息
}

void CS2SS_LocalInfoData::Decode(CPacket& p)
{
	type = p.ReadByte(); 
	id = p.ReadInt(); 
}

RDIntIntPair::RDIntIntPair()
{
	Clear();
}

void RDIntIntPair::Clear()
{
	id = 0; 
	count = 0; 
	testVector.clear(); 
}

const char* RDIntIntPair::GetName() const
{
	return "RDIntIntPair";
}

void RDIntIntPair::Encode(CPacket& p) const
{
	p.WriteInt(id); 
	p.WriteInt(count); 
	{
		p.WriteInt(testVector.size());
		for(auto it = testVector.begin(); it != testVector.end(); ++it)
		{
			const CS2SS_LocalInfoData& temp = *it;
			temp.Encode(p);
		}
	}
	p.Flush(); // 在包头部写入包大小信息
}

void RDIntIntPair::Decode(CPacket& p)
{
	id = p.ReadInt(); 
	count = p.ReadInt(); 
	{
		int sz = p.ReadInt();
		for(int i = 0; i < sz; ++i)
		{
			CS2SS_LocalInfoData temp;
			temp.Decode(p);
			testVector.push_back(temp);
		}
	}
}

RDItemInfo::RDItemInfo()
{
	Clear();
}

void RDItemInfo::Clear()
{
	itemId = 0; 
	itemCount = 0; 
}

const char* RDItemInfo::GetName() const
{
	return "RDItemInfo";
}

void RDItemInfo::Encode(CPacket& p) const
{
	p.WriteInt(itemId); 
	p.WriteInt(itemCount); 
	p.Flush(); // 在包头部写入包大小信息
}

void RDItemInfo::Decode(CPacket& p)
{
	itemId = p.ReadInt(); 
	itemCount = p.ReadInt(); 
}

GW2W_LinkNumReportData::GW2W_LinkNumReportData()
{
	Clear();
}

void GW2W_LinkNumReportData::Clear()
{
	info.Clear(); 
	openserver = 0; 
}

const char* GW2W_LinkNumReportData::GetName() const
{
	return "GW2W_LinkNumReportData";
}

void GW2W_LinkNumReportData::Encode(CPacket& p) const
{
	info.Encode(p);
	p.WriteByte(openserver); 
	p.Flush(); // 在包头部写入包大小信息
}

void GW2W_LinkNumReportData::Decode(CPacket& p)
{
	info.Decode(p);
	openserver = p.ReadByte(); 
}

S2C_ChampionGetFightRecordReData::S2C_ChampionGetFightRecordReData()
{
	Clear();
}

void S2C_ChampionGetFightRecordReData::Clear()
{
	roleId = 0; 
	ret = 0; 
}

const char* S2C_ChampionGetFightRecordReData::GetName() const
{
	return "S2C_ChampionGetFightRecordReData";
}

void S2C_ChampionGetFightRecordReData::Encode(CPacket& p) const
{
	p.WriteInt(roleId); 
	p.WriteInt(ret); 
	p.Flush(); // 在包头部写入包大小信息
}

void S2C_ChampionGetFightRecordReData::Decode(CPacket& p)
{
	roleId = p.ReadInt(); 
	ret = p.ReadInt(); 
}

RDTitleInfo::RDTitleInfo()
{
	Clear();
}

void RDTitleInfo::Clear()
{
	id = 0; 
	expire = 0; 
}

const char* RDTitleInfo::GetName() const
{
	return "RDTitleInfo";
}

void RDTitleInfo::Encode(CPacket& p) const
{
	p.WriteInt(id); 
	p.WriteInt(expire); 
	p.Flush(); // 在包头部写入包大小信息
}

void RDTitleInfo::Decode(CPacket& p)
{
	id = p.ReadInt(); 
	expire = p.ReadInt(); 
}

C2S_TitleGetInfoData::C2S_TitleGetInfoData()
{
	Clear();
}

void C2S_TitleGetInfoData::Clear()
{
	roleId = 0; 
}

const char* C2S_TitleGetInfoData::GetName() const
{
	return "C2S_TitleGetInfoData";
}

void C2S_TitleGetInfoData::Encode(CPacket& p) const
{
	p.WriteInt(roleId); 
	p.Flush(); // 在包头部写入包大小信息
}

void C2S_TitleGetInfoData::Decode(CPacket& p)
{
	roleId = p.ReadInt(); 
}

S2C_TitleGetInfoReData::S2C_TitleGetInfoReData()
{
	Clear();
}

void S2C_TitleGetInfoReData::Clear()
{
	roleId = 0; 
	ret = 0; 
	titles.clear(); 
	titles2.clear(); 
}

const char* S2C_TitleGetInfoReData::GetName() const
{
	return "S2C_TitleGetInfoReData";
}

void S2C_TitleGetInfoReData::Encode(CPacket& p) const
{
	p.WriteInt(roleId); 
	p.WriteInt(ret); 
	{
		p.WriteInt(titles.size());
		for(auto it = titles.begin(); it != titles.end(); ++it)
		{
			const RDTitleInfo& temp = *it;
			temp.Encode(p);
		}
	}
	{
		p.WriteInt(titles2.size());
		for(auto it = titles2.begin(); it != titles2.end(); ++it)
		{
			const int& temp = *it;
			p.WriteInt(temp); 
		}
	}
	p.Flush(); // 在包头部写入包大小信息
}

void S2C_TitleGetInfoReData::Decode(CPacket& p)
{
	roleId = p.ReadInt(); 
	ret = p.ReadInt(); 
	{
		int sz = p.ReadInt();
		for(int i = 0; i < sz; ++i)
		{
			RDTitleInfo temp;
			temp.Decode(p);
			titles.push_back(temp);
		}
	}
	{
		int sz = p.ReadInt();
		for(int i = 0; i < sz; ++i)
		{
			int temp;
			temp = p.ReadInt(); 
			titles2.push_back(temp);
		}
	}
}

