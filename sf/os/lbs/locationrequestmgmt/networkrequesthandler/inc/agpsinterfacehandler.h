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

#ifndef AGPSINTERFACEHANDLER_H_
#define AGPSINTERFACEHANDLER_H_

#include "lbscommoninternaldatatypes.h"
#include <lbs/lbsnetprotocolbase.h>
#include <lbs/lbslocdatasourcegpsbase.h>
#include "locationupdatemonitor.h"
#include "measurementinfomonitor.h"
#include "lbsnetinternalapi.h"

class RLbsNetworkRegistrationStatus;

/** Represents TLbsNetPosRequestMethod in a single enum value.

This enum is used to summarise the positioning methods contained
in a TLbsNetPosRequestMethod into a single enum value, to make it
easier to compare, etc.
*/
typedef TUint TGpsRequestMode;

enum _TGpsRequestMode
	{
	// Unkown/undefined request mode
	EGpsRequestModeUnknown = 0,
	// Autonomous (basic GPS) mode.
	EGpsRequestModeAutonomous,
	// Terminal-based (Assisted GPS) mode.
	EGpsRequestModeTerminalBased,
	// Terminal-assisted (Assisted GPS) mode.
	EGpsRequestModeTerminalAssisted,
	// Simultaneous TA and TB (hybrid GPS) mode.
	EGpsRequestModeHybrid,
	// Special case where the capabilites are TA or TB and hybrid mode has arrived (when no request active)
	// and we look at the admin AGPS setting to decide whther to us TA or TB mode.
	// if admin says A or PTB then choose TB
	// if admin says PTA or ATA then choose TA
	EGpsSpecialTreatmentForHybrid
	};


/** States that a location request can be in.

Note: This enum is used as an index into the power mode
      advice look-up table in 
      CAgpsInterfaceHandler::CalculateCombinedPowerModeAdvice(),
      so if any values are added or removed from this, be
      careful to also update the look-up table or
      it will be out of sync with the enum values.
*/
typedef TUint TAgpsRequestState;

enum _TAgpsRequestState
	{
	// Unknown/undefined request state.
	EAgpsRequestStateUnknown = 0,
	// Idle state - exists but not sent to the AGPS manager.
	EAgpsRequestStateIdle,
	// Location request is on hold, got a response but waiting to be stopped.
	EAgpsRequestStateHold,
	// Location request has been sent to the AGPS manager.
	EAgpsRequestStateActive
	};


/** Small data class that records the parameters for a location request.

Most of the params are stored as pointers, to save memory. They
should point to the actual data within the CLbsPrivLocFsm that
made the location request.
*/
class TAgpsRequest
	{
public:
	TAgpsRequest() :
		iSessionId(0),
		iState(EAgpsRequestStateIdle),
		iPosQuality(0),
		iGpsMode(EGpsRequestModeUnknown),
		iEndTime(0),
		iGpsTimingOfCellFramesRequested(EFalse)
			{}

public:
	const TLbsNetSessionIdInt* iSessionId;
	TAgpsRequestState iState;
	const TLbsNetPosRequestQualityInt* iPosQuality;
	TGpsRequestMode iGpsMode;
	TTime iEndTime;
	TBool iGpsTimingOfCellFramesRequested;
	};

	
/** Observer interface for the privacy and location handler to use.

The callbacks in this class pass data back from the AGPS interface
back to the privacy and location handler, e.g. location updates,
measurement data updates.
*/
class MAgpsInterfaceHandlerObserver
	{
public:
	/** Called when a valid AGPS position update arrives.
	*/
	virtual void OnAgpsPositionUpdate(
			TInt aReason,
			const TPositionExtendedSatelliteInfo& aPosInfo,
			const TTime& aTimeStamp) = 0;

	/** Called when a measurement results update arrives.
	*/
	virtual void OnAgpsMeasurementUpdate(
			TInt aReason,
			const TPositionGpsMeasurementInfo& aPosInfo,
			const TTime& aTimeStamp) = 0;
	};


/** Describes the interface for setting the current X3P handler status.

The X3P handler needs to tell the AGPS interface handler what state it
is in, so that the correct power advice can be sent to the AGPS manager.
It uses SetX3pStatus() to do that.
*/
class MX3pStatusHandler
	{
public:
	/** The current state of the X3P handler.
	
	Note: This enum is used as an index into the power mode
      advice look-up table in 
      CAgpsInterfaceHandler::CalculateCombinedPowerModeAdvice(),
      so if any values are added or removed from this, be
      careful to also update the look-up table or
      it will be out of sync with the enum values.
    */
    typedef TUint TX3pStatus;
    
	enum _TX3pStatus
		{
		// No active X3P requests and no connected clients.
		EX3pStatusIdle = 0,
		// No active X3P requests but at least one connected client.
		EX3pStatusConnected,
		// One or more active X3P request(s).
		EX3pStatusActive
		};

public:
	/** Set the current X3P handler status.
	*/
	virtual void SetX3pStatus(MX3pStatusHandler::TX3pStatus aStatus) = 0;
	};


