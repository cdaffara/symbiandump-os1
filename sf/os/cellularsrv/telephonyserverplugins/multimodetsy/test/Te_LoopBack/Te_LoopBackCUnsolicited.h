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
// This file has the class definitions and scripts for the "Unsolicited Messages" Test in 
// the ETel Regression test harness.
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

#ifndef __CUNSOLICITED__H__
#define __CUNSOLICITED__H__

#include "Te_LoopbackTestStepBase.h"
#include "Te_LoopBackScriptEng.h"

//
// First a Script where everything is in order
//
const struct TScript KCUnsolicitedScript[]= {	 

	//*******************************************************************
	// Injecting Unsolicited messages into every query-response state
	// +CMT: and +CMTI: SMS messages should be silenced by AT+CNMI=0,0
	//*******************************************************************

	//	Start of initialization sequence **********************************************
	{	ERxString,	(TText8*)"AT\r",								0, ERxEvent	},
/**/{	ETxString, (TText8*)"+CRING: VOICE\r",						0, ETxEvent|EFailIfTimeOut	},
/**/{	ETxString, (TText8*)"+CRING: SYNC\r",						0, ETxEvent|EFailIfTimeOut	},
/**/{	ETxString, (TText8*)"+CRING: VOICE/REL SYNC\r",				0, ETxEvent|EFailIfTimeOut	},
/**/{	ETxString, (TText8*)"+CRING: ALT VOICE/SYNC\r",				0, ETxEvent|EFailIfTimeOut	},
/**/{	ETxString, (TText8*)"+CRING: VOICE/ASYNC\r",				0, ETxEvent|EFailIfTimeOut	},
/**/{	ETxString, (TText8*)"+CRING: VOICE/REL SYNC\r",				0, ETxEvent|EFailIfTimeOut	},
/**/{	ETxString, (TText8*)"+CRING: ALT VOICE/SYNC\r",				0, ETxEvent|EFailIfTimeOut	},
/**/{	ETxString, (TText8*)"+CRING: ALT VOICE/REL ASYNC\r",		0, ETxEvent|EFailIfTimeOut	},
/**/{	ETxString, (TText8*)"+CRING: ALT REL ASYNC/VOICE\r",		0, ETxEvent|EFailIfTimeOut	},
/**/{	ETxString, (TText8*)"+CRING: ALT REL ASYNC/REL ASYNC\r",	0, ETxEvent|EFailIfTimeOut	},
/**/{	ETxString, (TText8*)"+CRING: ALT VOICE/FAX\r",				0, ETxEvent|EFailIfTimeOut	},
	{	ETxStringOK, NULL,											0, ETxEvent|EFailIfTimeOut	},
	{	EIfRxStringRelativeJmp, (TText8*)"AT\r",	-1, 0	},
	{	ERxString, (TText8*)"AT+FCLASS=?",							0, ERxEvent	},
/**/{	ETxString, (TText8*)"+CRING: FAX\r",						0, ETxEvent|EFailIfTimeOut	},
/**/{	ETxString, (TText8*)"+CREG: 2,3\r",							0, ETxEvent|EFailIfTimeOut	},
/**/{	ETxString, (TText8*)"+CRING: VOICE\r",						0, ETxEvent|EFailIfTimeOut	},
	{	ETxString, (TText8*)"0,1,2\r",								0, ETxEvent|EFailIfTimeOut	},
	{	ETxStringOK, NULL,											0, ETxEvent|EFailIfTimeOut	},
	{	ERxString, (TText8*)"AT",									0, ERxEvent	},
	{	ETxStringOK, NULL,											0, ETxEvent|EFailIfTimeOut	},
	{	ERxString, (TText8*)"AT+CNMI=,0",							0, ERxEvent	},
	{	ETxStringOK, NULL,											0, ETxEvent|EFailIfTimeOut	},
	{	ERxString, (TText8*)"ATS7?",								0, ERxEvent	},
/**/{	ETxString, (TText8*)"+CREG: 2,4\r",							0, ETxEvent|EFailIfTimeOut	},
/**/{	ETxString, (TText8*)"+CRING: VOICE\r",						0, ETxEvent|EFailIfTimeOut	},
/**/{	ETxString, (TText8*)"+CRING: SYNC\r",						0, ETxEvent|EFailIfTimeOut	},
/**/{	ETxString, (TText8*)"+CRING: ASYNC\r",						0, ETxEvent|EFailIfTimeOut	},
/**/{	ETxString, (TText8*)"+CRING: REL SYNC\r",					0, ETxEvent|EFailIfTimeOut	},
/**/{	ETxString, (TText8*)"+CRING: REL ANYNC\r",					0, ETxEvent|EFailIfTimeOut	},
/**/{	ETxString, (TText8*)"+CRING: VOICE/ASYNC\r",				0, ETxEvent|EFailIfTimeOut	},
/**/{	ETxString, (TText8*)"+CREG: 2,3\r",							0, ETxEvent|EFailIfTimeOut	},
/**/{	ETxString, (TText8*)"+CRING: VOICE/REL SYNC\r",				0, ETxEvent|EFailIfTimeOut	},
/**/{	ETxString, (TText8*)"+CRING: ALT VOICE/SYNC\r",				0, ETxEvent|EFailIfTimeOut	},
/**/{	ETxString, (TText8*)"+CRING: ALT VOICE/REL ASYNC\r",		0, ETxEvent|EFailIfTimeOut	},
/**/{	ETxString, (TText8*)"+CREG: 4,4\r",							0, ETxEvent|EFailIfTimeOut	},
/**/{	ETxString, (TText8*)"+CRING: ALT REL ASYNC/VOICE\r",		0, ETxEvent|EFailIfTimeOut	},
/**/{	ETxString, (TText8*)"+CRING: ALT REL ASYNC/REL ASYNC\r",	0, ETxEvent|EFailIfTimeOut	},
/**/{	ETxString, (TText8*)"+CRING: ALT VOICE/FAX\r",				0, ETxEvent|EFailIfTimeOut	},
	{	ETxString, (TText8*)"050\r",								0, ETxEvent|EFailIfTimeOut	},
	{	ETxStringOK, NULL,											0, ETxEvent|EFailIfTimeOut	},
	{	ERxString, (TText8*)"ATS0=0",								0, ERxEvent	},
/**/{	ETxString, (TText8*)"+CRING: VOICE\r",						0, ETxEvent|EFailIfTimeOut	},
	{	ETxStringOK, NULL,											0, ETxEvent|EFailIfTimeOut	},
	{	ERxString, (TText8*)"AT+CGMI",								0, ERxEvent	},
/**/{	ETxString, (TText8*)"+CRING: VOICE\r",						0, ETxEvent|EFailIfTimeOut	},
/**/{	ETxString, (TText8*)"+CREG: 2,3\r",							0, ETxEvent|EFailIfTimeOut	},
/**/{	ETxString, (TText8*)"+CRING: VOICE/REL SYNC\r",				0, ETxEvent|EFailIfTimeOut	},
/**/{	ETxString, (TText8*)"+CRING: ALT VOICE/SYNC\r",				0, ETxEvent|EFailIfTimeOut	},
/**/{	ETxString, (TText8*)"+CRING: FAX\r",						0, ETxEvent|EFailIfTimeOut	},
	{	ETxString, (TText8*)"ERICSSON\r",							0, ETxEvent|EFailIfTimeOut	},
	{	ETxStringOK, NULL,											0, ETxEvent|EFailIfTimeOut	},
	{	ERxString, (TText8*)"AT+CGMM",								0, ERxEvent	},
/**/{	ETxString, (TText8*)"+CRING: VOICE\r",						0, ETxEvent|EFailIfTimeOut	},
	{	ETxString, (TText8*)"1100801\r",							0, ETxEvent|EFailIfTimeOut	},
	{	ETxStringOK, NULL,											0, ETxEvent|EFailIfTimeOut	},
	{	ERxString, (TText8*)"AT+CGMR",								0, ERxEvent	},
/**/{	ETxString, (TText8*)"+CRING: VOICE/REL SYNC\r",				0, ETxEvent|EFailIfTimeOut	},
/**/{	ETxString, (TText8*)"+CRING: ALT VOICE/SYNC\r",				0, ETxEvent|EFailIfTimeOut	},
/**/{	ETxString, (TText8*)"+CRING: ALT VOICE/REL ASYNC\r",		0, ETxEvent|EFailIfTimeOut	},
/**/{	ETxString, (TText8*)"+CREG: 4,3\r",							0, ETxEvent|EFailIfTimeOut	},
/**/{	ETxString, (TText8*)"+CRING: ALT REL ASYNC/VOICE\r",		0, ETxEvent|EFailIfTimeOut	},
/**/{	ETxString, (TText8*)"+CRING: ALT REL ASYNC/REL ASYNC\r",	0, ETxEvent|EFailIfTimeOut	},
/**/{	ETxString, (TText8*)"+CRING: ALT VOICE/FAX\r",				0, ETxEvent|EFailIfTimeOut	},
/**/{	ETxString, (TText8*)"+CRING: VOICE\r",						0, ETxEvent|EFailIfTimeOut	},
	{	ETxString, (TText8*)"9808261341\r",							0, ETxEvent|EFailIfTimeOut	},
	{	ETxStringOK, NULL,											0, ETxEvent|EFailIfTimeOut	},
	{	ERxString, (TText8*)"AT+CGSN",								0, ERxEvent	},
/**/{	ETxString, (TText8*)"+CRING: FAX\r",						0, ETxEvent|EFailIfTimeOut	},
/**/{	ETxString, (TText8*)"+CRING: VOICE\r",						0, ETxEvent|EFailIfTimeOut	},
/**/{	ETxString, (TText8*)"+CRING: FAX\r",						0, ETxEvent|EFailIfTimeOut	},
	{	ETxString, (TText8*)"520002512954000\r",					0, ETxEvent|EFailIfTimeOut	},
	{	ETxStringOK, NULL,											0, ETxEvent|EFailIfTimeOut	},
	{	ERxString, (TText8*)"AT+CIMI",								0, ERxEvent	},
/**/{	ETxString, (TText8*)"+CRING: VOICE\r",						0, ETxEvent|EFailIfTimeOut	},
	{	ETxString, (TText8*)"ERROR\r",								0, ETxEvent|EFailIfTimeOut	},
	{	ERxString, (TText8*)"AT+CMGF=?",							0, ERxEvent	},
/**/{	ETxString, (TText8*)"+CRING: VOICE\r",						0, ETxEvent|EFailIfTimeOut	},
	{	ETxString, (TText8*)"ERROR\r",								0, ETxEvent|EFailIfTimeOut	},
	{	ERxString, (TText8*)"AT",									0, ERxEvent	},
/**/{	ETxString, (TText8*)"+CRING: VOICE\r",						0, ETxEvent|EFailIfTimeOut	},
/**/{	ETxString, (TText8*)"+CRING: VOICE/REL SYNC\r",				0, ETxEvent|EFailIfTimeOut	},
/**/{	ETxString, (TText8*)"+CREG: 2,4\r",							0, ETxEvent|EFailIfTimeOut	},
/**/{	ETxString, (TText8*)"+CRING: ALT VOICE/SYNC\r",				0, ETxEvent|EFailIfTimeOut	},
/**/{	ETxString, (TText8*)"+CRING: FAX\r",						0, ETxEvent|EFailIfTimeOut	},
	{	ETxStringOK, NULL,											0, ETxEvent|EFailIfTimeOut	},
	{	ERxString, (TText8*)"AT+CPMS=?",							0, ERxEvent	},
/**/{	ETxString, (TText8*)"+CRING: VOICE/REL SYNC\r",				0, ETxEvent|EFailIfTimeOut	},
/**/{	ETxString, (TText8*)"+CREG: 2,3\r",							0, ETxEvent|EFailIfTimeOut	},
/**/{	ETxString, (TText8*)"+CRING: ALT VOICE/SYNC\r",				0, ETxEvent|EFailIfTimeOut	},
/**/{	ETxString, (TText8*)"+CRING: VOICE\r",						0, ETxEvent|EFailIfTimeOut	},
	{	ETxString, (TText8*)"+CPMS: (\"ME\",\"SM\"),(\"ME\",\"SM\")\r",	0, ETxEvent|EFailIfTimeOut	},
	{	ETxStringOK, NULL,											0, ETxEvent|EFailIfTimeOut	},
	{	ERxString, (TText8*)"AT+CPMS?",								0, ERxEvent	},
/**/{	ETxString, (TText8*)"+CRING: FAX\r",						0, ETxEvent|EFailIfTimeOut	},
	{	ETxString, (TText8*)"+CPMS: \"SM\",0,15,\"SM\",0,15\r",	0, ETxEvent|EFailIfTimeOut	},
	{	ETxStringOK, NULL,											0, ETxEvent|EFailIfTimeOut	},
	{	ERxString, (TText8*)"AT+CPBS=?",							0, ERxEvent	},
/**/{	ETxString, (TText8*)"+CRING: VOICE\r",						0, ETxEvent|EFailIfTimeOut	},
	{	ETxString, (TText8*)"+CPBS: (\"ME\",\"SM\")\r",				0, ETxEvent|EFailIfTimeOut	},
	{	ETxStringOK, NULL,											0, ETxEvent|EFailIfTimeOut	},
	{	ERxString, (TText8*)"AT+CREG=0",							0, ERxEvent	},
/**/{	ETxString, (TText8*)"+CREG: 2,3\r",							0, ETxEvent|EFailIfTimeOut	},
/**/{	ETxString, (TText8*)"+CRING: VOICE\r",						0, ETxEvent|EFailIfTimeOut	},
	{	ETxStringOK, NULL,											0, ETxEvent|EFailIfTimeOut	},
	{	ERxString, (TText8*)"AT+CREG=2",							0, ERxEvent	},
/**/{	ETxString, (TText8*)"+CRING: VOICE\r",						0, ETxEvent|EFailIfTimeOut	},
	{	ETxString, (TText8*)"ERROR\r",								0, ETxEvent|EFailIfTimeOut	},
	{	ERxString, (TText8*)"AT+CREG=1",							0, ERxEvent	},
/**/{	ETxString, (TText8*)"+CRING: FAX\r",						0, ETxEvent|EFailIfTimeOut	},
/**/{	ETxString, (TText8*)"+CREG: 2,0\r",							0, ETxEvent|EFailIfTimeOut	},
/**/{	ETxString, (TText8*)"+CRING: VOICE/REL SYNC\r",				0, ETxEvent|EFailIfTimeOut	},
/**/{	ETxString, (TText8*)"+CRING: ALT VOICE/SYNC\r",				0, ETxEvent|EFailIfTimeOut	},
/**/{	ETxString, (TText8*)"+CRING: ALT VOICE/REL ASYNC\r",		0, ETxEvent|EFailIfTimeOut	},
/**/{	ETxString, (TText8*)"+CRING: ALT REL ASYNC/VOICE\r",		0, ETxEvent|EFailIfTimeOut	},
/**/{	ETxString, (TText8*)"+CRING: VOICE/REL SYNC\r",				0, ETxEvent|EFailIfTimeOut	},
/**/{	ETxString, (TText8*)"+CRING: ALT VOICE/SYNC\r",				0, ETxEvent|EFailIfTimeOut	},
/**/{	ETxString, (TText8*)"+CRING: FAX\r",						0, ETxEvent|EFailIfTimeOut	},
	{	ETxStringOK, NULL,											0, ETxEvent|EFailIfTimeOut	},
	{	ERxString, (TText8*)"AT+CREG?",								0, ERxEvent	},
/**/{	ETxString, (TText8*)"+CRING: VOICE\r",						0, ETxEvent|EFailIfTimeOut	},
	{	ETxString, (TText8*)"ERROR\r",								0, ETxEvent|EFailIfTimeOut	},
	{	ERxString, (TText8*)"AT+CRC=1",								0, ERxEvent	},
/**/{	ETxString, (TText8*)"+CRING: VOICE\r",						0, ETxEvent|EFailIfTimeOut	},
	{	ETxStringOK, NULL,											0, ETxEvent|EFailIfTimeOut	},
	{	ERxString, (TText8*)"AT+COPS?",								0, ERxEvent	},
/**/{	ETxString, (TText8*)"+CRING: VOICE/REL SYNC\r",				0, ETxEvent|EFailIfTimeOut	},
/**/{	ETxString, (TText8*)"+CRING: ALT VOICE/SYNC\r",				0, ETxEvent|EFailIfTimeOut	},
/**/{	ETxString, (TText8*)"+CRING: ALT VOICE/REL ASYNC\r",		0, ETxEvent|EFailIfTimeOut	},
/**/{	ETxString, (TText8*)"+CRING: ALT REL ASYNC/VOICE\r",		0, ETxEvent|EFailIfTimeOut	},
/**/{	ETxString, (TText8*)"+CRING: VOICE\r",						0, ETxEvent|EFailIfTimeOut	},
	{	ETxString, (TText8*)"+COPS: 0,0,\"UK VODAFONE\"\r",			0, ETxEvent|EFailIfTimeOut	},
	{	ETxStringOK, NULL,											0, ETxEvent|EFailIfTimeOut	},
	{	ERxString, (TText8*)"AT+CBC",								0, ERxEvent	},
/**/{	ETxString, (TText8*)"+CRING: VOICE/REL SYNC\r",				0, ETxEvent|EFailIfTimeOut	},
/**/{	ETxString, (TText8*)"+CRING: ALT VOICE/SYNC\r",				0, ETxEvent|EFailIfTimeOut	},
/**/{	ETxString, (TText8*)"+CREG: 0,0\r",							0, ETxEvent|EFailIfTimeOut	},
/**/{	ETxString, (TText8*)"+CRING: VOICE\r",						0, ETxEvent|EFailIfTimeOut	},
	{	ETxString, (TText8*)"ERROR\r",								0, ETxEvent|EFailIfTimeOut	},
	{	ERxString, (TText8*)"AT+CSQ",								0, ERxEvent	},
/**/{	ETxString, (TText8*)"+CRING: VOICE\r",						0, ETxEvent|EFailIfTimeOut	},
	{	ETxString, (TText8*)"ERROR\r",								0, ETxEvent|EFailIfTimeOut	},
	{	ERxString, (TText8*)"AT+CGCLASS",							0, ERxEvent	},
/**/{	ETxString, (TText8*)"+CRING: VOICE\r",						0, ETxEvent|EFailIfTimeOut	},
/**/{	ETxString, (TText8*)"+CRING: FAX\r",						0, ETxEvent|EFailIfTimeOut	},
	{	ETxString, (TText8*)"ERROR\r",								0, ETxEvent|EFailIfTimeOut	},
	{	ERxString, (TText8*)"AT+CGCLASS",							0, ERxEvent	},
/**/{	ETxString, (TText8*)"+CRING: FAX\r",						0, ETxEvent|EFailIfTimeOut	},
/**/{	ETxString, (TText8*)"+CRING: VOICE\r",						0, ETxEvent|EFailIfTimeOut	},
	{	ETxString, (TText8*)"ERROR\r",								0, ETxEvent|EFailIfTimeOut	},
	{	ERxString, (TText8*)"AT+CGQREQ",							0, ERxEvent	},
/**/{	ETxString, (TText8*)"+CRING: VOICE/REL SYNC\r",				0, ETxEvent|EFailIfTimeOut	},
/**/{	ETxString, (TText8*)"+CRING: ALT VOICE/SYNC\r",				0, ETxEvent|EFailIfTimeOut	},
/**/{	ETxString, (TText8*)"+CRING: ALT VOICE/REL ASYNC\r",		0, ETxEvent|EFailIfTimeOut	},
/**/{	ETxString, (TText8*)"+CRING: ALT REL ASYNC/VOICE\r",		0, ETxEvent|EFailIfTimeOut	},
/**/{	ETxString, (TText8*)"+CREG: 1,1\r",							0, ETxEvent|EFailIfTimeOut	},
/**/{	ETxString, (TText8*)"+CREG: 2,3\r",							0, ETxEvent|EFailIfTimeOut	},
/**/{	ETxString, (TText8*)"+CRING: VOICE\r",						0, ETxEvent|EFailIfTimeOut	},
	{	ETxString, (TText8*)"ERROR\r",								0, ETxEvent|EFailIfTimeOut	},		
	{	ERxString, (TText8*)"AT+CGATT",								0, ERxEvent	},
/**/{	ETxString, (TText8*)"+CRING: VOICE\r",						0, ETxEvent|EFailIfTimeOut	},
/**/{	ETxString, (TText8*)"+CREG: 2,3\r",							0, ETxEvent|EFailIfTimeOut	},
	{	ETxString, (TText8*)"ERROR\r",								0, ETxEvent|EFailIfTimeOut	},
	{	ERxString, (TText8*)"AT+CNMI=?",							0, ERxEvent	},
/**/{	ETxString, (TText8*)"+CRING: VOICE\r",						0, ETxEvent|EFailIfTimeOut	},
	{	ETxString, (TText8*)"+CNMI: (0-2),(0,1,3),(0),(0),(0,1)\r",	0, ETxEvent|EFailIfTimeOut	},
	{	ETxStringOK, NULL,											0, ETxEvent|EFailIfTimeOut	},
	{	ERxString, (TText8*)"AT+CBST=?",							0, ERxEvent	},
/**/{	ETxString, (TText8*)"+CRING: VOICE\r",						0, ETxEvent|EFailIfTimeOut	},
/**/{	ETxString, (TText8*)"+CRING: FAX\r",						0, ETxEvent|EFailIfTimeOut	},
	{	ETxStringOK, NULL,											0, ETxEvent|EFailIfTimeOut	},
	{	ETxStringOK, NULL,											0, ETxEvent|EFailIfTimeOut	},
	{	ERxString, (TText8*)"AT+CSCA?",								0, ERxEvent	},
/**/{	ETxString, (TText8*)"+CRING: VOICE/REL SYNC\r",				0, ETxEvent|EFailIfTimeOut	},

/**/{	ETxString, (TText8*)"+CRING: VOICE/REL SYNC\r",				0, ETxEvent|EFailIfTimeOut	},
/**/{	ETxString, (TText8*)"+CRING: ALT VOICE/SYNC\r",				0, ETxEvent|EFailIfTimeOut	},
/**/{	ETxString, (TText8*)"+CRING: ALT VOICE/REL ASYNC\r",		0, ETxEvent|EFailIfTimeOut	},
/**/{	ETxString, (TText8*)"+CRING: ALT REL ASYNC/VOICE\r",		0, ETxEvent|EFailIfTimeOut	},
/**/{	ETxString, (TText8*)"+CRING: ALT VOICE/SYNC\r",				0, ETxEvent|EFailIfTimeOut	},
/**/{	ETxString, (TText8*)"+CREG: 2,3\r",							0, ETxEvent|EFailIfTimeOut	},
/**/{	ETxString, (TText8*)"+CRING: VOICE\r",						0, ETxEvent|EFailIfTimeOut	},
/**/{	ETxString, (TText8*)"+CRING: FAX\r",						0, ETxEvent|EFailIfTimeOut	},
	{	ETxString, (TText8*)"+CSCA: \"441632960000\",145",			0, ETxEvent|EFailIfTimeOut	},
	{	ETxStringOK, NULL,											0, ETxEvent|EFailIfTimeOut	},
	{	ERxString, (TText8*)"AT+CGREG=1",							0, ERxEvent	},
/**/{	ETxString, (TText8*)"+CRING: VOICE\r",						0, ETxEvent|EFailIfTimeOut	},
/**/{	ETxString, (TText8*)"+CREG: 1,5\r",							0, ETxEvent|EFailIfTimeOut	},
/**/{	ETxString, (TText8*)"+CRING: FAX\r",						0, ETxEvent|EFailIfTimeOut	},
/**/{	ETxString, (TText8*)"+CRING: VOICE/REL SYNC\r",				0, ETxEvent|EFailIfTimeOut	},
/**/{	ETxString, (TText8*)"+CRING: ALT VOICE/SYNC\r",				0, ETxEvent|EFailIfTimeOut	},
/**/{	ETxString, (TText8*)"+CRING: ALT VOICE/REL ASYNC\r",		0, ETxEvent|EFailIfTimeOut	},
/**/{	ETxString, (TText8*)"+CRING: ALT REL ASYNC/VOICE\r",		0, ETxEvent|EFailIfTimeOut	},
/**/{	ETxString, (TText8*)"+CRING: VOICE\r",						0, ETxEvent|EFailIfTimeOut	},
	{	ETxString, (TText8*)"ERROR\r",								0, ETxEvent|EFailIfTimeOut	},
	{	ERxString, (TText8*)"AT*ECAM=1",							0, ERxEvent	},
/**/{	ETxString, (TText8*)"+CRING: ALT VOICE/REL ASYNC\r",		0, ETxEvent|EFailIfTimeOut	},
/**/{	ETxString, (TText8*)"+CRING: ALT REL ASYNC/VOICE\r",		0, ETxEvent|EFailIfTimeOut	},
/**/{	ETxString, (TText8*)"+CRING: ALT VOICE/SYNC\r",				0, ETxEvent|EFailIfTimeOut	},
/**/{	ETxString, (TText8*)"+CREG: 2,3\r",							0, ETxEvent|EFailIfTimeOut	},
	{	ETxString, (TText8*)"ERROR\r",								0, ETxEvent|EFailIfTimeOut	},
	//	End of initialization sequence ************************************************
	{	EWait,		 NULL,											2, ETimeOutEvent},
	{	EComplete,	 NULL,											0, 0	},
	};

