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
 
#ifndef __TSYSMON_STEPBASE_H__
#define __TSYSMON_STEPBASE_H__

#include <startup.hrh>
#include <test/testexecutestepbase.h>


_LIT(KAppFwkSysMonTestPanic, "AppFwkSysMonTestPanic");

// Default throttle time of SysMon (cant be included because it is defined in a cpp file)
const TInt KThrottleTime = 15000000;

class RProcess;
class CStartupProperties;

class CTestStepBase : public CTestStep
	{
protected:
	void ResumeL(RProcess& aProcess);
	void MonitorL(const RProcess& aProcess, const CStartupProperties& aProp);
	void StartAndMonitorL(RProcess& aProcess, TStartMethod aStartMethod);
	static void StartViewlessBgApplicationL(const TDesC& aAppName, TThreadId& aTreadId, TRequestStatus& aRequestStatus);
	static TBool Exists(const TDesC& aProcessName);
	};

#endif
