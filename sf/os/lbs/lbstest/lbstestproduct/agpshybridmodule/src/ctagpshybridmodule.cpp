// Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
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

//! @file ctagpshybridmodule.cpp

// User includes
#include "ctagpshybridmodule.h"
#include <lbs/test/ctdumper.h>

// Lbs includes
#include <lbs/lbsassistancereferencetime.h>
#include <lbs/lbsassistancereferencelocation.h>
#include <lbs/test/lbsnetsimtest.h>
#include <lbs/test/lbsparamlogger.h>
#include <lbserrors.h>
#include "LbsInternalInterface.h"
#include "lbsdevloggermacros.h"

#include <lbs/test/tlbsdefs.h>
#define __ASSERT_ALWAYSX(c,p) (void)((c)||(RDebug::Printf("Assert at line %d in file %s ",__LINE__,__FILE__),p,0));

// Literals Used
const TInt KAssistanceDataTimeDelay = 1000000;
const TInt KDelayUpdateTimeDelay = 500000;

const TInt KExpectedNumOfEntries = 2;		// Expected number of update items to report when in hybird mode.
const TInt KExtraUpdates = 2;				// Number of additional updates when in multi-update mode. NOTE: may allow user to update, via option.
const TInt KIntervalOffset = 1000000;		// 1 second to ensure to ensure updates are not delivered too close to either 'target time' or 'max fix time'.

const TInt KPosUpdateTimerId = 1;
const TInt KDelayUpdateTimerId = 2;
const TInt KQuickPositionUpdate = 1000;

const TInt KLbsModuleNumOfUpdates = 3;  // Can't be greater than 16

#define POS_LAT                         49.2
#define POS_LONG                        3.5
#define POS_ALT                         50.0
#define POS_HORZ_ACCURACY               2
#define POS_VERT_ACCURACY               3
#define SPEED                           26.0
#define VERTICAL_SPEED                  20.0
#define HEADING                         25.0
#define COURSE                          30.0
#define SPEED_ACCURACY                  2.0
#define VERTICAL_SPEED_ACCURACY         3.0
#define HEADING_ACCURACY                10.0    
#define COURSE_ACCURACY                 4.0
//TGpsTimingMeasurementData
#define GPS_TIMING_OF_CELL_MsPart       16383                          
#define GPS_TIMING_OF_CELL_LsPart       4294967295UL
#define REFERENCE_IDENTITY              511
#define SFN                             4095
//TDetailedErrorReport
#define SD_OF_LONG_ERROR                5.0 
#define SD_OF_LAT_ERROR                 6.0
#define SD_OF_ALT_ERROR                 7.0 
#define SD_OF_SEMI_MAJOR_AXIS_ERROR     8.0
#define SD_OF_SEMI_MINOR_AXIS_ERROR     9.0
#define ORIEN_OF_SEMI_MAJOR_AXIS_ERROR  10.0
#define RMS_VAL_OF_SD_OF_RANGE          11.0

#define GEOIDAL_SEPARATION              12.0
#define MAGNETIC_VARIATION              13.0
#define COURSE_OVER_GROUND_MAGNETIC     14.0

CT_AGpsHybridModule::CT_AGpsHybridModule(MLbsLocationSourceGpsObserver& aObserver)
	:
	CLbsLocationSourceGpsBase(aObserver), iGPSModeNotSupported(EFalse)
	{
	}


CLbsLocationSourceGpsBase* CT_AGpsHybridModule::NewL(MLbsLocationSourceGpsObserver& aObserver)
	{
	CT_AGpsHybridModule* self = new(ELeave) CT_AGpsHybridModule(aObserver);

	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);

	return self;
	}


void CT_AGpsHybridModule::ConstructL()
	{
	LBSLOG(ELogP1, "CT_AGpsHybridModule::ConstructL()\n");

	CLbsLocationSourceGpsBase::ConstructL();
	
	// Create position update and assistance data timers.
	iPosUpdateTimer = CT_LbsTimerUtils::NewL(this, KPosUpdateTimerId);
	iDelayUpdateTimer = CT_LbsTimerUtils::NewL(this, KDelayUpdateTimerId);	

	// Create the AGPS2TH channel handler to listen for incoming data.	
	iTHHandler = CT_TestHarnessHandler::NewL(this);

	// Set default error code.
	iError = KErrNone;

	// Assistance data processing on.
	SetOption(ELbsHybridModuleOptions_AssistanceDataOn, ETrue);
	
	//Setup the assistance data that we need to request
	ClearAssistanceData();

	// Provide some default update data.
	PopulateUpdateArrayL();	
	iUpdateArrIndex = 0;
	
	// Report module status.
	TPositionModuleStatus modStatus;
	
	modStatus.SetDeviceStatus(TPositionModuleStatus::EDeviceReady);
	modStatus.SetDataQualityStatus(TPositionModuleStatus::EDataQualityNormal);
		
	iLocSourceGpsObserver.UpdateDeviceStatus(modStatus.DeviceStatus());
	iLocSourceGpsObserver.UpdateDataQualityStatus(modStatus.DataQualityStatus());

	TInt error = RProperty::Define(KUidSystemCategory, ELbsTestAGpsModuleResetAssistanceDataFlag, RProperty::EInt);
	error = RProperty::Set(KUidSystemCategory, ELbsTestAGpsModuleResetAssistanceDataFlag, ELbsTestAGpsResetAssistanceDataNotReceived);
	// Clear module update log.

    error = RProperty::Define(KUidSystemCategory, ELbsTestAGpsModuleAssistanceDataRequestFlag, RProperty::EInt);
    error = RProperty::Set(KUidSystemCategory, ELbsTestAGpsModuleAssistanceDataRequestFlag, ELbsTestAGpsModuleAssistanceDataExpected);
	LBSDUMPNEWLOG();
	}



