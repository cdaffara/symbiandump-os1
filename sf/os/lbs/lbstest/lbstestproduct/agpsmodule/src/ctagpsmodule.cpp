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
// This is the cpp file which contains the
//

//! @file ctapgsmodule.cpp

// User includes
#include "ctagpsmodule.h"

// Epoc includes
#include <lbs/lbsassistancereferencetime.h>
#include <lbs/lbsassistancereferencelocation.h>

#include <lbssatellite.h>
#include <lbs/test/lbsnetsimtest.h>
#include <lbserrors.h>
#include "lbsdevloggermacros.h"

// Literals Used
const TLbsAssistanceDataGroup KAssistanceDataUndefined = 0x00000200;
const TInt KAssistanceDataTimeDelay = 1000000;
const TInt KDelayUpdateTimeDelay = 500000;
const TInt KDelayStatusTime = 2 * 1000 * 1000;

CT_AGpsModule::CT_AGpsModule(MLbsLocationSourceGpsObserver& aObserver)
	:
	CLbsLocationSourceGpsBase(aObserver)
	{
	}


CLbsLocationSourceGpsBase* CT_AGpsModule::NewL(MLbsLocationSourceGpsObserver& aObserver)
	{
	CT_AGpsModule* self = new(ELeave) CT_AGpsModule(aObserver);

	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);

	return self;
	}


void CT_AGpsModule::ConstructL()
	{
	LBSLOG(ELogP1, "CT_AGpsModule::ConstructL()\n");

	CLbsLocationSourceGpsBase::ConstructL();
	
	// Create position update and assistance data timers.
	iPosUpdateTimer = CT_LbsTimerUtils::NewL(this, KPosUpdateTimerId);
	iDelayUpdateTimer = CT_LbsTimerUtils::NewL(this, KDelayUpdateTimerId);	
	iDelayStatusTimer = CT_LbsTimerUtils::NewL(this, KDelayStatusTimerId);	

	// Create and start the module data bus monitor, to listen for incoming data.	
	iModuleDataBusMonitor = CT_AGpsModuleDataBusMonitor::NewL(this);
	iModuleDataBusMonitor->StartMonitorL();

	iPosInfoIndex = 0;

	// On module startup select normal test mode - assistance data on.	
	iAssDataEventType = TModuleDataIn::EAssDataEventExpectSingle;
	iAssDataTestMode = TModuleDataIn::EModuleTestModeAssDataOn;
	iOptionsChangeReportMode = TModuleDataIn::EGpsModuleDoNotReportOptionsChange;

	iModuleStatus.SetDeviceStatus(TPositionModuleStatus::EDeviceUnknown);
	iModuleStatus.SetDataQualityStatus(TPositionModuleStatus::EDataQualityUnknown);
	//Set default error code
	iError = KErrNone;
	}


CT_AGpsModule::~CT_AGpsModule()
	{
	LBSLOG(ELogP1, "CT_AGpsModule::~CT_AGpsModule()\n");
	
	T_LbsUtils utils;
	
	utils.ResetAndDestroy_PosInfoArr(iPosInfoArr);

	iModuleDataBusMonitor->CancelMonitor();
	delete iModuleDataBusMonitor;
	
	delete iDelayStatusTimer;
	delete iDelayUpdateTimer;
	delete iPosUpdateTimer;
	}	


void CT_AGpsModule::SetGpsOptions(const TLbsGpsOptions& aGpsOptions)
	{
	LBSLOG(ELogP1, "CT_AGpsModule::SetGpsOptions()\n");
	LBSLOG2(ELogP1, "\t\taGpsOptions = %d\n", aGpsOptions.GpsMode());

	iGpsOptions = aGpsOptions;
	
	// Return the answer to the test step if required by test underway
	if (iOptionsChangeReportMode == TModuleDataIn::EGpsModuleDoReportOptionsChange)
		{
		TPckgBuf<TModuleDataOut> modDataOutBuf;
		TModuleDataOut& modDataOut = modDataOutBuf();

		modDataOut.iRequestType = TModuleDataOut:: EModuleGpsMode;
		modDataOut.iGpsMode = aGpsOptions.GpsMode();   
			   
		LBSLOG2(ELogP2, "CT_AGpsModule::SetGpsOptions() sending notification of options change to gps mode %d\n", modDataOut.iGpsMode);
		TInt err;
		err = RProperty::Set(KUidSystemCategory, ELbsTestAGpsModuleOut, modDataOutBuf);
		User::LeaveIfError(err);
		User::After( 200 * 1000);
		}
	}


