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

#include <sysmonclisess.h>
#include <startupproperties.h>
#include "testapps.h"
#include "tsysmon_stepnoprocmon.h"


CStepNoProcMon::CStepNoProcMon()
	{
	SetTestStepName(KCTestCaseNoProcMon);
	}

TVerdict CStepNoProcMon::doTestStepL()
	{
	INFO_PRINTF1(_L("TEST APPFWK-SYSMON-0007"));
	
	DoTestNullHandleL();
	DoTestKilledHandleL();
	DoTestWrongHandleL();
	
	return TestStepResult();
	}

/**
Old Test CaseID 		APPFWK-SYSMON-0007
New Test CaseID 		DEVSRVS-SYSSTART-SYSMON-0007
 */

void CStepNoProcMon::DoTestNullHandleL()
	{
	INFO_PRINTF1(_L("Requesting SysMon to monitor a process NULL handle"));
	
	RProcess process;
	process.Close();

	RSysMonSession sess;
	sess.OpenL();
	CleanupClosePushL(sess);
	
	CStartupProperties *prop = CStartupProperties::NewLC(KTestProcGood, KNullDesC);
	prop->SetMonitored(ETrue);
	prop->SetStartMethod(EWaitForStart);	
	prop->SetRecoveryParams(EIgnoreOnFailure, 0);
	prop->SetNoOfRetries(1);
	prop->SetTimeout(0);
			
	TRAPD(err, sess.MonitorL(*prop, process)); // check that passing in a null handle would return KErrArugment
	
	// check that the initiation of monitoring failed 
	TESTE(KErrArgument == err, err);
	if(KErrArgument == err)
		{
		INFO_PRINTF1(_L("Request refused with KErrArgument"));
		}
		
	CleanupStack::PopAndDestroy(prop);;
	CleanupStack::PopAndDestroy(&sess);
	}

/**
Check that passing in a handle to a killed process will return KErrArugment
*/
void CStepNoProcMon::DoTestKilledHandleL()
	{
	INFO_PRINTF1(_L("Requesting SysMon to monitor a handle to a killed process"));

	RProcess process;
	CleanupClosePushL(process);
	User::LeaveIfError(process.Create(KTestProcGood, KNullDesC));
	ResumeL(process);
	
	process.Kill(KErrNone);
	INFO_PRINTF1(_L("Process killed."));
	
	RSysMonSession sess;
	sess.OpenL();
	CleanupClosePushL(sess);
	
	CStartupProperties* prop = CStartupProperties::NewLC(KTestProcGood, KNullDesC);
	prop->SetMonitored(ETrue);
	prop->SetStartMethod(EWaitForStart);	
	prop->SetRecoveryParams(EIgnoreOnFailure, 0);
	prop->SetNoOfRetries(1);
	prop->SetTimeout(0);
				
	TRAPD(err, sess.MonitorL(*prop, process));
	
	// check that the initiation of monitoring failed 
	TESTE(KErrDied == err, err);
	if(KErrDied == err)
		{
		INFO_PRINTF1(_L("Request refused with KErrDied"));
		}
		
	CleanupStack::PopAndDestroy(prop);;
	CleanupStack::PopAndDestroy(&sess);	
	CleanupStack::PopAndDestroy(&process);
	}

void CStepNoProcMon::DoTestWrongHandleL()
	{
	INFO_PRINTF1(_L("Requesting SysMon to monitor wrong process handle"));
	
	RProcess process; // initiated with handle to current process

	RSysMonSession sess;
	sess.OpenL();
	CleanupClosePushL(sess);
	
	CStartupProperties *prop = CStartupProperties::NewLC(KTestProcGood, KNullDesC);
	prop->SetMonitored(ETrue);
	prop->SetStartMethod(EWaitForStart);	
	prop->SetRecoveryParams(EIgnoreOnFailure, 0);
	prop->SetNoOfRetries(1);
	prop->SetTimeout(0);
			
	TRAPD(err, sess.MonitorL(*prop, process)); // check that passing in a null handle would return KErrArugment
	
	// check that the initiation of monitoring failed 
	TESTE(KErrArgument == err, err);
	if(KErrArgument == err)
		{
		INFO_PRINTF1(_L("Request refused with KErrArgument"));
		}
		
	CleanupStack::PopAndDestroy(prop);;
	CleanupStack::PopAndDestroy(&sess);
	}


