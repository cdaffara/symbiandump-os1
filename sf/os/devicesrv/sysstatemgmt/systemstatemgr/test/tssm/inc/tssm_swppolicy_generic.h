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


#ifndef __TSSM_SWPPOLICY_GENERIC_H__
#define __TSSM_SWPPOLICY_GENERIC_H__

#include <e32base.h>
#include <f32file.h>

#include <ssm/ssmswppolicy.h>
#include <ssm/ssmconditionalcallback.h>

// Forward declare
class CSsmCommandListResourceReader;

_LIT(KTestSwpPolicyGenericFile, "ssm.swp.policy.test.generic.dll");

//Use this known panic reasons / category if the test fails (as TEST macro is not available here)
enum TSwpGenericTestPanicReason
	{
	ESwpInvalidResourceReader = 5050,
	ESwpConditionalCommand,
	};
_LIT(KSwpGenericTestPanicCategory,"SwpGenericTest");


NONSHARABLE_CLASS(CSsmSwpPolicyGeneric) : public CBase, public MSsmSwpPolicy, public MSsmConditionalCallback
	{
public:
	IMPORT_C static MSsmSwpPolicy* NewL(); //lint !e1511
	
	//from MSsmSwpPolicy
	void ConstructL();
	void Initialize(TRequestStatus& aStatus);
	void InitializeCancel();
	void Release();
	TResponse TransitionAllowed(const TSsmSwp& aSwp, const RMessagePtr2& aMessage);
	void PrepareCommandList(const TSsmSwp& aSwp, TRequestStatus& aStatus);
	void PrepareCommandListCancel();
	CSsmCommandList* CommandList();
	void HandleCleReturnValue(const TSsmSwp& aSwp, TInt aError, TInt aSeverity, TRequestStatus& aStatus);
	void HandleCleReturnValueCancel();
	
	//from MSsmConditionalCallback
	TBool ConditionalCommandAllowedL(CResourceFile& aResourceFile, TInt aResourceId);
	
private:
	//from CBase
	CSsmSwpPolicyGeneric();
	~CSsmSwpPolicyGeneric();

private:
	RFs iFs;
	CSsmCommandListResourceReader* iCommandListResourceReader;
	};

#endif
