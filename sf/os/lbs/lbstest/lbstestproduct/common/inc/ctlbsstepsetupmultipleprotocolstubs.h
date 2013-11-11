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
// @file CT_LbsStep_SetupProtocolStub.h
// Sets up the multiple PM stubs
// 
//

/**
 @file
 @internalTechnology
 @test
*/

#ifndef __CT_LBS_STEP_SETUP_MULTIPLE_PROTOCOL_STUBS_H__
#define __CT_LBS_STEP_SETUP_MULTIPLE_PROTOCOL_STUBS_H__

#include "ctlbsstep.h"
#include "ctlbsserver.h"

_LIT(KLbsStep_SetupMultipleProtocolStubs, "LbsStep_SetupMultipleProtocolStubs"); 

class CT_LbsStep_SetupMultipleProtocolStubs : public CT_LbsStep
	{
public:
	static CT_LbsStep_SetupMultipleProtocolStubs* New(CT_LbsServer& aParent);
	TVerdict doTestStepL();

private:
	CT_LbsStep_SetupMultipleProtocolStubs(CT_LbsServer& aParent);
	};

#endif // __CT_LBS_STEP_SETUP_MULTIPLE_PROTOCOL_STUBS_H__