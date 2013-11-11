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
// This file has the class definitions and scripts for the "Data Call with diff.
// speed" Test in the ETel Regression test harness.
// The call scripts are processed and handled by the emulator side of the test via the
// scripting engine.  Each line in the script represents an action that the emulator must
// take, either a receive action (which defines an expected string) or a transmit action with the
// ASCII data to be sent.  See the description of the scripting language for more information.
// 
//

/**
 @file
 @internalComponent
*/

#ifndef __CDATA_CALL_DIFF_PARAM_H__
#define __CDATA_CALL_DIFF_PARAM_H__

#include "Te_LoopbackTestStepBase.h"
#include "Te_LoopBackScriptEng.h"

//_LIT8(KWriteTestRemoteHangupData,"ABCD\r\n");
//_LIT8(KWriteTestRemoteHangupData2,"EFGH\r\n");

// KCallDiffParamDefVarDelay is the default Variable Delay value when the user did NOT enter a variable
// delay value to be used as a timeout by script processing when the iParam value is set -1 in a
// TScript script whose iCommand is an EWait.
const TInt KCallDiffParamDefVarDelay=15;

// KCallDiffParamMinVarDelay is Minimum Variable Delay value that can be input by the user as the timeout
// by script processing when iParam value is set -1 in a TScript script whose iCommand is an EWait.
const TInt KCallDiffParamMinVarDelay=1;

// KCallDiffParamMaxVarDelay is Maximum Variable Delay value that can be input by the user as the timeout
// by script processing when iParam value is set -1 in a TScript script whose iCommand is an EWait.
const TInt KCallDiffParamMaxVarDelay=45;

// KCallDiffParamMaxDigits is Maximum number of digits allowed to be entered by the operator when
// Variable Delay value that can be input by the user as the timeout by script
// processing when the iParam value is set -1 in a TScript script whose iCommand is an EWait.
const TInt KCallDiffParamMaxDigits=3;

