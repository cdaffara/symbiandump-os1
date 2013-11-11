// Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
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
 @file
 @internalTechnology
 @released
*/

#include "lbsdevloggermacros.h"
#include "agpsinterfacehandler.h"
#include "nrhpanic.h"
#include "lbsnetregstatusint.h"


// Pre-defined technology types for common positioning modes.
const TPositionModuleInfo::TTechnologyType KTerminalAssistedMode = 
		(TPositionModuleInfo::ETechnologyNetwork |
		 TPositionModuleInfo::ETechnologyAssisted);
const TPositionModuleInfo::TTechnologyType KTerminalBasedMode = 
		(TPositionModuleInfo::ETechnologyTerminal |
		 TPositionModuleInfo::ETechnologyAssisted);
const TPositionModuleInfo::TTechnologyType KAutonomousMode = 
		(TPositionModuleInfo::ETechnologyTerminal);

/** Constructor.
*/
CAgpsInterfaceHandler::CAgpsInterfaceHandler(
		MAgpsInterfaceHandlerObserver& aObserver,
		CLbsAdmin& aAdmin,
		RLbsNetworkRegistrationStatus& aNetRegStatus) :
	iObserver(aObserver),
	iAdmin(aAdmin),
	iNetRegStatus(aNetRegStatus),
	iX3pStatus(EX3pStatusIdle),
	iCombinedPowerModeAdvice(TLbsPositionUpdateRequestBase::EPowerAdviceOff),
	iCombinedRequestGpsMode(EGpsRequestModeUnknown)
	{
	}

/** Destructor.
*/
CAgpsInterfaceHandler::~CAgpsInterfaceHandler()
	{
	iRequests.Close();
	delete iMeasurementUpdateMonitor;
	delete iLocationUpdateMonitor;
	}

/**
*/
CAgpsInterfaceHandler* CAgpsInterfaceHandler::NewL(
		MAgpsInterfaceHandlerObserver& aObserver,
		CLbsAdmin& aAdmin,
		RLbsNetworkRegistrationStatus& aNetRegStatus)
	{
	CAgpsInterfaceHandler* self = new (ELeave) CAgpsInterfaceHandler(aObserver, aAdmin, aNetRegStatus);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;
	}

/**
*/

const TInt KLbsDefaultMaxNumLocationRequests   = 4;

void CAgpsInterfaceHandler::ConstructL()
	{
	iLocationUpdateMonitor = CLbsLocUpdateMonitor::NewL(*this);
	iMeasurementUpdateMonitor = CLbsMeasurementInfoMonitor::NewL(*this);
	
	TInt err = iAdmin.Get(KLbsSettingMaximumExternalLocateRequests, iMaxNumLocationRequests);
	if (err != KErrNone)
		{
		iMaxNumLocationRequests = KLbsDefaultMaxNumLocationRequests;
		}

	iRequests.ReserveL(iMaxNumLocationRequests+1); // +1 for processing emergencies

	// Get the A-GPS device mode capabilities:
	err = LbsModuleInfo::GetDeviceCapabilities(KLbsGpsLocManagerUid, iDeviceGpsModeCaps);
	if(err != KErrNone || (iDeviceGpsModeCaps==TPositionModuleInfoExtended::EDeviceGpsModeNone))
		{
		// Assume module supports hybrid if it has not reported its capabilities in module info file
		iDeviceGpsModeCaps = TPositionModuleInfoExtended::EDeviceGpsModeSimultaneousTATB;
		}
	

	}
	
/** Called to initialise a location request.

This is used to tell the AGPS Interface that a GPS location request
is goind to happen 'soon'. Usually, this means we should update the
power mode advice to at least 'standby', to give the GPS hardware
as much time as possible to warm up.

@param aSessionId Session Id of the request that is started.

@return KErrNone if no problems, or one of the standard Symbian
		error codes if there was a problem storing the new request.
*/
TInt CAgpsInterfaceHandler::PreStartPositioning(
		const TLbsNetSessionIdInt& aSessionId, TBool aEmergency)
	{
	// Add the request to the array
	TAgpsRequest newRequest;
	newRequest.iSessionId = &aSessionId;
	newRequest.iState = EAgpsRequestStateIdle;

	TInt err = AddOrUpdateRequest(newRequest, aEmergency);

	if (err != KErrNone)
		{
		return err;
		}
	
	// Update the GPS power mode advice to the AGPS Manager
	TLbsPositionUpdateRequestBase::TPowerAdvice newPowerAdvice;
	CalculateCombinedPowerModeAdvice(newPowerAdvice);
	if (newPowerAdvice != iCombinedPowerModeAdvice)
		{
		iCombinedPowerModeAdvice = newPowerAdvice;
		iLocationUpdateMonitor->SendPowerAdviceRequest(iCombinedPowerModeAdvice);
		}
	
	return err;
	}

/** Called to start a location request.

This is used when the CLbsPrivLocFsm actually wants to send
a location request to the AGPS Manager.

This function must combine the new request with any existing
request(s) - this includes quality params and GPS mode, before
sending a request to the AGPS Manager.

@param aSessionId Session Id of the request that is started.
@param aMethod GPS mode(s) to use, as set by the network.
@param aQuality quality params to use, as set by the network.

@return KErrNone if no problems, or one of the standard Symbian
		error codes if there was a problem storing the new request.
*/
TInt CAgpsInterfaceHandler::StartPositioning(
		const TLbsNetSessionIdInt& aSessionId,
		const TLbsNetPosRequestMethodInt& aMethod,
		const TLbsNetPosRequestQualityInt& aQuality,
		TBool aEmergency)
	{
	// Check that the new request GPS mode is valid
	TGpsRequestMode newRequestGpsMode = ConvertPosMethodToGpsRequestMode(aMethod);
	TGpsRequestMode newCombinedGpsMode;
	TInt err = CalculateNewCombinedRequestMethod(
								iCombinedRequestGpsMode,
								newRequestGpsMode,
								newCombinedGpsMode,
								aEmergency);
	if (err != KErrNone)
		{
		RemoveRequest(aSessionId);
		return err;
		}
	
	// Add (or update) the request to the buffer
	TAgpsRequest newRequest;
	newRequest.iSessionId = &aSessionId;
	newRequest.iState = EAgpsRequestStateActive;
	newRequest.iGpsMode = newRequestGpsMode;
	newRequest.iPosQuality = &aQuality;
	TTime now;
	now.UniversalTime();
	newRequest.iEndTime = (now + aQuality.MaxFixTime());
	newRequest.iGpsTimingOfCellFramesRequested = aMethod.GpsTimingOfCellFramesRequested();
	err = AddOrUpdateRequest(newRequest, aEmergency);
	if (err != KErrNone)
		{
		return err;
		}
	
	// Use the new combined GPS mode
	iCombinedRequestGpsMode = newCombinedGpsMode;
	
	// Combine Quality.
	CalculateCombinedRequestQuality();

	// Send the new combined request to the AGPS manager.
	SendCombinedLocationRequest();

	return err;
	}

