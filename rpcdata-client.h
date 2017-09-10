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

