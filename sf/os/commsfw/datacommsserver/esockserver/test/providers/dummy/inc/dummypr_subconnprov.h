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
// DummySCPR_subConnProv.h
// Dummy SubConnection Provider class definition.
// 
//

/**
 @file
 @internalComponent
*/

#if !defined(__DummySCPR_SUBCONNPROV_H__)
#define __DummySCPR_SUBCONNPROV_H__

#include <comms-infras/ss_commsprov.h>
#include <comms-infras/corescpr.h>
#include <comms-infras/corescprstates.h>
#include <comms-infras/ss_corepractivities.h>

class CDummySubConnectionProviderFactory;
class CDummyVanillaSubConnectionProviderFactory;

//-================================================
//
// States and Transitions
//
//-================================================
namespace DummySCPRStates
{
    class TRaiseGranted;
} // namespace DummyCPRStates

namespace DummyExtendedSCPRStates
{
	class TRequestCommsBinderFromCPR;
	class TStoreFlowParams;
	class TRequestCommsBinder;
}



/* We have to have CDummySubConnectionProvider inside the ESock namespace so it can befriend
mcfnode_cast - GCC simply doesn't allow befriending something in another template. See
http://gcc.gnu.org/ml/gcc-prs/2002-10/msg01010.html for Herb Sutter's GOTW column on exactly
this - it contains this discouraging summary:

The reason that I'm writing an article about it is because, alas,
befriending a template in another namespace is easier said than done:

- The Bad News: There are two perfectly good standards-conforming
 ways to do it, and neither one works on all current compilers.

- The Good News: One of the perfectly good standards-conforming ways
 does work on every current compiler I tried except for gcc.

So, since it's only test code & we need it building on GCC, I've hacked around it */
class CDummySubConnectionProvider : public CCoreSubConnectionProvider
/**

@internalComponent
@released since v9.4
 */
    {
#if defined(__X86GCC__) || defined(__GCCXML__)
	// gcc-mingw does not support template function instantiations as friends, only the
	// generic template.
	template<class NODE>
	friend  NODE* mcfnode_cast(Messages::ANode* aNode);
	template<class NODE>
	friend  NODE& mcfnode_cast(Messages::ANode& aNode);
#else
    friend CDummySubConnectionProvider* Messages::mnode_cast<CDummySubConnectionProvider>(ANode* aNode);
    friend CDummySubConnectionProvider& Messages::mnode_cast<CDummySubConnectionProvider>(ANode& aNode);
#endif
    friend class CDummySubConnectionProviderFactory;
    friend class CDummyVanillaSubConnectionProviderFactory;
    friend class DummySCPRStates::TRaiseGranted;

public:
	virtual ~CDummySubConnectionProvider();
    void CDummySubConnectionProvider::SetToIncomingL();

protected:
    static CDummySubConnectionProvider* NewL(ESock::CSubConnectionProviderFactoryBase& aFactory);
    static CDummySubConnectionProvider* NewVanillaL(ESock::CSubConnectionProviderFactoryBase& aFactory);
    
    CDummySubConnectionProvider(ESock::CSubConnectionProviderFactoryBase& aFactory, const MeshMachine::TNodeActivityMap& aActivityMap);

private:
    TBool incomingStatus;
	};


class CDummyExtendedSubConnectionProvider : public CCoreSubConnectionProvider
/**
This dummy is intended to create flow shim based layer beneath it during its own creation
so that CTransportFlowShim is available to CSocket after the flow request has been created.

@internalComponent
@released since v9.5
 */
    {
#if defined(__X86GCC__) || defined(__GCCXML__)
	// gcc-mingw does not support template function instantiations as friends, only the
	// generic template.
	template<class NODE>
	friend  NODE* mcfnode_cast(Messages::ANode* aNode);
#else
    friend CDummyExtendedSubConnectionProvider* Messages::mnode_cast<CDummyExtendedSubConnectionProvider>(ANode* aNode);
#endif
    friend class CDummyExtendedSubConnectionProviderFactory;
    friend class DummyExtendedSCPRStates::TRequestCommsBinderFromCPR;
    friend class DummyExtendedSCPRStates::TStoreFlowParams;
    friend class DummyExtendedSCPRStates::TRequestCommsBinder;

public:
	virtual ~CDummyExtendedSubConnectionProvider();
	void ConstructL();

protected:
    static CDummyExtendedSubConnectionProvider* NewL(ESock::CSubConnectionProviderFactoryBase& aFactory);
    CDummyExtendedSubConnectionProvider(ESock::CSubConnectionProviderFactoryBase& aFactory);

	void Received(MeshMachine::TNodeContextBase& aContext);
    void ReceivedL(const Messages::TRuntimeCtxId& aSender, const Messages::TNodeId& aRecipient, Messages::TSignatureBase& aMessage);

private:
    TBool incomingStatus;
    ESock::TFlowParams iFlowParams;
	};


