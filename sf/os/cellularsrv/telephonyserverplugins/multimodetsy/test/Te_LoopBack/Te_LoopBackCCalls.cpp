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
*/

#include <e32test.h>
#include "Te_LoopBackCCalls.h"

#include "../../hayes/TSYCONFG.H" // for KInternetAccessPoint

const TInt KOneSecond=1000000;		// One second Pause

//
// Test-side class
// With the assistance of the base class, this class must start the emulator
// and drive the ETel API.
//
CTestDriveAllCalls* CTestDriveAllCalls::NewL(const TScriptList aScriptListEntry)
	{
	CTestDriveAllCalls* phoneLine=new(ELeave) CTestDriveAllCalls(aScriptListEntry);
	CleanupStack::PushL(phoneLine);
	phoneLine->ConstructL();
	CleanupStack::Pop();
	return phoneLine;
	}

CTestDriveAllCalls::CTestDriveAllCalls(const TScriptList aScriptListEntry) : iScriptListEntry(aScriptListEntry)
	{
	
	}


TInt CTestDriveAllCalls::RunTestL()
	{
	iCurrentScript=iScriptListEntry;
	return StartEmulatorL();
	}

TInt CTestDriveAllCalls::DriveETelApiL()
//
// This function contains the real meat of the Client-side test code
//
	{
	TRequestStatus	reqStatus;
	INFO_PRINTF1(_L("Initialising the Phone..."));

	RMobilePhone::TMMTableSettings tableSettings;
	tableSettings.iLocId=KInternetAccessPoint;
	RMobilePhone::TMMTableSettingsPckg tableSettingsPckg(tableSettings);
	iPhone.InitialiseMM(reqStatus , tableSettingsPckg); 	

	User::WaitForRequest(reqStatus);

	TESTL(reqStatus == KErrNone);

	ERR_PRINTF1(_L("Voice Call tests"));
	TestVoiceCallL();
	User::After(5*KOneSecond);		// Give the user time to see the test results

	ERR_PRINTF1(_L("Cancel Requests test"));
	TestCancelRequestsL();
	User::After(5*KOneSecond);		// Give the user time to see the test results

	ERR_PRINTF1(_L("Data Call tests"));
	TestDataCallL();
	
	return KErrNone;
	}

