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
#include "lbscancelmolrtest.h"
#include "Te_lbsnetsimtestSuiteDefs.h"


_LIT(KDestination, "");

CLbsCancelMoLrTest::~CLbsCancelMoLrTest()
/**
 * Destructor
 */
	{
	}

CLbsCancelMoLrTest::CLbsCancelMoLrTest()
/**
 * Constructor
 */
	{
	// **MUST** call SetTestStepName in the constructor as the controlling
	// framework uses the test step name immediately following construction to set
	// up the step's unique logging ID.
	SetTestStepName(KCancelMoLrTest);
	}

TVerdict CLbsCancelMoLrTest::doTestStepPreambleL()
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


TVerdict CLbsCancelMoLrTest::doTestStepL()
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



TVerdict CLbsCancelMoLrTest::doTestStepPostambleL()
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
CLbsCancelMoLrTest::CAOGateway::CAOGateway(CLbsCancelMoLrTest* aBase) :
	CAOGateway_Base(aBase),
	iDestination(KDestination), iMeasurementControlLocation(EFalse)
	{
	iLbsNetSim = new(ELeave) RLbsNetSim();
	iLbsNetSim->ConnectL(this);
	}
	
CLbsCancelMoLrTest::CAOGateway::~CAOGateway()
	{
	Cancel();	

	iLbsNetSim->Close();
	delete iLbsNetSim;
	}
	
void CLbsCancelMoLrTest::CAOGateway::RunL()
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
		case EGatewayMeasurementControlFailure:
			{
			TPositionInfo position;
			iLbsNetSim->MeasurementControlFailureL(KErrAbort);
			iBase->SetState(EError);
			break;
			}
		case EGatewayReleaseLcsMoLr:
			{
			iLbsNetSim->ReleaseLcsMoLrL(KErrNone);
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
	
void CLbsCancelMoLrTest::CAOGateway::DoCancel()
	{
	}
	
TBool CLbsCancelMoLrTest::CAOGateway::Finished()
	{
	return (iMeasurementControlLocation);
	}
	
void CLbsCancelMoLrTest::CAOGateway::ProcessMeasurementControlLocation(const TPositionInfoBase& aPosition, const RLbsAssistanceDataBuilderSet& aData, const TLbsNetPosRequestQuality& aQuality)
	{
	(void)aPosition;
	(void)aData;
	(void)aQuality;
	iMeasurementControlLocation = ETrue;
	iBase->SetState(EGatewayMeasurementControlFailure);
	Activate();
	}


//
// CAOTest

CLbsCancelMoLrTest::CAOTest::CAOTest(CLbsCancelMoLrTest* aBase, TInt aDelay, TBool aStepMode) :
	CAOTest_Base(aBase, aDelay, aStepMode),
	iNotificationRegisterLcsMoLr(EFalse), 
	iNotificationReleaseLcsMoLr(EFalse), iNotificationMeasurementControlLocation(EFalse), 
	iNotificationMeasurementControlFailure(EFalse)
	{
	iLbsNetSimTest = new(ELeave) RLbsNetSimTest();
	iLbsNetSimTest->ConnectL(this);
	if (aDelay > 0)
		{
		iLbsNetSimTest->SetResponseTime(aDelay, ETrue);
		}
	iLbsNetSimTest->SetAssistanceDataProvider(aBase->Provider());
	iLbsNetSimTest->SetStepMode(iStepMode);
	}
	
CLbsCancelMoLrTest::CAOTest::~CAOTest()
	{
	Cancel();

	iLbsNetSimTest->Close();
	delete iLbsNetSimTest;
	}
	
void CLbsCancelMoLrTest::CAOTest::RunL()
	{
	switch (iBase->State())
		{
		} // switch
		
	}
	
void CLbsCancelMoLrTest::CAOTest::DoCancel()
	{
	}
	
TBool CLbsCancelMoLrTest::CAOTest::Finished()
	{
	return (iNotificationRegisterLcsMoLr && 
			iNotificationReleaseLcsMoLr && 
			iNotificationMeasurementControlLocation &&
			iNotificationMeasurementControlFailure);
	}
	
void CLbsCancelMoLrTest::CAOTest::NotifyRegisterLcsMoLr(const TDesC& aData)
	{
	(void)aData;
	iNotificationRegisterLcsMoLr = ETrue;
	if (iStepMode)
		{
		iLbsNetSimTest->Next(KErrNone);
		}
	}
void CLbsCancelMoLrTest::CAOTest::NotifyReleaseLcsMoLr(TInt)
	{
	iNotificationReleaseLcsMoLr = ETrue;
	
	iBase->SetState(EFinished);
	iBase->ActivateGateway();
	if (iStepMode)
		{
		iLbsNetSimTest->Next(KErrNone);
		}
	}
void CLbsCancelMoLrTest::CAOTest::NotifyMeasurementReportControlFailure(TInt)
	{
	iNotificationMeasurementControlFailure = ETrue;
	
	iBase->SetState(EGatewayReleaseLcsMoLr);
	iBase->ActivateGateway();
	if (iStepMode)
		{
		iLbsNetSimTest->Next(KErrNone);
		}
	}
void CLbsCancelMoLrTest::CAOTest::NotifyMeasurementControlLocation(const TPositionInfo& aPosition, const RLbsAssistanceDataBuilderSet& aData, const TLbsNetPosRequestQuality& aQuality)
	{
	(void)aPosition;
	(void)aData;
	(void)aQuality;
	iNotificationMeasurementControlLocation = ETrue;
	if (iStepMode)
		{
		iLbsNetSimTest->Next(KErrNone);
		}
	}
