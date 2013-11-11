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
 @file LocIndicatorLib0Step.cpp
 @internalTechnology
*/

#include "LocIndicatorLib0Step.h"
#include "Te_LocIndicatorLibWaiter.cpp"


CLocIndicatorLib0Step::~CLocIndicatorLib0Step()
/**
 * Destructor
 */
	{
	}

CLocIndicatorLib0Step::CLocIndicatorLib0Step()
/**
 * Constructor
 */
	{
	// **MUST** call SetTestStepName in the constructor as the controlling
	// framework uses the test step name immediately following construction to set
	// up the step's unique logging ID.
	SetTestStepName(KLocIndicatorLib0Step);
	}

TVerdict CLocIndicatorLib0Step::doTestStepPreambleL()
/**
 * @return - TVerdict code
 * Override of base class virtual
 */
	{
	SetTestStepResult(EPass);
	return TestStepResult();
	}


TVerdict CLocIndicatorLib0Step::doTestStepL()
/**
 * @return - TVerdict code
 * Override of base class pure virtual
 * Our implementation only gets called if the base class doTestStepPreambleL() did
 * not leave. That being the case, the current test result value will be EPass.
 */
	{
	if (TestStepResult()==EPass)
		{
		// Firstly test non callback version, as used by base band
		CPosIntGpsHwStatus* gpsHwStatus = CPosIntGpsHwStatus::NewLC();
		
		CPosIntGpsHwStatus::TIntGpsHwStatus stateOn = CPosIntGpsHwStatus::EStatusOn;
		gpsHwStatus->SetStatusL(stateOn);
		
		CPosIntGpsHwStatus::TIntGpsHwStatus validateStateOn;
		gpsHwStatus->GetStatusL(validateStateOn);

		TEST(stateOn == validateStateOn);
		
		
		
		// Now test callback version as used by client side
		CActiveScheduler* scheduler = new(ELeave) CActiveScheduler;
		CleanupStack::PushL(scheduler);
		CActiveScheduler::Install(scheduler);
		CPosIntGpsHwStatus* gpsHwStatusCallback = CPosIntGpsHwStatus::NewLC(*this);
				
		const TTimeIntervalMicroSeconds32 KEventTimeout(10000000);
		
		iHwStatus = CPosIntGpsHwStatus::EStatusOff;
		gpsHwStatus->SetStatusL(iHwStatus);
		WaitForEventL(KEventTimeout, *this);
		
		iHwStatus = CPosIntGpsHwStatus::EStatusOn;
		gpsHwStatus->SetStatusL(iHwStatus);
		WaitForEventL(KEventTimeout, *this);

		iHwStatus = CPosIntGpsHwStatus::EStatusActive;
		gpsHwStatus->SetStatusL(iHwStatus);
		WaitForEventL(KEventTimeout, *this);
		
		iHwStatus = CPosIntGpsHwStatus::EStatusUnknown;
		gpsHwStatus->SetStatusL(iHwStatus);
		// EStatusUnknown will be converted to EStatusOff
		iHwStatus = CPosIntGpsHwStatus::EStatusOff; 
		WaitForEventL(KEventTimeout, *this);
		
		
		CleanupStack::PopAndDestroy(gpsHwStatusCallback);
		CleanupStack::PopAndDestroy(scheduler);
		CleanupStack::PopAndDestroy(gpsHwStatus);
		}
	return TestStepResult();
	}



TVerdict CLocIndicatorLib0Step::doTestStepPostambleL()
/**
 * @return - TVerdict code
 * Override of base class virtual
 */
	{
	return TestStepResult();
	}


void CLocIndicatorLib0Step::OnStatusUpdateEvent(CPosIntGpsHwStatus::TIntGpsHwStatus aStatus, TInt aError)
	{
	TEST(aError == KErrNone);
	TEST(aStatus == iHwStatus);
	ReturnToTestStep();
	}
