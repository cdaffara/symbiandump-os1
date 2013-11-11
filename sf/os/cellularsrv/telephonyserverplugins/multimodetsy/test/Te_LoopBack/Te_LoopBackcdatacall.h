// Copyright (c) 1997-2009 Nokia Corporation and/or its subsidiary(-ies).
// All rights reserved.
// This component and the accompanying materials are made available
// under the terms of "Eclipse Public License v1.0"
// which accompanies this distribution, and is available
// at the URL "http://www.eclipse.org/legal/epl-v10.html".
//
// Initial Contributors:
// Nokia Corporation - initial contribution.
//
// Contributors:
//
// Description:
// This file has the class definitions and scripts for the Data Call Test in the ETel Regression
// test harness.  The call scripts are processed and handled by the emulator side of the test via
// the scripting engine.  Each line in the script represents an action that the emulator must
// take, either a receive action (which defines an expected string) or a transmit action with the
// ASCII data to be sent.  See the description of the scripting language for more information.
// 
//

/**
 @file
 @internalComponent 
*/

#ifndef __CDATACALL_H__
#define __CDATACALL_H__

#include "Te_LoopbackTestStepBase.h"
#include "Te_LoopBackScriptEng.h"

_LIT8(KWriteTestData,"ABCD\r\n");
_LIT8(KWriteTestData2,"EFGH\r\n");

