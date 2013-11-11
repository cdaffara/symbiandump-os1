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

#ifndef LBSLOCSOURCEGPS_H
#define LBSLOCSOURCEGPS_H

//************************************************************************************************************
#include <lbs.h>
#include <lbs/lbslocdatasourcegpsbase.h>
#include "LbsLocSourceTestChannel.h"
#include "lbstimer.h"
#include <e32property.h>

//#include "LbsLasUpdateSource.h"

class TLbsGpsOptions;
class CLasUpdateSource;


//************************************************************************************************************
//   CLbsLocationSourceGps
//************************************************************************************************************
class CLbsLocationSourceGps : public CLbsLocationSourceGpsBase, private MLbsCallbackTimerObserver
	{
   public:
   	IMPORT_C static CLbsLocationSourceGpsBase* NewL(MLbsLocationSourceGpsObserver& aObserver);
   	
	~CLbsLocationSourceGps();
   	
   public:
   	void SetGpsOptions(const TLbsGpsOptions& aGpsOptions);
   	
   	void RequestLocationUpdate(const TTime& aTargetTime, const TLbsLocRequestQuality& aQuality);
   	
   	void CancelLocationRequest();

		void AdvisePowerMode(CLbsLocationSourceGpsBase::TPowerMode aMode);
		
		void AssistanceDataEvent(TInt aError, TLbsAsistanceDataGroup aDataMask);
		
		
		void HandleLocationUpdate(const TPositionInfoBase& aPosInfo);
		
		void HandleDeviceStatusUpdate(TPositionModuleStatus::TDeviceStatus aDeviceStatus);

		void HandleQualityStatusUpdate(TPositionModuleStatus::TDataQualityStatus aDataQuality);
		
		void Shutdown();
		
		void NotifyServiceToClose(TRequestStatus* aStatus);

	private:
		CLbsLocationSourceGps(MLbsLocationSourceGpsObserver& aObserver);
		void ConstructL();

		//From MCallbackTimerObserver	
		virtual void OnTimerEventL(TInt aTimerId);
		virtual TInt OnTimerError(TInt aTimerId, TInt aError);
		
	private:
		enum TAction
			{
			EActionNone,
			EActionRequestAssistanceData,
			EActionPublishLocationUpdate,
			EActionShutdownModule
			};

	private:
		RAgpsModuleTestChannel	iTestChannel;
		CLbsCallbackTimer* 		iTimer;
		TAction					iPendingAction;  
		TTime					iTargetTime;
		TBool iHybridMode;
		RProperty iUpdateCodeProperty;
		RProperty iUpdateFixTimeProperty;
		TUint 	iDeviceStatusIndex;
	};

// Test Access points
// These test access  points are uses by the testexecute module test steps to communicate 
// with the test A-GPS integration module. For example to send UpdateDeviceStatus to the
// Manager or to instruct it how to respond to RequestLocationUpdates.  



#endif //LBSLOCSOURCEGPS_H
