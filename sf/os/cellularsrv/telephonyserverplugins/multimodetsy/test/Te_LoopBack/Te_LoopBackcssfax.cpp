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
// This file implements simultaneous retrieval of signal strength information and an incoming
// fax.  Also, the test is repeated with retrieval of signal strength information and an incoming
// data call.
// See CDataCall.cpp for fully documented test scenario.
// 
//

/**
 @file
*/

#include <e32test.h>
#include <etelmm.h>
#include <et_clsvr.h>


#include "Te_LoopBackcssfax.h"
#include "../../hayes/TSYCONFG.H" // for KInternetAccessPoint

//
// Test-side class
// With the assistance of the base class, this class must start the emulator
// and drive the ETel API.
//
CTestDriveSsFax* CTestDriveSsFax::NewL(const TScriptList aScriptListEntry)
	{
	CTestDriveSsFax* aA=new(ELeave) CTestDriveSsFax(aScriptListEntry);
	CleanupStack::PushL(aA);
	aA->ConstructL();
	CleanupStack::Pop();
	return aA;
	}

CTestDriveSsFax::CTestDriveSsFax(const TScriptList aScriptListEntry) : iScriptListEntry(aScriptListEntry)
	{}

TInt CTestDriveSsFax::RunTestL()
	{
	iCurrentScript=iScriptListEntry;
	return StartEmulatorL();
	}

TInt CTestDriveSsFax::DriveETelApiL()
//
// This function contains the real meat of the Client-side test code
//
	{
	_LIT(KFaxLineName,"Fax");
	_LIT(KMmPhoneName,"GsmPhone1");

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
	User::After(2000000L);					// Wait for a 2secs to make sure the port is "access denied"

	INFO_PRINTF1(_L("Get Signal Strength\n"));
	TInt32 signalStrength;
	TInt8 bar=0;
	mmPhone.GetSignalStrength(stat1,signalStrength,bar);
	User::WaitForRequest(stat1);
	TESTL(stat1==KErrAccessDenied);			// The port is "access denied" by this time

	User::WaitForRequest(stat2);
	User::After(300000L);
	TESTL(faxCall.HangUp()==KErrNone);
	
	mmPhone.Close();
	faxLine.Close();
	faxCall.Close();
	return KErrNone;
	}

//
// Emulator-side class
// With the assistance of the base class, this class must run the designated script
//
CTestSsFax* CTestSsFax::NewL(const TScript* aScript)
	{
	CTestSsFax* aA=new(ELeave) CTestSsFax(aScript);
	CleanupStack::PushL(aA);
	aA->ConstructL();
	CleanupStack::Pop();
	return aA;
	}

CTestSsFax::CTestSsFax(const TScript* aScript) : iScript(aScript)
	{}

void CTestSsFax::ConstructL()
	{
	CATScriptEng::ConstructL();
	}

TInt CTestSsFax::Start()
	{
	StartScript(iScript);
	return KErrNone;
	}

void CTestSsFax::SpecificAlgorithmL(TInt /* aParam */)
	{
	}

void CTestSsFax::Complete(TInt aError)
	{
	iReturnValue=aError;
	CActiveScheduler::Stop();
	}

//
// Test-side class
// With the assistance of the base class, this class must start the emulator
// and drive the ETel API.
//

class RSpecialCall : public RCall
	{
public:
	void RecoverDataPortAsync(TRequestStatus& aStatus);
	};

void RSpecialCall::RecoverDataPortAsync(TRequestStatus& aStatus)
	{
	Blank(EEtelCallRecoverDataPort,aStatus);
	}

CTestDriveSSData* CTestDriveSSData::NewL(const TScriptList aScriptListEntry)
	{
	CTestDriveSSData* aA=new(ELeave) CTestDriveSSData(aScriptListEntry);
	CleanupStack::PushL(aA);
	aA->ConstructL();
	CleanupStack::Pop();
	return aA;
	}

CTestDriveSSData::CTestDriveSSData(const TScriptList aScriptListEntry) : iScriptListEntry(aScriptListEntry)
	{}

TInt CTestDriveSSData::RunTestL()
	{
	iCurrentScript=iScriptListEntry;
	return StartEmulatorL();
	}

TInt CTestDriveSSData::DriveETelApiL()
//
// This function contains the real meat of the Client-side test code
//
	{
	_LIT(KDataLineName,"Data");
	_LIT(KMmPhoneName,"GsmPhone1");

	RMobilePhone mmPhone;
	INFO_PRINTF1(_L("Opening Multimode Phone\n"));
	TESTL(mmPhone.Open(iServer,KMmPhoneName)==KErrNone);

	RLine line;
	INFO_PRINTF1(_L("Opening Data Line\n"));
	TESTL(line.Open(iPhone,KDataLineName)==KErrNone);

	INFO_PRINTF1(_L("Opening New Data Call\n"));
	RSpecialCall call;
	TESTL(call.OpenNewCall(line)==KErrNone);

	TRequestStatus stat,stat1,reqStatus;

	RMobilePhone::TMMTableSettings tableSettings;
	tableSettings.iLocId=KInternetAccessPoint;
	RMobilePhone::TMMTableSettingsPckg tableSettingsPckg(tableSettings);
	iPhone.InitialiseMM(reqStatus , tableSettingsPckg); 	
	User::WaitForRequest(reqStatus);
	TESTL(reqStatus == KErrNone);

	_LIT(KDialString,"+1234");
	TInt status = call.Dial(KDialString);
	TESTL(status == KErrNone);

	TInt32 signalStrength;
	TInt8 bar = 0;
	mmPhone.GetSignalStrength(stat1,signalStrength,bar);

	RCall::TCommPort commPort;
	TESTL(call.LoanDataPort(commPort)==KErrNone);

	RCommServ cs;
	TESTL(cs.Connect()==KErrNone);

	RComm port;
	TESTL(port.Open(cs,commPort.iPort,ECommShared)==KErrNone);

	port.Write(stat,KDataSsWriteTestData);
	User::WaitForRequest(stat);
	TESTL(stat.Int()==KErrNone);

    //-- a small delay between successive writes to the COM port
    //-- I had to insert it to fix mistiming between script execution and sending AT-commands to modem
    User::After(500000);		

	port.Write(stat,KDataSsWriteTestData);
	User::WaitForRequest(stat);
	TESTL(stat.Int()==KErrNone);

	port.Close();
	cs.Close();

	call.RecoverDataPort();

	User::After(500000L);

	User::WaitForRequest(stat1); // Result of GetSignalStrength()
	TESTL(stat1==KErrAccessDenied);

	TESTL(call.HangUp()==KErrNone);

	mmPhone.Close();
	line.Close();
	call.Close();
	return KErrNone;
	}

//
// Emulator-side class
// With the assistance of the base class, this class must run the designated script
//
CTestSSData* CTestSSData::NewL(const TScript* aScript)
	{
	CTestSSData* aA=new(ELeave) CTestSSData(aScript);
	CleanupStack::PushL(aA);
	aA->ConstructL();
	CleanupStack::Pop();
	return aA;
	}

CTestSSData::CTestSSData(const TScript* aScript) : iScript(aScript)
	{}

void CTestSSData::ConstructL()
	{
	CATScriptEng::ConstructL();
	}

TInt CTestSSData::Start()
	{
	StartScript(iScript);
	return KErrNone;
	}

void CTestSSData::SpecificAlgorithmL(TInt /* aParam */)
	{
	}

void CTestSSData::Complete(TInt aError)
	{
	iReturnValue=aError;
	CActiveScheduler::Stop();
	}
