/**
* Copyright (c) 2002-2009 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description:
*
*/



/**
 @file ProcessLaunchTest1Step.h
 @internalTechnology
*/
#if (!defined __PROCESSLAUNCHTEST1_STEP_H__)
#define __PROCESSLAUNCHTEST1_STEP_H__

#include <e32property.h>
#include <test/TestExecuteStepBase.h>
#include "Te_ProcessLaunchTestSuiteStepBase.h"

class CProcessLaunchTest1Step : public CTe_ProcessLaunchTestSuiteStepBase
	{
public:
	CProcessLaunchTest1Step();
	~CProcessLaunchTest1Step();
	virtual TVerdict doTestStepPreambleL();
	virtual TVerdict doTestStepL();
	virtual TVerdict doTestStepPostambleL();

// Please add/modify your class members here:
private:
	RProperty iSuicide;
	};

_LIT(KProcessLaunchTest1Step,"ProcessLaunchTest1Step");

#endif // __PROCESSLAUNCHTEST1_STEP_H__
