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
// This contains the header file for an LBS privacy controller close down test
// 
//

#ifndef __CT_LBS_PRIVFW_STEP_CTLRCLOSEDOWN_H__
#define __CT_LBS_PRIVFW_STEP_CTLRCLOSEDOWN_H__


#include "ctlbsprivfwserver.h"
#include "ctlbsprivfwstepbase.h"

#include "ctlbsprivfwbaserequest.h"

#include "ctlbsprivfwnotifierdataobserver.h"
#include "ctlbsprivfwapidataobserver.h"
#include "ctlbsprivfwapiwrapper.h"

#include "epos_cposgsmprivacyrequestinfo.h"
#include "epos_cposnetworkprivacy.h"

//Literals Used

_LIT(KLbsPrivFwStep_CtlrCloseDown, "LbsPrivFwStep_CtlrCloseDown");

class CT_LbsPrivFwStepCtlrCloseDown : public CT_LbsPrivFwStepBase
{

public:
	virtual ~CT_LbsPrivFwStepCtlrCloseDown();
	static CT_LbsPrivFwStepCtlrCloseDown* New(CT_LbsPrivFwServer& aParent);

	virtual TVerdict doTestStepPreambleL();
	virtual TVerdict doTestStepPostambleL();

	// override from base class
	virtual void ProcessRequestComplete(TInt aRequestId, TInt aCompletionCode);

protected:

	void ConstructL();
	virtual TVerdict doTestStepL();
    CT_LbsPrivFwStepCtlrCloseDown(CT_LbsPrivFwServer& aParent);
	void SubmitNotifyLocationRequest(CPosGSMPrivacyRequestInfo* aReqInfo, 
			TPosNetworkPrivacyRequestInfoType aPrivacyInfoType,
			CT_LbsPrivFwApiDataObserver* aPrivObserver, 
			CPrivFwApiWrapper* aNetPrivacy);

private:
	CPrivFwApiWrapper* iNetPrivacy;

};

#endif // __CT_LBS_PRIVFW_STEP_CTLRCLOSEDOWN_H__
