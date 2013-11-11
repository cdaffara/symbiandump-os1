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
// This file implements a two fax reception test scenario.
// See CDataCall.cpp for fully documented test scenario.
// 
//

/**
 @file
*/

#include <e32test.h>
#include <etelmm.h>
#include "Te_LoopBackctwofaxrx.h"
#include "../../hayes/TSYCONFG.H" // for KInternetAccessPoint

//
// Test-side class
// With the assistance of the base class, this class must start the emulator
// and drive the ETel API.
//
CTestDriveTwoFaxRx* CTestDriveTwoFaxRx::NewL(const TScriptList aScriptListEntry)
	{
	CTestDriveTwoFaxRx* aA=new(ELeave) CTestDriveTwoFaxRx(aScriptListEntry);
	CleanupStack::PushL(aA);
	aA->ConstructL();
	CleanupStack::Pop();
	return aA;
	}

CTestDriveTwoFaxRx::CTestDriveTwoFaxRx(const TScriptList aScriptListEntry) : iScriptListEntry(aScriptListEntry)
	{}

TInt CTestDriveTwoFaxRx::RunTestL()
	{
	iCurrentScript=iScriptListEntry;
	return StartEmulatorL();
	}

TInt CTestDriveTwoFaxRx::DriveETelApiL()
//
// This function contains the real meat of the Client-side test code
//
	{
	_LIT(KFaxLineName,"Fax");

	RLine faxLine;
	INFO_PRINTF1(_L("Opening Fax Line\n"));
	TESTL(faxLine.Open(iPhone,KFaxLineName)==KErrNone);

	TRequestStatus stat1,stat2,reqStatus;
	RMobilePhone::TMMTableSettings tableSettings;
	tableSettings.iLocId=KInternetAccessPoint;
	RMobilePhone::TMMTableSettingsPckg tableSettingsPckg(tableSettings);
	iPhone.InitialiseMM(reqStatus , tableSettingsPckg); 	
	User::WaitForRequest(reqStatus);
	TESTL(reqStatus == KErrNone);

// Now wait for an incoming fax call...
	INFO_PRINTF1(_L("Waiting for an incoming fax call\n"));
	TName callName;
	faxLine.NotifyIncomingCall(stat1,callName);
	User::WaitForRequest(stat1);
	TESTL(stat1==KErrNone);

	RCall faxCall;
	INFO_PRINTF1(_L("Opening Fax Call to answer\n"));
	TESTL(faxCall.OpenNewCall(faxLine)==KErrNone);

	INFO_PRINTF1(_L("Answering a Fax Call\n"));
	RCall::TFaxSessionSettings faxSessionSettings;
	faxSessionSettings.iMode=RCall::EReceive;
	faxSessionSettings.iFaxRetrieveType=RCall::EFaxOnDemand;
	faxSessionSettings.iFaxClass=EClassAuto;
	faxSessionSettings.iFaxId.Zero();
	faxSessionSettings.iMaxSpeed=14400;
	faxSessionSettings.iMinSpeed=12000;
	faxSessionSettings.iRxResolution=EFaxNormal;
	faxSessionSettings.iRxCompression=EModifiedHuffman;
	faxCall.SetFaxSettings(faxSessionSettings);
	faxCall.AnswerIncomingCall(stat2);
	User::WaitForRequest(stat2);
	User::After(300000L);
	TESTL(faxCall.HangUp()==KErrNone);
	faxCall.Close();

// Now wait for a second incoming fax call...
	INFO_PRINTF1(_L("Waiting for a second incoming fax call\n"));
	TName callName2;
	faxLine.NotifyIncomingCall(stat1,callName2);
	User::WaitForRequest(stat1);
	TESTL(stat1==KErrNone);

	RCall faxCall2;
	INFO_PRINTF1(_L("Opening Fax Call to answer\n"));
	TESTL(faxCall2.OpenNewCall(faxLine)==KErrNone);

	INFO_PRINTF1(_L("Answering a Fax Call\n"));
	faxSessionSettings.iMode=RCall::EReceive;
	faxSessionSettings.iFaxRetrieveType=RCall::EFaxOnDemand;
	faxSessionSettings.iFaxClass=EClassAuto;
	faxSessionSettings.iFaxId.Zero();
	faxSessionSettings.iMaxSpeed=14400;
	faxSessionSettings.iMinSpeed=12000;
	faxSessionSettings.iRxResolution=EFaxNormal;
	faxSessionSettings.iRxCompression=EModifiedHuffman;
	faxCall2.SetFaxSettings(faxSessionSettings);
	faxCall2.AnswerIncomingCall(stat2);
	User::WaitForRequest(stat2);
	User::After(300000L);
	TESTL(faxCall2.HangUp()==KErrNone);
	faxCall2.Close();

	faxLine.Close();
	return KErrNone;
	}

//
// Emulator-side class
// With the assistance of the base class, this class must run the designated script
//
CTestTwoFaxRx* CTestTwoFaxRx::NewL(const TScript* aScript)
	{
	CTestTwoFaxRx* aA=new(ELeave) CTestTwoFaxRx(aScript);
	CleanupStack::PushL(aA);
	aA->ConstructL();
	CleanupStack::Pop();
	return aA;
	}

CTestTwoFaxRx::CTestTwoFaxRx(const TScript* aScript) : iScript(aScript)
	{}

void CTestTwoFaxRx::ConstructL()
	{
	CATScriptEng::ConstructL();
	}

TInt CTestTwoFaxRx::Start()
	{
	StartScript(iScript);
	return KErrNone;
	}

void CTestTwoFaxRx::SpecificAlgorithmL(TInt /* aParam */)
	{
	}

void CTestTwoFaxRx::Complete(TInt aError)
	{
	iReturnValue=aError;
	CActiveScheduler::Stop();
	}
