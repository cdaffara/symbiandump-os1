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
// @file ctlbsprivfwstepnoinstall.h
// This contains the header file for 
// 
//

#ifndef __CT_LBS_PRIVFW_STEP_NO_INSTALL_H__
#define __CT_LBS_PRIVFW_STEP_NO_INSTALL_H__

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

_LIT(KLbsPrivFwStep_NoInstall, "KLbsPrivFwStep_NoInstall");

class CT_LbsPrivFwStepNoInstall : public CT_LbsPrivFwStepBase
{

public:
	virtual ~CT_LbsPrivFwStepNoInstall();
	static CT_LbsPrivFwStepNoInstall* New(CT_LbsPrivFwServer& aParent);

	virtual TVerdict doTestStepPreambleL();
	virtual TVerdict doTestStepPostambleL();

protected:

	void ConstructL();
	virtual TVerdict doTestStepL();
	CT_LbsPrivFwStepNoInstall(CT_LbsPrivFwServer& aParent);
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

private:
   static TInt TimerCallBack(TAny* aPtr);

private:
    CPeriodic*			iTimer;

};

#endif // __CT_LBS_PRIVFW_STEP_NO_INSTALL_H__
