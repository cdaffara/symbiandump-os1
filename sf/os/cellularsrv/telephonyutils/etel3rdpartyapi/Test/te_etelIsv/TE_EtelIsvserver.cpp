// Copyright (c) 2003-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// TE_ETEL3RDPARTYSERVER.CPP
// ETelISV test harness entry point.
// 
//

/**
 @file
*/


#include "TE_EtelIsvserver.h"
#include "TE_EtelIsvPhoneSettings.h"
#include "TE_EtelIsvNetworkInfo.h"
#include "TE_EtelIsvLineInfo.h"
#include "TE_EtelIsvSuppServices.h"
#include "TE_EtelIsvCallFunc.h"
#include "TE_EtelIsvNotifiers.h"
#include "TE_EtelIsvStress.h"


_LIT(KServerName,"TE_ETELISV1");


CEtelIsvTestServer* CEtelIsvTestServer::NewL()
/**
 * @return - Instance of the test server
 * Called inside the MainL() function to create and start the
 * CTestServer derived server.
 */
	{
	CEtelIsvTestServer * server = new (ELeave) CEtelIsvTestServer();
	CleanupStack::PushL(server);
	// CServer base class call
	server->StartL(KServerName);
	CleanupStack::Pop(server);
	return server;
	}

LOCAL_C void MainL()
/**
 * Much simpler, uses the new Rendezvous() call to sync with the client
 */
	{
	StartC32();

	CActiveScheduler* sched=NULL;
	sched=new(ELeave) CActiveScheduler;
	CActiveScheduler::Install(sched);
	CEtelIsvTestServer* server = NULL;
	// Create the CTestServer derived server
	TRAPD(err,server = CEtelIsvTestServer::NewL());
	if(!err)
		{
		// Sync with the client and enter the active scheduler
		RProcess::Rendezvous(KErrNone);
		sched->Start();
		}
	delete server;
	delete sched;
	}

GLDEF_C TInt E32Main()
/**
 * @return - Standard Epoc error code on exit
 */
	{
	CTrapCleanup* cleanup = CTrapCleanup::New();
	if(cleanup == NULL)
		{
		return KErrNoMemory;
		}
	TRAP_IGNORE(MainL());
	delete cleanup;
	return KErrNone;
    }
