// Copyright (c) 2006-2010 Nokia Corporation and/or its subsidiary(-ies).
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
// Pdp MCPR
// 
//

/**
 @file
 @internalComponent
*/

#ifndef SYMBIAN_PDPMCPR_H
#define SYMBIAN_PDPMCPR_H

#include <etelqos.h>
#include <comms-infras/ss_mcprnodemessages.h>
#include <comms-infras/linkprovision.h>
#include <networking/pppconfig.h>
#include <networking/qos3gpp_subconparams.h>
#include <comms-infras/ss_log.h>

#include <comms-infras/agentmcpr.h>
#include <comms-infras/coremcprstates.h>

#include "PDPProvision.h"
#include "pdpmcprfactory.h"


#if defined __CFLOG_ACTIVE || defined SYMBIAN_TRACE_ENABLE
#define KPdpMCprTag KESockMetaConnectionTag
_LIT8(KPdpMCprSubTag, "pdpmcpr");
#endif

class CPsdAvailabilityListener;

class CPdpMetaConnectionProvider : public CAgentMetaConnectionProvider
/** PPP meta connection provider

@internalTechnology
@released Since 9.4 */
    {
public:
    typedef CPdpMetaConnectionProviderFactory FactoryType;

	static CPdpMetaConnectionProvider* NewL(ESock::CMetaConnectionProviderFactoryBase& aFactory, const ESock::TProviderInfo& aProviderInfo);
	virtual ~CPdpMetaConnectionProvider();

public:
#ifdef SYMBIAN_NETWORKING_CONTENTION_MANAGEMENT
	void ContentionResolved(const Messages::TNodeId& aPendingCprId, TBool aResult);
	void ContentionOccured();
	void ReportContentionAvailabilityStatus(const ESock::TAvailabilityStatus& aStatus) const;
#endif
	
protected:
    CPdpMetaConnectionProvider(ESock::CMetaConnectionProviderFactoryBase& aFactory, const ESock::TProviderInfo& aProviderInfo);
    void ConfigurePDPLayerL();
    void SetAccessPointConfigFromDbL();
    void FillInLCPConfig();
    void FillInAuthConfig();
    void ConstructL();

	void StartAvailabilityMonitoringL(const Messages::TNodeCtxId& aAvailabilityActivity);
	void CancelAvailabilityMonitoring();

protected:
	virtual void ReceivedL(const Messages::TRuntimeCtxId& aSender, const Messages::TNodeId& aRecipient, Messages::TSignatureBase& aMessage);

private:
	CPsdAvailabilityListener* iAvailabilityListener;
    };

namespace PdpMCprErrorRecoveryActivity
{
class CPdpErrorRecoveryActivity : public MeshMachine::CNodeParallelActivityBase
	{
public:
	static MeshMachine::CNodeActivityBase* NewL(const MeshMachine::TNodeActivity& aActivitySig, MeshMachine::AMMNodeBase& aNode);
	~CPdpErrorRecoveryActivity();

protected:
	typedef MeshMachine::TNodeContext<CPdpMetaConnectionProvider, MCprStates::TContext> TContext;

public:

DECLARE_SMELEMENT_HEADER( TAwaitingContentionResult, MeshMachine::TState<TContext>, NetStateMachine::MState, TContext )
	virtual TBool Accept();
DECLARE_SMELEMENT_FOOTER( TAwaitingContentionResult )

DECLARE_SMELEMENT_HEADER( TProcessErrorRecoveryReq, MeshMachine::TStateTransition<TContext>, NetStateMachine::MStateTransition, TContext )
	virtual void DoL();
DECLARE_SMELEMENT_FOOTER( TProcessErrorRecoveryReq )

DECLARE_SMELEMENT_HEADER( TProcessContentionResult, MeshMachine::TStateTransition<TContext>, NetStateMachine::MStateTransition, TContext )
	virtual void DoL();
DECLARE_SMELEMENT_FOOTER( TProcessContentionResult )

DECLARE_SMELEMENT_HEADER( TPdpContentionNoTagOrError, MeshMachine::TStateFork<TContext>, NetStateMachine::MStateFork, TContext  )
	virtual TInt TransitionTag();
DECLARE_SMELEMENT_FOOTER( TPdpContentionNoTagOrError )

public:
	Messages::TNodeId iPendingCprId;
	Messages::TNodeSignal::TMessageId iCprMessageId;
	TUint iCprActivity;
	TInt iErrorCode;

protected:
	CPdpErrorRecoveryActivity(const MeshMachine::TNodeActivity& aActivitySig, MeshMachine::AMMNodeBase& aNode, TUint aActivitiesCount);
	};
}


#endif //SYMBIAN_PDPMCPR_H
