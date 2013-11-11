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
// @file CT_LbsClientTracking.h
// This contains the header file for Tracking Tests
// 
//

#ifndef __CT_LBS_CLIENT_TRACKING_H__
#define __CT_LBS_CLIENT_TRACKING_H__

#include "ctlbsclientstep.h"
#include "ctlbsclientserver.h"

#include <lbs/test/ctlbsagpshandler.h>

// Literals used
_LIT(KLbsClientStep_Tracking, "LbsClientStep_Tracking");

_LIT(KExpectedErr, "expected_error");
_LIT(KExpectedErr2, "expected_error2");
_LIT(KExpectedErr3, "expected_error3");
_LIT(KExpectedErr4, "expected_error4");

class CT_LbsClientStep_Tracking : public CT_LbsClientStep, MT_ResponseObserver
	{
public:
	~CT_LbsClientStep_Tracking();

	static CT_LbsClientStep_Tracking* New(CT_LbsClientServer& aParent);
	virtual TVerdict doTestStepL();

	// from MT_ResponseObserver
	virtual void ProcessAGpsResponseMessage(const TT_LbsAGpsResponseMsg::TModuleResponseType aResponse);
		
private:
	CT_LbsClientStep_Tracking(CT_LbsClientServer& aParent);
	void ConstructL();
	
	TBool VerifyFirstPosTime(const TTime& aTimeRequested, const TPositionInfo& aPosInfo, const TPositionUpdateOptions& aUpdateOpts);
	TBool VerifyPosTime(const TTime& aTimeFirstPosReceived, const TPositionUpdateOptions& aUpdateOpts, TPositionInfo& aPosInfo, TInt aPosCount);
	TInt GetExpectedError(const TDesC& aError);
	TInt SetExpectedError(TInt aExpectedErrVal,TInt aDefaultErr);
	void DoTestCase();
	
	void DoInterval_TestCaseId_01();
	void DoInterval_TestCaseId_02();
	void DoInterval_TestCaseId_03();
	void DoInterval_TestCaseId_04();
	void DoInterval_TestCaseId_05();
	void DoInterval_TestCaseId_06();
	void DoInterval_TestCaseId_07();
	void DoInterval_TestCaseId_08();
	
	void ConfigureModuleTimeOut(TTimeIntervalMicroSeconds& aTimeOut);
private:
	TInt iVerifyInterval;
	TInt iExpectedErr;
	TInt iExpectedErr2;
	TInt iExpectedErr3;
	TInt iExpectedErr4;
	TInt iUpdateIndex;
	TInt iUpdateWindowIndex;
	TPositionUpdateOptions iUpdateOpts;
	TPositionInfo iPosInfo;

 	TBool iCheckIfSameAsPreviousPosition;
 	TPositionInfo iPreviousPos;
 	TBool iSameAsPreviousPos;
	};

#endif //__CT_LBS_CLIENT_TRACKING_H__
