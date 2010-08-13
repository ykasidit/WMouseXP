/*
Copyright (C) 2007-2010 Kasidit Yusuf.

This file is part of WMouseXP.

WMouseXP is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

WMouseXP is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with WMouseXP. If not, see <http://www.gnu.org/licenses/>.
*/


/////////////////////////////////////////////////////////////////////
// OBEX.h Header for OBEX packet generation and decoding classes
// Autor: Kasidit Yusuf
/////////////////////////////////////////////////////////////////////

#ifndef OBEX_H
#define OBEX_H
#include "OBEXStructs.h"
#include "Conversions.h"

#define vCardNameStrLen 12	//exclude null as normal strlen
#define obexProfileStrLen 21 //exclude null as normal strlen



class OBEXPacket
{

	public:
		OBEXPacket(BYTE code, BYTE* payLoad, USHORT payLoadLength);
		OBEXPacket(BYTE* array, USHORT Length);
		~OBEXPacket();

	USHORT packetLength;
	OBEXPacketStruct* wholePacket;	
	private:
	BYTE allocated;
};

class OBEXConnectPacket : public OBEXPacket
{
	public:
		OBEXConnectPacket(BYTE code, USHORT MaxOBEXPacketLength);
		OBEXConnectPacket(BYTE* array, USHORT length);
		
	
	OBEXConnectParamsStruct* params;
	USHORT paramsLength;

};

class OBEXPutCommand /*this is NOT inherited from OBEXPacket. This generates OBEX put packets in sequence using BYTE* CgetNextPacket(USHORT* packetLength) until the last packet that completes the seding of the whole file*/
{
	public:	
		OBEXPutCommand(USHORT MaxOBEXPacketLength, CStdioFile* file);		
		OBEXPutCommand(LPTSTR name, USHORT MaxOBEXPacketSize,BYTE* array, DWORD length);;//alternative, RAM based

		OBEXPutCommand(BYTE* array, USHORT length);//reverse, incoming
		
		OBEXPacket* CgetNextPacket(); //use this for normal/no-profile objects/files
		OBEXPacket* CgetNextVcardPacket(); //use this for vCard objects
		
		USHORT nPacketsToSend;

	private:
		CStdioFile* file;
		bool IsVcard;
		bool IsVcal;

		DWORD chunkBuffPos;
		DWORD packetCount;
		DWORD firstHeadersLength;
		DWORD consequtiveHeadersLength;
		DWORD sumBytesToSendAllPackets;

		USHORT MaxOBEXPacketSize;

		//static const char* vCardName;//INCLUDING NULL!!!// = ("text/x-vCard");
		//static const char* obexProfile;//INCLUDING NULL!!!//("x-obex/object-profile");

		CString iNameAs; //in case no stdiofile, send from RAM buffer resource
		DWORD iSendLength;//in case no stdiofile, send from RAM buffer resource
		BYTE* iSendBuff;
		DWORD iNBytesWritten;


};



#endif