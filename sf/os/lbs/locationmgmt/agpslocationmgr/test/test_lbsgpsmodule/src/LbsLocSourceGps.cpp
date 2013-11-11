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

#include <lbs/lbsextendedsatellite.h>
#include "LbsLocSourceGps.h"
#include "lbstimer.h"
#include "lbsdevloggermacros.h"

#include <lbs/lbsassistancealmanac.h>
#include <lbs/lbsassistancereferencetime.h>
#include <lbs/lbsassistanceaquisitionassistance.h>
#include <lbs/lbsgpsmeasurement.h>
#include <lbs/lbsadmin.h>
#include <lbs/lbslocdatasourceclasstypes.h>

//TTimeIntervalMicroSeconds
const TInt64 KDelayLocReqToAssDataReq = 400000;//0.4s
const TInt64 KDelayAssDataReqToLocUpd = 800000;//0.8s Added to TargetTime
const TReal KModuleAccuracy = 50; //Used when reporting Location Updates
const TInt KModuleSatelliteId = 101; // Randomly selected value for reporting measurment updates


CLbsLocationSourceGps::CLbsLocationSourceGps(MLbsLocationSourceGpsObserver& aObserver) :
	CLbsLocationSourceGpsBase(aObserver), iPendingAction(EActionNone)
	{
	}


EXPORT_C CLbsLocationSourceGpsBase* CLbsLocationSourceGps::NewL
	(
	MLbsLocationSourceGpsObserver& aObserver
	)
	{
	CLbsLocationSourceGps* self = new(ELeave) CLbsLocationSourceGps(aObserver);

	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);

	return self;
	}

   	
void CLbsLocationSourceGps::ConstructL()
	{
	CLbsLocationSourceGpsBase::ConstructL();
	iTestChannel.OpenL();
	iTestChannel.SetMsg(RAgpsModuleTestChannel::ETestMsgNewL);
	iTimer = CLbsCallbackTimer::NewL(*this);
	
	iLocSourceGpsObserver.UpdateDeviceStatus(TPositionModuleStatus::EDeviceUnknown);
	iLocSourceGpsObserver.UpdateDataQualityStatus(TPositionModuleStatus::EDataQualityUnknown);
	
    // Define a property that allows the test step to specify the return error code 
    // wanted for the location update. Initialize that property to zero (KErrNone).
	const TSecurityPolicy KReadPolicy(ECapability_None);
    const TSecurityPolicy KWritePolicy(ECapability_None);
    const TInt KUpdateCodePropertyKey = 0x11110100; // made up. To be used by test step. Should go in some header.
    RProcess process;
    RProperty::Define(KUpdateCodePropertyKey, RProperty::EInt, KReadPolicy, KWritePolicy);
    User::LeaveIfError(iUpdateCodeProperty.Attach(process.SecureId(), 
	                                             KUpdateCodePropertyKey));     
	__ASSERT_DEBUG(iUpdateCodeProperty.Handle() != NULL, User::Invariant());
	// Set initial property value
	iUpdateCodeProperty.Set(0);                                         
    const TInt KUpdateFixTimeKey = 0x11110101; // made up. To be used by test step. Should go in some header.
    TInt err = RProperty::Define(KUpdateFixTimeKey, RProperty::EInt, KReadPolicy, KWritePolicy);
    User::LeaveIfError(iUpdateFixTimeProperty.Attach(process.SecureId(), 
	                                             KUpdateFixTimeKey));     
	__ASSERT_DEBUG(iUpdateFixTimeProperty.Handle() != NULL, User::Invariant());
	// Set initial property value
	err = iUpdateFixTimeProperty.Set(0);
	

	}

CLbsLocationSourceGps::~CLbsLocationSourceGps()
	{
	iTestChannel.Close();
	iTimer->Cancel();
	delete iTimer;
	}	
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------


void CLbsLocationSourceGps::SetGpsOptions(const TLbsGpsOptions& aGpsOptions)
	{
	
	// Line commented out because there are currently no tests expecting
	// this message, and as it is sometimes causing problems (by
	// overwritting other messages published on the same property that 
	// are expected by tests)	
	// iTestChannel.SetMsg(RAgpsModuleTestChannel::ETestMsgSetGpsOptions);
	CLbsAdmin::TGpsMode mode = aGpsOptions.GpsMode();
   	
	if (aGpsOptions.ClassType() == ELbsGpsOptionsClass)
 		{
 		iHybridMode = EFalse;
		CLbsAdmin::TGpsMode mode = aGpsOptions.GpsMode();

   		switch (mode)
   			{
   			case CLbsAdmin::EGpsAutonomous:
   		   		// for testing purposes callback to Manager that device is initialising...
				iLocSourceGpsObserver.UpdateDeviceStatus(TPositionModuleStatus::EDeviceInitialising);
   			break;
   			default:
				iLocSourceGpsObserver.UpdateDeviceStatus(TPositionModuleStatus::EDeviceInactive);
   			break;
   			}
 		}
 	else if (aGpsOptions.ClassType() == ELbsGpsOptionsArrayClass)
 		{
 		iHybridMode = ETrue;
 		}
	}