/** Temporarily stop an existing location request.

This function is used when a location request has finished,
but the session it belongs to hasn't finished yet. For example,
in terminal assisted or hybrid mode, there is more than one location
request within a session, so the it should not be removed from the
list until it has completely finished.
*/
void CAgpsInterfaceHandler::HoldPositioning(
		const TLbsNetSessionIdInt& aSessionId, 
		TInt /*aReason*/)
	{
	// Find a matching request.
	TInt index = iRequests.Find(aSessionId, IsSessionIdEqual);
	if (index == KErrNotFound)
		{
		// If can't find one, just ignore it (but LBSLOG_ERR)?
		LBSLOG_ERR2(ELogP3, "Failed to find a match for sessionId %d.", aSessionId.SessionNum());
		return;
		}

	// Set the request to 'hold'.
	iRequests[index].iState = EAgpsRequestStateHold;
	}

/** Stop and remove a location request.

This function is used when a session has finished, and
the location request can be removed from the list.
*/
void CAgpsInterfaceHandler::StopPositioning(
		const TLbsNetSessionIdInt& aSessionId)
	{	
	// Remove request from iRequests.
	RemoveRequest(aSessionId);
	
	if (IsAnyRequestState(EAgpsRequestStateActive))
		{
		// If there are any other active requests, combine them and send
		// a new location request.
		
		// Combine quality
		CalculateCombinedRequestQuality();

		// Send the new combined request to the AGPS manager.
		SendCombinedLocationRequest();
		}
	else if (!IsAnyRequestState(EAgpsRequestStateHold)
			 && iLocationRequestActive)
		{
		// There are no requests that are either active or on hold, so send 
		// a cancel message to the AGPS manager.
		CalculateCombinedPowerModeAdvice(iCombinedPowerModeAdvice);
		iLocationUpdateMonitor->SendCancelRequest(iCombinedPowerModeAdvice);
		iLocationRequestActive = EFalse;
		
		// Also reset the combined request parameters
		iCombinedRequestGpsMode = EGpsRequestModeUnknown;
		}
	else
		{
		// Update the GPS power mode advice to the AGPS Manager
		TLbsPositionUpdateRequestBase::TPowerAdvice newPowerAdvice;
		CalculateCombinedPowerModeAdvice(newPowerAdvice);
		if (newPowerAdvice != iCombinedPowerModeAdvice)
			{
			iCombinedPowerModeAdvice = newPowerAdvice;
			iLocationUpdateMonitor->SendPowerAdviceRequest(iCombinedPowerModeAdvice);
			}		
		}
	}

/** Check that it is ok to send the position update to state machine(s).

Some position updates should be ignored by the NRH, these are:
1) Conflict control updates (aConflictControl = ETrue), only the 
   LocServer is interested in these.
2) In hybrid, if the params are 'invalid'. Some licensees use special
   values in the position update to flag that it should not be used,
   e.g. if the horizontal accuracy is 0.
*/
TBool CAgpsInterfaceHandler::CheckPositionUpdateIsValid(
		TBool aConflictControl,
		TInt aReason,
		const TPositionExtendedSatelliteInfo& aPosInfo,
		const TTime& /*aActualTime*/)
	{
	// Some updates are only meant for the LocServer - they
	// have aConflictControl == ETrue. We shouldn't send these
	// to the PrivLocHandler.
	if (aConflictControl)
		{
		return EFalse;
		}

	// For hybrid position, first check whether the data is valid. If any of 
	// longitude, latitude, or horizontal accuracy is missing, or horizontal 
	// accuracy is zero, then it's not valid, so there's no point going any 
	// further.
	if ((iCombinedRequestGpsMode == EGpsRequestModeHybrid) && (aReason == KErrNone))
		{
		TPosition pos;
		aPosInfo.GetPosition(pos);
		if (Math::IsNaN(pos.Latitude()) || 
			Math::IsNaN(pos.Longitude()) || 
			Math::IsNaN(pos.HorizontalAccuracy()) || 
			(pos.HorizontalAccuracy() == 0))
			{
			return EFalse;
			}
		}
	
	return ETrue;
	}
	
/** Get the latest update value from the GPS position bus.

Note: Some updates are intended only for the LocServer, these
all have conflictControl == ETrue, so in this case we return 
KErrNotFound to tell the caller it shouldn't be used.

@param aReason Reason code for the update. Set by the AGPS manager.
@param aPosInfo Position data for the update.
@param aTimeStamp Time the update arrived in the LBS system.

@return KErrNone usually, but KErrNotFound if the update is invalid.
*/
TInt CAgpsInterfaceHandler::GetPosition(
		TInt& aReason,
		TPositionExtendedSatelliteInfo& aPosInfo,
		TTime& aTimeStamp)
	{
	TBool conflictControl;
	iLocationUpdateMonitor->GetPosition(
			conflictControl,
			aReason,
			aPosInfo,
			aTimeStamp);
			
	if (CheckPositionUpdateIsValid(conflictControl,
								   aReason,
								   aPosInfo,
								   aTimeStamp))
		{
		return KErrNone;
		}
	else
		{
		return KErrNotFound;
		}
	}

