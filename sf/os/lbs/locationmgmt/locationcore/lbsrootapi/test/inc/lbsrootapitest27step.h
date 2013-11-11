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
 @file LbsRootApiTest27Step.h
*/
#if (!defined __LBSROOTAPITEST27_STEP_H__)
#define __LBSROOTAPITEST27_STEP_H__
#include <test/testexecutestepbase.h>
#include "te_lbsrootapitestsuiteStepBase.h"

class CLbsRootApiTest27Step : public CTe_LbsRootApiTestSuiteStepBase
	{
public:
	CLbsRootApiTest27Step();
	~CLbsRootApiTest27Step();
	virtual TVerdict doTestStepPreambleL();
	virtual TVerdict doTestStepL();
	virtual TVerdict doTestStepPostambleL();
// Please add/modify your class members here:
private:
    void DoMainL();
	};

_LIT(KLbsRootApiTest27Step,"LbsRootApiTest27Step");

#endif // __LBSROOTAPITEST27_STEP_H__
