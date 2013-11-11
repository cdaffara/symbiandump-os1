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
// This is the main DLL entry point for the TSU_MMF_DRMPlugin_U.dll
// 
//

// EPOC includes
#include <e32base.h>

// Test system includes
#include "TSU_MMF_DRMPluginSuite_U.h"
#include "TSU_MMF_DRMPlugin_U.h"



/**
 *
 * Create an instance of this test suite.
 *
 * NewTestSuiteL is exported at ordinal 1.
 * This provides the interface to allow the Test Framework
 * to create instances of this test suite.
 *
 * @return	"CTestSuite_MMF_DRMPlugin*"
 *			The newly created test suite
 */
EXPORT_C CTestSuite_MMF_DRMPlugin* NewTestSuiteL() 
    { 
	CTestSuite_MMF_DRMPlugin* result = new (ELeave) CTestSuite_MMF_DRMPlugin;
	CleanupStack::PushL(result);
	result->ConstructL();
	CleanupStack::Pop(result); // result
	return result;
    }

/**
 *
 * CTestSuiteDRMPluginasses destructor
 *
 * @xxxx
 * 
 */
CTestSuite_MMF_DRMPlugin::~CTestSuite_MMF_DRMPlugin()
	{
	}


/**
 *
 * Get test suite version.
 *
 * @return	"TPtrC"
 *			The version string.
 *
 * @xxxx
 *
 */
_LIT(KTxtVersion,"0.1");
TPtrC CTestSuite_MMF_DRMPlugin::GetVersion( void ) const
	{

	return KTxtVersion();
	}

/**
 *
 * Add a test step into the suite.
 *
 * @param	"CTestStepSelfTest* aPtrTestStep"
 *			The test step to be added.
 * @xxxx
 * 
 */


/**
 *
 * Initialiser for test suite.
 * This creates all the test steps and stores them
 * inside CTestSuiteDRMPluginasses
 *
 * @xxxx
 * 
 */
