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
// Description: Test for testing sysmon's CancelAllMonitor functionality
//

/**
 @file
 @test
 @internalComponent - Internal Symbian test code
*/
 
#ifndef __TSYSMON_STEPCANCELALLMONITORS_H__
#define __TSYSMON_STEPCANCELALLMONITORS_H__


#include "tsysmon_stepbase.h"
#include <e32property.h>

_LIT(KCTestCaseCancelAllMonitors, "CancelAllMonitors");

class CStepCancelAllMonitors : public CTestStepBase
	{
public:
    CStepCancelAllMonitors();
	
	//from CTestStep
	TVerdict doTestStepL();
private:
    ~CStepCancelAllMonitors();
	void DoTestCancelAllMonitorsL();
	
private:
    RSemaphore iProcStartSignalSem;
    RSemaphore iAppStartSignalSem;
	};

#endif  //__TSYSMON_STEPCANCELALLMONITORS_H__
