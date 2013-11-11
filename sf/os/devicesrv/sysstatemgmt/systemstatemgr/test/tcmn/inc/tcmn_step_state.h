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
 

#ifndef __TCMN_STEP_STATE_H__
#define __TCMN_STEP_STATE_H__

#include <test/testexecutestepbase.h>

_LIT(KTCmnStateStep,"CmnStateStep");

class CCmnStateTest : public CTestStep
	{
public:
	CCmnStateTest();
	~CCmnStateTest();

	//from CTestStep
	TVerdict doTestStepPreambleL();
	TVerdict doTestStepL();
	TVerdict doTestStepPostambleL();
	
private:
	void doTestTSsmState();	
	void doTestTSsmStateConstructors();
	void doTestTSsmStateOperators();
	void doTestTSsmStateOtherMethods();
	// Panic testing
	void doTestTSsmStatePanic();
	void StartThread(TInt aOption);

private:	
	void doTestTSsmStateTransition();
	};

#endif
