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

#ifndef __TSS_ERRORS_H__
#define __TSS_ERRORS_H__


#include "tss_step_base.h"

//APPFWK-SSS-009
_LIT( KStartSafeErrors, "StartSafeErrors" );

enum TSsErrorScenario
	{
	ESsErrorScenarioNoMonitorFlag,
	ESsErrorScenarioNullFileNameProcess,
	ESsErrorScenarioNullFileNameApp,
	ESsErrorScenarioFictitiousFileNameProcess,
	ESsErrorScenarioFictitiousFileNameApp,
	ESsErrorScenarioCommandTypeOutOfRange,
	ESsErrorScenarioStartMethodOutOfRange,
	ESsErrorScenarioNegativeTimeout,
	ESsErrorScenarioNegativeRetries,
	ESsErrorScenarioNegativeRetriesFails,
	ESsErrorScenarioFireAndForgetWithNullFilename,
	ESsErrorScenarioStartAndCancelWithDuffIndex,
	ESsErrorScenarioStartAppWhichPanicsKErrNoneAsApp,
	ESsErrorScenarioStartAppWhichPanicsKErrNoneAsProcess,
	ESsErrorScenarioStartAppWhichPanicsAsApp,
	ESsErrorScenarioStartAppWhichPanicsAsProcess,
	ESsErrorScenarioOOM
	};

	
	
class CSsTestStepErrors : public CSsStepBase
	{
	
public:
	CSsTestStepErrors();
	~CSsTestStepErrors();
	
	void DoTestNoMonitorFlagL();
	void DoTestStartAsProcessWithNullFileNameL();
	void DoTestStartAsAppWithNullFileNameL();
	void DoTestStartAsProcessWithFictitiousFileNameL();
	void DoTestStartAsAppWithFictitiousFileNameL();	
	void DoTestCommandTypeOutOfRangeL();
	void DoTestStartMethodOutOfRangeL();
	void DoTestNegativeTimeoutL();
	void DoTestNegativeRetriesL();
	void DoTestNegativeRetriesStartupFailsL();
	void DoTestFireAndForgetWithNullFilenameL();
	void DoTestStartandCancelWithBadIndexL();
	void DoTestStartAppWhichPanicsKErrNoneAsAppL();
	void DoTestStartAppWhichPanicsKErrNoneAsProcessL();
	void DoTestStartAppWhichPanicsAsProcessL();
	void DoTestStartAppWhichPanicsAsAppL();
	void DoTestOOML();
	
public:
	// From CTestStep
	TVerdict doTestStepL();
	TVerdict doTestStepPreambleL();
	TVerdict doTestStepPostambleL();

private:
	void SsTestNotify( TInt aNotify );
private:
	TVerdict iInterimVerdict; // Note: EPass == 0
	CActiveScheduler* iActiveScheduler;
	TSsErrorScenario iScenario;
	};


#endif // __TSS_ERRORS_H__
