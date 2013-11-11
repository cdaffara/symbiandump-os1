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
 @internalComponent - Internal Symbian test code 
*/
 

#ifndef __TCLE_STEP_RCLESESSIONFAIL_H__
#define __TCLE_STEP_RCLECESSIONFAIL_H__

#include <test/testexecutestepbase.h>

#include "clesrv.h"
#include "clecli.h"

#include "tcle_cleclient.h"

_LIT(KTCleRCleSessionFailStep,"CleRCleSessionFailStep");

class CCleRCleSessionFailTest : public CTestStep
	{
public:
	CCleRCleSessionFailTest();
	~CCleRCleSessionFailTest();

	//from CTestStep
	TVerdict doTestStepPreambleL();
	TVerdict doTestStepL();
	TVerdict doTestStepPostambleL();

private:
	
private:
	RCleSessionTest iCleCli;
	};


#endif
