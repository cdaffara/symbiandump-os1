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
#include "lbsx3ptest.h"
#include "Te_lbsnetsimtestSuiteDefs.h"


_LIT(KX3PDestination, "0771999999");


CLbsX3PTest::~CLbsX3PTest()
/**
 * Destructor
 */
	{
	}

CLbsX3PTest::CLbsX3PTest()
/**
 * Constructor
 */
	{
	// **MUST** call SetTestStepName in the constructor as the controlling
	// framework uses the test step name immediately following construction to set
	// up the step's unique logging ID.
	SetTestStepName(KX3PTest);
	}

TVerdict CLbsX3PTest::doTestStepPreambleL()
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


TVerdict CLbsX3PTest::doTestStepL()
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



TVerdict CLbsX3PTest::doTestStepPostambleL()
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
CLbsX3PTest::CAOGateway::CAOGateway(CLbsX3PTest* aBase) :
	CAOGateway_Base(aBase), iDestination(KX3PDestination), iMeasurementControlLocation(EFalse),iFacilityLcsMoLrResult(EFalse)
	{
	iLbsNetSim = new(ELeave) RLbsNetSim();
	iLbsNetSim->ConnectL(this);
	}
	
CLbsX3PTest::CAOGateway::~CAOGateway()
	{
	Cancel();	
	
	iLbsNetSim->Close();
	delete iLbsNetSim;
	}
	
void CLbsX3PTest::CAOGateway::RunL()
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
	
void CLbsX3PTest::CAOGateway::DoCancel()
	{
	}
	
TBool CLbsX3PTest::CAOGateway::Finished()
	{
	return (iFacilityLcsMoLrResult && iMeasurementControlLocation);
	}
	
void CLbsX3PTest::CAOGateway::ProcessMeasurementControlLocation(const TPositionInfoBase& aPosition, const RLbsAssistanceDataBuilderSet& aData, const TLbsNetPosRequestQuality& aQuality)
	{
	(void)aPosition;
	(void)aData;
	(void)aQuality;
	iMeasurementControlLocation = ETrue;
	
	iBase->SetState(EGatewayMeasurementReportLocation);
	Activate();
	}

void CLbsX3PTest::CAOGateway::ProcessFacilityLcsMoLrResult(TInt, const TPositionInfoBase*)
	{
	iFacilityLcsMoLrResult = ETrue;
	iBase->SetState(EGatewayReleaseLcsMoLr);
	Activate();
	}

//
// CAOTest

CLbsX3PTest::CAOTest::CAOTest(CLbsX3PTest* aBase, TInt aDelay, TBool aStepMode) :
	CAOTest_Base(aBase, aDelay, aStepMode), 
	iNotificationRegisterLcsMoLr(EFalse), iNotificationMeasurementReportLocation(EFalse),
	iNotificationFacilityLcsMoLrResult(EFalse), iNotificationReleaseLcsMoLr(EFalse),  
	iNotificationMeasurementControlLocation(EFalse)
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
	
CLbsX3PTest::CAOTest::~CAOTest()
	{
	Cancel();	

	iLbsNetSimTest->Close();
	delete iLbsNetSimTest;
	}
	
void CLbsX3PTest::CAOTest::RunL()
	{
	switch (iBase->State())
		{
		} // switch
		
	}
	
void CLbsX3PTest::CAOTest::DoCancel()
	{
	}
	
TBool CLbsX3PTest::CAOTest::Finished()
	{
	return (iNotificationRegisterLcsMoLr && 
			iNotificationMeasurementReportLocation && 
			iNotificationFacilityLcsMoLrResult && 
			iNotificationReleaseLcsMoLr && 
			iNotificationMeasurementControlLocation);
	}
	
void CLbsX3PTest::CAOTest::NotifyRegisterLcsMoLr(const TDesC& aData)
	{
	(void)aData;
	iNotificationRegisterLcsMoLr = ETrue;
	if (iStepMode)
		{
		iLbsNetSimTest->Next(KErrNone);
		}
	}
void CLbsX3PTest::CAOTest::NotifyReleaseLcsMoLr(TInt)
	{
	iNotificationReleaseLcsMoLr = ETrue;
	
	iBase->SetState(EFinished);
	iBase->ActivateGateway();
	if (iStepMode)
		{
		iLbsNetSimTest->Next(KErrNone);
		}
	}
void CLbsX3PTest::CAOTest::NotifyMeasurementReportLocation(const TPositionInfo&)
	{
	iNotificationMeasurementReportLocation = ETrue;
	
	//iBase->SetState(EGatewayReleaseLcsMoLr);
	//iBase->ActivateGateway();
	if (iStepMode)
		{
		iLbsNetSimTest->Next(KErrNone);
		}
	}
void CLbsX3PTest::CAOTest::NotifyMeasurementControlLocation(const TPositionInfo& aPosition, const RLbsAssistanceDataBuilderSet& aData, const TLbsNetPosRequestQuality& aQuality)
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
void CLbsX3PTest::CAOTest::NotifyFacilityLcsMoLrResult(TInt, const TPositionInfo&)
	{
	iNotificationFacilityLcsMoLrResult = ETrue;

	if (iStepMode)
		{
		iLbsNetSimTest->Next(KErrNone);
		}
	}	
