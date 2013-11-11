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
//

/**
 @file
 @test
 @internalComponent - Internal Symbian test code
*/

#ifndef __TSS_APPTIMEOUT_H__
#define __TSS_APPTIMEOUT_H__


#include "tss_step_base.h"
#include "testandstopper.h"
#include "ssmtestprocgoodsession.h"

//APPFWK-SSS-0005

_LIT(KCTestCaseAppTimeout, "AppTimeOut");


class CSsTestStepAppTimeout : public CSsStepBase
	{
	
public:
	CSsTestStepAppTimeout();
	~CSsTestStepAppTimeout();
	
	TVerdict doTestStepPreambleL();
	TVerdict doTestStepPostambleL();
	
public:
	// From CTestStep
	TVerdict doTestStepL();
	
	void DoTestAppTimeoutAsAppL();
	void DoTestAppTimeoutAsProcL();

private:
	void DoTestWaitForSignalWithLessTimeOutAndNoRetriesL();
	void DoTestWaitForSignalWithLessTimeOutAndWithRetriesL();
	void DoTestWaitForSignalWithMoreTimeOutAndNoRetriesL();
	void DoTestWaitForSignalWithMoreTimeOutAndWithRetriesL();
	void DoTestWaitForSignalWithRetriesForFailureAppL();
	void DoTestWaitForSignalWithRetriesForSuccessfullAppL();
	void DoTestWaitForSignalNoRetriesAndNoTimeOutL();
	void DoTestWaitForSignalWithLessTimeOutAndMoreRetriesL();
	TInt DoTestAndStopperL(TInt aRetries, TInt aTimeOut, const TDesC& aFileName);
	
private:
	CActiveScheduler* iActiveScheduler;
	};

#endif // __TSS_APPTIMEOUT_H__
