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
 

#ifndef __TCMD_STEP_REQSWPCHANGE_H__
#define __TCMD_STEP_REQSWPCHANGE_H__

#include "tcmd_step_base.h"
#include "ssmcommandfactory.h"

_LIT(KTCCmdTestReqSwpChange, "CCmdTestReqSwpChange");

class CCmdTestReqSwpChange : public CCmdTestBase
	{
public:
	CCmdTestReqSwpChange();
	~CCmdTestReqSwpChange();

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
	void ConstructAndDeleteL(TCmdErrorSeverity aSeverity, TSsmExecutionBehaviour aExecutionBehaviour, TSsmSwp& aSwpInfo, const TInt aVersion = ECmdReqSwPropertyInitialVersion, const TUint16 aPriority = KDefaultPriority);
	void CompareInputAndOutputL(TCmdErrorSeverity aSeverity, TSsmExecutionBehaviour aExecutionBehaviour, TSsmSwp& aSwpInfo, CBufFlat* aWriteBuf, const TUint16 aPriority = KDefaultPriority);
	void CallInternalizeDataTestL(TCmdErrorSeverity aSeverity, TSsmExecutionBehaviour aExecutionBehaviour, TSsmSwp& aSwpInfo, const TUint16 aPriority = KDefaultPriority);
	void ReqSwpChangeAndExecuteL(TCmdErrorSeverity aSeverity, TSsmSwp& aSwpInfo, TSsmExecutionBehaviour aExecutionBehaviour, TInt aExpectedErr, TBool aCancel, TInt32 aTimeout = 0);
	};

#endif	// __TCMD_STEP_REQSWPCHANGE_H__