void CT_AGpsHybridModule::PopulateUpdateArrayL()
	{
	TLbsModuleUpdateItem* update;
	TPositionExtendedSatelliteInfo posInfo;
    TCourse course;
	TPosition pos;
	TPositionGpsMeasurementInfo measureInfo;	
	TPositionGpsMeasurementData measureData;
    TGpsTimingMeasurementData gpsTimingData;
    TDetailedErrorReport detailedErrorReport;
	for (TInt i = 0; i < KLbsModuleNumOfUpdates; i++)
		{
		// Create the update.
		update = new (ELeave) TLbsModuleUpdateItem();
		
		// Set position info.
		pos.SetCoordinate(i + POS_LAT, i + POS_LONG, i + POS_ALT);
		pos.SetAccuracy(POS_HORZ_ACCURACY, POS_VERT_ACCURACY);
		pos.SetCurrentTime();
		posInfo.SetPosition(pos);

        // Set Speed and Direction
        course.SetSpeed(SPEED);
        course.SetVerticalSpeed(VERTICAL_SPEED);
        course.SetHeading(HEADING);
        course.SetSpeedAccuracy(SPEED_ACCURACY);
        course.SetVerticalSpeedAccuracy(VERTICAL_SPEED_ACCURACY);
        course.SetHeadingAccuracy(HEADING_ACCURACY);
        course.SetCourse(COURSE);
        course.SetCourseAccuracy(COURSE_ACCURACY);
        posInfo.SetCourse(course);
        
        // Set timing assistance measurement data
        gpsTimingData.SetDataType(TGpsTimingMeasurementData::EGpsTimingDataTypeUtran);
        gpsTimingData.SetNetworkMode(TGpsTimingMeasurementData::ENetworkModeFdd);
        gpsTimingData.SetGPSTimingOfCellMsPart(GPS_TIMING_OF_CELL_MsPart);
        gpsTimingData.SetGPSTimingOfCellLsPart(GPS_TIMING_OF_CELL_LsPart);
        gpsTimingData.SetReferenceIdentity(REFERENCE_IDENTITY);
        gpsTimingData.SetSfn(SFN);
        posInfo.SetGpsTimingData(gpsTimingData);
                
        //Set GNSS Pseudorange Error Statistics 
        detailedErrorReport.SetStanDeviOfLongitudeError(SD_OF_LONG_ERROR);
        detailedErrorReport.SetStanDeviOfLatiitudeError(SD_OF_LAT_ERROR);
        detailedErrorReport.SetStanDeviOfAltitudeError(SD_OF_ALT_ERROR);
        detailedErrorReport.SetStanDeviOfSemiMajorAxisError(SD_OF_SEMI_MAJOR_AXIS_ERROR);
        detailedErrorReport.SetStanDeviOfSemiMinorAxisError(SD_OF_SEMI_MINOR_AXIS_ERROR);
        detailedErrorReport.SetOrientationOfSemiMajorAxisError(ORIEN_OF_SEMI_MAJOR_AXIS_ERROR);
        detailedErrorReport.SetRmsValOfStanDeviOfRange(RMS_VAL_OF_SD_OF_RANGE);
        posInfo.SetDetailedErrorReport(detailedErrorReport);
        // Set Geoidal separation
        posInfo.SetGeoidalSeparation(GEOIDAL_SEPARATION);
        //Set Magnetic variation
        posInfo.SetMagneticVariation(MAGNETIC_VARIATION);
        //Set Course over ground
        posInfo.SetCourseOverGroundMagnetic(COURSE_OVER_GROUND_MAGNETIC);
		update->SetPosition(posInfo);				
		
		// Set measurement info.
		for (TInt j = 0; j < i; j++)
			{
			measureData.SetSatelliteId(j + 1);
			measureData.SetCarrierNoiseRatio((j+1) + 1);
			measureData.SetDoppler((j+1) + 2);
			measureData.SetWholeGpsChips((j+1) + 3);
			measureData.SetFractionalGpsChips((j+1) + 4);
			measureData.SetMultiPathIndicator(TPositionGpsMeasurementData::EMultiPathLow);
			measureData.SetPseudoRangeRmsError((j+1) + 5);

			User::LeaveIfError(measureInfo.AppendMeasurementData(measureData));
			}
		measureInfo.SetGpsTimeOfWeek(i + 1000);
		update->SetMeasurement(measureInfo);
		
		// Set error.
		update->SetError(KErrNone);

		// Add new update to update array.
		iUpdateArr.AppendL(update);

		// Clear structs for next time around.
		measureInfo.ClearMeasurementData();
		}
	}


CT_AGpsHybridModule::~CT_AGpsHybridModule()
	{
	LBSLOG(ELogP1, "CT_AGpsHybridModule::~CT_AGpsHybridModule()\n");
	
	iUpdateArr.ResetAndDestroy();

	iTHHandler->Cancel();
	delete iTHHandler;
	
	delete iDelayUpdateTimer;
	delete iPosUpdateTimer;
	}	


