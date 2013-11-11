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
//

/**
 @file
 @internalTechnology
*/

#include <elements/mm_states.h>
#include <ss_internal_activities.h>

#ifndef SYMBIAN_ACTIVITYTEST
#define SYMBIAN_ACTIVITYTEST

namespace CancelTestBindToActivity
    {
    DECLARE_NODEACTIVITY(CancelBindTo)
    }

class CActivityCancelTest : public MeshMachine::CNodeRetryActivity
                                      
/**
@internalTechnology
*/
    {
public:
    //TODO
    enum {ECancelTestActivity = ESock::ECFActivityBuildStack };
    static MeshMachine::CNodeActivityBase* NewL(const MeshMachine::TNodeActivity& aActivitySig, MeshMachine::AMMNodeBase& aNode)
        {
        CActivityCancelTest* self = new (ELeave) CActivityCancelTest(aActivitySig, aNode);
        return self;
        }

    class TProgressiveMutex
        {
        public:
        static TBool IsBlocked(MeshMachine::TNodeContextBase& aContext)
            {
            return static_cast<CActivityCancelTest&>(*aContext.iNodeActivity).ProgressiveMutex();
            }
        };
    
    void NextLoop()
        {
        iMaxLoopCount++;
        iCurrentLoopCount = 0;        
        }
    
    TBool ProgressiveMutex()
        {
        return ++iCurrentLoopCount <= iMaxLoopCount;
        }
    
protected:
    CActivityCancelTest(const MeshMachine::TNodeActivity& aActivitySig, MeshMachine::AMMNodeBase& aNode)
    :   CNodeRetryActivity(aActivitySig, aNode)
        {
        }
    
    void StoreRequestL(const Messages::TSignatureBase& aRequest);
    const Messages::TSignatureBase& StoredRequest()
        { 
        return *reinterpret_cast<const Messages::TSignatureBase*>(iStoredRequest.Ptr()); 
        }
    
    
private:
    TBuf8<__Align8(Messages::TSignalBase::KMaxInlineMessageSize + Messages::TSignalBase::KMaxUnstoredOverhead)> iStoredRequest;
    TInt iMaxLoopCount;
    TInt iCurrentLoopCount;
    
public:
    typedef MeshMachine::TContext TContext;
    
    template<typename TMESSAGE>
    class TAwaitingMessageState : public MeshMachine::TState<TContext>
        {
    public:
        NETSM_TPL_DECLARE_CTR(TAwaitingMessageState, NetStateMachine::MState, TContext)

        explicit TAwaitingMessageState(TContext& aContext) :
            MeshMachine::TState<TContext>(aContext) {}

        virtual TBool Accept()
                {
    #ifdef __GCCXML__
                return EFalse;
    #else
                if (Messages::address_cast<const Messages::TNodeCtxId>(iContext.iSender).NodeCtx() != CActivityCancelTest::ECancelTestActivity)
                    {
                    Messages::TNodeSignal& msg = this->iContext.iMessage; //Arm compiler is getting confused otherwise
                    return msg.IsMessage<TMESSAGE>();
                    }
    #endif
                return EFalse;                
                }
        };  

    DECLARE_SMELEMENT_HEADER( TNoTagForwardOrActiveTagBackward, MeshMachine::TStateFork<TContext>, NetStateMachine::MStateFork, TContext )
        virtual TInt TransitionTag();
    DECLARE_SMELEMENT_FOOTER( TNoTagForwardOrActiveTagBackward )    
    
    DECLARE_SMELEMENT_HEADER( TBeginTest, MeshMachine::TStateTransition<TContext>, NetStateMachine::MStateTransition, TContext )
        virtual void DoL();
    DECLARE_SMELEMENT_FOOTER( TBeginTest )
    
    DECLARE_SMELEMENT_HEADER( TBeginLoop, MeshMachine::TStateTransition<TContext>, NetStateMachine::MStateTransition, TContext )
        virtual void DoL();
    DECLARE_SMELEMENT_FOOTER( TBeginLoop )    

    DECLARE_SMELEMENT_HEADER( TCancel, MeshMachine::TStateTransition<TContext>, NetStateMachine::MStateTransition, TContext )
        virtual void DoL();
    DECLARE_SMELEMENT_FOOTER( TCancel )

    DECLARE_SMELEMENT_HEADER( TEndTest, MeshMachine::TStateTransition<TContext>, NetStateMachine::MStateTransition, TContext )
        virtual void DoL();
    DECLARE_SMELEMENT_FOOTER( TEndTest )
    
    DECLARE_SERIALIZABLE_TRANSITION(
        TProgressiveCancel,
        TProgressiveMutex,
        TCancel
        )      
    };


#endif //SYMBIAN_ACTIVITYTEST


