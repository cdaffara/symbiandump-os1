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
//

/**
 @file
 @test
 @internalComponent
*/

#ifndef __T_WSERV_INTEG_STEP_FPS_H__
#define __T_WSERV_INTEG_STEP_FPS_H__

#include "t_wservintegstepbase.h"

_LIT(KWServFpsApp,           "z:\\sys\\bin\\t_fpsapp.exe");
_LIT(KT_WServIntegStepFps, "WServIntegFps");

class CT_WServIntegStepFps : public CT_WServIntegStep
	{
public:
    // Constructor
	CT_WServIntegStepFps();
	virtual enum TVerdict	doTestStepL();
	};

#endif //__T_WSERV_INTEG_STEP_FPS_H__
