/************************************************************************************
// Sample code for calling IVT BlueSoleil APIs

  Copyright (c) 2004 -2005 , IVT Corporation. All rights reserved.

*******************************************************************************************/

//modified for WMouseXP by Kasidit Yusuf


#include "stdafx.h"
#include <stdio.h>
#include "ConnectCheck.h"

#define ERROR_TIMES 10

#include <Winsock2.h>

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CConnectCheck::CConnectCheck()
{
	m_nDatasize = 32 + sizeof(ICMPHEADER);
	m_pICPacket = NULL;
	m_pRecvbuf = NULL;
	m_nSeq = 0;
}

CConnectCheck::~CConnectCheck()
{
	delete[] m_pICPacket;
	delete[] m_pRecvbuf;
	m_pICPacket=NULL;
	m_pRecvbuf=NULL;
}
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
Description:
	Ping send the icmp packet to the destination host and receive the echo packet
		 from the destination host. The prodedure last two minutes.
Arguments:
	strName		the name of the destination host,if use ip address,set it to NULL.
	strAddr		the ip address of the destination host,if use name,set it to NULL.
	times		times=1 is to last 2min,times=0 is to send only one packet
Return Value:
   	TRUE		Ping the destination host successfully for two minutes.		
	FALSE		Failed to ping the destination host for two minutes.
---------------------------------------------------------------------------*/
BOOL CConnectCheck::Ping(char* strName, LPCTSTR strAddr, int times)
{
	WSADATA wsaData;
			
	if(WSAStartup(MAKEWORD(2,1),&wsaData))
	{
		printf(">> socket start up incorrectly!");
		return FALSE;
	}
	m_socket = WSASocket(AF_INET,SOCK_RAW,IPPROTO_ICMP,NULL,0,0);
	if(m_socket == INVALID_SOCKET)
	{
		printf(">> create socket failure!");
		return FALSE;
	}
	int timeout = 1000;
	int senset = setsockopt(m_socket,SOL_SOCKET,SO_SNDTIMEO,(char*)(&timeout),sizeof(timeout));
	if(senset == SOCKET_ERROR)
	{
		printf(">> socket sending timeout set fails!\n");
		return FALSE;
	}
	memset(&m_saiDestaddr, 0, sizeof(m_saiDestaddr));
	if (strAddr)
	{
		m_saiDestaddr.sin_family = AF_INET;
		m_ulAddr = inet_addr(strAddr);
		if(m_ulAddr == INADDR_NONE)
		{
			printf(">> the IP address is wrong!\n");
			return FALSE;
		}
		m_saiDestaddr.sin_addr.s_addr = m_ulAddr;
	}
	else if (strName)
	{	//add by hch
		struct hostent *hp = gethostbyname(strName);
		if ((!hp)) 
		{
			int n = WSAGetLastError(); 
			return FALSE;
		}
		memcpy(&(m_saiDestaddr.sin_addr),hp->h_addr,hp->h_length);
		m_saiDestaddr.sin_family=hp->h_addrtype;
	}
	else
		return FALSE;

	m_pICPacket = new char[1024];
	memset(m_pICPacket, 0, 1024);
	m_pRecvbuf = new char[1024];
	memset(m_pRecvbuf, 0, 1024);

	FillIcmpData();
	
	m_nError=0;
	m_nSend=0;

	ConnectItOrNot(strAddr, times);

	if (m_nError != 0)
		if((m_nSend/m_nError) <= ERROR_TIMES)
			return FALSE;
	return TRUE;
}
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
Description:
	FillIcmpData fill the icmp packet.
Arguments:
	None
Return Value:
   	None
---------------------------------------------------------------------------*/
void CConnectCheck::FillIcmpData()
{
	char* datapart;
	ICMPHEADER* icheader;

	memset(m_pICPacket,0,1024);

	icheader=(ICMPHEADER*)m_pICPacket;
	icheader->type=8;
	icheader->code=0;
	icheader->checksum=0;
	icheader->identification=(USHORT)GetCurrentProcessId();
	icheader->seqnum=0;

	datapart=m_pICPacket+sizeof(ICMPHEADER);
	memset(datapart,'A',m_nDatasize-sizeof(ICMPHEADER));
}
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
Description:
	CheckSum calculate the check sum.
Arguments:
	None
Return Value:
   	return the check sum