CTestStep* CEtelIsvTestServer::CreateTestStep(const TDesC& aStepName)
/**
 * @return - A CTestStep derived instance
 * Implementation of CTestServer pure virtual
 */
	{
	CTestStep*  testStep(NULL);

	//  Test for Phone settings support  //
	if (aStepName == _L("TestGetPhoneId"))
		{
		testStep = new CTestGetPhoneId;
		}
	else if (aStepName == _L("TestGetPhoneIdError"))
		{
		testStep = new CTestGetPhoneIdError;
		}
	else if (aStepName == _L("TestGetSubscriberId"))
		{
		testStep = new CTestGetSubscriberId;
		}
	else if (aStepName == _L("TestGetSubscriberIdError"))
		{
		testStep = new CTestGetSubscriberIdError;
		}
	else if (aStepName == _L("TestFlightModeWithPublishSubscribe"))
		{
		testStep = new CTestFlightModeWithPublishSubscribe;
		}
	else if (aStepName == _L("TestGetIndicator"))
		{
		testStep = new CTestGetIndicator;
		}
	else if (aStepName == _L("TestGetBatteryInfo"))
		{
		testStep = new CTestGetBatteryInfo;
		}
	else if (aStepName == _L("TestGetSignalStrength"))
		{
		testStep = new CTestGetSignalStrength;
		}
	else if (aStepName == _L("TestGetLockInfo"))
		{
		testStep = new CTestGetLockInfo;
		}
	else if (aStepName == _L("TestSendDTMFTones"))
		{
		testStep = new CTestSendDTMFTones;
		}
	else if (aStepName == _L("TestPhoneFuncFlightModeOnWithPublishSubscribe"))
		{
		testStep = new CTestPhoneFuncFlightModeOnWithPublishSubscribe;	
		}
	//  Test for Network information support  //
	else if (aStepName == _L("TestGetNetworkRegistrationStatus"))
		{
		testStep = new CTestGetNetworkRegistrationStatus;
		}
	else if (aStepName == _L("TestGetCurrentNetworkInfo"))
		{
		testStep = new CTestGetCurrentNetworkInfo;
		}
	else if (aStepName == _L("TestGetCurrentNetworkInfoEighteenChars"))
		{
		testStep = new CTestGetCurrentNetworkInfoEighteenChars;
		}
	else if (aStepName == _L("TestGetCurrentNetworkInfoEighteenCharsV2"))
		{
		testStep = new CTestGetCurrentNetworkInfoEighteenCharsV2;
		}
	else if (aStepName == _L("TestGetCurrentNetworkInfoTwentyThreeChars"))
		{
		testStep = new CTestGetCurrentNetworkInfoTwentyThreeChars;
		}
	else if (aStepName == _L("TestGetCurrentNetworkInfoTwentyThreeCharsV2"))
		{
		testStep = new CTestGetCurrentNetworkInfoTwentyThreeCharsV2;	
		}
	else if (aStepName == _L("TestGetCurrentNetworkName"))
		{
		testStep = new CTestGetCurrentNetworkName;
		}
	else if (aStepName == _L("TestGetOperatorName"))
		{
		testStep = new CTestGetOperatorName;
		}
	else if (aStepName == _L("TestNetworkFuncFlightModeOnWithPublishSubscribe"))
		{
		testStep = new CTestNetworkFuncFlightModeOnWithPublishSubscribe;
		}
	//  Test for line information functionality support  //
	else if (aStepName == _L("TestGetLineStatus"))
		{
		testStep = new CTestGetLineStatus(iMTCallCount);
		}
	else if (aStepName == _L("TestLineFuncFlightModeOnWithPublishSubscribe"))
		{
		testStep = new CTestLineFuncFlightModeOnWithPublishSubscribe;
		}
	//  Test for call functionality support  //
	else if (aStepName == _L("TestGetCallInfo"))
		{
		testStep = new CTestGetCallInfo;
		}
	else if (aStepName == _L("TestGetCallDynamicCaps"))
		{
		testStep = new CTestGetCallDynamicCaps;
		}
	else if (aStepName == _L("TestDialNewCall"))
		{
		testStep = new CTestDialNewCall;
		}
	else if (aStepName == _L("TestAnswerIncomingCall"))
		{
		testStep = new CTestAnswerIncomingCall(iMTCallCount);
		}
	else if (aStepName == _L("TestHold"))
		{
		testStep = new CTestHold;
		}
	else if (aStepName == _L("TestResume"))
		{
		testStep = new CTestResume;
		}
	else if (aStepName == _L("TestSwap"))
		{
		testStep = new CTestSwap;
		}
	else if (aStepName == _L("TestHoldResume2Calls"))
		{
		testStep = new CTestHoldResume2Calls;					
		}
	else if (aStepName == _L("TestHangup"))
		{
		testStep = new CTestHangup;	
		}
	else if (aStepName == _L("TestCallFuncFlightModeOnWithPublishSubscribe"))
		{
		testStep = new CTestCallFuncFlightModeOnWithPublishSubscribe(iMTCallCount);
		}
	else if (aStepName == _L("TestDeprecatedDataCallAPIs"))
	    {
		testStep = new CTestDeprecatedDataCallAPIs();
		}
	//  Test for supplementary services support  //
	else if (aStepName == _L("TestGetSupplementaryServiceStatus"))
		{
		testStep = new CTestGetSupplementaryServiceStatus;
		}
	else if (aStepName == _L("TestGetIdentityService"))
		{
		testStep = new CTestGetIdentityServiceStatus;
		}
	else if (aStepName == _L("TestSuppServFuncFlightModeOnWithPublishSubscribe"))
		{
		testStep = new CTestSuppServFuncFlightModeOnWithPublishSubscribe;
		}
	// Test for notifiers support //
	else if (aStepName == _L("TestRegisterForNotification"))
		{
		testStep = new CTestNotifyChange;
		}
	else if (aStepName == _L("TestRegisterForNotificationCancel"))
		{
		testStep = new CTestNotifyChangeCancel;
		}
	else if (aStepName == _L("TestCallAndNotify"))
		{
		testStep = new CTestCallAndNotify;
		}
	// Stress Test //
	else if (aStepName == _L("TestStress"))
		{
		testStep = new CTestStress(iMTCallCount);
		}
	else if (aStepName == _L("TestFailedOpen"))
		{
		testStep = new CTestFailedOpen();
		}
 	else if(aStepName == _L("TestGetLineStatusNoCaps"))
 		{
 		testStep = new CTestGetLineStatusNoCaps(iMTCallCount);			
 		}

	//
	// Set the test step name here to save code!!!
	//
	if (testStep != NULL)
		{
		testStep->SetTestStepName(aStepName);
		}

	return testStep;
	}
