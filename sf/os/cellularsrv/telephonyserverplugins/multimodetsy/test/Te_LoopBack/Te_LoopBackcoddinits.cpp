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
// Test the TSY with a series of slightly strange modem initialisation responses
// This file implements various odd initialization sequences and modem responses 
// to test the TSY.
// See CDataCall.cpp for fully documented test scenario.
// 
//

/**
 @file
*/


#include <e32test.h>
#include <mmretrieve.h>
#include <etelmm.h>
#include "Te_LoopBackcoddinits.h"
#include "../../hayes/TSYCONFG.H" // for KInternetAccessPoint

//
// Test-side class
// With the assistance of the base class, this class must start the emulator
// and drive the ETel API.
//
CTestDriveOddInit* CTestDriveOddInit::NewL(const TScriptList aScriptListEntry)
	{
	CTestDriveOddInit* aA = new(ELeave) CTestDriveOddInit(aScriptListEntry);
	CleanupStack::PushL(aA);
	aA->ConstructL();
	CleanupStack::Pop();
	return (aA);
	}

CTestDriveOddInit::CTestDriveOddInit(const TScriptList aScriptListEntry) :
	iScriptListEntry(aScriptListEntry)
	{}

CTestDriveOddInit::~CTestDriveOddInit()
	{}

TInt CTestDriveOddInit::RunTestL()
	{
	iCurrentScript = iScriptListEntry;
	return StartEmulatorL();
	}

TInt CTestDriveOddInit::DriveETelApiL()
//
// This function contains the real meat of the Client-side test code
//
	{
	TRequestStatus reqStatus;
	RMobilePhone::TMMTableSettings tableSettings;
	tableSettings.iLocId=KInternetAccessPoint;
	RMobilePhone::TMMTableSettingsPckg tableSettingsPckg(tableSettings);
	iPhone.InitialiseMM(reqStatus , tableSettingsPckg); 	
	User::WaitForRequest(reqStatus);
	TESTL(reqStatus==KErrNone);

	return (KErrNone);
	}

//
// Emulator-side class
// With the assistance of the base class, this class must run the designated script
//
CTestOddInit* CTestOddInit::NewL(const TScript* aScript)
	{
	CTestOddInit* aA = new (ELeave) CTestOddInit(aScript);
	CleanupStack::PushL(aA);
	aA->ConstructL();
	CleanupStack::Pop();
	return (aA);
	}

CTestOddInit::CTestOddInit(const TScript* aScript) :
	iScript(aScript)
	{}

void CTestOddInit::ConstructL()
	{
	CATScriptEng::ConstructL();
	}

CTestOddInit::~CTestOddInit()
	{}

TInt CTestOddInit::Start()
	{
	StartScript(iScript);
	return (KErrNone);
	}

void CTestOddInit::SpecificAlgorithmL(TInt /* aParam */)
	{
	}

void CTestOddInit::Complete(TInt aError)
	{
	iReturnValue=aError;
	CActiveScheduler::Stop();
	}
