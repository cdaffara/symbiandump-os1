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
// Sub Session States
// 
//

/**
 @file
 @internalComponent
*/

#ifndef SYMBIAN_ESOCKSTATES_H
#define SYMBIAN_ESOCKSTATES_H

#include <comms-infras/ss_nodemessages.h>
#include <comms-infras/ss_activities.h>
#include <comms-infras/ss_mmnode.h>
#include <comms-infras/ss_mmnode_subsess.h>
#include <comms-infras/ss_coreprstates.h>
#include <comms-infras/ss_esockactivities.h>
#include <comms-infras/ss_nodemessages_internal_esock.h>

template <TInt EXTMESSAGEID>
ESock::TCFInternalEsock::TSubSess* subsessmessage_cast(Messages::TSignatureBase* aMessage)
    {
   ESock::TCFInternalEsock::TSubSess* m = Messages::message_cast<ESock::TCFInternalEsock::TSubSess>(aMessage);
   return m && m->iValue == EXTMESSAGEID ? m : NULL;
   }

namespace SubSessStates
{

typedef MeshMachine::TNodeContext<ESock::CMMSockSubSession, CoreStates::TContext> TContext;

//
// Default Cancel and Error handling for states of CESockClientActivityBase Activities
// Intended for ECAB'isation of an otherwise normal state
// Not intended to be used as the first ECAB state (no TransitionTag handling) which
// is never actually desired (specific Accept must be present for the first state of ECAB)

class AECABState
    {
protected:
	static TBool Accept(MeshMachine::TNodeContextBase& aContext, TBool aSuperAccept);
	};

template<class TSTATE>
class TECABState : public TSTATE, public AECABState
    {
public:
	typedef typename TSTATE::TContext TContext;
	NETSM_TPL_DECLARE_CTR(TECABState, NetStateMachine::MState, TContext)

    explicit TECABState(TContext& aContext)
    :	TSTATE(aContext)
        {
        }

	virtual TBool Accept()
    	{
    	return AECABState::Accept(this->iContext, TSTATE::Accept());
    	}

	virtual void Cancel()
        {
        if(this->iContext.iNodeActivity != NULL)
        	{
        	this->iContext.iNodeActivity->SetError(KErrCancel);
        	}
        TSTATE::Cancel();
        }
    };

//
//Default self generated error handling for transitions of CESockClientActivityBase Activities
template <class TCONTEXT>
class TECABStateTransitionBase : public MeshMachine::TStateTransition<TCONTEXT>
    {
public:
	//No NETSM_TPL_DECLARE_CTR is necessary as this template is used as a base only
    explicit TECABStateTransitionBase(TCONTEXT& aContext)
    :	MeshMachine::TStateTransition<TCONTEXT>(aContext) {}

	virtual void Error(TInt aError)
	    {
        MeshMachine::TStateTransition<TCONTEXT>::iContext.iReturn = aError;
        if(MeshMachine::TStateTransition<TCONTEXT>::iContext.iNodeActivity != NULL)
        	{
        	static_cast<SubSessActivities::CESockClientActivityBase&>(*MeshMachine::TStateTransition<TCONTEXT>::iContext.iNodeActivity).SetError(aError);
	        MeshMachine::TStateTransition<TCONTEXT>::Error(aError);
        	}
        }
    };

//
// Default Error handling for transitions of CESockClientActivityBase Activities
// Intended for ECAB'isation of an otherwise normal transition
template<class TSTATETRANSITION>
class TECABTransition : public TSTATETRANSITION
    {
public:
	typedef typename TSTATETRANSITION::TContext TContext;
	NETSM_TPL_DECLARE_CTR(TECABTransition, NetStateMachine::MStateTransition, TContext)

    explicit TECABTransition(TContext& aContext)
    :	TSTATETRANSITION(aContext)
        {
        }

	virtual void Error(TInt aError)
        {
        if(this->iContext.iNodeActivity != NULL)
        	{
        	static_cast<SubSessActivities::CESockClientActivityBase&>(*this->iContext.iNodeActivity).SetError(KErrCancel);
	        TSTATETRANSITION::Error(aError);
        	}
        }
    };

//
//Template state awaiting an IPC message
template<TInt IPC>
class TAwaitingIPC : public MeshMachine::TState<TContext>
    {
public:
	NETSM_TPL_DECLARE_CTR(TAwaitingIPC, NetStateMachine::MState, TContext)

	explicit TAwaitingIPC(TContext& aContext)
	:	MeshMachine::TState<TContext>(aContext)
		{
		}

	virtual TBool Accept()
    	{
		return subsessmessage_cast<IPC>(&this->iContext.iMessage) != NULL;
//		return subsessmessage_cast<IPC>(&this->iContext.iMessage) ?
//		    ETrue : EFalse;
    	}
    };

template<class TBASESTATE, TCapability CAP1 = ECapability_None, TCapability CAP2 = ECapability_None, TCapability CAP3 = ECapability_None>
class TAwaitingSecuredIpc : public TBASESTATE
   {
public:
	NETSM_TPL_DECLARE_CTR(TAwaitingSecuredIpc, NetStateMachine::MState, TContext)

   TAwaitingSecuredIpc(TContext& aContext) : TBASESTATE(aContext)
      {
      }

   virtual TBool Accept()
      {
      if(TBASESTATE::Accept())
         {
         ESock::TCFInternalEsock::TSubSess& msg = Messages::message_cast<ESock::TCFInternalEsock::TSubSess>(this->iContext.iMessage);
         const RMessage2& clientMessage = msg.iMessage;

   		_LIT_SECURITY_POLICY_C3(policy, CAP1, CAP2, CAP3);
   		if(policy.CheckPolicy(clientMessage))
   			{
   			return ETrue;
   			}
// TODO(SPT): Can this just set the error on the activity and then SetIdle()?
//Response: no, because there is no activity yet, it would be instantiated only after this states accepted.
			clientMessage.Complete(KErrPermissionDenied);
			TBASESTATE::iContext.Node().DontCompleteCurrentRequest();
         }
      return EFalse;
      }
   };

DECLARE_SMELEMENT_HEADER( TAcquireMessageOwnership, SubSessStates::TECABStateTransitionBase<TContext>, NetStateMachine::MStateTransition, TContext )
	virtual void DoL();
DECLARE_SMELEMENT_FOOTER( TAcquireMessageOwnership )

DECLARE_AGGREGATED_TRANSITION3(
	TProcessClose,
	CoreStates::TAbortAllActivitiesNodeDeletion,
	CoreNetStates::TSendClientLeavingRequestToServiceProviders,
	CoreNetStates::TSetIdleIfNoServiceProviders
	)

DECLARE_SMELEMENT_HEADER( TBinEvent, SubSessStates::TECABStateTransitionBase<TContext>, NetStateMachine::MStateTransition, TContext )
	virtual void DoL();
DECLARE_SMELEMENT_FOOTER( TBinEvent )

DECLARE_SMELEMENT_HEADER( TCancelAndCloseClientExtIfaces, SubSessStates::TECABStateTransitionBase<TContext>, NetStateMachine::MStateTransition, TContext )
	virtual void DoL();
DECLARE_SMELEMENT_FOOTER( TCancelAndCloseClientExtIfaces )

} //namespace SubSessStates

#endif //SYMBIAN_ESOCKSTATES_H


