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
 
#ifndef __TSS_PROCSTARTWITHFANDF_H__
#define __TSS_PROCSTARTWITHFANDF_H__

#include "tss_step_base.h"
#include "ssmtestapps.h"
#include "testandstopper.h"
#include "ssmtestprocgoodsession.h"

_LIT(KCTestCaseProcStartWithFAndF, "ProcStartWithFAndF");

class CSsTestStepProcStartWithFAndF : public CSsStepBase 
	{
public:
	CSsTestStepProcStartWithFAndF();
	~CSsTestStepProcStartWithFAndF();
	
	TVerdict doTestStepPreambleL();
	TVerdict doTestStepPostambleL();
	
public:
	// From CTestStep
	TVerdict doTestStepL();
	void DoTestFAndFWithLessTimeOutAndNoRetriesL();
	void DoTestFAndFWithLessTimeOutAndWithRetriesL();
	void DoTestFAndFWithMoreTimeOutAndNoRetriesL();
	void DoTestFAndFWithMoreTimeOutAndWithRetriesL();
	void DoTestFAndFWithRetriesForFailureAppL();
	void DoTestFAndFWithRetriesForSuccessfullAppL();
	void DoTestFAndFNoRetriesAndNoTimeOutL();
	void DoTestFAndFWithLessTimeOutAndMoreRetriesL();

private:
	void DoTestAndStopperL(TInt aRetries, TInt aTimeOut, const TDesC& aFileName);
	
private:
	CActiveScheduler* iActiveScheduler;
	};

#endif //__TSS_PROCSTARTWITHFANDF_H__
