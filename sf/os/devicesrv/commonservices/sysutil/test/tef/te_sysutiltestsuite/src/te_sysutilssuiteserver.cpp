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
// Test Server for the System Utils Tests
// For (WINS && !EKA2) versions will be xxxServer.Dll and require a thread to be started
// in the process of the client. The client initialises the server by calling the
// one and only ordinal.
// 
//

/**
 @file 
 @internalTechnology
*/

#include "te_sysutilssuiteserver.h"
#include "te_sysutilsgetswversion.h"
#include "te_sysutilsgetlangversion.h"
#include "te_sysutilsgetlangswversion.h"
#include "te_sysutilsffsspacebelowcritical.h"
#include "te_sysutilsmmcspacebelowcritical.h"
#include "te_sysutilsdiskspacebelowcritical.h"
#include "te_sysutilscheckrfsconnection.h"
#include "te_sysutilscheckdriveletter.h"

#include "te_sysutilsdiskspacenotfoundbelowcritical.h"
#include "te_sysutilsdiskspacebelowcriticallevellerror.h"
#include "te_sysutilsdiskspacebelowcriticalquery.h"
#include "te_sysutilsfssspacebelowcriticalquery.h"
#include "te_sysutilsmmcspacebelowcriticalquery.h"
#include "te_sysutilsmmcspacebelowcriticalrfs.h"
#include "te_sysutilsffsspacebelowcriticalrfs.h"
#include "te_sysutilsoomtest.h"
#include "te_sysutilsdummyapis.h"
#include "te_sysutilsdisksbelowcritical.h"
#include "te_sysutilsgetversioninsufficientbuffer.h"
#include "te_sysutilscheckdriveletter.h"

#include "te_sysutilstestdevicetypeinfo.h"
#include "te_sysutilstestlocalisedini.h"
#include "te_sysutilstestpersistenceini.h"
#include "te_sysutilstestattributeslimit.h"
#include "te_sysutilstestpluginoverride.h"
#include "te_sysutilstestperfswpsproperty.h"
#include "te_sysutilstestperflangpsproperty.h"
#include "te_sysutilstestperflangswpsproperty.h"
#include "te_sysutilstestperfdevicetypeinfopsproperty.h"

#include "te_sysutilsgetprinformation.h"

_LIT(KServerName,"te_sysutiltestsuite");

/**
Same code for Secure and non-secure variants
Called inside the MainL() function to create and start the
CTestServer derived server.

@return - Instance of the test server
 */
CTeSysUtilsSuite* CTeSysUtilsSuite::NewL()
	{
	CTeSysUtilsSuite * server = new (ELeave) CTeSysUtilsSuite();
	CleanupStack::PushL(server);

	server->ConstructL(KServerName);
	CleanupStack::Pop(server);
	return server;
	}



/**
Secure variant
Much simpler, uses the new Rendezvous() call to sync with the client
 */
LOCAL_C void MainL()
	{
	// Leave the hooks in for platform security
	RProcess().DataCaging(RProcess::EDataCagingOn);
	RProcess().DataCaging(RProcess::ESecureApiOn);
	CActiveScheduler* sched=NULL;
	sched=new(ELeave) CActiveScheduler;
	CActiveScheduler::Install(sched);
	CTeSysUtilsSuite* server = NULL;
	// Create the CTestServer derived server
	TRAPD(err,server = CTeSysUtilsSuite::NewL());
	if(!err)
		{
		// Sync with the client and enter the active scheduler
		RProcess::Rendezvous(KErrNone);
		sched->Start();
		}
	delete server;
	delete sched;
	}



/**
Secure variant only
Process entry point. Called by client using RProcess API

@return - Standard Epoc error code on process exit
 */
GLDEF_C TInt E32Main()
	{
	__UHEAP_MARK;
	CTrapCleanup* cleanup = CTrapCleanup::New();
	if(cleanup == NULL)
		{
		return KErrNoMemory;
		}
	TRAPD(err,MainL());
	delete cleanup;
	__UHEAP_MARKEND;
	return err;
	}


/**
Secure and non-secure variants
Implementation of CTestServer pure virtual

@return - A CTestStep derived instance
 */
