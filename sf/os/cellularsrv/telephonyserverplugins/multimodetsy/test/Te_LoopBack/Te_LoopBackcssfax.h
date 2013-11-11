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
// Test trying to retrieve Signal Strength information while a fax is being received
// 
//

/**
 @file
 @internalComponent 
*/

#ifndef __CSSFAX_H__
#define __CSSFAX_H__

#include "Te_LoopbackTestStepBase.h"
#include "Te_LoopBackScriptEng.h"

_LIT8(KDataSsWriteTestData,"ABCD\r\n");
_LIT8(KDataSsWriteTestData2,"EFGH\r\n");

//
// First a Script where everything is in order
//
const struct TScript KSsFaxScriptA[]= {
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
//  Try an Incoming Call Fax request
//
	{	EWait,		NULL,					1,	ETimeOutEvent	},
	{	ETxString, (TText8*)"\r\n+CRING: FAX\r\n",	0, ETxEvent|EFailIfTimeOut	},

//	{	ERxString, (TText8*)"ATM0L0X3S8=0",			0, ERxEvent	},
//	{	ETxStringOK, NULL,					0, ETxEvent|EFailIfTimeOut	},

	{	ERxString, (TText8*)"AT+FCLASS=2",	0, ERxEvent	},
	{	ETxString, (TText8*)"\r\nOK\r\n",	0, ETxEvent|EFailIfTimeOut	},

	{	ERxString, (TText8*)"AT",			0, ERxEvent	},
	{	ETxStringOK, NULL,					0, ETxEvent|EFailIfTimeOut	},

	{	ERxString, (TText8*)"AT+FDCC=?",	0, ERxEvent	},
	{	ETxString, (TText8*)"(0-1),(0-3),(0-4),(0-2),(0-3),(0-1),(0-1),(0-7)",
											0, ETxEvent|EFailIfTimeOut	},
	{	ETxStringOK, NULL,					0, ETxEvent|EFailIfTimeOut	},

	{	ERxString, (TText8*)"AT+FDCC=0,3,0,2,0,0,0,0",	0, ERxEvent	},
	{	ETxStringOK, NULL,					0, ETxEvent|EFailIfTimeOut	},

	{	ERxString, (TText8*)"AT+FAA=0",		0, ERxEvent	},
	{	ETxStringOK, NULL,					0, ETxEvent|EFailIfTimeOut	},

	{	ERxString, (TText8*)"AT+FCR=1",		0, ERxEvent	},
	{	ETxStringOK, NULL,					0, ETxEvent|EFailIfTimeOut	},

	{	ERxString, (TText8*)"AT+FLID=\"",	0, ERxEvent	},
	{	ERxString, (TText8*)"\"",			0, ERxEvent	},
	{	EWait,		NULL,					1,	ETimeOutEvent	},
	{	ETxStringOK, NULL,					0, ETxEvent|EFailIfTimeOut	},

	{	ERxString, (TText8*)"ATA",			0, ERxEvent	},
	{	ETxString, (TText8*)"+FCO",			0, ETxEvent|EFailIfTimeOut	},
	{	ETxString, (TText8*)"+FCS:0,3,0,2,0,0,0,0",			0, ETxEvent|EFailIfTimeOut	},
	{	ETxStringOK, NULL,					0, ETxEvent|EFailIfTimeOut	},

	{	ERxString, (TText8*)"AT+FDR",		0, ERxEvent	},
	{	ETxString, (TText8*)"CONNECT",		0, ETxEvent|EFailIfTimeOut	},

	{	ERxString, (TText8*)"ATH",			0, ERxEvent	},
	{	ETxStringOK, NULL,					0, ETxEvent|EFailIfTimeOut	},

	{	ERxString, (TText8*)"ATE0+FCLASS=0",0, ERxEvent	},
	{	ETxStringOK, NULL,					0, ETxEvent|EFailIfTimeOut	},

	{	EWait,		NULL,			2, ETimeOutEvent},
	{	EComplete,	 NULL,					0, 0	}
	};

const struct TScript KSsDataScriptA[]= {
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

	{	ERxString, (TText8*)"+++",			0, ERxEvent	},

	{	ERxString, (TText8*)"AT",			0, ERxEvent	},
	{	ETxStringOK, NULL,					0, ETxEvent|EFailIfTimeOut	},

	{	ERxString, (TText8*)"ATO",			0, ERxEvent	},
	{	ETxString, (TText8*)"\r\nCONNECT 9600\r\n",	0, ETxEvent|EFailIfTimeOut	},
	
	{	ERxString, (TText8*)"ATH",			0, ERxEvent	},
	{	ETxStringOK, NULL,					0, ETxEvent|EFailIfTimeOut	},

	{	EWait,		NULL,			2, ETimeOutEvent},
	{	EComplete,	 NULL,					0, 0	}
	};

class CTestDriveSsFax : public CTestBase
	{
public:
	static CTestDriveSsFax* NewL(const TScriptList aScriptListEntry);
	virtual TInt RunTestL();

protected:
	virtual TInt DriveETelApiL();

private:
	CTestDriveSsFax(const TScriptList aScriptListEntry);

	TScriptList iScriptListEntry;
	};

class CTestSsFax : public CATScriptEng
	{
public:
	static CTestSsFax* NewL(const TScript* aScript);
	virtual void SpecificAlgorithmL(TInt aParam);
	virtual void Complete(TInt aError);
	virtual TInt Start();

private:
	CTestSsFax(const TScript* aScript);
	void ConstructL();
	const TScript* iScript;
	};

class CTestDriveSSData : public CTestBase
	{
public:
	static CTestDriveSSData* NewL(const TScriptList aScriptListEntry);
	virtual TInt RunTestL();

protected:
	virtual TInt DriveETelApiL();

private:
	CTestDriveSSData(const TScriptList aScriptListEntry);

	TScriptList iScriptListEntry;
	};

class CTestSSData : public CATScriptEng
	{
public:
	static CTestSSData* NewL(const TScript* aScript);
	virtual void SpecificAlgorithmL(TInt aParam);
	virtual void Complete(TInt aError);
	virtual TInt Start();

private:
	CTestSSData(const TScript* aScript);
	void ConstructL();

	const TScript* iScript;
	};

#endif // __CSSFAX_H__