void CT_AGpsHybridModule::SetGpsOptions(const TLbsGpsOptions& aGpsOptions)
	{
	iGPSModeNotSupported = EFalse;
	LBSTESTLOG_METHOD1(INBOUND, "CT_AGpsHybridModule::SetGpsOptions", aGpsOptions);
	LBSLOG(ELogP1, "CT_AGpsHybridModule::SetGpsOptions()\n");
	PublishGpsOptions(aGpsOptions);

	// Process gps options.
	iGpsOptionsType = aGpsOptions.ClassType();
	
	if (iGpsOptionsType & ELbsGpsOptionsArrayClass)
		{
		const TLbsGpsOptionsArray& optionsArr = reinterpret_cast<const TLbsGpsOptionsArray&>(aGpsOptions);
		
		iGpsOptions =  optionsArr;
		}
	else	
		{
		TPositionModuleInfoExtended::TDeviceGpsModeCapabilities deviceCapabilities;
		TInt err = LbsModuleInfo::GetDeviceCapabilities(KLbsGpsLocManagerUid, deviceCapabilities);
		if((!(deviceCapabilities & TPositionModuleInfoExtended::EDeviceGpsModeSimultaneousTATB)) && (!(deviceCapabilities & TPositionModuleInfoExtended::EDeviceGpsModeTerminalAssisted)))
			{
			if(aGpsOptions.GpsMode() == CLbsAdmin::EGpsPreferTerminalAssisted || 
				aGpsOptions.GpsMode() == CLbsAdmin::EGpsAlwaysTerminalAssisted)
				{
				iGPSModeNotSupported = ETrue;
				}	
			}
		if((!(deviceCapabilities & TPositionModuleInfoExtended::EDeviceGpsModeSimultaneousTATB)) && (!(deviceCapabilities & TPositionModuleInfoExtended::EDeviceGpsModeTerminalBased)))
			{
			if(aGpsOptions.GpsMode() == CLbsAdmin::EGpsPreferTerminalBased || 
					aGpsOptions.GpsMode() == CLbsAdmin::EGpsAlwaysTerminalBased ||
					aGpsOptions.GpsMode() == CLbsAdmin::EGpsAutonomous)
				{
				iGPSModeNotSupported = ETrue;
				}	
			}
						
		iGpsOptions.SetGpsMode(aGpsOptions.GpsMode());
		iGpsOptions.ClearOptionItems();
		}
	
	// Determine if assistance data will ever be required.
	switch (iGpsOptions.GpsMode())
		{
		case CLbsAdmin::EGpsPreferTerminalBased:
		case CLbsAdmin::EGpsAlwaysTerminalBased:
		case CLbsAdmin::EGpsAlwaysTerminalAssisted:
		case CLbsAdmin::EGpsPreferTerminalAssisted:
			{
			// Currently it is possible for the test harness to switch on/off assistance data requesting.
			if (GetOption(ELbsHybridModuleOptions_AssistanceDataOn))
				{
				iAssistanceDataOn = ETrue;
				}
			else
				{
				iAssistanceDataOn = EFalse;
				}	
			}
			break;
		
		case CLbsAdmin::EGpsAutonomous:
		default:
			{
			iAssistanceDataOn = EFalse;
			}
		}
	}


void CT_AGpsHybridModule::RequestLocationUpdate(const TTime& aTargetTime, const TLbsLocRequestQuality& aQuality)
	{
	LBSTESTLOG_METHOD2(INBOUND, "CT_AGpsHybridModule::RequestLocationUpdate", aTargetTime, aQuality);
	LBSLOG(ELogP1, "CT_AGpsHybridModule::RequestLocationUpdate()\n");
	
	// If there is currently a position request outstanding, cancel it.
	if (iPosUpdateTimer->IsActive())
		{
		iPosUpdateTimer->Cancel();
		}									
	if (iDelayUpdateTimer->IsActive())
		{
		iDelayUpdateTimer->Cancel();
		}


	// Update our quality.
	iQuality = aQuality;


	// Response with update at the required target time + any time out period. The time out allows
	// the module to delay return giving a position update, normally it will be set to zero.
	if (aTargetTime == TTime(0))
		{
		iTargetTime.UniversalTime();
		}
	else
		{	
		iTargetTime = aTargetTime + iTimeOut;
		}
		
	// Is there a delay to add on!
	if (iUpdateArrIndex < iUpdateArr.Count())
		{
		TInt delay = iUpdateArr[iUpdateArrIndex]->Delay() * 1000;
		if (delay != 0)
			{
			if(delay < 0)
				{
				iTargetTime -= TTimeIntervalMicroSeconds(-delay);
				}
			else
				{
				iTargetTime += TTimeIntervalMicroSeconds(delay);
				}	
			}
		}
	
	if(iGPSModeNotSupported)
		{
		iTargetTime = KQuickPositionUpdate;
		}

	iPosUpdateTimer->SetTimer(iTargetTime);
	

	// Reset interval update variables is multi-update module option is on.
	if (GetOption(ELbsHybridModuleOptions_MultiUpdatesOn))
		{
		iExtraUpdatesCtr = KExtraUpdates;
		iIntervalDelay = (iQuality.MaxFixTime().Int64() - 2 * KIntervalOffset) / iExtraUpdatesCtr;
		}
	}


void CT_AGpsHybridModule::CancelLocationRequest()
	{
	LBSTESTLOG_METHOD(INBOUND, "CT_AGpsHybridModule::CancelLocationRequest");
	
	T_LbsUtils utils;
	utils.IncrementIntegrationModulesCountOfCancels();

	iPosUpdateTimer->CancelTimer();
	iDelayUpdateTimer->CancelTimer();
	}


void CT_AGpsHybridModule::AdvisePowerMode(CLbsLocationSourceGpsBase::TPowerMode aMode)
	{
	LBSLOG(ELogP1, "CT_AGpsHybridModule::AdvisePowerMode()\n");
	LBSTESTLOG_METHOD1(INBOUND, "CT_AGpsHybridModule::AdvisePowerMode", aMode);

	// NOTE: could report device status on power mode change.

	if (aMode == CLbsLocationSourceGpsBase::EPowerModeClose)
		{
		iLocSourceGpsObserver.Shutdown();
		}
	}


