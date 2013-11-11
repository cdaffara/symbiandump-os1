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
//

/**
 @file
 @internalComponent 
*/

#ifndef __CCALLS_H__
#define __CCALLS_H__

#include "Te_LoopbackTestStepBase.h"
#include "Te_LoopBackScriptEng.h"

//
// First a Script where everything is in order
//
const struct TScript KAllCallsScript[]= {
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

//	Start the Voice Call tests. Incoming call indication
	{	ETxString, (TText8*)"\r\n+CRING: VOICE\r\n",	0, ETxEvent|EFailIfTimeOut	},

	{	ERxString, (TText8*)"AT",			0, ERxEvent	},
	{	ETxStringOK, NULL,					0, ETxEvent|EFailIfTimeOut	},

//	Answer incoming call
	{	ERxString, (TText8*)"ATA",			0, ERxEvent	},
	{	ETxStringOK, NULL,					0, ETxEvent|EFailIfTimeOut	},

	{	ERxString, (TText8*)"AT+",			0, ERxEvent	},
	{	ETxString, (TText8*)"ERROR",		0, ETxEvent|EFailIfTimeOut	},

//	Wait a short while then Hang up the call
	{	EWait,		NULL,					4, ETimeOutEvent},
	{	ERxString, (TText8*)"ATH",			0, ERxEvent	},
	{	ETxStringOK, NULL,					0, ETxEvent|EFailIfTimeOut	},

//	Start the Data Call tests. Incoming call indication. Wait a short while before 
//	starting the test, otherwise the incoming call indication will be received before
//	the data test has started and the data line will not yet have been created.
	{	EWait,		NULL,					20, ETimeOutEvent},
	{	ETxString, (TText8*)"\r\n+CRING: REL ASYNC\r\n",	0, ETxEvent|EFailIfTimeOut	},
	{	ERxString, (TText8*)"AT+FCLASS=0",	0, ERxEvent	},
	{	ETxStringOK, NULL,					0, ETxEvent|EFailIfTimeOut	},

	{	ERxString, (TText8*)"ATM0L0X3S8=0",	0, ERxEvent	},
	{	ETxStringOK, NULL,					0, ETxEvent|EFailIfTimeOut	},

// This is the Data Init String
	{	ERxString, (TText8*)"AT345678901234567890123456789012345678901234567890",	0, ERxEvent	},
	{	ETxStringOK, NULL,						0, ETxEvent|EFailIfTimeOut	},
	{	ERxString, (TText8*)"AT",			0, ERxEvent	},
	{	ETxStringOK, NULL,					0, ETxEvent|EFailIfTimeOut	},

//	Answer incoming data call
	{	ERxString, (TText8*)"ATA",	0, ERxEvent	},
	{	ETxString, (TText8*)"\r\nCONNECT 9600\r\n",	0, ETxEvent|EFailIfTimeOut	},

//	Data stream that is being written to the serial port
	{	ERxString, (TText8*)"Telephony Test Code\r\n",		0, ERxEvent	},

//	The data port has now been loaned and recovered (data transmission has taken place)
	{	ERxString, (TText8*)"AT",	0, ERxEvent	},

//	The Escape sequence 
	{	ERxString, (TText8*)"+++",	0, ERxEvent	},
	{	ETxStringOK, NULL,			0, ETxEvent|EFailIfTimeOut	},

//	Should now be in on-line Command mode
	{	ERxString, (TText8*)"AT",	0, ERxEvent	},
	{	ETxStringOK, NULL,			0, ETxEvent|EFailIfTimeOut	},

	{	ERxString, (TText8*)"ATO",	0, ERxEvent	},
	{	ETxString, (TText8*)"\r\nCONNECT 9600\r\n",	0, ETxEvent|EFailIfTimeOut	},

//	Should be in on-line Data mode. Hang up the data call
	{	ERxString, (TText8*)"ATH",	0, ERxEvent	},
	{	ETxStringOK, NULL,			0, ETxEvent|EFailIfTimeOut	},

	{	EWait,		NULL,			2, ETimeOutEvent},
	{	EComplete,	 NULL,					0, 0	}
	};

class CTestDriveAllCalls : public CTestBase
	{
public:
	static CTestDriveAllCalls* NewL(const TScriptList aScriptListEntry);
	virtual TInt RunTestL();

protected:
	virtual TInt DriveETelApiL();

private:
	CTestDriveAllCalls(const TScriptList aScriptListEntry);
	void TestVoiceCallL();
	void TestCancelRequestsL();
	void TestDataCallL();

	TScriptList iScriptListEntry;
	};

class CTestAllCalls : public CATScriptEng
	{
public:
	static CTestAllCalls* NewL(const TScript* aScript);
	virtual void SpecificAlgorithmL(TInt aParam);
	virtual void Complete(TInt aError);
	virtual TInt Start();

private:
	CTestAllCalls(const TScript* aScript);
	void ConstructL();

	const TScript* iScript;
	};

#endif