namespace DummySCPRStates
{
typedef MeshMachine::TNodeContext<CDummySubConnectionProvider, SCprStates::TContext> TContext;

DECLARE_SMELEMENT_HEADER( TRaiseGranted, MeshMachine::TStateTransition<TContext>, NetStateMachine::MStateTransition, DummySCPRStates::TContext )
	virtual void DoL();
DECLARE_SMELEMENT_FOOTER( TRaiseGranted )

DECLARE_SMELEMENT_HEADER( TAwaitingReBindTo, MeshMachine::TState<TContext>, NetStateMachine::MState, TContext )
	virtual TBool Accept();
DECLARE_SMELEMENT_FOOTER( TAwaitingReBindTo )

DECLARE_SMELEMENT_HEADER( TAwaitingBrokenStart, MeshMachine::TState<TContext>, NetStateMachine::MState, TContext )
	virtual TBool Accept();
DECLARE_SMELEMENT_FOOTER( TAwaitingBrokenStart )

DECLARE_SMELEMENT_HEADER( TRebindSelf, MeshMachine::TStateTransition<TContext>, NetStateMachine::MStateTransition, DummySCPRStates::TContext )
    virtual void DoL();
DECLARE_SMELEMENT_FOOTER( TRebindSelf )





struct TWaitForBindToMutex
    {
    static TBool IsBlocked(const MeshMachine::TNodeContextBase& aContext)
        {
        return aContext.iNode.CountActivities(ESock::ECFActivityBindTo) == 0;
        }
    };

struct TWaitForNoDataClients
    {
    static TBool IsBlocked(const MeshMachine::TNodeContextBase& aContext)
        {
        return aContext.iNode.GetFirstClient<Messages::TDefaultClientMatchPolicy>(Messages::TClientType(ESock::TCFClientType::EData)) != NULL;
        }
    };

DECLARE_SERIALIZABLE_STATE(
    TNoTagWaitForBindTo,
    TWaitForBindToMutex,
    MeshMachine::TNoTag
    )

DECLARE_SERIALIZABLE_STATE(
    TNoTagWaitNoDataClients,
    TWaitForNoDataClients,
    MeshMachine::TNoTag
    )



DECLARE_SMELEMENT_HEADER( TCancelPreviousSelfRequest, MeshMachine::TStateTransition<TContext>, NetStateMachine::MStateTransition, DummySCPRStates::TContext )
    virtual void DoL();
DECLARE_SMELEMENT_FOOTER( TCancelPreviousSelfRequest )

class CrazyIdle : public MeshMachine::CNodeRetryActivity
    {
public:
    DECLARE_SMELEMENT_HEADER( TAwaitingCancelled, MeshMachine::TState<TContext>, NetStateMachine::MState, TContext )
        virtual TBool Accept();
    DECLARE_SMELEMENT_FOOTER( TAwaitingCancelled )

    static MeshMachine::CNodeActivityBase* CrazyIdle::NewL( const MeshMachine::TNodeActivity& aActivitySig, MeshMachine::AMMNodeBase& aNode );
protected:
    CrazyIdle( const MeshMachine::TNodeActivity& aActivitySig, MeshMachine::AMMNodeBase& aNode )
    :MeshMachine::CNodeRetryActivity(aActivitySig, aNode) 
     {};
private:
    ~CrazyIdle();
    };

DECLARE_ACTIVITY_MAP(stateMap)
} // namespace DummyCPRStates

