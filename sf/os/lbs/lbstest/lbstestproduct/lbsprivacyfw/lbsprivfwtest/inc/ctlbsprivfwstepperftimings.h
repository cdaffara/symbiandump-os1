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
// @file ctlbsprivfwstepperftimings.h
// This contains the header file for an LBS privacy handler test
// 
//

#ifndef __CT_LBS_PRIVFW_STEP_PERF_TIMINGS_H__
#define __CT_LBS_PRIVFW_STEP_PERF_TIMINGS_H__

#include "ctlbsprivfwserver.h"
#include "ctlbsprivfwstepbase.h"

#include "ctlbsprivfwbaserequest.h"
#include "ctlbsprivfwperftimingsrequest.h"

#include "ctlbsprivfwreqsubmitter.h"

#include "ctlbsprivfwnotifierdataobserver.h"
#include "ctlbsprivfwapidataobserver.h"
#include "ctlbsprivfwapiwrapper.h"

#include "epos_cposgsmprivacyrequestinfo.h"
#include "epos_cposnetworkprivacy.h"

//Literals Used

_LIT(KLbsPrivFwStep_PerfTimings, "KLbsPrivFwStep_PerfTimings");

class CT_LbsPrivFwStepPerfTimings : public CT_LbsPrivFwStepBase
{

public:
	virtual ~CT_LbsPrivFwStepPerfTimings();
	static CT_LbsPrivFwStepPerfTimings* New(CT_LbsPrivFwServer& aParent);

	virtual TVerdict doTestStepPreambleL();
	virtual TVerdict doTestStepPostambleL();

protected:

	void ConstructL();
	virtual TVerdict doTestStepL();
	CT_LbsPrivFwStepPerfTimings(CT_LbsPrivFwServer& aParent);

private:

	TTimeIntervalMicroSeconds iFailTimeLimit;
	TInt iRequestCount; // Number of privacy requests to process;
	CT_LbsPrivFwReqSubmitter * iRequestSubmitter;
};

#endif // __CT_LBS_PRIVFW_STEP_PERF_TIMINGS_H__