/** Get the latest update from the GPS measurement results bus.
*/
void CAgpsInterfaceHandler::GetMeasurement(
		TInt& aReason,
		TPositionGpsMeasurementInfo& aPosInfo,
		TTime& aTimeStamp)
	{
	iMeasurementUpdateMonitor->GetMeasurement(aReason, aPosInfo, aTimeStamp);
	}

/** Callback for when a GPS update is sent by the AGPS manager.
*/
void CAgpsInterfaceHandler::OnPositionUpdate(
			TBool aConflictControl,
			TInt aReason,
			const TPositionExtendedSatelliteInfo& aPosInfo,
			const TTime& aActualTime)
	{	
	LBSLOG(ELogP1, "CAgpsInterfaceHandler::OnPositionUpdate");
	if (CheckPositionUpdateIsValid(aConflictControl,
								   aReason,
								   aPosInfo,
								   aActualTime))
		{
		// Send the update to the PrivLocHandler
		iObserver.OnAgpsPositionUpdate(aReason, aPosInfo, aActualTime);
		}
	}

/** Callback for when a GPS Measured Results update is sent by the AGPS manager.
*/
void CAgpsInterfaceHandler::OnMeasurementInfoUpdate(
			TInt aReason, 
			const TPositionGpsMeasurementInfo& aMeasurementResults,
			const TTime& aActualTime)
	{
	// Send result to observer
	iObserver.OnAgpsMeasurementUpdate(aReason, aMeasurementResults, aActualTime);
	}

/** Combines the quality parameters of all the active requests.

Note: Only requests which are 'active' should be considered when
combining the quality params.
*/
void CAgpsInterfaceHandler::CalculateCombinedRequestQuality()
	{
	// Set the initial values to start from
	TReal32 minHorizAcc(10000000000000000.0);
	TReal32 minVertAcc(10000000000000000.0);
	TTimeIntervalMicroSeconds maxMaxFixTime(0);
	TTime now;
	now.UniversalTime();
	
	// Combine each request in turn but only if it is 'active'.
	const TInt count = iRequests.Count();
	for (TInt i = 0; i < count; i++)
		{
		if (iRequests[i].iState == EAgpsRequestStateActive)
			{
			// Horizontal accuracy = strictest accuracy.
			minHorizAcc = Min(minHorizAcc,
							  iRequests[i].iPosQuality->MinHorizontalAccuracy());
	
			// Vertical accuracy = strictest accuracy.
			minVertAcc = Min(minVertAcc,
							 iRequests[i].iPosQuality->MinVerticalAccuracy());
			
			// Max fix time = greatest/longest time.
			TTimeIntervalMicroSeconds maxFixTime = (iRequests[i].iEndTime.Int64() - now.Int64());
			maxMaxFixTime = Max(maxMaxFixTime, maxFixTime);
			}
		}
	
	// Set the combined accuracy
	iCombinedRequestQuality.SetMinHorizontalAccuracy(minHorizAcc);
	iCombinedRequestQuality.SetMinVerticalAccuracy(minVertAcc);
	iCombinedRequestQuality.SetMaxFixTime(maxMaxFixTime);
	}

/** Calculates the power mode advice to send to the AGPS manager.

Use the state of the X3P handler and the combined state of the current
requests to calculate the power mode advice to send to the AGPS
manager.
*/
void CAgpsInterfaceHandler::CalculateCombinedPowerModeAdvice(
		TLbsPositionUpdateRequestBase::TPowerAdvice& aCombinedPowerAdvice)
	{
	// Calculate combined state for all requests
	TAgpsRequestState combinedState(EAgpsRequestStateUnknown);
	if (IsAnyRequestState(EAgpsRequestStateActive))
		{
		combinedState = EAgpsRequestStateActive;
		}
	else if (IsAnyRequestState(EAgpsRequestStateHold))
		{
		combinedState = EAgpsRequestStateHold;
		}
	else if (IsAnyRequestState(EAgpsRequestStateIdle))
		{
		combinedState = EAgpsRequestStateIdle;
		}
	
	// Select the power advice to send to the AGPS manager by using
	// a look-up table.
	//
	// The look-up table below is described in the NRH design document,
	// so if it is changed, the design doc should also be updated to 
	// keep it in sync.
	const TInt KNumX3pStates = 3;
	const TInt KNumLocRequestStates = 4;
	static const TLbsPositionUpdateRequestBase::TPowerAdvice KPowerAdviceTable[KNumX3pStates][KNumLocRequestStates] = 
		{
			{ TLbsPositionUpdateRequestBase::EPowerAdviceOff, 
			  TLbsPositionUpdateRequestBase::EPowerAdviceOn, 
			  TLbsPositionUpdateRequestBase::EPowerAdviceOn, 
			  TLbsPositionUpdateRequestBase::EPowerAdviceOn },
			  
			{ TLbsPositionUpdateRequestBase::EPowerAdviceStandby, 
			  TLbsPositionUpdateRequestBase::EPowerAdviceOn, 
			  TLbsPositionUpdateRequestBase::EPowerAdviceOn, 
			  TLbsPositionUpdateRequestBase::EPowerAdviceOn },
			  
			{ TLbsPositionUpdateRequestBase::EPowerAdviceOn, 
			  TLbsPositionUpdateRequestBase::EPowerAdviceOn, 
			  TLbsPositionUpdateRequestBase::EPowerAdviceOn, 
			  TLbsPositionUpdateRequestBase::EPowerAdviceOn }
		};
	
	// Note: Because this table uses the enum values directly as indices,
	//       if either enum changes, you need to re-do the table above as well.
	__ASSERT_ALWAYS(iX3pStatus < KNumX3pStates, Panic(ENrhPanicInvalidPowerModeAdviceIndex));
	aCombinedPowerAdvice = KPowerAdviceTable[iX3pStatus][combinedState];
	}

/**
 Goes through all requests and decides if the timing of cell frames is requested or not
 
 @return ETrue if the timing of cell frames is requested
 */