namespace DummyExtendedSCPRStates
{
typedef MeshMachine::TNodeContext<CDummyExtendedSubConnectionProvider, SCprStates::TContext> TContext;

DECLARE_SMELEMENT_HEADER( TAwaitingBinderResponse, MeshMachine::TState<TContext>, NetStateMachine::MState, TContext )
	virtual TBool Accept();
DECLARE_SMELEMENT_FOOTER( TAwaitingBinderResponse )

DECLARE_SMELEMENT_HEADER( TRequestCommsBinderFromCPR, MeshMachine::TStateTransition<TContext>, NetStateMachine::MStateTransition, TContext )
	virtual void DoL();
DECLARE_SMELEMENT_FOOTER( TRequestCommsBinderFromCPR )

DECLARE_SMELEMENT_HEADER( TRequestCommsBinder, MeshMachine::TStateTransition<TContext>, NetStateMachine::MStateTransition, TContext )
	virtual void DoL();
DECLARE_SMELEMENT_FOOTER( TRequestCommsBinder )

DECLARE_SMELEMENT_HEADER( TAwaitingBuildStack, MeshMachine::TState<TContext>, NetStateMachine::MState, TContext )
	virtual TBool Accept();
DECLARE_SMELEMENT_FOOTER( TAwaitingBuildStack )

DECLARE_SMELEMENT_HEADER( TSendBuildStackResponse, MeshMachine::TStateTransition<TContext>, NetStateMachine::MStateTransition, TContext )
	virtual void DoL();
DECLARE_SMELEMENT_FOOTER( TSendBuildStackResponse )

DECLARE_SMELEMENT_HEADER( TSendBuildStack, MeshMachine::TStateTransition<TContext>, NetStateMachine::MStateTransition, TContext )
	virtual void DoL();
DECLARE_SMELEMENT_FOOTER( TSendBuildStack )

DECLARE_SMELEMENT_HEADER( TAwaitingBuildStackResponse, MeshMachine::TState<TContext>, NetStateMachine::MState, TContext )
	virtual TBool Accept();
DECLARE_SMELEMENT_FOOTER( TAwaitingBuildStackResponse )

DECLARE_SMELEMENT_HEADER( TSendBindToResponse, MeshMachine::TStateTransition<TContext>, NetStateMachine::MStateTransition, TContext )
	virtual void DoL();
DECLARE_SMELEMENT_FOOTER( TSendBindToResponse )

DECLARE_SMELEMENT_HEADER( TStoreFlowParams, MeshMachine::TStateTransition<TContext>, NetStateMachine::MStateTransition, TContext )
	virtual void DoL();
DECLARE_SMELEMENT_FOOTER( TStoreFlowParams )

DECLARE_SMELEMENT_HEADER( TCreateDataClient, MeshMachine::TStateTransition<TContext>, NetStateMachine::MStateTransition, TContext )
	virtual void DoL();
DECLARE_SMELEMENT_FOOTER( TCreateDataClient )

DECLARE_SMELEMENT_HEADER( TSendBindToComplete, MeshMachine::TStateTransition<TContext>, NetStateMachine::MStateTransition, TContext )
	virtual void DoL();
DECLARE_SMELEMENT_FOOTER( TSendBindToComplete )

DECLARE_ACTIVITY_MAP(stateMap)
}

class CDummyBuildStackActivity : public PRActivities::CCommsBinderActivity
	{
public:
	void StoreFlowParameters(const ESock::RCFParameterFamilyBundleC aFlowParameters)
		{
		if(!iFlowParameters.IsNull())
			{
			iFlowParameters.Close();
			}
		iFlowParameters.Open(aFlowParameters);
		}
	typedef MeshMachine::TNodeContext<CDummyExtendedSubConnectionProvider, SCprStates::TContext> TContext;

	static MeshMachine::CNodeActivityBase* CDummyBuildStackActivity::NewL( const MeshMachine::TNodeActivity& aActivitySig, MeshMachine::AMMNodeBase& aNode );

protected:
	CDummyBuildStackActivity(const MeshMachine::TNodeActivity& aActivitySig, MeshMachine::AMMNodeBase& aNode, TUint aNextActivityCount);

public:
	Messages::TNodeCtxId iBinderSource;

private:
	~CDummyBuildStackActivity();

	ESock::RCFParameterFamilyBundleC iFlowParameters;
	};


#endif	// __DummySCPR_SUBCONNPROV_H__

