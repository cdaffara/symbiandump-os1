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
 
#ifndef __TSYSMON_STEPPROCMONINIT_H__
#define __TSYSMON_STEPPROCMONINIT_H__

#include "tsysmon_stepbase.h"

//APPFWK-SYSMON-0001
_LIT(KCTestCaseProcMonInit, "ProcMonInit");


class CStepProcMonInit : public CTestStepBase
	{
public:
	CStepProcMonInit();
	
	// from CTestStep
	TVerdict doTestStepL();
	
private:
	void DoTestProcL();
	void DoTestAppL();
	void DoTestMonitorL(const RProcess& aProcess, const TDesC& aFilename);
	void DoTestErrArgumentL();
	};

#endif