void CT_AGpsModule::RequestLocationUpdate(const TTime& aTargetTime, const TLbsLocRequestQuality& aQuality)
	{
	LBSLOG(ELogP1, "CT_AGpsModule::RequestLocationUpdate()\n");
	
	if(iOneOffTimeOut && iFirstRequestPast)
		{
		iTimeOut = 0;
		iOneOffTimeOut = EFalse;
		}
	iFirstRequestPast = ETrue;
	
	// If there is currently a position request outstanding, cancel it.
	if (iPosUpdateTimer->IsActive())
		{
		iPosUpdateTimer->Cancel();
		}									
	if (iDelayUpdateTimer->IsActive())
		{
		iDelayUpdateTimer->Cancel();
		}
	if (iDelayStatusTimer->IsActive())
		{
		iDelayStatusTimer->Cancel();
		}
		
	// Response with update at the required target time + any time out period. The time out allows
	// the module to delay return giving a position update, normally it will be set to zero.
	if (aTargetTime == TTime(0))
		{
		iTargetTime.UniversalTime();
		iTargetTime += iTimeOut;
		}
	else
		{	
		iTargetTime = aTargetTime + iTimeOut;
		}

	TDateTime tTime = iTargetTime.DateTime();
	LBSLOG5(ELogP4, "CT_AGpsModule iTargetTime = %d:%d:%d.%d", tTime.Hour(), tTime.Minute(), tTime.Second(), tTime.MicroSecond());
	iPosUpdateTimer->SetTimer(iTargetTime);

	// what do we do with quality
	(void)aQuality;
	}


void CT_AGpsModule::CancelLocationRequest()
	{
	LBSLOG(ELogP1, "CT_AGpsModule::CancelLocationRequest()\n");	
	
	iPosUpdateTimer->CancelTimer();
	iDelayUpdateTimer->CancelTimer();
	iDelayStatusTimer->CancelTimer();
	}


void CT_AGpsModule::AdvisePowerMode(CLbsLocationSourceGpsBase::TPowerMode aMode)
	{
	LBSLOG(ELogP1, "CT_AGpsModule::AdvisePowerMode()\n");

	if (aMode == CLbsLocationSourceGpsBase::EPowerModeClose)
		{
		iLocSourceGpsObserver.Shutdown();
		}
	}