void CT_AGpsHybridModule::AssistanceDataEvent(TInt aError, TLbsAsistanceDataGroup aDataMask)
	{
	TBool verified = ETrue;

	LBSTESTLOG_METHOD2(INBOUND, "CT_AGpsHybridModule::AssistanceDataEvent", aError, aDataMask);
	
	LBSLOG(ELogP1, "CT_AGpsHybridModule::AssistanceDataEvent()\n");
	LBSLOG2(ELogP1, "\t\tiAssistanceDataRequested = %d\n", iAssistanceDataRequested);
	LBSLOG2(ELogP1, "\t\tiAssistanceDataAvailable = %d\n", iAssistanceDataAvailable);
	LBSLOG(ELogP1, "\t\t\n");

	if(aError == KPositionAssistanceDataReset)
		{
		TTime timeStamp;
		RUEPositioningGpsReferenceTimeReader gpsRefTimeReader;
		TRAPD(err,gpsRefTimeReader.OpenL());
		if(err == KErrNone)
			{
			if(aDataMask & EAssistanceDataReferenceTime)
				{ // make sure that the assistance data cache doesn't contain the item
				TInt err = iLocSourceGpsObserver.GetAssistanceDataItem(EAssistanceDataReferenceTime, gpsRefTimeReader, timeStamp);
				if(err == KErrNotFound)
					{ // if everything is as expected then signal that for the test to pick it up
					if(iAssistanceDataAvailable && iDelayUpdateTimer->IsActive())
						{ // If the module was waiting to return a position and the reset affects it, then it will
						  // start waiting to ask for assistance data
						iDelayUpdateTimer->CancelTimer();
						iPosUpdateTimer->SetTimer(iTargetTime);
						}
					
					// indicate that we need the full set of assistance data now
					ClearAssistanceData();
					
					SignalResetAssitanceDataReceived();
					}
				}
			else if(iAssistanceDataAvailable)
				{ // make sure that although there was a reset the assistance data cache still contains the item
				TInt err = iLocSourceGpsObserver.GetAssistanceDataItem(EAssistanceDataReferenceTime, gpsRefTimeReader, timeStamp);
				if(err == KErrNone)
					{ // if everything is as expected then signal that for the test to pick it up
					SignalResetAssitanceDataReceived();
					}			
				}
			else
				{ // make sure that the assistance data is still absent
				TInt err = iLocSourceGpsObserver.GetAssistanceDataItem(EAssistanceDataReferenceTime, gpsRefTimeReader, timeStamp);
				if(err == KErrNotFound)
					{ // if everything is as expected then signal that for the test to pick it up
					SignalResetAssitanceDataReceived();
					}			
				}
			gpsRefTimeReader.Close();
			}
		}
	else if (iAssistanceDataRequested)
		{
		iAssistanceDataRequested = EFalse;
		iAssistanceDataAvailable = ETrue;
		
        // check the assistance data received was part of what was requested
        if(iOutstandingAssistanceData & aDataMask != aDataMask)
        	{
            // NOTE: maybe panic - at the moment log a warning.
            LBSLOG(ELogP1, "CT_AGpsHybridModule - WARNING assistance data did not verify.\n");
        	}
		//Remove the assistance data received from the outstanding mask

        iOutstandingAssistanceData &= ~aDataMask;
		
		// Verify the error and the mask
		if ((aError != KErrNone) || (aDataMask != iOutstandingAssistanceData))
			{
			verified = EFalse;
			}
		else	
			{
			// Verify the assistance data.
			verified = VerifyRefTimeData();
			}

        if(((aError <= KPositionNetworkErrorBase) && (aError >= KErrPositionHighPriorityReceive)    //Network Err
            || ((aDataMask == KLbsDummyAssistanceDataMaskWithError) && (aError != KErrNone)))       //Special Dummy Assistance Data Event Error
            && (iGpsOptions.GpsMode() == CLbsAdmin::EGpsPreferTerminalAssisted ) )   //Only in Prefer Terminal Assisted Mode
            {
            TLbsGpsOptions newGpsOptions;
            newGpsOptions.SetGpsMode(CLbsAdmin::EGpsAutonomous);
            SetGpsOptions(newGpsOptions);
            ClearAssistanceData();
            }

        if (!verified)
			{
			// NOTE: maybe panic - at the moment log a warning.
			LBSLOG(ELogP1, "CT_AGpsHybridModule - WARNING assistance data did not verify.\n");
			}

		
		// Return position now the assistance data is available.
		
		// Take some time to process the assistance data, before the position is actually returned.
		TTime time;
			
		time.UniversalTime();
		time += TTimeIntervalMicroSeconds(KAssistanceDataTimeDelay);
		time += iTimeOut;
		iDelayUpdateTimer->SetTimer(time);			
		}
	// Unsolicited, mark as data available and don't verify.
	else
		{
		iAssistanceDataAvailable = ETrue;

		// NOTE: may verify correct type.
		}
	}	
	

