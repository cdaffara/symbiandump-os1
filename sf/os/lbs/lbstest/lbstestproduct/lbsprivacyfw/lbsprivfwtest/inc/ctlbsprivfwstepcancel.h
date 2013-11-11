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
// @file ctlbsprivfwstepcancel.h
// This contains the header file for an LBS privacy handler test
// 
//

#ifndef __CT_LBS_PRIVFW_STEP_CANCEL_H__
#define __CT_LBS_PRIVFW_STEP_CANCEL_H__

#include "ctlbsprivfwserver.h"
#include "ctlbsprivfwstepbase.h"

#include "ctlbsprivfwcancelrequest.h"

#include "ctlbsprivfwnotifierdataobserver.h"
#include "ctlbsprivfwapidataobserver.h"
#include "ctlbsprivfwapiwrapper.h"

#include "epos_cposgsmprivacyrequestinfo.h"
#include "epos_cposnetworkprivacy.h"

//Literals Used

_LIT(KLbsPrivFwStep_Cancel, "KLbsPrivFwStep_Cancel");

class CT_LbsPrivFwStepCancel : public CT_LbsPrivFwStepBase
{

public:
	virtual ~CT_LbsPrivFwStepCancel();
	static CT_LbsPrivFwStepCancel* New(CT_LbsPrivFwServer& aParent);

	//virtual TVerdict doTestStepPreambleL();
	//virtual TVerdict doTestStepPostambleL();

protected:

	void ConstructL();
	virtual TVerdict doTestStepL();
	CT_LbsPrivFwStepCancel(CT_LbsPrivFwServer& aParent);
	
	void SubmitVerifyLocationRequest(CPosGSMPrivacyRequestInfo* aReqInfo, 
		TPosNetworkPrivacyRequestInfoType aPrivacyInfoType,
		CT_LbsPrivFwApiDataObserver* aPrivObserver, 
		CPrivFwApiWrapper* aNetPrivacy, 
		CPosNetworkPrivacy::TRequestDecision aDecision,
		TResponseType aIntendedDecision, 
		TTimeIntervalMicroSeconds aResponseDelay,
		CT_LbsPrivFwCancelRequest::TReqCancelBehaviour aCancelBehaviour,
    	CPosNetworkPrivacy::TCancelReason iCancelReason = CPosNetworkPrivacy::ECancelReasonNotAvailable );

	void SubmitNotifyLocationRequest(CPosGSMPrivacyRequestInfo* aReqInfo, 
		TPosNetworkPrivacyRequestInfoType aPrivacyInfoType,
		CT_LbsPrivFwApiDataObserver* aPrivObserver, 
		CPrivFwApiWrapper* aNetPrivacy,
		TTimeIntervalMicroSeconds aResponseDelay,
		CT_LbsPrivFwCancelRequest::TReqCancelBehaviour aCancelBehaviour,
    	CPosNetworkPrivacy::TCancelReason iCancelReason = CPosNetworkPrivacy::ECancelReasonNotAvailable );
    	

};

#endif // __CT_LBS_PRIVFW_STEP_CANCEL_H__
