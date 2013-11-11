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

#ifndef __TCMD_STEP_LOADSUP_H__
#define __TCMD_STEP_LOADSUP_H__

#include "tcmd_step_base.h"
#include "ssmcommandfactory.h"

_LIT(KTCCmdTestLoadSup, "CCmdTestLoadSup");

class CCmdTestLoadSup : public CCmdTestBase
	{
public:
	CCmdTestLoadSup();
	~CCmdTestLoadSup();

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

	// from MSsmCommandFactory
	CSsmCommandBase* ConstructCommandFromResourceLC(TSsmCommandType aType, TSsmCommandParameters& aCommandParameters);
	CSsmCommandBase* ConstructCommandFromStreamLC(TSsmCommandType aType, RReadStream& aReadStream, TArray<MSsmCommand*>& aDeferredList);
	
private:
	void ConstructAndDeleteViaReaderL(TSsmCommandParameters& aParams);
	void ConstructAndDeleteL(TCmdErrorSeverity severity, TSsmExecutionBehaviour executionBehaviour, TInt16 aRetries, const TSsmSupInfo& aInfo, const TInt aVersion = ECmdLoadSupInitialVersion, const TUint16 aPriority = KDefaultPriority);
	void CompareInputAndOutputL(TCmdErrorSeverity aSeverity, TSsmExecutionBehaviour aExecutionBehaviour, TInt16 aRetries, const TSsmSupInfo& aInfo, CBufFlat* aWriteBuf, const TUint16 aPriority = KDefaultPriority);
	void CallInternalizeDataTestL(TCmdErrorSeverity aSeverity, TSsmExecutionBehaviour executionBehaviour, TInt16 aRetries, const TSsmSupInfo& aInfo, const TUint16 aPriority = KDefaultPriority);
	void LoadSupAndExecuteL(TCmdErrorSeverity aSeverity, TSsmSupInfo& aSwpInfo, TSsmExecutionBehaviour aExecutionBehaviour, 
													TInt16 aRetries, TInt aExpectedErr, TBool aCancel, TInt32 aTimeout);
	};

#endif	// __TCMD_STEP_LOADSUP_H__
