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
// @file ctlbsagpshaivalidatestep.h
// This contains the header file for Location Data Source Tests
// 
//

#ifndef __CT_LBS_DATA_SRC_STEP_AGPS_H__
#define __CT_LBS_DATA_SRC_STEP_AGPS_H__

#include "ctlbshaivalidatesteproot.h"


const TInt	KDeviceStatusChangeTimerId	= KLastRootTimerId + 1;


class MLbsTestStepObserver
    {
public:	
    virtual void DoWork() = 0;
    };

// AO setup	
class CTinyTestClass : public CActive
    {
public:
	static CTinyTestClass* NewL(MLbsTestStepObserver* aOwner);
	void StartAOService();
protected:
    CTinyTestClass(MLbsTestStepObserver* aOwner);
    void ConstructL();
    void RunL();
    void DoCancel();
private:
    MLbsTestStepObserver *iOwner;
    };


class CT_LbsAGPSHAIValidateStep_Main : public CT_LbsAGPSHAIValidateStepRoot, MLbsTestStepObserver
	{
public:
	~CT_LbsAGPSHAIValidateStep_Main();

	static CT_LbsAGPSHAIValidateStep_Main* New(CT_LbsAGPSHAIServerRoot& aParent);
	
	static CT_LbsAGPSHAIValidateStep_Main* NewL(CT_LbsAGPSHAIServerRoot& aParent);
	
	TVerdict doTestStepL();

	void HandleTimerL(TInt aTimerId, const TTime& aTargetTime);
	
	//from base class
	void UpdateLocation(TInt aStatus, const TPositionInfoBase* aPosition[], TInt aNumItems, const TTime& aTargetTime);

protected:	
	TInt GetAssistanceDataItem(TLbsAssistanceDataItem /*aItem*/, RDataReaderRootBase& /*aDataRoot*/, TTime& /*aTimeStamp*/);
	TInt GetAssistanceDataItemTimeStamp(TLbsAssistanceDataItem /*aItem*/, TTime& /*aTimeStamp*/);
	void RequestAssistanceData(TLbsAsistanceDataGroup aDataItemMask);

	// from MLbsTestStepObserver
    void DoWork();

private:
	// internal methods
	// private constructors:
	CT_LbsAGPSHAIValidateStep_Main(CT_LbsAGPSHAIServerRoot& aParent);
	void ConstructL();
	void CreateAllTimersL();
	void CancelAllTimers();
	void DeleteAllTimers();

	
	// test methods
	void DoHAIPowerOnTestCase();
	void DoHAIStandByTestCase();
	void DoHAIPowerOffTestCase();

	// heplers
	void StartDeviceStatusChangeTimer();
	void StartShutdownRequestTimer();

	
private:
	// timer to indicate that we should have received a device status change report
	CT_LbsTimerUtils*	iDeviceStatusChangeTimer;	
	TPositionModuleStatus::TDeviceStatus iExpectedDeviceStatus;
	CTinyTestClass* 	iTinyClass;
	};

#endif //__CT_LBS_DATA_SRC_STEP_AGPS_H__
