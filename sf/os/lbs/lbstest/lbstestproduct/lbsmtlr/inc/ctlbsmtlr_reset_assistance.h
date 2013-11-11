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
// @file ctlbsmtlr_reset_assistance.h
// 
//

#ifndef __CT_LBS_MTLR_RESET_ASSISTANCE_H__
#define __CT_LBS_MTLR_RESET_ASSISTANCE_H__

// LBS includes.
#include <lbs.h>
#include <lbscommon.h>
#include <lbs/lbsnetcommon.h>
#include <lbs/lbsprivacycontroller.h>
#include <lbs/lbsadmin.h>
#include <lbs/lbsnetprotocolbase.h>
#include <lbs/lbsnetcommon.h>
#include <lbs/test/lbsnetsimtest.h>
#include <lbs/test/ctlbsasyncwaiter.h>

// LBS test includes.
#include "ctlbsmtlrstep.h"
#include "ctlbsmtlrserver.h"

// Literals used
_LIT(KLbsMTLRResetAssistace, "LbsMTLRResetAssistance");

// Callbacks flags.

class CT_LbsMTLRResetAssistance : public CT_LbsMTLRStep, MLbsPrivacyObserver
	{
public:
	~CT_LbsMTLRResetAssistance();

	static CT_LbsMTLRResetAssistance* New(CT_LbsMTLRServer& aParent);
	virtual TVerdict doTestStepL();

	// From MLbsPrivacyObserver:	
	void ProcessNetworkLocationRequest(TUint aRequestId, const TLbsExternalRequestInfo& aRequestInfo, const TNotificationType& aNotificationType);
	void ProcessNetworkPositionUpdate(TUint aRequestId, const TPositionInfo& aPosInfo);
	void ProcessRequestComplete(TUint aRequestId, TInt aReason);
	
	// From MLbsNetSimTestObserver.	
	void NotifyReleaseLcsLocationNotification(const CLbsNetworkProtocolBase::TLbsPrivacyResponse& aResult);
	void NotifyError(MLbsNetSimTestObserver::EFunction aFunction, int aError);
	void NotifyMeasurementReportLocation(const TPositionInfo& aPosition);
	void NotifyMeasurementReportControlFailure(TInt aReason);
	
private:
	CT_LbsMTLRResetAssistance(CT_LbsMTLRServer& aParent);
	void ConstructL();
	TInt OpenNetSim();
	void CloseNetSim();
	TInt64 DoRequests(TBool aCold, TBool aReset);
	TInt GetLocationL(TLbsNetPosRequestPrivacy aReqType, TLbsExternalRequestInfo aReqInfo);

private:
	CLbsPrivacyController* iController;
	CT_LbsAsyncWaiter*	iWaiter;
	};

#endif //__CT_LBS_MTLR_RESET_ASSISTANCE_H__
