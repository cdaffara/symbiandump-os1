/**
* Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
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
 @file EmptyAssistanceDataRequestStep.h
*/
#if (!defined __EmptyAssistanceDataRequest_STEP_H__)
#define __EmptyAssistanceDataRequest_STEP_H__
#include <test/testexecutestepbase.h>
#include "Te_lbssupltestmoduleSuiteStepBase.h"


class CEmptyAssistanceDataRequestStep : public CTe_lbssupltestmoduleSuiteStepBase, public MTestStepObserver
	{
public:
	CEmptyAssistanceDataRequestStep(CTe_lbssupltestmoduleSuite* aServer);
	~CEmptyAssistanceDataRequestStep();
	virtual TVerdict doTestStepPreambleL();
	virtual TVerdict doTestStepL();
	virtual TVerdict doTestStepPostambleL();

	// From MEmptyAssistanceDataRequestObserver
	void notifyResult(const TInt& aResult);	
	
private:
	 CActiveScheduler* iActSchd;
	};


_LIT(KEmptyAssistanceDataRequestStep,"EmptyAssistanceDataRequestStep");

#endif