/**
 * This class is used by the client side of the test.  It contains the method DriveETelApi
 * which is the primary client side method.  This class inherits from the CTestBase class.
 * All client side test classes must contain these entries.
 */
class CTestDriveUnsolicited : public CTestBase
	{
public:
	static CTestDriveUnsolicited* NewL(const TScriptList aScriptListEntry,
	TInt aVarDelay=0);

	// Called by the user interface from the C function PerformTestL.
	virtual TInt RunTestL();

protected:
	// Called by the CTestBase::StartEmulator method to drive the ETel API.
	virtual TInt DriveETelApiL();

private:
	CTestDriveUnsolicited(const TScriptList aScriptListEntry, TInt aVarDelay);
	void DriveTestL() ;

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
class CTestUnsolicited : public CATScriptEng
	{
public:
	static CTestUnsolicited* NewL(const TScript* aScript);
	static CTestUnsolicited* NewL(const TScript* aScript, const TInt aVarDelay);

	// Called by the scripting engine to implement the EExecuteSpecial Command.
	virtual void SpecificAlgorithmL(TInt aParam);

	// Called by the scripting engine to end the execution of the script.
	virtual void Complete(TInt aError);

	// Called by the scripting engine to start the execution of the script.
	virtual TInt Start();

private:
	CTestUnsolicited(const TScript* aScript);
	CTestUnsolicited(const TScript* aScript, const TInt aVarDelay);
	void ConstructL();

	const TScript* iScript;			//< Pointer to the first line in the associated script.
	};

#endif //__CUNSOLICITED__H__

