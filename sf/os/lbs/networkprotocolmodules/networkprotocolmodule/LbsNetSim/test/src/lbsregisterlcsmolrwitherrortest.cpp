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
#include "lbsregisterlcsmolrwitherrortest.h"
#include "Te_lbsnetsimtestSuiteDefs.h"


_LIT(KDestination, "");


CLbsRegisterLcsMoLrWithErrorTest::~CLbsRegisterLcsMoLrWithErrorTest()
/**
 * Destructor
 */
	{
	}

CLbsRegisterLcsMoLrWithErrorTest::CLbsRegisterLcsMoLrWithErrorTest()
/**
 * Constructor
 */
	{
	SetTestStepName(KRegisterLcsMoLrWithErrorTest);
	}

TVerdict CLbsRegisterLcsMoLrWithErrorTest::doTestStepPreambleL()
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
	iState = EGatewayRegisterLcsMoLr;
	
	SetTestStepResult(EPass);
	return TestStepResult();
	}


TVerdict CLbsRegisterLcsMoLrWithErrorTest::doTestStepL()
/**
 * @return - TVerdict code
 * Override of base class pure virtual
 * Our implementation only gets called if the base class doTestStepPreambleL() did
 * not leave. That being the case, the current test result value will be EPass.
 */
	{
	
	if (TestStepResult()==EPass)
		{
		iGateway->Activate();
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



TVerdict CLbsRegisterLcsMoLrWithErrorTest::doTestStepPostambleL()
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
CLbsRegisterLcsMoLrWithErrorTest::CAOGateway::CAOGateway(CLbsRegisterLcsMoLrWithErrorTest* aBase) :
	CAOGateway_Base(aBase), iDestination(KDestination), iErrors(0)
	{
	iLbsNetSim = new(ELeave) RLbsNetSim();
	iLbsNetSim->ConnectL(this);
	}
	
CLbsRegisterLcsMoLrWithErrorTest::CAOGateway::~CAOGateway()
	{
	Cancel();	

	iLbsNetSim->Close();
	delete iLbsNetSim;
	}
	
void CLbsRegisterLcsMoLrWithErrorTest::CAOGateway::RunL()
	{
	switch (iBase->State())
		{
		case EError:
			{
			ASSERT(EFalse);
			}
		case EGatewayRegisterLcsMoLr:
			{
			iLbsNetSim->RegisterLcsMoLrL(iDestination);
			iBase->SetState(EError);
			break;
			}
		case EFinished:
			{
			CActiveScheduler::Stop();
			break;
			}
			
		} // switch
	}
	
void CLbsRegisterLcsMoLrWithErrorTest::CAOGateway::DoCancel()
	{
	}
	
TBool CLbsRegisterLcsMoLrWithErrorTest::CAOGateway::Finished()
	{
	return (iErrors == 1);
	}
	
void CLbsRegisterLcsMoLrWithErrorTest::CAOGateway::ProcessError(TInt)
	{
	++iErrors;

	iBase->SetState(EFinished);

	Activate();
	}
	



//
// CAOTest

CLbsRegisterLcsMoLrWithErrorTest::CAOTest::CAOTest(CLbsRegisterLcsMoLrWithErrorTest* aBase, TInt aDelay, TBool aStepMode) :
	CAOTest_Base(aBase, aDelay, aStepMode)
	{
	iLbsNetSimTest = new(ELeave) RLbsNetSimTest();
	iLbsNetSimTest->ConnectL(this);
	iLbsNetSimTest->SetResponseError(RLbsNetSimTest::KNetSimNetworkNotAvailable, ETrue);
	iLbsNetSimTest->SetAssistanceDataProvider(aBase->Provider());
	iLbsNetSimTest->SetStepMode(iStepMode);
	}
	
CLbsRegisterLcsMoLrWithErrorTest::CAOTest::~CAOTest()
	{
	Cancel();	
	
	iLbsNetSimTest->Close();
	delete iLbsNetSimTest;
	}
	
void CLbsRegisterLcsMoLrWithErrorTest::CAOTest::RunL()
	{
	switch (iBase->State())
		{
		} // switch
		
	}
	
void CLbsRegisterLcsMoLrWithErrorTest::CAOTest::DoCancel()
	{
	}
	
TBool CLbsRegisterLcsMoLrWithErrorTest::CAOTest::Finished()
	{
	return ETrue;
	}