//
// First a Script where everything is in order
//
const struct TScript KCDataCallDiffParamScript[]= {
//*************************************************
//	DialDiffSpeed(RMobileCall::ESpeedUnspecified) ;
//*************************************************
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
// Initialisation complete.
//*************************************************
//	DialDiffDataService(RMobileCall::EServiceUnspecified) ; //44
//*************************************************
// Perform the Dial request
	{	ERxString, (TText8*)"AT+FCLASS=0",	0, ERxEvent	},
	{	ETxStringOK, NULL,					0, ETxEvent|EFailIfTimeOut	},

// Note: Run another test where we ERROR this request
	{	ERxString, (TText8*)"ATM1L0X4S8=4",	0, ERxEvent	},
	{	ETxStringOK, NULL,					0, ETxEvent|EFailIfTimeOut	},


	{	ERxString, (TText8*)"AT345678901234567890123456789012345678901234567890",	0, ERxEvent	},
	{	ETxStringOK, NULL,					0, ETxEvent|EFailIfTimeOut	},
// This is just another quick "AT", "OK" sequence to check we're still alright
	{	ERxString, (TText8*)"AT",			0, ERxEvent	},
	{	ETxStringOK, NULL,					0, ETxEvent|EFailIfTimeOut	},
// This is the data call parameters setup command (new for MMTSY since 13/Sep/01)
	{	ERxString, (TText8*)"AT+CBST=7",			0, ERxEvent	},
	{	ETxStringOK, NULL,					0, ETxEvent|EFailIfTimeOut	},

// Issue the dial command and wait for connection
	{	ERxString, (TText8*)"ATD+1234",		0, ERxEvent	},
// Set signal for connection
	{	ESignalMark, NULL,					KSignalDCD, 0	},
	{	ETxString, (TText8*)"\r\nCONNECT\r\n", 0, ETxEvent|EFailIfTimeOut	},

	{	ERxString, (TText8*)"+++",			0, ERxEvent	},
	{	ERxString, (TText8*)"ATH",			0, ERxEvent	},
	{	ETxStringOK, NULL,					0, ETxEvent|EFailIfTimeOut	},
//*************************************************
//	DialDiffDataService(RMobileCall::EServiceDataCircuitAsync) ; //45
//*************************************************
// Perform the Dial request
	{	ERxString, (TText8*)"AT+FCLASS=0",	0, ERxEvent	},
	{	ETxStringOK, NULL,					0, ETxEvent|EFailIfTimeOut	},

// Note: Run another test where we ERROR this request
	{	ERxString, (TText8*)"ATM1L0X4S8=4",	0, ERxEvent	},
	{	ETxStringOK, NULL,					0, ETxEvent|EFailIfTimeOut	},


	{	ERxString, (TText8*)"AT345678901234567890123456789012345678901234567890",	0, ERxEvent	},
	{	ETxStringOK, NULL,					0, ETxEvent|EFailIfTimeOut	},
// This is just another quick "AT", "OK" sequence to check we're still alright
	{	ERxString, (TText8*)"AT",			0, ERxEvent	},
	{	ETxStringOK, NULL,					0, ETxEvent|EFailIfTimeOut	},
// This is the data call parameters setup command (new for MMTSY since 13/Sep/01)
	{	ERxString, (TText8*)"AT+CBST=7,0,1",			0, ERxEvent	},
	{	ETxStringOK, NULL,					0, ETxEvent|EFailIfTimeOut	},

// Issue the dial command and wait for connection
	{	ERxString, (TText8*)"ATD+1234",		0, ERxEvent	},
// Set signal for connection
	{	ESignalMark, NULL,					KSignalDCD, 0	},
	{	ETxString, (TText8*)"\r\nCONNECT\r\n", 0, ETxEvent|EFailIfTimeOut	},

	{	ERxString, (TText8*)"+++",			0, ERxEvent	},
	{	ERxString, (TText8*)"ATH",			0, ERxEvent	},
	{	ETxStringOK, NULL,					0, ETxEvent|EFailIfTimeOut	},
//*************************************************
//	DialDiffDataService(RMobileCall::EServiceDataCircuitAsyncRdi) ; //46
//*************************************************
// Perform the Dial request
	{	ERxString, (TText8*)"AT+FCLASS=0",	0, ERxEvent	},
	{	ETxStringOK, NULL,					0, ETxEvent|EFailIfTimeOut	},

// Note: Run another test where we ERROR this request
	{	ERxString, (TText8*)"ATM1L0X4S8=4",	0, ERxEvent	},
	{	ETxStringOK, NULL,					0, ETxEvent|EFailIfTimeOut	},


	{	ERxString, (TText8*)"AT345678901234567890123456789012345678901234567890",	0, ERxEvent	},
	{	ETxStringOK, NULL,					0, ETxEvent|EFailIfTimeOut	},
// This is just another quick "AT", "OK" sequence to check we're still alright
	{	ERxString, (TText8*)"AT",			0, ERxEvent	},
	{	ETxStringOK, NULL,					0, ETxEvent|EFailIfTimeOut	},
// This is the data call parameters setup command (new for MMTSY since 13/Sep/01)
	{	ERxString, (TText8*)"AT+CBST=7,4,1",			0, ERxEvent	},
	{	ETxStringOK, NULL,					0, ETxEvent|EFailIfTimeOut	},

// Issue the dial command and wait for connection
	{	ERxString, (TText8*)"ATD+1234",		0, ERxEvent	},
// Set signal for connection
	{	ESignalMark, NULL,					KSignalDCD, 0	},
	{	ETxString, (TText8*)"\r\nCONNECT\r\n", 0, ETxEvent|EFailIfTimeOut	},

	{	ERxString, (TText8*)"+++",			0, ERxEvent	},
	{	ERxString, (TText8*)"ATH",			0, ERxEvent	},
	{	ETxStringOK, NULL,					0, ETxEvent|EFailIfTimeOut	},
//*************************************************
//	DialDiffDataService(RMobileCall::EServicePADAsyncUDI) ; //48
//*************************************************
// Perform the Dial request
	{	ERxString, (TText8*)"AT+FCLASS=0",	0, ERxEvent	},
	{	ETxStringOK, NULL,					0, ETxEvent|EFailIfTimeOut	},

// Note: Run another test where we ERROR this request
	{	ERxString, (TText8*)"ATM1L0X4S8=4",	0, ERxEvent	},
	{	ETxStringOK, NULL,					0, ETxEvent|EFailIfTimeOut	},


	{	ERxString, (TText8*)"AT345678901234567890123456789012345678901234567890",	0, ERxEvent	},
	{	ETxStringOK, NULL,					0, ETxEvent|EFailIfTimeOut	},
// This is just another quick "AT", "OK" sequence to check we're still alright
	{	ERxString, (TText8*)"AT",			0, ERxEvent	},
	{	ETxStringOK, NULL,					0, ETxEvent|EFailIfTimeOut	},
// This is the data call parameters setup command (new for MMTSY since 13/Sep/01)
	{	ERxString, (TText8*)"AT+CBST=7,2,1",			0, ERxEvent	},
	{	ETxStringOK, NULL,					0, ETxEvent|EFailIfTimeOut	},

// Issue the dial command and wait for connection
	{	ERxString, (TText8*)"ATD+1234",		0, ERxEvent	},
// Set signal for connection
	{	ESignalMark, NULL,					KSignalDCD, 0	},
	{	ETxString, (TText8*)"\r\nCONNECT\r\n", 0, ETxEvent|EFailIfTimeOut	},

	{	ERxString, (TText8*)"+++",			0, ERxEvent	},
	{	ERxString, (TText8*)"ATH",			0, ERxEvent	},
	{	ETxStringOK, NULL,					0, ETxEvent|EFailIfTimeOut	},
//*************************************************
//	DialDiffDataService(RMobileCall::EServicePADAsyncRDI) ; //49
//*************************************************
// Perform the Dial request
	{	ERxString, (TText8*)"AT+FCLASS=0",	0, ERxEvent	},
	{	ETxStringOK, NULL,					0, ETxEvent|EFailIfTimeOut	},

// Note: Run another test where we ERROR this request
	{	ERxString, (TText8*)"ATM1L0X4S8=4",	0, ERxEvent	},
	{	ETxStringOK, NULL,					0, ETxEvent|EFailIfTimeOut	},


	{	ERxString, (TText8*)"AT345678901234567890123456789012345678901234567890",	0, ERxEvent	},
	{	ETxStringOK, NULL,					0, ETxEvent|EFailIfTimeOut	},
// This is just another quick "AT", "OK" sequence to check we're still alright
	{	ERxString, (TText8*)"AT",			0, ERxEvent	},
	{	ETxStringOK, NULL,					0, ETxEvent|EFailIfTimeOut	},
// This is the data call parameters setup command (new for MMTSY since 13/Sep/01)
	{	ERxString, (TText8*)"AT+CBST=7,6,1",			0, ERxEvent	},
	{	ETxStringOK, NULL,					0, ETxEvent|EFailIfTimeOut	},

// Issue the dial command and wait for connection
	{	ERxString, (TText8*)"ATD+1234",		0, ERxEvent	},
// Set signal for connection
	{	ESignalMark, NULL,					KSignalDCD, 0	},
	{	ETxString, (TText8*)"\r\nCONNECT\r\n", 0, ETxEvent|EFailIfTimeOut	},

	{	ERxString, (TText8*)"+++",			0, ERxEvent	},
	{	ERxString, (TText8*)"ATH",			0, ERxEvent	},
	{	ETxStringOK, NULL,					0, ETxEvent|EFailIfTimeOut	},
//*************************************************
//	DialDiffDataService(RMobileCall::EServicePacketAccessSyncUDI) ; //50
//*************************************************
// Perform the Dial request
	{	ERxString, (TText8*)"AT+FCLASS=0",	0, ERxEvent	},
	{	ETxStringOK, NULL,					0, ETxEvent|EFailIfTimeOut	},

// Note: Run another test where we ERROR this request
	{	ERxString, (TText8*)"ATM1L0X4S8=4",	0, ERxEvent	},
	{	ETxStringOK, NULL,					0, ETxEvent|EFailIfTimeOut	},


	{	ERxString, (TText8*)"AT345678901234567890123456789012345678901234567890",	0, ERxEvent	},
	{	ETxStringOK, NULL,					0, ETxEvent|EFailIfTimeOut	},
// This is just another quick "AT", "OK" sequence to check we're still alright
	{	ERxString, (TText8*)"AT",			0, ERxEvent	},
	{	ETxStringOK, NULL,					0, ETxEvent|EFailIfTimeOut	},
// This is the data call parameters setup command (new for MMTSY since 13/Sep/01)
	{	ERxString, (TText8*)"AT+CBST=7,3,1",			0, ERxEvent	},
	{	ETxStringOK, NULL,					0, ETxEvent|EFailIfTimeOut	},

// Issue the dial command and wait for connection
	{	ERxString, (TText8*)"ATD+1234",		0, ERxEvent	},
// Set signal for connection
	{	ESignalMark, NULL,					KSignalDCD, 0	},
	{	ETxString, (TText8*)"\r\nCONNECT\r\n", 0, ETxEvent|EFailIfTimeOut	},

	{	ERxString, (TText8*)"+++",			0, ERxEvent	},
	{	ERxString, (TText8*)"ATH",			0, ERxEvent	},
	{	ETxStringOK, NULL,					0, ETxEvent|EFailIfTimeOut	},
//*************************************************
//	DialDiffDataService(RMobileCall::EServicePacketAccessSyncRDI) ;  //51
//*************************************************
// Perform the Dial request
	{	ERxString, (TText8*)"AT+FCLASS=0",	0, ERxEvent	},
	{	ETxStringOK, NULL,					0, ETxEvent|EFailIfTimeOut	},

// Note: Run another test where we ERROR this request
	{	ERxString, (TText8*)"ATM1L0X4S8=4",	0, ERxEvent	},
	{	ETxStringOK, NULL,					0, ETxEvent|EFailIfTimeOut	},


	{	ERxString, (TText8*)"AT345678901234567890123456789012345678901234567890",	0, ERxEvent	},
	{	ETxStringOK, NULL,					0, ETxEvent|EFailIfTimeOut	},
// This is just another quick "AT", "OK" sequence to check we're still alright
	{	ERxString, (TText8*)"AT",			0, ERxEvent	},
	{	ETxStringOK, NULL,					0, ETxEvent|EFailIfTimeOut	},
// This is the data call parameters setup command (new for MMTSY since 13/Sep/01)
	{	ERxString, (TText8*)"AT+CBST=7,7,1",			0, ERxEvent	},
	{	ETxStringOK, NULL,					0, ETxEvent|EFailIfTimeOut	},

// Issue the dial command and wait for connection
	{	ERxString, (TText8*)"ATD+1234",		0, ERxEvent	},
// Set signal for connection
	{	ESignalMark, NULL,					KSignalDCD, 0	},
	{	ETxString, (TText8*)"\r\nCONNECT\r\n", 0, ETxEvent|EFailIfTimeOut	},

	{	ERxString, (TText8*)"+++",			0, ERxEvent	},
	{	ERxString, (TText8*)"ATH",			0, ERxEvent	},
	{	ETxStringOK, NULL,					0, ETxEvent|EFailIfTimeOut	},

//*************************************************
//	DialDiffDataService(RMobileCall::EServiceDataCircuitSyncRdi) ; //47
//*************************************************
// Perform the Dial request
	{	ERxString, (TText8*)"AT+FCLASS=0",	0, ERxEvent	},
	{	ETxStringOK, NULL,					0, ETxEvent|EFailIfTimeOut	},

// Note: Run another test where we ERROR this request
	{	ERxString, (TText8*)"ATM1L0X4S8=4",	0, ERxEvent	},
	{	ETxStringOK, NULL,					0, ETxEvent|EFailIfTimeOut	},


	{	ERxString, (TText8*)"AT345678901234567890123456789012345678901234567890",	0, ERxEvent	},
	{	ETxStringOK, NULL,					0, ETxEvent|EFailIfTimeOut	},
// This is just another quick "AT", "OK" sequence to check we're still alright
	{	ERxString, (TText8*)"AT",			0, ERxEvent	},
	{	ETxStringOK, NULL,					0, ETxEvent|EFailIfTimeOut	},
// This is the data call parameters setup command (new for MMTSY since 13/Sep/01)
	{	ERxString, (TText8*)"AT+CBST=7,5,1",			0, ERxEvent	},
	{	ETxStringOK, NULL,					0, ETxEvent|EFailIfTimeOut	},

// Issue the dial command and wait for connection
	{	ERxString, (TText8*)"ATD+1234",		0, ERxEvent	},
// Set signal for connection
	{	ESignalMark, NULL,					KSignalDCD, 0	},
	{	ETxString, (TText8*)"\r\nCONNECT\r\n", 0, ETxEvent|EFailIfTimeOut	},

	{	ERxString, (TText8*)"+++",			0, ERxEvent	},
	{	ERxString, (TText8*)"ATH",			0, ERxEvent	},
	{	ETxStringOK, NULL,					0, ETxEvent|EFailIfTimeOut	},
//*************************************************
//	DialDiffSpeed(RMobileCall::ESpeedUnspecified) ;
//*************************************************
// Perform the Dial request
//
	{	ERxString, (TText8*)"AT+FCLASS=0",	0, ERxEvent	},
	{	ETxStringOK, NULL,					0, ETxEvent|EFailIfTimeOut	},

// Note: Run another test where we ERROR this request
	{	ERxString, (TText8*)"ATM1L0X4S8=4",	0, ERxEvent	},
	{	ETxStringOK, NULL,					0, ETxEvent|EFailIfTimeOut	},

// This is the Data Init String
	{	ERxString, (TText8*)"AT345678901234567890123456789012345678901234567890",	0, ERxEvent	},
	{	ETxStringOK, NULL,						0, ETxEvent|EFailIfTimeOut	},

// This is just another quick "AT", "OK" sequence to check we're still alright
	{	ERxString, (TText8*)"AT",			0, ERxEvent	},
	{	ETxStringOK, NULL,					0, ETxEvent|EFailIfTimeOut	},


// Issue the dial command and wait for connection
	{	ERxString, (TText8*)"ATD+1234",		0, ERxEvent	},
// Set signal for connection
	{	ESignalMark, NULL,					KSignalDCD, 0	},
	{	ETxString, (TText8*)"\r\nCONNECT\r\n", 0, ETxEvent|EFailIfTimeOut	},

	{	ERxString, (TText8*)"+++",			0, ERxEvent	},
	{	ERxString, (TText8*)"ATH",			0, ERxEvent	},
	{	ETxStringOK, NULL,					0, ETxEvent|EFailIfTimeOut	},

//*************************************************
//	DialDiffSpeed(RMobileCall::ESpeedAutobauding) ;
//*************************************************
//
// Initialisation complete.
// Perform the Dial request
//
	{	ERxString, (TText8*)"AT+FCLASS=0",	0, ERxEvent	},
	{	ETxStringOK, NULL,					0, ETxEvent|EFailIfTimeOut	},

// Note: Run another test where we ERROR this request
	{	ERxString, (TText8*)"ATM1L0X4S8=4",	0, ERxEvent	},
	{	ETxStringOK, NULL,					0, ETxEvent|EFailIfTimeOut	},
// This is the data call parameters setup command (new for MMTSY since 13/Sep/01)
	{	ERxString, (TText8*)"AT345678901234567890123456789012345678901234567890",	0, ERxEvent	},
	{	ETxStringOK, NULL,					0, ETxEvent|EFailIfTimeOut	},
// This is just another quick "AT", "OK" sequence to check we're still alright
	{	ERxString, (TText8*)"AT",			0, ERxEvent	},
	{	ETxStringOK, NULL,					0, ETxEvent|EFailIfTimeOut	},

// This is the data call parameters setup command (new for MMTSY since 13/Sep/01)
	{	ERxString, (TText8*)"AT+CBST=0,0,1",			0, ERxEvent	},
	{	ETxStringOK, NULL,					0, ETxEvent|EFailIfTimeOut	},
// Issue the dial command and wait for connection
	{	ERxString, (TText8*)"ATD+1234",		0, ERxEvent	},
// Set signal for connection
	{	ESignalMark, NULL,					KSignalDCD, 0	},
	{	ETxString, (TText8*)"\r\nCONNECT\r\n", 0, ETxEvent|EFailIfTimeOut	},

	{	ERxString, (TText8*)"+++",			0, ERxEvent	},
	{	ERxString, (TText8*)"ATH",			0, ERxEvent	},
	{	ETxStringOK, NULL,					0, ETxEvent|EFailIfTimeOut	},

//*************************************************
//	DialDiffSpeed(RMobileCall::ESpeed2400, RMobileCall::EProtocolV22bis) ; //4
//*************************************************
	{	ERxString, (TText8*)"AT+FCLASS=0",	0, ERxEvent	},
	{	ETxStringOK, NULL,					0, ETxEvent|EFailIfTimeOut	},

// Note: Run another test where we ERROR this request
	{	ERxString, (TText8*)"ATM1L0X4S8=4",	0, ERxEvent	},
	{	ETxStringOK, NULL,					0, ETxEvent|EFailIfTimeOut	},
	{	ERxString, (TText8*)"AT345678901234567890123456789012345678901234567890",	0, ERxEvent	},
	{	ETxStringOK, NULL,					0, ETxEvent|EFailIfTimeOut	},
// This is just another quick "AT", "OK" sequence to check we're still alright
	{	ERxString, (TText8*)"AT",			0, ERxEvent	},
	{	ETxStringOK, NULL,					0, ETxEvent|EFailIfTimeOut	},
// This is the data call parameters setup command (new for MMTSY since 13/Sep/01)
	{	ERxString, (TText8*)"AT+CBST=4,0,1",			0, ERxEvent	},
	{	ETxStringOK, NULL,					0, ETxEvent|EFailIfTimeOut	},

// Issue the dial command and wait for connection
	{	ERxString, (TText8*)"ATD+1234",		0, ERxEvent	},
// Set signal for connection
	{	ESignalMark, NULL,					KSignalDCD, 0	},
	{	ETxString, (TText8*)"\r\nCONNECT\r\n", 0, ETxEvent|EFailIfTimeOut	},

	{	ERxString, (TText8*)"+++",			0, ERxEvent	},
	{	ERxString, (TText8*)"ATH",			0, ERxEvent	},
	{	ETxStringOK, NULL,					0, ETxEvent|EFailIfTimeOut	},
//*************************************************
//	DialDiffSpeed(RMobileCall::ESpeed2400, RMobileCall::EProtocolV110) ; //5
//*************************************************
	{	ERxString, (TText8*)"AT+FCLASS=0",	0, ERxEvent	},
	{	ETxStringOK, NULL,					0, ETxEvent|EFailIfTimeOut	},

// Note: Run another test where we ERROR this request
	{	ERxString, (TText8*)"ATM1L0X4S8=4",	0, ERxEvent	},
	{	ETxStringOK, NULL,					0, ETxEvent|EFailIfTimeOut	},
	{	ERxString, (TText8*)"AT345678901234567890123456789012345678901234567890",	0, ERxEvent	},
	{	ETxStringOK, NULL,					0, ETxEvent|EFailIfTimeOut	},
// This is just another quick "AT", "OK" sequence to check we're still alright
	{	ERxString, (TText8*)"AT",			0, ERxEvent	},
	{	ETxStringOK, NULL,					0, ETxEvent|EFailIfTimeOut	},
// This is the data call parameters setup command (new for MMTSY since 13/Sep/01)
	{	ERxString, (TText8*)"AT+CBST=68,0,1",			0, ERxEvent	},
	{	ETxStringOK, NULL,					0, ETxEvent|EFailIfTimeOut	},

// Issue the dial command and wait for connection
	{	ERxString, (TText8*)"ATD+1234",		0, ERxEvent	},
// Set signal for connection
	{	ESignalMark, NULL,					KSignalDCD, 0	},
	{	ETxString, (TText8*)"\r\nCONNECT\r\n", 0, ETxEvent|EFailIfTimeOut	},

	{	ERxString, (TText8*)"+++",			0, ERxEvent	},
	{	ERxString, (TText8*)"ATH",			0, ERxEvent	},
	{	ETxStringOK, NULL,					0, ETxEvent|EFailIfTimeOut	},
//*************************************************
//	DialDiffSpeed(RMobileCall::ESpeed2400, RMobileCall::EProtocolV120) ; //6
//*************************************************
	{	ERxString, (TText8*)"AT+FCLASS=0",	0, ERxEvent	},
	{	ETxStringOK, NULL,					0, ETxEvent|EFailIfTimeOut	},

// Note: Run another test where we ERROR this request
	{	ERxString, (TText8*)"ATM1L0X4S8=4",	0, ERxEvent	},
	{	ETxStringOK, NULL,					0, ETxEvent|EFailIfTimeOut	},
	{	ERxString, (TText8*)"AT345678901234567890123456789012345678901234567890",	0, ERxEvent	},
	{	ETxStringOK, NULL,					0, ETxEvent|EFailIfTimeOut	},
// This is just another quick "AT", "OK" sequence to check we're still alright
	{	ERxString, (TText8*)"AT",			0, ERxEvent	},
	{	ETxStringOK, NULL,					0, ETxEvent|EFailIfTimeOut	},
// This is the data call parameters setup command (new for MMTSY since 13/Sep/01)
	{	ERxString, (TText8*)"AT+CBST=36,0,1",			0, ERxEvent	},
	{	ETxStringOK, NULL,					0, ETxEvent|EFailIfTimeOut	},

// Issue the dial command and wait for connection
	{	ERxString, (TText8*)"ATD+1234",		0, ERxEvent	},
// Set signal for connection
	{	ESignalMark, NULL,					KSignalDCD, 0	},
	{	ETxString, (TText8*)"\r\nCONNECT\r\n", 0, ETxEvent|EFailIfTimeOut	},

	{	ERxString, (TText8*)"+++",			0, ERxEvent	},
	{	ERxString, (TText8*)"ATH",			0, ERxEvent	},
	{	ETxStringOK, NULL,					0, ETxEvent|EFailIfTimeOut	},
//*************************************************
//	DialDiffSpeed(RMobileCall::ESpeed2400, RMobileCall::EProtocolX31FlagStuffing) ; //6
//*************************************************
	{	ERxString, (TText8*)"AT+FCLASS=0",	0, ERxEvent	},
	{	ETxStringOK, NULL,					0, ETxEvent|EFailIfTimeOut	},

// Note: Run another test where we ERROR this request
	{	ERxString, (TText8*)"ATM1L0X4S8=4",	0, ERxEvent	},
	{	ETxStringOK, NULL,					0, ETxEvent|EFailIfTimeOut	},
	{	ERxString, (TText8*)"AT345678901234567890123456789012345678901234567890",	0, ERxEvent	},
	{	ETxStringOK, NULL,					0, ETxEvent|EFailIfTimeOut	},
// This is just another quick "AT", "OK" sequence to check we're still alright
	{	ERxString, (TText8*)"AT",			0, ERxEvent	},
	{	ETxStringOK, NULL,					0, ETxEvent|EFailIfTimeOut	},
// This is the data call parameters setup command (new for MMTSY since 13/Sep/01)
	{	ERxString, (TText8*)"AT+CBST=68,0,1",			0, ERxEvent	},
	{	ETxStringOK, NULL,					0, ETxEvent|EFailIfTimeOut	},

// Issue the dial command and wait for connection
	{	ERxString, (TText8*)"ATD+1234",		0, ERxEvent	},
// Set signal for connection
	{	ESignalMark, NULL,					KSignalDCD, 0	},
	{	ETxString, (TText8*)"\r\nCONNECT\r\n", 0, ETxEvent|EFailIfTimeOut	},

	{	ERxString, (TText8*)"+++",			0, ERxEvent	},
	{	ERxString, (TText8*)"ATH",			0, ERxEvent	},
	{	ETxStringOK, NULL,					0, ETxEvent|EFailIfTimeOut	},
//*************************************************
//	DialDiffSpeed(RMobileCall::ESpeed4800, RMobileCall::EProtocolV32) ;	//6
//*************************************************
// Perform the Dial request
	{	ERxString, (TText8*)"AT+FCLASS=0",	0, ERxEvent	},
	{	ETxStringOK, NULL,					0, ETxEvent|EFailIfTimeOut	},

// Note: Run another test where we ERROR this request
	{	ERxString, (TText8*)"ATM1L0X4S8=4",	0, ERxEvent	},
	{	ETxStringOK, NULL,					0, ETxEvent|EFailIfTimeOut	},
	{	ERxString, (TText8*)"AT345678901234567890123456789012345678901234567890",	0, ERxEvent	},
	{	ETxStringOK, NULL,					0, ETxEvent|EFailIfTimeOut	},
// This is just another quick "AT", "OK" sequence to check we're still alright
	{	ERxString, (TText8*)"AT",			0, ERxEvent	},
	{	ETxStringOK, NULL,					0, ETxEvent|EFailIfTimeOut	},
// This is the data call parameters setup command (new for MMTSY since 13/Sep/01)
	{	ERxString, (TText8*)"AT+CBST=6,0,1",			0, ERxEvent	},
	{	ETxStringOK, NULL,					0, ETxEvent|EFailIfTimeOut	},

// Issue the dial command and wait for connection
	{	ERxString, (TText8*)"ATD+1234",		0, ERxEvent	},
// Set signal for connection
	{	ESignalMark, NULL,					KSignalDCD, 0	},
	{	ETxString, (TText8*)"\r\nCONNECT\r\n", 0, ETxEvent|EFailIfTimeOut	},

	{	ERxString, (TText8*)"+++",			0, ERxEvent	},
	{	ERxString, (TText8*)"ATH",			0, ERxEvent	},
	{	ETxStringOK, NULL,					0, ETxEvent|EFailIfTimeOut	},
//*************************************************
//	DialDiffSpeed(RMobileCall::ESpeed4800, RMobileCall::EProtocolV110) ;//7
//*************************************************
// Perform the Dial request
	{	ERxString, (TText8*)"AT+FCLASS=0",	0, ERxEvent	},
	{	ETxStringOK, NULL,					0, ETxEvent|EFailIfTimeOut	},

// Note: Run another test where we ERROR this request
	{	ERxString, (TText8*)"ATM1L0X4S8=4",	0, ERxEvent	},
	{	ETxStringOK, NULL,					0, ETxEvent|EFailIfTimeOut	},
	{	ERxString, (TText8*)"AT345678901234567890123456789012345678901234567890",	0, ERxEvent	},
	{	ETxStringOK, NULL,					0, ETxEvent|EFailIfTimeOut	},
// This is just another quick "AT", "OK" sequence to check we're still alright
	{	ERxString, (TText8*)"AT",			0, ERxEvent	},
	{	ETxStringOK, NULL,					0, ETxEvent|EFailIfTimeOut	},
// This is the data call parameters setup command (new for MMTSY since 13/Sep/01)
	{	ERxString, (TText8*)"AT+CBST=70,0,1",			0, ERxEvent	},
	{	ETxStringOK, NULL,					0, ETxEvent|EFailIfTimeOut	},

// Issue the dial command and wait for connection
	{	ERxString, (TText8*)"ATD+1234",		0, ERxEvent	},
// Set signal for connection
	{	ESignalMark, NULL,					KSignalDCD, 0	},
	{	ETxString, (TText8*)"\r\nCONNECT\r\n", 0, ETxEvent|EFailIfTimeOut	},

	{	ERxString, (TText8*)"+++",			0, ERxEvent	},
	{	ERxString, (TText8*)"ATH",			0, ERxEvent	},
	{	ETxStringOK, NULL,					0, ETxEvent|EFailIfTimeOut	},
//*************************************************
//	DialDiffSpeed(RMobileCall::ESpeed4800, RMobileCall::EProtocolV120) ;//8
//*************************************************
// Perform the Dial request
	{	ERxString, (TText8*)"AT+FCLASS=0",	0, ERxEvent	},
	{	ETxStringOK, NULL,					0, ETxEvent|EFailIfTimeOut	},

// Note: Run another test where we ERROR this request
	{	ERxString, (TText8*)"ATM1L0X4S8=4",	0, ERxEvent	},
	{	ETxStringOK, NULL,					0, ETxEvent|EFailIfTimeOut	},
	{	ERxString, (TText8*)"AT345678901234567890123456789012345678901234567890",	0, ERxEvent	},
	{	ETxStringOK, NULL,					0, ETxEvent|EFailIfTimeOut	},
// This is just another quick "AT", "OK" sequence to check we're still alright
	{	ERxString, (TText8*)"AT",			0, ERxEvent	},
	{	ETxStringOK, NULL,					0, ETxEvent|EFailIfTimeOut	},
// This is the data call parameters setup command (new for MMTSY since 13/Sep/01)
	{	ERxString, (TText8*)"AT+CBST=38,0,1",			0, ERxEvent	},
	{	ETxStringOK, NULL,					0, ETxEvent|EFailIfTimeOut	},

// Issue the dial command and wait for connection
	{	ERxString, (TText8*)"ATD+1234",		0, ERxEvent	},
// Set signal for connection
	{	ESignalMark, NULL,					KSignalDCD, 0	},
	{	ETxString, (TText8*)"\r\nCONNECT\r\n", 0, ETxEvent|EFailIfTimeOut	},

	{	ERxString, (TText8*)"+++",			0, ERxEvent	},
	{	ERxString, (TText8*)"ATH",			0, ERxEvent	},
	{	ETxStringOK, NULL,					0, ETxEvent|EFailIfTimeOut	},
//*************************************************
//	DialDiffSpeed(RMobileCall::ESpeed4800, RMobileCall::EProtocolX31FlagStuffing) ; //6
//*************************************************
	{	ERxString, (TText8*)"AT+FCLASS=0",	0, ERxEvent	},
	{	ETxStringOK, NULL,					0, ETxEvent|EFailIfTimeOut	},

// Note: Run another test where we ERROR this request
	{	ERxString, (TText8*)"ATM1L0X4S8=4",	0, ERxEvent	},
	{	ETxStringOK, NULL,					0, ETxEvent|EFailIfTimeOut	},
	{	ERxString, (TText8*)"AT345678901234567890123456789012345678901234567890",	0, ERxEvent	},
	{	ETxStringOK, NULL,					0, ETxEvent|EFailIfTimeOut	},
// This is just another quick "AT", "OK" sequence to check we're still alright
	{	ERxString, (TText8*)"AT",			0, ERxEvent	},
	{	ETxStringOK, NULL,					0, ETxEvent|EFailIfTimeOut	},
// This is the data call parameters setup command (new for MMTSY since 13/Sep/01)
	{	ERxString, (TText8*)"AT+CBST=70,0,1",			0, ERxEvent	},
	{	ETxStringOK, NULL,					0, ETxEvent|EFailIfTimeOut	},

// Issue the dial command and wait for connection
	{	ERxString, (TText8*)"ATD+1234",		0, ERxEvent	},
// Set signal for connection
	{	ESignalMark, NULL,					KSignalDCD, 0	},
	{	ETxString, (TText8*)"\r\nCONNECT\r\n", 0, ETxEvent|EFailIfTimeOut	},

	{	ERxString, (TText8*)"+++",			0, ERxEvent	},
	{	ERxString, (TText8*)"ATH",			0, ERxEvent	},
	{	ETxStringOK, NULL,					0, ETxEvent|EFailIfTimeOut	},
//*************************************************
//	DialDiffSpeed(RMobileCall::ESpeed9600, RMobileCall::EProtocolV32) ;	//9
//*************************************************
// Perform the Dial request
	{	ERxString, (TText8*)"AT+FCLASS=0",	0, ERxEvent	},
	{	ETxStringOK, NULL,					0, ETxEvent|EFailIfTimeOut	},

// Note: Run another test where we ERROR this request
	{	ERxString, (TText8*)"ATM1L0X4S8=4",	0, ERxEvent	},
	{	ETxStringOK, NULL,					0, ETxEvent|EFailIfTimeOut	},

	{	ERxString, (TText8*)"AT345678901234567890123456789012345678901234567890",	0, ERxEvent	},
	{	ETxStringOK, NULL,					0, ETxEvent|EFailIfTimeOut	},
// This is just another quick "AT", "OK" sequence to check we're still alright
	{	ERxString, (TText8*)"AT",			0, ERxEvent	},
	{	ETxStringOK, NULL,					0, ETxEvent|EFailIfTimeOut	},
// This is the data call parameters setup command (new for MMTSY since 13/Sep/01)
	{	ERxString, (TText8*)"AT+CBST=7,0,1",			0, ERxEvent	},
	{	ETxStringOK, NULL,					0, ETxEvent|EFailIfTimeOut	},

// Issue the dial command and wait for connection
	{	ERxString, (TText8*)"ATD+1234",		0, ERxEvent	},
// Set signal for connection
	{	ESignalMark, NULL,					KSignalDCD, 0	},
	{	ETxString, (TText8*)"\r\nCONNECT\r\n", 0, ETxEvent|EFailIfTimeOut	},

	{	ERxString, (TText8*)"+++",			0, ERxEvent	},
	{	ERxString, (TText8*)"ATH",			0, ERxEvent	},
	{	ETxStringOK, NULL,					0, ETxEvent|EFailIfTimeOut	},
//*************************************************
//	DialDiffSpeed(RMobileCall::ESpeed9600, RMobileCall::EProtocolV34) ;	//10
//*************************************************
// Perform the Dial request
	{	ERxString, (TText8*)"AT+FCLASS=0",	0, ERxEvent	},
	{	ETxStringOK, NULL,					0, ETxEvent|EFailIfTimeOut	},

// Note: Run another test where we ERROR this request
	{	ERxString, (TText8*)"ATM1L0X4S8=4",	0, ERxEvent	},
	{	ETxStringOK, NULL,					0, ETxEvent|EFailIfTimeOut	},

	{	ERxString, (TText8*)"AT345678901234567890123456789012345678901234567890",	0, ERxEvent	},
	{	ETxStringOK, NULL,					0, ETxEvent|EFailIfTimeOut	},
// This is just another quick "AT", "OK" sequence to check we're still alright
	{	ERxString, (TText8*)"AT",			0, ERxEvent	},
	{	ETxStringOK, NULL,					0, ETxEvent|EFailIfTimeOut	},
// This is the data call parameters setup command (new for MMTSY since 13/Sep/01)
	{	ERxString, (TText8*)"AT+CBST=12,0,1",			0, ERxEvent	},
	{	ETxStringOK, NULL,					0, ETxEvent|EFailIfTimeOut	},

// Issue the dial command and wait for connection
	{	ERxString, (TText8*)"ATD+1234",		0, ERxEvent	},
// Set signal for connection
	{	ESignalMark, NULL,					KSignalDCD, 0	},
	{	ETxString, (TText8*)"\r\nCONNECT\r\n", 0, ETxEvent|EFailIfTimeOut	},

	{	ERxString, (TText8*)"+++",			0, ERxEvent	},
	{	ERxString, (TText8*)"ATH",			0, ERxEvent	},
	{	ETxStringOK, NULL,					0, ETxEvent|EFailIfTimeOut	},
//*************************************************
//	DialDiffSpeed(RMobileCall::ESpeed9600, RMobileCall::EProtocolV110) ;//11
//*************************************************
// Perform the Dial request
	{	ERxString, (TText8*)"AT+FCLASS=0",	0, ERxEvent	},
	{	ETxStringOK, NULL,					0, ETxEvent|EFailIfTimeOut	},

// Note: Run another test where we ERROR this request
	{	ERxString, (TText8*)"ATM1L0X4S8=4",	0, ERxEvent	},
	{	ETxStringOK, NULL,					0, ETxEvent|EFailIfTimeOut	},

	{	ERxString, (TText8*)"AT345678901234567890123456789012345678901234567890",	0, ERxEvent	},
	{	ETxStringOK, NULL,					0, ETxEvent|EFailIfTimeOut	},
// This is just another quick "AT", "OK" sequence to check we're still alright
	{	ERxString, (TText8*)"AT",			0, ERxEvent	},
	{	ETxStringOK, NULL,					0, ETxEvent|EFailIfTimeOut	},
// This is the data call parameters setup command (new for MMTSY since 13/Sep/01)
	{	ERxString, (TText8*)"AT+CBST=71,0,1",			0, ERxEvent	},
	{	ETxStringOK, NULL,					0, ETxEvent|EFailIfTimeOut	},

// Issue the dial command and wait for connection
	{	ERxString, (TText8*)"ATD+1234",		0, ERxEvent	},
// Set signal for connection
	{	ESignalMark, NULL,					KSignalDCD, 0	},
	{	ETxString, (TText8*)"\r\nCONNECT\r\n", 0, ETxEvent|EFailIfTimeOut	},

	{	ERxString, (TText8*)"+++",			0, ERxEvent	},
	{	ERxString, (TText8*)"ATH",			0, ERxEvent	},
	{	ETxStringOK, NULL,					0, ETxEvent|EFailIfTimeOut	},
//*************************************************
//	DialDiffSpeed(RMobileCall::ESpeed9600, RMobileCall::EProtocolV120) ;//12
//*************************************************
// Perform the Dial request
	{	ERxString, (TText8*)"AT+FCLASS=0",	0, ERxEvent	},
	{	ETxStringOK, NULL,					0, ETxEvent|EFailIfTimeOut	},

// Note: Run another test where we ERROR this request
	{	ERxString, (TText8*)"ATM1L0X4S8=4",	0, ERxEvent	},
	{	ETxStringOK, NULL,					0, ETxEvent|EFailIfTimeOut	},

	{	ERxString, (TText8*)"AT345678901234567890123456789012345678901234567890",	0, ERxEvent	},
	{	ETxStringOK, NULL,					0, ETxEvent|EFailIfTimeOut	},
// This is just another quick "AT", "OK" sequence to check we're still alright
	{	ERxString, (TText8*)"AT",			0, ERxEvent	},
	{	ETxStringOK, NULL,					0, ETxEvent|EFailIfTimeOut	},
// This is the data call parameters setup command (new for MMTSY since 13/Sep/01)
	{	ERxString, (TText8*)"AT+CBST=39,0,1",			0, ERxEvent	},
	{	ETxStringOK, NULL,					0, ETxEvent|EFailIfTimeOut	},

// Issue the dial command and wait for connection
	{	ERxString, (TText8*)"ATD+1234",		0, ERxEvent	},
// Set signal for connection
	{	ESignalMark, NULL,					KSignalDCD, 0	},
	{	ETxString, (TText8*)"\r\nCONNECT\r\n", 0, ETxEvent|EFailIfTimeOut	},

	{	ERxString, (TText8*)"+++",			0, ERxEvent	},
	{	ERxString, (TText8*)"ATH",			0, ERxEvent	},
	{	ETxStringOK, NULL,					0, ETxEvent|EFailIfTimeOut	},
//*************************************************
//	DialDiffSpeed(RMobileCall::ESpeed9600, RMobileCall::EProtocolX31FlagStuffing) ; //6
//*************************************************
	{	ERxString, (TText8*)"AT+FCLASS=0",	0, ERxEvent	},
	{	ETxStringOK, NULL,					0, ETxEvent|EFailIfTimeOut	},

// Note: Run another test where we ERROR this request
	{	ERxString, (TText8*)"ATM1L0X4S8=4",	0, ERxEvent	},
	{	ETxStringOK, NULL,					0, ETxEvent|EFailIfTimeOut	},
	{	ERxString, (TText8*)"AT345678901234567890123456789012345678901234567890",	0, ERxEvent	},
	{	ETxStringOK, NULL,					0, ETxEvent|EFailIfTimeOut	},
// This is just another quick "AT", "OK" sequence to check we're still alright
	{	ERxString, (TText8*)"AT",			0, ERxEvent	},
	{	ETxStringOK, NULL,					0, ETxEvent|EFailIfTimeOut	},
// This is the data call parameters setup command (new for MMTSY since 13/Sep/01)
	{	ERxString, (TText8*)"AT+CBST=71,0,1",			0, ERxEvent	},
	{	ETxStringOK, NULL,					0, ETxEvent|EFailIfTimeOut	},

// Issue the dial command and wait for connection
	{	ERxString, (TText8*)"ATD+1234",		0, ERxEvent	},
// Set signal for connection
	{	ESignalMark, NULL,					KSignalDCD, 0	},
	{	ETxString, (TText8*)"\r\nCONNECT\r\n", 0, ETxEvent|EFailIfTimeOut	},

	{	ERxString, (TText8*)"+++",			0, ERxEvent	},
	{	ERxString, (TText8*)"ATH",			0, ERxEvent	},
	{	ETxStringOK, NULL,					0, ETxEvent|EFailIfTimeOut	},
//*************************************************
//	DialDiffSpeed(RMobileCall::ESpeed14400, RMobileCall::EProtocolV34) ; //13
//*************************************************
// Perform the Dial request
	{	ERxString, (TText8*)"AT+FCLASS=0",	0, ERxEvent	},
	{	ETxStringOK, NULL,					0, ETxEvent|EFailIfTimeOut	},

// Note: Run another test where we ERROR this request
	{	ERxString, (TText8*)"ATM1L0X4S8=4",	0, ERxEvent	},
	{	ETxStringOK, NULL,					0, ETxEvent|EFailIfTimeOut	},


	{	ERxString, (TText8*)"AT345678901234567890123456789012345678901234567890",	0, ERxEvent	},
	{	ETxStringOK, NULL,					0, ETxEvent|EFailIfTimeOut	},
// This is just another quick "AT", "OK" sequence to check we're still alright
	{	ERxString, (TText8*)"AT",			0, ERxEvent	},
	{	ETxStringOK, NULL,					0, ETxEvent|EFailIfTimeOut	},

// This is the data call parameters setup command (new for MMTSY since 13/Sep/01)
	{	ERxString, (TText8*)"AT+CBST=14,0,1",			0, ERxEvent	},
	{	ETxStringOK, NULL,					0, ETxEvent|EFailIfTimeOut	},

// Issue the dial command and wait for connection
	{	ERxString, (TText8*)"ATD+1234",		0, ERxEvent	},
// Set signal for connection
	{	ESignalMark, NULL,					KSignalDCD, 0	},
	{	ETxString, (TText8*)"\r\nCONNECT\r\n", 0, ETxEvent|EFailIfTimeOut	},

	{	ERxString, (TText8*)"+++",			0, ERxEvent	},
	{	ERxString, (TText8*)"ATH",			0, ERxEvent	},
	{	ETxStringOK, NULL,					0, ETxEvent|EFailIfTimeOut	},
//*************************************************
//	DialDiffSpeed(RMobileCall::ESpeed14400, RMobileCall::EProtocolV110) ; //14
//*************************************************
// Perform the Dial request
	{	ERxString, (TText8*)"AT+FCLASS=0",	0, ERxEvent	},
	{	ETxStringOK, NULL,					0, ETxEvent|EFailIfTimeOut	},

// Note: Run another test where we ERROR this request
	{	ERxString, (TText8*)"ATM1L0X4S8=4",	0, ERxEvent	},
	{	ETxStringOK, NULL,					0, ETxEvent|EFailIfTimeOut	},


	{	ERxString, (TText8*)"AT345678901234567890123456789012345678901234567890",	0, ERxEvent	},
	{	ETxStringOK, NULL,					0, ETxEvent|EFailIfTimeOut	},
// This is just another quick "AT", "OK" sequence to check we're still alright
	{	ERxString, (TText8*)"AT",			0, ERxEvent	},
	{	ETxStringOK, NULL,					0, ETxEvent|EFailIfTimeOut	},

// This is the data call parameters setup command (new for MMTSY since 13/Sep/01)
	{	ERxString, (TText8*)"AT+CBST=75,0,1",			0, ERxEvent	},
	{	ETxStringOK, NULL,					0, ETxEvent|EFailIfTimeOut	},

// Issue the dial command and wait for connection
	{	ERxString, (TText8*)"ATD+1234",		0, ERxEvent	},
// Set signal for connection
	{	ESignalMark, NULL,					KSignalDCD, 0	},
	{	ETxString, (TText8*)"\r\nCONNECT\r\n", 0, ETxEvent|EFailIfTimeOut	},

	{	ERxString, (TText8*)"+++",			0, ERxEvent	},
	{	ERxString, (TText8*)"ATH",			0, ERxEvent	},
	{	ETxStringOK, NULL,					0, ETxEvent|EFailIfTimeOut	},
//*************************************************
//	DialDiffSpeed(RMobileCall::ESpeed14400, RMobileCall::EProtocolV120) ; //15
//*************************************************
// Perform the Dial request
	{	ERxString, (TText8*)"AT+FCLASS=0",	0, ERxEvent	},
	{	ETxStringOK, NULL,					0, ETxEvent|EFailIfTimeOut	},

// Note: Run another test where we ERROR this request
	{	ERxString, (TText8*)"ATM1L0X4S8=4",	0, ERxEvent	},
	{	ETxStringOK, NULL,					0, ETxEvent|EFailIfTimeOut	},


	{	ERxString, (TText8*)"AT345678901234567890123456789012345678901234567890",	0, ERxEvent	},
	{	ETxStringOK, NULL,					0, ETxEvent|EFailIfTimeOut	},
// This is just another quick "AT", "OK" sequence to check we're still alright
	{	ERxString, (TText8*)"AT",			0, ERxEvent	},
	{	ETxStringOK, NULL,					0, ETxEvent|EFailIfTimeOut	},

// This is the data call parameters setup command (new for MMTSY since 13/Sep/01)
	{	ERxString, (TText8*)"AT+CBST=43,0,1",			0, ERxEvent	},
	{	ETxStringOK, NULL,					0, ETxEvent|EFailIfTimeOut	},

// Issue the dial command and wait for connection
	{	ERxString, (TText8*)"ATD+1234",		0, ERxEvent	},
// Set signal for connection
	{	ESignalMark, NULL,					KSignalDCD, 0	},
	{	ETxString, (TText8*)"\r\nCONNECT\r\n", 0, ETxEvent|EFailIfTimeOut	},

	{	ERxString, (TText8*)"+++",			0, ERxEvent	},
	{	ERxString, (TText8*)"ATH",			0, ERxEvent	},
	{	ETxStringOK, NULL,					0, ETxEvent|EFailIfTimeOut	},
//*************************************************
//	DialDiffSpeed(RMobileCall::ESpeed14400, RMobileCall::EProtocolX31FlagStuffing) ; //6
//*************************************************
	{	ERxString, (TText8*)"AT+FCLASS=0",	0, ERxEvent	},
	{	ETxStringOK, NULL,					0, ETxEvent|EFailIfTimeOut	},

// Note: Run another test where we ERROR this request
	{	ERxString, (TText8*)"ATM1L0X4S8=4",	0, ERxEvent	},
	{	ETxStringOK, NULL,					0, ETxEvent|EFailIfTimeOut	},
	{	ERxString, (TText8*)"AT345678901234567890123456789012345678901234567890",	0, ERxEvent	},
	{	ETxStringOK, NULL,					0, ETxEvent|EFailIfTimeOut	},
// This is just another quick "AT", "OK" sequence to check we're still alright
	{	ERxString, (TText8*)"AT",			0, ERxEvent	},
	{	ETxStringOK, NULL,					0, ETxEvent|EFailIfTimeOut	},
// This is the data call parameters setup command (new for MMTSY since 13/Sep/01)
	{	ERxString, (TText8*)"AT+CBST=75,0,1",			0, ERxEvent	},
	{	ETxStringOK, NULL,					0, ETxEvent|EFailIfTimeOut	},

// Issue the dial command and wait for connection
	{	ERxString, (TText8*)"ATD+1234",		0, ERxEvent	},
// Set signal for connection
	{	ESignalMark, NULL,					KSignalDCD, 0	},
	{	ETxString, (TText8*)"\r\nCONNECT\r\n", 0, ETxEvent|EFailIfTimeOut	},

	{	ERxString, (TText8*)"+++",			0, ERxEvent	},
	{	ERxString, (TText8*)"ATH",			0, ERxEvent	},
	{	ETxStringOK, NULL,					0, ETxEvent|EFailIfTimeOut	},
//*************************************************
//	DialDiffSpeed(RMobileCall::ESpeed19200, RMobileCall::EProtocolV34) ; //16
//*************************************************
// Perform the Dial request
	{	ERxString, (TText8*)"AT+FCLASS=0",	0, ERxEvent	},
	{	ETxStringOK, NULL,					0, ETxEvent|EFailIfTimeOut	},

// Note: Run another test where we ERROR this request
	{	ERxString, (TText8*)"ATM1L0X4S8=4",	0, ERxEvent	},
	{	ETxStringOK, NULL,					0, ETxEvent|EFailIfTimeOut	},


	{	ERxString, (TText8*)"AT345678901234567890123456789012345678901234567890",	0, ERxEvent	},
	{	ETxStringOK, NULL,					0, ETxEvent|EFailIfTimeOut	},
// This is just another quick "AT", "OK" sequence to check we're still alright
	{	ERxString, (TText8*)"AT",			0, ERxEvent	},
	{	ETxStringOK, NULL,					0, ETxEvent|EFailIfTimeOut	},
// This is the data call parameters setup command (new for MMTSY since 13/Sep/01)
	{	ERxString, (TText8*)"AT+CBST=15,0,1",			0, ERxEvent	},
	{	ETxStringOK, NULL,					0, ETxEvent|EFailIfTimeOut	},

// Issue the dial command and wait for connection
	{	ERxString, (TText8*)"ATD+1234",		0, ERxEvent	},
// Set signal for connection
	{	ESignalMark, NULL,					KSignalDCD, 0	},
	{	ETxString, (TText8*)"\r\nCONNECT\r\n", 0, ETxEvent|EFailIfTimeOut	},

	{	ERxString, (TText8*)"+++",			0, ERxEvent	},
	{	ERxString, (TText8*)"ATH",			0, ERxEvent	},
	{	ETxStringOK, NULL,					0, ETxEvent|EFailIfTimeOut	},
//*************************************************
//	DialDiffSpeed(RMobileCall::ESpeed19200, RMobileCall::EProtocolV110) ;//17
//*************************************************
// Perform the Dial request
	{	ERxString, (TText8*)"AT+FCLASS=0",	0, ERxEvent	},
	{	ETxStringOK, NULL,					0, ETxEvent|EFailIfTimeOut	},

// Note: Run another test where we ERROR this request
	{	ERxString, (TText8*)"ATM1L0X4S8=4",	0, ERxEvent	},
	{	ETxStringOK, NULL,					0, ETxEvent|EFailIfTimeOut	},


	{	ERxString, (TText8*)"AT345678901234567890123456789012345678901234567890",	0, ERxEvent	},
	{	ETxStringOK, NULL,					0, ETxEvent|EFailIfTimeOut	},
// This is just another quick "AT", "OK" sequence to check we're still alright
	{	ERxString, (TText8*)"AT",			0, ERxEvent	},
	{	ETxStringOK, NULL,					0, ETxEvent|EFailIfTimeOut	},
// This is the data call parameters setup command (new for MMTSY since 13/Sep/01)
	{	ERxString, (TText8*)"AT+CBST=79,0,1",			0, ERxEvent	},
	{	ETxStringOK, NULL,					0, ETxEvent|EFailIfTimeOut	},

// Issue the dial command and wait for connection
	{	ERxString, (TText8*)"ATD+1234",		0, ERxEvent	},
// Set signal for connection
	{	ESignalMark, NULL,					KSignalDCD, 0	},
	{	ETxString, (TText8*)"\r\nCONNECT\r\n", 0, ETxEvent|EFailIfTimeOut	},

	{	ERxString, (TText8*)"+++",			0, ERxEvent	},
	{	ERxString, (TText8*)"ATH",			0, ERxEvent	},
	{	ETxStringOK, NULL,					0, ETxEvent|EFailIfTimeOut	},
//*************************************************
//	DialDiffSpeed(RMobileCall::ESpeed19200, RMobileCall::EProtocolV120) ;//18
//*************************************************
// Perform the Dial request
	{	ERxString, (TText8*)"AT+FCLASS=0",	0, ERxEvent	},
	{	ETxStringOK, NULL,					0, ETxEvent|EFailIfTimeOut	},

// Note: Run another test where we ERROR this request
	{	ERxString, (TText8*)"ATM1L0X4S8=4",	0, ERxEvent	},
	{	ETxStringOK, NULL,					0, ETxEvent|EFailIfTimeOut	},


	{	ERxString, (TText8*)"AT345678901234567890123456789012345678901234567890",	0, ERxEvent	},
	{	ETxStringOK, NULL,					0, ETxEvent|EFailIfTimeOut	},
// This is just another quick "AT", "OK" sequence to check we're still alright
	{	ERxString, (TText8*)"AT",			0, ERxEvent	},
	{	ETxStringOK, NULL,					0, ETxEvent|EFailIfTimeOut	},
// This is the data call parameters setup command (new for MMTSY since 13/Sep/01)
	{	ERxString, (TText8*)"AT+CBST=47,0,1",			0, ERxEvent	},
	{	ETxStringOK, NULL,					0, ETxEvent|EFailIfTimeOut	},

// Issue the dial command and wait for connection
	{	ERxString, (TText8*)"ATD+1234",		0, ERxEvent	},
// Set signal for connection
	{	ESignalMark, NULL,					KSignalDCD, 0	},
	{	ETxString, (TText8*)"\r\nCONNECT\r\n", 0, ETxEvent|EFailIfTimeOut	},

	{	ERxString, (TText8*)"+++",			0, ERxEvent	},
	{	ERxString, (TText8*)"ATH",			0, ERxEvent	},
	{	ETxStringOK, NULL,					0, ETxEvent|EFailIfTimeOut	},
//*************************************************
//	DialDiffSpeed(RMobileCall::ESpeed19200, RMobileCall::EProtocolX31FlagStuffing) ; //6
//*************************************************
	{	ERxString, (TText8*)"AT+FCLASS=0",	0, ERxEvent	},
	{	ETxStringOK, NULL,					0, ETxEvent|EFailIfTimeOut	},

// Note: Run another test where we ERROR this request
	{	ERxString, (TText8*)"ATM1L0X4S8=4",	0, ERxEvent	},
	{	ETxStringOK, NULL,					0, ETxEvent|EFailIfTimeOut	},
	{	ERxString, (TText8*)"AT345678901234567890123456789012345678901234567890",	0, ERxEvent	},
	{	ETxStringOK, NULL,					0, ETxEvent|EFailIfTimeOut	},
// This is just another quick "AT", "OK" sequence to check we're still alright
	{	ERxString, (TText8*)"AT",			0, ERxEvent	},
	{	ETxStringOK, NULL,					0, ETxEvent|EFailIfTimeOut	},
// This is the data call parameters setup command (new for MMTSY since 13/Sep/01)
	{	ERxString, (TText8*)"AT+CBST=79,0,1",			0, ERxEvent	},
	{	ETxStringOK, NULL,					0, ETxEvent|EFailIfTimeOut	},

// Issue the dial command and wait for connection
	{	ERxString, (TText8*)"ATD+1234",		0, ERxEvent	},
// Set signal for connection
	{	ESignalMark, NULL,					KSignalDCD, 0	},
	{	ETxString, (TText8*)"\r\nCONNECT\r\n", 0, ETxEvent|EFailIfTimeOut	},

	{	ERxString, (TText8*)"+++",			0, ERxEvent	},
	{	ERxString, (TText8*)"ATH",			0, ERxEvent	},
	{	ETxStringOK, NULL,					0, ETxEvent|EFailIfTimeOut	},
//*************************************************
//	DialDiffSpeed(RMobileCall::ESpeed28800, RMobileCall::EProtocolV34) ; //19
//*************************************************
// Perform the Dial request
	{	ERxString, (TText8*)"AT+FCLASS=0",	0, ERxEvent	},
	{	ETxStringOK, NULL,					0, ETxEvent|EFailIfTimeOut	},

// Note: Run another test where we ERROR this request
	{	ERxString, (TText8*)"ATM1L0X4S8=4",	0, ERxEvent	},
	{	ETxStringOK, NULL,					0, ETxEvent|EFailIfTimeOut	},


	{	ERxString, (TText8*)"AT345678901234567890123456789012345678901234567890",	0, ERxEvent	},
	{	ETxStringOK, NULL,					0, ETxEvent|EFailIfTimeOut	},
// This is just another quick "AT", "OK" sequence to check we're still alright
	{	ERxString, (TText8*)"AT",			0, ERxEvent	},
	{	ETxStringOK, NULL,					0, ETxEvent|EFailIfTimeOut	},
// This is the data call parameters setup command (new for MMTSY since 13/Sep/01)
	{	ERxString, (TText8*)"AT+CBST=16,0,1",			0, ERxEvent	},
	{	ETxStringOK, NULL,					0, ETxEvent|EFailIfTimeOut	},

// Issue the dial command and wait for connection
	{	ERxString, (TText8*)"ATD+1234",		0, ERxEvent	},
// Set signal for connection
	{	ESignalMark, NULL,					KSignalDCD, 0	},
	{	ETxString, (TText8*)"\r\nCONNECT\r\n", 0, ETxEvent|EFailIfTimeOut	},

	{	ERxString, (TText8*)"+++",			0, ERxEvent	},
	{	ERxString, (TText8*)"ATH",			0, ERxEvent	},
	{	ETxStringOK, NULL,					0, ETxEvent|EFailIfTimeOut	},
//*************************************************
//	DialDiffSpeed(RMobileCall::ESpeed28800, RMobileCall::EProtocolV110) ; //20
//*************************************************
// Perform the Dial request
	{	ERxString, (TText8*)"AT+FCLASS=0",	0, ERxEvent	},
	{	ETxStringOK, NULL,					0, ETxEvent|EFailIfTimeOut	},

// Note: Run another test where we ERROR this request
	{	ERxString, (TText8*)"ATM1L0X4S8=4",	0, ERxEvent	},
	{	ETxStringOK, NULL,					0, ETxEvent|EFailIfTimeOut	},


	{	ERxString, (TText8*)"AT345678901234567890123456789012345678901234567890",	0, ERxEvent	},
	{	ETxStringOK, NULL,					0, ETxEvent|EFailIfTimeOut	},
// This is just another quick "AT", "OK" sequence to check we're still alright
	{	ERxString, (TText8*)"AT",			0, ERxEvent	},
	{	ETxStringOK, NULL,					0, ETxEvent|EFailIfTimeOut	},
// This is the data call parameters setup command (new for MMTSY since 13/Sep/01)
	{	ERxString, (TText8*)"AT+CBST=80,0,1",			0, ERxEvent	},
	{	ETxStringOK, NULL,					0, ETxEvent|EFailIfTimeOut	},

// Issue the dial command and wait for connection
	{	ERxString, (TText8*)"ATD+1234",		0, ERxEvent	},
// Set signal for connection
	{	ESignalMark, NULL,					KSignalDCD, 0	},
	{	ETxString, (TText8*)"\r\nCONNECT\r\n", 0, ETxEvent|EFailIfTimeOut	},

	{	ERxString, (TText8*)"+++",			0, ERxEvent	},
	{	ERxString, (TText8*)"ATH",			0, ERxEvent	},
	{	ETxStringOK, NULL,					0, ETxEvent|EFailIfTimeOut	},
//*************************************************
//	DialDiffSpeed(RMobileCall::ESpeed28800, RMobileCall::EProtocolV120) ; //21
//*************************************************
// Perform the Dial request
	{	ERxString, (TText8*)"AT+FCLASS=0",	0, ERxEvent	},
	{	ETxStringOK, NULL,					0, ETxEvent|EFailIfTimeOut	},

// Note: Run another test where we ERROR this request
	{	ERxString, (TText8*)"ATM1L0X4S8=4",	0, ERxEvent	},
	{	ETxStringOK, NULL,					0, ETxEvent|EFailIfTimeOut	},


	{	ERxString, (TText8*)"AT345678901234567890123456789012345678901234567890",	0, ERxEvent	},
	{	ETxStringOK, NULL,					0, ETxEvent|EFailIfTimeOut	},
// This is just another quick "AT", "OK" sequence to check we're still alright
	{	ERxString, (TText8*)"AT",			0, ERxEvent	},
	{	ETxStringOK, NULL,					0, ETxEvent|EFailIfTimeOut	},
// This is the data call parameters setup command (new for MMTSY since 13/Sep/01)
	{	ERxString, (TText8*)"AT+CBST=48,0,1",			0, ERxEvent	},
	{	ETxStringOK, NULL,					0, ETxEvent|EFailIfTimeOut	},

// Issue the dial command and wait for connection
	{	ERxString, (TText8*)"ATD+1234",		0, ERxEvent	},
// Set signal for connection
	{	ESignalMark, NULL,					KSignalDCD, 0	},
	{	ETxString, (TText8*)"\r\nCONNECT\r\n", 0, ETxEvent|EFailIfTimeOut	},

	{	ERxString, (TText8*)"+++",			0, ERxEvent	},
	{	ERxString, (TText8*)"ATH",			0, ERxEvent	},
	{	ETxStringOK, NULL,					0, ETxEvent|EFailIfTimeOut	},
//*************************************************
//	DialDiffSpeed(RMobileCall::ESpeed28800, RMobileCall::EProtocolX31FlagStuffing) ; //6
//*************************************************
	{	ERxString, (TText8*)"AT+FCLASS=0",	0, ERxEvent	},
	{	ETxStringOK, NULL,					0, ETxEvent|EFailIfTimeOut	},

// Note: Run another test where we ERROR this request
	{	ERxString, (TText8*)"ATM1L0X4S8=4",	0, ERxEvent	},
	{	ETxStringOK, NULL,					0, ETxEvent|EFailIfTimeOut	},
	{	ERxString, (TText8*)"AT345678901234567890123456789012345678901234567890",	0, ERxEvent	},
	{	ETxStringOK, NULL,					0, ETxEvent|EFailIfTimeOut	},
// This is just another quick "AT", "OK" sequence to check we're still alright
	{	ERxString, (TText8*)"AT",			0, ERxEvent	},
	{	ETxStringOK, NULL,					0, ETxEvent|EFailIfTimeOut	},
// This is the data call parameters setup command (new for MMTSY since 13/Sep/01)
	{	ERxString, (TText8*)"AT+CBST=80,0,1",			0, ERxEvent	},
	{	ETxStringOK, NULL,					0, ETxEvent|EFailIfTimeOut	},

// Issue the dial command and wait for connection
	{	ERxString, (TText8*)"ATD+1234",		0, ERxEvent	},
// Set signal for connection
	{	ESignalMark, NULL,					KSignalDCD, 0	},
	{	ETxString, (TText8*)"\r\nCONNECT\r\n", 0, ETxEvent|EFailIfTimeOut	},

	{	ERxString, (TText8*)"+++",			0, ERxEvent	},
	{	ERxString, (TText8*)"ATH",			0, ERxEvent	},
	{	ETxStringOK, NULL,					0, ETxEvent|EFailIfTimeOut	},
//*************************************************
//	DialDiffSpeed(RMobileCall::ESpeed38400, RMobileCall::EProtocolV110) ;//22
//*************************************************
// Perform the Dial request
	{	ERxString, (TText8*)"AT+FCLASS=0",	0, ERxEvent	},
	{	ETxStringOK, NULL,					0, ETxEvent|EFailIfTimeOut	},

// Note: Run another test where we ERROR this request
	{	ERxString, (TText8*)"ATM1L0X4S8=4",	0, ERxEvent	},
	{	ETxStringOK, NULL,					0, ETxEvent|EFailIfTimeOut	},


	{	ERxString, (TText8*)"AT345678901234567890123456789012345678901234567890",	0, ERxEvent	},
	{	ETxStringOK, NULL,					0, ETxEvent|EFailIfTimeOut	},
// This is just another quick "AT", "OK" sequence to check we're still alright
	{	ERxString, (TText8*)"AT",			0, ERxEvent	},
	{	ETxStringOK, NULL,					0, ETxEvent|EFailIfTimeOut	},
// This is the data call parameters setup command (new for MMTSY since 13/Sep/01)
	{	ERxString, (TText8*)"AT+CBST=81,0,1",			0, ERxEvent	},
	{	ETxStringOK, NULL,					0, ETxEvent|EFailIfTimeOut	},

// Issue the dial command and wait for connection
	{	ERxString, (TText8*)"ATD+1234",		0, ERxEvent	},
// Set signal for connection
	{	ESignalMark, NULL,					KSignalDCD, 0	},
	{	ETxString, (TText8*)"\r\nCONNECT\r\n", 0, ETxEvent|EFailIfTimeOut	},

	{	ERxString, (TText8*)"+++",			0, ERxEvent	},
	{	ERxString, (TText8*)"ATH",			0, ERxEvent	},
	{	ETxStringOK, NULL,					0, ETxEvent|EFailIfTimeOut	},
//*************************************************
//	DialDiffSpeed(RMobileCall::ESpeed38400, RMobileCall::EProtocolV120) ;//23
//*************************************************
// Perform the Dial request
	{	ERxString, (TText8*)"AT+FCLASS=0",	0, ERxEvent	},
	{	ETxStringOK, NULL,					0, ETxEvent|EFailIfTimeOut	},

// Note: Run another test where we ERROR this request
	{	ERxString, (TText8*)"ATM1L0X4S8=4",	0, ERxEvent	},
	{	ETxStringOK, NULL,					0, ETxEvent|EFailIfTimeOut	},


	{	ERxString, (TText8*)"AT345678901234567890123456789012345678901234567890",	0, ERxEvent	},
	{	ETxStringOK, NULL,					0, ETxEvent|EFailIfTimeOut	},
// This is just another quick "AT", "OK" sequence to check we're still alright
	{	ERxString, (TText8*)"AT",			0, ERxEvent	},
	{	ETxStringOK, NULL,					0, ETxEvent|EFailIfTimeOut	},
// This is the data call parameters setup command (new for MMTSY since 13/Sep/01)
	{	ERxString, (TText8*)"AT+CBST=49,0,1",			0, ERxEvent	},
	{	ETxStringOK, NULL,					0, ETxEvent|EFailIfTimeOut	},

// Issue the dial command and wait for connection
	{	ERxString, (TText8*)"ATD+1234",		0, ERxEvent	},
// Set signal for connection
	{	ESignalMark, NULL,					KSignalDCD, 0	},
	{	ETxString, (TText8*)"\r\nCONNECT\r\n", 0, ETxEvent|EFailIfTimeOut	},

	{	ERxString, (TText8*)"+++",			0, ERxEvent	},
	{	ERxString, (TText8*)"ATH",			0, ERxEvent	},
	{	ETxStringOK, NULL,					0, ETxEvent|EFailIfTimeOut	},
//*************************************************
//	DialDiffSpeed(RMobileCall::ESpeed38400, RMobileCall::EProtocolX31FlagStuffing) ; //6
//*************************************************
	{	ERxString, (TText8*)"AT+FCLASS=0",	0, ERxEvent	},
	{	ETxStringOK, NULL,					0, ETxEvent|EFailIfTimeOut	},

// Note: Run another test where we ERROR this request
	{	ERxString, (TText8*)"ATM1L0X4S8=4",	0, ERxEvent	},
	{	ETxStringOK, NULL,					0, ETxEvent|EFailIfTimeOut	},
	{	ERxString, (TText8*)"AT345678901234567890123456789012345678901234567890",	0, ERxEvent	},
	{	ETxStringOK, NULL,					0, ETxEvent|EFailIfTimeOut	},
// This is just another quick "AT", "OK" sequence to check we're still alright
	{	ERxString, (TText8*)"AT",			0, ERxEvent	},
	{	ETxStringOK, NULL,					0, ETxEvent|EFailIfTimeOut	},
// This is the data call parameters setup command (new for MMTSY since 13/Sep/01)
	{	ERxString, (TText8*)"AT+CBST=81,0,1",			0, ERxEvent	},
	{	ETxStringOK, NULL,					0, ETxEvent|EFailIfTimeOut	},

// Issue the dial command and wait for connection
	{	ERxString, (TText8*)"ATD+1234",		0, ERxEvent	},
// Set signal for connection
	{	ESignalMark, NULL,					KSignalDCD, 0	},
	{	ETxString, (TText8*)"\r\nCONNECT\r\n", 0, ETxEvent|EFailIfTimeOut	},

	{	ERxString, (TText8*)"+++",			0, ERxEvent	},
	{	ERxString, (TText8*)"ATH",			0, ERxEvent	},
	{	ETxStringOK, NULL,					0, ETxEvent|EFailIfTimeOut	},
//*************************************************
//	DialDiffSpeed(RMobileCall::ESpeed48000, RMobileCall::EProtocolV110) ;//24
//*************************************************
// Perform the Dial request
	{	ERxString, (TText8*)"AT+FCLASS=0",	0, ERxEvent	},
	{	ETxStringOK, NULL,					0, ETxEvent|EFailIfTimeOut	},

// Note: Run another test where we ERROR this request
	{	ERxString, (TText8*)"ATM1L0X4S8=4",	0, ERxEvent	},
	{	ETxStringOK, NULL,					0, ETxEvent|EFailIfTimeOut	},


	{	ERxString, (TText8*)"AT345678901234567890123456789012345678901234567890",	0, ERxEvent	},
	{	ETxStringOK, NULL,					0, ETxEvent|EFailIfTimeOut	},
// This is just another quick "AT", "OK" sequence to check we're still alright
	{	ERxString, (TText8*)"AT",			0, ERxEvent	},
	{	ETxStringOK, NULL,					0, ETxEvent|EFailIfTimeOut	},
// This is the data call parameters setup command (new for MMTSY since 13/Sep/01)
	{	ERxString, (TText8*)"AT+CBST=82,0,1",			0, ERxEvent	},
	{	ETxStringOK, NULL,					0, ETxEvent|EFailIfTimeOut	},

// Issue the dial command and wait for connection
	{	ERxString, (TText8*)"ATD+1234",		0, ERxEvent	},
// Set signal for connection
	{	ESignalMark, NULL,					KSignalDCD, 0	},
	{	ETxString, (TText8*)"\r\nCONNECT\r\n", 0, ETxEvent|EFailIfTimeOut	},

	{	ERxString, (TText8*)"+++",			0, ERxEvent	},
	{	ERxString, (TText8*)"ATH",			0, ERxEvent	},
	{	ETxStringOK, NULL,					0, ETxEvent|EFailIfTimeOut	},
//*************************************************
//	DialDiffSpeed(RMobileCall::ESpeed48000, RMobileCall::EProtocolV120) ;//25
//*************************************************
// Perform the Dial request
	{	ERxString, (TText8*)"AT+FCLASS=0",	0, ERxEvent	},
	{	ETxStringOK, NULL,					0, ETxEvent|EFailIfTimeOut	},

// Note: Run another test where we ERROR this request
	{	ERxString, (TText8*)"ATM1L0X4S8=4",	0, ERxEvent	},
	{	ETxStringOK, NULL,					0, ETxEvent|EFailIfTimeOut	},


	{	ERxString, (TText8*)"AT345678901234567890123456789012345678901234567890",	0, ERxEvent	},
	{	ETxStringOK, NULL,					0, ETxEvent|EFailIfTimeOut	},
// This is just another quick "AT", "OK" sequence to check we're still alright
	{	ERxString, (TText8*)"AT",			0, ERxEvent	},
	{	ETxStringOK, NULL,					0, ETxEvent|EFailIfTimeOut	},
// This is the data call parameters setup command (new for MMTSY since 13/Sep/01)
	{	ERxString, (TText8*)"AT+CBST=50,0,1",			0, ERxEvent	},
	{	ETxStringOK, NULL,					0, ETxEvent|EFailIfTimeOut	},

// Issue the dial command and wait for connection
	{	ERxString, (TText8*)"ATD+1234",		0, ERxEvent	},
// Set signal for connection
	{	ESignalMark, NULL,					KSignalDCD, 0	},
	{	ETxString, (TText8*)"\r\nCONNECT\r\n", 0, ETxEvent|EFailIfTimeOut	},

	{	ERxString, (TText8*)"+++",			0, ERxEvent	},
	{	ERxString, (TText8*)"ATH",			0, ERxEvent	},
	{	ETxStringOK, NULL,					0, ETxEvent|EFailIfTimeOut	},
//*************************************************
//	DialDiffSpeed(RMobileCall::ESpeed48000, RMobileCall::EProtocolX31FlagStuffing) ; //6
//*************************************************
	{	ERxString, (TText8*)"AT+FCLASS=0",	0, ERxEvent	},
	{	ETxStringOK, NULL,					0, ETxEvent|EFailIfTimeOut	},

// Note: Run another test where we ERROR this request
	{	ERxString, (TText8*)"ATM1L0X4S8=4",	0, ERxEvent	},
	{	ETxStringOK, NULL,					0, ETxEvent|EFailIfTimeOut	},
	{	ERxString, (TText8*)"AT345678901234567890123456789012345678901234567890",	0, ERxEvent	},
	{	ETxStringOK, NULL,					0, ETxEvent|EFailIfTimeOut	},
// This is just another quick "AT", "OK" sequence to check we're still alright
	{	ERxString, (TText8*)"AT",			0, ERxEvent	},
	{	ETxStringOK, NULL,					0, ETxEvent|EFailIfTimeOut	},
// This is the data call parameters setup command (new for MMTSY since 13/Sep/01)
	{	ERxString, (TText8*)"AT+CBST=82,0,1",			0, ERxEvent	},
	{	ETxStringOK, NULL,					0, ETxEvent|EFailIfTimeOut	},

// Issue the dial command and wait for connection
	{	ERxString, (TText8*)"ATD+1234",		0, ERxEvent	},
// Set signal for connection
	{	ESignalMark, NULL,					KSignalDCD, 0	},
	{	ETxString, (TText8*)"\r\nCONNECT\r\n", 0, ETxEvent|EFailIfTimeOut	},

	{	ERxString, (TText8*)"+++",			0, ERxEvent	},
	{	ERxString, (TText8*)"ATH",			0, ERxEvent	},
	{	ETxStringOK, NULL,					0, ETxEvent|EFailIfTimeOut	},
//*************************************************
//	DialDiffSpeed(RMobileCall::ESpeed56000, RMobileCall::EProtocolV110) ;//34
//*************************************************
// Perform the Dial request
	{	ERxString, (TText8*)"AT+FCLASS=0",	0, ERxEvent	},
	{	ETxStringOK, NULL,					0, ETxEvent|EFailIfTimeOut	},

// Note: Run another test where we ERROR this request
	{	ERxString, (TText8*)"ATM1L0X4S8=4",	0, ERxEvent	},
	{	ETxStringOK, NULL,					0, ETxEvent|EFailIfTimeOut	},


	{	ERxString, (TText8*)"AT345678901234567890123456789012345678901234567890",	0, ERxEvent	},
	{	ETxStringOK, NULL,					0, ETxEvent|EFailIfTimeOut	},
// This is just another quick "AT", "OK" sequence to check we're still alright
	{	ERxString, (TText8*)"AT",			0, ERxEvent	},
	{	ETxStringOK, NULL,					0, ETxEvent|EFailIfTimeOut	},
// This is the data call parameters setup command (new for MMTSY since 13/Sep/01)
	{	ERxString, (TText8*)"AT+CBST=83,0,1",			0, ERxEvent	},
	{	ETxStringOK, NULL,					0, ETxEvent|EFailIfTimeOut	},

// Issue the dial command and wait for connection
	{	ERxString, (TText8*)"ATD+1234",		0, ERxEvent	},
// Set signal for connection
	{	ESignalMark, NULL,					KSignalDCD, 0	},
	{	ETxString, (TText8*)"\r\nCONNECT\r\n", 0, ETxEvent|EFailIfTimeOut	},

	{	ERxString, (TText8*)"+++",			0, ERxEvent	},
	{	ERxString, (TText8*)"ATH",			0, ERxEvent	},
	{	ETxStringOK, NULL,					0, ETxEvent|EFailIfTimeOut	},
//*************************************************
//	DialDiffSpeed(RMobileCall::ESpeed56000, RMobileCall::EProtocolV120) ;//35
//*************************************************
// Perform the Dial request
	{	ERxString, (TText8*)"AT+FCLASS=0",	0, ERxEvent	},
	{	ETxStringOK, NULL,					0, ETxEvent|EFailIfTimeOut	},

// Note: Run another test where we ERROR this request
	{	ERxString, (TText8*)"ATM1L0X4S8=4",	0, ERxEvent	},
	{	ETxStringOK, NULL,					0, ETxEvent|EFailIfTimeOut	},


	{	ERxString, (TText8*)"AT345678901234567890123456789012345678901234567890",	0, ERxEvent	},
	{	ETxStringOK, NULL,					0, ETxEvent|EFailIfTimeOut	},
// This is just another quick "AT", "OK" sequence to check we're still alright
	{	ERxString, (TText8*)"AT",			0, ERxEvent	},
	{	ETxStringOK, NULL,					0, ETxEvent|EFailIfTimeOut	},
// This is the data call parameters setup command (new for MMTSY since 13/Sep/01)
	{	ERxString, (TText8*)"AT+CBST=51,0,1",			0, ERxEvent	},
	{	ETxStringOK, NULL,					0, ETxEvent|EFailIfTimeOut	},

// Issue the dial command and wait for connection
	{	ERxString, (TText8*)"ATD+1234",		0, ERxEvent	},
// Set signal for connection
	{	ESignalMark, NULL,					KSignalDCD, 0	},
	{	ETxString, (TText8*)"\r\nCONNECT\r\n", 0, ETxEvent|EFailIfTimeOut	},

	{	ERxString, (TText8*)"+++",			0, ERxEvent	},
	{	ERxString, (TText8*)"ATH",			0, ERxEvent	},
	{	ETxStringOK, NULL,					0, ETxEvent|EFailIfTimeOut	},
//*************************************************
//	DialDiffSpeed(RMobileCall::ESpeed2400, RMobileCall::EProtocolX31FlagStuffing) ; //36
//*************************************************
// Perform the Dial request
	{	ERxString, (TText8*)"AT+FCLASS=0",	0, ERxEvent	},
	{	ETxStringOK, NULL,					0, ETxEvent|EFailIfTimeOut	},

// Note: Run another test where we ERROR this request
	{	ERxString, (TText8*)"ATM1L0X4S8=4",	0, ERxEvent	},
	{	ETxStringOK, NULL,					0, ETxEvent|EFailIfTimeOut	},


	{	ERxString, (TText8*)"AT345678901234567890123456789012345678901234567890",	0, ERxEvent	},
	{	ETxStringOK, NULL,					0, ETxEvent|EFailIfTimeOut	},
// This is just another quick "AT", "OK" sequence to check we're still alright
	{	ERxString, (TText8*)"AT",			0, ERxEvent	},
	{	ETxStringOK, NULL,					0, ETxEvent|EFailIfTimeOut	},
// This is the data call parameters setup command (new for MMTSY since 13/Sep/01)
	{	ERxString, (TText8*)"AT+CBST=83,0,1",			0, ERxEvent	},
	{	ETxStringOK, NULL,					0, ETxEvent|EFailIfTimeOut	},

// Issue the dial command and wait for connection
	{	ERxString, (TText8*)"ATD+1234",		0, ERxEvent	},
// Set signal for connection
	{	ESignalMark, NULL,					KSignalDCD, 0	},
	{	ETxString, (TText8*)"\r\nCONNECT\r\n", 0, ETxEvent|EFailIfTimeOut	},

	{	ERxString, (TText8*)"+++",			0, ERxEvent	},
	{	ERxString, (TText8*)"ATH",			0, ERxEvent	},
	{	ETxStringOK, NULL,					0, ETxEvent|EFailIfTimeOut	},
//*************************************************
//	DialDiffSpeed(RMobileCall::ESpeed56000, RMobileCall::EProtocolBitTransparent) ;//37
//*************************************************
// Perform the Dial request
	{	ERxString, (TText8*)"AT+FCLASS=0",	0, ERxEvent	},
	{	ETxStringOK, NULL,					0, ETxEvent|EFailIfTimeOut	},

// Note: Run another test where we ERROR this request
	{	ERxString, (TText8*)"ATM1L0X4S8=4",	0, ERxEvent	},
	{	ETxStringOK, NULL,					0, ETxEvent|EFailIfTimeOut	},


	{	ERxString, (TText8*)"AT345678901234567890123456789012345678901234567890",	0, ERxEvent	},
	{	ETxStringOK, NULL,					0, ETxEvent|EFailIfTimeOut	},
// This is just another quick "AT", "OK" sequence to check we're still alright
	{	ERxString, (TText8*)"AT",			0, ERxEvent	},
	{	ETxStringOK, NULL,					0, ETxEvent|EFailIfTimeOut	},
// This is the data call parameters setup command (new for MMTSY since 13/Sep/01)
	{	ERxString, (TText8*)"AT+CBST=115,0,1",			0, ERxEvent	},
	{	ETxStringOK, NULL,					0, ETxEvent|EFailIfTimeOut	},

// Issue the dial command and wait for connection
	{	ERxString, (TText8*)"ATD+1234",		0, ERxEvent	},
// Set signal for connection
	{	ESignalMark, NULL,					KSignalDCD, 0	},
	{	ETxString, (TText8*)"\r\nCONNECT\r\n", 0, ETxEvent|EFailIfTimeOut	},

	{	ERxString, (TText8*)"+++",			0, ERxEvent	},
	{	ERxString, (TText8*)"ATH",			0, ERxEvent	},
	{	ETxStringOK, NULL,					0, ETxEvent|EFailIfTimeOut	},
//*************************************************
//	DialDiffSpeed(RMobileCall::ESpeed64000, RMobileCall::EProtocolBitTransparent) ;//39
//*************************************************
// Perform the Dial request
	{	ERxString, (TText8*)"AT+FCLASS=0",	0, ERxEvent	},
	{	ETxStringOK, NULL,					0, ETxEvent|EFailIfTimeOut	},

// Note: Run another test where we ERROR this request
	{	ERxString, (TText8*)"ATM1L0X4S8=4",	0, ERxEvent	},
	{	ETxStringOK, NULL,					0, ETxEvent|EFailIfTimeOut	},


	{	ERxString, (TText8*)"AT345678901234567890123456789012345678901234567890",	0, ERxEvent	},
	{	ETxStringOK, NULL,					0, ETxEvent|EFailIfTimeOut	},
// This is just another quick "AT", "OK" sequence to check we're still alright
	{	ERxString, (TText8*)"AT",			0, ERxEvent	},
	{	ETxStringOK, NULL,					0, ETxEvent|EFailIfTimeOut	},
// This is the data call parameters setup command (new for MMTSY since 13/Sep/01)
	{	ERxString, (TText8*)"AT+CBST=116,0,1",			0, ERxEvent	},
	{	ETxStringOK, NULL,					0, ETxEvent|EFailIfTimeOut	},

// Issue the dial command and wait for connection
	{	ERxString, (TText8*)"ATD+1234",		0, ERxEvent	},
// Set signal for connection
	{	ESignalMark, NULL,					KSignalDCD, 0	},
	{	ETxString, (TText8*)"\r\nCONNECT\r\n", 0, ETxEvent|EFailIfTimeOut	},

	{	ERxString, (TText8*)"+++",			0, ERxEvent	},
	{	ERxString, (TText8*)"ATH",			0, ERxEvent	},
	{	ETxStringOK, NULL,					0, ETxEvent|EFailIfTimeOut	},
//*************************************************
//	DialDiffQoS(RMobileCall::EQoSUnspecified) ;
//*************************************************
// Perform the Dial request
	{	ERxString, (TText8*)"AT+FCLASS=0",	0, ERxEvent	},
	{	ETxStringOK, NULL,					0, ETxEvent|EFailIfTimeOut	},

// Note: Run another test where we ERROR this request
	{	ERxString, (TText8*)"ATM1L0X4S8=4",	0, ERxEvent	},
	{	ETxStringOK, NULL,					0, ETxEvent|EFailIfTimeOut	},


	{	ERxString, (TText8*)"AT345678901234567890123456789012345678901234567890",	0, ERxEvent	},
	{	ETxStringOK, NULL,					0, ETxEvent|EFailIfTimeOut	},
// This is just another quick "AT", "OK" sequence to check we're still alright
	{	ERxString, (TText8*)"AT",			0, ERxEvent	},
	{	ETxStringOK, NULL,					0, ETxEvent|EFailIfTimeOut	},
// This is the data call parameters setup command (new for MMTSY since 13/Sep/01)
	{	ERxString, (TText8*)"AT+CBST=7,0",			0, ERxEvent	},
	{	ETxStringOK, NULL,					0, ETxEvent|EFailIfTimeOut	},

// Issue the dial command and wait for connection
	{	ERxString, (TText8*)"ATD+1234",		0, ERxEvent	},
// Set signal for connection
	{	ESignalMark, NULL,					KSignalDCD, 0	},
	{	ETxString, (TText8*)"\r\nCONNECT\r\n", 0, ETxEvent|EFailIfTimeOut	},

	{	ERxString, (TText8*)"+++",			0, ERxEvent	},
	{	ERxString, (TText8*)"ATH",			0, ERxEvent	},
	{	ETxStringOK, NULL,					0, ETxEvent|EFailIfTimeOut	},
//*************************************************
//	DialDiffQoS(RMobileCall::EQoSTransparent) ;
//*************************************************
// Perform the Dial request
	{	ERxString, (TText8*)"AT+FCLASS=0",	0, ERxEvent	},
	{	ETxStringOK, NULL,					0, ETxEvent|EFailIfTimeOut	},

// Note: Run another test where we ERROR this request
	{	ERxString, (TText8*)"ATM1L0X4S8=4",	0, ERxEvent	},
	{	ETxStringOK, NULL,					0, ETxEvent|EFailIfTimeOut	},


	{	ERxString, (TText8*)"AT345678901234567890123456789012345678901234567890",	0, ERxEvent	},
	{	ETxStringOK, NULL,					0, ETxEvent|EFailIfTimeOut	},
// This is just another quick "AT", "OK" sequence to check we're still alright
	{	ERxString, (TText8*)"AT",			0, ERxEvent	},
	{	ETxStringOK, NULL,					0, ETxEvent|EFailIfTimeOut	},
// This is the data call parameters setup command (new for MMTSY since 13/Sep/01)
	{	ERxString, (TText8*)"AT+CBST=7,0,0",			0, ERxEvent	},
	{	ETxStringOK, NULL,					0, ETxEvent|EFailIfTimeOut	},

// Issue the dial command and wait for connection
	{	ERxString, (TText8*)"ATD+1234",		0, ERxEvent	},
// Set signal for connection
	{	ESignalMark, NULL,					KSignalDCD, 0	},
	{	ETxString, (TText8*)"\r\nCONNECT\r\n", 0, ETxEvent|EFailIfTimeOut	},

	{	ERxString, (TText8*)"+++",			0, ERxEvent	},
	{	ERxString, (TText8*)"ATH",			0, ERxEvent	},
	{	ETxStringOK, NULL,					0, ETxEvent|EFailIfTimeOut	},
//*************************************************
//	DialDiffQoS(RMobileCall::EQoSNonTransparent) ;
//*************************************************
// Perform the Dial request
	{	ERxString, (TText8*)"AT+FCLASS=0",	0, ERxEvent	},
	{	ETxStringOK, NULL,					0, ETxEvent|EFailIfTimeOut	},

// Note: Run another test where we ERROR this request
	{	ERxString, (TText8*)"ATM1L0X4S8=4",	0, ERxEvent	},
	{	ETxStringOK, NULL,					0, ETxEvent|EFailIfTimeOut	},


	{	ERxString, (TText8*)"AT345678901234567890123456789012345678901234567890",	0, ERxEvent	},
	{	ETxStringOK, NULL,					0, ETxEvent|EFailIfTimeOut	},
// This is just another quick "AT", "OK" sequence to check we're still alright
	{	ERxString, (TText8*)"AT",			0, ERxEvent	},
	{	ETxStringOK, NULL,					0, ETxEvent|EFailIfTimeOut	},
// This is the data call parameters setup command (new for MMTSY since 13/Sep/01)
	{	ERxString, (TText8*)"AT+CBST=7,0,1",			0, ERxEvent	},
	{	ETxStringOK, NULL,					0, ETxEvent|EFailIfTimeOut	},

// Issue the dial command and wait for connection
	{	ERxString, (TText8*)"ATD+1234",		0, ERxEvent	},
// Set signal for connection
	{	ESignalMark, NULL,					KSignalDCD, 0	},
	{	ETxString, (TText8*)"\r\nCONNECT\r\n", 0, ETxEvent|EFailIfTimeOut	},

	{	ERxString, (TText8*)"+++",			0, ERxEvent	},
	{	ERxString, (TText8*)"ATH",			0, ERxEvent	},
	{	ETxStringOK, NULL,					0, ETxEvent|EFailIfTimeOut	},
//*************************************************
//	DialDiffQoS(RMobileCall::EQosTransparentPreferred) ;
//*************************************************
// Perform the Dial request
	{	ERxString, (TText8*)"AT+FCLASS=0",	0, ERxEvent	},
	{	ETxStringOK, NULL,					0, ETxEvent|EFailIfTimeOut	},

// Note: Run another test where we ERROR this request
	{	ERxString, (TText8*)"ATM1L0X4S8=4",	0, ERxEvent	},
	{	ETxStringOK, NULL,					0, ETxEvent|EFailIfTimeOut	},


	{	ERxString, (TText8*)"AT345678901234567890123456789012345678901234567890",	0, ERxEvent	},
	{	ETxStringOK, NULL,					0, ETxEvent|EFailIfTimeOut	},
// This is just another quick "AT", "OK" sequence to check we're still alright
	{	ERxString, (TText8*)"AT",			0, ERxEvent	},
	{	ETxStringOK, NULL,					0, ETxEvent|EFailIfTimeOut	},
// This is the data call parameters setup command (new for MMTSY since 13/Sep/01)
	{	ERxString, (TText8*)"AT+CBST=7,0,2",			0, ERxEvent	},
	{	ETxStringOK, NULL,					0, ETxEvent|EFailIfTimeOut	},

// Issue the dial command and wait for connection
	{	ERxString, (TText8*)"ATD+1234",		0, ERxEvent	},
// Set signal for connection
	{	ESignalMark, NULL,					KSignalDCD, 0	},
	{	ETxString, (TText8*)"\r\nCONNECT\r\n", 0, ETxEvent|EFailIfTimeOut	},

	{	ERxString, (TText8*)"+++",			0, ERxEvent	},
	{	ERxString, (TText8*)"ATH",			0, ERxEvent	},
	{	ETxStringOK, NULL,					0, ETxEvent|EFailIfTimeOut	},
//*************************************************
//	DialDiffQoS(RMobileCall::EQosNonTransparentPreferred) ;
//*************************************************
// Perform the Dial request
	{	ERxString, (TText8*)"AT+FCLASS=0",	0, ERxEvent	},
	{	ETxStringOK, NULL,					0, ETxEvent|EFailIfTimeOut	},

// Note: Run another test where we ERROR this request
	{	ERxString, (TText8*)"ATM1L0X4S8=4",	0, ERxEvent	},
	{	ETxStringOK, NULL,					0, ETxEvent|EFailIfTimeOut	},


	{	ERxString, (TText8*)"AT345678901234567890123456789012345678901234567890",	0, ERxEvent	},
	{	ETxStringOK, NULL,					0, ETxEvent|EFailIfTimeOut	},
// This is just another quick "AT", "OK" sequence to check we're still alright
	{	ERxString, (TText8*)"AT",			0, ERxEvent	},
	{	ETxStringOK, NULL,					0, ETxEvent|EFailIfTimeOut	},
// This is the data call parameters setup command (new for MMTSY since 13/Sep/01)
	{	ERxString, (TText8*)"AT+CBST=7,0,3",			0, ERxEvent	},
	{	ETxStringOK, NULL,					0, ETxEvent|EFailIfTimeOut	},

// Issue the dial command and wait for connection
	{	ERxString, (TText8*)"ATD+1234",		0, ERxEvent	},
// Set signal for connection
	{	ESignalMark, NULL,					KSignalDCD, 0	},
	{	ETxString, (TText8*)"\r\nCONNECT\r\n", 0, ETxEvent|EFailIfTimeOut	},

	{	ERxString, (TText8*)"+++",			0, ERxEvent	},
	{	ERxString, (TText8*)"ATH",			0, ERxEvent	},
	{	ETxStringOK, NULL,					0, ETxEvent|EFailIfTimeOut	},
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	{	EComplete,	 NULL,					0, 0	},
	};

