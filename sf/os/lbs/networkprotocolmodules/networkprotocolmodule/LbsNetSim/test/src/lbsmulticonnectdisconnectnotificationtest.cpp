// Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// Example CTestStep derived implementation
// 
//

/**
 @file LbsConnectDisconnectTest.cpp
*/
#include "lbsmulticonnectdisconnectnotificationtest.h"
#include "Te_lbsnetsimtestSuiteDefs.h"

CLbsMultiConnectDisconnectNotificationTest::~CLbsMultiConnectDisconnectNotificationTest()
/**
 * Destructor
 */
	{
	}

CLbsMultiConnectDisconnectNotificationTest::CLbsMultiConnectDisconnectNotificationTest()
/**
 * Constructor
 */
	{
	// **MUST** call SetTestStepName in the constructor as the controlling
	// framework uses the test step name immediately following construction to set
	// up the step's unique logging ID.
	SetTestStepName(KMultiConnectDisconnectNotificationTest);
	}

TVerdict CLbsMultiConnectDisconnectNotificationTest::doTestStepPreambleL()
/**
 * @return - TVerdict code
 * Override of base class virtual
 */
	{
	__UHEAP_MARK;
	
	// Get the delay
	GetIntFromConfig(ConfigSection(), KDelay, iDelay);
	// Assistance data provider
	TInt provider;
	GetIntFromConfig(ConfigSection(), KProvider, provider);
	iProvider = TUid::Uid(provider);
	// Step mode?
	TBool stepMode = EFalse;
	GetBoolFromConfig(ConfigSection(), KStepMode, stepMode);

	iState = EStart;
	iTest = new (ELeave) CAOTest(this, iDelay, stepMode);  // Stopped in postamble
	iGateway = new (ELeave) CAOGateway(this); // Stopped during test
	
	SetTestStepResult(EPass);
	return TestStepResult();
	}


TVerdict CLbsMultiConnectDisconnectNotificationTest::doTestStepL()
/**
 * @return - TVerdict code
 * Override of base class pure virtual
 * Our implementation only gets called if the base class doTestStepPreambleL() did
 * not leave. That being the case, the current test result value will be EPass.
 */
	{
	
	if (TestStepResult()==EPass)
		{
		iTest->Activate();
		iAs->Start();
		
		if (iTest->Finished() && iGateway->Finished() && (State() == EFinished))
			{
			SetTestStepResult(EPass);
			}
		else
			{
			SetTestStepResult(EFail);
			}
		}
		
	  return TestStepResult();
	}



TVerdict CLbsMultiConnectDisconnectNotificationTest::doTestStepPostambleL()
/**
 * @return - TVerdict code
 * Override of base class virtual
 */
	{
	delete iTest;
	delete iGateway;

	__UHEAP_MARKEND;
	
	return TestStepResult();
	}

//
// CAOGateway
CLbsMultiConnectDisconnectNotificationTest::CAOGateway::CAOGateway(CLbsMultiConnectDisconnectNotificationTest* aBase) :
	CAOGateway_Base(aBase)	
	{
	iLbsNetSim = new(ELeave) RLbsNetSim();
	iLbsNetSim->ConnectL(this);
	}
	
CLbsMultiConnectDisconnectNotificationTest::CAOGateway::~CAOGateway()
	{
	Cancel();

	iLbsNetSim->Close();
	delete iLbsNetSim;
	}
	
void CLbsMultiConnectDisconnectNotificationTest::CAOGateway::RunL()
	{
	_LIT(KError, "CAOGateway::RunL() - Error");
	User::Panic(KError, KErrAbort);
	}
	
void CLbsMultiConnectDisconnectNotificationTest::CAOGateway::DoCancel()
	{
	}
	
TBool CLbsMultiConnectDisconnectNotificationTest::CAOGateway::Finished()
	{
	return ETrue;
	}
	
//
// CAOTest

CLbsMultiConnectDisconnectNotificationTest::CAOTest::CAOTest(CLbsMultiConnectDisconnectNotificationTest* aBase, TInt aDelay, TBool aStepMode) :
	CAOTest_Base(aBase, aDelay, aStepMode),
	iConnectionCount(0)
	{
	iLbsNetSimTest = new(ELeave) RLbsNetSimTest();
	}
	
CLbsMultiConnectDisconnectNotificationTest::CAOTest::~CAOTest()
	{
	Cancel();

	delete iLbsNetSimTest;
	}
	
void CLbsMultiConnectDisconnectNotificationTest::CAOTest::RunL()
	{
	switch (iBase->State())
		{
		case EStart:
			{
			iBase->SetState(EConnectTest1);
			Activate();
			break;
			}
		case EConnectTest1:
			{
			iLbsNetSimTest->ConnectL(this);
			if (iDelay > 0)
				{
				iLbsNetSimTest->SetResponseTime(iDelay, ETrue);
				}
			iLbsNetSimTest->SetAssistanceDataProvider(iBase->Provider());
			iLbsNetSimTest->SetStepMode(iStepMode);
			iBase->SetState(ENone);
			break;
			}
		case EGotConnectTest1:
			{
			iBase->SetState(EDisconnectTest);
			Activate();
			break;
			}
		case EDisconnectTest:
			{
			iLbsNetSimTest->Close();
			iBase->SetState(EConnectTest2);
			Activate();
			break;
			}
		case EConnectTest2:
			{
			iLbsNetSimTest->ConnectL(this);
			if (iDelay > 0)
				{
				iLbsNetSimTest->SetResponseTime(iDelay, ETrue);
				}
			iLbsNetSimTest->SetAssistanceDataProvider(iBase->Provider());
			iBase->SetState(ENone);
			break;
			}
		case EGotConnectTest2:
			{
			iBase->SetState(EFinished);
			Activate();
			break;
			}
		case EFinished:
			{
			iLbsNetSimTest->Close();
			
			// Test complete
			CActiveScheduler::Stop();
			break;
			}
		} // switch
	}
	
void CLbsMultiConnectDisconnectNotificationTest::CAOTest::DoCancel()
	{
	}
	
TBool CLbsMultiConnectDisconnectNotificationTest::CAOTest::Finished()
	{
	return (iConnectionCount == 2);
	}
	
void CLbsMultiConnectDisconnectNotificationTest::CAOTest::Connected()
	{
	++iConnectionCount;
	if (iConnectionCount == 1)
		{
		iBase->SetState(EGotConnectTest1);
		Activate();
		}
	else if (iConnectionCount == 2)
		{
		iBase->SetState(EGotConnectTest2);
		Activate();
		}
	}