void CT_AGpsModule::AssistanceDataEvent(TInt aError, TLbsAsistanceDataGroup aDataMask)
	{
	TInt err;
	TBool verified = ETrue;
	
	LBSLOG(ELogP1, "CT_AGpsModule::AssistanceDataEvent()\n");
	LBSLOG2(ELogP1, "\t\tiAssistanceDataRequested = %d\n", iAssistanceDataRequested);
	LBSLOG2(ELogP1, "\t\tiAssistanceDataAvailable = %d\n", iAssistanceDataAvailable);
	LBSLOG(ELogP1, "\t\t\n");

	if(aError == KPositionAssistanceDataReset)
		{
		return;
		}

	if ((iAssDataTestMode == TModuleDataIn::EModuleTestModeAssDataOn_Unsolicited) ||
		(iAssistanceDataRequested))
		{
		iAssistanceDataRequested = EFalse;
		iAssistanceDataAvailable = ETrue;
		
		switch (iAssDataTestMode)
			{
			// Invalid data request.
			case TModuleDataIn::EModuleTestModeAssDataOn_InvalidDataRequest:
				{
				// Test for the correct err code.
				if (aError != KErrArgument)
					{
					verified = EFalse;
					break;
					}
			
				// Test for the correct data mask bit.

				// Currently the highest assistance data bit EAssistanceDataPositioningGpsUtcModel = 0x00000100
				// thus we use 0x00000200.
				if (aDataMask != KAssistanceDataUndefined)
					{
					verified = EFalse;
					}
				}
				break;

			// No network available when requesting data.
			case TModuleDataIn::EModuleTestModeAssDataOn_NoNetwork:
				{
				// Test for the correct err code.
				if (aError != KErrNotReady)
					{
					verified = EFalse;
					}	
				}
				break;

			case TModuleDataIn::EModuleTestModeAssDataOn:
			case TModuleDataIn::EModuleTestModeAssDataOn_TimeStamp:
			case TModuleDataIn::EModuleTestModeAssDataOn_Unsolicited:
			case TModuleDataIn::EModuleTestModeAssDataOn_EarlyPosUpdates:
				{
				// Verify the error.
				if (aError != KErrNone)
					{
					verified = EFalse;
					break;
					}

				// Verify the mask.
				if (aDataMask != EAssistanceDataReferenceTime)
					{
					verified = EFalse;
					break;
					}
					
				// Verify the assistance data.
				verified = VerifyRefTimeData();
				}
				break;
			
			case TModuleDataIn::EModuleTestModeAssDataOn_SomeDataNotAvailable:
				{
				// Two events are expected for this test mode - note the test harness will ensure both events are delivered.
				
				// The first event is given when aError is equal to KErrNone,
				// and the second when aError is equal to KErrNotFound.
				
				// First event, ensure the reference time data is present.
				if (aError == KErrNone)
					{
	//				verified = ( == aDataMask);

					verified = VerifyRefTimeData();
					}
					
				// Second event, ensure reference location is NOT available.
				else if (aError == KErrNotFound)
					{
					// Verify mask.
					verified = (EAssistanceDataReferenceLocation == aDataMask);
					
					// Verify data.
					if (verified)
						{
						verified = VerifyRefLocationData();
						}
					}
				
				// Un-expected error.	
				else
					{
					verified = EFalse;
					}
				}
				break;

			// Cancel data request test.
			case TModuleDataIn::EModuleTestModeAssDataOn_Cancel_Stage1:
			case TModuleDataIn::EModuleTestModeAssDataOn_Cancel_Stage2:
				{
				// Expecting not be called for this test, if we are the cancel will also be verified by
				// processing the NetSim MO-LR release event from the test step.

				// Do nothing here - thus verified ok.
				}
				break;


			// Invalid data request.
			case TModuleDataIn::EModuleTestModeAssDataOn_InvalidDataGet:
				{				
				// Verify the error.
				if (aError != KErrNone)
					{
					verified = EFalse;
					break;
					}

				// Use a gps ref time reader.
				RUEPositioningGpsReferenceTimeReader gpsRefTimeReader;
				TTime timeStamp;
				
				// Open the reader.
				TRAP(err, gpsRefTimeReader.OpenL());
				if (err)
					{
					verified = EFalse;
					break;
					}
				
				// Ask for un-defined/un-known assistance data, pass in a invalid mask using a valid reader.
				
				// Currently the highest assistance data bit EAssistanceDataPositioningGpsUtcModel	= 0x00000100
				// thus we use 0x00000200.
				LBSLOG(ELogP2, "CT_AGpsModule::AssistanceDataEvent() Requesting Assistance Data Item\n");
				err = iLocSourceGpsObserver.GetAssistanceDataItem(static_cast<TLbsAssistanceDataItem>(KAssistanceDataUndefined), gpsRefTimeReader, timeStamp);
				if (err != KErrArgument)
					{
					verified = EFalse;
					}
				
				gpsRefTimeReader.Close();
				}
				break;
				

			default:
				{
				// un-expected test state...
				}
			}


		// Post the result of the verification back to the test step.
		TPckgBuf<TModuleDataOut> modDataOutBuf;
		TModuleDataOut& modDataOut = modDataOutBuf();
		
		if (verified)
			{
			modDataOut.iRequestType = TModuleDataOut::EModuleResponse;
			modDataOut.iResponse = TModuleDataOut::EModuleAssDataOk;   
			}
	   
		else
			{
			modDataOut.iRequestType = TModuleDataOut::EModuleResponse;
			modDataOut.iResponse = TModuleDataOut::EModuleErr;
			}
	   
		LBSLOG2(ELogP2, "CT_AGpsModule::AssisTDataEvent() sending notification of AssIST Data Event. Response = %d\n", modDataOut.iResponse);
		err = RProperty::Set(KUidSystemCategory, ELbsTestAGpsModuleOut, modDataOutBuf);	// err will be lost, keep to aid debugging.

		
		// Return position now the assistance data is available.
		
		// Allways return position.
		if ((iGpsOptions.GpsMode() == CLbsAdmin::EGpsAlwaysTerminalBased) ||
		
		// Return position if not already given.
			((iGpsOptions.GpsMode() == CLbsAdmin::EGpsPreferTerminalBased) && iAssistanceDataAvailable))
			{
				if (iAssDataTestMode == TModuleDataIn::EModuleTestModeAssDataOn_EarlyPosUpdates) // see INC109178
				{
				UpdateLocation();	// in .ini make this location a NaN one					
				}
			// Take some time to process the assistance data, before the position is actually returned.
			TTime time;
			
			time.UniversalTime();
			time += TTimeIntervalMicroSeconds(KAssistanceDataTimeDelay);
			iDelayUpdateTimer->SetTimer(time);			
			}		
		}
	}	
	

