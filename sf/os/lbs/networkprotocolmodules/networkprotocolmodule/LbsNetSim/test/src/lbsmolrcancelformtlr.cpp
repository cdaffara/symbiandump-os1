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
#include "lbsmolrcancelformtlr.h"
#include "Te_lbsnetsimtestSuiteDefs.h"


_LIT(KDestination, "");


CLbsMolrCancelForMtLr::~CLbsMolrCancelForMtLr()
/**
 * Destructor
 */
	{
	}

CLbsMolrCancelForMtLr::CLbsMolrCancelForMtLr()
/**
 * Constructor
 */
	{
	SetTestStepName(KMoLrCancelForMtLr);
	}

TVerdict CLbsMolrCancelForMtLr::doTestStepPreambleL()
/**
 * @return - TVerdict code
 * Override of base class virtual
 */
	{
	RHeap& heap = User::Heap();
	TInt totalsize = 0;
	TInt nocells = heap.AllocSize(totalsize);
	TInt size = heap.Size();
	TInt biggestblock = 0;
	TInt avail = heap.Available(biggestblock);
	
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


TVerdict CLbsMolrCancelForMtLr::doTestStepL()
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



TVerdict CLbsMolrCancelForMtLr::doTestStepPostambleL()
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
CLbsMolrCancelForMtLr::CAOGateway::CAOGateway(CLbsMolrCancelForMtLr* aBase) :
	CAOGateway_Base(aBase), iDestination(KDestination),
	iRegisterLcsLocationNotification(EFalse), iMeasurementControlLocation(EFalse),
	iMeasurementControlLocationError(EFalse)
	{
	iLbsNetSim = new(ELeave) RLbsNetSim();
	iLbsNetSim->ConnectL(this);
	}
	
CLbsMolrCancelForMtLr::CAOGateway::~CAOGateway()
	{
	Cancel();	

	iLbsNetSim->Close();
	delete iLbsNetSim;
	}
	
void CLbsMolrCancelForMtLr::CAOGateway::RunL()
	{
	switch (iBase->State())
		{
		case EStart:
			{
			iBase->SetState(EGWRegisterLcsMoLr);
			Activate();
			break;
			}
		case EGWRegisterLcsMoLr:
			{
			iLbsNetSim->RegisterLcsMoLrL(iDestination); // => Test: NotifyRegisterLcsMoLr
			iBase->SetState(EError);
			break;
			}
		case EGWReleaseLcsMoLr:
			{
			iLbsNetSim->ReleaseLcsMoLrL(KErrCancel); // Test: NotifyReleaseLcsMoLr
			iBase->SetState(EError);
			break;
			}
		case EGWReleaseLcsLocationNotification:
			{
			CLbsNetworkProtocolBase::TLbsPrivacyResponse response;
			iLbsNetSim->ReleaseLcsLocationNotificationL(response); // => Test: ReleaseLcsLocationNotification
			iBase->SetState(EError);
			break;			
			}
		case EGWMeasurementReportLocation:
			{
			TPositionInfo position;
			iLbsNetSim->MeasurementReportLocationL(position);
			iBase->SetState(EError);
			break;
			}
		case EFinished:
			{
			CActiveScheduler::Stop();
			break;
			}
		default:
			{
			ASSERT(EFalse);
			}
		} // switch
	}
	
void CLbsMolrCancelForMtLr::CAOGateway::DoCancel()
	{
	}
	
TBool CLbsMolrCancelForMtLr::CAOGateway::Finished()
	{
	return (iRegisterLcsLocationNotification && 
			iMeasurementControlLocation &&
			iMeasurementControlLocationError);
	}
	
void CLbsMolrCancelForMtLr::CAOGateway::ProcessRegisterLcsLocationNotification(const TLbsExternalRequestInfo&, const TLbsNetPosRequestPrivacy&)
	{
	iRegisterLcsLocationNotification = ETrue;
	
	iBase->SetState(EGWReleaseLcsLocationNotification);
	Activate();
	}
void CLbsMolrCancelForMtLr::CAOGateway::ProcessMeasurementControlLocation(const TPositionInfoBase&, const RLbsAssistanceDataBuilderSet&, const TLbsNetPosRequestQuality&)
	{
	iMeasurementControlLocation = ETrue;
	
	iBase->SetState(EGWMeasurementReportLocation);
	Activate();
	}
void CLbsMolrCancelForMtLr::CAOGateway::ProcessMeasurementControlLocationError(TInt)
	{
	iMeasurementControlLocationError = ETrue; // GW: RegisterLcsLocationNotification
	
	iBase->SetState(EGWReleaseLcsMoLr);
	Activate();
	
	}
	



//
// CAOTest

CLbsMolrCancelForMtLr::CAOTest::CAOTest(CLbsMolrCancelForMtLr* aBase, TInt aDelay, TBool aStepMode) :
	CAOTest_Base(aBase, aDelay, aStepMode),
	iNotificationRegisterLcsMoLr(EFalse), 
	iReleaseLcsLocationNotification(EFalse), iMeasurementReportLocation(EFalse),
	iNotificationReleaseLcsMoLr(EFalse), iNotifiyMeasurementControlLocation(EFalse)
	{
	iLbsNetSimTest = new(ELeave) RLbsNetSimTest();
	iLbsNetSimTest->ConnectL(this);
	if (aDelay > 0)
	{
	iLbsNetSimTest->SetResponseTime(aDelay, ETrue);
	}
	iLbsNetSimTest->SetAssistanceDataProvider(aBase->Provider());
	iLbsNetSimTest->SetStepMode(iStepMode);
	
	// Go into emergency mode
	iLbsNetSimTest->SetEmergenyStatus(ETrue);
	}
	
CLbsMolrCancelForMtLr::CAOTest::~CAOTest()
	{
	Cancel();	
	
	iLbsNetSimTest->Close();
	delete iLbsNetSimTest;
	}
	
void CLbsMolrCancelForMtLr::CAOTest::RunL()
	{
	switch (iBase->State())
		{
		case ETestStartNetworkPrivacyRequest:
			{
			TLbsNetPosRequestPrivacy aType;
			TLbsExternalRequestInfo aRequest;

			iLbsNetSimTest->StartNetworkPrivacyRequest(aType, aRequest); // GW: MeasurementControlLocationError
			iBase->SetState(EError);
			break;
			}
		case ETestStartNetworkLocationRequest:
			{
			iLbsNetSimTest->StartNetworkLocationRequest(); // GW: ProcessMeasurementControlLocation
			iBase->SetState(EError);
			break;
			}
		default:
			{
			ASSERT(EFalse);
			}
		} // switch
		
	}
	
void CLbsMolrCancelForMtLr::CAOTest::DoCancel()
	{
	}
	
TBool CLbsMolrCancelForMtLr::CAOTest::Finished()
	{
	return (iNotificationRegisterLcsMoLr &&
			iReleaseLcsLocationNotification &&
			iMeasurementReportLocation &&
			iNotificationReleaseLcsMoLr &&
			iNotifiyMeasurementControlLocation);
	}
	
void CLbsMolrCancelForMtLr::CAOTest::NotifyRegisterLcsMoLr(const TDesC&)
	{
	iNotificationRegisterLcsMoLr = ETrue;
	
	iBase->SetState(ETestStartNetworkPrivacyRequest);
	Activate();
	if (iStepMode)
		{
		iLbsNetSimTest->Next(KErrNone);
		}
	}
void CLbsMolrCancelForMtLr::CAOTest::NotifyReleaseLcsMoLr(TInt)
	{
	iNotificationReleaseLcsMoLr = ETrue;
	if (iStepMode)
		{
		iLbsNetSimTest->Next(KErrNone);
		}
	}
void CLbsMolrCancelForMtLr::CAOTest::NotifyReleaseLcsLocationNotification(const CLbsNetworkProtocolBase::TLbsPrivacyResponse&)
	{
	iReleaseLcsLocationNotification = ETrue;
	
	iBase->SetState(ETestStartNetworkLocationRequest);
	Activate();
	if (iStepMode)
		{
		iLbsNetSimTest->Next(KErrNone);
		}
	}
void CLbsMolrCancelForMtLr::CAOTest::NotifyMeasurementReportLocation(const TPositionInfo&)
	{
	iMeasurementReportLocation = ETrue;
	
	iBase->SetState(EFinished);
	iBase->ActivateGateway();
	if (iStepMode)
		{
		iLbsNetSimTest->Next(KErrNone);
		}
	}
void CLbsMolrCancelForMtLr::CAOTest::NotifyMeasurementControlLocation(const TPositionInfo&, const RLbsAssistanceDataBuilderSet&, const TLbsNetPosRequestQuality&)
	{
	iNotifiyMeasurementControlLocation = ETrue;
	if (iStepMode)
		{
		iLbsNetSimTest->Next(KErrNone);
		}
	}