TBool CAgpsInterfaceHandler::CalculateCombinedTimingOfCellFramesRequested()
	{
	TBool timingOfCellFramesRequested(EFalse);
	
	// Combine each request in turn but only if it is 'active'.
	const TInt count = iRequests.Count();
	// Make the flag to be ETrue if there are any requests active or on hold that have the flag
	// set to ETrue
	for (TInt i = 0; i < count; i++)
		{
		if((iRequests[i].iState == EAgpsRequestStateActive || 
				iRequests[i].iState == EAgpsRequestStateHold) && 
			iRequests[i].iGpsTimingOfCellFramesRequested)
			{
			timingOfCellFramesRequested = ETrue;
			break;
			}
		}
	return timingOfCellFramesRequested;
	}

/** Performs the common actions for sending a location request to the AGPS manager.

This function does common tasks such as calculating the power mode to send,
starting or stopping the needed monitors and actually sending the combined
request.

Note: Combining the quality params and/or GPS mode params is done differently
      depending on whether a request is being started, stopped or put on hold,
      so that is not done in this function.
*/
void CAgpsInterfaceHandler::SendCombinedLocationRequest()
	{	
	// Always start the GPS position update monitor, even in TA mode,
	// because we use this update bus to listen for error codes, 
	// e.g. KPositionEarlyComplete.
	iLocationUpdateMonitor->StartMonitor();
	
	// Start measurement monitor if combined request has GPS mode 
	// with Terminal-Assisted or hybrid:
	if (iCombinedRequestGpsMode == EGpsRequestModeTerminalAssisted
		|| iCombinedRequestGpsMode == EGpsRequestModeHybrid)
		{
		iMeasurementUpdateMonitor->StartMonitor();
		}
	else
		{
		iMeasurementUpdateMonitor->StopMonitor();
		}
	
	// Convert the TGpsRequestMode into TLbsNetPosRequestMethodInt
	TLbsNetPosRequestMethodInt posMethod;
	posMethod.SetGpsTimingOfCellFramesRequested(CalculateCombinedTimingOfCellFramesRequested());
	ConvertGpsRequestModeToPosMethod(iCombinedRequestGpsMode, posMethod);

	// Calculate the current power advice.
	CalculateCombinedPowerModeAdvice(iCombinedPowerModeAdvice);
	
	// Send combined location request.
	iLocationUpdateMonitor->SendLocationRequest(
			iCombinedPowerModeAdvice, 
			posMethod, 
			iCombinedRequestQuality);
	iLocationRequestActive = ETrue;	
	}

/** Called by CX3pHandler tp update the state of X3P requests.

For X3P, if there is a client connected to the NRH server, we assume that
there will a request soon, so we can send 'standby' power mode advice, to 
allow the GPS hardware to prepare for a location request before the request
has actually arrived.
*/
void CAgpsInterfaceHandler::SetX3pStatus(MX3pStatusHandler::TX3pStatus aStatus)
	{
	// Only send a power advice if the X3P status has actually changed.
	if (aStatus != iX3pStatus)
		{
		iX3pStatus = aStatus;
		TLbsPositionUpdateRequestBase::TPowerAdvice newPowerAdvice;
		CalculateCombinedPowerModeAdvice(newPowerAdvice);
		if (newPowerAdvice != iCombinedPowerModeAdvice)
			{
			iCombinedPowerModeAdvice = newPowerAdvice;
			iLocationUpdateMonitor->SendPowerAdviceRequest(iCombinedPowerModeAdvice);
			}
		}
	}

/** Comparison function for use with RArray::Find()

@return ETrue if session Ids match, EFalse otherwise.
*/
TBool CAgpsInterfaceHandler::IsSessionIdEqual(
		const TLbsNetSessionIdInt* aSessionId, 
		const TAgpsRequest& aRequest)
	{
	return (*aSessionId == *aRequest.iSessionId);
	}

/** Comparison function for use with RArray::Find()

@return ETrue if states match, EFalse otherwise.
*/
TBool CAgpsInterfaceHandler::IsRequestStateEqual(
		const TAgpsRequestState* aState, 
		const TAgpsRequest& aRequest)
	{
	return (*aState == aRequest.iState);
	}

/** Add or update a location request in the buffer.

If the request already exists on the list then its parameters are updated.
If it doesn't exist then the request is appended to the list.
For emergencies we haveone reserved slot used just for the emergency location request
*/
TInt CAgpsInterfaceHandler::AddOrUpdateRequest(const TAgpsRequest& aRequest, TBool aEmergency)
	{
	TInt err(KErrNone);
	TInt index = iRequests.Find(*aRequest.iSessionId, IsSessionIdEqual);
	if (index >= 0)
		{
		// Update params of existing request
		iRequests[index] = aRequest;
		}
	else if (aEmergency)
	    {
	    // Note that we can guarantee at least ONE free slot. See below.
	    // add emergency request to list
	    err = iRequests.Append(aRequest);
	    }
	else if (iRequests.Count() <= iMaxNumLocationRequests)
		{
		// Add new request to iRequests. Note, only allocate up to 
		// iMaxNumLocationRequests slots and leave at least one free for use for emergency
		err = iRequests.Append(aRequest);
		}
	else
		{
		// Exceeded the max number of requests, reject it!
		LBSLOG_ERR3(ELogP3,
					"CAgpsInterfaceHandler::AddOrUpdateRequest: Can't add new request, active requests (%d), limit (%d)", 
					iRequests.Count(), 
					iMaxNumLocationRequests);
		return KErrInUse;		
		}
	return err;
	}

/** Remove a request from the buffer.

@param aSessionId SessionId of the request to remove.
*/
void CAgpsInterfaceHandler::RemoveRequest(const TLbsNetSessionIdInt& aSessionId)
	{
	TInt index = iRequests.Find(aSessionId, IsSessionIdEqual);
	if (index >= 0)
		{
		iRequests.Remove(index);
		}
	else
		{
		LBSLOG_WARN2(ELogP3, "CAgpsInterfaceHandler::RemoveRequest: Failed to find match for sessionId %d.", aSessionId.SessionNum());
		}
	}

