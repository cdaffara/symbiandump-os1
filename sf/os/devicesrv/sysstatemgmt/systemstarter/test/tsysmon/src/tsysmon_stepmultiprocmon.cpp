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
#include "tsysmon_stepmultiprocmon.h"


CStepMultiProcMon::CStepMultiProcMon()
	{
	SetTestStepName(KCTestCaseMultiProcMon);
	}

/**
Old Test CaseID 		APPFWK-SYSMON-0002
New Test CaseID 		DEVSRVS-SYSSTART-SYSMON-0002
 */

TVerdict CStepMultiProcMon::doTestStepL()
	{
	INFO_PRINTF1(_L("TEST APPFWK-SYSMON-0002"));
	
	// Launch two instances of the same executable
	
	INFO_PRINTF1(_L("Going to set first process running"));
	RProcess process1;
	CleanupClosePushL(process1);
	User::LeaveIfError(process1.Create(KTestProcGood, KNullDesC));
	ResumeL(process1);

	INFO_PRINTF1(_L("Starting a second instance of the same executable"));
	RProcess process2;
	CleanupClosePushL(process2);
	User::LeaveIfError(process2.Create(KTestProcGood, KNullDesC));
	ResumeL(process2);
	
	// Do the testing
	DoTestMonitorL(process1, process2);
	
	// Tidy up
	process1.Kill(KErrNone);
	process2.Kill(KErrNone);
	CleanupStack::PopAndDestroy(&process2);
	CleanupStack::PopAndDestroy(&process1);
	
	
	return TestStepResult();	
	}

/**
 * Contains the actual testing. Verifying that it is possible to monitor more than one
 * instance of the same executable.
 */
void CStepMultiProcMon::DoTestMonitorL(const RProcess& aProc1, const RProcess& aProc2)
	{
	RSysMonSession sess;
	CleanupClosePushL(sess);	
	sess.OpenL();	

	
	CStartupProperties *prop = CStartupProperties::NewLC(KTestProcGood, KNullDesC);
	prop->SetMonitored(ETrue);
	prop->SetNoOfRetries(0); // SysMon will not do any restart attempts
			
	INFO_PRINTF1(_L("Going to monitor first process"));
	sess.MonitorL(*prop, aProc1);	
	INFO_PRINTF1(_L("Successfully setup monitoring of first process."));
	
	INFO_PRINTF1(_L("Going to monitor second process"));
	sess.MonitorL(*prop, aProc2);
	INFO_PRINTF1(_L("Successfully setup monitoring of second process."));
		
	INFO_PRINTF1(_L("Try to register monitoring for second time"));
	TRAPD(err, sess.MonitorL(*prop, aProc1));
	TESTE(err == KErrAlreadyExists, err);
	if(err == KErrAlreadyExists)
		{
		INFO_PRINTF1(_L("Second monitor request for first process returned KErrAlreadyExists as expected"));
		}
	TRAP(err, sess.MonitorL(*prop, aProc2));
	TESTE(err == KErrAlreadyExists, err);
	if(err == KErrAlreadyExists)
		{
		INFO_PRINTF1(_L("Second monitor request for second process returned KErrAlreadyExists as expected"));
		}
		
	CleanupStack::PopAndDestroy(prop);
	CleanupStack::PopAndDestroy(&sess);
	}
