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
 @file CalculationMethodsStep.h
 @internalTechnology
*/
#if (!defined __CALCULATIONMETHODS_STEP_H__)
#define __CALCULATIONMETHODS_STEP_H__
#include <test/TestExecuteStepBase.h>
#include "te_lbsapisuitestepbase.h"

class CCalculationMethodsStep : public CTe_LbsApiSuiteStepBase
	{
public:
	CCalculationMethodsStep();
	~CCalculationMethodsStep();
	virtual TVerdict doTestStepPreambleL();
	virtual TVerdict doTestStepL();
	virtual TVerdict doTestStepPostambleL();

private:
	void StartL(TInt aIndex);
	void Calculation_InvalidL();
	void Calculation_BasicsL();
	void Calculation_TCoordinate_MoveBigL();
	};

_LIT(KCalculationMethodsStep,"CalculationMethodsStep");

#endif