/** Search to find if any requests' state matches the given state.

@param aState Request state to compare to.

@return ETrue if any state has a matching state. EFalse otherwise.
*/
TBool CAgpsInterfaceHandler::IsAnyRequestState(
		TAgpsRequestState aState)
	{
	TInt index = iRequests.Find(aState, IsRequestStateEqual);
	return (index >= 0);
	}

/** Convert a TLbsNetPosRequestMethodInt into a TGpsRequestMode enum.
*/
TGpsRequestMode CAgpsInterfaceHandler::ConvertPosMethodToGpsRequestMode(
		const TLbsNetPosRequestMethodInt& aPosMethod)
	{
	TGpsRequestMode gpsMode(EGpsRequestModeUnknown);
	
	const TInt count = aPosMethod.NumPosMethods();
	for (TInt i = 0; i < count; i++)
		{
		TLbsNetPosMethodInt method;
		aPosMethod.GetPosMethod(i, method);
		switch (method.PosMode())
			{
			case KAutonomousMode:
				{
				if (gpsMode == EGpsRequestModeTerminalAssisted)
					{
					gpsMode = EGpsRequestModeHybrid;
					}
				else
					{
					gpsMode = EGpsRequestModeAutonomous;
					}
				break;
				}
			case KTerminalBasedMode:
				{
				if (gpsMode == EGpsRequestModeTerminalAssisted)
					{
					gpsMode = EGpsRequestModeHybrid;
					}
				else
					{
					gpsMode = EGpsRequestModeTerminalBased;
					}
				break;
				}
			case KTerminalAssistedMode:
				{
				if (gpsMode == EGpsRequestModeTerminalBased
					|| gpsMode == EGpsRequestModeAutonomous)
					{
					gpsMode = EGpsRequestModeHybrid;
					}
				else
					{
					gpsMode = EGpsRequestModeTerminalAssisted;
					}
				break;
				}
			}
		}
	
	return gpsMode;
	}


/** Convert a TGpsRequestMode enum into a TLbsNetPosRequestMethodInt.
*/
void CAgpsInterfaceHandler::ConvertGpsRequestModeToPosMethod(
		TGpsRequestMode aMode, 
		TLbsNetPosRequestMethodInt& aPosMethod)
	{
	TLbsNetPosMethodInt methods[2];
	switch (aMode)
		{
		case EGpsRequestModeAutonomous:
			{
			methods[0].SetPosMethod(KLbsPositioningMeansGpsInt, KAutonomousMode);
			aPosMethod.SetPosMethods(methods, 1);
			break;
			}
		case EGpsRequestModeTerminalBased:
			{
			methods[0].SetPosMethod(KLbsPositioningMeansGpsInt, KTerminalBasedMode);
			aPosMethod.SetPosMethods(methods, 1);
			break;
			}
		case EGpsRequestModeTerminalAssisted:
			{
			methods[0].SetPosMethod(KLbsPositioningMeansGpsInt, KTerminalAssistedMode);
			aPosMethod.SetPosMethods(methods, 1);
			break;
			}
		case EGpsRequestModeHybrid:
			{
			methods[0].SetPosMethod(KLbsPositioningMeansGpsInt, KTerminalBasedMode);
			methods[1].SetPosMethod(KLbsPositioningMeansGpsInt, KTerminalAssistedMode);
			aPosMethod.SetPosMethods(methods, 2);
			break;
			}

		case EGpsRequestModeUnknown:
		default:
			{
			GetDefaultPosMethod(aPosMethod);
			break;
			}
		}

	}

