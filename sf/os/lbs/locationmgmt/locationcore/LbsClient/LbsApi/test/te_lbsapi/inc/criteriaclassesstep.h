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
 @file CriteriaClassesStep.h
 @internalTechnology
*/
#if (!defined __CRITERIACLASSES_STEP_H__)
#define __CRITERIACLASSES_STEP_H__
#include <test/TestExecuteStepBase.h>
#include "te_lbsapisuitestepbase.h"

class CCriteriaClassesStep : public CTe_LbsApiSuiteStepBase
	{
public:
	CCriteriaClassesStep();
	~CCriteriaClassesStep();
	virtual TVerdict doTestStepPreambleL();
	virtual TVerdict doTestStepL();
	virtual TVerdict doTestStepPostambleL();

private:
	void StartL(TInt aIndex);
	
	void TPositionSelectionOrder_ConstructionL();
	void TPositionSelectionOrder_SetL();
	void TPositionSelectionOrder_ClearL();
	void TPositionSelectionOrder_NumSelectionItemsL();
	void TPositionCriteria_ConstructionL();
	void TPositionCriteria_SetL();
	void TPositionCriteria_ResetAndClearL();
	};

_LIT(KCriteriaClassesStep,"CriteriaClassesStep");

#endif
