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
// Definition of main logoc of agps manager
// 
//

/**
 @file
 @internalTechnology
*/

#ifndef MAINLOGIC_H
#define MAINLOGIC_H

#include <e32base.h>
#include <lbs.h>
#include <lbs/lbsextendedsatellite.h>
#include <lbs/lbsassistancedatabase.h>
#include <lbs/lbslocdatasourcegpsbase.h>
#include <lbs/epos_intgpshwstatus.h>

#include "LbsInternalInterface.h"
#include "lbscommoninternaldatatypes.h"
#include "lbsrootapi.h"

#include "lbsprocesssupervisor.h"
#include "lbstimer.h"
#include "lbslocationrequesthandler.h"
#include "lbsassistancedatacacheapi.h"
#include "lbsnetworkgatewayhandler.h"
#include "cquietus.h"
#include "lbsautoclockadjust.h"
#include "lbsearlycompletionupdatehandler.h"
#include "LbsLogger.h"
#include "lbsnetregstatusint.h"
#if defined(_DEBUG)
#include "LbsOomListener.h"
#endif

class CLbsAdmin;


const TPositionModuleId KSirfModuleId= {0x10282249}; // Ecom plugin implementation Id


//************************************************************************************************************
//	CManagerMainLogic
//************************************************************************************************************
class CManagerMainLogic :	public CBase,
							public MLbsLocationSourceGpsObserver,
							public MLbsCallbackTimerObserver,
							public MLocationRequestHandlerObserver,
							public MNetworkGatewayHandlerObserver,
							public MLbsProcessCloseDown,
							public MLbsEarlyCompletionUpdateObserver
	{
public:
	static CManagerMainLogic* NewL();
	~CManagerMainLogic();
public:

	enum TLocReqStatus
		{
		EReqNone 			 = 0,
		EReqReceived 		 = 1,
		EReqSessionStarted 	 = 2,
		EReqReceivedDuringSession = 3,
		EReqSessionCompleted = 4
		};

	class TSessionClosureData
		{
	public:
		TInt iLastStatus;
		TPositionExtendedSatelliteInfo iLastPosInfo;
		TTime iLastTargetTime;
		TBool iNotify;
		};

public:
	// from MLbsLocationSourceGpsObserver
	TVersion Version();
	void UpdateLocation(TInt aStatus, const TPositionInfoBase* aPosition[], TInt aNumItems, const TTime& aTargetTime);
	void UpdateDeviceStatus(TPositionModuleStatus::TDeviceStatus aDeviceStatus);
	void UpdateDataQualityStatus(TPositionModuleStatus::TDataQualityStatus aDataQuality);
	TInt GetAssistanceDataItem(TLbsAssistanceDataItem aItem, RDataReaderRootBase& aDataRoot, TTime& aTimeStamp);
	TInt GetAssistanceDataItemTimeStamp(TLbsAssistanceDataItem aItem, TTime& aTimeStamp);
	void RequestAssistanceData(TLbsAsistanceDataGroup aDataItemMask);
	void Shutdown();

	void NotifyServiceToClose(TRequestStatus* aStatus);
	
	// from MLocationRequestHandlerObserver
	void OnLocationRequestStateChanged(const RLbsPositionUpdateRequests::TChannelIdentifer& aChannel,const TRequestStateChangeReason& aReason);
	void OnSystemStatusAdvice(const RLbsPositionUpdateRequests::TChannelIdentifer& aChannel, TBool aTracking);
	void OnPowerModeAdvice(const RLbsPositionUpdateRequests::TChannelIdentifer& aChannel, CLbsLocationSourceGpsBase::TPowerMode& aPowerMode);
	// from MNetworkGatewayHandlerObserver
	void OnAssistanceDataResponse(TInt aError, TLbsAsistanceDataGroup aMask);
    void OnSessionComplete(TInt aReason, const TLbsNetSessionIdInt& aSessionId);
	// from MLbsCloseDownObserver
	void OnProcessCloseDown();
	
	// from MLbsCallbackTimerObserver
	virtual void OnTimerEventL(TInt aTimerId);
	virtual TInt OnTimerError(TInt aTimerId, TInt aError);
	
	// from MLbsEarlyCompletionUpdateObserver
	virtual void EarlyCompletionUpdate(TPositionInfo& aEarlyCompletionUpdate, TTime& aTargetTime, TTime& aActualTime);


private:

	CManagerMainLogic();
	void ConstructL();
	void CombineLocationRequests(const TUid& aOriginatingChannel, const TRequestStateChangeReason& aOriginatorReason = EReasonNone);
	void ConfigureGpsMode(const TUid& aChannelId, const TRequestStateChangeReason& aReason);

	void GetTimeAndQuality(const CLocationRequestHandler& aRequestHandler, TTime& aStartTime, TLbsLocRequestQualityInt& aQuality);
	TPositionModuleInfo::TTechnologyType GetTechnologyTypeFromNrhRequest(const TInt& aIndex);
	TPositionModuleInfo::TTechnologyType GetTechnologyTypeFromLsRequest(const TInt& aIndex);

	void RestartTimerIfTracking();
	void SendSystemStatusAdvice(TBool aTracking);
	void GetFinalNetPositionLagFromAdminProfileL();	
	void SetGpsOptions(const TPositionModuleInfo::TTechnologyType aNewPositioningMethod);
	TBool IsUpdateAsExpected(const TPositionInfoBase* aPosInfoArray[],const TInt& aNumItems, TInt& aPositionCalculationIndex, TInt& aMeasurementIndex);
	void GetGpsUpdateIndexes(const TPositionInfoBase* aPosInfoArray[], const TInt& aNumItems, TInt& aPositionCalculationIndex, TInt& aMeasurementIndex);
	void PublishDummyPosition(TInt aReason);
	void PublishDummyAssistDataEvent(TInt aReason);
	void ConvertToTLbsLocRequestQuality(const TLbsLocRequestQualityInt& aSource, TLbsLocRequestQuality& aDest) const;
	void GetTaGpsModeFromAdmin(CLbsAdmin::TGpsMode& aGpsMode);
	TPositionModuleInfo::TTechnologyType CalculatePositioningMethod();
	TPositionModuleInfo::TTechnologyType LookUpPositioningMethodInCombinationTables();

	void CalculateActiveLSMode(const TUid& aChannelId, const TRequestStateChangeReason& aReason, TBool& aForceSelfLocate);
	
private:
	TBool iClosingDown;
	CLbsCloseDownRequestDetector*	iCloseDownRequestDetector;
	CQuietus*					iQuietus;

	CLbsLocationSourceGpsBase*	iLocationSource;
	CNetworkGatewayHandler*		iNetworkGatewayHandler;
	CLocationRequestHandler*	iNRHLocationRequestHandler;
	CLocationRequestHandler*	iLocSvrLocationRequestHandler;
	CEarlyCompletionUpdateHandler* iEarlyCompletionUpdateHandler;
	
	CLbsCallbackTimer*			iTimer;

	
	CAutoClockAdjust*			iAutoClockAdjust;
	RLbsPositionUpdates 		iPositionUpdates;// For publishing position updates for Loc Server and NRH.
	RLbsGpsMeasurementUpdates	iMeasurementUpdates;
	RLbsModuleStatus			iModuleStatus;
	
	TLocReqStatus				iLSReqState;
	TBool       				iIsNrhCancelDue; // A NRH request must be cancelled even if it has timed out
	TLbsNetSessionIdInt			iSessionId;
	TBool						iLastTrackingFlagSentToNG;
	TBool						iTrackingStateUnknown;
	// LBSLogging
	RLbsLogger					iLogger;
	TLbsAsistanceDataGroup		iLogAssitaceDataGroup;

	
	// Position update cache (for republishing in some scenarios only)
	TSessionClosureData         iSessionClosureData;
	
	TTimeIntervalMicroSeconds 	iFinalNetPositionLag;	
	CLbsAdmin*                  iAdmin;
	
	//Stores the last power advice recieved
	CLbsLocationSourceGpsBase::TPowerMode iPowerModeLS;
	CLbsLocationSourceGpsBase::TPowerMode iPowerModeNRH;

	// the current behaviour mode of Lbs
	CLbsAdmin::TLbsBehaviourMode iLbsBehaviourMode;
	
	// Capabilities of underlying GPS hardware regarding
	// positioning methods it supports
	TPositionModuleInfoExtended::TDeviceGpsModeCapabilities iCapabilities;
	
	RLbsNetworkRegistrationStatus iNetRegStatus;
	
	// Positioning method used last to configure GPS options
	TPositionModuleInfo::TTechnologyType iCurrentPositioningMethod;
	
	// Positioning method last used to configure the GPS module
	// options due to a Location Request from the Location server.
	// It is set to KLbsMethodNone when the Location server reques goes inactive.
	TPositionModuleInfo::TTechnologyType iActiveLSMode;
	
	// Positioning method last sent to the Network Gateway in
	// a SelfLocateRequest. It is set to KLbsMethodNone
	// when self location session terminates.
	TPositionModuleInfo::TTechnologyType iCurrentNetworkMode;

    #if defined(_DEBUG)
		CLbsOomListener*	iOomListener;
    #endif	
	
	// Flag which gets set when in Prefer Terminal Assisted and there is 
	// an error from the network. This flag is needed because the AGPS manager 
	// always ensures positions/measurements are returned correctly. So it 
	// would typically wait for measurements when in PTA. But in the case 
	// of a fatal network error the module can fall-back to Autonomous. In that 
	// case the PTA mode is set and positions are returned. So we need this 
	// flag to know when we can accept positions in PTA.
	TBool iFallbackFromPTAToAutonomousPossible;
	// Flag that specifies if currently gps timing of cell frames is requested 
	TBool iCurrentGpsTimingOfCellFramesRequested;
	
	CPosIntGpsHwStatus* 	iPosIntGpsHwStatus;

	};


#endif //MAINLOGIC_H