void CT_AGpsHybridModule::HandleTimerL(TInt aTimerId, const TTime& aTargetTime)
	{
	if(iGPSModeNotSupported)
		{
		TPositionInfoBase* deliveryArr[1];
		TPositionModuleInfoExtended::TDeviceGpsModeCapabilities deviceCapabilities;
		TInt err = LbsModuleInfo::GetDeviceCapabilities(KLbsGpsLocManagerUid, deviceCapabilities);
		TPositionGpsMeasurementInfo mes;
		TPositionExtendedSatelliteInfo posInfo;
		
		if(err == KErrNone && deviceCapabilities == TPositionModuleInfoExtended::EDeviceGpsModeTerminalAssisted)
			{ // return measurment with KErrNotSupported if the module can only do that
			deliveryArr[0] = &mes;
			}
		else
			{ // return a position with KErrNotSupported
			deliveryArr[0] = &posInfo;			
			}
		iLocSourceGpsObserver.UpdateLocation(KErrNotSupported, const_cast<const TPositionInfoBase**>(&deliveryArr[0]), 1, iTargetTime);
		return;
		}

	(void)aTargetTime;

	LBSLOG(ELogP1, "CT_AGpsHybridModule::HandleTimerL()\n");
	LBSLOG2(ELogP1, "\t\taTimerId = %d\n", aTimerId);
	LBSLOG2(ELogP1, "\t\tiAssistanceDataOn = %d\n", iAssistanceDataOn);
	LBSLOG2(ELogP1, "\t\tiAssistanceDataRequested = %d\n", iAssistanceDataRequested);
	LBSLOG2(ELogP1, "\t\tiAssistanceDataAvailable = %d\n", iAssistanceDataAvailable);
	LBSLOG(ELogP1, "\t\t\n");
	
	// Our timer fired, handle assistance data request and pos update.
	if (aTimerId == KPosUpdateTimerId)
	{
		// Only request assistance data if it's not available and we have not asked before.
		if ((iAssistanceDataOn) && (!iAssistanceDataAvailable) && (!iAssistanceDataRequested))
			{
			// Now requesting assistance data.
			iAssistanceDataRequested = ETrue;

			// cooment1
			if (!GetOption(ELbsHybridModuleOptions_DisableReqAssistData))
				{
				// Request some simple assistance data.
				iLocSourceGpsObserver.RequestAssistanceData(iOutstandingAssistanceData);
				
				//In some of the tests, Assistance Data is not supplied and position is expected.
				//There is no way to notify Test client API of assistance data is not delivered so the code
				//below is a work around for those scenarios - Need to find a better way to do this.
			    RProperty assDataReqProperty;
			    User::LeaveIfError(assDataReqProperty.Attach(KUidSystemCategory, ELbsTestAGpsModuleAssistanceDataRequestFlag));
			    TInt assDataReqStatus;
			    TInt error = assDataReqProperty.Get(assDataReqStatus);
			    if (error == KErrNone && assDataReqStatus == ELbsTestAGpsModuleAssistanceDataNotExpected)
			        {
		            TTime time;
		                            
		            time.UniversalTime();
		            time += TTimeIntervalMicroSeconds(KAssistanceDataTimeDelay);
		            time += iTimeOut;
		            iDelayUpdateTimer->SetTimer(time);
			        }
				}
				
			// Time stamp the request, to enable assistance data time stamp verfication.
			iAssistanceDataRequestedTime.UniversalTime();
			
			// Return an initial position. Once the assistance data is delivered, an additional update will be returned
			// (read from the update array).
			if(iGpsOptions.GpsMode() == CLbsAdmin::EGpsPreferTerminalAssisted || 
					iGpsOptions.GpsMode() == CLbsAdmin::EGpsAlwaysTerminalAssisted)
				{
				ReturnNanMeas();
				}
			else
				{
				ReturnNanPosition();
				}
			}
		else
			{
			if((iAssistanceDataOn) && (iAssistanceDataAvailable))
				{
					// Force a get assistance data. 
					// Note: we are not verifying anything for now ...
					VerifyRefTimeData();
				}

			// For each location request, we must allways call RequestAssistanceData if data is required or not,
			// when no data is required use EAssistanceDataNone.
			if (!GetOption(ELbsHybridModuleOptions_DisableReqAssistData))
				{
				//Check to see whether the Dynamic AssistanceData flag is enabled, if it is re-request
				// any assistance data we have not yet received (this is required for SUPL)
				if(GetOption(ELbsHybridModuleOptions_DynamicAssistanceData))
					{
					iLocSourceGpsObserver.RequestAssistanceData(iOutstandingAssistanceData);
					}
				else
					{
					iLocSourceGpsObserver.RequestAssistanceData(EAssistanceDataNone);
					}
				}

			// Delay before returning the actual position.
			TTime time;
							
			time.UniversalTime();
			time += TTimeIntervalMicroSeconds(KDelayUpdateTimeDelay);
			time += iTimeOut;
			iDelayUpdateTimer->SetTimer(time);
			}
		}

	// Done processing assistance data or delaying the update, return position.
	else if (aTimerId == KDelayUpdateTimerId)
		{
		// Issue an update.
		UpdateLocation();
		
		// If multi-updating is on, re-issue an update at interval time.
		if (GetOption(ELbsHybridModuleOptions_MultiUpdatesOn) && (iExtraUpdatesCtr > 0))
			{
			TTime time;
			
			time.UniversalTime();
			time += iIntervalDelay;
			
			iDelayUpdateTimer->SetTimer(time);
			iExtraUpdatesCtr--;
			}
		}
	}

