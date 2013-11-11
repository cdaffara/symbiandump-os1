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
// @file ctlbsclockstepselectplugin.h
// This contains the header file for Select Clock Plugin Test Step
// 
//

/**
 @file
 @internalTechnology
 @test
*/

#ifndef __CT_LBS_CLOCK_STEP_SELECT_PLUGIN_H__
#define __CT_LBS_CLOCK_STEP_SELECT_PLUGIN_H__

#include "ctlbsclockstep.h"
#include "ctlbsclockserver.h"

_LIT(KLbsClockStep_SelectPlugin, "LbsClockStep_SelectPlugin"); 

class CT_LbsClockStep_SelectPlugin : public CT_LbsClockStep
	{
public:
	static CT_LbsClockStep_SelectPlugin* New(CT_LbsClockServer& aParent);
	TVerdict doTestStepL();

private:
	CT_LbsClockStep_SelectPlugin(CT_LbsClockServer& aParent);
	
	TBool SelectClockPluginL();
	
	};

#endif // __CT_LBS_CLOCK_STEP_SELECT_PLUGIN_H__