void CTestDriveAllCalls::TestVoiceCallL()
/**
 * This function performs Voice call tests
 */
	{
	RProcess().SetPriority(EPriorityForeground);
	RThread().SetPriority(EPriorityNormal);
	_LIT(KVoiceLineName,"Voice");
	TRequestStatus status1;
	
	// Open a voice line and call
	RLine voiceLine;
	INFO_PRINTF1(_L("Opening Voice Line"));
	TESTL(voiceLine.Open(iPhone,KVoiceLineName)==KErrNone);

	RCall voiceCall;
	INFO_PRINTF1(_L("Opening New Voice Call"));
	TESTL(voiceCall.OpenNewCall(voiceLine)==KErrNone);

	// Get the call capabilities
	INFO_PRINTF1(_L("\nGet Call Capabilities...\n"));
	RCall::TCaps callCaps;
	TInt ret=voiceCall.GetCaps(callCaps);
	CHECKPOINT_EXPR((ret == KErrNone || ret == KErrNotSupported), _L("*** Failed RCall::GetCaps() ***"));
	if (ret==KErrNotSupported)
		INFO_PRINTF1(_L("Get Call Capabilities is NOT Supported\n"));
	if (ret==KErrNone)
		{
		INFO_PRINTF1(_L("Get Call Capabilities is Successful\n"));
		
		if ((callCaps.iFlags&RCall::KCapsVoice) !=0)
			INFO_PRINTF1(_L("This call supports Voice calls\n"));
		}

	// Get the Call Status (The script has posted +CRING: VOICE, so the call is ringing!
	INFO_PRINTF1(_L("\nGet Call Status...\n"));
	RCall::TStatus callStatus;
	ret=voiceCall.GetStatus(callStatus);
	CHECKPOINT_EXPR((ret == KErrNone || ret == KErrNotSupported), _L("*** Failed RCall::GetStatus() ***"));
	if (ret==KErrNotSupported)
		INFO_PRINTF1(_L("Get Call Status is NOT Supported\n"));
	if (ret==KErrNone)
		{
		TESTL(callStatus==RCall::EStatusRinging);
		INFO_PRINTF1(_L("This Voice call is currently ringing\n"));
		}

	// Answer an incoming call 
	voiceCall.AnswerIncomingCall(status1);		
	User::WaitForRequest(status1);
	TESTL(status1 == KErrNone);

	// Call Parameters
	INFO_PRINTF1(_L("\nGet Call Parameters...\n"));
	RCall::TCallParams callParams; 
	RCall::TCallParamsPckg callParamsPckg(callParams);
	ret=voiceCall.GetCallParams(callParamsPckg);
	CHECKPOINT_EXPR((ret == KErrNone || ret == KErrNotSupported), _L("*** Failed RCall::GetCallParams() ***"));
	if (ret==KErrNotSupported)
		INFO_PRINTF1(_L("Get Call Parameters is NOT Supported\n"));
	if (ret==KErrNone)
		INFO_PRINTF1(_L("Get Call Parameters is successful\n"));
	
	INFO_PRINTF1(_L("The call is in progress...\n"));
	User::After(5*KOneSecond);

	// Bearer Service Information
	INFO_PRINTF1(_L("\nGet Bearer Service Info...\n"));
	RCall::TBearerService bearerServiceInfo;
	ret=voiceCall.GetBearerServiceInfo(bearerServiceInfo);
	CHECKPOINT_EXPR((ret == KErrNone || ret == KErrNotSupported), _L("*** Failed RCall::GetBearerServiceInfo() ***"));
	if (ret==KErrNotSupported)
		INFO_PRINTF1(_L("Get BearerService Information is NOT Supported\n"));
	if (ret==KErrNone)
		INFO_PRINTF1(_L("Get BearerService Information is successful\n"));

	// Call Duration
	INFO_PRINTF1(_L("Get Call Duration...\n"));
	TTimeIntervalSeconds callDuration;
	ret=voiceCall.GetCallDuration(callDuration);
	CHECKPOINT_EXPR((ret == KErrNone || ret == KErrNotSupported), _L("*** Failed RCall::GetCallDuration() ***"));
	if (ret==KErrNotSupported)
		{
		INFO_PRINTF1(_L("Get Current Call Duration is NOT Supported"));
		}
	if (ret==KErrNone)
		{
		INFO_PRINTF2(_L("Call duration = %d"), callDuration.Int());
		}

	// Call Information
	INFO_PRINTF1(_L("\nGet Current Call Info..."));
	RCall::TCallInfo callInfo;
	ret=voiceCall.GetInfo(callInfo);
	CHECKPOINT_EXPR((ret == KErrNone || ret == KErrNotSupported), _L("*** Failed RCall::GetCallInfo() ***"));
	if (ret==KErrNotSupported)
		INFO_PRINTF1(_L("Get Current Call info is NOT Supported"));
	if (ret==KErrNone)
		{
		INFO_PRINTF1(_L("Displaying Information about the Current Call"));
		INFO_PRINTF2(_L("Call name: %S"), &callInfo.iCallName);
		INFO_PRINTF2(_L("Line name: %S"), &callInfo.iLineName);
		if (callInfo.iHookStatus==RCall::EHookStatusOff)
			{
			INFO_PRINTF1(_L("Current Hook Status: Off"));
			}
		if (callInfo.iStatus==RCall::EStatusConnected)
			{
			INFO_PRINTF1(_L("Current Call Status: Connected"));
			}
		}


	User::After(5*KOneSecond);		// Give the user time to see the test results

	// Call Ownership information
	INFO_PRINTF1(_L("Get Call Ownership..\n"));
	RCall::TOwnershipStatus callOwner;
	ret=voiceCall.GetOwnershipStatus(callOwner);
	CHECKPOINT_EXPR((ret == KErrNone || ret == KErrNotSupported), _L("*** Failed RCall::GetOwnershipStatus() ***"));
	if (ret==KErrNotSupported)
		{
		INFO_PRINTF1(_L("Get Call OwnerShip is NOT Supported"));
		}
	else if (ret==KErrNone)
		{
		INFO_PRINTF1(_L("Get Call OwnerShip is Supported"));
		
		switch(callOwner)
			{
			case RCall::EOwnershipUnowned:
				INFO_PRINTF1(_L("The call is unowned"));
				break;
			case RCall::EOwnershipOwnedByAnotherClient:
				INFO_PRINTF1(_L("The call is owned by another Client"));
				break;
			case RCall::EOwnershipOwnedByThisClient:
				INFO_PRINTF1(_L("The call is owned by this client"));
				break;
			default:
				INFO_PRINTF1(_L("Error in Call Ownership details"));
				break;
			}	
		}
	else
		INFO_PRINTF2(_L("Get Call Ownership returned Error: %d"), ret);
		
	
	// Transfer Ownership Request (Note that no other client is interested in this call!)
	INFO_PRINTF1(_L("Voice Call 1 is trying to Transfer Ownership of the Call.."));
	ret=voiceCall.TransferOwnership();
	INFO_PRINTF2(_L("Transfer Ownership Request returned %d"), ret);
	if (ret==KErrEtelNoClientInterestedInThisCall)
		INFO_PRINTF1(_L("There is no Client interested in this call."));


	// Hang up Cancel Request
	TRequestStatus cancelStatus;
	INFO_PRINTF1(_L("Hang Up Cancel Request")); 
	voiceCall.HangUp(cancelStatus);
	User::After(3*KOneSecond);		// Want the TSY to receive the Hang up command.
	voiceCall.HangUpCancel();
	User::WaitForRequest(cancelStatus);
	TESTL(cancelStatus==KErrNone);
	if (cancelStatus==KErrNone)
		{
		INFO_PRINTF1(_L("Hang up cancel Request Unsuccessful."));
		INFO_PRINTF1(_L("The Voice call has been dropped."));
		INFO_PRINTF1(_L("This is expected behaviour!."));
		}
	else 
		{
		INFO_PRINTF1(_L("Hang up cancel Request Successful."));
		INFO_PRINTF1(_L("This is unexpected!. Going to assume that the call has been dropped"));
		}
	
	// Close the voice line and calls
	voiceLine.Close();
	voiceCall.Close();
 	RProcess().SetPriority(EPriorityHigh);
 	RThread().SetPriority(EPriorityMuchMore);	
	}

