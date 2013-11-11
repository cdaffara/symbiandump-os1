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
 @file RepeatGetAssistanceDataStep.h
*/
#if (!defined __REPEATGETASSISTANCEDATA_STEP_H__)
#define __REPEATGETASSISTANCEDATA_STEP_H__
#include <test/testexecutestepbase.h>
#include "Te_lbssupltestmoduleSuiteStepBase.h"


class CRepeatGetAssistanceDataStep : public CTe_lbssupltestmoduleSuiteStepBase, public MTestStepObserver
	{
public:
	CRepeatGetAssistanceDataStep(CTe_lbssupltestmoduleSuite* aServer);
	~CRepeatGetAssistanceDataStep();
	virtual TVerdict doTestStepPreambleL();
	virtual TVerdict doTestStepL();
	virtual TVerdict doTestStepPostambleL();

	// From MTestStepObserver
	void notifyResult(const TInt& aResult);	
	
private:
	 CActiveScheduler* iActSchd;
	};

_LIT(KRepeatGetAssistanceDataStep,"RepeatGetAssistanceDataStep");

#endif
