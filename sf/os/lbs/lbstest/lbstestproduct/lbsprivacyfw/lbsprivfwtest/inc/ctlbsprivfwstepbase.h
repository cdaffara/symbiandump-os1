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
// @file ctlbsprivfwstepbase.h
// This contains the header file for LBS Assistance GPS Data Test Step Base
// 
//

#ifndef __CT_LBS_PRIVFW_STEP_BASE_H__
#define __CT_LBS_PRIVFW_STEP_BASE_H__

// System includes
#include <test/testexecutestepbase.h>
// User includes

#include "ctlbsprivfwserver.h"
#include "ctlbsprivfwbaserequest.h"
#include "ctlbsprivfwverifyrequest.h"

#include "ctlbsprivfwnotifierdataobserver.h"
#include "ctlbsprivfwapidataobserver.h"
#include <lbs/test/lbsprivfwdefs.h>
#include "ctlbsprivfwapiwrapper.h"

#include <lbs/lbsadmin.h>

#include "epos_cpossuplprivacyrequestinfo.h"
#include "epos_cposgsmprivacyrequestinfo.h"
#include "epos_cposnetworkprivacy.h"


//Literals Used

//_LIT(KTestCaseId, "tc_id");

_LIT(KTe_testCaseId,"tc_id");

_LIT(KTe_SPPMInUse,"UseSPPM");
//_LIT(KTe_testHandlerType,"TestHandlerType");

class CT_LbsPrivFwStepBase : public CTestStep, MApiDataObserver, MNotifierDataObserver
{

public:
	virtual ~CT_LbsPrivFwStepBase();

	virtual TVerdict doTestStepPreambleL();
	virtual TVerdict doTestStepPostambleL();
	
	// From MApiDataObserver
	virtual void ProcessApiData(TInt aRequestId, TInt aCompletionCode);
	// From MNotifierDataObserver
	virtual void ProcessNotifierData(TInt aRequestId, TLbsTestRequestInfo& aPrivNotifierData);
	virtual void ProcessRequestComplete(TInt aRequestId, TInt aCompletionCode);
	virtual void ProcessNotiferCtrlCloseDown();
	
	// Called back from requests to check if test is finished
	virtual void CheckShutdown();

	virtual TBool NeedSessionTerminate();
protected:

	void ConstructL();
	CT_LbsPrivFwStepBase(CT_LbsPrivFwServer& aParent);
	void VerifyPrivacyResponse(void);
	TVerdict doTestStepL();
	void DeterminePrivacyHandlerL();
	
	TInt FindArrayIndex(TUint aReqId);
	TInt FindArrayIndexByClientName(const TLbsTestRequestInfo& aTestInfo);
	TBool IsProcessRunning(const TDesC& aProcessName);
	TBool IsProcessStopped(const TDesC& aProcessName);

protected:	

	CPrivFwProxy* iProxy; // Owned
	CT_LbsPrivFwNotifierDataObserver* iNotifierObserver;
	CT_LbsPrivFwApiDataObserver* iApiObserver;

	CT_LbsPrivFwServer& iParent;
	TInt				iAnswer;
	TInt				iResponse;
	TPrivacyHandlerType	iPrivacyHandlerType;
	TInt				iTestCaseId;
	TLbsCTApiUsed			iApiUsed;
	
	TMessageType		iVerifyReqType;
	TMessageType		iNotifyReqType;
	TMessageType		iVerifyRespType;
	TMessageType		iNotifyRespType;
	TMessageType		iParamInfoType;
	// whether the test is running with the SPPM (rather than the privacy protocol module)
	TBool               iSPPMInUse;
	
    RPointerArray<CT_LbsPrivFwBaseRequest> iArray;
    
};

#endif // __CT_LBS_PRIVFW_STEP_BASE_H__