//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void CLbsLocationSourceGps::RequestLocationUpdate
	(
	const TTime& aTargetTime,
	const TLbsLocRequestQuality& aQuality)
	{
	LBSLOG(ELogP1, "CLbsLocationSourceGps::RequestLocationUpdate - Enter");
	
	if(iPendingAction==EActionShutdownModule)
		{
		LBSLOG(ELogP1, "CLbsLocationSourceGps::RequestLocationUpdate - Exit");
		return;
		}
	
	iTestChannel.SetMsg(RAgpsModuleTestChannel::ETestMsgRequestLocationUpdate);
	// Set the fix time property
	TInt timeInt = I64INT(aQuality.MaxFixTime().Int64());
	TInt err = iUpdateFixTimeProperty.Set(timeInt);
	// Ignore error. Ultimately, if this fails the current test case will fail

	LBSLOG(ELogP2, "Location update request from the manager");
	TDateTime time = aTargetTime.DateTime();
	LBSLOG5(ELogP2, "   TargetTime:   %d:%d:%d.%d", time.Hour(), time.Minute(), time.Second(), time.MicroSecond());
	LBSLOG2(ELogP2, "   Max Fix Time: %ld", aQuality.MaxFixTime().Int64());
	LBSLOG3(ELogP2, "   Accuracy H/V: %f/%f", aQuality.MinHorizontalAccuracy(), aQuality.MinVerticalAccuracy());
	
	
	iTargetTime = aTargetTime;
	iTimer->Cancel();
	iTimer->EventAfter(static_cast<TTimeIntervalMicroSeconds>(KDelayLocReqToAssDataReq), EActionRequestAssistanceData);
	iPendingAction = EActionRequestAssistanceData;
	LBSLOG(ELogP1, "CLbsLocationSourceGps::RequestLocationUpdate - Exit");
	}
   	
void CLbsLocationSourceGps::CancelLocationRequest()
	{
	LBSLOG(ELogP1, "CLbsLocationSourceGps::CancelLocationRequest - Enter");
	if(iPendingAction==EActionShutdownModule)
		{
		LBSLOG(ELogP1, "CLbsLocationSourceGps::RequestLocationUpdate - Exit");
		return;
		}
	
	iTestChannel.SetMsg(RAgpsModuleTestChannel::ETestMsgCancelLocationRequest);
	iTimer->Cancel();
	iPendingAction = EActionNone;

   	// for testing purposes callback to Manager that device is on standby...
	iLocSourceGpsObserver.UpdateDeviceStatus(TPositionModuleStatus::EDeviceStandBy);
	
	if (iHybridMode)
		{
		// While in hybrid mode, a change to the Data Quality Status will let the test
		// know that cancel (this method) has been called in GPS Integration Module.
		//
		iLocSourceGpsObserver.UpdateDataQualityStatus(TPositionModuleStatus::EDataQualityPartial);
		}
 
	LBSLOG(ELogP1, "CLbsLocationSourceGps::CancelLocationRequest - Exit");
	}

static const TPositionModuleStatus::TDeviceStatus KModuleStatusTable[8] = 
	{
	TPositionModuleStatus::EDeviceUnknown,		//  EDeviceUnknown,		-> EStatusOff
	TPositionModuleStatus::EDeviceInitialising,	//  EDeviceInitialising -> EStatusOn
	TPositionModuleStatus::EDeviceError,		//  EDeviceError 	 	-> EStatusOff
	TPositionModuleStatus::EDeviceStandBy,		//  EDeviceStandBy		-> EStatusOn
	TPositionModuleStatus::EDeviceDisabled,		//  EDeviceDisabled 	-> EStatusOff
	TPositionModuleStatus::EDeviceReady, 		//  EDeviceReady		-> EStatusOn
	TPositionModuleStatus::EDeviceInactive, 	//  EDeviceInactive 	-> EStatusOff	
 	TPositionModuleStatus::EDeviceActive		//  EDeviceActive 		-> EStatusActive
	};

