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
// Test performing a premature closure on a fax session and witness the TSY's response
// This file implements tests performing a premature closure of a fax session.
// See CDataCall.cpp for fully documented test scenario.
// 
//

/**
 @file
*/

#include <e32test.h>
#include <etelmm.h>
#include "Te_Loopbackcfaxpremclose.h"
#include "../../hayes/TSYCONFG.H" // for KInternetAccessPoint

//
// Test-side class
// With the assistance of the base class, this class must start the emulator
// and drive the ETel API.
//
CTestDrivePremClose* CTestDrivePremClose::NewL(const TScriptList aScriptListEntry)
	{
	CTestDrivePremClose* aA=new(ELeave) CTestDrivePremClose(aScriptListEntry);
	CleanupStack::PushL(aA);
	aA->ConstructL();
	CleanupStack::Pop();
	return aA;
	}

CTestDrivePremClose::CTestDrivePremClose(const TScriptList aScriptListEntry) : iScriptListEntry(aScriptListEntry)
	{}

TInt CTestDrivePremClose::RunTestL()
	{
	iCurrentScript=iScriptListEntry;
	return StartEmulatorL();
	}

TInt CTestDrivePremClose::DriveETelApiL()
//
// This function contains the real meat of the Client-side test code
//
	{
	_LIT(KFaxLineName,"Fax");
	_LIT(KMmPhoneName, "GsmPhone1");
	RMobilePhone mmPhone;
	INFO_PRINTF1(_L("Opening Multimode Phone\n"));
	TESTL(mmPhone.Open(iServer,KMmPhoneName)==KErrNone);

	RLine faxLine;
	INFO_PRINTF1(_L("Opening Fax Line\n"));
	TESTL(faxLine.Open(iPhone,KFaxLineName)==KErrNone);
	RCall faxCall;
	INFO_PRINTF1(_L("Opening New fax Call\n"));
	TESTL(faxCall.OpenNewCall(faxLine)==KErrNone);

	TRequestStatus stat1,stat2,reqStatus;
	RMobilePhone::TMMTableSettings tableSettings;
	tableSettings.iLocId=KInternetAccessPoint;
	RMobilePhone::TMMTableSettingsPckg tableSettingsPckg(tableSettings);
	iPhone.InitialiseMM(reqStatus , tableSettingsPckg); 	
	User::WaitForRequest(reqStatus);
	TESTL(reqStatus == KErrNone);

// Now wait for an incoming fax call...
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
	User::After(50000000L);
 	if (stat2!=KRequestPending)
 		User::WaitForRequest(stat2);

	faxCall.Close();

	TInt32 signalStrength=0;	
	TInt8 bar=0;
	mmPhone.GetSignalStrength(stat1,signalStrength,bar);
	User::WaitForRequest(stat1);
	INFO_PRINTF2(_L("Signal Strength = %d\n"), signalStrength);
	TESTL(stat1 == KErrNone);
	TESTL(signalStrength == -51);
	
	mmPhone.Close();
	faxLine.Close();
	return KErrNone;
	}

//
// Test-side class
// With the assistance of the base class, this class must start the emulator
// and drive the ETel API.
//
CTestDrivePremCloseB* CTestDrivePremCloseB::NewL(const TScriptList aScriptListEntry)
	{
	CTestDrivePremCloseB* aA=new(ELeave) CTestDrivePremCloseB(aScriptListEntry);
	CleanupStack::PushL(aA);
	aA->ConstructL();
	CleanupStack::Pop();
	return aA;
	}

CTestDrivePremCloseB::CTestDrivePremCloseB(const TScriptList aScriptListEntry) : iScriptListEntry(aScriptListEntry)	{}

TInt CTestDrivePremCloseB::RunTestL()
	{
	iCurrentScript=iScriptListEntry;
	return StartEmulatorL();
	}

TInt CTestDrivePremCloseB::DriveETelApiL()
//
// This function contains the real meat of the Client-side test code
//
	{
	_LIT(KFaxLineName,"Fax");
	_LIT(KMmPhoneName,"MmPhone1");

	RMobilePhone mmPhone;
	INFO_PRINTF1(_L("Opening Multimode Phone\n"));
	TESTL(mmPhone.Open(iServer,KMmPhoneName)==KErrNone);

	RLine faxLine;
	INFO_PRINTF1(_L("Opening Fax Line\n"));
	TESTL(faxLine.Open(iPhone,KFaxLineName)==KErrNone);
	RCall faxCall;
	INFO_PRINTF1(_L("Opening New fax Call\n"));
	TESTL(faxCall.OpenNewCall(faxLine)==KErrNone);

	TRequestStatus stat1,stat2,reqStatus;
	RMobilePhone::TMMTableSettings tableSettings;
	tableSettings.iLocId=KInternetAccessPoint;
	RMobilePhone::TMMTableSettingsPckg tableSettingsPckg(tableSettings);
	iPhone.InitialiseMM(reqStatus , tableSettingsPckg); 	
	User::WaitForRequest(reqStatus);
	TESTL(reqStatus == KErrNone);

// Now wait for an incoming fax call...
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
	User::After(38000000L);				// Wait for 38 secs to make sure the port is "access denied"

	RFax fax;
	fax.Open(faxCall);
	fax.TerminateFaxSession();
	fax.Close();

	faxCall.Close();

	TInt32 signalStrength;
	TInt8 bar=0;
	mmPhone.GetSignalStrength(stat1,signalStrength,bar);
	
	User::WaitForRequest(stat1);
	INFO_PRINTF2(_L("Signal Strength = %d\n"), signalStrength);
	TESTL(signalStrength == -51);
	TESTL(stat1 == KErrNone);

	mmPhone.Close();
	faxLine.Close();
	return KErrNone;
	}

//
// Emulator-side class
// With the assistance of the base class, this class must run the designated script
//
CTestPremClose* CTestPremClose::NewL(const TScript* aScript)
	{
	CTestPremClose* aA=new(ELeave) CTestPremClose(aScript);
	CleanupStack::PushL(aA);
	aA->ConstructL();
	CleanupStack::Pop();
	return aA;
	}

CTestPremClose::CTestPremClose(const TScript* aScript) : iScript(aScript)
	{}

void CTestPremClose::ConstructL()
	{
	CATScriptEng::ConstructL();
	}

TInt CTestPremClose::Start()
	{
	StartScript(iScript);
	return KErrNone;
	}

void CTestPremClose::SpecificAlgorithmL(TInt /* aParam */)
	{
	}

void CTestPremClose::Complete(TInt aError)
	{
	iReturnValue=aError;
	CActiveScheduler::Stop();
	}
