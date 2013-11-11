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


#ifndef __TSSM_STATEPOLICY_CONDITIONALCOMMANDS_H__
#define __TSSM_STATEPOLICY_CONDITIONALCOMMANDS_H__

#include <e32base.h>
#include <f32file.h>
#include <ssm/ssmstatepolicy.h>
#include <ssm/ssmconditionalcallback.h>

// Forward declare
class CSsmCommandListResourceReader;

//Major state identifier for state policy dll
const TUint16 ESsmTestConditionalCommandsState = 0x0091;

/**
 * Test state policy with conditional command interpretation. 
 * 
 * @test
 * @internalComponent 
 */		
NONSHARABLE_CLASS(CSsmStatePolicyConditionalCommands) : public CBase, public MSsmStatePolicy, public MSsmConditionalCallback
	{
public:
	IMPORT_C static MSsmStatePolicy* NewL(); //lint !e1511
	
	//from MSsmStatePolicy
	void ConstructL();
	void Initialize(TRequestStatus& aStatus);
	void InitializeCancel();
	void Release();
	TResponse TransitionAllowed(const TSsmStateTransition& aRequest, TSsmStateTransition const* aCurrent, TSsmStateTransition const* aQueued, const RMessagePtr2& aMessage);
	void PrepareCommandList(TSsmState aState, TInt aReason, TRequestStatus& aStatus);
	void PrepareCommandListCancel();
	CSsmCommandList* CommandList();
	TBool GetNextState(TSsmState aCurrentTransition, TInt aReason, TInt aError, TInt aSeverity, TSsmState& aNextState);
	
	// from MSsmConditionalCallback
	virtual TBool ConditionalCommandAllowedL(CResourceFile& aResourceFile, TInt aResourceId);
	
private:
	//from CBase
	CSsmStatePolicyConditionalCommands();
	~CSsmStatePolicyConditionalCommands();
	
private:
	RFs iFs;
	CSsmCommandListResourceReader* iCommandListResourceReader;
	};

#endif
