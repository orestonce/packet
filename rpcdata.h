#pragma once
#include "localmacro.h"

#include <string>
#include <vector>
class CPacket;

class RDStringInfo
{
public :
	RDStringInfo();
	void Clear();
	const char* GetName() const;
	void Encode(CPacket& p) const;
	void Decode(CPacket& p);
public :
	std::string str; // 
	long msgId; // 消息ID
};

class RDIntShortPair
{
public :
	RDIntShortPair();
	void Clear();
	const char* GetName() const;
	void Encode(CPacket& p) const;
	void Decode(CPacket& p);
public :
	int id; // 
	short count; // 
};

class CS2SS_LocalInfoData
{
public :
	CS2SS_LocalInfoData();
	void Clear();
	const char* GetName() const;
	void Encode(CPacket& p) const;
	void Decode(CPacket& p);
public :
	char type; // 服务器类型
	int id; // 本地ID
};

class RDIntIntPair
{
public :
	RDIntIntPair();
	void Clear();
	const char* GetName() const;
	void Encode(CPacket& p) const;
	void Decode(CPacket& p);
public :
	int id; // 
	int count; // 
	std::vector<CS2SS_LocalInfoData> testVector; // vector的测试
};

class RDItemInfo
{
public :
	RDItemInfo();
	void Clear();
	const char* GetName() const;
	void Encode(CPacket& p) const;
	void Decode(CPacket& p);
public :
	int itemId; // 
	int itemCount; // 
};

class GW2W_LinkNumReportData
{
public :
	GW2W_LinkNumReportData();
	void Clear();
	const char* GetName() const;
	void Encode(CPacket& p) const;
	void Decode(CPacket& p);
public :
	RDStringInfo info; // 连接信息数
	char openserver; // 是否为对外开放server
};

class S2C_ChampionGetFightRecordReData
{
public :
	S2C_ChampionGetFightRecordReData();
	void Clear();
	const char* GetName() const;
	void Encode(CPacket& p) const;
	void Decode(CPacket& p);
public :
	int roleId; // 角色ID
	int ret; // 返回值
};

class RDTitleInfo
{
public :
	RDTitleInfo();
	void Clear();
	const char* GetName() const;
	void Encode(CPacket& p) const;
	void Decode(CPacket& p);
public :
	int id; // 称号ID
	int expire; // 称号过期时间
};

class C2S_TitleGetInfoData
{
public :
	C2S_TitleGetInfoData();
	void Clear();
	const char* GetName() const;
	void Encode(CPacket& p) const;
	void Decode(CPacket& p);
public :
	int roleId; // 角色ID
};

class S2C_TitleGetInfoReData
{
public :
	S2C_TitleGetInfoReData();
	void Clear();
	const char* GetName() const;
	void Encode(CPacket& p) const;
	void Decode(CPacket& p);
public :
	int roleId; // 角色ID
	int ret; // 返回值
	std::vector<RDTitleInfo> titles; // 称号信息
	std::vector<int> titles2; // 称号信息
};

