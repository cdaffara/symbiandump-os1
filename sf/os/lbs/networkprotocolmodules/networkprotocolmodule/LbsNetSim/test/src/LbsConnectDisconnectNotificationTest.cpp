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
#include "LbsConnectDisconnectNotificationTest.h"
#include "Te_lbsnetsimtestSuiteDefs.h"

CLbsConnectDisconnectNotificationTest::~CLbsConnectDisconnectNotificationTest()
/**
 * Destructor
 */
	{
	}

CLbsConnectDisconnectNotificationTest::CLbsConnectDisconnectNotificationTest()
/**
 * Constructor
 */
	{
	// **MUST** call SetTestStepName in the constructor as the controlling
	// framework uses the test step name immediately following construction to set
	// up the step's unique logging ID.
	SetTestStepName(KConnectDisconnectNotificationTest);
	}

TVerdict CLbsConnectDisconnectNotificationTest::doTestStepPreambleL()
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


TVerdict CLbsConnectDisconnectNotificationTest::doTestStepL()
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



TVerdict CLbsConnectDisconnectNotificationTest::doTestStepPostambleL()
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
CLbsConnectDisconnectNotificationTest::CAOGateway::CAOGateway(CLbsConnectDisconnectNotificationTest* aBase) :
	CAOGateway_Base(aBase)
	{
	iLbsNetSim = new(ELeave) RLbsNetSim();
	}
	
CLbsConnectDisconnectNotificationTest::CAOGateway::~CAOGateway()
	{
	delete iLbsNetSim;

	Cancel();
	}
	
void CLbsConnectDisconnectNotificationTest::CAOGateway::RunL()
	{
	switch (iBase->State())
		{
		case EStart:
			{
			// Nothing
			break;
			}
		case ETestConnected:
			{
			iLbsNetSim->ConnectL(this); // CAOTest::Connected => ETestGotConnection
			iBase->SetState(EGatewayConnected);
			break;
			}
		case EGatewayConnected:
			{
			break;
			}
		case ETestGotConnection:
			{
			iLbsNetSim->Close(); // CAOTest::Disconnected => EFinished
			iBase->SetState(EGatewayDisconnected);
			break;
			}
		case EGatewayDisconnected:
			{
			break;
			}
		case ETestGotDisconnection:
			{
			break;
			}
		case EFinished:
			{
			break;
			}
		} // switch
	}
	
void CLbsConnectDisconnectNotificationTest::CAOGateway::DoCancel()
	{
	}
	
TBool CLbsConnectDisconnectNotificationTest::CAOGateway::Finished()
	{
	return ETrue;
	}
	
//
// CAOTest

CLbsConnectDisconnectNotificationTest::CAOTest::CAOTest(CLbsConnectDisconnectNotificationTest* aBase, TInt aDelay, TBool aStepMode) :
	CAOTest_Base(aBase, aDelay, aStepMode)
	{
	iConnected = EFalse;
	iDisconnected = EFalse;
	
	iLbsNetSimTest = new(ELeave) RLbsNetSimTest();
	}
	
CLbsConnectDisconnectNotificationTest::CAOTest::~CAOTest()
	{
	Cancel();
	
	delete iLbsNetSimTest;
	}
	
void CLbsConnectDisconnectNotificationTest::CAOTest::RunL()
	{
	switch (iBase->State())
		{
		case EStart:
			{
			iLbsNetSimTest->ConnectL(this);
			if (iDelay > 0)
				{
				iLbsNetSimTest->SetResponseTime(iDelay, ETrue);
				}
			iLbsNetSimTest->SetAssistanceDataProvider(iBase->Provider());
			iLbsNetSimTest->SetStepMode(iStepMode);
			iBase->SetState(ETestConnected);
			iBase->ActivateGateway();
			break;
			}
		case ETestConnected:
			{
			// Nothing
			break;
			}
		case EGatewayConnected:
			{
			// Nothing
			break;
			}
		case ETestGotConnection:
			{
			// Nothing
			break;
			}
		case EGatewayDisconnected:
			{
			// Nothing
			break;
			}
		case ETestGotDisconnection:
			{
			// Nothing
			break;
			}
		case EFinished:
			{
			// Finish
			iLbsNetSimTest->Close();
			
			// Test complete
			CActiveScheduler::Stop();
			break;
			}
		} // switch
		
	}
	
void CLbsConnectDisconnectNotificationTest::CAOTest::DoCancel()
	{
	}
	
TBool CLbsConnectDisconnectNotificationTest::CAOTest::Finished()
	{
	return iConnected && iDisconnected;
	}

void CLbsConnectDisconnectNotificationTest::CAOTest::Connected()
	{
	iConnected = ETrue;
	iBase->SetState(ETestGotConnection);
	
	iBase->ActivateGateway();
	}
void CLbsConnectDisconnectNotificationTest::CAOTest::Disconnected()
	{
	iDisconnected = ETrue;
	iBase->SetState(EFinished);
	
	Activate();
	}
