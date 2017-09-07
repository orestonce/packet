#pragma once

/*
 * @class 读写缓冲区
 *	包格式:		[操作码(2字节)][包字节数(4字节)][数据(可变长度)]
 *	使用大端字节序, IO失败会抛出异常
 * */
class CPacket
{
public :
	enum {
		PACKET_MAX_SIZE	= 1024 * 1024,	// 包最大为1MB
		PACKET_HEAD_SIZE	= 6,		// 包最小为6字节
	};

	CPacket(short op = 0);

	void	Reset();
	
	short	GetOp();
	void	SetOp(short op);

	// 读取buffer
	char*	GetBuffer();
	int	GetLength();

	// 设置buffer
	void	SetBuffer(const char* data, int dataLength);
	
	// 以下函数用于保存数据
	void WriteLong	(long data);
	void WriteInt	(int data);
	void WriteShort	(short data);
	void WriteByte	(char data);
	void WriteByteArray(const char *data, int dataLength);
	void Flush();	// 将当前写的数据大小写到buffer里面
	
	// 以下函数用于读取数据
	long	ReadLong();
	int	ReadInt();
	short	ReadShort();
	char	ReadByte();
	void	ReadByteArray(char* data, int dataLength);
public :
	static long	ReadLong( const char*	ptr);
	static int	ReadInt ( const char*	ptr);
	static short	ReadShort(const char*	ptr);
	static char	ReadByte( const char*	ptr);
	static void	WriteLong( char *ptr,	long  data);
	static void	WriteInt(  char *ptr,	int   data);
	static void	WriteShort(char *ptr,	short data);
	static void	WriteByte( char *ptr,	char  data);
	
	static bool	IsValidLength(int len);
	
	// data 最小为PACKET_HEAD_SIZE
	static int	ParseLength(const char *data);
	static short	ParseOp(const char* data);

	static bool	HostIsBigEndian();
	static long	HostToNetLong(long data);
	static int	HostToNetInt(int data);
	static short	HostToNetShort(short data);
private:
	char _buffer[ PACKET_MAX_SIZE ];	// 数据包最大为PACKET_MAX_SIZE
	int  _rpos;			// 读位置
	int  _wpos;			// 写位置
};

