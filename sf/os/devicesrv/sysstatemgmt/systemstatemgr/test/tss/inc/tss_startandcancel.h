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

#ifndef __TSS_STARTANDCANCEL_H__
#define __TSS_STARTANDCANCEL_H__



#include "tss_step_base.h"
#include "testandcanceler.h"

//APPFWK-SSS-0007

_LIT(KCTestCaseStartAndCancel, "StartAndCancel");

const TInt KSsTestTimeToCancel = 10000; // 10 ms

class CSsTestStepStartAndCancel : public CSsStepBase
	{
	
public:
	CSsTestStepStartAndCancel();
	~CSsTestStepStartAndCancel();
	
	TVerdict doTestStepPreambleL();
	TVerdict doTestStepPostambleL();
	
public:
	void DoTestStartProcessAndCancelL();
	void DoTestStartAppAndCancelL();
	void DoTestMultiStartAndDeleteL();

	// From MSsTestAsyncNotifier
	void SsTestNotify( TInt aNotify );
	
public:
	// From CTestStep
	TVerdict doTestStepL();
	
private:
	CActiveScheduler* iActiveScheduler;
	
	};

#endif // __TSS_STARTANDCANCEL_H__
