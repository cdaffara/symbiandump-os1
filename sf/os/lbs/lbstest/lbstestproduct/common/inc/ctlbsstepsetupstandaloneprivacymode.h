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
// @file CT_LbsStep_SetupStandalonePrivacyMode.h
// This contains the header file for Set Autonomous Mode Test Step
// 
//

/**
 @file
 @internalTechnology
 @test
*/

#ifndef __CT_LBS_STEP_SETUP_STANDALONE_PRIVACY_MODE_H__
#define __CT_LBS_STEP_SETUP_STANDALONE_PRIVACY_MODE_H__

#include "ctlbsstep.h"
#include "ctlbsserver.h"

_LIT(KLbsStep_SetupStandalonePrivacyMode, "LbsStep_SetupStandalonePrivacyMode"); 

// lbsroot stuff (not ideal, currently defined in internal header file lbsrootcenrepdefs.h
const TUint KLbsCenRepUidValue					= 0x10282266;
const TUid  KLbsCenRepUid						= {KLbsCenRepUidValue};

const TUint32 KLbsRootProcessNameFirstKey			= 0x00001000;
const TUint32 KLbsRootProcessUidFirstKey			= 0x00002000;
const TUint32 KLbsRootProcessIsServerFirstKey		= 0x00003000;

const TUint32 KLbsSystemTransientKey				= 0x00000001;
const TUint32 KLbsRootProcessTransientKey			= 0x00000002;

const TUint32 KRootKeyMask							= 0xFFFFF000;	// don't care what the last 3 digits are


class CT_LbsStep_SetupStandalonePrivacyMode : public CT_LbsStep
	{
public:
	static CT_LbsStep_SetupStandalonePrivacyMode* New(CT_LbsServer& aParent);
	TVerdict doTestStepL();

private:
	CT_LbsStep_SetupStandalonePrivacyMode(CT_LbsServer& aParent);
	};

#endif // __CT_LBS_STEP_SETUP_STANDALONE_PRIVACY_MODE_H__