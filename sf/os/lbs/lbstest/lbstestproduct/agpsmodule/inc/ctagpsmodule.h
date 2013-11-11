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
// This is the header file which contains the test version of a intergration module
//

//! @file ctagpsmodule.h

#ifndef __CT_AGPS_MODULE_H__
#define __CT_AGPS_MODULE_H__


// Epoc includes

// LBS includes
#include <lbs.h>
#include <lbscommon.h>
#include <lbspositioninfo.h>
#include <lbs/lbslocdatasourcegpsbase.h>

// LBS test includes
#include "ctagpsmoduledata.h"
#include <lbs/test/tlbsutils.h>
#include <lbs/test/ctlbstimerutils.h>

// Forward Declaration
const TInt KPosUpdateTimerId = 1;
const TInt KDelayUpdateTimerId = 2;
const TInt KDelayStatusTimerId = 3;

class CT_AGpsModule : public CLbsLocationSourceGpsBase, MT_LbsTimerUtilsObserver, MT_AGpsModuleDataBusObserver
	{	
public:
	static CLbsLocationSourceGpsBase* NewL(MLbsLocationSourceGpsObserver& aObserver);
	~CT_AGpsModule();

	// CLbsLocationSourceGpsBase functions.
	void RequestLocationUpdate(const TTime& aTargetTime, const TLbsLocRequestQuality& aQuality);
	void CancelLocationRequest();
	void AdvisePowerMode(CLbsLocationSourceGpsBase::TPowerMode aMode);
	void SetGpsOptions(const TLbsGpsOptions& aGpsOptions);
	void AssistanceDataEvent(TInt aError, TLbsAsistanceDataGroup aDataMask);

	// MT_AGpsModuleTimerObserver function.
	void HandleTimerL(TInt aTimerId, const TTime& aTargetTime);

	// MT_AGpsModuleDataBusObserver function.
	 void HandleDataBusUpdateL(const TModuleDataIn& aModuleDataIn);
		
protected:
	CT_AGpsModule(MLbsLocationSourceGpsObserver& aObserver);
	void ConstructL();

private:

	// Helper verify assistance data functions.
	TBool VerifyRefTimeData();
	TBool VerifyRefLocationData();

	// Helper function to return position info data.
	void UpdateLocation();

	TLbsGpsOptions iGpsOptions;

	/** Position update timer used to handle position update requests from manager. */
	CT_LbsTimerUtils* iPosUpdateTimer;
	
	/** Used to cause the module to delay returning the actual position, to reflect a more
		realistic behaviour when receiving assistance data and returning positions. */
	CT_LbsTimerUtils* iDelayUpdateTimer;

	/** Used to delay sending status to manager */
	CT_LbsTimerUtils* iDelayStatusTimer;


	CT_AGpsModuleDataBusMonitor* iModuleDataBusMonitor;
	
	RPointerArray<TAny>	iPosInfoArr;
	TInt iPosInfoIndex;
	
	TPositionModuleInfo iModuleInfo;
	TPositionModuleStatus iModuleStatus;
	//whether to delay reporting module status until we're asked to (defaults to not delaying)
	TBool iDelayReportingModStatus;	
	TPositionModuleStatusEvent iModuleStatusEvent;
	
	TModuleDataIn::TAssDataEventType	iAssDataEventType;
	TModuleDataIn::TTestMode 			iAssDataTestMode;
	TModuleDataIn::TGpsOptionsChangeReportMode iOptionsChangeReportMode;
	
	TTimeIntervalMicroSeconds iTimeOut;
	// whether timeout should apply to only the next Request:
	TBool	iOneOffTimeOut;
	TBool	iFirstRequestPast;
	
	TInt iError;
	
	TInt iImmediateMeasurements;
	
	/** Used to alter the time stamp of the posinfo. */
	TTimeIntervalMicroSeconds iTimeStampOffset;

	/** Target time. */
	TTime iTargetTime;

	/**
	*/
	TBool iAssistanceDataRequested;
	
	/** Flag used to determine if a assistance data request is required.
	
	Set to ETrue is request not required.
	Set to EFalse if the assistance data has not been already requested.
	*/
	TBool iAssistanceDataAvailable;
	
	/** Request time, used to record the time the request for assistance was made, to
		allow validation of assistance data time stamp.
	*/
	TTime iAssistanceDataRequestedTime;
	
	};

#endif //__CT_AGPS_MODULE_H__
