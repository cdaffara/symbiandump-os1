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


#ifndef __TCMD_STEP_PUBLISHSYSTEMSTATE_H__
#define __TCMD_STEP_PUBLISHSYSTEMSTATE_H__

#include <ssm/ssmstates.hrh>
#include "tcmd_step_base.h"

_LIT(KTCCmdTestPublishSystemState, "CCmdTestPublishSystemState");

class CCmdTestPublishSystemState : public CCmdTestBase
	{
public:
	CCmdTestPublishSystemState();
	~CCmdTestPublishSystemState();

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
	void doTestSupportingClassesL();
	
	// from MSsmCommandFactory
	CSsmCommandBase* ConstructCommandFromStreamLC(TSsmCommandType aType, RReadStream& aReadStream, TArray<MSsmCommand*>& aDeferredList);

private:
	void ConstructAndDeleteViaReaderL(TSsmCommandParameters& aParams);
	void ConstructAndDeleteL(TCmdErrorSeverity severity, TSsmExecutionBehaviour aExecutionBehaviour, TSsmPublishSystemStateInfo& aInfo, const TInt aVersion = ECmdPublishSystemStateInitialVersion, const TUint16 aPriority = KDefaultPriority);
	void CompareInputAndOutputL(TCmdErrorSeverity aSeverity, TSsmExecutionBehaviour aExecutionBehaviour, TSsmPublishSystemStateInfo& aInfo, CBufFlat* aWriteBuf, const TUint16 aPriority = KDefaultPriority);
	void CallInternalizeDataTestL(TCmdErrorSeverity aSeverity, TSsmExecutionBehaviour aExecutionBehaviour, TSsmPublishSystemStateInfo& aInfo, const TUint16 aPriority = KDefaultPriority);
	void PublishSystemStateL(TCmdErrorSeverity aSeverity, TSsmExecutionBehaviour aExecutionBehaviour, 
													TSsmPublishSystemStateInfo aPubSysStateInfo, TInt aExpectedCmdStatus, TBool aCancel, TInt32 aTimeout = 0);
private:
	TSsmMainSystemStates iMainstate;
	};

#endif	// __TCMD_STEP_PUBLISHSYSTEMSTATE_H__