void CT_AGpsHybridModule::UpdateLocation()
	{
	LBSTESTLOG_METHOD(OUTBOUND, "CT_AGpsHybridModule::UpdateLocation");

	LBSLOG(ELogP1, "CT_AGpsHybridModule::UpdateLocation()\n");

	// If updates available.
	if (iUpdateArrIndex < iUpdateArr.Count())
		{
		TInt err = KErrNone;
		TPositionInfoBase* deliveryArr[KExpectedNumOfEntries];
		TInt numOfEntries;

		// Configure position info.
		TPositionExtendedSatelliteInfo posInfo = iUpdateArr[iUpdateArrIndex]->Position();

		// Set base class items.
		posInfo.SetModuleId(TUid::Uid(APGS_MODINFO_MODULE_ID));
		posInfo.SetUpdateType(EPositionUpdateGeneral);
		if (iAssistanceDataOn)
			{
			posInfo.SetPositionMode(TPositionModuleInfo::ETechnologyTerminal | TPositionModuleInfo::ETechnologyAssisted);
			}
		else
			{
			posInfo.SetPositionMode(TPositionModuleInfo::ETechnologyTerminal);
			}


		posInfo.SetPositionModeReason(EPositionModeReasonNone);
					
		// Set the position timestamp.
		TPosition position;
		
		posInfo.GetPosition(position);
		position.SetCurrentTime();
		if (iExtraUpdatesCtr) 
			{
			TRealX nan;
			nan.SetNaN();			
			TReal64 nanValue = nan;
			position.SetCoordinate(nanValue, nanValue, nanValue);			
			position.SetAccuracy(nanValue, nanValue);
			}
		posInfo.SetPosition(position);
		
		// Set the satelliteTime.
		TTime timeStamp;
		
		timeStamp.UniversalTime();
		posInfo.SetSatelliteTime(timeStamp);
		
		// Access the measurement.
		TPositionGpsMeasurementInfo measurement = iUpdateArr[iUpdateArrIndex]->Measurement();

		// Determine how and what to deliver.
		if (iImmediateMeasurements)
			{
			measurement.SetPositionCalculationPossible(ETrue);
			}
		if (iGpsOptionsType & ELbsGpsOptionsArrayClass)
			{
			// ensure or warn 2 == iGpsOptions.NumOptionItems()
			numOfEntries = iGpsOptions.NumOptionItems();
			
			if (numOfEntries <= KExpectedNumOfEntries)
				{
				for (TInt i = 0; i < numOfEntries; i++)
					{
					TLbsGpsOptionsItem item;
					
					// Decide which data to deliver, either measurement or position.
					err = iGpsOptions.GetOptionItem(i, item);
					
					// how to deal with errs anywhere...
					if (err == KErrNone)
						{
						switch (item.PosUpdateType())
							{
							case TLbsGpsOptionsItem::EPosUpdateCalculation:
								{
								deliveryArr[i] = &posInfo;
								}
								break;
								
							case TLbsGpsOptionsItem::EPosUpdateMeasurement:
								{
								deliveryArr[i] = &measurement;
								}
								break;
								
							default:
								{
								// err or something
								err = KErrNotSupported;
								}
								break;
							}
						}
					else
						{
						break;
						}
					}
				}
				
			else
				{
				err = KErrNotSupported;
				}
								
			// Process any error. Under error conditions return a valid position only, but report the error.
			if (err)
				{
				numOfEntries = 1;
				deliveryArr[0] = &posInfo;
				}
			}
		else
			{
			numOfEntries = 1;
			if(iGpsOptions.GpsMode() == CLbsAdmin::EGpsPreferTerminalAssisted || 
					iGpsOptions.GpsMode() == CLbsAdmin::EGpsAlwaysTerminalAssisted)
				{ // if in terminal assisted then return measurments
				deliveryArr[0] = &measurement;
				}
			else
				{ // otherwise return positions
				deliveryArr[0] = &posInfo;
				}
			}


		// Return the stored update err, if we encounted no errors.
		if (err == KErrNone)
			{
			err = iUpdateArr[iUpdateArrIndex]->Error();
			}

		// Deliver the required measurement and position data.
		iLocSourceGpsObserver.UpdateLocation(err, const_cast<const TPositionInfoBase**>(&deliveryArr[0]), numOfEntries, iTargetTime);

		// Log update data.
		DumpUpdateToRefFile(const_cast<const TPositionInfoBase**>(&deliveryArr[0]), numOfEntries);

		// Select next update, for future request.
		if (!iExtraUpdatesCtr) 
			{
			iUpdateArrIndex++;
			}
		}

	// We have no more updates to return.
	else
		{
		iLocSourceGpsObserver.UpdateLocation(KErrGeneral, NULL, 0, iTargetTime);
		}
	}


void CT_AGpsHybridModule::ReturnNanPosition()
	{
	LBSTESTLOG_METHOD(OUTBOUND, "CT_AGpsHybridModule::ReturnNanPosition");

	LBSLOG(ELogP1, "CT_AGpsHybridModule::ReturnNanPosition()\n");

	// Module is in the in-correct state, for this call. Should be in prefer terminal, and non-hybrid mode.
	if (iAssistanceDataOn == EFalse)
		{
		User::Invariant();
		}

	// Configure position.
	TRealX nan;
	nan.SetNaN();
	TReal64 nanValue = nan;
			
	TPosition pos;
	TCourse course;
	TDetailedErrorReport detailedErrorReport;
	TGpsTimingMeasurementData gpsTimingData;
	TPositionExtendedSatelliteInfo posInfo;
	
	pos.SetCoordinate(nanValue, nanValue, nanValue);			
	pos.SetAccuracy(nanValue, nanValue);
	pos.SetCurrentTime();
	
	posInfo.SetPosition(pos);

	course.SetSpeed(nanValue);
    course.SetVerticalSpeed(nanValue);
    course.SetHeading(nanValue);
    course.SetSpeedAccuracy(nanValue);
    course.SetVerticalSpeedAccuracy(nanValue);
    course.SetHeadingAccuracy(nanValue);
    course.SetCourse(nanValue);
    course.SetCourseAccuracy(nanValue);
    posInfo.SetCourse(course);
    
    // Set timing assistance measurement data
    gpsTimingData.SetDataType(TGpsTimingMeasurementData::EGpsTimingDataTypeUtran);
    gpsTimingData.SetNetworkMode(TGpsTimingMeasurementData::ENetworkModeFdd);
    gpsTimingData.SetGPSTimingOfCellMsPart(nanValue);
    gpsTimingData.SetGPSTimingOfCellLsPart(nanValue);
    gpsTimingData.SetReferenceIdentity(nanValue);
    gpsTimingData.SetSfn(nanValue);
    posInfo.SetGpsTimingData(gpsTimingData);
    
    
    //Set GNSS Pseudorange Error Statistics 
    detailedErrorReport.SetStanDeviOfLongitudeError(nanValue);
    detailedErrorReport.SetStanDeviOfLatiitudeError(nanValue);
    detailedErrorReport.SetStanDeviOfAltitudeError(nanValue);
    detailedErrorReport.SetStanDeviOfSemiMajorAxisError(nanValue);
    detailedErrorReport.SetStanDeviOfSemiMinorAxisError(nanValue);
    detailedErrorReport.SetOrientationOfSemiMajorAxisError(nanValue);
    detailedErrorReport.SetRmsValOfStanDeviOfRange(nanValue);
    posInfo.SetDetailedErrorReport(detailedErrorReport);
    // Set Geoidal separation
    posInfo.SetGeoidalSeparation(nanValue);
    //Set Magnetic variation
    posInfo.SetMagneticVariation(nanValue);
    //Set Course over ground
    posInfo.SetCourseOverGroundMagnetic(nanValue);
	// Configure position info.

	// Set base class items.
	posInfo.SetModuleId(TUid::Uid(APGS_MODINFO_MODULE_ID));
	posInfo.SetUpdateType(EPositionUpdateGeneral);
	posInfo.SetPositionMode(TPositionModuleInfo::ETechnologyTerminal | TPositionModuleInfo::ETechnologyAssisted);
	posInfo.SetPositionModeReason(EPositionModeReasonNone);

	// Set the satelliteTime.
	TTime timeStamp;	
	timeStamp.UniversalTime();

	posInfo.SetSatelliteTime(timeStamp);


	// Deliver the position data.
	TInt err = KErrNone;
	TPositionInfoBase* deliveryArr[KExpectedNumOfEntries];
		
	deliveryArr[0] = &posInfo;

	iLocSourceGpsObserver.UpdateLocation(err, const_cast<const TPositionInfoBase**>(&deliveryArr[0]), 1, iTargetTime);

	// Log update data.
	DumpUpdateToRefFile(const_cast<const TPositionInfoBase**>(&deliveryArr[0]), 1);
	}

