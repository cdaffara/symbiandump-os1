/**
* Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
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
 @file startsimtsyStep.h
*/
#if (!defined STARTSIMTSY_STEP_H)
#define STARTSIMTSY_STEP_H
#include <testexecutestepbase.h>
#include "Te_EsockStepBase.h"

/**
Class implementing startsimtsyStep

@internalComponent
*/
class CstartsimtsyStep : public CTe_EsockStepBase
	{
public:
	CstartsimtsyStep(CCEsockTestBase*& aEsockTest);
	TVerdict doTestStepPreambleL();
	TVerdict doSingleTestStep();
	TBool DescriptionExists();

	void SetTestSectionNumber(TInt aTestNumber);
    TInt TestSectionNumber();
private:
    //TPtrC iSimTsyName;
    TInt iTestSectionNumber;
	};

_LIT(KstartsimtsyStep,"startsimtsyStep");

#endif //STARTSIMTSY_STEP_H

