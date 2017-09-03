#pragma once

/*
 * @class 读写缓冲区
 *	包格式:		[操作码(2字节)][包字节数(4字节)][数据(可变长度)]
 *	使用大端字节序, IO失败会抛出异常
 * */
class CPacket
{
public :
	CPacket(short op = 0);
	
	bool Init(const char* data, int dataLength);
	// 读取buffer
	char*	GetBuffer();
	
	// 获取数据长度
	int	GetLength();
	
	// 以下函数用于保存数据
	void WriteInt	(int data);
	void WriteShort	(short data);
	void WriteByte	(char data);
	void WriteByteArray(const char *data, int dataLength);

	// 将当前写的数据大小写到buffer里面
	void		Flush();
	
	// 以下函数用于读取数据
	int	ReadInt();
	short	ReadShort();
	char	ReadByte();
	void	ReadByteArray(char* data, int dataLength);
	
	short	GetOp();
	void	SetOp(short op);

	static int GetPackMinLength();
	static bool ParseLength(const char *data, int dataLength, int& packLength);
private:
	char _buffer[1024 * 1024];	// 数据包最大为1MB
	int  _rpos;			// 读位置
	int  _wpos;			// 写位置
};

