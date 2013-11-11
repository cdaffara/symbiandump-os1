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

#ifndef __TSS_APPSTARTMULTIPLE_H__
#define __TSS_APPSTARTMULTIPLE_H__

#include "tss_step_base.h"
#include "testandstopper.h"
#include "testandcanceler.h"
#include"ssmtestapprvafterretry.h"


//APPFWK-SSS-0004
_LIT( KCTestCaseAppStartMultiple, "AppStartMultiple" );


class CSsTestStepAppStartMultiple : public CSsStepBase
	{
	
public:
	CSsTestStepAppStartMultiple();
	~CSsTestStepAppStartMultiple();
	
	TVerdict doTestStepPreambleL();
	TVerdict doTestStepPostambleL();
	
public:
	void DoTestMultiAppAsyncL();
	void DoTestMultiAppWithTimeoutAsyncL();
	void DoTestMultiAppAllTimeoutAsyncL();
	void DoTestMultiAppIncludingRetryAsyncL();

	void DoTestMultiProcWithTimeoutAsyncL();
	void DoTestMultiProcAllTimeoutAsyncL();
	void DoTestMultiProcIncludingRetryAsyncL();
	void DoTestMultiProcAsyncL();
	
	void DoLashup();
	
	  // From MSsTestAsyncNotifier
	void SsTestNotify( TInt aNotify );
	
public:
	// From CTestStep
	TVerdict doTestStepL();
	
private:
	CActiveScheduler* iActiveScheduler;
	
	};

#endif // __TSS_APPSTARTMULTIPLE_H__
