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
// @file CTLbsStepSetPrivHandler.h
// This contains the header file for Set Privacy Controller Test Step
// 
//

/**
 @file
 @internalTechnology
 @test
*/

#ifndef __CT_LBS_STEP_SET_PRIVACY_HANDLER_H__
#define __CT_LBS_STEP_SET_PRIVACY_HANDLER_H__

#include "ctlbsstep.h"
#include "ctlbsserver.h"
#include <lbs/lbsadmin.h>

_LIT(KLbsStep_SetPrivacyHandler, "LbsStep_SetPrivacyHandler"); 

class CT_LbsStep_SetPrivacyHandler : public CT_LbsStep
	{
public:
	static CT_LbsStep_SetPrivacyHandler* New(CT_LbsServer& aParent);
	TVerdict doTestStepL();

private:
	CT_LbsStep_SetPrivacyHandler(CT_LbsServer& aParent);
	
	TBool SetAndVerifyPrivacyHandlerL(CLbsAdmin::TPrivacyHandler aPrivacyHandlerType);
	
	};

#endif // __CT_LBS_STEP_SET_PRIVACY_HANDLER_H__