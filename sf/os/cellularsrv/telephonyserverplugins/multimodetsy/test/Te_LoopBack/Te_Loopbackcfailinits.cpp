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
// Test out the TSY with a series of initialisation failure causes
// This file implements tests of the TSY with a series of initialization failure causes.
// See CDataCall.cpp for fully documented test scenario.
// 
//

/**
 @file
*/

#include <e32test.h>
#include <etelmm.h>
#include "Te_Loopbackcfailinits.h"

//
// Test-side class
// With the assistance of the base class, this class must start the emulator
// and drive the ETel API.
//
CTestDriveFailInit* CTestDriveFailInit::NewL(const TScriptList aScriptListEntry)
	{
	CTestDriveFailInit* aA=new(ELeave) CTestDriveFailInit(aScriptListEntry);
	CleanupStack::PushL(aA);
	aA->ConstructL();
	CleanupStack::Pop();
	return aA;
	}

CTestDriveFailInit::CTestDriveFailInit(const TScriptList aScriptListEntry) : iScriptListEntry(aScriptListEntry)
	{}

TInt CTestDriveFailInit::RunTestL()
	{
	iCurrentScript=iScriptListEntry;
	return StartEmulatorL();
	}

TInt CTestDriveFailInit::DriveETelApiL()
//
// This function contains the real meat of the Client-side test code
//
	{
	RMobileSmsMessaging sms;
	TRequestStatus stat0, stat1;
	INFO_PRINTF1(_L("Opening Sms Messaging\n"));
	TESTL(sms.Open(iPhone)==KErrNone);

	iPhone.Initialise(stat0);
	sms.SetReceiveMode (stat1, RMobileSmsMessaging::EReceiveStored);

	User::WaitForRequest(stat0);  
	TESTL(stat0==KErrEtelModemNotDetected);
	INFO_PRINTF1(_L("Phone initialisation completed with KErrEtelModemNotDetected.\n"));

	User::WaitForRequest(stat1);
	TESTL(stat0==KErrEtelModemNotDetected);
	INFO_PRINTF1(_L("SMS SetReceiveMode completed with KErrEtelModemNotDetected.\n"));

	// Normally, sms.ReceiveMessage would be called. As this test is expected to fail,
	// we don't have to call it.

	iPhone.Initialise(stat0);

	User::WaitForRequest(stat0);
	TESTL(stat0==KErrEtelModemNotDetected);

	sms.Close();
	return KErrNone;
	}

//
// Emulator-side class
// With the assistance of the base class, this class must run the designated script
//
CTestFailInit* CTestFailInit::NewL(const TScript* aScript)
	{
	CTestFailInit* aA=new(ELeave) CTestFailInit(aScript);
	CleanupStack::PushL(aA);
	aA->ConstructL();
	CleanupStack::Pop();
	return aA;
	}

CTestFailInit::CTestFailInit(const TScript* aScript) : iScript(aScript)
	{}

void CTestFailInit::ConstructL()
	{
	CATScriptEng::ConstructL();
	}

TInt CTestFailInit::Start()
	{
	StartScript(iScript);
	return KErrNone;
	}

void CTestFailInit::SpecificAlgorithmL(TInt /* aParam */)
	{
	}

void CTestFailInit::Complete(TInt aError)
	{
	iReturnValue=aError;
	CActiveScheduler::Stop();
	}
