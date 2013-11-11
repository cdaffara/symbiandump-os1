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
 @file CTestLtsySmsControlSendSmsCase1Step.h
 @internalTechnology
*/
#if (!defined __CTESTLTSYSMSCONTROLSENDSMSCASE1_STEP_H__)
#define __CTESTLTSYSMSCONTROLSENDSMSCASE1_STEP_H__
#include <TestExecuteStepBase.h>
#include "te_integration_stltsysuitestepbase.h"

class CCTestLtsySmsControlSendSmsCase1Step : public CTe_integration_stltsySuiteStepBase
	{
public:
	CCTestLtsySmsControlSendSmsCase1Step();
	~CCTestLtsySmsControlSendSmsCase1Step();
	virtual TVerdict doTestStepPreambleL();
	virtual TVerdict doTestStepL();
	virtual TVerdict doTestStepPostambleL();

// Please add/modify your class members here:
private:
	};

_LIT(KCTestLtsySmsControlSendSmsCase1Step,"CTestLtsySmsControlSendSmsCase1Step");

#endif