void CT_AGpsHybridModule::ReturnNanMeas()
	{
	LBSTESTLOG_METHOD(OUTBOUND, "CT_AGpsHybridModule::ReturnNanMeasurement");

	LBSLOG(ELogP1, "CT_AGpsHybridModule::ReturnNanMeasurement()\n");	
	
	TRealX nan;
	nan.SetNaN();
	TReal64 nanValue = nan;
		
	TPositionGpsMeasurementData measureData;	
	TPositionGpsMeasurementInfo measurement;
	
	measureData.SetSatelliteId(nan);
	measureData.SetCarrierNoiseRatio(nan);
	measureData.SetDoppler(nan);
	measureData.SetWholeGpsChips(nan);
	measureData.SetFractionalGpsChips(nan);
	measureData.SetMultiPathIndicator(TPositionGpsMeasurementData::EMultiPathLow);
	measureData.SetPseudoRangeRmsError(nan);

	User::LeaveIfError(measurement.AppendMeasurementData(measureData));
	measurement.SetGpsTimeOfWeek(nan);

	// Deliver the position data.
	TInt err = KErrNone;
	TPositionInfoBase* deliveryArr[KExpectedNumOfEntries];
		
	deliveryArr[0] = &measurement;

	iLocSourceGpsObserver.UpdateLocation(err, const_cast<const TPositionInfoBase**>(&deliveryArr[0]), 1, iTargetTime);

	// Log update data.
	DumpUpdateToRefFile(const_cast<const TPositionInfoBase**>(&deliveryArr[0]), 1);
	}


TInt CT_AGpsHybridModule::ProcessRequestUpdateInit(const TDesC& aConfigFileName, const TDesC& aConfigSection)
	{
	LBSLOG(ELogP1, "CT_AGpsHybridModule::ProcessRequestConfigL()\n");

	T_LbsUtils utils;

	TInt err = KErrNone;
	// Reset the update array, before we re-assign new updates.
	iUpdateArr.ResetAndDestroy();
	iUpdateArrIndex = 0;

	// Read the configuration ini file to update the update data items.
	TRAP(err, utils.GetConfigured_UpdateArrayL(aConfigFileName, aConfigSection, iUpdateArr));
						
	// NOTE: in the old module we used to reset everything, this maybe required here also.

	return err;
	}
 

// We're being asked to update the current time out value.
void CT_AGpsHybridModule::ProcessRequestTimeOut(const TTimeIntervalMicroSeconds& aTimeOut)
	{
	iTimeOut = aTimeOut;
	}
			
// We're being asked to update the current error
void CT_AGpsHybridModule::ProcessRequestError(TInt aError)
	{
	iError = aError;	
	}

void CT_AGpsHybridModule::ProcessImmediateMeasurements(TInt aImmediateMeasurements)
	{
	iImmediateMeasurements = aImmediateMeasurements;	
	}


// We're being asked to update  the test module options
TInt CT_AGpsHybridModule::ProcessRequestModuleOptions(TLbsHybridModuleOptions aModuleOption, TBool aValue)
	{
	TInt err = KErrNone;
	
	//Reset the assistance Data the may have previously been requested
	ClearAssistanceData();

	switch (aModuleOption)
		{
		case EHybridModuleOptions_ClearAll:
		case ELbsHybridModuleOptions_SetAll:
			{
			// We're being asked to update all the test module options
			iModuleOptions = aModuleOption;
			break;				
			}
		case ELbsHybridModuleOptions_AssistanceDataOn:
			{
			// We're being asked to update a particular test mode options
			SetOption(aModuleOption, aValue);	
			break;				
			}
		case ELbsHybridModuleOptions_MultiUpdatesOn:
			{
			// We're being asked to update a particular test mode options
			SetOption(aModuleOption, aValue);	
			// If MultiUpdates is being switch off reset the counter
			if (!aValue)
				{
				iExtraUpdatesCtr = 0;	
				}
			break;		
			}
		case ELbsHybridModuleOptions_DisableReqAssistData:
			{
			// We're being asked to update a particular test mode options
			SetOption(aModuleOption, aValue);	
			break;
			}
		case ELbsHybridModuleOptions_DynamicAssistanceData:
			{
			//Turn on dynamic assistance data to always re-request assistance data we have not received
			SetOption(aModuleOption, aValue);
			break;
			}
		default:
			{
			err = KErrNotSupported;
			break;				
			}
		}

	return err;
	}

