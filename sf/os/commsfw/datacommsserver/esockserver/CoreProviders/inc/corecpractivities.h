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
// Core CPR Activities
// 
//

/**
 @file
 @internalComponent
*/

#ifndef SYMBIAN_CORECPRACTIVITIES_H
#define SYMBIAN_CORECPRACTIVITIES_H

#include <elements/sm_core.h>
#include <comms-infras/ss_corepractivities.h>
#include <comms-infras/corecprstates.h>



namespace CprActivities
{

DECLARE_EXPORT_ACTIVITY_MAP(coreCprActivities)


} //namespace CprActivities

namespace CprDataClientStartActivity
{
typedef MeshMachine::TNodeContext<CCoreConnectionProvider, PRStates::TContext> TContext;

EXPORT_DECLARE_SMELEMENT_HEADER( TNoTagOrNoTagBackward, MeshMachine::TStateFork<TContext>, NetStateMachine::MStateFork, TContext )
	IMPORT_C virtual TInt TransitionTag();
DECLARE_SMELEMENT_FOOTER( TNoTagOrNoTagBackward )

EXPORT_DECLARE_SMELEMENT_HEADER( TStartDataClient, MeshMachine::TStateTransition<TContext>, NetStateMachine::MStateTransition, TContext )
	IMPORT_C virtual void DoL();
DECLARE_SMELEMENT_FOOTER( TStartDataClient )

class CDataClientStartActivity : public MeshMachine::CNodeActivityBase
	{
public:
    static MeshMachine::CNodeActivityBase* NewL(const MeshMachine::TNodeActivity& aActivitySig, MeshMachine::AMMNodeBase& aNode)
			{ return new(ELeave) CDataClientStartActivity(aActivitySig, aNode); }

	virtual ~CDataClientStartActivity() {}

public:
	Messages::TClientIter<Messages::TDefaultClientMatchPolicy> iClientIter;

protected:
	IMPORT_C CDataClientStartActivity(const MeshMachine::TNodeActivity& aActivitySig, MeshMachine::AMMNodeBase& aNode);
	};

}

namespace CprPriorityUpdateActivity
{
class CPriorityUpdateActivity : public MeshMachine::CNodeParallelActivityBase
	{
public:
	TUint Priority() const
		{
		return iLastPriority;
		}
	void SetPriority(TUint aPriority)
		{
		iLastPriority = aPriority;
		}
	TBool UpdatePriority() const
		{
		return iUpdatePriority;
		}
public:
	EXPORT_DECLARE_SMELEMENT_HEADER( TNoTagOrSendPriorityToCtrlProvider, MeshMachine::TStateFork<CprStates::TContext>, NetStateMachine::MStateFork, CprStates::TContext )
		IMPORT_C virtual TInt TransitionTag();
	DECLARE_SMELEMENT_FOOTER( TNoTagOrSendPriorityToCtrlProvider )

	EXPORT_DECLARE_SMELEMENT_HEADER( TNoTagOrSendPriorityToServProvider, MeshMachine::TStateFork<CprStates::TContext>, NetStateMachine::MStateFork, CprStates::TContext )
		IMPORT_C virtual TInt TransitionTag();
	DECLARE_SMELEMENT_FOOTER( TNoTagOrSendPriorityToServProvider )

	EXPORT_DECLARE_SMELEMENT_HEADER( TUpdatePriorityForServiceProviders, MeshMachine::TStateTransition<CprStates::TContext>, NetStateMachine::MStateTransition, CprStates::TContext )
		IMPORT_C virtual void DoL();
	DECLARE_SMELEMENT_FOOTER( TUpdatePriorityForServiceProvider )

	EXPORT_DECLARE_SMELEMENT_HEADER( TUpdatePriorityForControlProvider, MeshMachine::TStateTransition<CprStates::TContext>, NetStateMachine::MStateTransition, CprStates::TContext )
		IMPORT_C virtual void DoL();
	DECLARE_SMELEMENT_FOOTER( TUpdatePriorityForControlProvider )

protected:
	CPriorityUpdateActivity(const MeshMachine::TNodeActivity& aActivitySig, MeshMachine::AMMNodeBase& aNode, TUint aActivitiesCount);
	
protected:
	TBool iUpdatePriority;

private:
	TUint iLastPriority;
	};
}

namespace CprControlClientJoinActivity
{
class CControlClientJoinActivity : public CprPriorityUpdateActivity::CPriorityUpdateActivity
	{
public:
	IMPORT_C static MeshMachine::CNodeActivityBase* NewL(const MeshMachine::TNodeActivity& aActivitySig, MeshMachine::AMMNodeBase& aNode);
	IMPORT_C virtual ~CControlClientJoinActivity();

public:

