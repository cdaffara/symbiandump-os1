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
// @file ctlbsprivfwstepnotify.h
// This contains the header file for LBS Assistance GPS Data Test Step Base
// 
//

#ifndef __CT_LBS_PRIVFW_STEP_NOTIFY_H__
#define __CT_LBS_PRIVFW_STEP_NOTIFY_H__

#include "ctlbsprivfwserver.h"
#include "ctlbsprivfwstepbase.h"

#include "ctlbsprivfwbaserequest.h"
#include "ctlbsprivfwnotifyrequest.h"

#include "ctlbsprivfwnotifierdataobserver.h"
#include "ctlbsprivfwapidataobserver.h"
#include "ctlbsprivfwapiwrapper.h"

#include "epos_cposgsmprivacyrequestinfo.h"
#include "epos_cposnetworkprivacy.h"

//Literals Used

_LIT(KLbsPrivFwStep_Notify, "KLbsPrivFwStep_Notify");

class CT_LbsPrivFwStepNotify : public CT_LbsPrivFwStepBase
{

public:
	virtual ~CT_LbsPrivFwStepNotify();
	static CT_LbsPrivFwStepNotify* New(CT_LbsPrivFwServer& aParent);

	virtual TVerdict doTestStepPreambleL();
	virtual TVerdict doTestStepPostambleL();

protected:

	void ConstructL();
	virtual TVerdict doTestStepL();
	CT_LbsPrivFwStepNotify(CT_LbsPrivFwServer& aParent);
	void SubmitNotifyLocationRequest(CPosGSMPrivacyRequestInfo* aReqInfo, 
			TPosNetworkPrivacyRequestInfoType aPrivacyInfoType,
			CT_LbsPrivFwApiDataObserver* aPrivObserver, 
			CPrivFwApiWrapper* aNetPrivacy);


};

#endif // __CT_LBS_PRIVFW_STEP_QNN_NOTIFY_H__
