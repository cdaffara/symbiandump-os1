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
// generic framework node states templates and helper macros
// 
//

/**
 @file
 @publishedPartner
 @released
*/


#ifndef SYMBIAN_MM_STATES_H
#define SYMBIAN_MM_STATES_H

#include <elements/sm_core.h>
#include <elements/mm_context.h>
#include <elements/mm_log.h>
#include <elements/mm_activities.h> //for AContextStore, logging only
#include <elements/nm_signatures.h>


#ifdef _DEBUG
// Panic category for "absolutely impossible!" vanilla ASSERT()-type panics from this module
// (if it could happen through user error then you should give it an explicit, documented, category + code)
_LIT(KSpecAssert_ElemMeshMachStaH, "ElemMeshMachStaH");
#endif

//-=========================================================
//
// MACROS
//
//-=========================================================
#define DECLARE_SMELEMENT_HEADER(element, base, intf, context) \
NONSHARABLE_CLASS(element) : public base \
	{ \
public:	\
	explicit element(context& aContext) : \
	    base(aContext) {} \
	NETSM_DECLARE_CTR(intf)

#define EXPORT_DECLARE_SMELEMENT_HEADER(element, base, intf, context) \
class element : public base \
	{ \
public:	\
	explicit element(context& aContext) : \
	    base(aContext) {} \
	EXPORT_NETSM_DECLARE_CTR(intf) \

#define DECLARE_SMELEMENT_FOOTER(element) \
	}; \



#define DEFINE_SMELEMENT(element, intf, context) \
	NETSM_DEFINE_CTR(element, intf, context) \

#define EXPORT_DEFINE_SMELEMENT(element, intf, context) \
	EXPORT_NETSM_DEFINE_CTR(element, intf, context) \



#define DECLARE_AGGREGATED_TRANSITION2(Name, T1, T2) \
typedef MeshMachine::TAggregatedTransition<T1, T2, TContext> Name;

#define DECLARE_AGGREGATED_TRANSITION3(Name, T1, T2, T3) \
DECLARE_AGGREGATED_TRANSITION2(Name##Name__TR3, T1, T2) \
DECLARE_AGGREGATED_TRANSITION2(Name, Name##Name__TR3, T3)

#define DECLARE_AGGREGATED_TRANSITION4(Name, T1, T2, T3, T4) \
DECLARE_AGGREGATED_TRANSITION3(Name##Name__TR4, T1, T2, T3) \
DECLARE_AGGREGATED_TRANSITION2(Name, Name##Name__TR4, T4)

#define DECLARE_AGGREGATED_TRANSITION5(Name, T1, T2, T3, T4, T5) \
DECLARE_AGGREGATED_TRANSITION3(Name##Name__TR5, T1, T2, T3) \
DECLARE_AGGREGATED_TRANSITION3(Name, Name##Name__TR5, T4, T5)

#define DECLARE_AGGREGATED_TRANSITION6(Name, T1, T2, T3, T4, T5, T6) \
DECLARE_AGGREGATED_TRANSITION4(Name##Name__TR6, T1, T2, T3, T4) \
DECLARE_AGGREGATED_TRANSITION3(Name, Name##Name__TR6, T5, T6)

#define DECLARE_AGGREGATED_TRANSITION7(Name, T1, T2, T3, T4, T5, T6, T7) \
DECLARE_AGGREGATED_TRANSITION4(Name##Name__TR7, T1, T2, T3, T4) \
DECLARE_AGGREGATED_TRANSITION4(Name, Name##Name__TR7, T5, T6, T7)


#define DECLARE_SERIALIZABLE_STATE(Name, Policy, State) \
typedef MeshMachine::TSerializableStateFork<Policy, State> Name;

#define DECLARE_SERIALIZABLE_STATE_WITH_ERRORTAG(Name, Policy, ErrorTagPolicy, State) \
typedef MeshMachine::TSerializableStateFork<Policy, State> Name;

#define DECLARE_SERIALIZABLE_TRANSITION(Name, Policy, StateTransition) \
typedef MeshMachine::TSerializableTransition<Policy, StateTransition> Name;





namespace MeshMachine
{

//In an (unusual) case where an MState needs to raise
//raise an error, it can produce KErrorTag as its
//transition tag. The way the actual error code
//is conveyed is undefined at this level. It is suggested
//to use the context for that. It is herby enforced to
//use a naming convention for states that can raise
//an error 'E' suffix (e.g.: AwaitSomethingE).
const TInt KNoTag                       = NetStateMachine::KExecuteAlways - 3;
const TInt KErrorTag                    = NetStateMachine::KExecuteAlways - 4;
const TInt KCancelTag                   = NetStateMachine::KExecuteAlways - 5;
const TInt KActiveTag                   = NetStateMachine::KExecuteAlways - 6;


class AMMNodeBase;
typedef MeshMachine::TNodeContext<AMMNodeBase> TContext;

//-=========================================================
//
//Default cancel handling for states
//
//-=========================================================
template <class TCONTEXT>
class TState : public NetStateMachine::AContext<TCONTEXT>,
               public NetStateMachine::MState
    {
public:
	NETSM_TPL_DECLARE_CTR(TState, NetStateMachine::MState, TCONTEXT)
	typedef TCONTEXT TContext;

    TState(TCONTEXT& aContext) :
        NetStateMachine::AContext<TCONTEXT>(aContext)
        {
        }

	virtual void Cancel()
        {
        if (this->iContext.iNodeActivity)
        	{
        	this->iContext.iNodeActivity->SetError(KErrCancel);
        	}
        }
    };

//-=========================================================
//
// Default error handling State Transition
//
//-=========================================================
template <class TCONTEXT>
class TStateTransition : public NetStateMachine::AContext<TCONTEXT>,
                         public NetStateMachine::MStateTransition
    {
public:
	NETSM_TPL_DECLARE_CTR(TStateTransition, NetStateMachine::MStateTransition, TCONTEXT)
	typedef TCONTEXT TContext;

    TStateTransition(TCONTEXT& aContext) :
        NetStateMachine::AContext<TCONTEXT>(aContext)
        {
        }

	virtual void Error( TInt aError )
	    {
        NetStateMachine::AContext<TCONTEXT>::iContext.iReturn = aError;
        if(NetStateMachine::AContext<TCONTEXT>::iContext.iNodeActivity != NULL)
        	{
        	NetStateMachine::AContext<TCONTEXT>::iContext.iNodeActivity->SetError(aError);
	        NetStateMachine::AContext<TCONTEXT>::iContext.iNodeActivity->SetIdle(); //By default, set activity to idle
        	}
        }
    };

//-=========================================================
//
//Default state fork
//
//-=========================================================
template <class TCONTEXT>
class TStateFork : public NetStateMachine::AContext<TCONTEXT>,
                   public NetStateMachine::MStateFork
    {
public:
    NETSM_TPL_DECLARE_CTR(TStateFork, NetStateMachine::MStateTransition, TCONTEXT)
	typedef TCONTEXT TContext;

    TStateFork(TCONTEXT& aContext) :
        NetStateMachine::AContext<TCONTEXT>(aContext)
        {
        }

	virtual TInt TransitionTag()
    	{
    	return KNoTag | NetStateMachine::EForward;
    	}
    };


//-=========================================================
//
//Serializable states & transitions
//
//-=========================================================
template <TInt ERRORTAG>
class TStateErrorTransitionTag
	{
public:
	static TInt ErrorTag(const TNodeContextBase& /*aContext*/) { return ERRORTAG; };
	};

template <class TPOLICY, typename TSTATEFORK, class TERRORTAGPOLICY = TStateErrorTransitionTag<KErrorTag> >
class TSerializableStateFork : public TSTATEFORK
    {
public:
	typedef typename TSTATEFORK::TContext TContext;

	NETSM_TPL_DECLARE_CTR(TSerializableStateFork, NetStateMachine::MStateFork, TContext)

    TSerializableStateFork(TContext& aContext)
    :	TSTATEFORK(aContext)
		{
		}

	virtual TInt TransitionTag()
    	{
    	if (TPOLICY::IsBlocked(TSTATEFORK::iContext))
    		{
    		return (AActivitySemaphore::ParkState(TSTATEFORK::iContext)==KErrNone)?
    			NetStateMachine::EIgnore : TERRORTAGPOLICY::ErrorTag(TSTATEFORK::iContext);
    		}
    	else
    		{
		   	MeshMachine::AActivitySemaphore* as = static_cast<MeshMachine::AActivitySemaphore*>(TSTATEFORK::iContext.iNodeActivity->FetchExtInterface(MeshMachine::AActivitySemaphore::KInterfaceId));
			__ASSERT_DEBUG(as, User::Panic(KSpecAssert_ElemMeshMachStaH, 1)); //State/transition mismatched with the activity object (activity doesn't support serialisation - wrong derivation?)

			// Log whether the state unparked
			if(as->IsWaiting())
				{
				AActivitySemaphore::UnparkState(TSTATEFORK::iContext);
				}

    		return TSTATEFORK::TransitionTag();
    		}
    	}
    };

template <class TPOLICY, typename TSTATETRANSITION>
class TSerializableTransition : public TSTATETRANSITION
    {
public:
	typedef typename TSTATETRANSITION::TContext TContext;

	NETSM_TPL_DECLARE_CTR(TSerializableTransition, NetStateMachine::MStateTransition, TContext)

    TSerializableTransition(TContext& aContext)
    :	TSTATETRANSITION(aContext)
		{
		}

    virtual void DoL()
    	{
   		//If this assertion fires, the serilisable transition has been
   		//wrongly positioned in the first triple of its activity.
   		__ASSERT_DEBUG(TSTATETRANSITION::iContext.iNodeActivity, User::Panic(KSpecAssert_ElemMeshMachStaH, 2));
    	if (TPOLICY::IsBlocked(TSTATETRANSITION::iContext))
    		{
    		AActivitySemaphore::ParkTransitionL(TSTATETRANSITION::iContext);
    		}
    	else
    		{
#ifdef SYMBIAN_TRACE_ENABLE
			// Log whether the transition unparked
		   	MeshMachine::AActivitySemaphore* as = static_cast<MeshMachine::AActivitySemaphore*>(TSTATETRANSITION::iContext.iNodeActivity->FetchExtInterface(MeshMachine::AActivitySemaphore::KInterfaceId));
			__ASSERT_DEBUG(as, User::Panic(KSpecAssert_ElemMeshMachStaH, 3)); //State/transition mismatched with the activity object (activity doesn't support serialisation - wrong derivation?)
			if(as->IsWaiting())
				{
				MESH_LOG_CONTEXT_EXT(KMeshMachineSubTag, TSTATETRANSITION::iContext, _L8("TSerializableTransition::DoL unparked"));
				}
#endif
    		TSTATETRANSITION::DoL();
    		}
    	}
    };

//-=========================================================
//
//Aggregated transitions
//
//-=========================================================

template <class T1, class T2, class TCONTEXT>
class TAggregatedTransition : public TStateTransition<TCONTEXT>
    {
public:
	NETSM_TPL_DECLARE_CTR(TAggregatedTransition, NetStateMachine::MStateTransition, TCONTEXT)

    TAggregatedTransition(TCONTEXT& aContext)
    :	TStateTransition<TCONTEXT>(aContext)
        {
        }

	virtual void DoL()
    	{
    	T1 t1(NetStateMachine::AContext<TCONTEXT>::iContext);
    	T2 t2(NetStateMachine::AContext<TCONTEXT>::iContext);
    	t1.DoL();
    	t2.DoL();
    	}

	virtual void Error(TInt aError)
    	{
    	T1 t1(NetStateMachine::AContext<TCONTEXT>::iContext);
    	T2 t2(NetStateMachine::AContext<TCONTEXT>::iContext);
    	t1.Error(aError);
    	t2.Error(aError);
    	}
    };

//-=========================================================
//
//Commonly used states
//
//-=========================================================

template<typename TMESSAGE>
class TAwaitingMessageState : public MeshMachine::TState<MeshMachine::TContext>
	{
public:
	NETSM_TPL_DECLARE_CTR(TAwaitingMessageState, NetStateMachine::MState, MeshMachine::TContext)

	explicit TAwaitingMessageState(MeshMachine::TContext& aContext) :
		MeshMachine::TState<MeshMachine::TContext>(aContext) {}

	virtual TBool Accept()
		{
#ifdef __GCCXML__
		return EFalse;
#else
		Messages::TNodeSignal& msg = this->iContext.iMessage; //Arm compiler is getting confused otherwise
		return msg.IsMessage<TMESSAGE>();
#endif
		}
	};

template<typename TMESSAGE1, class TMESSAGE2>
class TAwaiting2MessagesState : public MeshMachine::TState<TContext>
	{
public:
	NETSM_TPL_DECLARE_CTR(TAwaiting2MessagesState, NetStateMachine::MState, TContext)

	explicit TAwaiting2MessagesState(TContext& aContext) :
		MeshMachine::TState<TContext>(aContext) {}

	virtual TBool Accept()
			{
#ifdef __GCCXML__
			return EFalse;
#else
			Messages::TNodeSignal& msg = this->iContext.iMessage; //Arm compiler is getting confused otherwise
			return msg.IsMessage<TMESSAGE1>()
				|| msg.IsMessage<TMESSAGE2>();
#endif
			}
	};

template<typename TMESSAGE, TUint32 FlagsToSet = 0, TUint32 FlagsToClear = 0>
class TAwaitingPeerMessage : public MeshMachine::TState<TContext>
    {
public:
	NETSM_TPL_DECLARE_CTR(TAwaitingPeerMessage, NetStateMachine::MState, TContext)

    TAwaitingPeerMessage(TContext& aContext) :
        MeshMachine::TState<TContext>(aContext)
        {
        }
	virtual TBool Accept()
    	{
#ifndef __GCCXML__
		Messages::TNodeSignal& msg = this->iContext.iMessage; //Arm compiler is getting confused otherwise
		if (msg.IsMessage<TMESSAGE>())
			{
			__ASSERT_DEBUG(this->iContext.iPeer, User::Panic(KSpecAssert_ElemMeshMachStaH, 4));
			this->iContext.iPeer->ClearFlags(FlagsToClear);
			this->iContext.iPeer->SetFlags(FlagsToSet);
			return ETrue;
			}
#endif
		return EFalse;
    	}
    };

class AAcceptErrorState
    {
protected:
	IMPORT_C static TBool Accept(MeshMachine::TNodeContextBase& aContext, TBool aSuperAccept);
	};

template<class TSTATE>
class TAcceptErrorState  : public TSTATE, public AAcceptErrorState
    {
public:
	typedef typename TSTATE::TContext TContext;
	NETSM_TPL_DECLARE_CTR(TAcceptErrorState, NetStateMachine::MState, TContext)

    explicit TAcceptErrorState(TContext& aContext)
    :	TSTATE(aContext)
        {
        }

	virtual TBool Accept()
    	{
    	return AAcceptErrorState::Accept(this->iContext, TSTATE::Accept());
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

EXPORT_DECLARE_SMELEMENT_HEADER( TAwaitingNull, TState<TContext>, NetStateMachine::MState, TContext )
	IMPORT_C virtual TBool Accept();
DECLARE_SMELEMENT_FOOTER( TAwaitingNull )

EXPORT_DECLARE_SMELEMENT_HEADER( TAwaitingAny, TState<TContext>, NetStateMachine::MState, TContext )
	IMPORT_C virtual TBool Accept();
DECLARE_SMELEMENT_FOOTER( TAwaitingAny )

EXPORT_DECLARE_SMELEMENT_HEADER( TAwaitingClientLeavingRequest, TState<TContext>, NetStateMachine::MState, TContext )
	IMPORT_C virtual TBool Accept();
DECLARE_SMELEMENT_FOOTER( TAwaitingClientLeavingRequest )

EXPORT_DECLARE_SMELEMENT_HEADER( TAwaitingLeaveComplete, TState<TContext>, NetStateMachine::MState, TContext )
    IMPORT_C virtual TBool Accept();
DECLARE_SMELEMENT_FOOTER( TAwaitingLeaveComplete )

EXPORT_DECLARE_SMELEMENT_HEADER( TAwaitingDestroy, TState<TContext>, NetStateMachine::MState, TContext )
	IMPORT_C virtual TBool Accept();
DECLARE_SMELEMENT_FOOTER( TAwaitingDestroy )

EXPORT_DECLARE_SMELEMENT_HEADER( TAwaitingCancel, TState<TContext>, NetStateMachine::MState, TContext )
	IMPORT_C virtual TBool Accept();
DECLARE_SMELEMENT_FOOTER( TAwaitingCancel )

EXPORT_DECLARE_SMELEMENT_HEADER( TAwaitingErrorRecoveryResponseOrError, TState<TContext>, NetStateMachine::MState, TContext )
	IMPORT_C virtual TBool Accept();
DECLARE_SMELEMENT_FOOTER( TAwaitingErrorRecoveryResponseOrError )

//-=========================================================
//
//Commonly used state forks
//
//-=========================================================

template<TInt TAG>
class TTag : public MeshMachine::TStateFork<TContext>
    {
public:
	NETSM_TPL_DECLARE_CTR(TTag, NetStateMachine::MStateFork, TContext)

    TTag(TContext& aContext)
    :	MeshMachine::TStateFork<TContext>(aContext)
        {
        }

	virtual TInt TransitionTag()
    	{
    	return TAG;
    	}
    };

template<TInt ACTIVITYSIG>
class TActiveOrNoTag  : public MeshMachine::TStateFork<TContext>
    {
public:
	NETSM_TPL_DECLARE_CTR(TActiveOrNoTag, NetStateMachine::MStateFork, TContext)

    explicit TActiveOrNoTag(TContext& aContext)
    :	MeshMachine::TStateFork<TContext>(aContext)
        {
        }

 	virtual TInt TransitionTag()
     	{
		return (this->iContext.iNode.CountActivities(ACTIVITYSIG))?
			MeshMachine::KActiveTag : MeshMachine::KNoTag;
     	}
     };

class AErrorTagOr
    {
protected:
	IMPORT_C static TInt TransitionTag(MeshMachine::TNodeContextBase& aContext);
	};

template<class TFORK>
class TErrorTagOr  : public TFORK, public AErrorTagOr
    {
public:
	typedef typename TFORK::TContext TContext;
	NETSM_TPL_DECLARE_CTR(TErrorTagOr, NetStateMachine::MStateFork, TContext)

     explicit TErrorTagOr(TContext& aContext)
     :	TFORK(aContext)
        {
        }

 	virtual TInt TransitionTag()
     	{
     	TUint tag = AErrorTagOr::TransitionTag(this->iContext);
     	return (tag!=0)? tag : TFORK::TransitionTag();
     	}
     };

EXPORT_DECLARE_SMELEMENT_HEADER( TNoTag, TStateFork<TContext>, NetStateMachine::MStateFork, TContext )
DECLARE_SMELEMENT_FOOTER( TNoTag )

EXPORT_DECLARE_SMELEMENT_HEADER( TNoTagBackward, TStateFork<TContext>, NetStateMachine::MStateFork, TContext )
	IMPORT_C virtual TInt TransitionTag();
DECLARE_SMELEMENT_FOOTER( TNoTagBackward )

EXPORT_DECLARE_SMELEMENT_HEADER( TErrorTag, TStateFork<TContext>, NetStateMachine::MStateFork, TContext )
	IMPORT_C virtual TInt TransitionTag();
DECLARE_SMELEMENT_FOOTER( TErrorTag )

EXPORT_DECLARE_SMELEMENT_HEADER( TNoTagOrErrorTag, TStateFork<TContext>, NetStateMachine::MStateFork, TContext )
	IMPORT_C virtual TInt TransitionTag();
DECLARE_SMELEMENT_FOOTER( TNoTagOrErrorTag )



/**
This state fork will return KNoTag if no error is present in the activity, or a
user supplied tag if an error condition has occured.
*/
template<TInt TAG>
NONSHARABLE_CLASS(TNoTagOrCustomErrorTag) : public MeshMachine::TStateFork<TContext>
    {
public:
    NETSM_TPL_DECLARE_CTR(TNoTagOrCustomErrorTag, NetStateMachine::MStateFork, TContext)

    TNoTagOrCustomErrorTag(TContext& aContext)
        : MeshMachine::TStateFork<TContext>(aContext)
        {
        }

    virtual TInt TransitionTag()
        {
        Messages::TEBase::TError* msg = Messages::message_cast<Messages::TEBase::TError>(&iContext.iMessage);
        if (KErrNone==iContext.iNodeActivity->Error() && NULL==msg)
            {
            return KNoTag | NetStateMachine::EForward;
            }
        if (msg)
            {
            iContext.iNodeActivity->SetError(msg->iValue);
            }
        return TAG;
        }
    };


//-=========================================================
//
// Commonly used transitions
//
//-=========================================================

//RaiseError - this transion, when invoked, just leaves with the error
//given as the template parameter.
template<TInt ERROR>
class TRaiseError : public MeshMachine::TStateTransition<TContext>
    {
public:
	NETSM_TPL_DECLARE_CTR(TRaiseError, NetStateMachine::MStateTransition,TContext)

    TRaiseError(TContext& aContext) :
        MeshMachine::TStateTransition<TContext>(aContext)
        {
        }

	virtual void DoL()
    	{
    	User::Leave(ERROR);
    	}
    };


//ARetryTransition - used to turn standard transitions into "retry" ones,
//so that they can be used directly in a "retry activity".
template<class TSTATETRANSITION, class MESSAGE>
class TRetryTransition : public TSTATETRANSITION
    {
public:
	typedef typename TSTATETRANSITION::TContext TContext;
	NETSM_TPL_DECLARE_CTR(TRetryTransition, NetStateMachine::MStateTransition, TContext)

    explicit TRetryTransition(TContext& aContext)
    :	TSTATETRANSITION(aContext)
        {
        }

	virtual void DoL()
    	{
		__ASSERT_DEBUG(this->iContext.iNodeActivity, User::Panic(KSpecAssert_ElemMeshMachStaH, 5));
    	MeshMachine::AContextStore* intf = reinterpret_cast<MeshMachine::AContextStore*>(this->iContext.iNodeActivity->FetchExtInterfaceL(MeshMachine::AContextStore::KInterfaceId));
 		Messages::TNodeSignal::TMessageId retryingFor = intf->RetryingForMessageId();
 		if (retryingFor.IsNull() || retryingFor == MESSAGE::Id())
 			{
 			User::LeaveIfError(intf->StoreContext(this->iContext));
     		TSTATETRANSITION::DoL();
     		}
		}
    };

//TPostMsgToSender - just posts a reply to the sender of the message being
//processed. Only works for SigVoid obviously
template<class MESSAGETYPE>
class TPostMessageToSender : public MeshMachine::TStateTransition<TContext>
    {
public:
	NETSM_TPL_DECLARE_CTR(TPostMessageToSender, NetStateMachine::MStateTransition, TContext)

    explicit TPostMessageToSender(TContext& aContext)
    :	MeshMachine::TStateTransition<TContext>(aContext)
        {
        }

	virtual void DoL()
    	{
		this->iContext.PostToSender(MESSAGETYPE().CRef());
		}
    };

//TPostMsgToOriginators - just posts a reply to all originators of the activity
//Only works for SigVoid obviously
template<class MESSAGETYPE, TUint16 FLAGSTOSET = 0, TUint16 FLAGSTOCLEAR = 0>
class TPostMessageToOriginators : public MeshMachine::TStateTransition<TContext>
    {
public:
	NETSM_TPL_DECLARE_CTR(TPostMessageToOriginators, NetStateMachine::MStateTransition, TContext)

    explicit TPostMessageToOriginators(TContext& aContext)
    :	MeshMachine::TStateTransition<TContext>(aContext)
        {
        }

	virtual void DoL()
    	{
    	__ASSERT_DEBUG(this->iContext.iNodeActivity, User::Panic(KSpecAssert_ElemMeshMachStaH, 6));
    	this->iContext.iNodeActivity->PostToOriginators(MESSAGETYPE().CRef(), FLAGSTOSET, FLAGSTOCLEAR);
		}
    };

EXPORT_DECLARE_SMELEMENT_HEADER( TDoNothing, TStateTransition<TContext>, NetStateMachine::MStateTransition, TContext )
	IMPORT_C virtual void DoL();
DECLARE_SMELEMENT_FOOTER( TDoNothing )

EXPORT_DECLARE_SMELEMENT_HEADER( TSetIdle, TStateTransition<TContext>, NetStateMachine::MStateTransition, TContext )
	IMPORT_C virtual void DoL();
DECLARE_SMELEMENT_FOOTER( TSetIdle )

EXPORT_DECLARE_SMELEMENT_HEADER( TRemoveClient, TStateTransition<TContext>, NetStateMachine::MStateTransition, TContext )
	IMPORT_C virtual void DoL();
DECLARE_SMELEMENT_FOOTER( TRemoveClient )

EXPORT_DECLARE_SMELEMENT_HEADER( TRaiseActivityError, TStateTransition<TContext>, NetStateMachine::MStateTransition, TContext )
	IMPORT_C virtual void DoL();
DECLARE_SMELEMENT_FOOTER( TRaiseActivityError )

EXPORT_DECLARE_SMELEMENT_HEADER( TRaiseAndClearActivityError, TStateTransition<TContext>, NetStateMachine::MStateTransition, TContext )
	IMPORT_C virtual void DoL();
DECLARE_SMELEMENT_FOOTER( TRaiseAndClearActivityError )

EXPORT_DECLARE_SMELEMENT_HEADER( TStoreError, TStateTransition<TContext>, NetStateMachine::MStateTransition, TContext )
	IMPORT_C virtual void DoL();
DECLARE_SMELEMENT_FOOTER( TStoreError )

EXPORT_DECLARE_SMELEMENT_HEADER( TClearError, TStateTransition<TContext>, NetStateMachine::MStateTransition, TContext )
	IMPORT_C virtual void DoL();
DECLARE_SMELEMENT_FOOTER( TClearError )

EXPORT_DECLARE_SMELEMENT_HEADER( TForwardMessageToOriginators, TStateTransition<TContext>, NetStateMachine::MStateTransition, TContext )
	IMPORT_C virtual void DoL();
DECLARE_SMELEMENT_FOOTER( TForwardMessageToOriginators )

} //namespace MeshMachine


#endif
	// SYMBIAN_MM_STATES_H

