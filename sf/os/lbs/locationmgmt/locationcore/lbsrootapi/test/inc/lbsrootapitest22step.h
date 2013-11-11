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
 @file LbsRootApiTest22Step.h
*/
#if (!defined __LBSROOTAPITEST22_STEP_H__)
#define __LBSROOTAPITEST22_STEP_H__
#include <test/testexecutestepbase.h>
#include "te_lbsrootapitestsuiteStepBase.h"

class CLbsRootApiTest22Step : public CTe_LbsRootApiTestSuiteStepBase
	{
public:
	CLbsRootApiTest22Step();
	~CLbsRootApiTest22Step();
	virtual TVerdict doTestStepPreambleL();
	virtual TVerdict doTestStepL();
	virtual TVerdict doTestStepPostambleL();

private:
	TBool CheckProcessIsRunning(const TDesC& aProcessNamePattern);
	TBool CheckProcessesAreRunning();

private:
	TInt iRestartCount;
	};

_LIT(KLbsRootApiTest22Step,"LbsRootApiTest22Step");
#endif // __LBSROOTAPITEST22_STEP_H__
