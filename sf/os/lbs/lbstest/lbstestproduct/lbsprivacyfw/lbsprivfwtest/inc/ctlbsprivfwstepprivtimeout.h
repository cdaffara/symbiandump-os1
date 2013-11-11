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
// @file ctlbsprivfwstepprivtimeout.h
// This contains the header file for an LBS privacy handler test
// 
//

#ifndef __CT_LBS_PRIVFW_STEP_PRIV_TIMEOUT_H__
#define __CT_LBS_PRIVFW_STEP_PRIV_TIMEOUT_H__


#include "ctlbsprivfwserver.h"
#include "ctlbsprivfwstepbase.h"

#include "ctlbsprivfwbaserequest.h"

#include "ctlbsprivfwnotifierdataobserver.h"
#include "ctlbsprivfwapidataobserver.h"
#include "ctlbsprivfwapiwrapper.h"

#include "epos_cposgsmprivacyrequestinfo.h"
#include "epos_cposnetworkprivacy.h"

//Literals Used

_LIT(KLbsPrivFwStep_PrivTimeout, "KLbsPrivFwStep_PrivTimeout");

class CT_LbsPrivFwStepPrivTimeout : public CT_LbsPrivFwStepBase
{

public:
	virtual ~CT_LbsPrivFwStepPrivTimeout();
	static CT_LbsPrivFwStepPrivTimeout* New(CT_LbsPrivFwServer& aParent);

	virtual TVerdict doTestStepPreambleL();
	virtual TVerdict doTestStepPostambleL();

	// override from base class to implement privacy API timeout checks
	virtual void CheckShutdown();
	
	// override from base class to ensure that closedown doesn't effect CheckShutDown() method
	virtual void ProcessNotiferCtrlCloseDown();

protected:

	void ConstructL();
	virtual TVerdict doTestStepL();
	CT_LbsPrivFwStepPrivTimeout(CT_LbsPrivFwServer& aParent);
	void SubmitVerifyLocationRequest(CPosGSMPrivacyRequestInfo* aReqInfo, 
			TPosNetworkPrivacyRequestInfoType aPrivacyInfoType,
			CT_LbsPrivFwApiDataObserver* aPrivObserver, 
			CPrivFwApiWrapper* aNetPrivacy, 
			CPosNetworkPrivacy::TRequestDecision aDecision,
			TResponseType aIntendedDecision,
			TUint aResponseDelay = 0);

private:
    static TInt TimerCallBack(TAny* aPtr);

	enum TShutdownState
	{	
	EWaitingForRequests = 0,
	EAllRequestsComplete,
	EPrivacyAPInotTimedOut,
	EPrivacyAPITimedOut
	};

private:
    CPeriodic*			iTimer;
    TShutdownState		iState;        		// Privacy API generated shutdown State
    TTimeIntervalMicroSeconds32 iTimeout;	// Privacy API inactivity timeout value
                                            // after which it will shuts LBS
};

#endif // __CT_LBS_PRIVFW_STEP_PRIV_TIMEOUT_H__