void CTestSuite_MMF_DRMPlugin::InitialiseL( void )
	{

	// store the name of this test suite
	iSuiteName = _L("TSU_MMF_DRMPlugin");

	// add test steps
	AddTestStepL( new(ELeave) CTestStep_MMF_DRMPlugin_U_0001 );
	AddTestStepL( new(ELeave) CTestStep_MMF_DRMPlugin_U_0002 );
	AddTestStepL( new(ELeave) CTestStep_MMF_DRMPlugin_U_0003 );
	AddTestStepL( new(ELeave) CTestStep_MMF_DRMPlugin_U_0004 );
	AddTestStepL( new(ELeave) CTestStep_MMF_DRMPlugin_U_0005 );
	AddTestStepL( new(ELeave) CTestStep_MMF_DRMPlugin_U_0006 );
	AddTestStepL( new(ELeave) CTestStep_MMF_DRMPlugin_U_0007 );
	AddTestStepL( new(ELeave) CTestStep_MMF_DRMPlugin_U_0008 );
	AddTestStepL( new(ELeave) CTestStep_MMF_DRMPlugin_U_0009 );
	AddTestStepL( new(ELeave) CTestStep_MMF_DRMPlugin_U_ImproperClosing(_L("MM-MMF-DRMPLUGIN-U-0010"), ECloseByPanic, ETrue) );
	AddTestStepL( new(ELeave) CTestStep_MMF_DRMPlugin_U_ImproperClosing(_L("MM-MMF-DRMPLUGIN-U-0011"), ECloseByPanic, EFalse) );
	AddTestStepL( new(ELeave) CTestStep_MMF_DRMPlugin_U_ImproperClosing(_L("MM-MMF-DRMPLUGIN-U-0012"), ECloseByKill, ETrue) );
	AddTestStepL( new(ELeave) CTestStep_MMF_DRMPlugin_U_ImproperClosing(_L("MM-MMF-DRMPLUGIN-U-0013"), ECloseByKill, EFalse) );
	AddTestStepL( new(ELeave) CTestStep_MMF_DRMPlugin_U_0014 );
	AddTestStepL( new(ELeave) CTestStep_MMF_DRMPlugin_U_0015 );
	AddTestStepL( new(ELeave) CTestStep_MMF_DRMPlugin_U_0016 );
	AddTestStepL( new(ELeave) CTestStep_MMF_DRMPlugin_U_0017 );
	AddTestStepL( new(ELeave) CTestStep_MMF_DRMPlugin_U_0018 );
	AddTestStepL( new(ELeave) CTestStep_MMF_DRMPlugin_U_InvalidHeapSize(_L("MM-MMF-DRMPLUGIN-U-0020"), _L("InvalidData"), _L("lo_heap_size ")) );
	AddTestStepL( new(ELeave) CTestStep_MMF_DRMPlugin_U_InvalidHeapSize(_L("MM-MMF-DRMPLUGIN-U-0021"), _L("InvalidData"), _L("hi_heap_size ")) );
	AddTestStepL( new(ELeave) CTestStep_MMF_DRMPlugin_U_0022 );
	AddTestStepL( new(ELeave) CTestStep_MMF_DRMPlugin_U_0023 );
	AddTestStepL( new(ELeave) CTestStep_MMF_DRMPlugin_U_ImproperClosingInvalidThread(_L("MM-MMF-DRMPLUGIN-U-0024"), ECloseByPanic) );
	AddTestStepL( new(ELeave) CTestStep_MMF_DRMPlugin_U_ImproperClosingInvalidThread(_L("MM-MMF-DRMPLUGIN-U-0025"), ECloseByKill) );
	AddTestStepL( new(ELeave) CTestStep_MMF_DRMPlugin_U_0026 );
	AddTestStepL( new(ELeave) CTestStep_MMF_DRMPlugin_U_0027 );
	AddTestStepL( new(ELeave) CTestStep_MMF_DRMPlugin_U_0028 );
	AddTestStepL( new(ELeave) CTestStep_MMF_DRMPlugin_U_0029 );
	AddTestStepL( new(ELeave) CTestStep_MMF_DRMPlugin_U_0030 );
	AddTestStepL( new(ELeave) CTestStep_MMF_DRMPlugin_U_0031 );
	AddTestStepL( new(ELeave) CTestStep_MMF_DRMPlugin_U_0032 );
	AddTestStepL( new(ELeave) CTestStep_MMF_DRMPlugin_U_0033 );
	AddTestStepL( new(ELeave) CTestStep_MMF_DRMPlugin_U_0040 );
	}


// -------------------------

/**
 *
 * Create an instance of CFSMNotifier
 *
 * @param	aListener
 *			a event receiver
 * @return	"CFSMNotifier*"
 *			The newly created FSM notifier
 */
CFSMNotifier* CFSMNotifier::NewL(MFSBListener* aListener)
	{
	CFSMNotifier* s = new(ELeave) CFSMNotifier(aListener);
	CleanupStack::PushL(s);
	s->ConstructL();
	CleanupStack::Pop();
	return s;
	}

/**
 *
 * CFSMNotifier destructor
 *
 * @xxxx
 * 
 */
CFSMNotifier::~CFSMNotifier()
	{
	Cancel();
	}

/**
 *
 * Issue a next state command
 *
 * @xxxx
 * 
 */
void CFSMNotifier::NextState(TTimeIntervalMicroSeconds32 anInterval)
	{
	ASSERT(!IsActive());
	iState++;
	After(anInterval);
	}
	
/**
 *
 * Default constructor
 *
 * @xxxx
 * 
 */
CFSMNotifier::CFSMNotifier(MFSBListener* aListener)
	: CTimer(EPriorityStandard), iState(-1), iListener(aListener)
	{
	}

void CFSMNotifier::ConstructL()
	{
	CTimer::ConstructL();
	CActiveScheduler::Add(this);
	}
	
void CFSMNotifier::RunL()
	{
	iListener->StateActivate(iState);
	}
// -------------------------
