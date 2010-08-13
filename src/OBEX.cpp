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
// OBEX.cpp Implementation for OBEX packet generation and decoding classes
// Autor: Kasidit Yusuf
/////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "OBEX.h"

OBEXPacket::OBEXPacket(BYTE code, BYTE* payLoad, USHORT payLoadLength)
{
	this->packetLength = payLoadLength + OBEXPacketHeaderLength;
	
	this->wholePacket = (OBEXPacketStruct*) new BYTE[this->packetLength];

	BYTE* bptr = (wholePacket->data);
	for(int i=0;i<payLoadLength;i++)
		{
			if(payLoad)
				bptr[i] =payLoad[i] ;
			else
				bptr[i] = 0;
		};


	this->allocated =1;
	this->wholePacket->code = code;
	BEUS len(this->packetLength);
	len.copyValTo((BYTE*)&(this->wholePacket->packetLength));	
}


OBEXPacket::OBEXPacket(BYTE* array, USHORT length)
{
	this->wholePacket = (OBEXPacketStruct*) array;
	this->packetLength = length;
	
	this->allocated=0;

};

OBEXPacket::~OBEXPacket()
{
	if(allocated)
		delete[] ((BYTE*) this->wholePacket);
}

OBEXConnectPacket::OBEXConnectPacket(BYTE code, USHORT MAXOBEXpacketLength) : OBEXPacket(code, NULL,  OBEXConnectParamsLength)
{
	this->paramsLength = OBEXConnectParamsLength;
	this->params = (OBEXConnectParamsStruct *)this->wholePacket->data;
	this->params->flags = OBEXFlags;
	this->params->OBEX_Version  = OBEXVersion;
	
	BEUS len(MAXOBEXpacketLength);
	len.copyValTo((BYTE*)&(this->params->MaxOBEXPacketLength));		
}

OBEXConnectPacket::OBEXConnectPacket(BYTE* array, USHORT length) : OBEXPacket(array,length)
{
	
	
}







OBEXPutCommand::OBEXPutCommand(USHORT MaxOBEXPacketSize,CStdioFile* file)		
{
	this->file = file;
	this->chunkBuffPos = 0;
	this->packetCount = 0;

	
	this->MaxOBEXPacketSize = MaxOBEXPacketSize;
	/*
	(MaxPktSz)n = (1st h)+(n-1)(consq h)+filesz
				= (1st h) + n(consq h) - (consq h) + filesz
	
	(MaxPktSz)n -n(consq h) = (1st h) - (consq h) + filesz
	n(MaxPktSz - consq h) =	 (1st h) - (consq h) + filesz

	n = ( (1st h) - (consq h) + filesz  ) / (MaxPktSz - consq h)
	
	
	  */	//ULONGLONG
	
	firstHeadersLength  =(DWORD) (OBEXPacketHeaderLength +NameHeaderLength +(USHORT)(((this->file->GetFileName().GetLength())+1)*2)  +  LengthHeaderAndObjectLength  +  BodyHeaderLength);
	consequtiveHeadersLength = (DWORD) (OBEXPacketHeaderLength+BodyHeaderLength);
	DWORD upper,lower;
	upper = (((DWORD)(file->GetLength()))+firstHeadersLength) - consequtiveHeadersLength;
	lower =   ((DWORD)MaxOBEXPacketSize-(DWORD)consequtiveHeadersLength);

	
	nPacketsToSend =   (USHORT)(upper/lower);
	nPacketsToSend += ((upper%lower)?1:0);

	sumBytesToSendAllPackets =  ((DWORD)file->GetLength()) +	 ((nPacketsToSend-1)* consequtiveHeadersLength) + firstHeadersLength;
   
	
	
	
		

}

OBEXPutCommand::OBEXPutCommand(BYTE* array, USHORT length)
{

} 


OBEXPutCommand::OBEXPutCommand(LPTSTR name, USHORT MaxOBEXPacketSize,BYTE* array, DWORD length)
{

	this->file = NULL;
	this->chunkBuffPos = 0;
	this->packetCount = 0;

	iNameAs = name;
	
	this->MaxOBEXPacketSize = MaxOBEXPacketSize;
	/*
	(MaxPktSz)n = (1st h)+(n-1)(consq h)+filesz
				= (1st h) + n(consq h) - (consq h) + filesz
	
	(MaxPktSz)n -n(consq h) = (1st h) - (consq h) + filesz
	n(MaxPktSz - consq h) =	 (1st h) - (consq h) + filesz

	n = ( (1st h) - (consq h) + filesz  ) / (MaxPktSz - consq h)
	
	
	  */	//ULONGLONG
	
	firstHeadersLength  =(DWORD) (OBEXPacketHeaderLength +NameHeaderLength +(USHORT)(((iNameAs.GetLength())+1)*2)  +  LengthHeaderAndObjectLength  +  BodyHeaderLength);
	consequtiveHeadersLength = (DWORD) (OBEXPacketHeaderLength+BodyHeaderLength);
	DWORD upper,lower;
	upper = (((DWORD)(length))+firstHeadersLength) - consequtiveHeadersLength;
	lower =   ((DWORD)MaxOBEXPacketSize-(DWORD)consequtiveHeadersLength);

	nPacketsToSend =   (USHORT)(upper/lower);
	nPacketsToSend += ((upper%lower)?1:0);

	sumBytesToSendAllPackets =  ((DWORD)length) +	 ((nPacketsToSend-1)* consequtiveHeadersLength) + firstHeadersLength;
   
	iSendLength = length;
	iNBytesWritten = 0;
	iSendBuff = array;


}
		
