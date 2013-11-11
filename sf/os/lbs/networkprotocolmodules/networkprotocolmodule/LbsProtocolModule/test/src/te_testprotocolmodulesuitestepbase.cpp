// Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
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
//

/**
 @file te_testprotocolmodulesuitestepbase.cpp
*/

#include <e32std.h>
#include "te_testprotocolmodulesuitestepbase.h"
#include "te_testprotocolmodulesuitedefs.h"

/**
 * @return - TVerdict
 * Implementation of CTestStep base class virtual
 * It is used for doing all initialisation common to derived classes in here.
 * Make it being able to leave if there are any errors here as there's no point in
 * trying to run a test step if anything fails.
 * The leave will be picked up by the framework.
 */
TVerdict CTe_testprotocolmoduleSuiteStepBase::doTestStepPreambleL()
	{
	// START MEMORY TEST
	__UHEAP_MARK;

	// Construct a new ActiveScheduler and install it
	iSched = new (ELeave) CActiveScheduler();
	CActiveScheduler::Install(iSched);
	
	// Create Module observer - Gateway side
	iGatewayObserver = CGatewayObserver::NewL();

	// Create Module observer - Network side
	iNetworkObserver = CNetworkObserver::NewL();

	ResetObserversL();
	
	SetTestStepResult(EPass);
	return TestStepResult();
	}


/**
 * @return - TVerdict
 * Implementation of CTestStep base class virtual
 * It is used for doing all after test treatment common to derived classes in here.
 * Make it being able to leave
 * The leave will be picked up by the framework.
 */
TVerdict CTe_testprotocolmoduleSuiteStepBase::doTestStepPostambleL()
	{
	// Delete Module observers
	delete iNetworkObserver;
	iNetworkObserver = NULL;
	delete iGatewayObserver;
	iGatewayObserver = NULL;
	
	// Remove ActiveScheduler
	CActiveScheduler::Install(NULL);
	delete iSched;
	iSched = NULL;
	
	// END MEMORY TEST
	__UHEAP_MARKEND;

	return TestStepResult();
	}

/** Destructor
*/
CTe_testprotocolmoduleSuiteStepBase::~CTe_testprotocolmoduleSuiteStepBase()
	{
	}


/** Constructor
*/
CTe_testprotocolmoduleSuiteStepBase::CTe_testprotocolmoduleSuiteStepBase()
	{
	}


/** Reset Gateway-side observer callback status.
*/
void CTe_testprotocolmoduleSuiteStepBase::ResetGatewayObserverL()
	{
	User::LeaveIfNull(iGatewayObserver);
	iGatewayObserver->ResetWasObserverCalled();
	}


/** Reset Network-side observer callback status.
*/
void CTe_testprotocolmoduleSuiteStepBase::ResetNetworkObserverL()
	{
	User::LeaveIfNull(iNetworkObserver);
	iNetworkObserver->ResetWasObserverCalled();
	}


/** Reset callback status for all observers.
*/
void CTe_testprotocolmoduleSuiteStepBase::ResetObserversL()
	{
	ResetGatewayObserverL();
	ResetNetworkObserverL();
	}


/** Wait for and check a Network-side observer callback.
@param aExpectedResult The expected type of callback.
@return TVerdict EFail if expected callback not received.
*/
TVerdict CTe_testprotocolmoduleSuiteStepBase::
					CheckNetworkCallbackL(TInt aExpectedResult)
	{
	User::LeaveIfNull(iNetworkObserver);

	// Wait for network observer callback
	iNetworkObserver->WaitForCallBack();
	
	// Check that callback matches the expected type
	if ((aExpectedResult != iNetworkObserver->CallBackResult()) ||
		!iNetworkObserver->WasObserverCalled()
	   )
		{
		SetTestStepResult(EFail);
		}

	// Immediately reset observer callback status
	ResetNetworkObserverL();
	return TestStepResult();
	}


/** Wait for and check a Gateway-side observer callback.
@param aExpectedResult The expected type of callback.
@return TVerdict EFail if expected callback not received.
*/
TVerdict CTe_testprotocolmoduleSuiteStepBase::
					CheckGatewayCallbackL(TInt aExpectedResult)
	{
	User::LeaveIfNull(iGatewayObserver);

	// Wait for gateway observer callback
	iGatewayObserver->WaitForCallBack();
	
	// Check that callback matches the expected type
	if ((aExpectedResult != iGatewayObserver->CallBackResult()) ||
		!iGatewayObserver->WasObserverCalled()
	   )
		{
		SetTestStepResult(EFail);
		}

	// Immediately reset observer callback status
	ResetGatewayObserverL();
	return TestStepResult();
	}


