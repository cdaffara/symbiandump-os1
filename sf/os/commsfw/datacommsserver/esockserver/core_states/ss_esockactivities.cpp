// Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// ESock Sub Session Activities
// THIS API IS INTERNAL TO NETWORKING AND IS SUBJECT TO CHANGE AND NOT FOR EXTERNAL USE
//
//

/**
 @file
 @internalComponent
*/

#include <comms-infras/ss_log.h>
#include <comms-infras/ss_nodemessages.h>
#include <ss_std.h>
#include <comms-infras/ss_esockstates.h>
#include "ss_esockactivities.h"


#ifdef _DEBUG
// Panic category for "absolutely impossible!" vanilla ASSERT()-type panics from this module
// (if it could happen through user error then you should give it an explicit, documented, category + code)
_LIT(KSpecAssert_ESockCrStaESclAC, "ESockCrStaESclAC");
#endif

#if defined LOG || defined ESOCK_EXTLOG_ACTIVE
	#define KESockActivityTag KESockCoreProviderTag
	_LIT8(KESockActivitySubTag, "esockactivity");
#endif

using namespace NetStateMachine;
using namespace SubSessStates;
using namespace SubSessActivities;
using namespace MeshMachine;
using namespace ESock;

//
//CESockClientActivityBase
MeshMachine::CNodeActivityBase* CESockClientActivityBase::NewL(const MeshMachine::TNodeActivity& aActivitySig, MeshMachine::AMMNodeBase& aNode)
	{
	return new (ELeave) CESockClientActivityBase(aActivitySig,aNode);
	}

CESockClientActivityBase::~CESockClientActivityBase()
	{
	if (!iMessage.IsNull())
		{ //Can be NULL only if the client has been panicked and we do not want to complete it
		__ASSERT_DEBUG(iSubSession, User::Panic(KSpecAssert_ESockCrStaESclAC, 1));

		LOG(ESockLogExternal::Printf(KESockActivityTag, KESockActivitySubTag, _L8("~CESockClientActivityBase %08x:\tRMessage2::Complete (%08x) with %d."), this, iMessage.Handle(), Error()));
		iMessage.Complete(Error());
		}
	SetError(KErrNone);
	}

void CESockClientActivityBase::StartL(TNodeContextBase& aContext, const Messages::XNodePeerId& aOriginator, const TStateTriple& aFirst)
	{
	//We should not be starting when the close activity is running!
	__ASSERT_DEBUG(ActivityId()==ECFActivityDestroy || aContext.iNode.CountActivities(ECFActivityDestroy)==0, User::Panic(KSpecAssert_ESockCrStaESclAC, 2));

	//Very important for client activities is to allow only one outstanding request at a time.
	//This is achieved by allowing only the first originator to start such activity
	if (iOriginators.Count())
		{
		MESH_LOG_CONTEXT_EXT(KESockMeshMachine, aContext, _L8("ERROR: CESockClientActivityBase::StartL->One outstanding request allowed"));
		User::Leave(KErrInUse);
		}

	//Either we are the close activity or there can not be close running when this activity is started
	__ASSERT_DEBUG(ActivitySigId()==ECFActivityDestroy || aContext.iNode.CountActivities(ECFActivityDestroy)==0, User::Panic(KSpecAssert_ESockCrStaESclAC, 3));

  	iOriginators.AppendL(aOriginator);

	MESH_LOG_ACTIVITY_EXT(KESockMeshMachine, this, &aContext, (_L8("CESockClientActivityBase %08x:\tStartL->starting activity"), this));
	__ASSERT_DEBUG(IsIdle(), User::Panic(KSpecAssert_ESockCrStaESclAC, 4));
	NetStateMachine::ACore::Start(&aContext, aFirst);

	MESH_LOG_ACTIVITY_EXT(KESockMeshMachine, this, &aContext, (_L8("CESockClientActivityBase %08x:\tStartL->activity started"), this));
	}

void CESockClientActivityBase::AcquireOwnership(RMessage2& aMessage, CSockSubSession& aSubSession)
	{
	__ASSERT_DEBUG(iMessage.IsNull(), User::Panic(KSpecAssert_ESockCrStaESclAC, 5));
	__ASSERT_DEBUG(!iSubSession, User::Panic(KSpecAssert_ESockCrStaESclAC, 6));
	iMessage = aMessage;
	aMessage = RMessage2(); //Null the original message
	iSubSession = &aSubSession;
	LOG(ESockLogExternal::Printf(KESockActivityTag, KESockActivitySubTag, _L8("CESockClientActivityBase %08x:\tRMessage2 ownership taken! RMessage2(%08x), SubSession(%08x), ActivityId(%d)"), this, iMessage.Handle(), &aSubSession, ActivityId()));
	aSubSession.DontCompleteCurrentRequest();
	}

