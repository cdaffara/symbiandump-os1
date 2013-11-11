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

#ifndef __CPHONEBOOK_H__
#define __CPHONEBOOK_H__

#include "Te_LoopbackTestStepBase.h"
#include "Te_LoopBackScriptEng.h"

//
// First a Script where everything is in order
//
const struct TScript KPhoneBookScript[]= {
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
	{	ETxString, (TText8*)"+CSQ: 15,99\r",		0, ETxEvent|EFailIfTimeOut	},
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

// Init Complete

	{	ERxString, (TText8*)"AT+CPBS=?",	0, ERxEvent	},
	{	ETxString, (TText8*)"+CPBS: (\"ME\",\"SM\")\r",	0, ETxEvent|EFailIfTimeOut	},
	{	ETxStringOK,	NULL,				0, ETxEvent|EFailIfTimeOut	},

//	Get info. about the ME iPhone book
	{	ERxString,	 (TText8*)"AT+CPBS=\"ME\"",	0, ERxEvent	},
	{	ETxStringOK,	NULL,					0, ETxEvent|EFailIfTimeOut	},

	{	ERxString,	(TText8*)"AT+CPBS?",	0, ERxEvent	},
	{	ETxString,	(TText8*)"+CPBS: \"ME\"\r",	0, ETxEvent|EFailIfTimeOut	},
	{	ETxStringOK,	NULL,					0, ETxEvent|EFailIfTimeOut	},

	{	ERxString,	(TText8*)"AT+CPBR=?",	0, ERxEvent	},
	{	ETxString,	(TText8*)"+CPBR: (1-99),20,18\r",	0, ETxEvent|EFailIfTimeOut	},
	{	ETxStringOK,	NULL,				0, ETxEvent|EFailIfTimeOut	},

	{	ERxString,	 (TText8*)"AT+CPBS=\"SM\"",	0, ERxEvent	},
	{	ETxStringOK,	NULL,					0, ETxEvent|EFailIfTimeOut	},

	{	ERxString,	(TText8*)"AT+CPBS?",	0, ERxEvent	},
	{	ETxString,	(TText8*)"+CPBS: \"SM\"\r",	0, ETxEvent|EFailIfTimeOut	},
	{	ETxStringOK,	NULL,					0, ETxEvent|EFailIfTimeOut	},

	{	ERxString,	(TText8*)"AT+CPBR=?",	0, ERxEvent	},
	{	ETxString,	(TText8*)"+CPBR: (1-100),20,14\r",	0, ETxEvent|EFailIfTimeOut	},
	{	ETxStringOK,	NULL,				0, ETxEvent|EFailIfTimeOut	},

// Phone book Requests.
// Open the SM iPhone book, get its caps and read all its entries.

	{	ERxString, (TText8*)"AT+CPBS=?",	0, ERxEvent	},
	{	ETxString, (TText8*)"+CPBS: (\"ME\",\"SM\")\r",	0, ETxEvent|EFailIfTimeOut	},
	{	ETxStringOK,	NULL,				0, ETxEvent|EFailIfTimeOut	},

	{	ERxString,	(TText8*)"AT+CPBS?",	0, ERxEvent	},
	{	ETxString,	(TText8*)"+CPBS: \"ME\"\r",	0, ETxEvent|EFailIfTimeOut	},
	{	ETxStringOK,	NULL,					0, ETxEvent|EFailIfTimeOut	},


	{	ERxString,	(TText8*)"AT+CPBR=?",	0, ERxEvent	},
	{	ETxString,	(TText8*)"+CPBR: (1-100),20,14\r",	0, ETxEvent|EFailIfTimeOut	},
	{	ETxStringOK,	NULL,				0, ETxEvent|EFailIfTimeOut	},

	{	ERxString, (TText8*)"AT+CPBS=?",	0, ERxEvent	},
	{	ETxString, (TText8*)"+CPBS: (\"ME\",\"SM\")\r",	0, ETxEvent|EFailIfTimeOut	},
	{	ETxStringOK,	NULL,				0, ETxEvent|EFailIfTimeOut	},



// Open the ME iPhone book, get its caps and read all its entries.
	{	ERxString,	 (TText8*)"AT+CPBS=\"ME\"",	0, ERxEvent	},
	{	ETxStringOK,	NULL,					0, ETxEvent|EFailIfTimeOut	},

	{	ERxString,	(TText8*)"AT+CPBS?",	0, ERxEvent	},
	{	ETxString,	(TText8*)"+CPBS: \"ME\"\r",	0, ETxEvent|EFailIfTimeOut	},
	{	ETxStringOK,	NULL,					0, ETxEvent|EFailIfTimeOut	},

	{	ERxString,	(TText8*)"AT+CPBR=?",	0, ERxEvent	},
	{	ETxString,	(TText8*)"+CPBR: (1-99),20,18\r",	0, ETxEvent|EFailIfTimeOut	},
	{	ETxStringOK,	NULL,				0, ETxEvent|EFailIfTimeOut	},

	{	ERxString,	(TText8*)"AT+CPBS?",	0, ERxEvent	},
	{	ETxString,	(TText8*)"+CPBS: \"ME\"\r",	0, ETxEvent|EFailIfTimeOut	},
	{	ETxStringOK,	NULL,					0, ETxEvent|EFailIfTimeOut	},

	{	ERxString,	 (TText8*)"AT+CPBS=\"SM\"",	0, ERxEvent	},
	{	ETxStringOK,	NULL,					0, ETxEvent|EFailIfTimeOut	},

// Delete the first five entries in the SM phonebook

	{	ERxString,	 (TText8*)"AT+CPBW=1",	0, ERxEvent	},
	{	ETxStringOK,	NULL,				0, ETxEvent|EFailIfTimeOut	},

	{	ERxString,	 (TText8*)"AT+CPBW=2",	0, ERxEvent	},
	{	ETxStringOK,	NULL,				0, ETxEvent|EFailIfTimeOut	},

	{	ERxString,	 (TText8*)"AT+CPBW=3",	0, ERxEvent	},
	{	ETxStringOK,	NULL,				0, ETxEvent|EFailIfTimeOut	},

	{	ERxString,	 (TText8*)"AT+CPBW=4",	0, ERxEvent	},
	{	ETxStringOK,	NULL,				0, ETxEvent|EFailIfTimeOut	},

	{	ERxString,	 (TText8*)"AT+CPBW=5",	0, ERxEvent	},
	{	ETxStringOK,	NULL,				0, ETxEvent|EFailIfTimeOut	},

	{	ERxString,	 (TText8*)"AT+CPBR=1,99",	0, ERxEvent	},
	{	ETxStringOK,	NULL,					0, ETxEvent|EFailIfTimeOut	},

	{	ERxString,	 (TText8*)"AT+CPBS=\"ME\"",	0, ERxEvent	},
	{	ETxStringOK,	NULL,					0, ETxEvent|EFailIfTimeOut	},

	{	ERxString,	 (TText8*)"AT+CPBW=1",	0, ERxEvent	},
	{	ETxStringOK,	NULL,				0, ETxEvent|EFailIfTimeOut	},

	{	ERxString,	 (TText8*)"AT+CPBW=2",	0, ERxEvent	},
	{	ETxStringOK,	NULL,				0, ETxEvent|EFailIfTimeOut	},
	
	{	ERxString,	 (TText8*)"AT+CPBW=3",	0, ERxEvent	},
	{	ETxStringOK,	NULL,				0, ETxEvent|EFailIfTimeOut	},
	
	{	ERxString,	 (TText8*)"AT+CPBW=4",	0, ERxEvent	},
	{	ETxStringOK,	NULL,				0, ETxEvent|EFailIfTimeOut	},
	
	{	ERxString,	 (TText8*)"AT+CPBW=5",	0, ERxEvent	},
	{	ETxStringOK,	NULL,				0, ETxEvent|EFailIfTimeOut	},

	{	ERxString,	 (TText8*)"AT+CPBR=1,99",	0, ERxEvent	},
	{	ETxStringOK,	NULL,					0, ETxEvent|EFailIfTimeOut	},

//TestPhoneBookReadWrite - Start
 
// Write a few entries to the SM iPhone book

	{	ERxString,	 (TText8*)"AT+CPBS=\"SM\"",	0, ERxEvent	},
	{	ETxStringOK,	NULL,					0, ETxEvent|EFailIfTimeOut	},

	{	ERxString,	 (TText8*)"AT+CPBW=1,\"02220111\",129,\"MuddyW\"",	0, ERxEvent	},
	{	ETxStringOK,	NULL,				0, ETxEvent|EFailIfTimeOut	},
	{	ERxString,	 (TText8*)"AT+CPBW=2,\"03334567\",129,\"ArethaF\"",	0, ERxEvent	},
	{	ETxStringOK,	NULL,				0, ETxEvent|EFailIfTimeOut	},

   	{	ERxString,	(TText8*)"AT+CPBR=1",	0, ERxEvent	},
	{	ETxString,	(TText8*)"+CPBR: 1,\"02220111\",0,\"MuddyW\"\r",	0, ETxEvent|EFailIfTimeOut	},
	{	ETxStringOK,	NULL,					0, ETxEvent|EFailIfTimeOut	},


	{	ERxString,	(TText8*)"AT+CPBR=2",	0, ERxEvent	},
	{	ETxString,	(TText8*)"+CPBR: 2,\"03334567\",0,\"ArethaF\"\r",	0, ETxEvent|EFailIfTimeOut	},
	{	ETxStringOK,	NULL,					0, ETxEvent|EFailIfTimeOut	},



//TestPhoneBookReadWrite - End

// TestAdvancedPhoneBook - Start

	{	ERxString, (TText8*)"AT+CPBS=?",	0, ERxEvent	},
	{	ETxString, (TText8*)"+CPBS: (\"ME\",\"SM\")\r",	0, ETxEvent|EFailIfTimeOut	},
	{	ETxStringOK,	NULL,				0, ETxEvent|EFailIfTimeOut	},

	{	ERxString,	(TText8*)"AT+CPBS?",	0, ERxEvent	},
	{	ETxString,	(TText8*)"+CPBS: \"SM\"\r",	0, ETxEvent|EFailIfTimeOut	},
	{	ETxStringOK,	NULL,					0, ETxEvent|EFailIfTimeOut	},

	{	ERxString,	(TText8*)"AT+CPBR=?",	0, ERxEvent	},
	{	ETxString,	(TText8*)"+CPBR: (1-100),20,14\r",	0, ETxEvent|EFailIfTimeOut	},
	{	ETxStringOK,	NULL,				0, ETxEvent|EFailIfTimeOut	},

	{	ERxString,	 (TText8*)"AT+CPBW=1,\"012345678901\",129,\"Tel 01\"",	0, ERxEvent	},
	{	ETxStringOK,	NULL,				0, ETxEvent|EFailIfTimeOut	},

	{	ERxString,	 (TText8*)"AT+CPBW=2,\"012345678902\",129,\"Tel 02\"",	0, ERxEvent	},
	{	ETxStringOK,	NULL,				0, ETxEvent|EFailIfTimeOut	},

	{	ERxString,	 (TText8*)"AT+CPBW=3,\"012345678903\",129,\"Tel 03\"",	0, ERxEvent	},
	{	ETxStringOK,	NULL,				0, ETxEvent|EFailIfTimeOut	},


	{	ERxString, (TText8*)"AT+CPBS=?",	0, ERxEvent	},
	{	ETxString, (TText8*)"+CPBS: (\"ME\",\"SM\")\r",	0, ETxEvent|EFailIfTimeOut	},
	{	ETxStringOK,	NULL,				0, ETxEvent|EFailIfTimeOut	},

	{	ERxString,	(TText8*)"AT+CPBS?",	0, ERxEvent	},
	{	ETxString,	(TText8*)"+CPBS: \"SM\"\r",	0, ETxEvent|EFailIfTimeOut	},
	{	ETxStringOK,	NULL,					0, ETxEvent|EFailIfTimeOut	},

	{	ERxString,	(TText8*)"AT+CPBR=?",	0, ERxEvent	},
	{	ETxString,	(TText8*)"+CPBR: (1-100),20,14\r",	0, ETxEvent|EFailIfTimeOut	},
	{	ETxStringOK,	NULL,				0, ETxEvent|EFailIfTimeOut	},

  	{	ERxString,	 (TText8*)"AT+CPBW=1",	0, ERxEvent	},
	{	ETxStringOK,	NULL,				0, ETxEvent|EFailIfTimeOut	},

	{	ERxString,	 (TText8*)"AT+CPBW=2",	0, ERxEvent	},
	{	ETxStringOK,	NULL,				0, ETxEvent|EFailIfTimeOut	},

// TestAdvancedPhoneBook - End

// TestPhoneBookNotifications - Start

	{	ERxString,	 (TText8*)"AT+CPBW=1,\"02220111\",129,\"MuddyW\"",	0, ERxEvent	},
	{	ETxStringOK,	NULL,				0, ETxEvent|EFailIfTimeOut	},

	{	ERxString,	 (TText8*)"AT+CPBW=1",	0, ERxEvent	},
	{	ETxStringOK,	NULL,				0, ETxEvent|EFailIfTimeOut	},

	{	ERxString,	 (TText8*)"AT+CPBW=1,\"02220111\",129,\"MuddyW\"",	0, ERxEvent	},
	{	ETxStringOK,	NULL,				0, ETxEvent|EFailIfTimeOut	},

	{	ERxString,	 (TText8*)"AT+CPBW=1",	0, ERxEvent	},
	{	ETxStringOK,	NULL,				0, ETxEvent|EFailIfTimeOut	},


// TestPhoneBookNotifications - End

// TestPhoneBookReadCancel - Start

	{	ERxString,	 (TText8*)"AT+CPBW=1,\"02220111\",129,\"MuddyW\"",	0, ERxEvent	},
	{	ETxStringOK,	NULL,				0, ETxEvent|EFailIfTimeOut	},
	{	ERxString,	 (TText8*)"AT+CPBW=2,\"03334567\",129,\"ArethaF\"",	0, ERxEvent	},
	{	ETxStringOK,	NULL,				0, ETxEvent|EFailIfTimeOut	},

// Read First time
	{	ERxString,	(TText8*)"AT+CPBR=1",	0, ERxEvent	},
	{	EWait,		NULL,							1,	ETimeOutEvent	},
	{	ETxString,	(TText8*)"+CPBR: 1,\"02220111\",0,\"MuddyW\"\r",	0, ETxEvent|EFailIfTimeOut	},
	{	ETxStringOK,	NULL,					0, ETxEvent|EFailIfTimeOut	},

// International Number Read and Write Tests
	{	ERxString,	 (TText8*)"AT+CPBW=1,\"+441234\",145,\"ABCD\"",	0, ERxEvent	},
	{	ETxStringOK,	NULL,				0, ETxEvent|EFailIfTimeOut	},
	{	ERxString,	 (TText8*)"AT+CPBW=2,\"+445678\",145,\"EFGH\"",	0, ERxEvent	},
	{	ETxStringOK,	NULL,				0, ETxEvent|EFailIfTimeOut	},

	{	ERxString,	(TText8*)"AT+CPBR=1,2",	0, ERxEvent	},
	{	ETxString,	(TText8*)"+CPBR: 1,\"+441234\",145,\"ABCD\"\r",	0, ETxEvent|EFailIfTimeOut	},
	{	ETxString,	(TText8*)"+CPBR: 2,\"+445678\",145,\"EFGH\"\r",	0, ETxEvent|EFailIfTimeOut	},
	{	ETxStringOK,	NULL,					0, ETxEvent|EFailIfTimeOut	},

// Testing Multiple Threads writing at the same time...
	{	ERxString,	 (TText8*)"AT+CPBW=1,\"12345678\",129,\"ABCDEFGH\"",	0, ERxEvent	},
	{	ETxStringOK,	NULL,				0, ETxEvent|EFailIfTimeOut	},
	{	ERxString,	 (TText8*)"AT+CPBW=1,\"12345678\",129,\"ABCDEFGH\"",	0, ERxEvent	},
	{	ETxStringOK,	NULL,				0, ETxEvent|EFailIfTimeOut	},

	{	EWait,		NULL,							1,	ETimeOutEvent	},

	{	EComplete,		NULL,				0, 0	} 
	};