//
// First a Script where everything is in order
//
const struct TScript KDataCallScript[]= {
	{	ERxString,	(TText8*)"AT\r",		0, ERxEvent	},
	{	ETxStringOK, NULL,					0, ETxEvent|EFailIfTimeOut	},

	{	EIfRxStringJmp, (TText8*)"AT\r",	1, 0	},

	{	ERxString, (TText8*)"AT+FCLASS=?",	0, ERxEvent	},
	{	ETxString, (TText8*)"0,1,2\r",		0, ETxEvent|EFailIfTimeOut	},
	{	ETxStringOK, NULL,					0, ETxEvent|EFailIfTimeOut	},

//
// The initialisation string - we don't care what it is
//
	{	ERxString, (TText8*)"AT",			0, ERxEvent	},
	{	ETxStringOK, NULL,					0, ETxEvent|EFailIfTimeOut	},

	{	ERxString, (TText8*)"AT+CNMI=,0",   0, ERxEvent	},
	{	ETxStringOK, NULL,					0, ETxEvent|EFailIfTimeOut	},

	{	ERxString, (TText8*)"ATS7?",		0, ERxEvent	},
	{	ETxString, (TText8*)"050\r",		0, ETxEvent|EFailIfTimeOut	},
	{	ETxStringOK, NULL,					0, ETxEvent|EFailIfTimeOut	},

	{	ERxString, (TText8*)"ATS0=0",		0, ERxEvent	},
	{	ETxStringOK, NULL,					0, ETxEvent|EFailIfTimeOut	},

	{	ERxString, (TText8*)"AT+CGMI",	0, ERxEvent	},
	{	ETxString, (TText8*)"ERICSSON\r",	0, ETxEvent|EFailIfTimeOut	},
	{	ETxStringOK, NULL,					0, ETxEvent|EFailIfTimeOut	},

	{	ERxString, (TText8*)"AT+CGMM",	0, ERxEvent	},
	{	ETxString, (TText8*)"1100801\r",	0, ETxEvent|EFailIfTimeOut	},
	{	ETxStringOK, NULL,					0, ETxEvent|EFailIfTimeOut	},

	{	ERxString, (TText8*)"AT+CGMR",	0, ERxEvent	},
	{	ETxString, (TText8*)"9808261341\r",	0, ETxEvent|EFailIfTimeOut	},
	{	ETxStringOK, NULL,					0, ETxEvent|EFailIfTimeOut	},

	{	ERxString, (TText8*)"AT+CGSN",	0, ERxEvent	},
	{	ETxString, (TText8*)"520002512954000\r",	0, ETxEvent|EFailIfTimeOut	},
	{	ETxStringOK, NULL,					0, ETxEvent|EFailIfTimeOut	},

	{	ERxString, (TText8*)"AT+CIMI",	0, ERxEvent	},
	{	ETxString, (TText8*)"ERROR\r",	0, ETxEvent|EFailIfTimeOut	},

	{	ERxString, (TText8*)"AT+CMGF=?",	0, ERxEvent	},
	{	ETxString, (TText8*)"+CMGF: (0)\r",	0, ETxEvent|EFailIfTimeOut	},
	{	ETxStringOK, NULL,					0, ETxEvent|EFailIfTimeOut	},

	{	ERxString, (TText8*)"AT+CMGF=0",	0, ERxEvent	},
	{	ETxStringOK, NULL,					0, ETxEvent|EFailIfTimeOut	},

	{	ERxString, (TText8*)"AT+CPMS=?",	0, ERxEvent	},
	{	ETxString, (TText8*)"+CPMS: (\"ME\",\"SM\"),(\"ME\",\"SM\")\r",	0, ETxEvent|EFailIfTimeOut	},
	{	ETxStringOK, NULL,					0, ETxEvent|EFailIfTimeOut	},

	{	ERxString, (TText8*)"AT+CPMS?",	0, ERxEvent	},
	{	ETxString, (TText8*)"+CPMS: \"SM\",0,15,\"SM\",0,15\r",	0, ETxEvent|EFailIfTimeOut	},
	{	ETxStringOK, NULL,					0, ETxEvent|EFailIfTimeOut	},

	{	ERxString, (TText8*)"AT+CPBS=?",	0, ERxEvent	},
	{	ETxString, (TText8*)"+CPBS: (\"ME\",\"SM\")\r",	0, ETxEvent|EFailIfTimeOut	},
	{	ETxStringOK, NULL,					0, ETxEvent|EFailIfTimeOut	},

	{	ERxString, (TText8*)"AT+CREG=0",	0, ERxEvent	},
	{	ETxStringOK, NULL,					0, ETxEvent|EFailIfTimeOut	},

	{	ERxString, (TText8*)"AT+CREG=2",	0, ERxEvent	},
	{	ETxString, (TText8*)"ERROR\r",		0, ETxEvent|EFailIfTimeOut	},

	{	ERxString, (TText8*)"AT+CREG=1",	0, ERxEvent	},
	{	ETxStringOK, NULL,					0, ETxEvent|EFailIfTimeOut	},

	{	ERxString, (TText8*)"AT+CREG?",	0, ERxEvent	},
	{	ETxString, (TText8*)"ERROR\r",	0, ETxEvent|EFailIfTimeOut	},
	
	{	ERxString, (TText8*)"AT+CRC=1",		0, ERxEvent	},
	{	ETxStringOK, NULL,					0, ETxEvent|EFailIfTimeOut	},

	{	ERxString, (TText8*)"AT+COPS?",	0, ERxEvent	},
	{	ETxString, (TText8*)"+COPS: 0,0,\"UK VODAFONE\"\r",	0, ETxEvent|EFailIfTimeOut	},
	{	ETxStringOK, NULL,					0, ETxEvent|EFailIfTimeOut	},

	{	ERxString, (TText8*)"AT+CBC",	0, ERxEvent	},
	{	ETxString, (TText8*)"ERROR\r",		0, ETxEvent|EFailIfTimeOut	},

	{	ERxString, (TText8*)"AT+CSQ",	0, ERxEvent	},
	{	ETxString, (TText8*)"+CSQ: 31,99\r",		0, ETxEvent|EFailIfTimeOut	},
	{	ETxStringOK, NULL,				0, ETxEvent|EFailIfTimeOut	},

	{	ERxString, (TText8*)"AT+CGCLASS",	0, ERxEvent	},
	{	ETxString, (TText8*)"ERROR\r",		0, ETxEvent|EFailIfTimeOut	},

	{	ERxString, (TText8*)"AT+CGCLASS",	0, ERxEvent	},
	{	ETxString, (TText8*)"ERROR\r",		0, ETxEvent|EFailIfTimeOut	},

	{	ERxString, (TText8*)"AT+CGQREQ",	0, ERxEvent	},
	{	ETxString, (TText8*)"ERROR\r",		0, ETxEvent|EFailIfTimeOut	},

	{	ERxString, (TText8*)"AT+CGATT",	0, ERxEvent	},
	{	ETxString, (TText8*)"ERROR\r",		0, ETxEvent|EFailIfTimeOut	},

	{	ERxString, (TText8*)"AT+CNMI=?",	0, ERxEvent	},
	{	ETxString, (TText8*)"+CNMI: (0-2),(0,1,3),(0),(0),(0,1)\r",	0, ETxEvent|EFailIfTimeOut	},
	{	ETxStringOK, NULL,					0, ETxEvent|EFailIfTimeOut	},

	{	ERxString, (TText8*)"AT+CBST=?",	0, ERxEvent	},
	{	ETxString, (TText8*)"+CBST: (0-7,12,14-16,34,36,38,39,43,47-51,65,66,68,70,71,75,79-81),(0,2),(1)\r",	0, ETxEvent|EFailIfTimeOut	},
	{	ETxStringOK, NULL,					0, ETxEvent|EFailIfTimeOut	},

	{	ERxString, (TText8*)"AT+CSCA?",		0, ERxEvent	},
	{	ETxString, (TText8*)"+CSCA: \"441632960000\",145",	0, ETxEvent|EFailIfTimeOut	},
	{	ETxStringOK, NULL,					0, ETxEvent|EFailIfTimeOut	},

	{	ERxString, (TText8*)"AT+CGREG=1",	0, ERxEvent	},
	{	ETxString, (TText8*)"ERROR\r",		0, ETxEvent|EFailIfTimeOut	},

	{	ERxString, (TText8*)"AT*ECAM=1",	0, ERxEvent	},
	{	ETxString, (TText8*)"ERROR\r",		0, ETxEvent|EFailIfTimeOut	},

//
//  Now, finally, we can handle the Dial request
//
	{	ERxString, (TText8*)"AT+FCLASS=0",	0, ERxEvent	},
	{	ETxStringOK, NULL,					0, ETxEvent|EFailIfTimeOut	},

// Note: Run another test where we ERROR this request
	{	ERxString, (TText8*)"ATM0L0X3S8=0",	0, ERxEvent	},
	{	ETxStringOK, NULL,					0, ETxEvent|EFailIfTimeOut	},

// This is the Data Init String
	{	ERxString, (TText8*)"AT345678901234567890123456789012345678901234567890",	0, ERxEvent	},
	{	ETxStringOK, NULL,						0, ETxEvent|EFailIfTimeOut	},

// This is just another quick "AT", "OK" sequence to check we're still alright
	{	ERxString, (TText8*)"AT",			0, ERxEvent	},
	{	ETxStringOK, NULL,					0, ETxEvent|EFailIfTimeOut	},

	{	ERxString, (TText8*)"ATD+1234",		0, ERxEvent	},
	{	ETxString, (TText8*)"\r\nCONNECT 9600\r\n",	0, ETxEvent|EFailIfTimeOut	},

	{	ERxString, (TText8*)"ABCD\r\n",		0, ERxEvent	},
	{	ERxString, (TText8*)"ABCD\r\n",		0, ERxEvent	},

	{	ERxString, (TText8*)"AT",			0, ERxEvent	},
	{	ERxString, (TText8*)"+++",			0, ERxEvent	},
	{	ETxStringOK, NULL,					0, ETxEvent|EFailIfTimeOut	},

	{	ERxString, (TText8*)"AT",			0, ERxEvent	},
	{	ETxStringOK, NULL,					0, ETxEvent|EFailIfTimeOut	},

	{	ERxString, (TText8*)"ATO",			0, ERxEvent	},
	{	ETxString, (TText8*)"\r\nCONNECT 9600\r\n",	0, ETxEvent|EFailIfTimeOut	},
	
	{	ERxString, (TText8*)"ATH",			0, ERxEvent	},
	{	ETxStringOK, NULL,					0, ETxEvent|EFailIfTimeOut	},

//
// And now simulate an incoming call...
//
	{	EWait,		NULL,					3,	ETimeOutEvent	},
	{	ETxString, (TText8*)"\r\n+CRING: REL ASYNC\r\n",	0, ETxEvent|EFailIfTimeOut	},

// Note: Run another test where we ERROR this request and the one below
	{	ERxString, (TText8*)"AT+FCLASS=0",	0, ERxEvent	},
	{	ETxStringOK, NULL,					0, ETxEvent|EFailIfTimeOut	},

	{	ERxString, (TText8*)"ATM0L0X3S8=0",	0, ERxEvent	},
	{	ETxStringOK, NULL,					0, ETxEvent|EFailIfTimeOut	},

// This is the Data Init String
	{	ERxString, (TText8*)"AT345678901234567890123456789012345678901234567890",	0, ERxEvent	},
	{	ETxStringOK, NULL,						0, ETxEvent|EFailIfTimeOut	},
	{	ERxString, (TText8*)"AT",			0, ERxEvent	},
	{	ETxStringOK, NULL,					0, ETxEvent|EFailIfTimeOut	},

	{	ERxString, (TText8*)"ATA",			0, ERxEvent	},
	{	ETxString, (TText8*)"\r\nCONNECT 9600\r\n",	0, ETxEvent|EFailIfTimeOut	},

	{	ERxString, (TText8*)"EFGH\r\n",		0, ERxEvent	},
	{	ERxString, (TText8*)"EFGH\r\n",		0, ERxEvent	},

	{	ERxString, (TText8*)"AT",			0, ERxEvent	},
	{	ERxString, (TText8*)"+++",			0, ERxEvent	},
	{	ETxStringOK, NULL,					0, ETxEvent|EFailIfTimeOut	},

	{	ERxString, (TText8*)"AT",			0, ERxEvent	},
	{	ETxStringOK, NULL,					0, ETxEvent|EFailIfTimeOut	},

	{	ERxString, (TText8*)"ATO",			0, ERxEvent	},
	{	ETxString, (TText8*)"\r\nCONNECT 9600\r\n",	0, ETxEvent|EFailIfTimeOut	},
	
	{	ERxString, (TText8*)"ATH",			0, ERxEvent	},
	{	ETxStringOK, NULL,					0, ETxEvent|EFailIfTimeOut	},

	{	EWait,		NULL,			2, ETimeOutEvent},
	{	EComplete,	 NULL,					0, 0	}
	};

