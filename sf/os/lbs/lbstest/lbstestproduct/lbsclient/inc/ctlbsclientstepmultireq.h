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
// @file ctlbsclientstepmultireq.h
// This contains the header file for multi requests Tests
// 
//

#ifndef __CT_LBS_CLIENT_STEP_MULTIREQ_H__
#define __CT_LBS_CLIENT_STEP_MULTIREQ_H__

#include "ctlbsclientstep.h"
#include "ctlbsclientserver.h"
#include "ctlbsclientgetlkpao.h"
#include "ctlbsclientnotifyposupdao.h"


// Literals used
_LIT(KLbsClientStep_MultiReq, "LbsClientStep_MultiReq");
_LIT(KExpected_Error, "expected_error");
_LIT(KLbsMultiReqTestSuitePanic,"Lbs Multi Requests Test Suite Panic");


class CT_LbsClientStep_MultiReq : public CT_LbsClientStep, public MT_GetLastKnownPosObserver, public MT_NotifyPosUpdateObserver
	{
public:
	~CT_LbsClientStep_MultiReq();

	static CT_LbsClientStep_MultiReq* NewL(CT_LbsClientServer& aParent);
	static CT_LbsClientStep_MultiReq* New(CT_LbsClientServer& aParent);

	virtual TVerdict doTestStepL();
	
//from MT_GetLastKnownPosObserver:
	virtual void GetLastKnownPositionCallback(TRequestStatus& aStatus);
//from MT_NotifyPosUpdateObserver:
	virtual void NotifyPositionUpdateCallback(TRequestStatus& aStatus);
// tidy up (must be public to allow calling through timer callback function)	
	void CT_LbsClientStep_MultiReq::AsyncTidyUp();
	
private:

	enum TMultiReqState
	/** 
	 *	 states
	 */
	    {
	    EStateUnknown, 							/*!< state unknown. */
	    EStateLastKnownPositionComplete, 		/*!< Last Known Position request has completed */
	    EStateLastNotifyPositionUpdateComplete, /*!< Notify Position Update request has completed */
	    EStateDone 								/*!< Last Known Position and Position Update requests have both completed  */
	    };
	TMultiReqState iState;

	CT_LbsClientStep_MultiReq(CT_LbsClientServer& aParent);

	// timer for requests timeouts
	CPeriodic* iTimer;
	
	CT_LbsClientGetLastKnownPosAO*	iGetLastKnownPosAO;
	CT_LbsClientNotifyPosUpdAO*		iNotifyPosUpdateAO;
	
	TPositionInfo* iTempPosInfo;
		
	};

#endif //__CT_LBS_CLIENT_STEP_MULTIREQ_H__
