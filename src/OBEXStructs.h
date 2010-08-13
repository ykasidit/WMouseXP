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
// OBEXStructs.h Structures for use in OBEX packet generation and decoding classes
// Autor: Kasidit Yusuf
/////////////////////////////////////////////////////////////////////

#ifndef OBEXSTRUCTS_H
#define OBEXSTRUCTS_H


#ifndef BYTE
#define BYTE unsigned char
#endif

#ifndef USHORT
#define USHORT unsigned short
#endif


// bit 7 and 8 in an HI can be intepreted for the params type and sequence


#define HI_Interpreter_ANDFactor 0xB0
// do bitwise AND of the HI with 0b1100 0000 (thats 0xB0: HI_Interpreter_ANDFactor) then switch the value to match the HI_Intepreters
#define OrFactorForFinalBit		 0x80
// do bitwise OR of OrFactorForFinalBit and the opcode to set the final bit (0b1000 0000)




#define HI_Intepreter_TwoByteUIntLengthThenNullTerminatedUnicodeText	0x00
#define HI_Intepreter_TwoByteUIntLengthThenByteSequence					0x40
#define HI_Intepreter_OneByteQuantity									0x80
#define HI_Intepreter_FourByteQuantity									0xC0


//HI - identifier header name Description
//don't use // in #define  becasue if will cancel out the code after insertion by preprocessor
#define HI_Count 0xC0  /*Number of objects (used by Connect)*/
#define HI_Name 0x01  /*name of the object (often a file name)*/
#define HI_Type 0x42  /*type of object - e.g. text, html, binary, manufacturer specific*/
#define HI_Length 0xC3  /*the length of the object in bytes*/
#define HI_Body 0x48  /*not-final body chunk*/
#define HI_EndOfBody 0x49  /*final body chunk*/

// Opcodes (Request codes)
#define Opcode_Connect		0x80 /*final bit always/already set*/
#define Opcode_Disconnect	0x81 /*final bit always/already set*/
#define Opcode_Put			0x02 /*set final bit manually/accordingly*/
#define Opcode_Get			0x03 /*set final bit manually/accordingly*/

//Response codes

#define RspCode_Continue				0x90	/*final bit always/already set*/
#define RspCode_Success					0xA0	/*final bit always/already set*/
#define RspCode_Created					0xA1	/*final bit always/already set*/
#define RspCode_Accepted				0xA2	/*final bit always/already set*/
#define RspCode_BadRequest				0xC0	/*final bit always/already set*/
#define RspCode_Unauthorized			0xC1	/*final bit always/already set*/
#define RspCode_Forbidden				0xC3	/*final bit always/already set*/
#define RspCode_NotFound				0xC4	/*final bit always/already set*/
#define RspCode_NotAllowed				0xC5	/*final bit always/already set*/
#define RspCode_NotAcceptable			0xC6	/*final bit always/already set*/
#define RspCode_Timeout					0xC8	/*final bit always/already set*/
#define RspCode_Conflict				0xC9	/*final bit always/already set*/
#define RspCode_EntityTooLarge			0xCD	/*final bit always/already set*/
#define RspCode_UnsupportedMediaType	0xCF	/*final bit always/already set*/
#define RspCode_DBFull					0xE0	/*final bit always/already set*/
#define RspCode_DBLocked				0xE1	/*final bit always/already set*/

#define OBEXPacketHeaderLength 3 /*code(1)+length(2)*/
#define OBEXConnectParamsLength  4 /*(ver:1 + flags:1 + maxlen:2)*/

#define NameHeaderLength 3
#define LengthHeaderAndObjectLength 5
#define BodyHeaderLength 3
#define TypeHeaderAndLengthLength	3
#define BodyChunkHeaderLength	3

#define OBEXVersion 0x10
#define OBEXFlags	0x00



typedef struct
{
	BYTE code; //opcode or response code
	BYTE packetLength[2];
	BYTE data[1]; // headers or request data

} OBEXPacketStruct, *OBEXPacketStructPtr;

typedef struct
{
	BYTE HI;	//Header ID
	BYTE HV[1];	//Header values (params)

} OBEXHeaderStruct, *OBEXHeaderStructPtr; //this goes into an obex packet


typedef struct
{
	BYTE	OBEX_Version;
	BYTE	flags;
	BYTE	MaxOBEXPacketLength[2];
	//then optional headers

}	OBEXConnectParamsStruct; // used for both conn req and conn rsp

typedef	struct
{
	BYTE	length[2];
	BYTE	unicodeText[1];

}	HV_TwoByteUIntLengthThenNullTerminatedUnicodeTextStruct;


typedef	struct
{
	BYTE	length[2];
	BYTE	byteSeq[1];

} HV_TwoByteUIntLengthThenByteSequenceStruct;

typedef	struct
{
	BYTE	contents[1];
	

} HV_OneByteQuantityStruct;

typedef	struct
{
	BYTE	contents[4];
	

} HV_FourByteQuantityStruct;



#endif