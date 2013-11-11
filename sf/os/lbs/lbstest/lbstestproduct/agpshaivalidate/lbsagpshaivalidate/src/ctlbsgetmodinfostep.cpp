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
// @file ctlbsgetmodinfostep.cpp
// This is the class implementation for the step to start the lbs system
// EPOC includes.
// 
//

// LBS includes. 
#include <lbs/lbsmoduleinfo.h>

// LBS test includes.
#include "ctlbsgetmodinfostep.h"
#include "lbstestloggermacros.h"
#include "ctlbs.h"


/**
 * Destructor
 */
CT_LbsAGPSHAIValidateStep_GetModuleInfo::~CT_LbsAGPSHAIValidateStep_GetModuleInfo()
	{
	}


/**
 * Constructor
 */
CT_LbsAGPSHAIValidateStep_GetModuleInfo::CT_LbsAGPSHAIValidateStep_GetModuleInfo(CT_LbsAGPSHAIServerRoot& aParent) : iParent(aParent)
	{
	SetTestStepName(KLbsGetModuleInfoStep);
	}


/**
Static Constructor
*/
CT_LbsAGPSHAIValidateStep_GetModuleInfo* CT_LbsAGPSHAIValidateStep_GetModuleInfo::New(CT_LbsAGPSHAIServerRoot& aParent)
	{
	return new CT_LbsAGPSHAIValidateStep_GetModuleInfo(aParent);
	// Note the lack of ELeave.
	// This means that having insufficient memory will return NULL;
	}

 
/**
 * @return - TVerdict code
 * Override of base class pure virtual
 * Our implementation only gets called if the base class doTestStepPreambleL() did
 * not leave. That being the case, the current test result value will be EPass.
 */
TVerdict CT_LbsAGPSHAIValidateStep_GetModuleInfo::doTestStepL()
	{
	// Test step used to start the location manager.
	TESTLOG(ELogP1, ">> CT_LbsAGPSHAIValidateStep_GetModuleInfo::doTestStepL()");

	if (TestStepResult() == EPass)
		{			
		GetModuleInfoL();
		}
		
	TESTLOG(ELogP1, "<< CT_LbsAGPSHAIValidateStep_GetModuleInfo::doTestStepL()");

	return TestStepResult();
	}

void CT_LbsAGPSHAIValidateStep_GetModuleInfo::GetModuleInfoL()
	{
	TESTLOG(ELogP1, ">> CT_LbsAGPSHAIValidateStep_GetModuleInfo::GetModuleInfoL()");	
	
	TPositionModuleId 	id = {KLbsModuleManagerPairUidValue};

	TPositionModuleInfo info;
	
	RLbsModuleInfo modInfoApi;
	CleanupClosePushL(modInfoApi);
	modInfoApi.OpenL();
	
	TESTLOG(ELogP2, "Getting module info\n");	
	
	TInt err;
	err = modInfoApi.GetModuleInfoById(id, info);
	if(KErrNone != err)
		{
		TESTLOG2(ELogP3, "Cannot get module info for HAI module with id %d.\n", id);
		ERR_PRINTF2(_L("<font><b>FAILED: Cannot get module info for HAI module with id %d</font></b>"), id);
		SetTestStepResult(EFail);
		}
	else
		{
		TBuf<KPositionMaxModuleName> moduleName;
		info.GetModuleName(moduleName);
		
		INFO_PRINTF2(_L("<font><b>Using HAI module with name %S.</font></b>"), &moduleName);		
		
		TESTLOG2(ELogP2, "Got module info for module %S\n", &moduleName);	

		TESTLOG(ELogP2, "Getting expected quality info\n");	
		
		// TO DO: verify the details in the module info (non-zero accuracy etc)?
		
		info.GetPositionQuality(iParent.iModuleInfoQuality);
		
		TESTLOG(ELogP2, "Got expected quality info\n");	
		}
	CleanupStack::PopAndDestroy(&modInfoApi);
	TESTLOG(ELogP1, "<< CT_LbsAGPSHAIValidateStep_GetModuleInfo::GetModuleInfoL()");
	}

