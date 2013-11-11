// Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
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
#ifndef __TCMD_STEP_AMASTARTER_H__
#define __TCMD_STEP_AMASTARTER_H__

#include "tcmd_step_base.h"
#include "ssmcommandfactory.h"

_LIT(KTCCmdTestAMAStarter, "CCmdTestAMAStarter");
class CCmdAmaStarter;

class CCmdTestAMAStarter : public CCmdTestBase
	{
public:
	CCmdTestAMAStarter();
	~CCmdTestAMAStarter();

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

	void CreateCmdAndExecuteL(TCmdErrorSeverity aSeverity, TSsmExecutionBehaviour aExecutionBehaviour, const TUid aDscId, TInt aExpectedErr, TBool aCancel = EFalse, TInt32 aTimeout = 0);
	CCmdAmaStarter* CreateCmdFromStreamL(TCmdErrorSeverity aSeverity, TSsmExecutionBehaviour aExecutionBehaviour, TUid aDscId, const TUint16 aPriority = KDefaultPriority);
	CSsmCommandBase* ConstructCommandFromStreamLC(TSsmCommandType aType, RReadStream& aReadStream, TArray<MSsmCommand*>& aDeferredList);
private:
	void CompareCommandsDataL(CCmdAmaStarter* aTestCmd, TCmdErrorSeverity aSeverity, TSsmExecutionBehaviour aExecutionBehaviour, TUid aDscId, const TUint16 aPriority = KDefaultPriority);
	

private:
	CCmdAmaStarter* iTestCmd;
	};

	
#endif	// __TCMD_STEP_AMASTARTER_H__