void CTestDriveAllCalls::TestCancelRequestsL()
/**
 * This function tests Cancel Requests, Notifications in particular.
 */
	{
	_LIT(KVoiceLineName,"Voice");
	
	// Open a voice line and call
	RLine voiceLine;
	INFO_PRINTF1(_L("Opening Voice Line"));
	TESTL(voiceLine.Open(iPhone,KVoiceLineName)==KErrNone);

	RCall voiceCall;
	INFO_PRINTF1(_L("Opening a new Voice Call"));
	TESTL(voiceCall.OpenNewCall(voiceLine)==KErrNone);

	RCall voiceCall2;
	INFO_PRINTF1(_L("Opening a 2nd Voice Call"));
	TESTL(voiceCall2.OpenNewCall(voiceLine)==KErrNone);

	// Answer Incoming call Cancel Request
	TRequestStatus cancelStatus;
	INFO_PRINTF1(_L("Answer Incoming Call Cancel Request")); 
	voiceCall.AnswerIncomingCall(cancelStatus);
	voiceCall.AnswerIncomingCallCancel();
	User::WaitForRequest(cancelStatus);
	INFO_PRINTF2(_L("Answer Incoming Cancel status = %d"), cancelStatus.Int());

	// Notify Hook Change Cancel Request
	INFO_PRINTF1(_L("Notify Hook Change Cancel Request")); 
	RCall::THookStatus hookStatus;
	voiceCall.NotifyHookChange(cancelStatus, hookStatus);
	voiceCall.NotifyHookChangeCancel();
	User::WaitForRequest(cancelStatus);
	INFO_PRINTF2(_L("Notify Hook Change Cancel status = %d"), cancelStatus.Int());

	// Notify Caps Change Cancel Request
	INFO_PRINTF1(_L("Notify Caps change Cancel Request")); 
	RLine::TCaps lineCaps;
	voiceLine.NotifyCapsChange(cancelStatus, lineCaps);
	voiceLine.NotifyCapsChangeCancel();
	User::WaitForRequest(cancelStatus);
	INFO_PRINTF2(_L("Notify caps Change Cancel status = %d"), cancelStatus.Int());

	// Notify Call Status Change Cancel Request
	INFO_PRINTF1(_L("Notify CallStatusChange Cancel Request...")); 
	RCall::TStatus callStatus1;
	voiceCall.NotifyStatusChange(cancelStatus, callStatus1);
	voiceCall.NotifyStatusChangeCancel();
	User::WaitForRequest(cancelStatus);
	INFO_PRINTF2(_L("Notify CallStatusChange Cancel: %d"), cancelStatus.Int());

	// Notify CallDuration Change Cancel Request
	INFO_PRINTF1(_L("NotifyCallDurationChange Cancel Request...")); 
	TTimeIntervalSeconds callDuration;
	voiceCall.NotifyCallDurationChange(cancelStatus, callDuration);
	voiceCall.NotifyCallDurationChangeCancel();
	User::WaitForRequest(cancelStatus);
	INFO_PRINTF2(_L("Notify CallDurationChange Cancel: %d"), cancelStatus.Int());

	// Acquire Ownership Cancel Request 
	INFO_PRINTF1(_L("AcquireOwnership Cancel Request..."));
	voiceCall2.AcquireOwnership(cancelStatus);
	voiceCall2.AcquireOwnershipCancel();
	User::WaitForRequest(cancelStatus);
	if (cancelStatus==KErrEtelCallNotActive)
		{
		INFO_PRINTF1(_L("Voice Call 2 is not currently active"));
		}
	else
		{
		INFO_PRINTF2(_L("AcquireOwnership Cancel status: %d"), cancelStatus.Int());
		}

	// Close the voice line and call
	voiceLine.Close();
	voiceCall.Close();
	voiceCall2.Close();
	}

