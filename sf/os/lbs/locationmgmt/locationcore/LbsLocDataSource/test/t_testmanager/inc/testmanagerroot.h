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
// sirfmanagerroot.h
// 
//

#ifndef LASROOT_H
#define LASROOT_H

#include <lbs/lbslocdatasourcegpsbase.h>
#include <e32test.h>

//class CLocationManagerGps;

//************************************************************************************************************
//   CLocationManagerRoot
//************************************************************************************************************
class CLocationManagerRoot : public CTimer, public MLbsLocationSourceGpsObserver
	{
public:
	static CLocationManagerRoot* NewL(RTest& aTest, TInt aCount);
	~CLocationManagerRoot();

	void RunL();
	void DoCancel();
	
	void CompleteSelf(TInt aReason = KErrNone);

private:
	CLocationManagerRoot(RTest& aTest, TInt aCount);
	void ConstructL();

private:
	//from MLbsLocationSourceGpsObserver
	virtual void UpdateLocation(TInt aStatus, const TPositionInfoBase* aPosInfoArray[], TInt aNumItems, const TTime& aTargetTime);
  	virtual void UpdateDeviceStatus(TPositionModuleStatus::TDeviceStatus aDeviceStatus);
  	virtual void UpdateDataQualityStatus(TPositionModuleStatus::TDataQualityStatus aDataQuality);
	virtual TInt GetAssistanceDataItem(TLbsAssistanceDataItem aItem, RDataReaderRootBase& aDataRoot, TTime& aTimeStamp);
	virtual TInt GetAssistanceDataItemTimeStamp(TLbsAssistanceDataItem aItem, TTime& aTimeStamp);
	virtual void RequestAssistanceData(TLbsAsistanceDataGroup aDataItemMask);
  	virtual void Shutdown();

private:
	RTest& 	iTest;
	TBool 	iShutdown;
	TBool 	iCount;
	TBool 	iLoop;

	TTime 					iTargetTime;
	TLbsLocRequestQuality 	iQuality;

	CLbsLocationSourceGpsBase* iModule;
	};


#endif //LASROOT_H