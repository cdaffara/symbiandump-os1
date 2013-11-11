// Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
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
#include <Lbs.h>
#include <lbs/LbsLocDataSourceGpsBase.h>

#include "LbsLasUpdateSource.h"

class TLbsGpsOptions;
class CLasUpdateSource;


//************************************************************************************************************
//   CLbsLocationSourceGps
//************************************************************************************************************
class CLbsLocationSourceGps : public CLbsLocationSourceGpsBase, public MLasUpdateSourceObserver
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

   protected:
		CLbsLocationSourceGps(MLbsLocationSourceGpsObserver& aObserver);
		void ConstructL();

  	protected:
		CLasUpdateSource* iUpdateSource;
	};


#endif //LBSLOCSOURCEGPS_H
