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
 @file LoadDllStep.h
*/
#if (!defined __LOADDLL_STEP_H__)
#define __LOADDLL_STEP_H__
#include <test/testexecutestepbase.h>
#include "Te_lbssupltestmoduleSuiteStepBase.h"


class CLoadDllStep : public CTe_lbssupltestmoduleSuiteStepBase, public MTestStepObserver
	{
public:
	CLoadDllStep(CTe_lbssupltestmoduleSuite* aServer);
	~CLoadDllStep();
	virtual TVerdict doTestStepPreambleL();
	virtual TVerdict doTestStepL();
	virtual TVerdict doTestStepPostambleL();

	// From MLoadDllObserver
	void notifyResult(const TInt& aResult);	
	
private:
	 CActiveScheduler* iActSchd;
	};


_LIT(KLoadDllStep,"LoadDllStep");

#endif