class CTestDrivePhoneBook : public CTestBase
	{
public:
	static CTestDrivePhoneBook* NewL(const TScriptList aScriptListEntry);
	virtual TInt RunTestL();

protected:
	virtual TInt DriveETelApiL();

private:
	CTestDrivePhoneBook(const TScriptList aScriptListEntry);
	void GetPhoneInfoL();
	TRequestStatus IsPhoneBookAvailable (const TDesC& aPhoneBook, TInt &aCount);
	void ReadAndWriteL();
	void testGetPhBkCapsAndInfoL();
	TInt testClearPhBkL(const TDesC& phoneBookName);
	void testPhBkReadAndWrite();
	void testAdvancedPhBkL();
	void testPhBkNotificationsL();
	void testPhBkReadCancelL();
	void testPhoneBookReadAndWriteInternationalL();
	void testPhoneBookMultiWriteL();
	static TInt ThreadFnWritePhoneBookEntry(TAny*);

	// New methods using CPhoneBookBuffer
	void testPhoneBookReadAndWrite();
	void DotestPhoneBookReadAndWriteL();
	void testAdvancedPhoneBook(); 
	void DotestAdvancedPhoneBookL(); 
	void testPhoneBookNotifications();
	void DotestPhoneBookNotificationsL();
	void testPhoneBookReadCancel();
	void DotestPhoneBookReadCancelL();
	RMobilePhone iMmPhone;

	TBool iMESupported;

	TScriptList iScriptListEntry;
	};

class CTestPhoneBook : public CATScriptEng
	{
public:
	static CTestPhoneBook* NewL(const TScript* aScript);
	virtual void SpecificAlgorithmL(TInt aParam);
	virtual void Complete(TInt aError);
	virtual TInt Start();

private:
	CTestPhoneBook(const TScript* aScript);
	void ConstructL();

	const TScript* iScript;
	};

#endif
