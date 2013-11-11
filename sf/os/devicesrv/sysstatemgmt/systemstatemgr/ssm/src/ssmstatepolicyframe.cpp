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

#include <ssm/ssmstate.h>
#include "ssmstatepolicyframe.h"

#include "ssmdebug.h"
#include "ssmserverpanic.h"


/**
*/
typedef MSsmStatePolicy* (*TFuncStateNewL)(void);

/**
*/
CSsmStatePolicyFrame::CSsmStatePolicyFrame()
	{
	}

/**
*/
 CSsmStatePolicyFrame::CSsmStatePolicyFrame(MSsmStatePolicy* aStatePolicy)
 	: iStatePolicy(aStatePolicy)
 	{
 	}

/**
*/
CSsmStatePolicyFrame* CSsmStatePolicyFrame::NewL(TLibraryFunction aNewLFunc)
	{
	CSsmStatePolicyFrame* self = new (ELeave) CSsmStatePolicyFrame();
	CleanupStack::PushL(self);
	self->ConstructL(aNewLFunc);
	CleanupStack::Pop(self);
	return self;
	}

/**
*/
void CSsmStatePolicyFrame::ConstructL(TLibraryFunction aNewLFunc)
	{
	TFuncStateNewL newL = reinterpret_cast<TFuncStateNewL>( aNewLFunc );
	iStatePolicy = newL();
#ifdef _DEBUG
	if(!iStatePolicy)
		{
		DEBUGPRINT1(_L("First function in State Policy DLL didn't create a new instance"));
		}
#endif
	SSMLOGLEAVEIFNULL(iStatePolicy);
	}

/**
*/
CSsmStatePolicyFrame::~CSsmStatePolicyFrame()
 	{
	//Let the derived implementation handle its own destruction
	if(iStatePolicy)
		{
		iStatePolicy->Release();
		iStatePolicy = NULL;
		}
 	}

/**
Returns the major state handled by this DLL.
The value returned is read from the DLL filename
*/
TUint16 CSsmStatePolicyFrame::StatePolicyId() const
	{
	return iStatePolicyId;
	}

/**
*/
void CSsmStatePolicyFrame::SetStatePolicyId(TUint16 aMainState)
	{
	iStatePolicyId = aMainState;
	}

/**
*/
void CSsmStatePolicyFrame::CallInitialize(TRequestStatus& aStatus)
 	{
	__ASSERT_DEBUG(EInitialize == iNextAction, PanicNow(KPanicSysStateMgr,ESsmStatePolicyError1));
	__ASSERT_DEBUG(!iInitializeCalled, PanicNow(KPanicSysStateMgr,ESsmStatePolicyError2));
	__ASSERT_DEBUG(iStatePolicy, PanicNow(KPanicSysStateMgr,ESsmStatePolicyError7));
	if(iStatePolicy)
		{
		iStatePolicy->Initialize(aStatus);
		iInitializeCalled = ETrue;
		iNextAction = EPrepare;
		}
	}

/**
 */
 void CSsmStatePolicyFrame::CallInitializeCancel()
 	{
 	__ASSERT_DEBUG(iStatePolicy, PanicNow(KPanicSysStateMgr,ESsmStatePolicyError8));
 	if(iStatePolicy)
 		{
 		iStatePolicy->InitializeCancel();
 		iNextAction = EInitialize;
 		}
 	}

/**
*/
MSsmStatePolicy::TResponse CSsmStatePolicyFrame::CallTransitionAllowed(const TSsmStateTransition& aRequest, TSsmStateTransition const* aCurrent, TSsmStateTransition const* aQueued, const RMessagePtr2& aMessage)
	{
	MSsmStatePolicy::TResponse response = MSsmStatePolicy::ENotAllowed;
	__ASSERT_DEBUG(iStatePolicy, PanicNow(KPanicSysStateMgr,ESsmStatePolicyError10));
	if(iStatePolicy)
		{
		response = iStatePolicy->TransitionAllowed(aRequest, aCurrent, aQueued, aMessage);
		}
	return response;
	}

/**
*/
void CSsmStatePolicyFrame::CallPrepareCommandList(TSsmState aState, TInt aReason, TRequestStatus& aStatus)
	{
	__ASSERT_DEBUG(EPrepare == iNextAction || ENext == iNextAction, PanicNow(KPanicSysStateMgr,ESsmStatePolicyError3));
	__ASSERT_DEBUG(aState.MainState() == iStatePolicyId, PanicNow(KPanicSysStateMgr,ESsmStatePolicyError4));
	__ASSERT_DEBUG(iStatePolicy, PanicNow(KPanicSysStateMgr,ESsmStatePolicyError11));
	if(iStatePolicy)
		{
		iStatePolicy->PrepareCommandList(aState, aReason, aStatus);
		iNextAction = EGet;
		}
	} //lint !e1746 Suppress parameter 'aState' could be made const reference

/**
 */
 void CSsmStatePolicyFrame::CallPrepareCommandListCancel()
 	{
 	__ASSERT_DEBUG(iStatePolicy, PanicNow(KPanicSysStateMgr,ESsmStatePolicyError12));
 	if(iStatePolicy)
 		{
 		iStatePolicy->PrepareCommandListCancel();
 		iNextAction = EPrepare;
 		}
 	}

/**
*/
CSsmCommandList* CSsmStatePolicyFrame::CallCommandList()
	{
	__ASSERT_DEBUG(EGet == iNextAction, PanicNow(KPanicSysStateMgr,ESsmStatePolicyError5));
	__ASSERT_DEBUG(iStatePolicy, PanicNow(KPanicSysStateMgr,ESsmStatePolicyError13));
	CSsmCommandList* list = NULL;
	if(iStatePolicy)
		{
		list = iStatePolicy->CommandList();
		iNextAction = ENext;
		}
	return list;
	}

/**
*/
TBool CSsmStatePolicyFrame::CallGetNextState(TSsmState aCurrentTransition, TInt aReason, TInt aError, TInt aSeverity, TSsmState& aNextState)
	{
	__ASSERT_DEBUG(ENext == iNextAction, PanicNow(KPanicSysStateMgr,ESsmStatePolicyError6));
	__ASSERT_DEBUG(iStatePolicy, PanicNow(KPanicSysStateMgr,ESsmStatePolicyError14));
	TBool moreToCome = EFalse;
	if(iStatePolicy)
		{
		moreToCome = iStatePolicy->GetNextState(aCurrentTransition, aReason, aError, aSeverity, aNextState);
		iNextAction = EPrepare;
		}
	return moreToCome;
	} //lint !e1746 Suppress parameter 'aCurrentTransition' could be made const reference

