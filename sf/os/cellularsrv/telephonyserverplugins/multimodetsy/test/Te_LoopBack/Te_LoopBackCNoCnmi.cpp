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

#include <e32test.h>
#include <etelmm.h>
#include "Te_LoopBackCNoCnmi.h"

#include "../../hayes/TSYCONFG.H" // for KInternetAccessPoint

//
// Test-side class
// With the assistance of the base class, this class must start the emulator
// and drive the ETel API.
//
CTestDriveNoCnmi* CTestDriveNoCnmi::NewL(const TScriptList aScriptListEntry)
	{
	CTestDriveNoCnmi* cnmi=new(ELeave) CTestDriveNoCnmi(aScriptListEntry);
	CleanupStack::PushL(cnmi);
	cnmi->ConstructL();
	CleanupStack::Pop();
	return cnmi;
	}

CTestDriveNoCnmi::CTestDriveNoCnmi(const TScriptList aScriptListEntry) : iScriptListEntry(aScriptListEntry)
	{}

TInt CTestDriveNoCnmi::RunTestL()
	{
	iCurrentScript=iScriptListEntry;
	return StartEmulatorL();
	}

TInt CTestDriveNoCnmi::DriveETelApiL()
/**
 * This file contains a slightly different initialisation sequence. This iPhone does not
 * return a response to the AT+CNMI=? Query.
 * @return KErrNone
 */
	{
	// Open and initialise the iPhone object
	_LIT(KMmPhoneName,"GsmPhone1");
	TESTL(iCnPhone.Open(iServer,KMmPhoneName)==KErrNone);
	
	TRequestStatus	reqStatus;
	INFO_PRINTF1(_L("Initialising the Phone...\n"));

	RMobilePhone::TMMTableSettings tableSettings;
	tableSettings.iLocId=KInternetAccessPoint;
	RMobilePhone::TMMTableSettingsPckg tableSettingsPckg(tableSettings);
	iCnPhone.InitialiseMM(reqStatus , tableSettingsPckg); 	

	User::WaitForRequest(reqStatus);

	TESTL(reqStatus == KErrNone);


	// Post a Modem Notification request
	TRequestStatus modemStatus;
	RPhone::TModemDetection detected;
	iCnPhone.NotifyModemDetected(modemStatus, detected);

	// Get the iPhone's status 
	RPhone::TStatus phoneStatus1;
	TInt ret=iCnPhone.GetStatus(phoneStatus1);
	CHECKPOINT(ret, KErrNone, _L("*** Error retrieving the Phone's status ***\n"));
	if (ret==KErrNone)
		{
		TESTL(phoneStatus1.iModemDetected==RPhone::EDetectedPresent);
		INFO_PRINTF1(_L("The iPhone's modem has been detected\n"));
		TESTL(phoneStatus1.iMode==RPhone::EModeIdle);
		INFO_PRINTF1(_L("The iPhone is currently idle\n"));
		}

	// Establish a call in order to change the status of the iPhone. 
	// This should enable the modem notification request to complete.
	_LIT(KVoiceLineName,"Voice");
	RLine voiceLine;				
	TESTL(voiceLine.Open(iCnPhone,KVoiceLineName)==KErrNone);
	RCall voiceCall;
	TESTL(voiceCall.OpenNewCall(voiceLine)==KErrNone);

	// Answer an incoming call 
	INFO_PRINTF1(_L("Answer incoming call...\n"));
	TRequestStatus status;
	voiceCall.AnswerIncomingCall(status);		
	User::WaitForRequest(status);
	TESTL(status==KErrNone);

	// Get the iPhone's current status 
	RPhone::TStatus phoneStatus2;
	ret=iCnPhone.GetStatus(phoneStatus2);
	CHECKPOINT(ret, KErrNone, _L("*** Error retrieving the Phone's status ***\n"));

	// Confirm that the iPhone's current status is different from its initial status
	if (ret==KErrNone)
		{
		TESTL(phoneStatus2.iMode!=phoneStatus1.iMode);
		if (phoneStatus2.iMode == RPhone::EModeUnknown)
			{
			INFO_PRINTF1(_L("Phone mode: unknown\n"));
			}
		else if (phoneStatus2.iMode == RPhone::EModeEstablishingLink)
			{
			INFO_PRINTF1(_L("Phone mode: establishing link\n"));
			}
		else if (phoneStatus2.iMode == RPhone::EModeOnlineData)
			{
			INFO_PRINTF1(_L("Phone mode: on line data\n"));
			}
		else if (phoneStatus2.iMode == RPhone::EModeOnlineCommand)
			{
			INFO_PRINTF1(_L("Phone mode: on line command\n"));
			}
		else
			{
			ERR_PRINTF1(_L("Error: Phone mode undefined.\n"));
			User::Leave(KErrGeneral) ;
			}
		}
	
	// Complete the modem notification request
	User::WaitForRequest(modemStatus);

	// Hang up the call and Close the voice line and call
	voiceCall.HangUp(status);
	User::WaitForRequest(status);
	TESTL(status==KErrNone);
	voiceLine.Close();
	voiceCall.Close();

	// Close the iPhone
	iCnPhone.Close();		
	return KErrNone;
	}


//
// Emulator-side class
// With the assistance of the base class, this class must run the designated script
//
CTestNoCnmi* CTestNoCnmi::NewL(const TScript* aScript)
	{
	CTestNoCnmi* cnmi=new(ELeave) CTestNoCnmi(aScript);
	CleanupStack::PushL(cnmi);
	cnmi->ConstructL();
	CleanupStack::Pop();
	return cnmi;
	}

CTestNoCnmi::CTestNoCnmi(const TScript* aScript) : iScript(aScript)
	{}

void CTestNoCnmi::ConstructL()
	{
	CATScriptEng::ConstructL();
	}

TInt CTestNoCnmi::Start()
	{
	StartScript(iScript);
	return KErrNone;
	}

void CTestNoCnmi::SpecificAlgorithmL(TInt /* aParam */)
	{
	}

void CTestNoCnmi::Complete(TInt aError)
	{
	iReturnValue=aError;
	CActiveScheduler::Stop();
	}