void CT_AGpsModule::HandleTimerL(TInt aTimerId, const TTime& aTargetTime)
	{
	(void)aTargetTime;

	LBSLOG(ELogP1, ">>>CT_AGpsModule::HandleTimerL()\n");
	LBSLOG2(ELogP1, "\t\taTimerId = %d\n", aTimerId);
	LBSLOG2(ELogP1, "\t\tiAssistanceDataRequested = %d\n", iAssistanceDataRequested);
	LBSLOG2(ELogP1, "\t\tiAssistanceDataAvailable = %d\n", iAssistanceDataAvailable);
	LBSLOG(ELogP1, "\t\t\n");
	
	// Our timer fired, handle assistance data request and pos update.
	if (aTimerId == KPosUpdateTimerId)
	{

		// Allways request data.
		if ((iGpsOptions.GpsMode() == CLbsAdmin::EGpsAlwaysTerminalBased) ||
			
		// Request data is not available.
			((iGpsOptions.GpsMode() == CLbsAdmin::EGpsPreferTerminalBased) && !iAssistanceDataAvailable))
			{
			// Now requesting assistance data.
			iAssistanceDataRequested = ETrue;

			switch (iAssDataTestMode)
				{
				// Normal test mode when not testing assistance data.
				case TModuleDataIn::EModuleTestModeAssDataOff:
					{
					// Do nothing.
					}
					break;


				// Test modes used for test case ids which test assistance data.
				case TModuleDataIn::EModuleTestModeAssDataOn:
					{
					// Post back to the test step assistance data has just been requested.
					TPckgBuf<TModuleDataOut> modDataOutBuf;
					TModuleDataOut& modDataOut = modDataOutBuf();
					modDataOut.iResponse = TModuleDataOut::EModuleAssDataRequestedOk;

					LBSLOG(ELogP2, "CT_AGpsModule::HandleTimerL() sending notification of Assistance data Request\n");
  					TInt err = RProperty::Set(KUidSystemCategory, ELbsTestAGpsModuleOut, modDataOutBuf);	// err will be lost, keep to aid debugging.

					// Note fall through.
					}
					
				case TModuleDataIn::EModuleTestModeAssDataOn_TimeStamp:
				case TModuleDataIn::EModuleTestModeAssDataOn_NoNetwork:
				case TModuleDataIn::EModuleTestModeAssDataOn_InvalidDataGet:
				case TModuleDataIn::EModuleTestModeAssDataOn_EarlyPosUpdates:
					{
					// Request some simple assistance data.
					LBSLOG(ELogP2, "CT_AGpsModule::HandleTimerL() Requesting Assistance Data (EAssistanceDataReferenceTime)\n");
					iLocSourceGpsObserver.RequestAssistanceData(EAssistanceDataReferenceTime);
					
					// Time stamp the request, to enable assistance data time stamp verfication.
					iAssistanceDataRequestedTime.UniversalTime();
					}
					break;


				case TModuleDataIn::EModuleTestModeAssDataOn_SomeDataNotAvailable:
					{
					// Request assistance data where reference location is not returned by
					// the test assistance data provider.
					LBSLOG(ELogP2, "CT_AGpsModule::HandleTimerL() Requesting Assistance Data (EAssistanceDataReferenceTime | EAssistanceDataReferenceLocation)\n");
					iLocSourceGpsObserver.RequestAssistanceData(EAssistanceDataReferenceTime | 
																EAssistanceDataReferenceLocation);
					}
					break;


				// Cancel assistance data test.
				case TModuleDataIn::EModuleTestModeAssDataOn_Cancel_Stage1:
					{
					// Request some simple assistance data.
					LBSLOG(ELogP2, "CT_AGpsModule::HandleTimerL() Requesting Assistance Data (EAssistanceDataReferenceTime)\n");
					iLocSourceGpsObserver.RequestAssistanceData(EAssistanceDataReferenceTime);
					}	
					break;


				// Invalid data request.
				case TModuleDataIn::EModuleTestModeAssDataOn_InvalidDataRequest:
					{				
					// Ask for un-defined/un-known assistance data, pass in a invalid mask.
					
					// Currently the highest assistance data bit EAssistanceDataPositioningGpsUtcModel	= 0x00000100
					// thus we use 0x00000200.
					LBSLOG(ELogP2, "CT_AGpsModule::HandleTimerL() Requesting Assistance Data \n");
					iLocSourceGpsObserver.RequestAssistanceData(static_cast<TLbsAssistanceDataItem>(KAssistanceDataUndefined));
					}
					break;


				// Request assistance data from a empty cache test.
				case TModuleDataIn::EModuleTestModeAssDataOn_DataNotAvailable:
					{
					TBool verified = EFalse;
					TInt err;

					TTime timeStamp;
					RUEPositioningGpsReferenceTimeReader gpsRefTimeReader;
					
					// Request assistance data which not be held by the manager. Reference time data will be present
					// because the test assistance data provider would have returned as a result of the previous test.
					// TODO: ensure this is correct this always returns data, can't see how to make the
					// func fail
					
					// Open the reader to ensure we have valid parameter for the call, however we are not expecting
					// the reader to be filled.
					TRAP(err, gpsRefTimeReader.OpenL())
					if (err == KErrNone)
						{
						LBSLOG(ELogP2, "CT_AGpsModule::HandleTimerL() Requesting Assistance Data Item(not available)\n");
						// Request some assistance data which should not be avaiable.
						err = iLocSourceGpsObserver.GetAssistanceDataItem(EAssistanceDataReferenceTime, gpsRefTimeReader, timeStamp);
						if (err == KErrNotFound)
							{
							verified = ETrue;
							}	
						gpsRefTimeReader.Close();
						}
						
					// Return the answer to the test step.
					TPckgBuf<TModuleDataOut> modDataOutBuf;
					TModuleDataOut& modDataOut = modDataOutBuf();

					if (verified)
						{
						modDataOut.iRequestType = TModuleDataOut::EModuleResponse;
						modDataOut.iResponse = TModuleDataOut::EModuleAssDataOk;   
						}
		   
					else
						{
						modDataOut.iRequestType = TModuleDataOut::EModuleResponse;
						modDataOut.iResponse = TModuleDataOut::EModuleErr;
						}
		   
					LBSLOG(ELogP2, "CT_AGpsModule::HandleTimerL() sending notification of assistance data unavailable\n");
					err = RProperty::Set(KUidSystemCategory, ELbsTestAGpsModuleOut, modDataOutBuf);	// err will be losted, keep to aid debugging.
					}
					break;
					

				case TModuleDataIn::EModuleTestModeAssDataOn_Unsolicited:
				default:
					break;
				}
			}
		else
			{
			// For each location request, we must allways call RequestAssistanceData if data is required or not,
			// when no data is required use EAssistanceDataNone.
			LBSLOG(ELogP2, "CT_AGpsModule::HandleTimerL() Requesting Assistance Data (EAssistanceDataNone)\n");
			iLocSourceGpsObserver.RequestAssistanceData(EAssistanceDataNone);
			}


		// Return location. 
		if ((iGpsOptions.GpsMode() == CLbsAdmin::EGpsAutonomous) ||
			((iGpsOptions.GpsMode() == CLbsAdmin::EGpsPreferTerminalBased) && iAssistanceDataAvailable) ||
			
			// Special assistance test modes where location should be returned.
			(iAssDataTestMode == TModuleDataIn::EModuleTestModeAssDataOff) ||
			(iAssDataTestMode == TModuleDataIn::EModuleTestModeAssDataOn_DataNotAvailable) ||
			(iAssDataTestMode == TModuleDataIn::EModuleTestModeAssDataOn_Cancel_Stage1))
			{
			// Delay before returning the actual position.
			TTime time;
			
			time.UniversalTime();
			time += TTimeIntervalMicroSeconds(KDelayUpdateTimeDelay);
			iDelayUpdateTimer->SetTimer(time);
			}
		}
		
	// Done processing assistance data or delaying the update, return position.
	else if (aTimerId == KDelayUpdateTimerId)
		{
		UpdateLocation();
		}
	else if (aTimerId == KDelayStatusTimerId)
		{
		iLocSourceGpsObserver.UpdateDataQualityStatus(iModuleStatus.DataQualityStatus());
		}
	}


