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
 @file AssistanceDataSetMergeStep.h
*/
#if (!defined __ASSISTDATASETMERGE_STEP_H__)
#define __ASSISTDATASETMERGE_STEP_H__
#include <test/testexecutestepbase.h>
#include <lbs/lbsassistancedatabuilderset.h>
#include "Te_LbsAssistanceDataSuiteStepBase.h"

class CAssistanceDataSetMergeStep : public CTe_LbsAssistanceDataSuiteStepBase
	{
public:
	CAssistanceDataSetMergeStep(CTe_LbsAssistanceDataSuite& aOwningServer);
	~CAssistanceDataSetMergeStep();
	virtual TVerdict doTestStepPreambleL();
	virtual TVerdict doTestStepL();
	virtual TVerdict doTestStepPostambleL();

private:
	RLbsAssistanceDataBuilderSet set1;
	RLbsAssistanceDataBuilderSet set2;
	TTime time1;
	TTime time2;
	};

_LIT(KAssistanceDataSetMergeStep,"AssistanceDataSetMergeStep");

#endif
