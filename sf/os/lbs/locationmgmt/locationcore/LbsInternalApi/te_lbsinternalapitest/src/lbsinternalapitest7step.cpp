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
 @file lbsinternalapiTest7Step.cpp
*/
#include "lbsinternalapitest7step.h"
#include "te_lbsinternalapiTestsuitedefs.h"

#include <lbs/lbsgpsmeasurement.h>
#include "LbsInternalInterface.h"
#include "lbsqualityprofile.h"

ClbsinternalapiTest7Step::~ClbsinternalapiTest7Step()
/**
 * Destructor
 */
	{
	}

ClbsinternalapiTest7Step::ClbsinternalapiTest7Step()
/**
 * Constructor
 */
	{
	// **MUST** call SetTestStepName in the constructor as the controlling
	// framework uses the test step name immediately following construction to set
	// up the step's unique logging ID.
	SetTestStepName(KlbsinternalapiTest7Step);
	}

TVerdict ClbsinternalapiTest7Step::doTestStepPreambleL()
/**
 * @return - TVerdict code
 * Override of base class virtual
 */
	{
	INFO_PRINTF1(_L("Please delete this line or modify it. I am in Test Step Preamble in Class ClbsinternalapiTest7Step"));
	// uncomment the following 3 lines if you have common pre setting to all the test steps in there
	// CTe_lbsinternalapiTestSuiteStepBase::doTestStepPreambleL();
	// if (TestStepResult()!=EPass)
	//    return   TestStepResult();
	// process some pre setting to this test step then set SetTestStepResult to EFail or Epass.
	SetTestStepResult(EPass);
	return TestStepResult();
	}


TVerdict ClbsinternalapiTest7Step::doTestStepL()
/**
 * @return - TVerdict code
 * Override of base class pure virtual
 * Our implementation only gets called if the base class doTestStepPreambleL() did
 * not leave. That being the case, the current test result value will be EPass.
 */
	{
	  if (TestStepResult()==EPass)
		{
		//
		// Test the GPS Measurement Updates interface
		//
		
		RLbsGpsMeasurementUpdates::InitializeL();
		
		RLbsGpsMeasurementUpdates gpsMeasurements;
		gpsMeasurements.OpenL();
		CleanupClosePushL(gpsMeasurements);
		
		// Generate some GPS measurement data
		TPositionGpsMeasurementData gpsData;
		gpsData.SetSatelliteId(3);
		gpsData.SetCarrierNoiseRatio(0x14);
		gpsData.SetDoppler(0x09);
		gpsData.SetWholeGpsChips(0x124);
		gpsData.SetFractionalGpsChips(0x03);
		gpsData.SetMultiPathIndicator(TPositionGpsMeasurementData::EMultiPathHigh);
		gpsData.SetPseudoRangeRmsError(1);
		
		TPositionGpsMeasurementInfo gpsInfo;
		gpsInfo.SetGpsTimeOfWeek(3);
		gpsInfo.SetPositionMode(TPositionModuleInfo::ETechnologyTerminal);
		gpsInfo.SetPositionModeReason(KErrNone);
		gpsInfo.SetUpdateType(EPositionUpdateGeneral);
		gpsInfo.SetModuleId(TUid::Uid(0x10281D6A));
		gpsInfo.AppendMeasurementData(gpsData);
		TESTL(gpsInfo.NumMeasurements() == 1);
		
		//
		// Test setting measurement data
		//
		TTime timeNow;
		timeNow.UniversalTime();
		TInt err = gpsMeasurements.SetGpsMeasurementInfo(KErrNone, 
														 &gpsInfo,
														 sizeof(gpsInfo),
														 timeNow);
		TESTL(err == KErrNone);
		
		//
		// Test getting measurement data
		//
		TTime timeGot;
		TPositionGpsMeasurementInfo gpsInfo2;
		err = gpsMeasurements.GetGpsMeasurementInfo(&gpsInfo2,
													sizeof(gpsInfo2),
													timeGot);
		TESTL(err == KErrNone);
		TESTL(gpsInfo2.NumMeasurements() == gpsInfo.NumMeasurements());
		TESTL(gpsInfo2.GpsTimeOfWeek() == gpsInfo.GpsTimeOfWeek());
		TESTL(gpsInfo2.PositionMode() == gpsInfo.PositionMode());
		TESTL(gpsInfo2.PositionModeReason() == gpsInfo.PositionModeReason());
		TESTL(gpsInfo2.UpdateType() == gpsInfo.UpdateType());
		TESTL(gpsInfo2.ModuleId() == gpsInfo.ModuleId());
		TESTL(timeGot == timeNow);		
		
		//
		// Test notification of updates
		//
		TRequestStatus status;
		gpsMeasurements.NotifyGpsMeasurementUpdate(status);
		timeNow.UniversalTime();
		err = gpsMeasurements.SetGpsMeasurementInfo(KErrNone, 
													&gpsInfo,
													sizeof(gpsInfo),
													timeNow);
		TESTL(err == KErrNone);
		User::WaitForRequest(status);
		TESTL(status.Int() == KErrNone);
		err = gpsMeasurements.GetGpsMeasurementInfo(&gpsInfo2,
													sizeof(gpsInfo2),
													timeGot);
		TESTL(err == KErrNone);
		TESTL(timeGot == timeNow);		
		
		//
		// Test errors are passed along
		//
		err = gpsMeasurements.SetGpsMeasurementInfo(KErrNotReady, 
													&gpsInfo,
													sizeof(gpsInfo),
													timeNow);
		TESTL(err == KErrNone);
		err = gpsMeasurements.GetGpsMeasurementInfo(&gpsInfo2,
													sizeof(gpsInfo2),
													timeGot);
		TESTL(err == KErrNotReady);
		
		// End of tests
		CleanupStack::PopAndDestroy(&gpsMeasurements);
		
		RLbsGpsMeasurementUpdates::ShutDownL();
		}
	  return TestStepResult();
	}



TVerdict ClbsinternalapiTest7Step::doTestStepPostambleL()
/**
 * @return - TVerdict code
 * Override of base class virtual
 */
	{
	INFO_PRINTF1(_L("Please delete this line or modify it. I am in Test Step Postamble in Class ClbsinternalapiTest7Step"));
	// process something post setting to the test step
	// uncomment the following line if you have common post setting to all the test steps in there
	// CTe_lbsinternalapiTestSuiteStepBase::doTestStepPostambleL();
	// uncomment the following line if you have post process or remove the following line if no post process
	// SetTestStepResult(EPass);		// or EFail
	return TestStepResult();
	}