/** Handles the connection to the AGPS manager.

This class is responsible for sending location requests,
location cancels and power mode advice to the AGPS manager.

In response, it handles listening for and reporting location
updates and measurement results to the privacy and location 
handler.

Part of the task of sending location requests is having to
merge requests from multiple simultaneous sessions into a
single request for the AGPS manager. Currently, 3 simultaneous
requests would be the maximum - one each of MO-LR, X3P, MT-LR/NI-LR.

When calculating the current power mode advice, the status
of the X3P handler is merged with the status of any current
location requests.
*/
class CAgpsInterfaceHandler : public CBase,
							  public MLbsLocUpdateObserver,
							  public MLbsMeasurementInfoObserver,
							  public MX3pStatusHandler
	{
public:
	static CAgpsInterfaceHandler* NewL(MAgpsInterfaceHandlerObserver& aObserver,
									   CLbsAdmin& aAdmin,
									   RLbsNetworkRegistrationStatus& aNetRegStatus);
	~CAgpsInterfaceHandler();

public:
	TInt PreStartPositioning(const TLbsNetSessionIdInt& aSessionId,TBool aEmergency);
	TInt StartPositioning(const TLbsNetSessionIdInt& aSessionId,
			const TLbsNetPosRequestMethodInt& aMethod,
			const TLbsNetPosRequestQualityInt& aQuality,
			TBool aEmergency);
	void HoldPositioning(const TLbsNetSessionIdInt& aSessionId, 
			TInt aReason);
	void StopPositioning(const TLbsNetSessionIdInt& aSessionId);
	
	TInt GetPosition(TInt& aReason,
	        TPositionExtendedSatelliteInfo& aPosInfo,
			TTime& aTimeStamp);
	void GetMeasurement(TInt& aReason,
			TPositionGpsMeasurementInfo& aPosInfo,
			TTime& aTimeStamp);
	
	void GetDefaultPosMethod(TLbsNetPosRequestMethodInt& aPosMethod);
	void GetPosSpecialTeatmentForHybrid(TLbsNetPosRequestMethodInt& aPosMethod);
	
	TGpsRequestMode ConvertPosMethodToGpsRequestMode(const TLbsNetPosRequestMethodInt& aPosMethod);

public: // from MX3pStatusHandler
	void SetX3pStatus(MX3pStatusHandler::TX3pStatus aStatus);
	
protected:
	// from MLbsLocUpdateObserver
	void OnPositionUpdate(
				TBool aConflictControl,
				TInt aReason,
				const TPositionExtendedSatelliteInfo& aPosInfo,
				const TTime& aActualTime);

	// from MLbsMeasurementInfoObserver
	void OnMeasurementInfoUpdate(
				TInt aReason, 
				const TPositionGpsMeasurementInfo& aMeasurementResults,
				const TTime& aActualTime);
	
protected:
	CAgpsInterfaceHandler(MAgpsInterfaceHandlerObserver& aObserver,
						  CLbsAdmin& aAdmin,
						  RLbsNetworkRegistrationStatus& aNetRegStatus);

private:
	CAgpsInterfaceHandler();
	void ConstructL();
	
protected:
	void CalculateCombinedRequestQuality();
	

	void ConvertGpsRequestModeToPosMethod(TGpsRequestMode aMode, TLbsNetPosRequestMethodInt& aPosMethod);
	TInt CalculateNewCombinedRequestMethod(TGpsRequestMode aCurrentGpsMode,
										   TGpsRequestMode aNewRequestGpsMode,
										   TGpsRequestMode& aCombinedGpsMode,
										   TBool aEmergency);
	
	void CalculateCombinedPowerModeAdvice(
			TLbsPositionUpdateRequestBase::TPowerAdvice& aCombinedPowerAdvice);
	TBool CalculateCombinedTimingOfCellFramesRequested();
	
	void SendCombinedLocationRequest();
	
	TBool CheckPositionUpdateIsValid(
				TBool aConflictControl,
				TInt aReason,
				const TPositionExtendedSatelliteInfo& aPosInfo,
				const TTime& aActualTime);

protected:
	static TBool IsSessionIdEqual(
			const TLbsNetSessionIdInt* aSessionId, 
			const TAgpsRequest& aRequest);
	
	static TBool IsRequestStateEqual(
			const TAgpsRequestState* aState, 
			const TAgpsRequest& aRequest);
	
	TInt AddOrUpdateRequest(const TAgpsRequest& aRequest, TBool aEmergency);
	void RemoveRequest(const TLbsNetSessionIdInt& aSessionId);
	TBool IsAnyRequestState(TAgpsRequestState aState);

private:
	MAgpsInterfaceHandlerObserver& iObserver;
	CLbsAdmin& iAdmin;
	RLbsNetworkRegistrationStatus& iNetRegStatus;
	CLbsLocUpdateMonitor* iLocationUpdateMonitor;
	CLbsMeasurementInfoMonitor* iMeasurementUpdateMonitor;
	
	MX3pStatusHandler::TX3pStatus iX3pStatus;
	
	/** This is the A-GPS mode(s) supported by the plug-in A-GPS module. */
	TPositionModuleInfoExtended::TDeviceGpsModeCapabilities iDeviceGpsModeCaps;
	
	/** Records whether we have sent a location request and are currently waiting for a response. */
	TBool iLocationRequestActive;

	TLbsPositionUpdateRequestBase::TPowerAdvice iCombinedPowerModeAdvice;
	TGpsRequestMode iCombinedRequestGpsMode;
	TLbsLocRequestQualityInt iCombinedRequestQuality;
	TUint iMaxNumLocationRequests;
	RArray<TAgpsRequest> iRequests;
	};

#endif /*AGPSINTERFACEHANDLER_H_*/
