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
// Performs the Out of Memory Tests.\n
// 
//

/**
 @file
 @test
 @internalComponent - Internal Symbian test code  
*/

#include <sysmonclisess.h>
#include <startupproperties.h>
#include "tsysmon_stepoomsysmon.h"
#include "testapps.h"

/**
   Constructor
 */
CTOOMSysMonTestStep::CTOOMSysMonTestStep()
	{
	// Call base class method to set up the human readable name for logging
	SetTestStepName(KOOMTestCaseSysMon);
	}

/**
Old Test CaseID 		APPFWK-SYSMON-0062
New Test CaseID 		DEVSRVS-SHMA-SYSMON-0062
*/

TVerdict CTOOMSysMonTestStep::doTestStepL()
	{
	INFO_PRINTF1(_L("Testing System Monitor...OOM tests"));

	TInt fail = 0;
	TInt ret = 0;
	
	RSysMonSession sysMonSes;

	//OOM test for RSysMonSession::OpenL() method.
	for (fail = 1;; fail++)
		{
		__UHEAP_SETFAIL(RHeap::EDeterministic, fail);
		__UHEAP_MARK;
		TRAP(ret, sysMonSes.OpenL());
		TEST((ret == KErrNone) || (ret == KErrNoMemory));
		if (ret == KErrNone)
			{
			sysMonSes.Close();
			INFO_PRINTF1(_L("Created a session with system monitor"));
			__UHEAP_MARKEND;
			break;
			}
		__UHEAP_MARKEND;
		}
	__UHEAP_RESET;	

	INFO_PRINTF1(_L("Going to set process running"));

	RProcess proc;
	CleanupClosePushL(proc);
	User::LeaveIfError(proc.Create(KTestProcGood, KNullDesC));
	ResumeL(proc);

	INFO_PRINTF1(_L("Going to request process monitoring using startupproperties"));
	CStartupProperties *prop = CStartupProperties::NewLC(KTestProcGood, KNullDesC);
	prop->SetMonitored(ETrue);
	prop->SetNoOfRetries(0); // SysMon will not do any restart attempts

	sysMonSes.OpenL();

	//OOM test for RSysMonSession::MonitorL() method.
	for (fail = 1;; fail++)
		{
		__UHEAP_SETFAIL(RHeap::EDeterministic, fail);
		__UHEAP_MARK;
		TRAP(ret, sysMonSes.MonitorL(*prop, proc));
		TEST((ret == KErrNone) || (ret == KErrNoMemory));
		if (ret == KErrNone)
			{
			INFO_PRINTF1(_L("Successfully setup monitoring of process."));
			TRAP(ret, sysMonSes.CancelMonitorSelfL());
			TEST(ret == KErrNone);
			sysMonSes.Close();
			__UHEAP_MARKEND;
			break;
			}
		__UHEAP_MARKEND;
		}
	__UHEAP_RESET;	
	proc.Kill(KErrNone);

	RProcess proc1;
	CleanupClosePushL(proc1);
	User::LeaveIfError(proc1.Create(KTestProcGood, KNullDesC));
	ResumeL(proc1);

	sysMonSes.OpenL();
	CleanupClosePushL(sysMonSes);
	TRAP(ret, sysMonSes.MonitorL(*prop, proc1));
	TEST(ret == KErrNone);

	//OOM test for RSysMonSession::CancelMonitorSelfL() method.
	for (fail = 1;; fail++)
		{
		__UHEAP_SETFAIL(RHeap::EDeterministic, fail);
		__UHEAP_MARK;
		TRAP(ret, sysMonSes.CancelMonitorSelfL());
		TEST((ret == KErrNone) || (ret == KErrNoMemory));
		if (ret == KErrNone)
			{
			sysMonSes.Close();
			INFO_PRINTF1(_L("Successfully cancelled the monitoring of process."));
			__UHEAP_MARKEND;
			break;
			}
		__UHEAP_MARKEND;
		}
	__UHEAP_RESET;	

	proc1.Kill(KErrNone);
	CleanupStack::PopAndDestroy(4, &proc);

	INFO_PRINTF1(_L("System Monitor...OOM tests completed"));
	return TestStepResult();
	}


