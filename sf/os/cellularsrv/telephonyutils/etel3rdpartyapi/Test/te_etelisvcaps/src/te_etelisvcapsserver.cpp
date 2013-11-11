// Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// Implementation of TEF server for Etel3rdParty capability test.
// 
//

/**
 @file 
 @internalTechnology
*/

#include <rsshared.h>
#include "te_etelisvcapsserver.h"
#include "notifyfunctsteps.h"
#include "phonefunctsteps.h"
#include "callfunctsteps.h"
#include "linefunctsteps.h"
#include "networkfunctsteps.h"
#include "supplementaryfunctsteps.h"

// The server name. 
// Should match executable name. 
_LIT(KServerName,"te_etelisvcaps1");

CTe_CapsTestSuite* CTe_CapsTestSuite::NewL()
/**
 * @return - Instance of the test server
 * Same code for Secure and non-secure variants
 * Called inside the MainL() function to create and start the
 * CTestServer derived server.
 */
	{
	CTe_CapsTestSuite * server = new (ELeave) CTe_CapsTestSuite();
	CleanupStack::PushL(server);

	server->StartL(KServerName);
	CleanupStack::Pop(server);
	return server;
	}


CTestStep* CTe_CapsTestSuite::CreateTestStep(const TDesC& aStepName)
/**
 * @return - A CTestStep derived instance
 * Secure and non-secure variants
 * Implementation of CTestServer pure virtual
 */
	{
	CTestStep* testStep = NULL;
	if (aStepName == KCapsTestGetPhoneIdStr)
		{
		testStep = new CGetPhoneIdTestStep();
		}
	else if (aStepName == KCapsTestGetSubscriberIdStr)
		{
		testStep = new CGetSubscriberIdTestStep();
		}
	else if (aStepName == KCapsTestGetFlightModeStr)
		{
		testStep = new CGetFlightModeTestStep();
		}
	else if (aStepName == KCapsTestGetIndicatorStr)
		{
		testStep = new CGetIndicatorTestStep();
		}
	else if (aStepName == KCapsTestGetBatteryInfoStr)
		{
		testStep = new CGetBatteryInfoTestStep();
		}
	else if (aStepName == KCapsTestGetSignalStrengthStr)
		{
		testStep = new CGetSignalStrengthTestStep();
		}
	else if (aStepName == KCapsTestGetLockInfoStr)
		{
		testStep = new CGetLockInfoTestStep();
		}
	else if (aStepName == KCapsTestSendDTMFTonesStr)
		{
		testStep = new CSendDTMFTonesTestStep();
		}
	else if (aStepName == KCapsTestGetLineStatusStr)
		{
		testStep = new CGetLineStatusTestStep();
		}
	else if (aStepName == KCapsTestGetCallInfoStr)
		{
		testStep = new CGetCallInfoTestStep();
		}
	else if (aStepName == KCapsTestHoldStr)
		{
		testStep = new CHoldTestStep();
		}
	else if (aStepName == KCapsTestDialNewCallStr)
		{
		testStep = new CDialNewCallTestStep();
		}
	else if (aStepName == KCapsTestGetCallDynamicCapsStr)
		{
		testStep = new CGetCallDynamicCaps();
		}
	else if (aStepName == KCapsTestGetCallStatusStr)
		{
		testStep = new CGetCallStatus();
		}
	else if (aStepName == KCapsTestResumeStr)
		{
		testStep = new CResumeTestStep();
		}
	else if (aStepName == KCapsTestSwapStr)
		{
		testStep = new CSwapTestStep();
		}
	else if (aStepName == KCapsTestHangUpStr)
		{
		testStep = new CHangUpTestStep();
		}
	else if (aStepName == KCapsTestAnswerIncomingCallStr)
		{
		testStep = new CAnswerIncomingCallTestStep();
		}
	else if (aStepName == KCapsTestGetNetworkRegistrationStr)
		{
		testStep = new CGetNetworkRegistrationStatusTestStep();
		}
	else if (aStepName == KCapsTestGetNetworkInfoStr)
		{
		testStep = new CGetCurrentNetworkInfoTestStep();
		}
	else if (aStepName == KCapsTestGetNetworkNameStr)
		{
		testStep = new CGetCurrentNetworkNameTestStep();
		}
	else if (aStepName ==KCapsTestGetOperatorNameStr)
		{
		testStep = new CGetOperatorNameTestStep();
		}
	else if (aStepName == KCapsTestGetCallForwardingStr)
		{
		testStep = new CGetCallForwardingStatusTestStep();
		}
	else if (aStepName == KCapsTestGetCallBarringStr)
		{
		testStep = new CGetCallBarringStatusTestStep();
		}
	else if (aStepName == KCapsTestGetCallWaitingStr)
		{
		testStep = new CGetCallWaitingStatusTestStep();
		}
	else if (aStepName == KCapsTestGetIdentityServiceStr)
		{
		testStep = new CGetIdentityServiceTestStep();
		}
	else if (aStepName == KCapsTestFlightModeChangeStr)
		{
		testStep = new CNotifyFlightModeTestStep();
		}
	else if (aStepName == KCapsTestIndicatorChangeStr)
		{
		testStep = new CNotifyIndicatorTestStep();
		}
	else if (aStepName == KCapsTestBatteryInfoChange)
		{
		testStep = new CNotifyBatteryInfoTestStep(); 
		}
	else if (aStepName == KCapsTestSignalStrengthChange)
		{
		testStep = new CNotifySignalStrengthTestStep();
		}
	else if (aStepName == KCapsTestPin1LockInfoChange)
		{
		testStep = new CNotifyPin1LockInfoTestStep();
		}
	else if (aStepName == KCapsTestPin2LockInfoChange)
		{
		testStep = new CNotifyPin2LockInfoTestStep();
		}
	else if (aStepName == KCapsTestVoiceLineStatusChange)
		{
		testStep = new CNotifyVoiceLineStatusTestStep();
		}
	else if (aStepName == KCapsTestOwnedCall1StatusChange)
		{
		testStep = new CNotifyOwnedCall1StatusTestStep();
		}
	else if (aStepName == KCapsTestOwnedCall2StatusChange)
		{
		testStep = new CNotifyOwnedCall2StatusTestStep();
		}
	else if (aStepName == KCapsTestOwnedCall1RemotePartyInfoChange)
		{
		testStep = new CNotifyOwnedCall1RemotePartyInfoTestStep();
		}
	else if (aStepName == KCapsTestOwnedCall2RemotePartyInfoChange)
		{
		testStep = new CNotifyOwnedCall2RemotePartyInfoTestStep();
		}
	else if (aStepName == KCapsTestNetworkRegistrationStatusChange)
		{
		testStep = new CNotifyNetworkRegistrationStatusTestStep();
		}
	else if (aStepName == KCapsTestCurrentNetworkInfoChange)
		{
		testStep = new CNotifyCurrentNetworkInfoTestStep();
		}

	if (testStep != NULL)
		{
		testStep->SetTestStepName(aStepName);
		}
							
	return testStep;
	}



// Secure variants much simpler
// For EKA2, just an E32Main and a MainL()
LOCAL_C void MainL()
/**
 * Secure variant
 * Much simpler, uses the new Rendezvous() call to sync with the client
 */
	{
	StartC32();	
	// Leave the hooks in for platform security
#if (defined __DATA_CAGING__)
	RProcess().DataCaging(RProcess::EDataCagingOn);
	RProcess().DataCaging(RProcess::ESecureApiOn);
#endif
	CActiveScheduler* sched=NULL;
	sched=new(ELeave) CActiveScheduler;
	CActiveScheduler::Install(sched);
	CTe_CapsTestSuite* server = NULL;
	// Create the CTestServer derived server
	TRAPD(err,server = CTe_CapsTestSuite::NewL());
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
 * @return - Standard Epoc error code on process exit
 * Secure variant only
 * Process entry point. Called by client using RProcess API
 */
	{
	__UHEAP_MARK;
	CTrapCleanup* cleanup = CTrapCleanup::New();
	if(cleanup == NULL)
		{
		return KErrNoMemory;
		}
	TRAPD(err,MainL());
	delete cleanup;
	__UHEAP_MARKEND;
	return err;
	}