OBEXPacket* OBEXPutCommand::CgetNextPacket()
{
	
if(this->packetCount == this->nPacketsToSend)
		return NULL;
	USHORT paramsBuffPos=0;
	
	//OBEXPacket(BYTE code, BYTE* payLoad, USHORT payLoadLength)
	//((this->fileSize - this->buffPos) > (this->MaxOBEXPacketSize))? MaxOBEXPacketSize:(this->fileSize - this->buffPos)
	//printf("\nPKT size: %d\n",NameHeaderLength+((this->file->GetFileName().GetLength())+1)*2+LengthHeaderAndObjectLength+BodyHeaderLength+(((this->file->GetLength() - this->chunkBuffPos) > (this->MaxOBEXPacketSize))? MaxOBEXPacketSize:(this->file->GetLength() - this->chunkBuffPos)));

	//OBEXPacket* pkt = new OBEXPacket(Opcode_Put+((this->packetCount==this->nPacketsToSend-1)?OrFactorForFinalBit:0),NULL,(  (packetCount-nPacketsToSend) == 1  )?(USHORT)((sumBytesToSendAllPackets%((ULONGLONG)this->MaxOBEXPacketSize)) - OBEXPacketHeaderLength): MaxOBEXPacketSize - OBEXPacketHeaderLength );
	
	OBEXPacket* pkt = new OBEXPacket(Opcode_Put+((((USHORT)this->packetCount)==(this->nPacketsToSend-1))?OrFactorForFinalBit:0),NULL, ((USHORT)(  ( sumBytesToSendAllPackets - (((DWORD)MaxOBEXPacketSize)*packetCount) ) >= ((DWORD)MaxOBEXPacketSize))?MaxOBEXPacketSize:( sumBytesToSendAllPackets - (((DWORD)MaxOBEXPacketSize)*packetCount) )) - OBEXPacketHeaderLength );

	if(this->packetCount == 0)
	{


				pkt->wholePacket->data[paramsBuffPos++] = HI_Name;

				int intfnl = file?(file->GetFileName().GetLength()):(iNameAs.GetLength());
				
				BEUS fnl( NameHeaderLength+(((	intfnl	)+1/*Terminating NULL*/)*2)/*Unicode*/);

				fnl.copyValTo((pkt->wholePacket->data) +paramsBuffPos);

				paramsBuffPos+=2; //SHORT size

				
				for(USHORT i=0;i<intfnl;i++)
				{
					pkt->wholePacket->data[paramsBuffPos++]=0;
					pkt->wholePacket->data[paramsBuffPos++]=file?(file->GetFileName().GetAt(i)):iNameAs.GetAt(i);
				}

				pkt->wholePacket->data[paramsBuffPos++]=0; //two bytes of a UNICODE NULL
				pkt->wholePacket->data[paramsBuffPos++]=0; //two bytes of a UNICODE NULL

				pkt->wholePacket->data[paramsBuffPos++] = HI_Length;

				BEUW objSz((UWORD)file?file->GetLength():iSendLength);

				objSz.copyValTo((pkt->wholePacket->data) +paramsBuffPos);
				
				paramsBuffPos+=4; //WORD size

	}
	
	
	
	pkt->wholePacket->data[paramsBuffPos++] = (this->packetCount==this->nPacketsToSend-1)?HI_EndOfBody:HI_Body;


	BEUS chunkSz((pkt->packetLength) - ((USHORT)((packetCount==0)?firstHeadersLength:consequtiveHeadersLength)) +  BodyChunkHeaderLength );
	//chunksize includes the obex chunk header!!! according to spec

	chunkSz.copyValTo((pkt->wholePacket->data) +paramsBuffPos);
	paramsBuffPos+=2; //SHORT size



	for(USHORT i=0;i< (chunkSz.getValUSHORT() - BodyChunkHeaderLength/*we're just writing the contents*/) ;i++)
	{
		
		if(file)
			file->Read((pkt->wholePacket->data)+(paramsBuffPos++),1);//iNBytesWritten not used here
		else
			*((pkt->wholePacket->data)+(paramsBuffPos++)) = iSendBuff[iNBytesWritten++];
	}

	

	this->chunkBuffPos += chunkSz.getValUSHORT();


	this->packetCount++;


	return pkt;
}