//
// Now try a Script where the modem configuration string (speaker, volume, etc.)
// is ERRORed, as are the "AT+FCLASS=xxx" commands
//
// I've seen the modem configuration string ERRORed often with mobiles since they
// have no modem monitor speaker, they tend to error this configuration request
//
// Similarly the Morola Timeports do not support fax and so error all "AT+FCLASS=xxx"
// commands, even if they relate to data calls, which are supported
//

const struct TScript KDataCallErrorAScript[]= {
	{	ERxString,	(TText8*)"AT\r",		0, ERxEvent	},
	{	ETxStringOK, NULL,					0, ETxEvent|EFailIfTimeOut	},

	{	EIfRxStringJmp, (TText8*)"AT\r",	1, 0	},

	{	ERxString, (TText8*)"AT+FCLASS=?",	0, ERxEvent	},
	{	ETxString, (TText8*)"ERROR",		0, ETxEvent|EFailIfTimeOut	},

//
// The initialisation string - we don't care what it is
//
	{	ERxString, (TText8*)"AT",			0, ERxEvent	},
	{	ETxStringOK, NULL,					0, ETxEvent|EFailIfTimeOut	},

	{	ERxString, (TText8*)"AT+CNMI=,0",   0, ERxEvent	},
	{	ETxStringOK, NULL,					0, ETxEvent|EFailIfTimeOut	},

	{	ERxString, (TText8*)"ATS7?",		0, ERxEvent	},
	{	ETxString, (TText8*)"050\r",		0, ETxEvent|EFailIfTimeOut	},
	{	ETxStringOK, NULL,					0, ETxEvent|EFailIfTimeOut	},

	{	ERxString, (TText8*)"ATS0=0",		0, ERxEvent	},
	{	ETxStringOK, NULL,					0, ETxEvent|EFailIfTimeOut	},

	{	ERxString, (TText8*)"AT+CGMI",	0, ERxEvent	},
	{	ETxString, (TText8*)"ERICSSON\r",	0, ETxEvent|EFailIfTimeOut	},
	{	ETxStringOK, NULL,					0, ETxEvent|EFailIfTimeOut	},

	{	ERxString, (TText8*)"AT+CGMM",	0, ERxEvent	},
	{	ETxString, (TText8*)"1100801\r",	0, ETxEvent|EFailIfTimeOut	},
	{	ETxStringOK, NULL,					0, ETxEvent|EFailIfTimeOut	},

	{	ERxString, (TText8*)"AT+CGMR",	0, ERxEvent	},
	{	ETxString, (TText8*)"9808261341\r",	0, ETxEvent|EFailIfTimeOut	},
	{	ETxStringOK, NULL,					0, ETxEvent|EFailIfTimeOut	},

	{	ERxString, (TText8*)"AT+CGSN",	0, ERxEvent	},
	{	ETxString, (TText8*)"520002512954000\r",	0, ETxEvent|EFailIfTimeOut	},
	{	ETxStringOK, NULL,					0, ETxEvent|EFailIfTimeOut	},

	{	ERxString, (TText8*)"AT+CIMI",	0, ERxEvent	},
	{	ETxString, (TText8*)"ERROR\r",	0, ETxEvent|EFailIfTimeOut	},

	{	ERxString, (TText8*)"AT+CMGF=?",	0, ERxEvent	},
	{	ETxString, (TText8*)"+CMGF: (0)\r",	0, ETxEvent|EFailIfTimeOut	},
	{	ETxStringOK, NULL,					0, ETxEvent|EFailIfTimeOut	},

	{	ERxString, (TText8*)"AT+CMGF=0",	0, ERxEvent	},
	{	ETxStringOK, NULL,					0, ETxEvent|EFailIfTimeOut	},

	{	ERxString, (TText8*)"AT+CPMS=?",	0, ERxEvent	},
	{	ETxString, (TText8*)"+CPMS: (\"ME\",\"SM\"),(\"ME\",\"SM\")\r",	0, ETxEvent|EFailIfTimeOut	},
	{	ETxStringOK, NULL,					0, ETxEvent|EFailIfTimeOut	},

	{	ERxString, (TText8*)"AT+CPMS?",	0, ERxEvent	},
	{	ETxString, (TText8*)"+CPMS: \"SM\",0,15,\"SM\",0,15\r",	0, ETxEvent|EFailIfTimeOut	},
	{	ETxStringOK, NULL,					0, ETxEvent|EFailIfTimeOut	},

	{	ERxString, (TText8*)"AT+CPBS=?",	0, ERxEvent	},
	{	ETxString, (TText8*)"+CPBS: (\"ME\",\"SM\")\r",	0, ETxEvent|EFailIfTimeOut	},
	{	ETxStringOK, NULL,					0, ETxEvent|EFailIfTimeOut	},

	{	ERxString, (TText8*)"AT+CREG=0",	0, ERxEvent	},
	{	ETxStringOK, NULL,					0, ETxEvent|EFailIfTimeOut	},

	{	ERxString, (TText8*)"AT+CREG=2",	0, ERxEvent	},
	{	ETxString, (TText8*)"ERROR\r",		0, ETxEvent|EFailIfTimeOut	},

	{	ERxString, (TText8*)"AT+CREG=1",	0, ERxEvent	},
	{	ETxStringOK, NULL,					0, ETxEvent|EFailIfTimeOut	},

	{	ERxString, (TText8*)"AT+CREG?",	0, ERxEvent	},
	{	ETxString, (TText8*)"ERROR\r",	0, ETxEvent|EFailIfTimeOut	},
	
	{	ERxString, (TText8*)"AT+CRC=1",		0, ERxEvent	},
	{	ETxStringOK, NULL,					0, ETxEvent|EFailIfTimeOut	},

	{	ERxString, (TText8*)"AT+COPS?",	0, ERxEvent	},
	{	ETxString, (TText8*)"+COPS: 0,0,\"UK VODAFONE\"\r",	0, ETxEvent|EFailIfTimeOut	},
	{	ETxStringOK, NULL,					0, ETxEvent|EFailIfTimeOut	},

	{	ERxString, (TText8*)"AT+CBC",	0, ERxEvent	},
	{	ETxString, (TText8*)"ERROR\r",		0, ETxEvent|EFailIfTimeOut	},

	{	ERxString, (TText8*)"AT+CSQ",	0, ERxEvent	},
	{	ETxString, (TText8*)"+CSQ: 31,99\r",		0, ETxEvent|EFailIfTimeOut	},
	{	ETxStringOK, NULL,				0, ETxEvent|EFailIfTimeOut	},

	{	ERxString, (TText8*)"AT+CGCLASS",	0, ERxEvent	},
	{	ETxString, (TText8*)"ERROR\r",		0, ETxEvent|EFailIfTimeOut	},

	{	ERxString, (TText8*)"AT+CGCLASS",	0, ERxEvent	},
	{	ETxString, (TText8*)"ERROR\r",		0, ETxEvent|EFailIfTimeOut	},

	{	ERxString, (TText8*)"AT+CGQREQ",	0, ERxEvent	},
	{	ETxString, (TText8*)"ERROR\r",		0, ETxEvent|EFailIfTimeOut	},

	{	ERxString, (TText8*)"AT+CGATT",	0, ERxEvent	},
	{	ETxString, (TText8*)"ERROR\r",		0, ETxEvent|EFailIfTimeOut	},

	{	ERxString, (TText8*)"AT+CNMI=?",	0, ERxEvent	},
	{	ETxString, (TText8*)"+CNMI: (0-2),(0,1,3),(0),(0),(0,1)\r",	0, ETxEvent|EFailIfTimeOut	},
	{	ETxStringOK, NULL,					0, ETxEvent|EFailIfTimeOut	},

	{	ERxString, (TText8*)"AT+CBST=?",	0, ERxEvent	},
	{	ETxString, (TText8*)"+CBST: (0-7,12,14-16,34,36,38,39,43,47-51,65,66,68,70,71,75,79-81),(0,2),(1)\r",	0, ETxEvent|EFailIfTimeOut	},
	{	ETxStringOK, NULL,					0, ETxEvent|EFailIfTimeOut	},

	{	ERxString, (TText8*)"AT+CSCA?",		0, ERxEvent	},
	{	ETxString, (TText8*)"+CSCA: \"441632960000\",145",	0, ETxEvent|EFailIfTimeOut	},
	{	ETxStringOK, NULL,					0, ETxEvent|EFailIfTimeOut	},

	{	ERxString, (TText8*)"AT+CGREG=1",	0, ERxEvent	},
	{	ETxString, (TText8*)"ERROR\r",		0, ETxEvent|EFailIfTimeOut	},

	{	ERxString, (TText8*)"AT*ECAM=1",	0, ERxEvent	},
	{	ETxString, (TText8*)"ERROR\r",		0, ETxEvent|EFailIfTimeOut	},

//
//  Now, finally, we can handle the Dial request
//
	{	ERxString, (TText8*)"AT+FCLASS=0",	0, ERxEvent	},
	{	ETxString, (TText8*)"ERROR",		0, ETxEvent|EFailIfTimeOut	},

// Note: Run another test where we ERROR this request
	{	ERxString, (TText8*)"ATM0L0X3S8=0",	0, ERxEvent	},
	{	ETxString, (TText8*)"ERROR",		0, ETxEvent|EFailIfTimeOut	},

// This is the Data Init String
	{	ERxString, (TText8*)"AT345678901234567890123456789012345678901234567890",	0, ERxEvent	},
	{	ETxStringOK, NULL,						0, ETxEvent|EFailIfTimeOut	},

// This is just another quick "AT", "OK" sequence to check we're still alright
	{	ERxString, (TText8*)"AT",			0, ERxEvent	},
	{	ETxStringOK, NULL,					0, ETxEvent|EFailIfTimeOut	},

	{	ERxString, (TText8*)"ATD+1234",		0, ERxEvent	},
	{	ETxString, (TText8*)"\r\nCONNECT 9600\r\n",	0, ETxEvent|EFailIfTimeOut	},

	{	ERxString, (TText8*)"ABCD\r\n",		0, ERxEvent	},
	{	ERxString, (TText8*)"ABCD\r\n",		0, ERxEvent	},

	{	ERxString, (TText8*)"AT",			0, ERxEvent	},
	{	ERxString, (TText8*)"+++",			0, ERxEvent	},
	{	ETxStringOK, NULL,					0, ETxEvent|EFailIfTimeOut	},

	{	ERxString, (TText8*)"AT",			0, ERxEvent	},
	{	ETxStringOK, NULL,					0, ETxEvent|EFailIfTimeOut	},

	{	ERxString, (TText8*)"ATO",			0, ERxEvent	},
	{	ETxString, (TText8*)"\r\nCONNECT 9600\r\n",	0, ETxEvent|EFailIfTimeOut	},
	
	{	ERxString, (TText8*)"ATH",			0, ERxEvent	},
	{	ETxStringOK, NULL,					0, ETxEvent|EFailIfTimeOut	},

//
// And now simulate an incoming call...
//
	{	EWait,		NULL,					3,	ETimeOutEvent	},
	{	ETxString, (TText8*)"\r\n+CRING: REL ASYNC\r\n",	0, ETxEvent|EFailIfTimeOut	},

// Time to ERROR these requests
	{	ERxString, (TText8*)"AT+FCLASS=0",	0, ERxEvent	},
	{	ETxString, (TText8*)"ERROR",		0, ETxEvent|EFailIfTimeOut	},

	{	ERxString, (TText8*)"ATM0L0X3S8=0",	0, ERxEvent	},
	{	ETxString, (TText8*)"ERROR",		0, ETxEvent|EFailIfTimeOut	},

// This is the Data Init String
	{	ERxString, (TText8*)"AT345678901234567890123456789012345678901234567890",	0, ERxEvent	},
	{	ETxStringOK, NULL,						0, ETxEvent|EFailIfTimeOut	},
	{	ERxString, (TText8*)"AT",			0, ERxEvent	},
	{	ETxStringOK, NULL,					0, ETxEvent|EFailIfTimeOut	},

	{	ERxString, (TText8*)"ATA",			0, ERxEvent	},
	{	ETxString, (TText8*)"\r\nCONNECT 9600\r\n",	0, ETxEvent|EFailIfTimeOut	},

	{	ERxString, (TText8*)"EFGH\r\n",		0, ERxEvent	},
	{	ERxString, (TText8*)"EFGH\r\n",		0, ERxEvent	},

	{	ERxString, (TText8*)"AT",			0, ERxEvent	},
	{	ERxString, (TText8*)"+++",			0, ERxEvent	},
	{	ETxStringOK, NULL,					0, ETxEvent|EFailIfTimeOut	},

	{	ERxString, (TText8*)"AT",			0, ERxEvent	},
	{	ETxStringOK, NULL,					0, ETxEvent|EFailIfTimeOut	},

	{	ERxString, (TText8*)"ATO",			0, ERxEvent	},
	{	ETxString, (TText8*)"\r\nCONNECT 9600\r\n",	0, ETxEvent|EFailIfTimeOut	},
	
	{	ERxString, (TText8*)"ATH",			0, ERxEvent	},
	{	ETxStringOK, NULL,					0, ETxEvent|EFailIfTimeOut	},

	{	EWait,		NULL,			2, ETimeOutEvent},
	{	EComplete,	 NULL,					0, 0	}
	};


