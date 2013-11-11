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

#ifndef __TSS_APPSTART_H__
#define __TSS_APPSTART_H__

#include "tss_step_base.h"
#include "testandstopper.h"
#include "ssmtestapprvafterretry.h"
#include "testandcanceler.h"

//APPFWK-SSS-0001
_LIT(KCTestCaseAppStart, "AppStart");


class CSsTestStepAppStart : public CSsStepBase
	{
	
public:
	CSsTestStepAppStart();
	~CSsTestStepAppStart();
	
	TVerdict doTestStepPreambleL();
	TVerdict doTestStepPostambleL();
	
public:
	void DoTestFireAndForgetAsyncL();
	void DoTestWaitForStartAsyncL();
	void DoTestFireAndForgetSyncL();
	void DoTestWaitForStartSyncL();
	void DoTestWaitForStartSuccessiveSyncL();
	void DoTestAppStartWithRetriesL();
	
	// From MSsTestAsyncNotifier
	void SsTestNotify( TInt aNotify );
	
public:
	// From CTestStep
	TVerdict doTestStepL();
	
private:
	CActiveScheduler* iActiveScheduler;
	
	};

#endif // __TSS_APPSTART_H__