/** Set dummy data into assistance data set.
This utility method adds dummy data within an assistance data set.

@param aAssistData The assistance data set to have dummy data added.
@param aDataRequestMask Identifies the set of data types for which
	dummy data is to be added.
*/
void CTe_testprotocolmoduleSuiteStepBase::SetDummyAssistanceData(
								RLbsAssistanceDataBuilderSet& aAssistData, 
								TLbsAsistanceDataGroup aDataRequestMask)
	{
	// Clear any previously stored data.
	aAssistData.ClearAllBuilders();

	// Get UTC time
	TTime utcTime;
	utcTime.UniversalTime();

	// Set up dummy data for all relevant requested items
	if (aDataRequestMask & EAssistanceDataAquisitionAssistance)
		{
		RUEPositioningGpsAcquisitionAssistanceBuilder* acquisition;
		if (KErrNone == aAssistData.GetDataBuilder(acquisition))
			{
			acquisition->SetTimeStamp(utcTime);
			acquisition->SetField(TUEPositioningGpsAcquisitionAssistance::EGpsReferenceTime, 
								  TUint(123055));
			}
		}

	if (aDataRequestMask & EAssistanceDataNavigationModel)
		{
		RUEPositioningGpsNavigationModelBuilder* navigation;
		if (KErrNone == aAssistData.GetDataBuilder(navigation))
			{
			navigation->SetTimeStamp(utcTime);

			RNavigationModelSatInfoArrayBuilder satinfoArray;
			navigation->GetArrayBuilder(TUEPositioningGpsNavigationModel::ENavigationModelSatelliteInfoArray, 
										satinfoArray);
			RNavigationModelSatInfoBuilder satinfoBuilder;
			satinfoArray.GetFieldBuilder(0, satinfoBuilder);
			satinfoBuilder.SetField(TNavigationModelSatInfo::ESatId, TUint(1));
			}
		}

	if (aDataRequestMask & EAssistanceDataIonosphericModel)
		{
		RUEPositioningGpsIonosphericModelBuilder* ionospheric;
		if (KErrNone == aAssistData.GetDataBuilder(ionospheric))
			{
			ionospheric->SetTimeStamp(utcTime);
			ionospheric->SetField(TUEPositioningGpsIonosphericModel::EAlfa0, TUint(123));
			}
		}

	if (aDataRequestMask & EAssistanceDataReferenceLocation)
		{
		RReferenceLocationBuilder* refLoc;
		if (KErrNone == aAssistData.GetDataBuilder(refLoc))
			{
			refLoc->SetTimeStamp(utcTime);

			REllipsoidPointAltitudeEllipsoideBuilder locBuilder;
			refLoc->GetFieldBuilder(TReferenceLocation::EEllipsoidPointAltitudeEllipsoide, 
									locBuilder);
			locBuilder.SetField(TEllipsoidPointAltitudeEllipsoide::ELatitudeSign,
								TEllipsoidPointAltitudeEllipsoide::ESouth);
			}
		}

	if (aDataRequestMask & EAssistanceDataPositioningGpsUtcModel)
		{
		RUEPositioningGpsUtcModelBuilder* utcModel;
		if (KErrNone == aAssistData.GetDataBuilder(utcModel))
			{
			utcModel->SetTimeStamp(utcTime);
			utcModel->SetField(TUEPositioningGpsUtcModel::EA1, TUint(12));
			}
		}

	if (aDataRequestMask & EAssistanceDataAlmanac)
		{
		RUEPositioningGpsAlmanacBuilder* almanac;
		if (KErrNone == aAssistData.GetDataBuilder(almanac))
			{
			almanac->SetTimeStamp(utcTime);
			almanac->SetField(TUEPositioningGpsAlmanac::EWnA, TUint(12));
			}
		};

	if (aDataRequestMask & EAssistanceDataBadSatList)
		{
		RBadSatListBuilder* badsat;
		if (KErrNone == aAssistData.GetDataBuilder(badsat))
			{
			badsat->SetTimeStamp(utcTime);

			RBadSatArrayBuilder badsatArray;
			badsat->GetArrayBuilder(TBadSatList::EBadSatIdArray, 
										   badsatArray);
			badsatArray.SetElement(0, TUint(1));
			}
		}

	if (aDataRequestMask & EAssistanceDataReferenceTime)
		{
		RUEPositioningGpsReferenceTimeBuilder* refTime;
		if (KErrNone == aAssistData.GetDataBuilder(refTime))
			{
			refTime->SetTimeStamp(utcTime);
			refTime->SetField(TUEPositioningGpsReferenceTime::EGpsWeek, TUint(12));
			}
		}

	}