void CT_AGpsModule::UpdateLocation()
	{
	LBSLOG(ELogP1, "CT_AGpsModule::UpdateLocation()\n");

	if (iPosInfoIndex < iPosInfoArr.Count())
		{
		TPositionInfoBase* posInfo = reinterpret_cast<TPositionInfoBase*>(iPosInfoArr[iPosInfoIndex]);
			
		// Set base class items.
		posInfo->SetModuleId(TUid::Uid(APGS_MODINFO_MODULE_ID));
		posInfo->SetUpdateType(EPositionUpdateGeneral);
		TPositionModuleInfo::TTechnologyType techType = TPositionModuleInfo::ETechnologyUnknown;
		TPositionModeReason techTypeReason = EPositionModeReasonNone;

		switch (iAssDataTestMode)
			{
			case TModuleDataIn::EModuleTestModeAssDataOff:
				techType = TPositionModuleInfo::ETechnologyTerminal;
				techTypeReason = EPositionModeReasonNone;
				break;

			case TModuleDataIn::EModuleTestModeAssDataOn_NoNetwork:
			case TModuleDataIn::EModuleTestModeAssDataOn_DataNotAvailable:
			case TModuleDataIn::EModuleTestModeAssDataOn_InvalidDataGet:
			case TModuleDataIn::EModuleTestModeAssDataOn_InvalidDataRequest:
				techType = TPositionModuleInfo::ETechnologyTerminal;
				techTypeReason = EPositionModeReasonNetworkError;
				break;

			case TModuleDataIn::EModuleTestModeAssDataOn_TimeStamp:
			case TModuleDataIn::EModuleTestModeAssDataOn_Cancel_Stage1:
				techType = TPositionModuleInfo::ETechnologyTerminal;
				techTypeReason = EPositionModeReasonDataError;
				break;

			case TModuleDataIn::EModuleTestModeAssDataOn:
			case TModuleDataIn::EModuleTestModeAssDataOn_Unsolicited:
			case TModuleDataIn::EModuleTestModeAssDataOn_SomeDataNotAvailable:
			case TModuleDataIn::EModuleTestModeAssDataOn_EarlyPosUpdates:
				techType = TPositionModuleInfo::ETechnologyTerminal | TPositionModuleInfo::ETechnologyAssisted;
				techTypeReason = EPositionModeReasonNone;
				break;
			}
		posInfo->SetPositionMode(techType);
		posInfo->SetPositionModeReason(techTypeReason);

					
		// Set the timestamp.
		TPosition position;
		
		static_cast<TPositionInfo*>(posInfo)->GetPosition(position);
		
		position.SetCurrentTime();
					
		static_cast<TPositionInfo*>(posInfo)->SetPosition(position);


		// Set the satellite time for satellite info classes.
		if (posInfo->PositionClassType() & EPositionSatelliteInfoClass)
			{			
			TTime timeStamp;
			TPositionSatelliteInfo* satInfo = reinterpret_cast<TPositionSatelliteInfo*>(posInfo);		
	
			// Set the position timestamp.
			timeStamp.UniversalTime();
			
			// Set the satellite timestamp.
			timeStamp -= iTimeStampOffset;
			satInfo->SetSatelliteTime(timeStamp);				
			}

		iLocSourceGpsObserver.UpdateLocation(iError, (const_cast<const TPositionInfoBase**>(&posInfo)), 1, iTargetTime);

		// Only use the whole pos info array when in normal test mode.
		if ((iGpsOptions.GpsMode() == CLbsAdmin::EGpsAutonomous) || (iAssDataTestMode == TModuleDataIn::EModuleTestModeAssDataOn_EarlyPosUpdates)) // see INC109178
		   {
		    iPosInfoIndex++;
		   }
		}

	// We have no more position updates to return.
	else
		{
		iLocSourceGpsObserver.UpdateLocation(KErrGeneral, NULL, 0, iTargetTime);
		}
	}


