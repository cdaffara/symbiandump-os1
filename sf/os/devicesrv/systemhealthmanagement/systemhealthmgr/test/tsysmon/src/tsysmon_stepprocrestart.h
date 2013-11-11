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
 
#ifndef __TSYSMON_STEPPROCRESTART_H__
#define __TSYSMON_STEPPROCRESTART_H__

#include "tsysmon_stepbase.h"

//APPFWK-SYSMON-0003
_LIT(KCTestCaseProcRestart, "ProcRestart");

class CStepProcRestart : public CTestStepBase
	{
public:
	CStepProcRestart();
	
	//from CTestStep
	TVerdict doTestStepL();
	
private:
	void DoTestProcRestartL();
	void DoTestExtendedProcRestartL();
	void DoTestProcRestartAferRateOfFailureL();
	void DoTestProcRestartForRateOfFailureL();
	void DoTestProcRestartAferRateOfFailureL(TStartMethod aStartMethod);
	void DoTestProcRestartForRateOfFailureL(TStartMethod aStartMethod);
	TBool AssertRestart();
	void Cleanup();
	
private:
	RSemaphore iProcStartSignalSem;
	};

#endif
