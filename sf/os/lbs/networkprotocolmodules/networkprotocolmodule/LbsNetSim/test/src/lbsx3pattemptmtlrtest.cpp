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
 @file Lbsx3pTest.cpp
*/
#include "lbsx3pattemptmtlrtest.h"
#include "Te_lbsnetsimtestSuiteDefs.h"


_LIT(KX3PDestination, "0771999999");


CLbsX3PAttemptMrLrTest::~CLbsX3PAttemptMrLrTest()
/**
 * Destructor
 */
	{
	}

CLbsX3PAttemptMrLrTest::CLbsX3PAttemptMrLrTest()
/**
 * Constructor
 */
	{
	// **MUST** call SetTestStepName in the constructor as the controlling
	// framework uses the test step name immediately following construction to set
	// up the step's unique logging ID.
	SetTestStepName(KX3PTestAttemptMtLrTest);
	}

TVerdict CLbsX3PAttemptMrLrTest::doTestStepPreambleL()
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


TVerdict CLbsX3PAttemptMrLrTest::doTestStepL()
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



TVerdict CLbsX3PAttemptMrLrTest::doTestStepPostambleL()
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
CLbsX3PAttemptMrLrTest::CAOGateway::CAOGateway(CLbsX3PAttemptMrLrTest* aBase) :
	CAOGateway_Base(aBase),
	iDestination(KX3PDestination), iMeasurementControlLocation(EFalse)
	{
	iLbsNetSim = new(ELeave) RLbsNetSim();
	iLbsNetSim->ConnectL(this);
	}
	
CLbsX3PAttemptMrLrTest::CAOGateway::~CAOGateway()
	{
	Cancel();	
	
	iLbsNetSim->Close();
	delete iLbsNetSim;
	}
	
void CLbsX3PAttemptMrLrTest::CAOGateway::RunL()
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
		case EGatewayMeasurementReportLocation:
			{
			TPositionInfo position;
			iLbsNetSim->MeasurementReportLocationL(position);
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
	
void CLbsX3PAttemptMrLrTest::CAOGateway::DoCancel()
	{
	}
	
TBool CLbsX3PAttemptMrLrTest::CAOGateway::Finished()
	{
	return (iFacilityLcsMoLrResult && iMeasurementControlLocation);
	}
	
void CLbsX3PAttemptMrLrTest::CAOGateway::ProcessMeasurementControlLocation(const TPositionInfoBase& aPosition, const RLbsAssistanceDataBuilderSet& aData, const TLbsNetPosRequestQuality& aQuality)
	{
	(void)aPosition;
	(void)aData;
	(void)aQuality;
	iMeasurementControlLocation = ETrue;
	
	iBase->SetState(EGatewayMeasurementReportLocation);
	Activate();
	}

void CLbsX3PAttemptMrLrTest::CAOGateway::ProcessFacilityLcsMoLrResult(TInt, const TPositionInfoBase*)
	{
	iFacilityLcsMoLrResult = ETrue;
	iBase->SetState(EGatewayReleaseLcsMoLr);
	Activate();
	}
	
//
// CAOTest

CLbsX3PAttemptMrLrTest::CAOTest::CAOTest(CLbsX3PAttemptMrLrTest* aBase, TInt aDelay, TBool aStepMode) :
	CAOTest_Base(aBase, aDelay, aStepMode),
	iNotificationRegisterLcsMoLr(EFalse), iNotificationMeasurementReportLocation(EFalse),
	iNotificationFacilityLcsMoLrResult(EFalse), iNotificationReleaseLcsMoLr(EFalse), 
	iNotificationMeasurementControlLocation(EFalse), iNotificationError(EFalse)
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
	
CLbsX3PAttemptMrLrTest::CAOTest::~CAOTest()
	{
	Cancel();	

	iLbsNetSimTest->Close();
	delete iLbsNetSimTest;
	}
	
void CLbsX3PAttemptMrLrTest::CAOTest::RunL()
	{
	switch (iBase->State())
		{
		} // switch
		
	}
	
void CLbsX3PAttemptMrLrTest::CAOTest::DoCancel()
	{
	}
	
TBool CLbsX3PAttemptMrLrTest::CAOTest::Finished()
	{
	return (iNotificationRegisterLcsMoLr && 
			iNotificationMeasurementReportLocation && 
			iNotificationReleaseLcsMoLr && 
			iNotificationFacilityLcsMoLrResult && 
			iNotificationMeasurementControlLocation &&
			iNotificationError);
	}
	
void CLbsX3PAttemptMrLrTest::CAOTest::NotifyRegisterLcsMoLr(const TDesC& aData)
	{
	(void)aData;
	iNotificationRegisterLcsMoLr = ETrue;
	
	// Attemp to start an Mt-Lr
	iLbsNetSimTest->StartNetworkLocationRequest();
	if (iStepMode)
		{
		iLbsNetSimTest->Next(KErrNone);
		}
	}
void CLbsX3PAttemptMrLrTest::CAOTest::NotifyReleaseLcsMoLr(TInt)
	{
	iNotificationReleaseLcsMoLr = ETrue;
	
	iBase->SetState(EFinished);
	iBase->ActivateGateway();
	if (iStepMode)
		{
		iLbsNetSimTest->Next(KErrNone);
		}
	}
void CLbsX3PAttemptMrLrTest::CAOTest::NotifyMeasurementReportLocation(const TPositionInfo&)
	{
	iNotificationMeasurementReportLocation = ETrue;
	
//	iBase->SetState(EGatewayReleaseLcsMoLr);
//	iBase->ActivateGateway();
	if (iStepMode)
		{
		iLbsNetSimTest->Next(KErrNone);
		}
	}
void CLbsX3PAttemptMrLrTest::CAOTest::NotifyMeasurementControlLocation(const TPositionInfo& aPosition, const RLbsAssistanceDataBuilderSet& aData, const TLbsNetPosRequestQuality& aQuality)
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
void CLbsX3PAttemptMrLrTest::CAOTest::NotifyError(MLbsNetSimTestObserver::EFunction /*aFunction*/, TInt /*aError*/)
	{
	iNotificationError = ETrue;
	if (iStepMode)
		{
		iLbsNetSimTest->Next(KErrNone);
		}
	}
	
void CLbsX3PAttemptMrLrTest::CAOTest::NotifyFacilityLcsMoLrResult(TInt, const TPositionInfo&)
	{
	iNotificationFacilityLcsMoLrResult = ETrue;

	if (iStepMode)
		{
		iLbsNetSimTest->Next(KErrNone);
		}
	}	