	void SaveJoinMessageData(CprStates::TContext& aContext)
		{
		if (aContext.iMessage.IsMessage<ESock::TCFPeer::TJoinRequest>())
			{
			iSendJoinComplete = ETrue;
			}
		if (aContext.Node().FindClient(aContext.iSender))
			{
			iUpdatePriority = ETrue;
			}
		}

	TBool NeedToSendJoinComplete() const
		{
		return iSendJoinComplete;
		}

public:

	EXPORT_DECLARE_SMELEMENT_HEADER( TAddClientOrUpdatePriority, MeshMachine::TStateFork<CprStates::TContext>, NetStateMachine::MStateFork, CprStates::TContext )
		IMPORT_C virtual TInt TransitionTag();
	DECLARE_SMELEMENT_FOOTER( TAddClientOrUpdatePriority )

	EXPORT_DECLARE_SMELEMENT_HEADER( TNoTagOrSendActive, MeshMachine::TStateFork<CprStates::TContext>, NetStateMachine::MStateFork, CprStates::TContext )
		IMPORT_C virtual TInt TransitionTag();
	DECLARE_SMELEMENT_FOOTER( TNoTagOrSendActive )

	EXPORT_DECLARE_SMELEMENT_HEADER( TSendJoinCompleteIfRequest,  MeshMachine::TStateTransition<CprStates::TContext>, NetStateMachine::MStateTransition, CprStates::TContext )
		IMPORT_C virtual void DoL();
	DECLARE_SMELEMENT_FOOTER( TSendJoinCompleteIfRequest )

private:
	TBool iSendJoinComplete;

protected:
	CControlClientJoinActivity(const MeshMachine::TNodeActivity& aActivitySig, MeshMachine::AMMNodeBase& aNode, TUint aActivitiesCount);
	};
}

namespace CprClientLeaveActivity
{
class CClientLeaveActivity : public CprPriorityUpdateActivity::CPriorityUpdateActivity
	{
public:
	IMPORT_C static MeshMachine::CNodeActivityBase* New(const MeshMachine::TNodeActivity& aActivitySig, MeshMachine::TNodeContextBase& aContext);
	IMPORT_C virtual ~CClientLeaveActivity();

public:
	typedef MeshMachine::TNodeContext<CCoreConnectionProvider, CprStates::TContext> TContext;

	void SaveLeaveMessageData(const TContext& aContext)
		{
		iSendLeaveComplete = aContext.iMessage.IsMessage<ESock::TCFPeer::TLeaveRequest>() ? ETrue: EFalse;
		}
	
	TBool NeedToSendLeaveComplete() const
		{
		return iSendLeaveComplete;

		}
public:
	EXPORT_DECLARE_SMELEMENT_HEADER( TSendLeaveCompleteIfRequest, MeshMachine::TStateTransition<TContext>, NetStateMachine::MStateTransition, TContext )
		IMPORT_C virtual void DoL();
	DECLARE_SMELEMENT_FOOTER( TSendLeaveCompleteIfRequest )

	EXPORT_DECLARE_SMELEMENT_HEADER( TSaveLeaveMessageData, MeshMachine::TStateTransition<TContext>, NetStateMachine::MStateTransition, TContext )
		IMPORT_C virtual void DoL();
	DECLARE_SMELEMENT_FOOTER( TSaveLeaveMessageData )

	DECLARE_AGGREGATED_TRANSITION2(
		TSendLeaveCompleteAndSendDataClientIdleIfNeeded,
		TSendLeaveCompleteIfRequest,
		CoreNetStates::TSendDataClientIdleIfNoClients
		)
	DECLARE_AGGREGATED_TRANSITION3(
		TRemoveClientAndDestroyOrphanedDataClients,
		TSaveLeaveMessageData,
		MeshMachine::TRemoveClient,
		PRStates::TDestroyOrphanedDataClients
		)

private:
	TBool iSendLeaveComplete;

protected:
	CClientLeaveActivity(const MeshMachine::TNodeActivity& aActivitySig, MeshMachine::AMMNodeBase& aNode, TUint aActivitiesCount);
	};
}

namespace CprBindToActivity
{
class CCprBindToActivity : public PRActivities::CBindToActivity
	{
protected:
	typedef MeshMachine::TNodeContext<CCoreConnectionProvider, CoreNetStates::TContext> TContext;
public:
	EXPORT_DECLARE_SMELEMENT_HEADER( TSendControlClientJoinRequestWithPriority, MeshMachine::TStateTransition<CCprBindToActivity::TContext>, NetStateMachine::MStateTransition, CCprBindToActivity::TContext )
   		virtual void DoL();
	DECLARE_SMELEMENT_FOOTER( TSendControlClientJoinRequestWithPriority )

protected:
	CCprBindToActivity(const MeshMachine::TNodeActivity& aActivitySig,  MeshMachine::AMMNodeBase& aNode, TInt aNextActivityCount);

// private:
// if you are adding any data here don't forget to override NewL.
	};
}

#endif //SYMBIAN_CORECPRACTIVITIES_H

