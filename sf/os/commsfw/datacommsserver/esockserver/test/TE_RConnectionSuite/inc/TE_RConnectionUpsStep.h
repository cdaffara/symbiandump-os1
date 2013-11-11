
// Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
// All rights reserved.
// This component and the accompanying materials are made available
// under the terms of "Eclipse Public License v1.0"
// which accompanies this distribution, and is available
// at the URL "http://www.eclipse.org/legal/epl-v10.html".
//
// Initial Contributors:
// Nokia Corporation - initial contribution.
//
// Contributors:
//
// Description:
// This contains TE_RConnectionUpsStep which is the base class for all the 
// TE_RConnectionUpsTestSteps test step classes
// 
//

/**
 @file 
*/
 
#if (!defined __TE_RCONNECTIONUPSSTEP_H__)
#define __TE_RCONNECTIONUPSSTEP_H__

#include "TE_RConnectionStep.h"
#include <ups/policy.h>
#include <ups/upsnotifierutil.h>

const TUint KOneSecond = 1000000;
const TUint KOneMilliSecond = 1000;

_LIT(KUpsTestIniFile, "c:\\ipupstest.ini");
_LIT(KStartDelayKey, "StartDelay");
_LIT(KDeleteUpsDbKey, "DeleteUpsDb");
_LIT(KNotifierResponseKey, "NotifierResponse");
_LIT(KCheckResultKey, "Result");
_LIT(KIncrNotifyCountKey, "IncreaseNotifyCount");


using namespace UserPromptService;


class TE_RConnectionUpsStep : public TE_RConnectionStep
{
public:
	virtual ~TE_RConnectionUpsStep();

protected:
	virtual enum TVerdict doTestStepPreambleL();	

	//Functions which depend on information from test configuration details
	void DoTestStartDelay();
	TInt CheckAndDeleteUpsDB();
	TInt CheckResult(const TInt aResult);
	TInt SetNotifierResponse();
	TInt CheckNotifyCount(const TInt aCountBefore, const TInt aCountAfter);
	
	//Other utility Functions
	TInt DeleteUpsDB();
	TInt SetNotifierDelay(const TInt aDelay);
	TInt SetNotifierResponse(const CPolicy::TOptions aButtonPress);	
	TInt GetNotifyCount();
	TInt GetStoredNotifyCount();
	TInt SetStoredNotifyCount(const TInt aCount);
	CPromptData* GetNotifyDataL();
	void GetAndPrintNotifyData(const TInt aNotifyCount, const TBool aIsNewNotifyData = ETrue);
	
	//Load test config file data
	TInt GetConfigData();
	
	
private:
	//Members for storing items from the config file 
	TInt iStartDelay;
	TBool iDeleteUpsDB;
	TInt iNotifierResponseCount;
	RArray<CPolicy::TOptions> iNotifierResponse;
	TInt iResultCount;
	RArray<TInt> iResult;
	TInt iNotifyCheckCount;
	RArray<TInt> iIncrNotifyCount;


};

#endif /* __TE_RCONNECTIONUPSSTEP_H__ */


