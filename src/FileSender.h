#pragma once
#include "se_translator.h"
#include "hciserver.h"
#include "DataElement.h"
#include "SDP.h"
#include "RFCOMM.h"
#include "OBEX.h"
#include "L2CAPConnection.h"
#include "FileSenderCaller.h"

using namespace std;


#define L2CAP_Level_Start 0
//send ConReq
#define L2CAP_Level_WaitingForConResp 1
//send ConfReq
#define L2CAP_Level_Configuring 2
//reply their Confreq
//#define L2CAP_Level_Configured 3
//send SSReq
#define SDP_Level_AwaitingServiceSearchResponse 4
//send SAReq
#define SDP_Level_AwaitingServiceAttributeResponse 5

#define RFCOMM_Level_SABM_Signalling_Sent	30
#define RFCOMM_Level_PN_Message_Sent		31
#define RFCOMM_Level_SABM_OBEXOP_Sent		32
#define RFCOMM_Level_MSC_Sent				33

#define RFCOMM_Level_OBEX_Connect_Sent		34
#define RFCOMM_Level_OBEX_File_Sent			35

//#define RFCOMM_Level_OBEXOP_DISC_SENT		36
#define RFCOMM_Level_DISC_OPCH_SENT			37
#define RFCOMM_Level_DISC_SignallingCH_SENT	38
#define L2CAP_RFCOMMPSM_DISC_SENT			39


//#define RFCOMM_Level_PN_Sent	31
//#define RFCOMM_Level_SABM_Data_Sent

#define WaitingForSDP_L2CAPDiscReponse 6

#define StartNewL2CAPForRFCOMM	10
#define StartNewL2CAPForSDP		20
#define MaxHCIPacketSize		64
#define HCIHeaderSize			4

class FileSender :	public HCIClient
	{
	public:
		
		FileSender(FileSenderCaller* caller,CListBox* logLB, CListBox* devicesLB);//devicesLB is optional...
		CListBox* devicesLB;
		CListBox* logLB;
		~FileSender(void);
		void UpdateDevicesLB();
		
		
		//for user:		
		bool sendFileToDevice(CStdioFile *file, BTDevice device);// add call-back
		//bool sendVCardToDevice(CStdioFile *file, BTDevice device);// add call-back
		bool cancelSend(); //send Disconnect commands to each layer established...
		bool DoInquiry();	//add call-back
		bool GetNames(int i);	//add call-back
		///////////////////////////////////////
		//bool setFile(CStdioFile* file);

		//implementations of HCIClient:
		void HandleInquiryComplete(); //do call-back
		void HandleInput(USHORT Handle,BYTE* payLoad,USHORT lenght);
		//////////////////////////////////////////////////////////////
		   USHORT ourMTU;
	protected:
		bool SendL2CAPPacket(L2CAP_Packet* packet);
		bool SendL2CAPPayload(BYTE* payLoad, USHORT payLoadLength);
		bool SendRFCOMMFrame(RFCOMM_Frame* frame);
		bool SendRFCOMMData(BYTE* payLoad, USHORT payLoadLength);
		bool SendCreditedRFCOMMFrame(CreditedRFCOMM_Frame* creditedFrame);
		bool SendCreditedRFCOMMData(BYTE* payLoad, USHORT payLoadLength);
		bool SendOBEXPacket(OBEXPacket* pkt);
		
		se_translator translator;
		bool sendingFile;
		bool cancelNow;

		int ConnectionLevel;
		FileSenderCaller* caller;
		CStdioFile *file;
		OBEXPutCommand* opc;
		bool startedPut;

		USHORT ourCID;
		USHORT theirCID;
		USHORT theirMaxOBEXPacketSize;
		USHORT ourFlushTimeout;
		USHORT theirMTU;
		USHORT theirFlushTimeout;
		int  theirConfigureLevel;//=0;
		int  ourConfigureLevel;//=0;
		BYTE ourCommandIdentifier;//=1;
		BYTE theirCommandIdentifier;//=1;		
		BYTE readComplete;// = 1;
		USHORT bufferPos;// = 0;
		USHORT expectedPacketLength;// = 0;
		BYTE OBEXOPChannel;
		USHORT theirMaxFrameSize;
		USHORT ourMaxFrameSize;
		BYTE theirCreditToUs;
		BYTE ourCreditToThem;
		USHORT curL2CAPConnection;
		BYTE ourMSCResponded;// = 0;
		BYTE theirMSCResponded;// = 0;
		USHORT SDP_TransactionID;// = 0x0000;

	};