/**
 * This class is used by the client side of the test.  It contains the method DriveETelApi
 * which is the primary client side method.  This class inherits from the CTestBase class.
 * All client side test classes must contain these entries.
 */
class CTestDriveDataCall : public CTestBase
	{
public:
	static CTestDriveDataCall* NewL(const TScriptList aScriptListEntry);
	// Called by the user interface from the C function PerformTestL.
	virtual TInt RunTestL();

protected:
	// Called by the CTestBase::StartEmulator method to drive the ETel API.
	virtual TInt DriveETelApiL();

private:
	CTestDriveDataCall(const TScriptList aScriptListEntry);
	TScriptList iScriptListEntry;	//< Initialized by the constructor, it records the enumerated
									//< script entry number.
	};


/**
 * This class is used by the emulator side of the test.  It implements several virtual functions
 * which are test specific and referenced by other classes.  This class inherits from the
 * CATScriptEng class.  All Emulator side test classes should have this structure.
 */
class CTestDataCall : public CATScriptEng
	{
public:
	static CTestDataCall* NewL(const TScript* aScript);

	// Called by the scripting engine to implement the EExecuteSpecial Command.
	virtual void SpecificAlgorithmL(TInt aParam);

	// Called by the scripting engine to end the execution of the script.
	virtual void Complete(TInt aError);

	// Called by the scripting engine to start the execution of the script.
	virtual TInt Start();

private:
	CTestDataCall(const TScript* aScript);
	void ConstructL();

	const TScript* iScript;			//< Pointer to the first line in the associated script.
	};

#endif
