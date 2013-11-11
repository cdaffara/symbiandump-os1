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
// @file ctlbsclockstep_selectplugin.cpp
// This is the class implementation for the Select the Clock Plugin Test Step
// 
//

/**
 @file
 @internalTechnology
 @test
*/
		  
#include "ctlbsclockstepselectplugin.h"
#include <lbs/lbsadmin.h>

_LIT(KPluginUid, "plugin_uid");

CT_LbsClockStep_SelectPlugin::CT_LbsClockStep_SelectPlugin(CT_LbsClockServer& aParent) : CT_LbsClockStep(aParent)
	{
	SetTestStepName(KLbsClockStep_SelectPlugin);
	}

/**
Static Constructor
*/
CT_LbsClockStep_SelectPlugin* CT_LbsClockStep_SelectPlugin::New(CT_LbsClockServer& aParent)
	{
	return new CT_LbsClockStep_SelectPlugin(aParent);
	// Note the lack of ELeave.
	// This means that having insufficient memory will return NULL;
	}


/**
 */
TBool CT_LbsClockStep_SelectPlugin::SelectClockPluginL()
 	{
	TBool ret = ETrue;
	CLbsAdmin* admin = CLbsAdmin::NewL();
	CleanupStack::PushL(admin);

	// Read plugin uid from ini file.
	TInt value;
	TLbsSetClockModuleId pluginUid = TUid::Uid(0x00), checkUid;

	if (GetIntFromConfig(ConfigSection(), KPluginUid, value))
		{
		pluginUid = TUid::Uid(value);	
		}
		
	// Inform the clock component to use the test plugin.
	INFO_PRINTF1(_L("About to set the clock plugin uid"));
	User::LeaveIfError(admin->Set(KLbsSettingSetClockModule, pluginUid));
	INFO_PRINTF1(_L("Wait for 10 sec"));
	User::After(10000);
	INFO_PRINTF1(_L("About to read the clock plugin uid"));
	User::LeaveIfError(admin->Get(KLbsSettingSetClockModule, checkUid));

	if (pluginUid != checkUid)
		{
		INFO_PRINTF1(_L("Admin setting didn't work"));
		ret = EFalse;
		}					
	CleanupStack::PopAndDestroy(admin); 	
	
	return ret;
 	}

/**
@pre 	
@return Test verdict
@post	GPSMode set to autonomous
*/
TVerdict CT_LbsClockStep_SelectPlugin::doTestStepL()
	{
	INFO_PRINTF1(_L("&gt;&gt;CT_LbsClockStep_SelectPlugin::doTestStepL()"));

	if(!SelectClockPluginL())
		{
		SetTestStepResult(EFail);
		}
	
	INFO_PRINTF1(_L("&lt;&lt;CT_LbsClockStep_SelectPlugin::doTestStepL()"));

	return TestStepResult();
	}

