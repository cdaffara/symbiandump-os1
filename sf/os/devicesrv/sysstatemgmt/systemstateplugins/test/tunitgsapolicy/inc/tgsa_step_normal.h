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

#ifndef __TGSA_STEP_NORMAL_H__
#define __TGSA_STEP_NORMAL_H__

#include <test/testexecutestepbase.h>
#include "tgsa_step_base.h"

class CGsaStatePolicyNormal;
class TSsmState;

_LIT(KTGsaNormalStep,"GsaNormalStep");

class CGsaNormalTest : public CGsaTestBase
	{
public:
	CGsaNormalTest();
	~CGsaNormalTest();

	//from CTestStep
	TVerdict doTestStepL();

private:
	void doTestNewL();
	void doTestInitializeL();
	void doTestPrepareCommandListL();
	void doTestCommandListL();
	void doTestGetNextStateL();
	void doTestTransitionSupportedL();

	CGsaStatePolicyNormal* CreateAndInitializeNormalPolicyLC();
	void TestPrepareCommandListL(TUint16 aMainState, TUint16 aSubState, TInt aExpectedResult);
	void TestCommandListL(TUint16 aMainState, TUint16 aSubState, TInt aNumSubStates);
	void TestGetNextState(CGsaStatePolicyNormal* aPolicy, TSsmState aCurrentTransition, TInt aError, TSsmState aResult);
	};


#endif
