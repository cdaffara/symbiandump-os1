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
// @file ctlbsprivfstepverifynotify.h
// This contains the header file for LBS Assistance GPS Data Test Step Base
// 
//

#ifndef __CT_LBS_PRIVFW_STEP_VERIFY_NOTIFY_H__
#define __CT_LBS_PRIVFW_STEP_VERIFY_NOTIFY_H__

#include "ctlbsprivfwserver.h"
#include "ctlbsprivfwstepbase.h"
#include "ctlbsprivfwbaserequest.h"
#include "ctlbsprivfwnotifyrequest.h"
#include "ctlbsprivfwnotifierdataobserver.h"
#include "ctlbsprivfwapidataobserver.h"
#include "ctlbsprivfwapiwrapper.h"

#include "epos_cposgsmprivacyrequestinfo.h"
#include "epos_cpossuplprivacyrequestinfo.h"
#include "epos_cposnetworkprivacy.h"

//Literals Used

_LIT(KLbsPrivFwStep_VerifyNotify, "KLbsPrivFwStep_VerifyNotify");

class CT_LbsPrivFwStepVerifyNotify : public CT_LbsPrivFwStepBase
{

public:
	virtual ~CT_LbsPrivFwStepVerifyNotify();
	static CT_LbsPrivFwStepVerifyNotify* New(CT_LbsPrivFwServer& aParent);

	virtual TVerdict doTestStepPreambleL();
	virtual TVerdict doTestStepPostambleL();

protected:

	void ConstructL();
	virtual TVerdict doTestStepL();
	CT_LbsPrivFwStepVerifyNotify(CT_LbsPrivFwServer& aParent);
	void SubmitNotifyLocationRequest(CPosNetworkPrivacyRequestInfo* aReqInfo, 
			TPosNetworkPrivacyRequestInfoType aPrivacyInfoType,
			CT_LbsPrivFwApiDataObserver* aPrivObserver, 
			CPrivFwApiWrapper* aNetPrivacy);
	void SubmitVerifyLocationRequest(CPosNetworkPrivacyRequestInfo* aReqInfo, 
			TPosNetworkPrivacyRequestInfoType aPrivacyInfoType,
			CT_LbsPrivFwApiDataObserver* aPrivObserver, 
			CPrivFwApiWrapper* aNetPrivacy, 
			CPosNetworkPrivacy::TRequestDecision aDecision,
			TResponseType aIntendedDecision,
			TUint aResponseDelay = 0);

};

#endif // __CT_LBS_PRIVFW_STEP_VERIFY_NOTIFY_H__
