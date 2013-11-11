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
// Test some "canceling while making a data call" scenarios. 
// This file implements some canceling calls while making a data call test scenarios.
// See CDataCall.cpp for fully documented test scenario.
// 
//

/**
 @file
*/

#include <e32test.h>
#include <etelmm.h>
#include "Te_LoopBackcdatacallcancel.h"
#include "../../hayes/TSYCONFG.H" // for KInternetAccessPoint

//
// Test-side class
// With the assistance of the base class, this class must start the emulator
// and drive the ETel API.
//
CTestDriveDataCallCancel* CTestDriveDataCallCancel::NewL(const TScriptList aScriptListEntry)
	{
	CTestDriveDataCallCancel* aA=new(ELeave) CTestDriveDataCallCancel(aScriptListEntry);
	CleanupStack::PushL(aA);
	aA->ConstructL();
	CleanupStack::Pop();
	return aA;
	}

CTestDriveDataCallCancel::CTestDriveDataCallCancel(const TScriptList aScriptListEntry) : iScriptListEntry(aScriptListEntry)
	{}

TInt CTestDriveDataCallCancel::RunTestL()
	{
	iCurrentScript=iScriptListEntry;
	return StartEmulatorL();
	}

TInt CTestDriveDataCallCancel::DriveETelApiL()
//
// This function contains the real meat of the Client-side test code
//
	{
	_LIT(KDataLineName,"Data");
	RLine line;
	INFO_PRINTF1(_L("Opening Data Line\n"));
	TESTL(line.Open(iPhone,KDataLineName)==KErrNone);

	INFO_PRINTF1(_L("Opening New Data Call\n"));
	RCall call;
	TESTL(call.OpenNewCall(line)==KErrNone);

	TRequestStatus stat0,reqStatus;

	RMobilePhone::TMMTableSettings tableSettings;
	tableSettings.iLocId=KInternetAccessPoint;
	RMobilePhone::TMMTableSettingsPckg tableSettingsPckg(tableSettings);
	iPhone.InitialiseMM(reqStatus , tableSettingsPckg); 	
	User::WaitForRequest(reqStatus);
	TESTL(reqStatus == KErrNone);

	_LIT(KDialString,"+1234");
	call.Dial(stat0,KDialString);

	User::After(1000000L); 
	call.DialCancel();

	User::WaitForRequest(stat0);
	TESTL((stat0==KErrCancel) || (stat0==KErrTimedOut) || (stat0==KErrNone));

	line.Close();
	call.Close();
	return KErrNone;
	}

//
// Emulator-side class
// With the assistance of the base class, this class must run the designated script
//
CTestDataCallCancel* CTestDataCallCancel::NewL(const TScript* aScript)
	{
	CTestDataCallCancel* aA=new(ELeave) CTestDataCallCancel(aScript);
	CleanupStack::PushL(aA);
	aA->ConstructL();
	CleanupStack::Pop();
	return aA;
	}

CTestDataCallCancel::CTestDataCallCancel(const TScript* aScript) : iScript(aScript)
	{}

void CTestDataCallCancel::ConstructL()
	{
	CATScriptEng::ConstructL();
	}

TInt CTestDataCallCancel::Start()
	{
	StartScript(iScript);
	return KErrNone;
	}

void CTestDataCallCancel::SpecificAlgorithmL(TInt /* aParam */)
	{
	}

void CTestDataCallCancel::Complete(TInt aError)
	{
	iReturnValue=aError;
	CActiveScheduler::Stop();
	}
