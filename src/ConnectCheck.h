/************************************************************************************
// Sample code for calling IVT BlueSoleil APIs

  Copyright (c) 2004 -2005 , IVT Corporation. All rights reserved.

*******************************************************************************************/

//modified for WMouseXP by Kasidit Yusuf

#ifndef _CONNECTCHECK_H_
#define _CONNECTCHECK_H_



class CConnectCheck  
{
public:
	CConnectCheck();
	virtual ~CConnectCheck();
	//times=1 is to last 2min,times=0 is to send only one packet
	BOOL Ping(char* strName, LPCTSTR strAddr= NULL, int times=0); 
	
protected:
	BOOL DecodeResp(int bytes);
	void ConnectItOrNot(LPCTSTR strAddr, int times);
	USHORT CheckSum();
	void FillIcmpData();

protected:
	USHORT	m_nSeq; //the sequence number of the sent packets 
	int		m_nError; //the times error happened when sending or receiving the packets
	int		m_nSend; //the packet number of sending
	ULONG	m_ulAddr; //the IP address of the destination host
	int		m_nDatasize; //the packet size not including icmp and ip header
	char	*m_pRecvbuf; //the buffer receives the packet
	SOCKET	m_socket; //the socket to communication
	char	*m_pICPacket; //the buffer stores the icmp packet
	sockaddr_in	 m_saiSouaddr,m_saiDestaddr; //the socket struct stores the IP address

struct ICMPHEADER
{
	BYTE type;
	BYTE code;
	USHORT checksum;
	USHORT identification;
	USHORT seqnum;
};	// icmp packet header

struct IPHEADER
{
	UINT headerlen:4;
	UINT version:4;
	UCHAR service;
	USHORT totalen;
	USHORT identification;
	USHORT fl_fr;
	UCHAR totaliv;
	UCHAR protoc;
	USHORT checksum;
	UINT sourIP;
	UINT destIP;
}; //ip packet header
	
};

#endif // !defined(AFX_CONNECTCHECK_H__33380861_D26F_11D4_AAF9_0000B4B3075D__INCLUDED_)
