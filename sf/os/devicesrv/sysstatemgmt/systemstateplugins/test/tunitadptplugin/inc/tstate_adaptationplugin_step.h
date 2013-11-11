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


#ifndef __TSTATE_ADAPTATIONPLUGIN_STEP_H__
#define __TSTATE_ADAPTATIONPLUGIN_STEP_H__

#include "tadaptation_step_base.h"
#include "tstate_adaptationplugin_step.h"
#include <ssm/ssmadaptationcli.h>

class MStateAdaptation;

_LIT(KTCTestStateAdaptationPlugin, "CTestStateAdaptationPlugin");

class CTestStateAdaptationPlugin : public CAdaptationTestBase
	{
public:
	CTestStateAdaptationPlugin();
	~CTestStateAdaptationPlugin();

	//from CAdaptationTestBase
	TVerdict doTestStepL();

private:

	void TestRequestCoopSysStateChangeL();
	void TestRequestCoopSysSelfTestL();
	void TestRequestCoopSysPerformRestartActionsL();
	void TestRequestCoopSysPerformShutdownActionsL();
	void TestRequestCoopSysPerformRfsActionsL();
	void TestRequestCancel();
	void TestNotifyCoopSysEventL();
	void TestNotifyCancel();

private:
	TRequestStatus iRequestStatus;
	RSsmStateAdaptation iStateAdaptation;
	};

#endif	// __TSTATE_ADAPTATIONPLUGIN_STEP_H__
