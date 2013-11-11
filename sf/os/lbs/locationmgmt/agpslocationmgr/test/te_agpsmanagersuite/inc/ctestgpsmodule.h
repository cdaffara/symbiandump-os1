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
// A fake GPS Integration Module for AGPS Manager UNIT testing
// 
//

#ifndef __CTESTGPMODULE_H__
#define __CTESTGPMODULE_H__

#include "ctestgpsmoduleobserver.h"

class CTestGpsModule : public CLbsLocationSourceGpsBase
	{

public:

	// Called by the AGPS Manager
	static CLbsLocationSourceGpsBase* NewL(MLbsLocationSourceGpsObserver& aObserver);
	
	// Method for the test to set itself as
	// an observer (the AGPS Manager is an observer
	// of different events)
	void SetTestObserver(MTestGpsModuleObserver* aTestObserver);

	// From CLbsLocationSourceGpsBase...Called by the AGPS Manager and observed by the test
  	void RequestLocationUpdate(const TTime& aTargetTime, const TLbsLocRequestQuality& aQuality);  	
  	void CancelLocationRequest();
	void AdvisePowerMode(TPowerMode aMode);
	void SetGpsOptions(const TLbsGpsOptions& aGpsOptions);
	void AssistanceDataEvent(TInt aError, TLbsAsistanceDataGroup aDataMask);

	// The following methods map to the methods in MLbsLocationSourceGpsObserver. They
	// are called by the test and observed by the AGPS Manager.
	void InjectLocationUpdate(TInt aStatus, const TPositionInfoBase* aPosInfoArray[], TInt aNumItems, const TTime& aTargetTime);
  	void InjectDeviceStatus(TPositionModuleStatus::TDeviceStatus aDeviceStatus);
  	void InjectDataQualityStatus(TPositionModuleStatus::TDataQualityStatus aDataQuality);
	TInt InjectAssistanceDataItemRequest(TLbsAssistanceDataItem aItem, RDataReaderRootBase& aDataRoot, TTime& aTimeStamp);
	TInt InjectAssistanceDataItemTimeStampRequest(TLbsAssistanceDataItem aItem, TTime& aTimeStamp);
	void InjectAssistanceDataRequest(TLbsAsistanceDataGroup aDataItemMask);	
  	void InjectShutdownNotification();

private:

	CTestGpsModule(MLbsLocationSourceGpsObserver& aObserver);

	MTestGpsModuleObserver* iTestObserver; //Not owned but it can't be a reference since it is set after construction
	};

/** Global ptr to allow the test to use the
instance of CTestGpsModule created by
the AGPS Manager
the .
*/
namespace UnitTestGpsModuleGlobals
	{
	extern CTestGpsModule* G_TestGpsModulePtr;
	}
	
#endif // __CTESTGPMODULE_H__
