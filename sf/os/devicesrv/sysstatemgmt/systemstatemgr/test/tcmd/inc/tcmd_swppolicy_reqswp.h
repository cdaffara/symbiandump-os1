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


#ifndef __TCMD_SWPPOLICY_REQSWP_H__
#define __TCMD_SWPPOLICY_REQSWP_H__

#include <ssm/ssmswppolicy.h>

static const TUint KSwpPolicyReqSwpTest = 888;
_LIT(KSwpPolicyReqSwpTestFile,"ssm.swp.policy.test.cmdreqswp");

NONSHARABLE_CLASS(CSsmSwpPolicyTestReqSwp) : public CBase, public MSsmSwpPolicy
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
	
private:
	//from CBase
	CSsmSwpPolicyTestReqSwp();
	~CSsmSwpPolicyTestReqSwp();
	void doPrepareCommandListL(const TSsmSwp& aSwp);

private:
	CSsmCommandList* iCommandList;
	};

#endif	// __TCMD_SWPPOLICY_REQSWP_H__