void CLbsLocationSourceGps::AdvisePowerMode(CLbsLocationSourceGpsBase::TPowerMode aMode)
	{
	LBSLOG(ELogP1, "CLbsLocationSourceGps::AdvisePowerMode - Enter");
	iTestChannel.SetMsg(RAgpsModuleTestChannel::ETestMsgAdvisePowerMode);

	const TUid KNetworkGatewayUid = TUid::Uid(0x10281D46);
	 
  	switch (aMode)  
  	{
  		case EPowerModeUnknown:
  			{

  			break;
  			}
		case EPowerModeOn:
			{
			RUEPositioningGpsReferenceTimeBuilder	gpsReferenceTimeBuilder;
 			RUEPositioningGpsReferenceTimeReader    gpsReferenceTimeReader;
 
			gpsReferenceTimeBuilder.OpenL();
			gpsReferenceTimeReader.OpenL();
		 		
		 	const TUint testRefTimeWeek = 12;		
			const TUint testRefTimeTow1MSec = 567;	

   			//gpsReferenceTimeBuilder.SetField(TUEPositioningGpsReferenceTime::EGpsWeek,
            //                       testRefTimeWeek);
    		//gpsReferenceTimeBuilder.SetField(TUEPositioningGpsReferenceTime::EGpsTow1Msec, 
            //                         testRefTimeTow1MSec);
    		//gpsReferenceTimeReader.DataBuffer() = gpsReferenceTimeBuilder.DataBuffer();
    	
    		const TInt KTestDay = 13;
			const TMonth KTestMonth = EApril;
			const TInt KTestYear = 2006;

    		TDateTime dateTime;
    		dateTime.SetDay(KTestDay);
    		dateTime.SetMonth(KTestMonth);
    		dateTime.SetYear(KTestYear);
    		TTime timeStamp(dateTime);
    
			
			TBool passed = TRUE;
			
			iLocSourceGpsObserver.GetAssistanceDataItem(EAssistanceDataReferenceTime, 
	                                            gpsReferenceTimeReader, timeStamp);                                 
        	TUint week;
        	TUint tow1MSec;	                                            
        	gpsReferenceTimeReader.GetField(TUEPositioningGpsReferenceTime::EGpsTow1Msec,tow1MSec);
        	gpsReferenceTimeReader.GetField(TUEPositioningGpsReferenceTime::EGpsWeek,week);
			
			// Are these the values that were set by the module test program?
			passed &= (tow1MSec == testRefTimeTow1MSec);
			passed &= (week == testRefTimeWeek);
			gpsReferenceTimeBuilder.Close();
       		gpsReferenceTimeReader.Close();
                
   			if (passed)
   				{
   				iLocSourceGpsObserver.UpdateDeviceStatus(TPositionModuleStatus::EDeviceReady);
   				}
   			else
   				{
   				// return EDeviceActive if assisatnce data NOT OK.
   				iLocSourceGpsObserver.UpdateDeviceStatus(TPositionModuleStatus::EDeviceActive);
   				}
			break;
			}
   		case EPowerModeStandby:
 			{
 			// for testing purposes callback to Manager that device is in standby...
     		iLocSourceGpsObserver.UpdateDeviceStatus(TPositionModuleStatus::EDeviceStandBy);
   			break;
 			}
   		case EPowerModeOff:
   			{
 			// for testing purposes callback to Manager that device is inactive...
     		iLocSourceGpsObserver.UpdateDeviceStatus(TPositionModuleStatus::EDeviceInactive);
   			break;
   			}

		case EPowerModeClose:
			{
			if (iDeviceStatusIndex < 8)
				{
				iLocSourceGpsObserver.UpdateDeviceStatus(KModuleStatusTable[iDeviceStatusIndex]);
				iDeviceStatusIndex++;
				}
			else
				{
			
				// Wait for 5 seconds before allowing the module to shutdown
				iTimer->Cancel();
				iTimer->EventAfter(static_cast<TTimeIntervalMicroSeconds>(5000000), EActionShutdownModule);
				iPendingAction = EActionShutdownModule;
				}
			break;
			}
  		default:
  			break;
  		
  	}
  
      
	LBSLOG(ELogP1, "CLbsLocationSourceGps::AdvisePowerMode - Exit");
	}