void CESockClientActivityBase::PanicClient(TESockPanic aPanic)
	{
	__ASSERT_DEBUG(!iMessage.IsNull(), User::Panic(KSpecAssert_ESockCrStaESclAC, 7));
	__ASSERT_DEBUG(iSubSession, User::Panic(KSpecAssert_ESockCrStaESclAC, 8));
	RThread t;
	iMessage.Client(t);
	TExitType exittype=t.ExitType();
	t.Close();
	if(exittype == EExitPending) // is client alive?
		{
		LOG(ESockLogExternal::Printf(KESockActivityTag, KESockActivitySubTag, _L8("CESockClientActivityBase::PanicClient\tRMessage2(%08x), Reason=%d"), iMessage.Handle(), aPanic));
		iMessage.Panic(KESockClientPanic, aPanic);
		}
	iMessage = RMessage2(); //Clear the message so that it does not get completed
	iSubSession = NULL;
	}

void CESockClientActivityBase::ReadL(TInt aSrcParamIndex, TDes8& aDes)
	{
	__ASSERT_DEBUG(!iMessage.IsNull(), User::Panic(KSpecAssert_ESockCrStaESclAC, 9)); //Read only after it has been initialised!
	TInt err = iMessage.Read(aSrcParamIndex, aDes);
	if (err!=KErrNone)
		{
		PanicClient(ESockBadHandle);
		User::Leave(err);
		}
	}

TInt CESockClientActivityBase::GetDesLengthL(TInt aSrcParamIndex)
	{
 	__ASSERT_DEBUG(!iMessage.IsNull(), User::Panic(KSpecAssert_ESockCrStaESclAC, 10));
 	return iMessage.GetDesLengthL(aSrcParamIndex);
	}

TInt CESockClientActivityBase::GetDesMaxLengthL(TInt aSrcParamIndex)
	{
 	__ASSERT_DEBUG(!iMessage.IsNull(), User::Panic(KSpecAssert_ESockCrStaESclAC, 11));
 	return iMessage.GetDesMaxLengthL(aSrcParamIndex);
	}

void CESockClientActivityBase::WriteL(TInt aSrcParamIndex, TDes8& aDes)
	{
	iMessage.WriteL(aSrcParamIndex,aDes);
	}

TInt CESockClientActivityBase::Function()
	{
	return iMessage.Function();
	}

void CESockClientActivityBase::HasCapabilityL(TCapability aCapability, const char* aDiagnosticMessage)
    {
    return iMessage.HasCapabilityL(aCapability, aDiagnosticMessage);
    }




//
//CPreallocatedESockClientActivity
MeshMachine::CNodeActivityBase* CPreallocatedESockClientActivity::New(const MeshMachine::TNodeActivity& aActivitySig, MeshMachine::AMMNodeBase& aNode)
	{
	TAny* space = BorrowPreallocatedSpace(aNode, sizeof(CPreallocatedESockClientActivity));
	CPreallocatedESockClientActivity* self = new (space) CPreallocatedESockClientActivity(aActivitySig, aNode);
	self->InsertPreallocatedDestroyActivity();
	return self;
	}

CPreallocatedESockClientActivity::CPreallocatedESockClientActivity(const MeshMachine::TNodeActivity& aActivitySig, MeshMachine::AMMNodeBase& aNode)
:	CESockClientActivityBase(aActivitySig,aNode),
	APreallocatedOriginators<1>(iOriginators)
	{
	}

void CPreallocatedESockClientActivity::Destroy()
	{
	ReturnPreallocatedSpace(this);
	this->~CPreallocatedESockClientActivity(); //Run the destructor
	}




//
//CCloseActivity
MeshMachine::CNodeActivityBase* CCloseActivity::New(const MeshMachine::TNodeActivity& aActivitySig, MeshMachine::AMMNodeBase& aNode)
	{
	TAny* space = BorrowPreallocatedSpace(aNode, sizeof(CCloseActivity));
	CCloseActivity* self = new (space) CCloseActivity(aActivitySig, aNode);
	self->InsertPreallocatedDestroyActivity();
	return self;
	}

CCloseActivity::CCloseActivity(const MeshMachine::TNodeActivity& aActivitySig, MeshMachine::AMMNodeBase& aNode)
:	CESockClientActivityBase(aActivitySig,aNode),
	APreallocatedOriginators<1>(iOriginators)
	{
	}

void CCloseActivity::Destroy()
	{
	ReturnPreallocatedSpace(this);
	this->~CCloseActivity(); //Run the destructor

	ESock::CMMSockSubSession* subSessNode = static_cast<ESock::CMMSockSubSession*>(&iNode);
	delete subSessNode; //We are taking down the whole node. All relevant diagnostics and logging is done from MeshMachine::AMMNodeBase::~MeshMachine::AMMNodeBase().
	}

