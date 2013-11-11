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
 @file NavModelFinalCheckAndCleanupStep.h
*/
#if (!defined __NavModelFinalCheckAndCleanup_STEP_H__)
#define __NavModelFinalCheckAndCleanup_STEP_H__
#include <test/testexecutestepbase.h>
#include "Te_LbsAssistanceDataSuiteStepBase.h"

class CNavModelFinalCheckAndCleanupStep : public CTe_LbsAssistanceDataSuiteStepBase
	{
public:
	CNavModelFinalCheckAndCleanupStep(CTe_LbsAssistanceDataSuite& aOwningServer);
	~CNavModelFinalCheckAndCleanupStep();
	virtual TVerdict doTestStepPreambleL();
	virtual TVerdict doTestStepL();
	virtual TVerdict doTestStepPostambleL();

	};

_LIT(KNavModelFinalCheckAndCleanupStep,"NavModelFinalCheckAndCleanupStep");

#endif
