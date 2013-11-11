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


#ifndef __TCMD_STATEPOLICY_CREATESWP_H__
#define __TCMD_STATEPOLICY_CREATESWP_H__

#include <e32base.h>
#include <ssm/ssmstatepolicy.h>
#include <ssm/ssmstate.h>


const TUint16 KStatePolicyCreateSwp = 0x0077;
const TInt ESsmFailTest = 30;	// Redefine TSsmMainSystemStates ESsmFail to avoid using the real ESsmFail policy		

NONSHARABLE_CLASS(CSsmStatePolicyTestCreateSwp) : public CBase, public MSsmStatePolicy
	{
public:
	IMPORT_C static MSsmStatePolicy* NewL(); //lint !e1511
	
	//from CSsmStatePolicy
	void ConstructL();
	void Initialize(TRequestStatus& aStatus);
	void InitializeCancel();
	void Release();
	TResponse TransitionAllowed(const TSsmStateTransition& aRequest, TSsmStateTransition const* aCurrent, TSsmStateTransition const* aQueued, const RMessagePtr2& aMessage);
	void PrepareCommandList(TSsmState aState, TInt aReason, TRequestStatus& aStatus);
	void PrepareCommandListCancel();
	CSsmCommandList* CommandList();
	TBool GetNextState(TSsmState aCurrentTransition, TInt aReason, TInt aError, TInt aSeverity, TSsmState& aNextState);
	
private:
	//from CBase
	CSsmStatePolicyTestCreateSwp();
	~CSsmStatePolicyTestCreateSwp();

private:
	void doPrepareCommandListL(TSsmState aState, TInt aReason);
	void doPrepareCommandListL();

private:
	CSsmCommandList* iCommandList;
	};

#endif	// __TCMD_STATEPOLICY_CREATESWP_H__
