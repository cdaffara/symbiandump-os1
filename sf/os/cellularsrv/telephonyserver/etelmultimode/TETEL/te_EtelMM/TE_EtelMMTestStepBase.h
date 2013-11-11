// Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
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
//

#ifndef __TE_ETELMM_TESTSTEP_BASE__
#define __TE_ETELMM_TESTSTEP_BASE__

#include <test/testexecutestepbase.h>

#include <etelmm.h>
#include <etelmmcs.h>

const TInt KOOMTestNumberStart=239;	// The test count number at which OOM tests start and KErrNoMemory responses will be tolerated.
_LIT(KTestCheckConditionFailText, "Failed: Condition was false.");


#define TESTCHECKCONDITION(a) \
    { \
    if (!(a)) \
	    { \
	    INFO_PRINTF1(KTestCheckConditionFailText); \
        SetTestStepResult(EFail); \
        } \
    }

class CEtelMMTestStepBase : public CTestStep
	{
public:
 	virtual TVerdict doTestStepPreambleL();
	virtual TVerdict doTestStepPostambleL();
	void ValidateDataCallParams(const RMobileCall::TMobileDataCallParamsV1 &dataParams, TInt aCount);
	RCommServ iCommServer;
	RTelServer iTelServer;

private:
	CActiveScheduler* iTestScheduler;
	};


class CEtelMMLbsTestStepBase : public CEtelMMTestStepBase
{

public:
 	virtual TVerdict doTestStepPreambleL();
	virtual TVerdict doTestStepPostambleL();
	void ValidateMCParams();
	void ValidateMCRefTimeParams();
	void ValidateMCRefLocParams();
	void ValidateMCModelParams();
	void SendCompleteMeasurementReport();
	void SetMolrParams();
	void OOMTest_LBS();
	void OOMTest_DialEmergencyCall();
	RMobilePhone iPhone;
    RMobileLocationServices iLocationSvc;
	RMobileLocationServices::TMeasurementControlV7 iMeasurementControl;
	RMobileLocationServices::TMeasurementReportV7 iMeasurementReport;
	RMobileLocationServices::TStartMolrV7    iStartMolr;
    
};

#endif // __TE_ETELMM_TESTSTEP_BASE__