/** Process requests from the test step. Test data is passed to the test module via the input
	data bus.
*/
void CT_AGpsModule::HandleDataBusUpdateL(const TModuleDataIn& aModuleDataIn)
	{
	LBSLOG(ELogP1, "CT_AGpsModule::HandleDataBusUpdateL()\n");

	T_LbsUtils utils;

	switch (aModuleDataIn.iRequestType)
		{
		case TModuleDataIn::EModuleRequestConfig:
			{
			// Read the configuration ini file to update the module data items.
			utils.GetConfigured_PosInfosL(aModuleDataIn.iConfigFileName, aModuleDataIn.iConfigSection, iPosInfoArr);
			utils.GetConfigured_ModuleStatusL(aModuleDataIn.iConfigFileName, aModuleDataIn.iConfigSection, iModuleStatus, iDelayReportingModStatus);
		
			
			if(!iDelayReportingModStatus)// read from config
				{
				// Good opportunity to let the observer know what our device and data quality status are (since we've just got them)
				iLocSourceGpsObserver.UpdateDeviceStatus(iModuleStatus.DeviceStatus());
				iLocSourceGpsObserver.UpdateDataQualityStatus(iModuleStatus.DataQualityStatus());
				}
			
			// Reset the position array index, now that we have the new positions.
			iPosInfoIndex = 0;

			// Also a good place to reset the module back to defaults, as this is generally called
			// at the start of each test.
			iAssDataEventType = TModuleDataIn::EAssDataEventExpectSingle;
			iAssDataTestMode = TModuleDataIn::EModuleTestModeAssDataOn;
		    iOptionsChangeReportMode = TModuleDataIn::EGpsModuleDoNotReportOptionsChange;
			iAssistanceDataRequested = EFalse;
			iAssistanceDataAvailable = EFalse;
			iTimeOut = 0;
			iError = KErrNone;
			iTimeStampOffset = 0;
			}
			break;
			
		
         // We're being asked to kick off a device/quality status event(s)			.
		case TModuleDataIn::EModuleRequestEventKickOff:
			{
			if((aModuleDataIn.iKickOffEventType & TModuleDataIn::EKickOffEventTypeDevice) && 
			   (aModuleDataIn.iKickOffEventType & TModuleDataIn::EKickOffEventTypeDataQuality))
			   {
			   // to provide determinnistic behaviour call the DeviceStatus call back then
			   // wait for a time before sending DataQualityStatus
			   iLocSourceGpsObserver.UpdateDeviceStatus(iModuleStatus.DeviceStatus());
			   
			   TTime time;
			   time.UniversalTime();
			   time += TTimeIntervalMicroSeconds(KDelayStatusTime);
			   iDelayStatusTimer->SetTimer(time);
			   }
			else
				{
				if(aModuleDataIn.iKickOffEventType & TModuleDataIn::EKickOffEventTypeDevice)
					{
					iLocSourceGpsObserver.UpdateDeviceStatus(iModuleStatus.DeviceStatus());
					}
			
				if(aModuleDataIn.iKickOffEventType & TModuleDataIn::EKickOffEventTypeDataQuality)
					{
					iLocSourceGpsObserver.UpdateDataQualityStatus(iModuleStatus.DataQualityStatus());
					}
				}
			break;		
			}
		// We're being asked to switch the required test mode.
		case TModuleDataIn::EModuleRequestTestMode:
			{
			// For the cancel test we process the mode here.
			if (aModuleDataIn.iAssDataTestMode == TModuleDataIn::EModuleTestModeAssDataOn_Cancel_Stage2)
				{
				// Ensure we in the middle of a cancel test.
				ASSERT(iAssDataTestMode == TModuleDataIn::EModuleTestModeAssDataOn_Cancel_Stage1);

				// Now cancel the request for assistance data.
				LBSLOG(ELogP2, "CT_AGpsModule::HandleDataBusUpdateL() Requesting Assistance Data (0)\n");
				iLocSourceGpsObserver.RequestAssistanceData(0);
				}
			
			iAssDataEventType = aModuleDataIn.iAssDataEventType;
			iAssDataTestMode = aModuleDataIn.iAssDataTestMode;
			}
			break;
			
		case TModuleDataIn::EModuleRequestOptionsChangeReportMode:
			{
				iOptionsChangeReportMode = aModuleDataIn.iOptionsChangeReportMode;
			}
			break;
			
		// We're being asked to update the current time out value.
		case TModuleDataIn::EModuleRequestTimeOut:
			{
			iTimeOut = aModuleDataIn.iTimeOut;
			iOneOffTimeOut = aModuleDataIn.iOneOffTimeOut;
			iFirstRequestPast = EFalse;
			}
			break;
			
		// We're being to update the current error
		case TModuleDataIn::EModuleRequestError:
			{
			iError = aModuleDataIn.iError;	
			}
			break;

		// We're being asked to update the time stamp offset value.
		case TModuleDataIn::EModuleRequestTimeStampOffset:
			{
			iTimeStampOffset = aModuleDataIn.iTimeStampOffset;
			}
			break;

		}
	}


