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
//

/**
 @file
 @test
 @internalComponent - Internal Symbian test code  
*/

#ifndef __TCMD_STEP_PUBLISHSWP_H__
#define __TCMD_STEP_PUBLISHSWP_H__

#include "tcmd_step_base.h"
#include "ssmcommandfactory.h"

_LIT(KTCCmdTestPublishSwp, "CCmdTestPublishSwp");
class CCmdPublishSwp;

class CCmdTestPublishSwp : public CCmdTestBase
	{
public:
	CCmdTestPublishSwp();
	~CCmdTestPublishSwp();

	//from CTestStep
	TVerdict doTestStepL();
	
private:
	//from CCmdTestBase
	void doTestNewLWithGoodDataL();
	void doTestNewLWithBadDataL();
	void doTestInternalizeLWithGoodDataL();
	void doTestInternalizeLWithBadDataL();
	void doTestResourceReaderWithGoodDataL();
	void doTestResourceReaderWithMissingDataL();
	void doTestResourceReaderWithBadDataL();
	void doTestCommandTypeL();
	void doTestCmdFailureIgnoreSeverityL();
	void doTestCmdFailureHighSeverityL();
	void doTestCancelCmdIgnoreSeverityL();
	void doTestCancelCmdHighSeverityL();
	void doTestNormalExecutionL();
	
	CCmdPublishSwp* CreateCmdFromStreamL(TCmdErrorSeverity aSeverity, TSsmExecutionBehaviour aExecutionBehaviour, TSsmSwp& aSwp, const TUint16 aPriority = KDefaultPriority);
	void CreateCmdAndExecuteL(TCmdErrorSeverity aSeverity, TSsmExecutionBehaviour aExecutionBehaviour, 
											 TSsmSwp& aSwp, TInt aExpectedErr, TBool aCancel = EFalse, TInt32 aTimeout = 0);

	void CompareCommandsDataL(CCmdPublishSwp* aTestCmd, TCmdErrorSeverity aSeverity, TSsmExecutionBehaviour aExecutionBehaviour, TSsmSwp& aSwp, const TUint16 aPriority = KDefaultPriority);
	TInt CreateTestSwpKeyL();
private:
	CCmdPublishSwp* iTestCmd;
};

#endif	// __TCMD_STEP_PUBLISHSWP_H__
