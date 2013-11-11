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

#ifndef __GSASTATEPOLICYNORMAL_H__
#define __GSASTATEPOLICYNORMAL_H__

#include <e32base.h>
#include <ssm/ssmstatepolicy.h>
#include <ssm/ssmconditionalcallback.h>
#include <ssm/conditionevaluate.h>

class CSsmCommandListResourceReader;

/**
Normal state policy plug-in for GSA. This is a polymorphic dll.
Implements MSsmStatePolicy to initialize, prepare and return command list using CSsmCommandListResourceReader.
Implements MSsmConditionalCallback to decide whether to include a command in a command list or not.

@internalComponent
@released
@see MSsmStatePolicy
@see MSsmConditionalCallback
*/
NONSHARABLE_CLASS(CGsaStatePolicyNormal) : public CBase, public MSsmStatePolicy, public MSsmConditionalCallback
	{
public:
	IMPORT_C static MSsmStatePolicy* NewL();

	//from MSsmStatePolicy
	TResponse TransitionAllowed(const TSsmStateTransition& aRequest, TSsmStateTransition const* aCurrent, TSsmStateTransition const* aQueued, 
								const RMessagePtr2& aMessage);
	void Initialize(TRequestStatus& aStatus);
	void InitializeCancel();
	void PrepareCommandList(TSsmState aState, TInt aReason, TRequestStatus& aStatus);
	void PrepareCommandListCancel();
	CSsmCommandList* CommandList();
	TBool GetNextState(TSsmState aCurrentTransition, TInt aReason, TInt aError, TInt aSeverity, TSsmState& aNextState);
	void Release();

	// from MSsmConditionalCallback
	TBool ConditionalCommandAllowedL(CResourceFile& aResourceFile, TInt aResourceId);
	
private:
	~CGsaStatePolicyNormal();
	CGsaStatePolicyNormal();

	void ConstructL();
	TBool TransitionSupported(const TSsmState& aRequestedState) const;

private:
	RArray<TSsmState> iCurrentlySupportedTransitions;	
	CSsmCommandListResourceReader* iCommandListResourceReader;
	RFs iFs;
	//CConditionEvaluate is created and used only when patchable constant KSsmGracefulOffline is enabled
	CConditionEvaluate *iEvaluator;

	// Possible panic codes within Normal policy
	enum TNormalPanicCodes
		{
		/**
		 Panic when command list resource reader is NULL
		*/
		EInvalidResourceReader = 1,
		/**
		 Panic when Main state supplied is not 'ESsmNormal'
		*/
		EInvalidNormalState,
		/**
		 Panic when ConditionalCommandAllowedL is called for a command which hasn't implemented conditional_information
		*/
		EConditionalInfoNotImplemented
		};
	};

#endif	// __GSASTATEPOLICYNORMAL_H__