/** Verify the reference location assisatance data is NOT present.

@return ETrue if data is as expected.
*/
TBool CT_AGpsModule::VerifyRefLocationData()
	{
	TInt err;
	TBool verified = ETrue;
	RReferenceLocationReader refLocationReader;
	TTime timeStamp;
	
	// Open the reader to ensure we have valid parameter for the call, however we are not expecting
	// the reader to be filled.
	TRAP(err, refLocationReader.OpenL())
	if (err == KErrNone)
		{
		// Request some assistance data which should not be avaiable.
		LBSLOG(ELogP2, "CT_AGpsModule::VerifyRefLocationData() Requesting Assistance Data Item (not available)\n");
		err = iLocSourceGpsObserver.GetAssistanceDataItem(EAssistanceDataReferenceLocation, refLocationReader, timeStamp);
		if (err != KErrNotFound)
			{
			verified = EFalse;
			}	
		refLocationReader.Close();
		}				

	return verified;
	}


/** Verify the reference time assistance data is present and the time stamp is correct.

@return ETrue if data is as expected.
*/
TBool CT_AGpsModule::VerifyRefTimeData()
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
		LBSLOG(ELogP2, "CT_AGpsModule::VerifyRefTimeData() Requesting Assistance Data Item\n");
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
		// For time stamp test, use the time value retrived from GetAssistanceDataItemTimeStamp().
		if (iAssDataTestMode == TModuleDataIn::EModuleTestModeAssDataOn_TimeStamp)
			{
			err = iLocSourceGpsObserver.GetAssistanceDataItemTimeStamp(EAssistanceDataReferenceTime, timeStamp);
			if (err != KErrNone)
				{
				verified = EFalse;
				}
			}

		// Verify time stamp - ensure the returned time stamp is newer by a small margin.
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
		}

	return verified;
	}
