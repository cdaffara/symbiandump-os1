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
// @file CT_LbsStep_ResetDefaultAdmin.h
// This contains the header file for Reset Default Admin Settings Test Step
// 
//

/**
 @file
 @internalTechnology
 @test
*/

#ifndef __CT_LBS_STEP_RESET_DEFAULT_ADMIN_H__
#define __CT_LBS_STEP_RESET_DEFAULT_ADMIN_H__

#include "ctlbsstep.h"
#include "ctlbsserver.h"

_LIT(KLbsStep_ResetDefaultAdmin, "LbsStep_ResetDefaultAdmin"); 

class CT_LbsStep_ResetDefaultAdmin : public CT_LbsStep
	{
public:
	static CT_LbsStep_ResetDefaultAdmin* New(CT_LbsServer& aParent);
	TVerdict doTestStepL();

private:
	CT_LbsStep_ResetDefaultAdmin(CT_LbsServer& aParent);
	
	
	};

#endif // __CT_LBS_STEP_RESET_DEFAULT_ADMIN_H__