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
 

#ifndef __TCLE_STEP_WFS_BAD_H__
#define __TCLE_STEP_WFS_BAD_H__

#include "tcle_step_base.h"

_LIT(KTCleWfsBadStep,"CleWfsBadStep");

class CCleWfsBadStep : public CCleTestStepBase
	{
public:
	CCleWfsBadStep();
	~CCleWfsBadStep();

	//from CTestStep
	TVerdict doTestStepL();
	
protected:
	void CheckResultsL(TInt aCount);

	};


#endif //__TCLE_STEP_WFS_BAD_H__
