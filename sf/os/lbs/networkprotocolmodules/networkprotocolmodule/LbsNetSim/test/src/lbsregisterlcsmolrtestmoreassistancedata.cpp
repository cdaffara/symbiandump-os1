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
#include "lbsregisterlcsmolrtestmoreassistancedata.h"
#include "Te_lbsnetsimtestSuiteDefs.h"


_LIT(KDestination, "");


CLbsRegisterLcsMoLrTestMoreAssistanceData::~CLbsRegisterLcsMoLrTestMoreAssistanceData()
/**
 * Destructor
 */
	{
	}

CLbsRegisterLcsMoLrTestMoreAssistanceData::CLbsRegisterLcsMoLrTestMoreAssistanceData()
/**
 * Constructor
 */
	{
	// **MUST** call SetTestStepName in the constructor as the controlling
	// framework uses the test step name immediately following construction to set
	// up the step's unique logging ID.
	SetTestStepName(KRegisterLcsMoLrTestMoreAssistanceData);
	}

TVerdict CLbsRegisterLcsMoLrTestMoreAssistanceData::doTestStepPreambleL()
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


TVerdict CLbsRegisterLcsMoLrTestMoreAssistanceData::doTestStepL()
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



TVerdict CLbsRegisterLcsMoLrTestMoreAssistanceData::doTestStepPostambleL()
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
CLbsRegisterLcsMoLrTestMoreAssistanceData::CAOGateway::CAOGateway(CLbsRegisterLcsMoLrTestMoreAssistanceData* aBase) :
	CAOGateway_Base(aBase),
	iDestination(KDestination), iFacilityLcsMoLrResult(EFalse), iMeasurementControlLocation(0)
	{
	iLbsNetSim = new(ELeave) RLbsNetSim();
	iLbsNetSim->ConnectL(this);
	}
	
CLbsRegisterLcsMoLrTestMoreAssistanceData::CAOGateway::~CAOGateway()
	{
	Cancel();

	iLbsNetSim->Close();
	delete iLbsNetSim;
	}
	
void CLbsRegisterLcsMoLrTestMoreAssistanceData::CAOGateway::RunL()
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
		case EGatewayRequestMoreAssistanceData:
			{
			iLbsNetSim->MeasurementReportLocationRequestMoreAssistanceDataL(EAssistanceDataReferenceTime);
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
	
void CLbsRegisterLcsMoLrTestMoreAssistanceData::CAOGateway::DoCancel()
	{
	}
	
TBool CLbsRegisterLcsMoLrTestMoreAssistanceData::CAOGateway::Finished()
	{
	return (iFacilityLcsMoLrResult && (iMeasurementControlLocation == 2));
	}
	
void CLbsRegisterLcsMoLrTestMoreAssistanceData::CAOGateway::ProcessFacilityLcsMoLrResult(TInt, const TPositionInfoBase*)
	{
	iFacilityLcsMoLrResult = ETrue;
	iBase->SetState(EGatewayReleaseLcsMoLr);
	Activate();
	}
void CLbsRegisterLcsMoLrTestMoreAssistanceData::CAOGateway::ProcessMeasurementControlLocation(const TPositionInfoBase& aPosition, const RLbsAssistanceDataBuilderSet& aData, const TLbsNetPosRequestQuality& aQuality)
	{
	(void)aPosition;
	(void)aData;
	(void)aQuality;
	if (++iMeasurementControlLocation == 1)
		{
		iBase->SetState(EGatewayRequestMoreAssistanceData);
		}
	else
		{
		iBase->SetState(EGatewayMeasurementReportLocation);
		}
	Activate();
	}


//
// CAOTest

CLbsRegisterLcsMoLrTestMoreAssistanceData::CAOTest::CAOTest(CLbsRegisterLcsMoLrTestMoreAssistanceData* aBase, TInt aDelay, TBool aStepMode) :
	CAOTest_Base(aBase, aDelay, aStepMode),
	iNotificationRegisterLcsMoLr(EFalse), iNotificationMeasurementReportLocation(EFalse),
	iNotificationReleaseLcsMoLr(EFalse), iNotificationFacilityLcsMoLrResult(EFalse),
	iNotificationMeasurementControlLocation(EFalse), iNotificationMeasurementReportRequestMoreAssistanceData(EFalse)
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
	
CLbsRegisterLcsMoLrTestMoreAssistanceData::CAOTest::~CAOTest()
	{
	Cancel();

	iLbsNetSimTest->Close();
	delete iLbsNetSimTest;
	}
	
void CLbsRegisterLcsMoLrTestMoreAssistanceData::CAOTest::RunL()
	{
	switch (iBase->State())
		{
		} // switch
		
	}
	
void CLbsRegisterLcsMoLrTestMoreAssistanceData::CAOTest::DoCancel()
	{
	}
	
TBool CLbsRegisterLcsMoLrTestMoreAssistanceData::CAOTest::Finished()
	{
	return (iNotificationRegisterLcsMoLr && 
			iNotificationMeasurementReportLocation && 
			iNotificationFacilityLcsMoLrResult && 
			iNotificationReleaseLcsMoLr && 
			iNotificationMeasurementReportRequestMoreAssistanceData && 
			(iNotificationMeasurementControlLocation == 2));
	}
	
void CLbsRegisterLcsMoLrTestMoreAssistanceData::CAOTest::NotifyRegisterLcsMoLr(const TDesC& aData)
	{
	(void)aData;
	iNotificationRegisterLcsMoLr = ETrue;
	if (iStepMode)
		{
		iLbsNetSimTest->Next(KErrNone);
		}
	}
void CLbsRegisterLcsMoLrTestMoreAssistanceData::CAOTest::NotifyReleaseLcsMoLr(TInt)
	{
	iNotificationReleaseLcsMoLr = ETrue;
	
	iBase->SetState(EFinished);
	iBase->ActivateGateway();
	if (iStepMode)
		{
		iLbsNetSimTest->Next(KErrNone);
		}
	}
void CLbsRegisterLcsMoLrTestMoreAssistanceData::CAOTest::NotifyMeasurementReportLocation(const TPositionInfo&)
	{
	iNotificationMeasurementReportLocation = ETrue;
	if (iStepMode)
		{
		iLbsNetSimTest->Next(KErrNone);
		}
	}
void CLbsRegisterLcsMoLrTestMoreAssistanceData::CAOTest::NotifyMeasurementReportRequestMoreAssistanceData(const TLbsAssistanceDataGroup&)
	{
	iNotificationMeasurementReportRequestMoreAssistanceData = ETrue;
	if (iStepMode)
		{
		iLbsNetSimTest->Next(KErrNone);
		}
	}
void CLbsRegisterLcsMoLrTestMoreAssistanceData::CAOTest::NotifyMeasurementControlLocation(const TPositionInfo& aPosition, const RLbsAssistanceDataBuilderSet& aData, const TLbsNetPosRequestQuality& aQuality)
	{
	(void)aPosition;
	(void)aData;
	(void)aQuality;
	++iNotificationMeasurementControlLocation;
	if (iStepMode)
		{
		iLbsNetSimTest->Next(KErrNone);
		}
	}
void CLbsRegisterLcsMoLrTestMoreAssistanceData::CAOTest::NotifyFacilityLcsMoLrResult(TInt, const TPositionInfo&)
	{
	iNotificationFacilityLcsMoLrResult = ETrue;
	if (iStepMode)
		{
		iLbsNetSimTest->Next(KErrNone);
		}
	}
