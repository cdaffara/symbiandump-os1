// Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// DummyCPR_connProv.h
// TestDummy Connection Provider class definition.
// 
//

/**
 @file
 @internalComponent
*/

#if !defined(__DummyCPR_CONNPROV_H__)
#define __DummyCPR_CONNPROV_H__

#include <comms-infras/mobilitycpr.h>
#include <comms-infras/mobilitycprstates.h>
#include <elements/mm_activities_internal.h> 

#ifdef __FLOG_ACTIVE
// CommsDebugUtility logging tags. Use them to enable tracing for DummyCPR
_LIT8(KTestDummyLogTag1,"TestDummyConnProv");
_LIT8(KTestDummyLogTag2,"TestDummyRef");
#endif

enum TCFNodeActivityId
    {
    ECFActivityDummyCprDestroy                 = ESock::ECFActivityCustom +1,
    };


class CDummyConnectionProvider : public CMobilityConnectionProvider, public ESock::MLegacyControlApiExt,
                                 public ITFHIERARCHY_LINK_1(CDummyConnectionProvider, CMobilityConnectionProvider, ESock::MLegacyControlApiExt)

/**

@internalComponent
@released since v9.4
 */
    {
public:
	typedef ITFHIERARCHY_LINK_1(CDummyConnectionProvider, CMobilityConnectionProvider, ESock::MLegacyControlApiExt) TIfStaticFetcherNearestInHierarchy;

public:
	virtual ~CDummyConnectionProvider();
	virtual TInt ControlL(TUint aOptionLevel, TUint aOptionName, TDes8& aOption, ESock::MPlatsecApiExt* aPlatsecItf);
    void ReturnInterfacePtrL(ESock::MLegacyControlApiExt*& aInterface);

public:
    enum TConnType
         {
         EConnIncoming = 1,
         EConnNoIncoming = 2,
         };

    static CDummyConnectionProvider* NewL(ESock::CConnectionProviderFactoryBase& aFactory, TConnType aConnStatus);
    static CDummyConnectionProvider* NewVanillaL(ESock::CConnectionProviderFactoryBase& aFactory);
    
    CDummyConnectionProvider(ESock::CConnectionProviderFactoryBase& aFactory, const MeshMachine::TNodeActivityMap& aActivityMap, TConnType aConnStatus);
    
public:
	TConnType iConnStatus;
    };


class CDelayTimer: public CTimer
    {
public:
    static CDelayTimer* NewL( const Messages::TNodeId& aSender, const Messages::TNodeId& aRecipient, const Messages::TNodeSignal::TMessageId& aMessageId );

    virtual ~CDelayTimer();
    void Start( TInt aIntervalInSecs );    

public:
class TDelayMessage : public Messages::TSignatureBase
    {
public:
    TDelayMessage();
    TDelayMessage(const TNodeSignal::TMessageId& aMessageId);
    };

private:
    CDelayTimer( const Messages::TNodeId& aSender, const Messages::TNodeId& aRecipient, const Messages::TNodeSignal::TMessageId& aMessageId );
    void ConstructL();
    
protected: // From CTimer (CActive)
    void RunL();
        
private:
    Messages::TNodeId iSender;
    Messages::TNodeId iRecipient;
    Messages::TNodeSignal::TMessageId iMessageId;

public:
    //States and Transitions
    typedef MeshMachine::TNodeContext<ESock::CMMCommsProviderBase, CoreNetStates::TContext> TContext;
    
    template <TInt IntervalMs>
    class TSetTimerMs : public MeshMachine::TStateTransition<TContext>
        {
    public:
        NETSM_TPL_DECLARE_CTR(TSetTimerMs, NetStateMachine::MStateTransition, TContext)

        TSetTimerMs(TContext& aContext)
        :TStateTransition<TContext>(aContext)
            {
            }

        virtual void DoL()
            {
            CDelayTimer* delay = CDelayTimer::NewL(iContext.NodeId(), iContext.NodeId(), Messages::TEBase::TNull::Id());
            delay->Start(IntervalMs);
            }
        };
    
    
    DECLARE_SMELEMENT_HEADER( TAwaitingTimerExpired, MeshMachine::TState<TContext>, NetStateMachine::MState, TContext )
        virtual TBool Accept();
    DECLARE_SMELEMENT_FOOTER( TAwaitingTimerExpired )
    };


//-================================================
//
// States and Transitions
//
//-================================================
namespace DummyCPRStates
{
typedef MeshMachine::TNodeContext<CDummyConnectionProvider, CprStates::TContext> TContext;

DECLARE_SMELEMENT_HEADER( TCreateIncomingSCPR, MeshMachine::TStateTransition<TContext>, NetStateMachine::MStateTransition, DummyCPRStates::TContext )
	virtual void DoL();
DECLARE_SMELEMENT_FOOTER( TCreateIncomingSCPR )

DECLARE_SMELEMENT_HEADER( TSetClientAsIncoming, MeshMachine::TStateTransition<TContext>, NetStateMachine::MStateTransition, DummyCPRStates::TContext )
	virtual void DoL();
DECLARE_SMELEMENT_FOOTER( TSetClientAsIncoming )

DECLARE_SMELEMENT_HEADER( TAwaitingStart, MeshMachine::TState<TContext>, NetStateMachine::MState, DummyCPRStates::TContext )
	virtual TBool Accept();
DECLARE_SMELEMENT_FOOTER( TAwaitingStart )

DECLARE_SMELEMENT_HEADER( TAwaitingDestroy, MeshMachine::TState<TContext>, NetStateMachine::MState, DummyCPRStates::TContext )
    virtual TBool Accept();
DECLARE_SMELEMENT_FOOTER( TAwaitingDestroy )

DECLARE_SMELEMENT_HEADER( TThreeSecDelayAndPostToSelf, MeshMachine::TStateTransition<TContext>, NetStateMachine::MStateTransition, DummyCPRStates::TContext )
    virtual void DoL();
DECLARE_SMELEMENT_FOOTER( TThreeSecDelayAndPostToSelf )

} // namespace DummyCPRStates

#endif // __DummyCPR_CONNPROV_H__