/** Force the agps module to send the next update in the array to LBS.

This function is used to force an update even if there is no outstanding
request from LBS.
*/
void CT_AGpsHybridModule::ProcessRequestForcedUpdate()
	{
	UpdateLocation();
	}

/** Verify the reference time assistance data is present and the time stamp is correct.

@return ETrue if data is as expected.
*/
TBool CT_AGpsHybridModule::VerifyRefTimeData()
	{
	TInt err;
	TTime timeStamp;
	TBool verified = EFalse;

	// Get the assistance data - gps ref time.
	RUEPositioningGpsReferenceTimeReader gpsRefTimeReader;
	
	// Open the reader.
	TRAP(err, gpsRefTimeReader.OpenL());
	if (err == KErrNone)
		{
		// Get data.
		err = iLocSourceGpsObserver.GetAssistanceDataItem(EAssistanceDataReferenceTime, gpsRefTimeReader, timeStamp);
		if (err == KErrNone)
			{
			// Do the Verify.
			T_LbsUtils utils;
	
			verified = utils.VerifySimpleAssistanceData(gpsRefTimeReader);
			}

		// Close reader.
		gpsRefTimeReader.Close();
		}

	// Verify the time stamp.
	if (verified)
		{
		TTime minTime = iAssistanceDataRequestedTime;
		TTime maxTime;

		maxTime.UniversalTime();
		if((timeStamp < minTime) || (timeStamp > maxTime))
			{
			verified = EFalse;
			}
		}

	return verified;
	}



/** Enable or disable a particular module option.
*/
void CT_AGpsHybridModule::SetOption(TLbsHybridModuleOptions aModuleOption, TBool aValue)
	{
	if (aValue)
		{
		iModuleOptions |= aModuleOption; // Set bit.
		}
		
	else
		{
		iModuleOptions &= ~aModuleOption; // Clear bit.
		}
	}


/** Determine if a particular module option is enabled.
*/	
TBool CT_AGpsHybridModule::GetOption(TLbsHybridModuleOptions aModuleOption)
	{
	if (aModuleOption & iModuleOptions)
		{
		return ETrue;
		}
	return EFalse;
	}

/** Dump update to a log file (useful for test verification required).
*/	
void CT_AGpsHybridModule::DumpUpdateToRefFile(const TPositionInfoBase *aPosInfoArray[], TInt aNumItems)
	{
	LBSDUMP("New Update:\n");	

	for (TInt i = 0; i < aNumItems; i++)
		{
		if (aPosInfoArray[i]->PositionClassType() & EPositionSatelliteInfoClass)
			{
			LBSDUMP("   TPositionExtendedSatelliteInfo\n");	
			TPosition position;
			const TPositionInfo* posInfo = reinterpret_cast<const TPositionInfo*>(aPosInfoArray[i]);
			posInfo->GetPosition(position);
			LBSDUMP2("\tHorizontalAccuracy: %f\n", position.HorizontalAccuracy());
			LBSDUMP2("\tVerticalAccuracy: %f\n", position.VerticalAccuracy());
			LBSDUMP2("\tLatitude: %f\n", position.Latitude());
			LBSDUMP2("\tLongitude: %f\n", position.Longitude());
			LBSDUMP2("\tAltitude: %f\n", position.Altitude());
			LBSDUMP2("\tTimeStamp: %d\n", position.Time().Int64());
			}						

		if (aPosInfoArray[i]->PositionClassType() & EPositionGpsMeasurementInfoClass)
			{
			LBSDUMP("   TPositionGpsMeasurementInfo\n");	
			const TPositionGpsMeasurementInfo* measurement = reinterpret_cast<const TPositionGpsMeasurementInfo*>(aPosInfoArray[i]);
			TInt numMeasurements = measurement->NumMeasurements();
			
			for(TInt n = 0; n < numMeasurements; n++)
				{
				TPositionGpsMeasurementData measurementData;
				measurement->GetMeasurementData(n, measurementData);
				LBSDUMP2("      SatelliteId: %d\n", measurementData.SatelliteId());
				LBSDUMP2("\tCarrierNoiseRatio: %d\n", measurementData.CarrierNoiseRatio());
				LBSDUMP2("\tDoppler: %d\n", measurementData.Doppler());
				LBSDUMP2("\tWholeGpsChips: %d\n", measurementData.WholeGpsChips());
				LBSDUMP2("\tFractionalGpsChips: %d\n", measurementData.FractionalGpsChips());
				LBSDUMP2("\tMultiPathIndicator: %d\n", measurementData.MultiPathIndicator());
				LBSDUMP2("\tPseudoRangeRmsError: %u\n", measurementData.PseudoRangeRmsError());
				}							
			}				
		}
	}

void CT_AGpsHybridModule::PublishGpsOptions(const TLbsGpsOptions& aGpsOptions)
	{
	const TLbsGpsOptionsArray& optionsArr = reinterpret_cast<const TLbsGpsOptionsArray&>(aGpsOptions);
	TPckgC<TLbsGpsOptionsArray> pckgOptions(optionsArr);
	RProperty::Set(KUidSystemCategory, ELbsTestAGpsModuleModeChanges, pckgOptions);
	}

void CT_AGpsHybridModule::SignalResetAssitanceDataReceived()
	{
	RProperty::Set(KUidSystemCategory, ELbsTestAGpsModuleResetAssistanceDataFlag, ELbsTestAGpsResetAssistanceDataReceived);
	}

/* Clears the assistance data flag and sets the outstanding assistance data back
 	to what was originally requested. This should be set back to the types of
 	assistance data you require*/
void CT_AGpsHybridModule::ClearAssistanceData()
	{
	iAssistanceDataAvailable = EFalse;
	iAssistanceDataRequested = EFalse;
	//Setup the required assistance data here
	iOutstandingAssistanceData = EAssistanceDataReferenceTime;
	}