/**
 * This class is used by the client side of the test.  It contains the method DriveETelApi
 * which is the primary client side method.  This class inherits from the CTestBase class.
 * All client side test classes must contain these entries.
 */
class CTestDriveDataCallDiffParam : public CTestBase
	{
public:
	static CTestDriveDataCallDiffParam* NewL(const TScriptList aScriptListEntry,
	TInt aVarDelay=0);

	// Called by the user interface from the C function PerformTestL.
	virtual TInt RunTestL();

protected:
	// Called by the CTestBase::StartEmulator method to drive the ETel API.
	virtual TInt DriveETelApiL();

private:
	CTestDriveDataCallDiffParam(const TScriptList aScriptListEntry, TInt aVarDelay);
	void  DialDiffSpeedL(RMobileCall::TMobileCallDataSpeed aSpeed,
			RMobileCall::TMobileCallDataProtocol aArg=RMobileCall::EProtocolV32) ;
	void  DialDiffQoSL(RMobileCall::TMobileCallDataQoS aQoS) ;
	void  DialDiffProtocolL(RMobileCall::TMobileCallDataProtocol aArg);
	void  DialDiffDataServiceL(RMobileCall::TMobileCallDataService aArg) ;

	void Print_RMobileCall_TMobileCallDataSpeed(RMobileCall::TMobileCallDataSpeed& aArg);
	void Print_RMobileCall_TMobileCallDataProtocol(RMobileCall::TMobileCallDataProtocol& aArg);
	void Print_RMobileCall_TMobileCallDataQoS(RMobileCall::TMobileCallDataQoS& aArg);
	void Print_RMobileCall_TMobileCallDataService(RMobileCall::TMobileCallDataService aArg);
	TScriptList iScriptListEntry;	//< Initialized by the constructor, it records the enumerated
									//< script entry number.
	TInt        iVarDelay;			//< Variable delay value to be used as the timeout by the
									//< ...script processing when the iParam value is set -1 in
									//< ...a TScript script whose iCommand is an EWait.
	RMobilePhone iPhone3;

	};

/**
 * This class is used by the emulator side of the test.  It implements several virtual functions
 * which are test specific and referenced by other classes.  This class inherits from the
 * CATScriptEng class.  All Emulator side test classes should have this structure.
 */
class CTestDataCallDiffParam : public CATScriptEng
	{
public:
	static CTestDataCallDiffParam* NewL(const TScript* aScript);
	static CTestDataCallDiffParam* NewL(const TScript* aScript, const TInt aVarDelay);

	// Called by the scripting engine to implement the EExecuteSpecial Command.
	virtual void SpecificAlgorithmL(TInt aParam);

	// Called by the scripting engine to end the execution of the script.
	virtual void Complete(TInt aError);

	// Called by the scripting engine to start the execution of the script.
	virtual TInt Start();

private:
	CTestDataCallDiffParam(const TScript* aScript);
	CTestDataCallDiffParam(const TScript* aScript, const TInt aVarDelay);
	void ConstructL();

	const TScript* iScript;			//< Pointer to the first line in the associated script.
	};

#endif

