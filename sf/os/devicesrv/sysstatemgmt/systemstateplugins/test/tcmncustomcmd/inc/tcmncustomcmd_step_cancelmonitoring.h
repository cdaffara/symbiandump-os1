// Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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
// Description: Test for cancelallmonitors custom command
//

/**
 @file
 @test
 @internalComponent - Internal Symbian test code
*/


#ifndef __TCMNCUSTOMCMD_STEP_CANCELALLMON_H__
#define __TCMNCUSTOMCMD_STEP_CANCELALLMON_H__

#include <test/testexecutestepbase.h>

_LIT(KTCCustomCmdTestCancelMonitoring, "CCustomCmdTestCancelMonitoring");

class CCustomCmdTestCancelMonitoring: public CTestStep
	{
public:
    CCustomCmdTestCancelMonitoring();
	~CCustomCmdTestCancelMonitoring();

	//from CTestStep
	TVerdict doTestStepL();

private:
	void TestCustomCmdCancelMonitoringL();
	void CreateProcessAndMonitorL(RProcess& aProcess, const TDesC& aProcName, const TDesC& aArgs);
	void DoTestCancelMonitoringL(RProcess& aProcess1, RProcess& aProcess2);
	TBool Exists(const TDesC& aProcessName);

private:
    RSemaphore iProcStartSignalSem;
	};

#endif	// __TCMNCUSTOMCMD_STEP_CANCELALLMON_H__
