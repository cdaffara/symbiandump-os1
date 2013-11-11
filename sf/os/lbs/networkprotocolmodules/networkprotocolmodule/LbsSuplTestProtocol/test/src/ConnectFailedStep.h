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
 @file ConnectFailedStep.h
*/
#if (!defined __CONNECTFAILED_STEP_H__)
#define __CONNECTFAILED_STEP_H__
#include <test/TestExecuteStepBase.h>
#include "Te_lbssupltestmoduleSuiteStepBase.h"

class CConnectFailedStep : public CTe_lbssupltestmoduleSuiteStepBase, public MLbsAssistanceDataSourceObserver
	{
public:
	CConnectFailedStep(CTe_lbssupltestmoduleSuite* aServer);
	~CConnectFailedStep();
	virtual TVerdict doTestStepPreambleL();
	virtual TVerdict doTestStepL();
	virtual TVerdict doTestStepPostambleL();

	// From MLbsAssistanceDataSourceObserver
	void DataSourceNotification(const TInt& aError);
	
private:
	 CActiveScheduler* iActSchd;
	};

_LIT(KConnectFailedStep,"ConnectFailedStep");

#endif
