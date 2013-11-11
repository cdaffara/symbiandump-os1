/**
* Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
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
 @file NavModelSatInfoNodeStep.h
*/
#if (!defined __NAVMODELSATINFONODE_STEP_H__)
#define __NAVMODELSATINFONODE_STEP_H__
#include <test/testexecutestepbase.h>
#include "Te_LbsAssistanceDataSuiteStepBase.h"

class CNavModelSatInfoNodeStep : public CTe_LbsAssistanceDataSuiteStepBase
	{
public:
	CNavModelSatInfoNodeStep(CTe_LbsAssistanceDataSuite& aOwningTestServer);
	~CNavModelSatInfoNodeStep();
	virtual TVerdict doTestStepPreambleL();
	virtual TVerdict doTestStepL();
	virtual TVerdict doTestStepPostambleL();
	};

_LIT(KNavModelSatInfoNodeStep,"NavModelSatInfoNodeStep");

#endif
