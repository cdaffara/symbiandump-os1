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
 @file LbsRootApiTest1Step.h
*/
#if (!defined __LBSROOTAPITEST1_STEP_H__)
#define __LBSROOTAPITEST1_STEP_H__
#include <test/testexecutestepbase.h>
#include "te_lbsrootapitestsuiteStepBase.h"


class CLbsRootApiTest1Step : public CTe_LbsRootApiTestSuiteStepBase
	{
public:
	CLbsRootApiTest1Step();
	~CLbsRootApiTest1Step();
	virtual TVerdict doTestStepPreambleL();
	virtual TVerdict doTestStepL();
	virtual TVerdict doTestStepPostambleL();

private:
	void CheckProcessesAreUp();
	};

_LIT(KLbsRootApiTest1Step,"LbsRootApiTest1Step");
#endif
