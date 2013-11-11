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
// This is the header file which contains the test version of a intergration module
// that supports hybrid mode.
//

//! @file ctagpshybridmodule.h

#ifndef CTAGPSHYBRIDMODULE_H
#define CTAGPSHYBRIDMODULE_H


// LBS includes
#include <lbs.h>
#include <lbscommon.h>
#include <lbspositioninfo.h>
#include <lbs/lbslocdatasourcegpsbase.h>
#include <lbs/lbslocdatasourceclasstypes.h>
#include <lbssatellite.h>
#include <lbs/lbsgpsmeasurement.h>

// LBS test includes
#include <lbs/test/tlbsutils.h>
#include <lbs/test/ctlbstimerutils.h>
#include "ctestharnesshandler.h"


class CT_AGpsHybridModule : public CLbsLocationSourceGpsBase, MT_LbsTimerUtilsObserver, MT_RequestObserver
	{	
public:
	static CLbsLocationSourceGpsBase* NewL(MLbsLocationSourceGpsObserver& aObserver);
	~CT_AGpsHybridModule();

	// CLbsLocationSourceGpsBase functions.
	void RequestLocationUpdate(const TTime& aTargetTime, const TLbsLocRequestQuality& aQuality);
	void CancelLocationRequest();
	void AdvisePowerMode(CLbsLocationSourceGpsBase::TPowerMode aMode);
	void SetGpsOptions(const TLbsGpsOptions& aGpsOptions);
	void AssistanceDataEvent(TInt aError, TLbsAsistanceDataGroup aDataMask);

	// MT_AGpsModuleTimerObserver function.
	void HandleTimerL(TInt aTimerId, const TTime& aTargetTime);

	// MT_RequestObserver
	TInt ProcessRequestUpdateInit(const TDesC& aConfigFileName, const TDesC& aConfigSection);
	void ProcessRequestTimeOut(const TTimeIntervalMicroSeconds& aTimeOut);
	void ProcessRequestError(TInt aError);
	TInt ProcessRequestModuleOptions(TLbsHybridModuleOptions aModuleMode, TBool aValue);
	void ProcessRequestForcedUpdate();
	void ProcessImmediateMeasurements(TInt aImmediateMeasurements);
			
protected:
	CT_AGpsHybridModule(MLbsLocationSourceGpsObserver& aObserver);
	void ConstructL();

private:
	// Helper verify assistance data functions.
	TBool VerifyRefTimeData();
	TBool VerifyRefLocationData();

	// Helper function to return position info data.
	void UpdateLocation();
	void ReturnNanPosition();
	void ReturnNanMeas();

	// Populates update with dummy data.
	void PopulateUpdateArrayL();

	// Module options functions.
	void SetOption(TLbsHybridModuleOptions aModuleOption, TBool aValue);
	TBool GetOption(TLbsHybridModuleOptions aModuleOption);
	
	void DumpUpdateToRefFile(const TPositionInfoBase *aPosInfoArray[], TInt aNumItems);
	void PublishGpsOptions(const TLbsGpsOptions& aGpsOptions);
	void SignalResetAssitanceDataReceived();

	void ClearAssistanceData();
	
private:
	/** Module options held in a 32 bitmap word.
	*/
	TUint32 iModuleOptions;

	/** Module A-GPS options and mode.
	*/
	TLbsGpsOptionsArray iGpsOptions;
	
	/** Indication of the current A-GPS options class type in use.
	*/
	TLbsGpsOptionsClassType iGpsOptionsType;

	/** Position update timer used to handle position update requests from manager. */
	CT_LbsTimerUtils* iPosUpdateTimer;
	
	/** Used to cause the module to delay returning the actual position, to reflect a more
		realistic behaviour when receiving assistance data and returning positions. */
	CT_LbsTimerUtils* iDelayUpdateTimer;


	/** Interval variables required by the multi-update module option.
	*/
	TTimeIntervalMicroSeconds iIntervalDelay;
	TInt iExtraUpdatesCtr;


	/** Communication channel between A-Gps module and Test Harness.
	*/
	CT_TestHarnessHandler* iTHHandler;

	RPointerArray<TLbsModuleUpdateItem> iUpdateArr;
	TInt iUpdateArrIndex;
	
	TPositionModuleStatus iModuleStatus;
	
	TTimeIntervalMicroSeconds iTimeOut;
	TInt iError;
	TInt iImmediateMeasurements;
	
	/** Target time. */
	TTime iTargetTime;

	/** Quality of the request. Only interested in max fix time.
	*/
	TLbsLocRequestQuality iQuality;

	/** Flag used to determine if a assistance data request is required.
	
	Set to ETrue if assistance data is required.
	Set to EFalse if assistance is not required.
	*/
	TBool iAssistanceDataOn;

	/** Flag used to determine if assistance has been requested.

	Set to ETrue if assistance data has already been requested.
	Set to EFalse if assistance data has not been requested.
	*/
	TBool iAssistanceDataRequested;
	

	/** Flag used to determine if a assistance data is already available.
	
	Set to ETrue if it is available.
	Set to EFalse if assistance data has not been already received.
	*/
	TBool iAssistanceDataAvailable;
	
	/** Request time, used to record the time the request for assistance was made, to
		allow validation of assistance data time stamp.
	*/
	TTime iAssistanceDataRequestedTime;
	
	TBool iGPSModeNotSupported;
	
	/** List of the assistance data that we have requested but not yet received */
	TLbsAsistanceDataGroup iOutstandingAssistanceData;
	};

#endif // CTAGPSHYBRIDMODULE_H
