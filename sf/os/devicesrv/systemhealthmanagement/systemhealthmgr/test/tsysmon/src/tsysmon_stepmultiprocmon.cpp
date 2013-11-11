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
#include <ssm/ssmstartupproperties.h>
#include "testapps.h"
#include "tsysmon_stepmultiprocmon.h"


CStepMultiProcMon::CStepMultiProcMon()
	{
	SetTestStepName(KCTestCaseMultiProcMon);
	}

CStepMultiProcMon::~CStepMultiProcMon()
	{
	iProcess1.Close();
	iProcess2.Close();
	}

/**
Old Test CaseID 		APPFWK-SYSMON-0002
New Test CaseID 		DEVSRVS-SHMA-SYSMON-0002
*/

TVerdict CStepMultiProcMon::doTestStepL()
	{
	INFO_PRINTF1(_L("TEST APPFWK-SYSMON-0002"));

	CreateProcessL();
	// Do the testing
	DoTestMonitorUsingStartupPropL();
	// Tidy up
	iProcess1.Kill(KErrNone);
	iProcess2.Kill(KErrNone);

	CreateProcessL();
	DoTestMonitorUsingSsmStartupPropL();
	// Tidy up
	iProcess1.Kill(KErrNone);
	iProcess2.Kill(KErrNone);

	INFO_PRINTF1(_L("Test Step completed"));
	return TestStepResult();	
	}

void CStepMultiProcMon::CreateProcessL()
	{
	// Launch two instances of the same executable
	INFO_PRINTF1(_L("Going to set first process running"));
	User::LeaveIfError(iProcess1.Create(KTestProcGood, KNullDesC));
	ResumeL(iProcess1);

	INFO_PRINTF1(_L("Starting a second instance of the same executable"));
	User::LeaveIfError(iProcess2.Create(KTestProcGood, KNullDesC));
	ResumeL(iProcess2);
	}

/**
 * Contains the actual testing. Verifying that it is possible to monitor more than one
 * instance of the same executable.
 */
void CStepMultiProcMon::DoTestMonitorUsingStartupPropL()
	{
	RSysMonSession sess;
	CleanupClosePushL(sess);	
	sess.OpenL();	

	INFO_PRINTF1(_L("Going to request process monitoring using startupproperties"));
	CStartupProperties *prop = CStartupProperties::NewLC(KTestProcGood, KNullDesC);
	prop->SetMonitored(ETrue);
	prop->SetNoOfRetries(0); // SysMon will not do any restart attempts
			
	INFO_PRINTF1(_L("Going to monitor first process"));
	sess.MonitorL(*prop, iProcess1);	
	INFO_PRINTF1(_L("Successfully setup monitoring of first process."));
	
	INFO_PRINTF1(_L("Going to monitor second process"));
	sess.MonitorL(*prop, iProcess2);
	INFO_PRINTF1(_L("Successfully setup monitoring of second process."));
		
	INFO_PRINTF1(_L("Try to register monitoring for second time"));
	TRAPD(err, sess.MonitorL(*prop, iProcess1));
	TESTE(err == KErrAlreadyExists, err);
	if(err == KErrAlreadyExists)
		{
		INFO_PRINTF1(_L("Second monitor request for first process returned KErrAlreadyExists as expected"));
		}
	TRAP(err, sess.MonitorL(*prop, iProcess2));
	TESTE(err == KErrAlreadyExists, err);
	if(err == KErrAlreadyExists)
		{
		INFO_PRINTF1(_L("Second monitor request for second process returned KErrAlreadyExists as expected"));
		}
		
	CleanupStack::PopAndDestroy(prop);
	CleanupStack::PopAndDestroy(&sess);
	INFO_PRINTF1(_L("Process monitoring using startupproperties Completed"));
	}

void CStepMultiProcMon::DoTestMonitorUsingSsmStartupPropL()
	{
	RSysMonSession sess;
	CleanupClosePushL(sess);	
	sess.OpenL();	

	INFO_PRINTF1(_L("Going to request process monitoring using ssmstartupproperties"));
	CSsmStartupProperties *ssmProp = CSsmStartupProperties::NewLC(KTestProcGood, KNullDesC);
	ssmProp->SetExecutionBehaviour(ESsmWaitForSignal);	
	TSsmMonitorInfo monitorInfo;
	monitorInfo.iRestartPolicy = ESsmIgnoreOnFailure;
	monitorInfo.iRestartMode = 0;
	monitorInfo.iTimeout = 0;
	monitorInfo.iRetries = 0;

	ssmProp->SetMonitorInfoL(monitorInfo);// SysMon will not do any restart attempts

	INFO_PRINTF1(_L("Going to monitor first process"));
	sess.MonitorL(*ssmProp, iProcess1);	
	INFO_PRINTF1(_L("Successfully setup monitoring of first process."));
	
	INFO_PRINTF1(_L("Going to monitor second process"));
	sess.MonitorL(*ssmProp, iProcess2);
	INFO_PRINTF1(_L("Successfully setup monitoring of second process."));

	INFO_PRINTF1(_L("Try to register monitoring for second time"));
	TRAPD(err, sess.MonitorL(*ssmProp, iProcess1));
	TESTE(err == KErrAlreadyExists, err);
	if(err == KErrAlreadyExists)
		{
		INFO_PRINTF1(_L("Second monitor request for first process returned KErrAlreadyExists as expected"));
		}
	TRAP(err, sess.MonitorL(*ssmProp, iProcess2));
	TESTE(err == KErrAlreadyExists, err);
	if(err == KErrAlreadyExists)
		{
		INFO_PRINTF1(_L("Second monitor request for second process returned KErrAlreadyExists as expected"));
		}

	CleanupStack::PopAndDestroy(ssmProp);
	CleanupStack::PopAndDestroy(&sess);
	INFO_PRINTF1(_L("Process monitoring using ssmstartupproperties Completed"));
	}
