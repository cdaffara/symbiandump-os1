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
 @file CTestLtsySmsControlReceiveSmsCase1Step.h
 @internalTechnology
*/
#if (!defined __CTESTLTSYSMSCONTROLRECEIVESMSCASE1_STEP_H__)
#define __CTESTLTSYSMSCONTROLRECEIVESMSCASE1_STEP_H__
#include <TestExecuteStepBase.h>
#include "te_integration_stltsysuitestepbase.h"

class CCTestLtsySmsControlReceiveSmsCase1Step : public CTe_integration_stltsySuiteStepBase
	{
public:
	CCTestLtsySmsControlReceiveSmsCase1Step();
	~CCTestLtsySmsControlReceiveSmsCase1Step();
	virtual TVerdict doTestStepPreambleL();
	virtual TVerdict doTestStepL();
	virtual TVerdict doTestStepPostambleL();

// Please add/modify your class members here:
private:
	};

_LIT(KCTestLtsySmsControlReceiveSmsCase1Step,"CTestLtsySmsControlReceiveSmsCase1Step");

#endif
