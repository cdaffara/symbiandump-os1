/**
* Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
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
 @file checkbtpsyfromserverstep.h
 @internalTechnology
*/
#ifndef __CHECKBTPSYFROMSERVER_STEP_H__
#define __CHECKBTPSYFROMSERVER_STEP_H__
#include <test/TestExecuteStepBase.h>
#include <lbs.h>
#include <lbsCommon.h>
#include <EPos_CPosModules.h>
#include <EPos_CPosModuleUpdate.h>
#include <EPos_CPosModuleIdList.h> 
#include "te_lbssisinstallstepbase.h"
#include "te_lbssisinstalldefs.h"

const TPositionModuleId btGPSPsyModuleId = { 0x101FE999 };
const TInt btGPSPsyIdInt = 0x101FE999;

const TInt KMaxLogBuffer=200;

_LIT(KTe_BTExpected,"BTExpected");

class CCheckBTPSYFromServerStep : public CTe_LbsSisInstallStepBase
	{
public:
	CCheckBTPSYFromServerStep();
	~CCheckBTPSYFromServerStep();
	virtual TVerdict doTestStepPreambleL();
	virtual TVerdict doTestStepL();
	virtual TVerdict doTestStepPostambleL();

private:
	void CheckBTPSYFromServerL();
	void CCheckBTPSYFromServerStep::LocationRequestTestL();
	void CCheckBTPSYFromServerStep::AssertTrueSecL(TBool aCondition, const TDesC& aErrorMsg, TInt aErrorCode);
	void CCheckBTPSYFromServerStep::CheckBTPSYModuleInfoL( TPositionModuleInfo aInfo );
	
	CActiveScheduler*   iScheduler;
	TBool				iExpectToFindBTPsy;
	};

_LIT(KCheckBTPSYFromServerStep,"CheckBTPSYFromServerStep");

#endif //__CHECKBTPSYFROMSERVER_STEP_H__