---------------------------------------------------------------------------*/
USHORT CConnectCheck::CheckSum()
{
	ULONG cksum = 0;
	int size = m_nDatasize;
	USHORT *buffer;

	buffer = (USHORT*)m_pICPacket;
	while(size >1) 
	{
		cksum += *buffer++;
		size -= sizeof(USHORT);
	}

	if(size) 
	{
		cksum += *(UCHAR*)buffer;
	}

	cksum = (cksum >> 16) + (cksum & 0xffff);
	cksum += (cksum >>16);
	return (USHORT)(~cksum);
}
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
Description:
	ConnectItOrNot check whether in the two minutes BlueTester can ping the 
				   destination host successfully.
Arguments:
	strAddr		the IP address of destination host.
	times		times=0 is to send only one packet,times=1 is to last 2min
Return Value:
   	None
---------------------------------------------------------------------------*/
void CConnectCheck::ConnectItOrNot(LPCTSTR strAddr, int times)
{
	ULONG ulStartTime;
	int nSend,nRecv;
	struct timeval twait;
	fd_set fdsread;
	int nSelRet = -1;

	twait.tv_sec = 10;
	twait.tv_usec = 0;
	ulStartTime = GetTickCount();
	int count = 1;
	while( times == 0 ? count-- : (GetTickCount()-ulStartTime < 120000))
	{		
		((ICMPHEADER*)m_pICPacket)->seqnum = m_nSeq++;
		((ICMPHEADER*)m_pICPacket)->checksum = 0;
		((ICMPHEADER*)m_pICPacket)->checksum = CheckSum();

		nSend=sendto(m_socket,m_pICPacket,m_nDatasize,0,(struct sockaddr*)&m_saiDestaddr,sizeof(m_saiDestaddr));
		if(nSend==SOCKET_ERROR)
		{
			int ret1 = WSAGetLastError();
			if(ret1==WSAETIMEDOUT)
		        printf(">> Time out during sending the packet!\n");
			else
	    		printf(">> Error in sending the packet!\n");
			m_nError++;
			Sleep(5000);
			continue;
		}
		m_nSend++;
		printf(">> send 60 bytes to the %s\n",strAddr);
		
		FD_ZERO(&fdsread);
		FD_SET(m_socket, &fdsread);
		nSelRet = select(NULL,&fdsread,NULL,NULL,&twait);
		if (nSelRet == 0)
		{
			printf(">> Time out during receiving the packet!\n");
			m_nError++;
			continue;
		}
		if (nSelRet == SOCKET_ERROR)
		{
			int nTmp = WSAGetLastError();
	  		printf(">> Select failed, the error code is %d\n", nTmp);
			m_nError++;
			continue;
		}
		if( FD_ISSET(m_socket, &fdsread) )
		{
			int len=sizeof(m_saiSouaddr);
			nRecv=recvfrom(m_socket,m_pRecvbuf,1024,0,(struct sockaddr*)&m_saiSouaddr,&len);
			if(nRecv==SOCKET_ERROR)
			{
				int nTmp = WSAGetLastError();
				printf(">> Error in receving the packet, the error code is %d!\n", nTmp);
				m_nError++;
				continue;
			}

			if(DecodeResp(nRecv))
				printf(">> %d bytes from %s\n",nRecv,strAddr);
			else
				m_nError++;
			Sleep(100);
		}
	}

	return;
}
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
Description:
	DecodeResp check whether the received packet is the echo packet from the
			   destination host.
Arguments:
	None
Return Value:
   	TRUE		It is the echo type packet from the destination host
	FALSE		It is not the echo type packet from the destination host
---------------------------------------------------------------------------*/
BOOL CConnectCheck::DecodeResp(int bytes)
{
	IPHEADER *iphdr;
	ICMPHEADER *icmphdr;
	USHORT iphdrlen;

	iphdr = (IPHEADER *)m_pRecvbuf;
	iphdrlen = iphdr->headerlen * 4 ; // number of 32-bit words *4 = bytes
	icmphdr = (ICMPHEADER*)(m_pRecvbuf + iphdrlen);

	if (bytes  < iphdrlen + 8)	// 8 = sizeof(ICMPHEADER)
	{
		printf(">> No ICMP packet from the LAN!");
		return FALSE;
	}

	if (icmphdr->type != 0)		// echo type packet's type is 0
	{
		printf(">> Non_echo type packet from the LAN!");
		return FALSE;
	}

	if (icmphdr->identification != (USHORT)GetCurrentProcessId()) 
	{
		printf(">> Someone else's packet from the LAN!");
		return FALSE;
	}
	return TRUE;
}