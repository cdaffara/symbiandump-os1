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

#include "ctestgpsmodule.h"

/** Global ptr to allow the test to use the
instance of CTestGpsModule created by
the AGPS Manager
the .
*/
namespace UnitTestGpsModuleGlobals
	{
	CTestGpsModule* G_TestGpsModulePtr;
	}
	
 
CLbsLocationSourceGpsBase* CTestGpsModule::NewL(MLbsLocationSourceGpsObserver& aObserver)
	{
	CTestGpsModule* self = new(ELeave) CTestGpsModule(aObserver);
	CleanupStack::PushL(self);
	self->ConstructL();
	UnitTestGpsModuleGlobals::G_TestGpsModulePtr = self;
	CleanupStack::Pop(self);
	return self;
	}

CTestGpsModule::CTestGpsModule(MLbsLocationSourceGpsObserver& aObserver) :
	CLbsLocationSourceGpsBase(aObserver)
	{
	}
	
// Methods called by the AGPS Manager on the module
void CTestGpsModule::RequestLocationUpdate(const TTime& aTargetTime, const TLbsLocRequestQuality& aQuality)
	{
	iTestObserver->RequestLocationUpdate(aTargetTime, aQuality);
	}	
void CTestGpsModule::CancelLocationRequest()
	{
	iTestObserver->CancelLocationRequest();
	}
void CTestGpsModule::AdvisePowerMode(TPowerMode aMode)
	{
	iTestObserver->AdvisePowerMode(aMode);
	}
void CTestGpsModule::SetGpsOptions(const TLbsGpsOptions& aGpsOptions)
	{
	iTestObserver->SetGpsOptions(aGpsOptions);
	}
void CTestGpsModule::AssistanceDataEvent(TInt aError, TLbsAsistanceDataGroup aDataMask)
	{
	iTestObserver->AssistanceDataEvent(aError, aDataMask);
	}

// Methods called by the test (and forwarded to the AGPS Manager)
void CTestGpsModule::InjectLocationUpdate(TInt aStatus, const TPositionInfoBase* aPosInfoArray[], TInt aNumItems, const TTime& aTargetTime)
	{
	iLocSourceGpsObserver.UpdateLocation(aStatus, aPosInfoArray, aNumItems, aTargetTime);
	}
void CTestGpsModule::InjectDeviceStatus(TPositionModuleStatus::TDeviceStatus aDeviceStatus)
	{
	iLocSourceGpsObserver.UpdateDeviceStatus(aDeviceStatus);
	}	
void CTestGpsModule::InjectDataQualityStatus(TPositionModuleStatus::TDataQualityStatus aDataQuality)
	{
	iLocSourceGpsObserver.UpdateDataQualityStatus(aDataQuality);
	}
TInt CTestGpsModule::InjectAssistanceDataItemRequest(TLbsAssistanceDataItem aItem, RDataReaderRootBase& aDataRoot, TTime& aTimeStamp)
	{
	return iLocSourceGpsObserver.GetAssistanceDataItem(aItem, aDataRoot, aTimeStamp);
	}
TInt CTestGpsModule::InjectAssistanceDataItemTimeStampRequest(TLbsAssistanceDataItem aItem, TTime& aTimeStamp)
	{
	return iLocSourceGpsObserver.GetAssistanceDataItemTimeStamp(aItem, aTimeStamp);
	}
void CTestGpsModule::InjectShutdownNotification()
	{
	iLocSourceGpsObserver.Shutdown();
	}
void CTestGpsModule::InjectAssistanceDataRequest(TLbsAsistanceDataGroup aDataItemMask)
	{
	iLocSourceGpsObserver.RequestAssistanceData(aDataItemMask);
	}	

void CTestGpsModule::SetTestObserver(MTestGpsModuleObserver* aTestObserver)
	{
	iTestObserver = aTestObserver;
	}
