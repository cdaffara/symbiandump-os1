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
// CLbsLocSourceObserver.h
// 
//

#ifndef LBSLASMONITOR_H
#define LBSLASMONITOR_H

//************************************************************************************************************
#include <Lbs.h>
#include <lbs/LbsAssistanceDataBase.h>
#include <lbs/LbsLocDataSourceGpsBase.h>


//************************************************************************************************************
//   CLbsLocationSourceObserver
//************************************************************************************************************

class CLbsLocationSourceGpsObserver : public CBase, public MLbsLocationSourceGpsObserver
	{
	public:
		static CLbsLocationSourceGpsObserver* NewL(TInt aModuleNum);
		~CLbsLocationSourceGpsObserver();

	public:
		TVersion Version();
	
		void UpdateLocation(TInt aStatus, const TPositionInfoBase* aPosition[], TInt aNumItems, const TTime& aTargetTime);
	   	
	  	void UpdateDeviceStatus(TPositionModuleStatus::TDeviceStatus aDeviceStatus);
	   	
	  	void UpdateDataQualityStatus(TPositionModuleStatus::TDataQualityStatus aDataQuality);
	  	
		TInt GetAssistanceDataItem(TLbsAssistanceDataItem aItem, RDataReaderRootBase& aDataRoot, TTime& aTimeStamp);
		
		void RequestAssistanceData(TLbsAsistanceDataGroup aDataItemMask);
		
	  	void Shutdown();

		void NotifyServiceToClose(TRequestStatus* aStatus);
	
	private:
		CLbsLocationSourceGpsObserver(TInt aModuleNum);
		void ConstructL();	

		void PublishLocation(const TPositionInfoBase& aPosition);
		void PublishModuleStatus(TPositionModuleStatus::TDeviceStatus aDeviceStatus, TPositionModuleStatus::TDataQualityStatus aDataQuality);

	private:
	   const TInt iModuleNum;
	   TRequestStatus* iServiceCloseStatus;
	   TPositionModuleStatus::TDeviceStatus      iDeviceStatus;
	   TPositionModuleStatus::TDataQualityStatus iDataQuality;
	};

#endif //LBSLASMONITOR_H
