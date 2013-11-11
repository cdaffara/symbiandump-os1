// Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// @file ctlbsstepsetuppsy.cpp
// This is the class implementation for the step to set up PSYs before running tests on S60
// EPOC includes.
// 
//

// LBS includes. 
#include <epos_cposmodules.h>
#include <epos_cposmoduleupdate.h>
#include <epos_cposmoduleidlist.h>

// LBS test includes.
#include "ctlbsstepsetuppsy.h"

/**
 * Destructor
 */
CT_LbsStep_SetUpPsy::~CT_LbsStep_SetUpPsy()
	{
	}


/**
 * Constructor
 */
CT_LbsStep_SetUpPsy::CT_LbsStep_SetUpPsy(CT_LbsServer& aParent) : CT_LbsStep(aParent)
	{
	SetTestStepName(KLbsStep_SetUpPsy);
	}


/**
Static Constructor
*/
CT_LbsStep_SetUpPsy* CT_LbsStep_SetUpPsy::New(CT_LbsServer& aParent)
	{
	return new CT_LbsStep_SetUpPsy(aParent);
	// Note the lack of ELeave.
	// This means that having insufficient memory will return NULL;
	}

TBool CT_LbsStep_SetUpPsy::GetUidFromConfig(const TDesC& aSectName, const TDesC& aKeyName, TUint32& aResult)
	{
	// Doing this because CTestStep::GetHexFromConfig is broken (returns the value in an Int for some reason)
	const TInt KMaxHexStringSize = 10;// enough for 0x12345678
	TBuf<KMaxHexStringSize> hexBuffer;
	TPtrC ptr;
	ptr.Set(hexBuffer);
	if(GetStringFromConfig(aSectName, aKeyName, ptr))
		{
		// strip any leading "0x" or "0X"
		if(ptr[0] == TChar('0') && (ptr[1] == TChar('X') || ptr[1] == TChar('x')))
			{
			// leading "0x" encoded. So strip
			hexBuffer = ptr.Right(ptr.Length()-2);
			}
		else
			{
			hexBuffer = ptr;
			}
		TLex lex(hexBuffer);
		TInt res = lex.Val(aResult, EHex);
		if(res != KErrNone)
			{
			return EFalse;
			}
		return ETrue;	
		}
	else
		{
		// can't find the hex string keyword
		return EFalse;
		}

	}

/**
 * @return - TVerdict code
 * Override of base class pure virtual
 * Our implementation only gets called if the base class doTestStepPreambleL() did
 * not leave. That being the case, the current test result value will be EPass.
 */
TVerdict CT_LbsStep_SetUpPsy::doTestStepL()
	{
	// Test step used to set up the psy used during a test
	INFO_PRINTF1(_L("&gt;&gt;CT_LbsStep_SetUpPsy::doTestStepL()"));

	//*******************************************************************/
	
	if (TestStepResult() == EPass)
		{
		// If this test step has been launched by a script that loads
		// the test server with -SharedData and installs an active scheduler
		// with a dedicated test step, then the scheduler is already installed
		// in the thread running this test step. If this test step is however
		// launched by a script that includes the command "CONCURRENT"
		// then the active scheduler is not present and one has to be installed
		// before running the rest of thie method as it adds AOs to the scheduler.
		if( NULL == CActiveScheduler::Current() )
			{
			iScheduler = new (ELeave) CActiveScheduler();
			CActiveScheduler::Install(iScheduler);
			}
			
		// Get PSY UID from config:
		TUid psyUid;
		TUint32 configUid;
		if(GetUidFromConfig(ConfigSection(), KPsyUid, configUid))
			{
			psyUid.iUid = configUid;
			
		    CPosModules* db = CPosModules::OpenL();
		    CleanupStack::PushL(db);
		    CPosModuleUpdate* moduleUpdate = CPosModuleUpdate::NewLC();
	
		    moduleUpdate->SetUpdateAvailability(EFalse);
		    CPosModuleIdList* prioList = db->ModuleIdListLC();
	
		    // Disable all PSY:s except aPsyUid
		    for (TInt i = 0 ; i < prioList->Count(); i++)
		        {
		        if ((*prioList)[i] != psyUid)
		            {
		            db->UpdateModuleL((*prioList)[i], *moduleUpdate);
		            }
		        }
	
		    // Enable the PSY that came as an in parameter
		    moduleUpdate->SetUpdateAvailability(ETrue);
		    db->UpdateModuleL(psyUid, *moduleUpdate);
	
		    CleanupStack::PopAndDestroy(prioList);
		    CleanupStack::PopAndDestroy(moduleUpdate);
		    CleanupStack::PopAndDestroy(db);
			}
	
		// If this step has installed its own scheduler...
		if (iScheduler)
			{
			//CActiveScheduler::Stop();
			delete iScheduler;
			}
		}
		
	INFO_PRINTF1(_L("&lt;&lt;CT_LbsStep_SetUpPsy::doTestStepL()"));

	return TestStepResult();
	}
