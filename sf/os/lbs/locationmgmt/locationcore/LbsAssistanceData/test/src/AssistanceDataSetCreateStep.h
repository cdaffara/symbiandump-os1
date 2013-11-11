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
 @file AssistanceDataSetCreateStep.h
*/
#if (!defined __ASSISTDATASETCREATE_STEP_H__)
#define __ASSISTDATASETCREATE_STEP_H__
#include <test/testexecutestepbase.h>
#include "Te_LbsAssistanceDataSuiteStepBase.h"
#include <lbs/lbsassistancedatabuilderset.h>

class CAssistanceDataSetCreateStep : public CTe_LbsAssistanceDataSuiteStepBase
	{
public:
	CAssistanceDataSetCreateStep(CTe_LbsAssistanceDataSuite& aOwningServer);
	~CAssistanceDataSetCreateStep();
	virtual TVerdict doTestStepPreambleL();
	virtual TVerdict doTestStepL();
	virtual TVerdict doTestStepPostambleL();

private:
	};

_LIT(KAssistanceDataSetCreateStep,"AssistanceDataSetCreateStep");

#endif