OBEXPacket* OBEXPutCommand::CgetNextVcardPacket()
	{
		if(this->packetCount == this->nPacketsToSend)
		return NULL;

	CString vCardName("text/x-vCard");
	CString obexProfile("x-obex/object-profile");

	USHORT paramsBuffPos=0;
	
	//OBEXPacket(BYTE code, BYTE* payLoad, USHORT payLoadLength)
	//((this->fileSize - this->buffPos) > (this->MaxOBEXPacketSize))? MaxOBEXPacketSize:(this->fileSize - this->buffPos)
	//printf("\nPKT size: %d\n",NameHeaderLength+((this->file->GetFileName().GetLength())+1)*2+LengthHeaderAndObjectLength+BodyHeaderLength+(((this->file->GetLength() - this->chunkBuffPos) > (this->MaxOBEXPacketSize))? MaxOBEXPacketSize:(this->file->GetLength() - this->chunkBuffPos)));

	OBEXPacket* pkt = new OBEXPacket(Opcode_Put+((this->packetCount==this->nPacketsToSend-1)?OrFactorForFinalBit:0),NULL,TypeHeaderAndLengthLength+(obexProfile.GetLength()+1)+NameHeaderLength+(vCardName.GetLength()+1/*null ending*/)*2+LengthHeaderAndObjectLength+BodyHeaderLength+(((this->file->GetLength() - this->chunkBuffPos) > (this->MaxOBEXPacketSize))? MaxOBEXPacketSize:(this->file->GetLength() - this->chunkBuffPos)));


	if(this->packetCount == 0)
	{


				pkt->wholePacket->data[paramsBuffPos++] = HI_Name;
				
				BEUS fnl( NameHeaderLength+(( vCardName.GetLength()+1/*Terminating NULL*/)*2)/*Unicode*/);

				fnl.copyValTo((pkt->wholePacket->data) +paramsBuffPos);

				paramsBuffPos+=2; //SHORT size

				
				for(USHORT i=0;i<vCardName.GetLength();i++)
				{
					pkt->wholePacket->data[paramsBuffPos++]=0;
					pkt->wholePacket->data[paramsBuffPos++]=vCardName.GetAt(i);

				}   //unicode encoder

				pkt->wholePacket->data[paramsBuffPos++]=0; //two bytes of a UNICODE NULL
				pkt->wholePacket->data[paramsBuffPos++]=0; //two bytes of a UNICODE NULL

				pkt->wholePacket->data[paramsBuffPos++] = HI_Type;
				BEUS typeL( NameHeaderLength+(( obexProfile.GetLength()+1/*Terminating NULL*/))/*ascii*/);

				typeL.copyValTo((pkt->wholePacket->data) +paramsBuffPos);

				paramsBuffPos+=2; //SHORT size

				
				for(i=0;i<obexProfile.GetLength();i++)
				{
					
					pkt->wholePacket->data[paramsBuffPos++]=obexProfile.GetAt(i);

				}   //ascii

				pkt->wholePacket->data[paramsBuffPos++]=0; //ascii null terminater
				

				pkt->wholePacket->data[paramsBuffPos++] = HI_Length;

				BEUW objSz((UWORD)this->file->GetLength());

				objSz.copyValTo((pkt->wholePacket->data) +paramsBuffPos);
				
				paramsBuffPos+=4; //WORD size


	}
	
	
	
	pkt->wholePacket->data[paramsBuffPos++] = (this->packetCount==this->nPacketsToSend-1)?HI_EndOfBody:HI_Body;


	BEUS chunkSz(BodyHeaderLength+(((this->file->GetLength() - this->chunkBuffPos) > (this->MaxOBEXPacketSize))? MaxOBEXPacketSize:(file->GetLength() - this->chunkBuffPos)));


	chunkSz.copyValTo((pkt->wholePacket->data) +paramsBuffPos);
	paramsBuffPos+=2; //SHORT size


	for(USHORT i=0;i< (chunkSz.getValUSHORT() - BodyHeaderLength/*we're just writing the contents*/) ;i++)
		this->file->Read((pkt->wholePacket->data)+(paramsBuffPos++),1);

	this->chunkBuffPos += chunkSz.getValUSHORT();


	this->packetCount++;


	return pkt;

	
	}

