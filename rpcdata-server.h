#pragma once
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
	std::string str; // 角色的uid
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

class RDStringInfo2
{
public :
	RDStringInfo2();
	void Clear();
	const char* GetName() const;
	void Encode(CPacket& p) const;
	void Decode(CPacket& p);
public :
	std::string str; // 角色的uid
	long msgId; // 消息ID
};

class RDStringInfo3
{
public :
	RDStringInfo3();
	void Clear();
	const char* GetName() const;
	void Encode(CPacket& p) const;
	void Decode(CPacket& p);
public :
	std::string str; // 角色的uid
	long msgId; // 消息ID
};

class RDStringInfo4
{
public :
	RDStringInfo4();
	void Clear();
	const char* GetName() const;
	void Encode(CPacket& p) const;
	void Decode(CPacket& p);
public :
	std::string str; // 角色的uid
	long msgId; // 消息ID
};

