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
 

#ifndef __TCLE_STEP_FANDF_GOOD_H__
#define __TCLE_STEP_FANDF_GOOD_H__

#include "tcle_step_base.h"

_LIT(KTCleFandFGoodStep,"CleFandFGoodStep");

class CCleFandFGoodStep : public CCleTestStepBase
	{
public:
	CCleFandFGoodStep();
	~CCleFandFGoodStep();

	//from CTestStep
	TVerdict doTestStepL();
	};


#endif //__TCLE_STEP_FANDF_GOOD_H__
