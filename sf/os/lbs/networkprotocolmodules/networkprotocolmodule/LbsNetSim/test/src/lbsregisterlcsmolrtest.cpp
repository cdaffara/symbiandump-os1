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
#include "lbsregisterlcsmolrtest.h"
#include "Te_lbsnetsimtestSuiteDefs.h"


_LIT(KDestination, "");


CLbsRegisterLcsMoLrTest::~CLbsRegisterLcsMoLrTest()
/**
 * Destructor
 */
	{
	}

CLbsRegisterLcsMoLrTest::CLbsRegisterLcsMoLrTest()
/**
 * Constructor
 */
	{
	SetTestStepName(KRegisterLcsMoLrTest);
	}

TVerdict CLbsRegisterLcsMoLrTest::doTestStepPreambleL()
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
	GetBoolFromConfig(ConfigSection(), KStepMode, iStepMode);
	
	
	iState = EStart;
	iTest = new (ELeave) CAOTest(this, iDelay, iStepMode);  // Stopped in postamble
	iGateway = new (ELeave) CAOGateway(this); // Stopped during test
	iState = EGatewayRegisterLcsMoLr;
	
	SetTestStepResult(EPass);
	return TestStepResult();
	}


TVerdict CLbsRegisterLcsMoLrTest::doTestStepL()
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



TVerdict CLbsRegisterLcsMoLrTest::doTestStepPostambleL()
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
CLbsRegisterLcsMoLrTest::CAOGateway::CAOGateway(CLbsRegisterLcsMoLrTest* aBase) :
	CAOGateway_Base(aBase),
	iDestination(KDestination), iFacilityLcsMoLrResult(EFalse), iMeasurementControlLocation(EFalse)
	{
	iLbsNetSim = new(ELeave) RLbsNetSim();
	iLbsNetSim->ConnectL(this);
	}
	
CLbsRegisterLcsMoLrTest::CAOGateway::~CAOGateway()
	{
	Cancel();	

	iLbsNetSim->Close();
	delete iLbsNetSim;
	}
	
void CLbsRegisterLcsMoLrTest::CAOGateway::RunL()
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
	
void CLbsRegisterLcsMoLrTest::CAOGateway::DoCancel()
	{
	}
	
TBool CLbsRegisterLcsMoLrTest::CAOGateway::Finished()
	{
	return (iFacilityLcsMoLrResult && iMeasurementControlLocation);
	}
	
void CLbsRegisterLcsMoLrTest::CAOGateway::ProcessFacilityLcsMoLrResult(TInt, const TPositionInfoBase*)
	{
	iFacilityLcsMoLrResult = ETrue;
	iBase->SetState(EGatewayReleaseLcsMoLr);
	Activate();
	}
void CLbsRegisterLcsMoLrTest::CAOGateway::ProcessMeasurementControlLocation(const TPositionInfoBase& aPosition, const RLbsAssistanceDataBuilderSet& aData, const TLbsNetPosRequestQuality& aQuality)
	{
	(void)aPosition;
	(void)aData;
	(void)aQuality;
	iMeasurementControlLocation = ETrue;
	iBase->SetState(EGatewayMeasurementReportLocation);
	Activate();
	}


//
// CAOTest

CLbsRegisterLcsMoLrTest::CAOTest::CAOTest(CLbsRegisterLcsMoLrTest* aBase, TInt aDelay, TBool aStepMode) :
	CAOTest_Base(aBase, aDelay, aStepMode),
	iNotificationRegisterLcsMoLr(EFalse), iNotificationMeasurementReportLocation(EFalse),
	iNotificationReleaseLcsMoLr(EFalse), iNotificationFacilityLcsMoLrResult(EFalse),
	iNotificationMeasurementControlLocation(EFalse)
	{
	iLbsNetSimTest = new(ELeave) RLbsNetSimTest();
	iLbsNetSimTest->ConnectL(this);
	if (aDelay > 0)
		{
		iLbsNetSimTest->SetResponseTime(aDelay, ETrue);
		}
	iLbsNetSimTest->SetStepMode(iStepMode);
	iLbsNetSimTest->SetAssistanceDataProvider(aBase->Provider());
	}
	
CLbsRegisterLcsMoLrTest::CAOTest::~CAOTest()
	{
	Cancel();	
	
	iLbsNetSimTest->Close();
	delete iLbsNetSimTest;
	}
	
void CLbsRegisterLcsMoLrTest::CAOTest::RunL()
	{
	switch (iBase->State())
		{
		} // switch
		
	}
	
void CLbsRegisterLcsMoLrTest::CAOTest::DoCancel()
	{
	}
	
TBool CLbsRegisterLcsMoLrTest::CAOTest::Finished()
	{
	return (iNotificationRegisterLcsMoLr && 
			iNotificationMeasurementReportLocation && 
			iNotificationFacilityLcsMoLrResult && 
			iNotificationReleaseLcsMoLr && 
			iNotificationMeasurementControlLocation);
	}
	
void CLbsRegisterLcsMoLrTest::CAOTest::NotifyRegisterLcsMoLr(const TDesC& aData)
	{
	(void)aData;
	iNotificationRegisterLcsMoLr = ETrue;
	
	if (iStepMode)
		{
		iLbsNetSimTest->Next(KErrNone);
		}
	}
void CLbsRegisterLcsMoLrTest::CAOTest::NotifyReleaseLcsMoLr(TInt)
	{
	iNotificationReleaseLcsMoLr = ETrue;
	
	iBase->SetState(EFinished);
	iBase->ActivateGateway();
	
	if (iStepMode)
		{
		iLbsNetSimTest->Next(KErrNone);
		}
	}
void CLbsRegisterLcsMoLrTest::CAOTest::NotifyMeasurementReportLocation(const TPositionInfo&)
	{
	iNotificationMeasurementReportLocation = ETrue;

	if (iStepMode)
		{
		iLbsNetSimTest->Next(KErrNone);
		}
	}
void CLbsRegisterLcsMoLrTest::CAOTest::NotifyMeasurementControlLocation(const TPositionInfo& aPosition, const RLbsAssistanceDataBuilderSet& aData, const TLbsNetPosRequestQuality& aQuality)
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
void CLbsRegisterLcsMoLrTest::CAOTest::NotifyFacilityLcsMoLrResult(TInt, const TPositionInfo&)
	{
	iNotificationFacilityLcsMoLrResult = ETrue;

	if (iStepMode)
		{
		iLbsNetSimTest->Next(KErrNone);
		}
	}