/** Calculate the new combined GPS mode when a new request is added.

The new combined GPS mode depends on a combination of:
1) The GPS mode(s) the module supports.
2) The current combined GPS mode.
3) The GPS mode of the new request.

@param aCurrentPosMethod [In] The GPS mode of the current combined request.
@param aNewRequestPosMethod [In] The GPS mode of the new request.
@param aCurrentPosMethod [Out] The GPS mode of the newly combined requests.

@return KErrNone if the GPS mode was combined successfully, KErrNotSupported
        if the GPS mode of the new request is not supported by the GPS module,
        KErrInUse if the new request GPS mode clashes with the current combined
        request GPS mode, e.g. if the module supports either TA or TB, but not
        both and the new request is TB but the current request is TA.
*/
TInt CAgpsInterfaceHandler::CalculateNewCombinedRequestMethod(
		TGpsRequestMode aCurrentGpsMode,
		TGpsRequestMode aNewRequestGpsMode,
		TGpsRequestMode& aCombinedGpsMode,
		TBool aEmergency)
	{
	// Table entry
	struct TGpsModeTableEntry
		{
		TGpsRequestMode iCurrentGpsMode;
		TGpsRequestMode iNewRequestGpsMode;
		TGpsRequestMode iCombinedGpsMode;
		TInt iErrorCode;
		};
	
	// Each of the tables below are taken directly from the 
	// NRH design document. If any of them are changed (e.g.
	// for a defect fix), the table in the design doc should
	// also be updated to match.

	// Module supports Terminal-Based only.
	static const TGpsModeTableEntry KGpsModeCombinationTableTB[] = 
	{
	// Current request mode				// New request mode				// Newly combined request mode	// Error
	{EGpsRequestModeUnknown,			EGpsRequestModeTerminalBased,	EGpsRequestModeTerminalBased,	KErrNone},
	{EGpsRequestModeUnknown,			EGpsRequestModeHybrid,			EGpsRequestModeTerminalBased,	KErrNone},
	{EGpsRequestModeUnknown,			EGpsRequestModeTerminalAssisted,EGpsRequestModeUnknown,			KErrNotSupported},
	{EGpsRequestModeUnknown,			EGpsRequestModeAutonomous,		EGpsRequestModeAutonomous,		KErrNone},
	
	{EGpsRequestModeAutonomous,			EGpsRequestModeTerminalBased,	EGpsRequestModeTerminalBased,	KErrNone},
	{EGpsRequestModeAutonomous,			EGpsRequestModeHybrid,			EGpsRequestModeTerminalBased,	KErrNone},
	{EGpsRequestModeAutonomous,			EGpsRequestModeTerminalAssisted,EGpsRequestModeUnknown,			KErrNotSupported},
	{EGpsRequestModeAutonomous,			EGpsRequestModeAutonomous,		EGpsRequestModeAutonomous,		KErrNone},
	
	{EGpsRequestModeTerminalBased,		EGpsRequestModeTerminalBased,	EGpsRequestModeTerminalBased,	KErrNone},
	{EGpsRequestModeTerminalBased,		EGpsRequestModeHybrid,			EGpsRequestModeTerminalBased,	KErrNone},
	{EGpsRequestModeTerminalBased,		EGpsRequestModeTerminalAssisted,EGpsRequestModeUnknown,			KErrNotSupported},
	{EGpsRequestModeTerminalBased,		EGpsRequestModeAutonomous,		EGpsRequestModeTerminalBased,	KErrNone}
	};
	static const TInt KGpsModeCombinationTableTBCount(sizeof(KGpsModeCombinationTableTB) 
													  / sizeof(TGpsModeTableEntry));

	// Module supports Terminal-Assisted only.
	static const TGpsModeTableEntry KGpsModeCombinationTableTA[] = 
	{
	{EGpsRequestModeUnknown,			EGpsRequestModeTerminalBased,	EGpsRequestModeUnknown,			KErrNotSupported},
	{EGpsRequestModeUnknown,			EGpsRequestModeHybrid,			EGpsRequestModeTerminalAssisted,KErrNone},
	{EGpsRequestModeUnknown,			EGpsRequestModeTerminalAssisted,EGpsRequestModeTerminalAssisted,KErrNone},
	{EGpsRequestModeUnknown,			EGpsRequestModeAutonomous,		EGpsRequestModeUnknown,			KErrNotSupported},

	{EGpsRequestModeTerminalAssisted,	EGpsRequestModeTerminalBased,	EGpsRequestModeUnknown,			KErrNotSupported},
	{EGpsRequestModeTerminalAssisted,	EGpsRequestModeHybrid,			EGpsRequestModeTerminalAssisted,KErrNone},
	{EGpsRequestModeTerminalAssisted,	EGpsRequestModeTerminalAssisted,EGpsRequestModeTerminalAssisted,KErrNone},
	{EGpsRequestModeTerminalAssisted,	EGpsRequestModeAutonomous,		EGpsRequestModeUnknown,			KErrNotSupported}
	};
	static const TInt KGpsModeCombinationTableTACount(sizeof(KGpsModeCombinationTableTA) 
													  / sizeof(TGpsModeTableEntry));
	
	// Module supports either Terminal-Assisted or Terminal-Based (but not both at the same time).
	static const TGpsModeTableEntry KGpsModeCombinationTableTBOrTA[] = 
	{
	{EGpsRequestModeUnknown,			EGpsRequestModeTerminalBased,	EGpsRequestModeTerminalBased,	KErrNone},
	{EGpsRequestModeUnknown,			EGpsRequestModeHybrid,			EGpsSpecialTreatmentForHybrid,	KErrNone},
	{EGpsRequestModeUnknown,			EGpsRequestModeTerminalAssisted,EGpsRequestModeTerminalAssisted,KErrNone},
	{EGpsRequestModeUnknown,			EGpsRequestModeAutonomous,		EGpsRequestModeAutonomous,		KErrNone},

	{EGpsRequestModeAutonomous,			EGpsRequestModeTerminalBased,	EGpsRequestModeTerminalBased,	KErrNone},
	{EGpsRequestModeAutonomous,			EGpsRequestModeHybrid,			EGpsRequestModeTerminalBased,	KErrNone},
	{EGpsRequestModeAutonomous,			EGpsRequestModeTerminalAssisted,EGpsRequestModeUnknown,			KErrInUse},
	{EGpsRequestModeAutonomous,			EGpsRequestModeAutonomous,		EGpsRequestModeAutonomous,		KErrNone},

	{EGpsRequestModeTerminalBased,		EGpsRequestModeTerminalBased,	EGpsRequestModeTerminalBased,	KErrNone},
	{EGpsRequestModeTerminalBased,		EGpsRequestModeHybrid,			EGpsRequestModeTerminalBased,	KErrNone},
	{EGpsRequestModeTerminalBased,		EGpsRequestModeTerminalAssisted,EGpsRequestModeUnknown,			KErrInUse},
	{EGpsRequestModeTerminalBased,		EGpsRequestModeAutonomous,		EGpsRequestModeTerminalBased,	KErrNone},

	{EGpsRequestModeTerminalAssisted,	EGpsRequestModeTerminalBased,	EGpsRequestModeUnknown,			KErrInUse},
	{EGpsRequestModeTerminalAssisted,	EGpsRequestModeHybrid,			EGpsRequestModeTerminalAssisted,KErrNone},
	{EGpsRequestModeTerminalAssisted,	EGpsRequestModeTerminalAssisted,EGpsRequestModeTerminalAssisted,KErrNone},
	{EGpsRequestModeTerminalAssisted,	EGpsRequestModeAutonomous,		EGpsRequestModeUnknown,			KErrInUse}
	};
	static const TInt KGpsModeCombinationTableTBOrTACount(sizeof(KGpsModeCombinationTableTBOrTA) 
														  / sizeof(TGpsModeTableEntry));

	// Module supports Terminal-Assisted and Terminal-Base simultaneously.
	static const TGpsModeTableEntry KGpsModeCombinationTableTBAndTA[] = 
	{
	{EGpsRequestModeUnknown,			EGpsRequestModeTerminalBased,	EGpsRequestModeTerminalBased,	KErrNone},
	{EGpsRequestModeUnknown,			EGpsRequestModeHybrid,			EGpsRequestModeHybrid,			KErrNone},
	{EGpsRequestModeUnknown,			EGpsRequestModeTerminalAssisted,EGpsRequestModeTerminalAssisted,KErrNone},
	{EGpsRequestModeUnknown,			EGpsRequestModeAutonomous,		EGpsRequestModeAutonomous,		KErrNone},

	{EGpsRequestModeAutonomous,			EGpsRequestModeTerminalBased,	EGpsRequestModeTerminalBased,	KErrNone},
	{EGpsRequestModeAutonomous,			EGpsRequestModeHybrid,			EGpsRequestModeHybrid,			KErrNone},
	{EGpsRequestModeAutonomous,			EGpsRequestModeTerminalAssisted,EGpsRequestModeHybrid,			KErrNone},
	{EGpsRequestModeAutonomous,			EGpsRequestModeAutonomous,		EGpsRequestModeAutonomous,		KErrNone},

	{EGpsRequestModeTerminalBased,		EGpsRequestModeTerminalBased,	EGpsRequestModeTerminalBased,	KErrNone},
	{EGpsRequestModeTerminalBased,		EGpsRequestModeHybrid,			EGpsRequestModeHybrid,			KErrNone},
	{EGpsRequestModeTerminalBased,		EGpsRequestModeTerminalAssisted,EGpsRequestModeHybrid,			KErrNone},
	{EGpsRequestModeTerminalBased,		EGpsRequestModeAutonomous,		EGpsRequestModeTerminalBased,	KErrNone},

	{EGpsRequestModeTerminalAssisted,	EGpsRequestModeTerminalBased,	EGpsRequestModeHybrid,			KErrNone},
	{EGpsRequestModeTerminalAssisted,	EGpsRequestModeHybrid,			EGpsRequestModeHybrid,			KErrNone},
	{EGpsRequestModeTerminalAssisted,	EGpsRequestModeTerminalAssisted,EGpsRequestModeTerminalAssisted,KErrNone},
	{EGpsRequestModeTerminalAssisted,	EGpsRequestModeAutonomous,		EGpsRequestModeHybrid,			KErrNone},
	
	{EGpsRequestModeHybrid,				EGpsRequestModeTerminalBased,	EGpsRequestModeHybrid,			KErrNone},
	{EGpsRequestModeHybrid,				EGpsRequestModeHybrid,			EGpsRequestModeHybrid,			KErrNone},
	{EGpsRequestModeHybrid,				EGpsRequestModeTerminalAssisted,EGpsRequestModeHybrid,			KErrNone},
	{EGpsRequestModeHybrid,				EGpsRequestModeAutonomous,		EGpsRequestModeHybrid,			KErrNone}
	};
	static const TInt KGpsModeCombinationTableTBAndTACount(sizeof(KGpsModeCombinationTableTBAndTA) 
														   / sizeof(TGpsModeTableEntry));
	
	// Decide which table to use
	const TGpsModeTableEntry* modeTable(NULL);
	TInt modeEntryCount(0);
	switch (iDeviceGpsModeCaps)
		{
		case TPositionModuleInfoExtended::EDeviceGpsModeTerminalBased:
			{
			modeTable = KGpsModeCombinationTableTB;
			modeEntryCount = KGpsModeCombinationTableTBCount;
			break;
			}
		case TPositionModuleInfoExtended::EDeviceGpsModeTerminalAssisted:
			{
			modeTable = KGpsModeCombinationTableTA;
			modeEntryCount = KGpsModeCombinationTableTACount;
			break;
			}
		case (TPositionModuleInfoExtended::EDeviceGpsModeTerminalBased
			  | TPositionModuleInfoExtended::EDeviceGpsModeTerminalAssisted):
			{
			modeTable = KGpsModeCombinationTableTBOrTA;
			modeEntryCount = KGpsModeCombinationTableTBOrTACount;
			break;
			}
		case TPositionModuleInfoExtended::EDeviceGpsModeSimultaneousTATB:
			{
			modeTable = KGpsModeCombinationTableTBAndTA;
			modeEntryCount = KGpsModeCombinationTableTBAndTACount;
			break;
			}
		}
	
	// Search the table to find a matching entry and get the new combined mode
	aCombinedGpsMode = EGpsRequestModeUnknown;
	TInt combinedReason(KErrNotSupported);
	if (modeTable)
		{
		for (TInt i = 0; i < modeEntryCount; i++)
			{
			if (modeTable->iCurrentGpsMode == aCurrentGpsMode
				&& modeTable->iNewRequestGpsMode == aNewRequestGpsMode)
				{
				aCombinedGpsMode = modeTable->iCombinedGpsMode;
				combinedReason = modeTable->iErrorCode;
				break;
				}
			
			modeTable++;
			}
		}
	
	if (aEmergency && (combinedReason == KErrInUse))
		{
		combinedReason = KErrNone;
		aCombinedGpsMode = aNewRequestGpsMode;
		
		}
	else if (combinedReason == KErrNone && aCombinedGpsMode == EGpsSpecialTreatmentForHybrid)
		{
		TLbsNetPosRequestMethodInt posMethod;
		GetPosSpecialTeatmentForHybrid(posMethod);
		aCombinedGpsMode = ConvertPosMethodToGpsRequestMode(posMethod);
		}
	else if (combinedReason == KErrNone && aCombinedGpsMode == EGpsRequestModeUnknown)

		{
		// If the new combined mode is not set but there was no error,
		// it means we should use the default mode from the admin settings.
		TLbsNetPosRequestMethodInt posMethod;
		GetDefaultPosMethod(posMethod);
		aCombinedGpsMode = ConvertPosMethodToGpsRequestMode(posMethod);
		}


	return combinedReason;
	}