void CTestDriveAllCalls::TestDataCallL()
/**
 * This function performs Data calls tests.
 */
	{
	// The script has posted a +CRING: REL ASYNC, so a data call is expected
	INFO_PRINTF1(_L("\nAnswering an incoming data call.\n"));

	_LIT(KDataLineName,"Data");
	
	// Open a Data line
	RLine dataLine;
	INFO_PRINTF1(_L("Opening Data Line"));
	TESTL(dataLine.Open(iPhone,KDataLineName)==KErrNone);

	// Post a Notification on an Incoming call
	TRequestStatus notifyStatus;
	TName callName;
	dataLine.NotifyIncomingCall(notifyStatus, callName);
	User::WaitForRequest(notifyStatus);
	TESTL(notifyStatus==KErrNone);	
	INFO_PRINTF1(_L("An incoming data call has been detected."));

	// Open and answer a Data call
	RCall dataCall;
	TRequestStatus status1;
	TESTL(dataCall.OpenExistingCall(dataLine, callName)==KErrNone);
	
	dataCall.AnswerIncomingCall(status1);		
	User::WaitForRequest(status1);
	TESTL(status1 == KErrNone);

	// Get the call's status
	RCall::TStatus callStatus;
	TESTL(dataCall.GetStatus(callStatus)==KErrNone);
	if (callStatus==RCall::EStatusConnected)
		{
		INFO_PRINTF1(_L("Call Status: CONNECTED"));
		}

	// LoanDataPort (Cancel) request
	INFO_PRINTF1(_L("Loan Data Port Cancel request."));
	RCall::TCommPort commPort;
	TRequestStatus cancelStatus;
	dataCall.LoanDataPort(cancelStatus,commPort);
	User::After(3*KOneSecond);		// Want the request to get through to the TSY
	dataCall.LoanDataPortCancel();
	User::WaitForRequest(cancelStatus);
	INFO_PRINTF2(_L("LoanDataPort Cancel: %d"), cancelStatus.Int());
	
	if (cancelStatus==KErrNone)
		{
		INFO_PRINTF1(_L("LoanDataPort Cancel request NOT successful."));
		}
	else
		{
		INFO_PRINTF1(_L("LoanDataPort cancel request was successful."));
		INFO_PRINTF1(_L("This is unexpected, the rest of the test will not be executed!!"));
		INFO_PRINTF1(_L("Closing the data line and call."));
		dataLine.Close();	
		dataCall.Close();		
		return;
		}

	// Connect to the Comms Server and Open a serial port
	INFO_PRINTF1(_L("Connecting to the Comms server and opening a serial port."));
	RCommServ commServer;
	TESTL(commServer.Connect()==KErrNone);

	RComm port;
	TESTL(port.Open(commServer,commPort.iPort,ECommShared)==KErrNone);

	// Write some data to the serial port
	TRequestStatus writeStatus;
	_LIT8(KTestData,"Telephony Test Code\r\n");
	port.Write(writeStatus,KTestData);
	User::WaitForRequest(writeStatus);
	TESTL(writeStatus==KErrNone);
	INFO_PRINTF1(_L("Data has been successfully written to the serial port"));

	// Close the serial port & the comms Server. Recover the data port & hang up the call.
	port.Close();
	commServer.Close();
	TESTL(dataCall.RecoverDataPort()==KErrNone);

	INFO_PRINTF1(_L("Hanging up the Data call."));
	TESTL(dataCall.HangUp()==KErrNone);

	// Close the Data line and call
	dataLine.Close();
	dataCall.Close();		
	}


//
// Emulator-side class
// With the assistance of the base class, this class must run the designated script
//
CTestAllCalls* CTestAllCalls::NewL(const TScript* aScript)
	{
	CTestAllCalls* allCalls=new(ELeave) CTestAllCalls(aScript);
	CleanupStack::PushL(allCalls);
	allCalls->ConstructL();
	CleanupStack::Pop();
	return allCalls;
	}

CTestAllCalls::CTestAllCalls(const TScript* aScript) : iScript(aScript)
	{}

void CTestAllCalls::ConstructL()
	{
	CATScriptEng::ConstructL();
	}

TInt CTestAllCalls::Start()
	{
	StartScript(iScript);
	return KErrNone;
	}

void CTestAllCalls::SpecificAlgorithmL(TInt /* aParam */)
	{
    }

void CTestAllCalls::Complete(TInt aError)
	{
	iReturnValue=aError;
	CActiveScheduler::Stop();
	}
