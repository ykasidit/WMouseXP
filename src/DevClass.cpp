
/************************************************************************************
// Sample code for calling IVT BlueSoleil APIs

  Copyright (c) 2004 -2005 , IVT Corporation. All rights reserved.

*******************************************************************************************/

//modified for WMouseXP by Kasidit Yusuf

#include "stdafx.h"
#include "bt_def.h"



typedef struct _COD_DESC{
	DWORD dwValue;
	CHAR *lpszDesc;
}COD_DESC;

COD_DESC g_codDesc[]=
{
	{COD_SRVCLS_LIMITED,	"Limited."},
	{COD_SRVCLS_POSITION,	"Position."},
	{COD_SRVCLS_NETWORK,	"Network."},
	{COD_SRVCLS_RENDER,		"Render"},
	{COD_SRVCLS_CAPTURE,	"Capture."},
	{COD_SRVCLS_OBJECT,		"Object."},
	{COD_SRVCLS_AUDIO,		"Audio."},
	{COD_SRVCLS_TELEPHONE,	"Telephone."},
	{COD_SRVCLS_INFOR,		"Information."},
	
	{COD_DEVCLS_MISC,		"Miscellaneous."},
	{COD_DEVCLS_COMPUTER,	"Computer."},
	{COD_DEVCLS_PHONE,		"Phone."},
	{COD_DEVCLS_LAP,		"LAP."},
	{COD_DEVCLS_AUDIO,		"Audio."},
	{COD_DEVCLS_PERIPHERAL,	"Peripheral."},
	{COD_DEVCLS_IMAGE,		"Image."},
	{COD_DEVCLS_UNCLASSIFIED,"Unclassified."},

	{COD_COMPCLS_UNCLASSIFIED,	"Unclassified Computer."},
	{COD_COMPCLS_DESKTOP,	"Desktop Computer."},
	{COD_COMPCLS_SERVER,	"Server Computer."},
	{COD_COMPCLS_LAPTOP,	"Laptop Computer."},
	{COD_COMPCLS_HANDHELD,	"HandHeld Computer ."},
	{COD_COMPCLS_PALMSIZED,	"Palm-Sized Computer ."},
	{COD_COMPCLS_WEARABLE,	"Wearable Computer."},

	{COD_PHONECLS_UNCLASSIFIED,	"Unclassified Phone."},
	{COD_PHONECLS_CELLULAR,	"Cellular Phone."},
	{COD_PHONECLS_CORDLESS,	"Cordless Phone."},
	{COD_PHONECLS_SMARTPHONE,	"Smart Phone."},
	{COD_PHONECLS_WIREDMODEM,	"Wired Modem Phone."},
	{COD_PHONECLS_COMMONISDNACCESS,	"Common ISDN Access Phone."},
	{COD_PHONECLS_SIMCARDREADER,	"SIM Card Reader Phone."},

	{COD_LAP_FULL,	"Full Lap."},
	{COD_LAP_17,	"Lap 17."},
	{COD_LAP_33,	"Lap 33."},
	{COD_LAP_50,	"Lap 50."},
	{COD_LAP_67,	"Lap 67."},
	{COD_LAP_83,	"Lap 83."},
	{COD_LAP_99,	"Lap 99."},
	{COD_LAP_NOSRV,	"Lap Nosvr."},

	{COD_AV_UNCLASSIFIED,	"Unclassified AV."},
	{COD_AV_HEADSET,	"Headset AV."},
	{COD_AV_HANDSFREE,	"Hand-Free AV."},
	{COD_AV_HEADANDHAND,	"Head and Hand AV."},
	{COD_AV_MICROPHONE,	"Microphone AV."},
	{COD_AV_LOUDSPEAKER,	"Loud Speaker AV."},
	{COD_AV_HEADPHONES,	"Head Phone AV"},
	{COD_AV_PORTABLEAUDIO,	"Portable Audio AV"},
	{COD_AV_CARAUDIO,	"Car Audio AV."},
	{COD_AV_SETTOPBOX,	"Set Top Box AV."},
	{COD_AV_HIFIAUDIO,	"Hifi Audio AV."},
	{COD_AV_VCR,	"VCR AV."},
	{COD_AV_VIDEOCAMERA,	"Video Camera AV."},
	{COD_AV_CAMCORDER,	"Camcorder AV."},
	{COD_AV_VIDEOMONITOR,	"Video Monitor AV."},
	{COD_AV_VIDEODISPANDLOUDSPK,	"Video Display and Loud Speaker AV."},
	{COD_AV_VIDEOCONFERENCE,	"Video Conference AV."},
	{COD_AV_GAMEORTOY,	"Game or Toy AV."},

	{COD_PERIPHERAL_KEYBOARD,	"Keyboard Peripheral."},
	{COD_PERIPHERAL_POINT,	"Mouse/Point Peripheral."},
	{COD_PERIPHERAL_KEYORPOINT,	"Keyboard or Mouse/point Peripheral."},
	{COD_PERIPHERAL_UNCLASSIFIED,	"Unclassified Peripheral."},
	{COD_PERIPHERAL_JOYSTICK,	"Joy Stick Peripheral."},
	{COD_PERIPHERAL_GAMEPAD,	"Game Pad Peripheral."},
	{COD_PERIPHERAL_REMCONTROL,	"Rem Control Peripheral."},
	{COD_PERIPHERAL_SENSE,	"Sense Peripheral."},

	{COD_IMAGE_DISPLAY,	"Display Image."},
	{COD_IMAGE_CAMERA,	"Camera Image."},
	{COD_IMAGE_SCANNER,	"Scanner Image."},
	{COD_IMAGE_PRINTER,	"Printer Image."},

};
#define MAX_COD_DESC sizeof(g_codDesc)/sizeof(COD_DESC)

void PrintDeviceClassInfo(BYTE devClass[3])
{
	DWORD dwDevCls = *(DWORD*)devClass;
	for(int i=0; i<MAX_COD_DESC; i++){
		if(g_codDesc[i].dwValue == (dwDevCls & COD_SERVICE_CLASS_MASK))
			break;
	}
	printf("COD Service Class: %x\t%s\n", dwDevCls & COD_SERVICE_CLASS_MASK, i<MAX_COD_DESC ? g_codDesc[i].lpszDesc : "Unknown");
	for(i=0; i<MAX_COD_DESC; i++){
		if(g_codDesc[i].dwValue == (dwDevCls & COD_DEVICE_CLASS_MASK))
			break;
	}
	if(i<MAX_COD_DESC){
		printf("COD Device Class: %s\n", g_codDesc[i].lpszDesc );			
		return;
	}

	for(i=0; i<MAX_COD_DESC; i++){
		if(g_codDesc[i].dwValue == (dwDevCls & COD_MAJOR_DEVICE_CLASS_MASK))
			break;
	}
	printf("COD Major Device Class: %s\n", i<MAX_COD_DESC ? g_codDesc[i].lpszDesc : "Unknown");	
	

}