/** Get GPS positioning mode based on admin settings for the case
 *	when the capabiliies are TA or TB but a hybrid request has arrived.

@param aPosMethod The default positioning method is written in to this parameter.

@return KErrNone usually, but one of the standard Symbian OS error codes if there
        was an error obtaining the positioning method.
*/
void CAgpsInterfaceHandler::GetPosSpecialTeatmentForHybrid(TLbsNetPosRequestMethodInt& aPosMethod)
	{	
	
	// Get the current network registration status.
	RLbsNetworkRegistrationStatus::TLbsNetworkRegistrationStatus status;
	TLbsAdminSetting setting;
	TInt err = iNetRegStatus.GetNetworkRegistrationStatus(status);
	if (KErrNone != err)
		{
		LBSLOG_WARN(ELogP3, 
			   "CAgpsInterfaceHandler::GetDefaultPosMethod: Failed to read network registration status, using RLbsNetworkRegistrationStatus::ENetworkRegistrationUnknown");
		status = RLbsNetworkRegistrationStatus::ENetworkRegistrationUnknown;
		}
	
	switch (status)
		{
		case RLbsNetworkRegistrationStatus::ERegisteredHomeNetwork:
			{
			setting = KLbsSettingHomeGpsMode;
			break;
			}
		case RLbsNetworkRegistrationStatus::ERegisteredRoamingNetwork:
		case RLbsNetworkRegistrationStatus::ENotRegistered:
		case RLbsNetworkRegistrationStatus::ENetworkRegistrationUnknown:
		default:
			{
			setting = KLbsSettingRoamingGpsMode;
			break;
			}
		}

	// Get the default mode from admin settings.
	CLbsAdmin::TGpsMode gpsMode;
	err = iAdmin.Get(setting, gpsMode);
	if (KErrNone != err)
		{
		LBSLOG_WARN2(ELogP3, 
				"CAgpsInterfaceHandler::GetDefaultPosMethod: Failed to read admin setting: 0x%x, using CLbsAdmin::EGpsModeUnknown",
				setting);
		gpsMode = CLbsAdmin::EGpsModeUnknown;
		}

	switch (gpsMode)
		{

		case CLbsAdmin::EGpsAutonomous:
		case CLbsAdmin::EGpsPreferTerminalBased:
		case CLbsAdmin::EGpsAlwaysTerminalBased:
			{
			TLbsNetPosMethodInt methodTB;
			methodTB.SetPosMethod(KLbsPositioningMeansGps, KTerminalBasedMode);
			aPosMethod.SetPosMethods(&methodTB, 1);
			break;
			}
		case CLbsAdmin::EGpsPreferTerminalAssisted:
		case CLbsAdmin::EGpsAlwaysTerminalAssisted:
			{
			TLbsNetPosMethodInt methodTA;
			methodTA.SetPosMethod(KLbsPositioningMeansGps, KTerminalAssistedMode);
			aPosMethod.SetPosMethods(&methodTA, 1);
			break;
			}
	
		default:
			{
			// Default to terminal based mode.
			TLbsNetPosMethodInt methodTB;
			methodTB.SetPosMethod(KLbsPositioningMeansGps, KTerminalBasedMode);
			aPosMethod.SetPosMethods(&methodTB, 1);
			break;
			}
		}
	}
