// Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef __T_WSERV_SCREENCONSTRUCT_STEP_LOAD_H__
#define __T_WSERV_SCREENCONSTRUCT_STEP_LOAD_H__

#include <fbs.h>
#include <w32std.h>
#include <test/testexecutestepbase.h>
#include "twservscreenconstructconsts.h"

class CTWServScreenConstructStepLoad : public CTestStep
	{
public:
	CTWServScreenConstructStepLoad();
	~CTWServScreenConstructStepLoad();
	// from CTestStep
	virtual enum TVerdict doTestStepPreambleL();
	virtual enum TVerdict doTestStepL();
	virtual enum TVerdict doTestStepPostambleL();

private:	// Data members
	CWindowGc* 				iGc;
	CWsScreenDevice* 		iScreen;
	RWsSession 				iWsSession;
	RWindowGroup 			iWinGroup;
	};

#endif // __T_WSERV_SCREENCONSTRUCT_STEP_LOAD_H__
