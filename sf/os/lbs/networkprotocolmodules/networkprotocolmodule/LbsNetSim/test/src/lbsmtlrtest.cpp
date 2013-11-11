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
#include "lbsmtlrtest.h"
#include "Te_lbsnetsimtestSuiteDefs.h"

CLbsMtLrTest::~CLbsMtLrTest()
/**
 * Destructor
 */
	{
	}

CLbsMtLrTest::CLbsMtLrTest()
/**
 * Constructor
 */
	{
	// **MUST** call SetTestStepName in the constructor as the controlling
	// framework uses the test step name immediately following construction to set
	// up the step's unique logging ID.
	SetTestStepName(KMtLrTest);
	}

TVerdict CLbsMtLrTest::doTestStepPreambleL()
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
	iState = EStart;
	
	SetTestStepResult(EPass);
	return TestStepResult();
	}


TVerdict CLbsMtLrTest::doTestStepL()
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



TVerdict CLbsMtLrTest::doTestStepPostambleL()
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
CLbsMtLrTest::CAOGateway::CAOGateway(CLbsMtLrTest* aBase) :
	CAOGateway_Base(aBase), 
	iProcessRegisterLcsLocationNotification(EFalse), iProcessMeasurementControlLocation(EFalse)
	{
	iLbsNetSim = new(ELeave) RLbsNetSim();
	iLbsNetSim->ConnectL(this);
	}
	
CLbsMtLrTest::CAOGateway::~CAOGateway()
	{
	Cancel();	
	
	iLbsNetSim->Close();
	delete iLbsNetSim;
	}
	
void CLbsMtLrTest::CAOGateway::RunL()
	{
	switch (iBase->State())
		{
		case EError:
			{
			ASSERT(EFalse);
			}
		case EGatewayReleaseLcsLocationNotification:
			{
			CLbsNetworkProtocolBase::TLbsPrivacyResponse response;
			iLbsNetSim->ReleaseLcsLocationNotificationL(response);
			break;
			}
		case EGatewayMeasurementReportLocation:
			{
			TPositionInfo position;
			iLbsNetSim->MeasurementReportLocationL(position);
			break;
			}
		case EFinished:
			{
			CActiveScheduler::Stop();
			break;
			}
			
		} // switch
	}
	
void CLbsMtLrTest::CAOGateway::DoCancel()
	{
	}
	
TBool CLbsMtLrTest::CAOGateway::Finished()
	{
	return (iProcessRegisterLcsLocationNotification &&
			iProcessMeasurementControlLocation);
	}
	
void CLbsMtLrTest::CAOGateway::ProcessRegisterLcsLocationNotification(const TLbsExternalRequestInfo&, const TLbsNetPosRequestPrivacy&)
	{
	iProcessRegisterLcsLocationNotification = ETrue;
	iBase->SetState(EGatewayReleaseLcsLocationNotification);
	Activate();
	}
void CLbsMtLrTest::CAOGateway::ProcessMeasurementControlLocation(const TPositionInfoBase& aPosition, const RLbsAssistanceDataBuilderSet& aData, const TLbsNetPosRequestQuality& aQuality)
	{
	(void)aPosition;
	(void)aData;
	(void)aQuality;
	iProcessMeasurementControlLocation = ETrue;
	iBase->SetState(EGatewayMeasurementReportLocation);
	Activate();
	}


//
// CAOTest

CLbsMtLrTest::CAOTest::CAOTest(CLbsMtLrTest* aBase, TInt aDelay, TBool aStepMode) :
	CAOTest_Base(aBase, aDelay, aStepMode),
	iNotifiyReleaseLcsLocationNotification(EFalse),
	iNotifiyMeasurementControlLocation(EFalse), iNotifiyMeasurementReportLocation(EFalse)
	{
	iLbsNetSimTest = new(ELeave) RLbsNetSimTest();
	iLbsNetSimTest->ConnectL(this);
	if (aDelay > 0)
		{
		iLbsNetSimTest->SetResponseTime(aDelay, ETrue);
		}
	iLbsNetSimTest->SetAssistanceDataProvider(aBase->Provider());
	}
	
CLbsMtLrTest::CAOTest::~CAOTest()
	{
	Cancel();	

	iLbsNetSimTest->Close();
	delete iLbsNetSimTest;
	}
	
void CLbsMtLrTest::CAOTest::RunL()
	{
	switch (iBase->State())
		{
		case EError:
			{
			ASSERT(EFalse);
			break;
			}
		case EStart:
			{
			iBase->SetState(ETestStartNetworkPrivacyRequest);
			Activate();
			break;
			}
		case ETestStartNetworkPrivacyRequest:
			{
			TLbsNetPosRequestPrivacy aType;
			TLbsExternalRequestInfo aRequest;

			iLbsNetSimTest->StartNetworkPrivacyRequest(aType, aRequest);
			iBase->SetState(EError);
			break;
			}
		case ETestNotifyReleaseLcsLocationNotification:
			{
			iBase->SetState(ETestStartNetworkLocationRequest);
			Activate();
			break;
			}
		case ETestStartNetworkLocationRequest:
			{
			iLbsNetSimTest->StartNetworkLocationRequest();
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
	
void CLbsMtLrTest::CAOTest::DoCancel()
	{
	}
	
TBool CLbsMtLrTest::CAOTest::Finished()
	{
	return (iNotifiyReleaseLcsLocationNotification && 
			iNotifiyMeasurementReportLocation &&
			iNotifiyMeasurementControlLocation);
	}
	
void CLbsMtLrTest::CAOTest::NotifyReleaseLcsLocationNotification(const CLbsNetworkProtocolBase::TLbsPrivacyResponse&)
	{
	iNotifiyReleaseLcsLocationNotification = ETrue;
	iBase->SetState(ETestStartNetworkLocationRequest);
	Activate();
	if (iStepMode)
		{
		iLbsNetSimTest->Next(KErrNone);
		}
	}
void CLbsMtLrTest::CAOTest::NotifyMeasurementReportLocation(const TPositionInfo&)
	{
	iNotifiyMeasurementReportLocation = ETrue;
	iBase->SetState(EFinished);
	Activate();
	if (iStepMode)
		{
		iLbsNetSimTest->Next(KErrNone);
		}
	}
void CLbsMtLrTest::CAOTest::NotifyMeasurementControlLocation(const TPositionInfo& aPosition, const RLbsAssistanceDataBuilderSet& aData, const TLbsNetPosRequestQuality& aQuality)
	{
	(void)aPosition;
	(void)aData;
	(void)aQuality;
	
	iNotifiyMeasurementControlLocation = ETrue;
	if (iStepMode)
		{
		iLbsNetSimTest->Next(KErrNone);
		}
	}
