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

#ifndef __TSS_APPSTARTRVERROR_H__
#define __TSS_APPSTARTRVERROR_H__


#include "tss_step_base.h"
#include "testandstopper.h"


//APPFWK-SSS-0008
_LIT(KCTestCaseAppStartRvError, "AppStartRvError");


class CSsTestStepAppStartRvError : public CSsStepBase
	{
	
public:
	CSsTestStepAppStartRvError();
	~CSsTestStepAppStartRvError();
	
	void DoTestRvErrorProcAsyncL();
	void DoTestRvErrorProcSyncL();
	
	void DoTestRvErrorAppAsyncL();
	void DoTestRvErrorAppSyncL();

	TVerdict doTestStepPreambleL();
	TVerdict doTestStepPostambleL();
	// from MSsTestAsyncNotifier
	void SsTestNotify( TInt aNotify );
public:
	// From CTestStep
	TVerdict doTestStepL();
	
private:
	CActiveScheduler* iActiveScheduler;
	
	};

#endif // __TSS_APPSTARTRVERROR_H__