/** Get the default GPS positioning mode, based on admin settings, etc.

@param aPosMethod The default positioning method is written in to this parameter.

@return KErrNone usually, but one of the standard Symbian OS error codes if there
        was an error obtaining the default positioning method.
*/
void CAgpsInterfaceHandler::GetDefaultPosMethod(TLbsNetPosRequestMethodInt& aPosMethod)
	{	
	
	// Get the current network registration status.
	RLbsNetworkRegistrationStatus::TLbsNetworkRegistrationStatus status;
	TLbsAdminSetting setting;
	TInt err = iNetRegStatus.GetNetworkRegistrationStatus(status);
	if (KErrNone != err)
		{
		LBSLOG_WARN(ELogP3, 
			   "CAgpsInterfaceHandler::GetDefaultPosMethod: Failed to read network registration status, using RLbsNetworkRegistrationStatus::ENetworkRegistrationUnknown");
		status = RLbsNetworkRegistrationStatus::ENetworkRegistrationUnknown;
		}
	
	switch (status)
		{
		case RLbsNetworkRegistrationStatus::ERegisteredHomeNetwork:
			{
			setting = KLbsSettingHomeGpsMode;
			break;
			}
		case RLbsNetworkRegistrationStatus::ERegisteredRoamingNetwork:
		case RLbsNetworkRegistrationStatus::ENotRegistered:
		case RLbsNetworkRegistrationStatus::ENetworkRegistrationUnknown:
		default:
			{
			setting = KLbsSettingRoamingGpsMode;
			break;
			}
		}

	// Get the default mode from admin settings.
	CLbsAdmin::TGpsMode gpsMode;
	err = iAdmin.Get(setting, gpsMode);
	if (KErrNone != err)
		{
		LBSLOG_WARN2(ELogP3, 
				"CAgpsInterfaceHandler::GetDefaultPosMethod: Failed to read admin setting: 0x%x, using CLbsAdmin::EGpsModeUnknown",
				setting);
		gpsMode = CLbsAdmin::EGpsModeUnknown;
		}

	switch (gpsMode)
		{
		case CLbsAdmin::EGpsPreferTerminalBased:
		case CLbsAdmin::EGpsAlwaysTerminalBased:
			{
			TLbsNetPosMethodInt methodTB;
			methodTB.SetPosMethod(KLbsPositioningMeansGpsInt, KTerminalBasedMode);
			aPosMethod.SetPosMethods(&methodTB, 1);
			break;
			}
		case CLbsAdmin::EGpsPreferTerminalAssisted:
		case CLbsAdmin::EGpsAlwaysTerminalAssisted:
			{
			TLbsNetPosMethodInt methodTA;
			methodTA.SetPosMethod(KLbsPositioningMeansGpsInt, KTerminalAssistedMode);
			aPosMethod.SetPosMethods(&methodTA, 1);
			break;
			}
		case CLbsAdmin::EGpsAutonomous:
			{
			TLbsNetPosMethodInt methodA;
			methodA.SetPosMethod(KLbsPositioningMeansGpsInt, KAutonomousMode);
			aPosMethod.SetPosMethods(&methodA, 1);
			break;
			}
		default:
			{
			// Default to terminal based mode.
			TLbsNetPosMethodInt methodTB;
			methodTB.SetPosMethod(KLbsPositioningMeansGpsInt, KTerminalBasedMode);
			aPosMethod.SetPosMethods(&methodTB, 1);
			break;
			}
		}
	}

