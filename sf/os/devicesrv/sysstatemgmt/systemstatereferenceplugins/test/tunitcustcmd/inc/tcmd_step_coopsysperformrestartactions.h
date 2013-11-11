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


#ifndef __TCMD_STEP_COOPSYSPERFORMRESTARTACTIONS_H__
#define __TCMD_STEP_COOPSYSPERFORMRESTARTACTIONS_H__

#include <test/testexecutestepbase.h>

_LIT(KTCCustomCmdTestCoopSysPerformRestartActions, "CCustomCmdTestCoopSysPerformRestartActions");

class CCustomCmdTestCoopSysPerformRestartActions: public CTestStep
	{
public:
	CCustomCmdTestCoopSysPerformRestartActions();
	~CCustomCmdTestCoopSysPerformRestartActions();

	//from CTestStep
	TVerdict doTestStepL();
private:
	void TestCustomCmdCoopSysPerformRestartActionsL();

private:
	};

#endif	// __TCMD_STEP_COOPSYSPERFORMRESTARTACTIONS_H__