CTestStep* CTeSysUtilsSuite::CreateTestStep(const TDesC& aStepName)
	{
	CTestStep* testStep = NULL;
	
	if(aStepName == KSysUtilsGetSWVersionStep)
		testStep = new CSysUtilsGetSWVersionStep();
	else if(aStepName == KSysUtilsGetLangSWVersionStep)
		testStep = new CSysUtilsGetLangSWVersionStep();
	else if(aStepName == KSysUtilsGetLangVersionStep)
		testStep = new CSysUtilsGetLangVersionStep();
	else if(aStepName == KSysUtilsFFSSpaceBelowCriticalStep)
		testStep = new CSysUtilsFFSSpaceBelowCriticalStep();
	else if(aStepName == KSysUtilsFssSpaceBelowCriticalQueryStep)
		testStep = new CSysUtilsFssSpaceBelowCriticalQueryStep();
	else if(aStepName == KSysUtilsMMCSpaceBelowCriticalStep)
		testStep = new CSysUtilsMMCSpaceBelowCriticalStep();
	else if(aStepName == KSysUtilsMmcSpaceBelowCriticalQueryStep)
		testStep = new CSysUtilsMmcSpaceBelowCriticalQueryStep();
	else if(aStepName == KSysUtilsDiskSpaceBelowCriticalStep)
		testStep = new CSysUtilsDiskSpaceBelowCriticalStep();
	else if(aStepName == KSysUtilsDiskSpaceNotFoundBelowCriticalStep)
		testStep = new CSysUtilsDiskSpaceNotFoundBelowCriticalStep();
	else if(aStepName == KSysUtilsDiskSpaceBelowCriticalLevelLErrorStep)
		testStep = new CSysUtilsDiskSpaceBelowCriticalLevelLErrorStep(); 
	else if(aStepName == KSysUtilsDiskSpaceBelowCriticalQueryStep)
		testStep = new CSysUtilsDiskSpaceBelowCriticalQueryStep();
	else if(aStepName == KSysUtilsMMCSpaceBelowCriticalRfsStep)
		testStep = new CSysUtilsMMCSpaceBelowCriticalRfsStep();
	else if(aStepName == KSysUtilsFFSSpaceBelowCriticalRfsStep)
		testStep = new CSysUtilsFFSSpaceBelowCriticalRfsStep();
	else if(aStepName == KSysUtilsOOMTestStep)
		testStep = new CSysUtilsOOMTestStep();
	else if(aStepName == KSysUtilsDummyAPIsStep)
		testStep = new CSysUtilsDummyAPIsStep();
	else if(aStepName == KSysUtilsDisksBelowCriticalStep)
		testStep = new CSysUtilsDisksBelowCriticalStep();
	else if(aStepName == KSysUtilsGetVersionInsufficienBufferStep)
		testStep = new CSysUtilsGetVersionInsufficienBufferStep();
	else if(aStepName == KSysUtilsCheckRFsConnectionDSStep)
		testStep = new CSysUtilsCheckRFsConnectionDSStep();
	else if(aStepName == KSysUtilsCheckRFsConnectionFFSStep)
		testStep = new CSysUtilsCheckRFsConnectionFFSStep();
	else if(aStepName == KSysUtilsCheckRFsConnectionMMCStep)
		testStep = new CSysUtilsCheckRFsConnectionMMCStep();
	else if(aStepName == KSysUtilsCheckDriveLetterFFSStep)
		testStep = new CSysUtilsCheckDriveLetterFFSStep();
	else if(aStepName == KSysUtilsCheckDriveLetterMMCStep)
		testStep = new CSysUtilsCheckDriveLetterMMCStep();
	else if(aStepName == KSysUtilsTestDeviceTypeInfoStep)
		testStep = new CSysUtilsTestDeviceTypeInfoStep();
	else if(aStepName == KSysUtilsTestLocalisedIniStep)
			testStep = new CSysUtilsTestLocalisedIniStep();
	else if(aStepName == KSysUtilsTestPersistenceIniStep)
		testStep = new CSysUtilsTestPersistenceIniStep();
	else if(aStepName == KSysUtilsTestAttributesLimitStep)
		testStep = new CSysUtilsTestAttributesLimitStep();
	else if(aStepName == KSysUtilsTestPluginOverrideStep)
		testStep = new CSysUtilsTestPluginOverrideStep();
	else if(aStepName == KSysUtilsTestPerfSWPSPropertyStep)
		testStep = new CSysUtilsTestPerfSWPSPropertyStep();
	else if(aStepName == KSysUtilsTestPerfLangPSPropertyStep)
		testStep = new CSysUtilsTestPerfLangPSPropertyStep();
	else if(aStepName == KSysUtilsTestPerfLangSWPSPropertyStep)
		testStep = new CSysUtilsTestPerfLangSWPSPropertyStep();
	else if(aStepName == KSysUtilsTestPerfDeviceTypeInfoPSPropertyStep)
		testStep = new CSysUtilsTestPerfDeviceTypeInfoPSPropertyStep();
	else if(aStepName == KSysUtilsGetPRInformationStep)
	   testStep = new CSysUtilsGetPRInformationStep();
	
	return testStep;
	}
