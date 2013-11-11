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
 
#ifndef __TGSA_STEP_DUPLICATESUBSTATE_H__
#define __TGSA_STEP_DUPLICATESUBSTATE_H__

#include <test/testexecutestepbase.h>
#include <s32file.h>

_LIT(KTGsaDuplicateSubstateStep,"GsaDuplicateSubstateStep");
_LIT(KTGsaPanicResultFile,  "c:\\t_gsapanicresultfile.txt");

class CGsaDuplicateSubstateTest : public CTestStep
	{
public:
	CGsaDuplicateSubstateTest();
	~CGsaDuplicateSubstateTest();

	//from CTestStep
	TVerdict doTestStepPreambleL();
	TVerdict doTestStepL();
	TVerdict doTestStepPostambleL();
	
private:
	void DoTestDuplicateSubstateL();
	void TestEpocWindResultsL(const TDesC& aFileName, const TInt& aExpectedResult);
	};

#endif	// __TGSA_STEP_DUPLICATESUBSTATE_H__


