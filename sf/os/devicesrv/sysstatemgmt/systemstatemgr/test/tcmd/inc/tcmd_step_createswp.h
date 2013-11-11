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
 

#ifndef __TCMD_STEP_CREATESWP_H__
#define __TCMD_STEP_CREATESWP_H__

#include "tcmd_step_base.h"

_LIT(KTCCmdTestCreateSwp, "CCmdTestCreateSwp");

class CCmdTestCreateSwp : public CCmdTestBase
	{
public:
	CCmdTestCreateSwp();
	~CCmdTestCreateSwp();

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

private:
	void ConstructAndDeleteViaReaderL(TSsmCommandParameters& aParams);
	void ConstructAndDeleteL(TCmdErrorSeverity aSeverity, TSsmSwp& aSwpInfo, TDesC& aFilename, const TInt aVersion = ECmdCreateSwpInitialVersion , const TUint16 aPriority = KDefaultPriority);
	void CompareInputAndOutputL(TCmdErrorSeverity aSeverity, TSsmSwp& aSwpInfo, const TDesC& aFilename, CBufFlat* aWriteBuf, const TUint16 aPriority = KDefaultPriority);
	void StartStateManager();
	void CallInternalizeDataTestL(TCmdErrorSeverity aSeverity, TSsmSwp& aSwpInfo, const TDesC& aFilename, const TUint16 aPriority = KDefaultPriority);
	void CreateSwpAndExecuteL(TCmdErrorSeverity aSeverity, TSsmSwp& aSwpInfo, TDesC& aFilename, TInt aExpectedErr, TBool aCancel);
	};

#endif	// __TCMD_STEP_CREATESWP_H__