void CLbsLocationSourceGps::AssistanceDataEvent(TInt aError, TLbsAsistanceDataGroup aDataMask)
	{
	if(iPendingAction==EActionShutdownModule)
		{
		return;
		}
	
	iTestChannel.SetMsg(RAgpsModuleTestChannel::ETestMsgAssistanceDataEvent);
	
	// for testing purposes callback to Manager that device is ready...
	const TLbsAsistanceDataGroup KReceivedDataMask = EAssistanceDataReferenceTime | 
                                                 EAssistanceDataAlmanac;

	if ((aError == KErrNone) && (aDataMask==KReceivedDataMask))
		{
		iLocSourceGpsObserver.UpdateDeviceStatus(TPositionModuleStatus::EDeviceReady);
		}
	else
		{	
		iLocSourceGpsObserver.UpdateDeviceStatus(TPositionModuleStatus::EDeviceActive);
		
		iTimer->Cancel();
		iTimer->EventAfter(static_cast<TTimeIntervalMicroSeconds>(100000), EActionPublishLocationUpdate);
		iPendingAction = EActionPublishLocationUpdate;
		}
	}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void CLbsLocationSourceGps::HandleLocationUpdate(const TPositionInfoBase& aPosInfo)
	{
	const TPositionInfoBase* posInfo = &aPosInfo;
	iLocSourceGpsObserver.UpdateLocation(KErrNone, &posInfo, 1, 0);
	}
	
void CLbsLocationSourceGps::HandleDeviceStatusUpdate(TPositionModuleStatus::TDeviceStatus aDeviceStatus)
	{
	iLocSourceGpsObserver.UpdateDeviceStatus(aDeviceStatus);
	}

void CLbsLocationSourceGps::HandleQualityStatusUpdate(TPositionModuleStatus::TDataQualityStatus aDataQuality)
	{
	iLocSourceGpsObserver.UpdateDataQualityStatus(aDataQuality);
	}

void CLbsLocationSourceGps::Shutdown()
	{
	TTime time;
	time.UniversalTime();
	RDebug::Printf("%02d:%02d:%02d:%06d  [%Ld] xxxxx CLbsLocationSourceGps::Shutdown",
		time.DateTime().Hour(), time.DateTime().Minute(), time.DateTime().Second(),time.DateTime().MicroSecond(),
		RProcess().Id().Id());		
	
	iTestChannel.SetMsg(RAgpsModuleTestChannel::ETestMsgShutdownEvent);
	iLocSourceGpsObserver.Shutdown();
	}

void CLbsLocationSourceGps::OnTimerEventL(TInt aTimerId)
	{
	TPositionExtendedSatelliteInfo satInfo;
	TPositionGpsMeasurementInfo measurementInfo;
	TPositionGpsMeasurementData measuredData;
	const TPositionInfoBase* aPosInfoArray[2];
	TPosition pos;
	TInt numElementsArray;
	
	switch (aTimerId)
		{
		case EActionRequestAssistanceData:
			iTimer->EventAtUTC(iTargetTime + static_cast<TTimeIntervalMicroSeconds>(KDelayAssDataReqToLocUpd), EActionPublishLocationUpdate);
			iLocSourceGpsObserver.RequestAssistanceData(0);
			iPendingAction = EActionPublishLocationUpdate;
			break;
			
		case EActionPublishLocationUpdate:
			LBSLOG(ELogP2, "Location update from the AGPS test module");
			aPosInfoArray[0] = &satInfo;
			
			pos.SetHorizontalAccuracy(KModuleAccuracy);
			pos.SetVerticalAccuracy(KModuleAccuracy);
			satInfo.SetPosition(pos);
			satInfo.SetSatelliteTime(iTargetTime);

			if (iHybridMode)
				{
				measuredData.SetSatelliteId(KModuleSatelliteId);
				measurementInfo.AppendMeasurementData(measuredData);
				aPosInfoArray[1] = &measurementInfo;
				numElementsArray = 2;
				}
			else
				{
				aPosInfoArray[1] = NULL;
				numElementsArray = 1;
				}
			
			// Get the error code for the location update from the corresponding property (the test step
			// has set this)
			TInt errorCode;

			iUpdateCodeProperty.Get(errorCode);
			iLocSourceGpsObserver.UpdateLocation(errorCode, aPosInfoArray, numElementsArray, iTargetTime);
			break;
			
		case EActionShutdownModule:
			//Call shutdown on the Manager to let it know we are ready to close
			Shutdown();
			break;
		
		default:
			User::Invariant();
			break;
		}
	}
	
TInt CLbsLocationSourceGps::OnTimerError(TInt /*aTimerId*/, TInt aError)
	{
	return aError;
	}
