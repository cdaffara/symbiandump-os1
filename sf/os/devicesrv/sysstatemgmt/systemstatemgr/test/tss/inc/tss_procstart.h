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
 
#ifndef __TSS_PROCSTART_H__
#define __TSS_PROCSTART_H__


#include "tss_step_base.h"
#include "ssmtestapps.h"
#include "testandstopper.h"
#include "ssmtestprocgoodsession.h"

//APPFWK-SSS-0002

_LIT(KCTestCaseProcStart, "ProcStart");


class CSsTestStepProcStart : public CSsStepBase 
	{
	
public:
	CSsTestStepProcStart();
	~CSsTestStepProcStart();
	
	TVerdict doTestStepPreambleL();
	TVerdict doTestStepPostambleL();
	
public:
	// From CTestStep
	TVerdict doTestStepL();
	void DoTestFireAndForgetAsyncL();
	void DoTestWaitForStartAsyncL();
	void DoTestFireAndForgetSyncL();
	void DoTestWaitForStartSyncL();
	void DoTestStartTwoServersL();
	void DoTestForMonitorAlreadyMonProcL();
	
	// From MSsTestAsyncNotifier
	void SsTestNotify( TInt aNotify );
	
private:
	CActiveScheduler* iActiveScheduler;
	};

#endif // __TSS_PROCSTART_H__
