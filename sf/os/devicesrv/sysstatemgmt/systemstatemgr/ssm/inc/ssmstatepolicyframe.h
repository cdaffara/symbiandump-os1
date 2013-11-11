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

#ifndef __SSMSTATEPOLICYFRAME_H__
#define __SSMSTATEPOLICYFRAME_H__

#include <e32base.h>
#include <ssm/ssmstatepolicy.h>

class CSsmCommandList;
class TSsmState;
class TSsmStateTransition;

/**
This class owns the MSsmStatePolicy instance created from the System State Policy DLL.
Its main purpose is to construct and delete the MSsmStatePolicy object, and to guarantee 
correctness from the CSsmStateTransitionEngine.
@internalComponent
@released
*/
class CSsmStatePolicyFrame : public CBase
  	{
public:
	static CSsmStatePolicyFrame* NewL(TLibraryFunction aNewLFunc);
	CSsmStatePolicyFrame(MSsmStatePolicy* aStatePolicy);
	~CSsmStatePolicyFrame();

	TUint16 StatePolicyId() const;
	void SetStatePolicyId(TUint16 aMainState);
	inline TBool Initialized() const { return iInitializeCalled; }
	
	void CallInitialize(TRequestStatus& aStatus);
	void CallInitializeCancel();
	MSsmStatePolicy::TResponse CallTransitionAllowed(const TSsmStateTransition& aRequest, TSsmStateTransition const* aCurrent, TSsmStateTransition const* aQueued, const RMessagePtr2& aMessage);
	void CallPrepareCommandList(TSsmState aTargetState, TInt aReason, TRequestStatus& aStatus);
	void CallPrepareCommandListCancel();
	CSsmCommandList* CallCommandList();
	TBool CallGetNextState(TSsmState aCurrentTransition, TInt aReason, TInt aError, TInt aSeverity, TSsmState& aNextState);

private:
	CSsmStatePolicyFrame();
	void ConstructL(TLibraryFunction aNewLFunc);
	
private:				
	enum TInternalActions
		{
		EInitialize,
		EPrepare,
		EGet,
		ENext
		};
	/** the policy that has been loaded */
	MSsmStatePolicy* iStatePolicy;
	/** used to track the internal state of the object for extra integrity */
	TInternalActions iNextAction;
	TUint16 iStatePolicyId;
	TBool iInitializeCalled;	
	};
	
#endif
