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
// 
//

/**
 @file
 @internalComponent
*/

#ifndef SYMBIAN_SS_ESOCKACTIVITIES_H
#define SYMBIAN_SS_ESOCKACTIVITIES_H

#include <elements/mm_states.h>
#include <elements/mm_activities.h>
#include <comms-infras/ss_mmnode.h>


#ifdef _DEBUG
// Panic category for "absolutely impossible!" vanilla ASSERT()-type panics from this module
// (if it could happen through user error then you should give it an explicit, documented, category + code)
_LIT(KSpecAssert_ESockCrStaESclAH, "ESockCrStaESclAH");
#endif

namespace ESock
{
	class CMMSockSubSession;
}

namespace SessActivities
{
}

namespace SubSessActivities
{

//
//CESockClientActivityBase - client (IPC) originated activity base
class CESockClientActivityBase : public MeshMachine::CNodeRetryActivity
  //@internalTechnology
	{
public:
    static MeshMachine::CNodeActivityBase* NewL(const MeshMachine::TNodeActivity& aActivitySig, MeshMachine::AMMNodeBase& aNode);

public:
	//Remove second parameter after all CSock(Sub)Session calls completing/reading messages have been removed (all sub sessions MM based)
	void AcquireOwnership(RMessage2& aMessage, CSockSubSession& aSubSession);
	void PanicClient(TESockPanic aPanic);
	void ReadL(TInt aSrcParamIndex, TDes8& aDes);
	void WriteL(TInt aSrcParamIndex, TDes8& aDes);
	TInt Function();
	TInt Int0() { return iMessage.Int0(); }
	TInt Int1() { return iMessage.Int1(); }
	void HasCapabilityL(TCapability aCapability, const char* aDiagnosticMessage=0);
	TInt GetDesLengthL(TInt aSrcParamIndex);
 	TInt GetDesMaxLengthL(TInt aSrcParamIndex);

protected:
	CESockClientActivityBase(const MeshMachine::TNodeActivity& aActivitySig, MeshMachine::AMMNodeBase& aNode)
	:	MeshMachine::CNodeRetryActivity(aActivitySig, aNode)
		{
		//Either we are the close activity or there should not be the close activity
		//running when this activity is being created.
		__ASSERT_DEBUG(aActivitySig.iId==ESock::ECFActivityDestroy || aNode.CountActivities(ESock::ECFActivityDestroy)==0, User::Panic(KSpecAssert_ESockCrStaESclAH, 1));
		}
	virtual ~CESockClientActivityBase();
	virtual void StartL(MeshMachine::TNodeContextBase& aContext, const Messages::XNodePeerId& aOriginator, const NetStateMachine::TStateTriple& aFirst);

protected:
	RMessage2 iMessage;
	CSockSubSession *iSubSession;
	};

//
//CPreallocatedESockClientActivity
NONSHARABLE_CLASS(CPreallocatedESockClientActivity) : public CESockClientActivityBase, protected MeshMachine::APreallocatedOriginators<1>
	{
public:
    static MeshMachine::CNodeActivityBase* New(const MeshMachine::TNodeActivity& aActivitySig, MeshMachine::AMMNodeBase& aNode);

protected:
	CPreallocatedESockClientActivity(const MeshMachine::TNodeActivity& aActivitySig, MeshMachine::AMMNodeBase& aNode);

	virtual void Destroy();
private:
	/*
	Private NewL with no implementation to hide the CNodeActivityBase::NewL
	Creation of preallocated activities doesn't fail and hence a non-leaving ::New should be used instead*/
    static MeshMachine::CNodeActivityBase* NewL(const MeshMachine::TNodeActivity& aActivitySig, MeshMachine::AMMNodeBase& aNode);
	};

//
//CCloseActivity
class CCloseActivity : public CESockClientActivityBase, protected MeshMachine::APreallocatedOriginators<1>
	{
public:
    static MeshMachine::CNodeActivityBase* New(const MeshMachine::TNodeActivity& aActivitySig, MeshMachine::AMMNodeBase& aNode);

protected:
	CCloseActivity(const MeshMachine::TNodeActivity& aActivitySig, MeshMachine::AMMNodeBase& aNode);

	virtual void Destroy();
private:
	/*
	Private NewL with no implementation to hide the CNodeActivityBase::NewL
	Creation of preallocated activities doesn't fail and hence a non-leaving ::New should be used instead*/
    static MeshMachine::CNodeActivityBase* NewL(const MeshMachine::TNodeActivity& aActivitySig, MeshMachine::AMMNodeBase& aNode);
	};

} //namespace SubSessActivities

#endif //SYMBIAN_SS_ESOCKACTIVITIES_H

