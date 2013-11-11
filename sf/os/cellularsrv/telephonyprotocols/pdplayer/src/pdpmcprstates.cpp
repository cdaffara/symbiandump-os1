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
#include "OstTraceDefinitions.h"
#ifdef OST_TRACE_COMPILER_IN_USE
#include "pdpmcprstatesTraces.h"
#endif

#include <etelqos.h>
#include <comms-infras/ss_metaconnprov.h>
#include <comms-infras/ss_nodeinterfaces.h>
#include <elements/nm_messages_errorrecovery.h>
#include <comms-infras/ss_mcprnodemessages.h>
#include "pdpmcpr.h"
#include "pdpmcprstates.h"

using namespace ESock;
using namespace NetStateMachine;
using namespace Messages;

namespace PdpMCprStates
{
}

namespace PdpMCprErrorRecoveryActivity
{

DEFINE_SMELEMENT(CPdpErrorRecoveryActivity::TAwaitingContentionResult, NetStateMachine::MState, PdpMCprStates::TContext)
TBool CPdpErrorRecoveryActivity::TAwaitingContentionResult::Accept()
	{
	TPDPMessages::TPdpContentionResultMessage* msg = message_cast<TPDPMessages::TPdpContentionResultMessage>(&iContext.iMessage);
	CPdpErrorRecoveryActivity& activity = static_cast<CPdpErrorRecoveryActivity&>(*iContext.iNodeActivity);
	if(msg && msg->iNodeId == activity.iPendingCprId)
		{
		return ETrue;
		}
	return EFalse;
	}


DEFINE_SMELEMENT(CPdpErrorRecoveryActivity::TPdpContentionNoTagOrError, NetStateMachine::MStateFork, PdpMCprStates::TContext)
TInt CPdpErrorRecoveryActivity::TPdpContentionNoTagOrError::TransitionTag()
	{
	CPdpErrorRecoveryActivity& activity = static_cast<CPdpErrorRecoveryActivity&>(*iContext.iNodeActivity);

	if (activity.iPendingCprId == Messages::TNodeId::NullId())
		{
		return MeshMachine::KErrorTag;
		}
	else
		{
		return MeshMachine::KNoTag;
		}
	}

DEFINE_SMELEMENT(CPdpErrorRecoveryActivity::TProcessErrorRecoveryReq, NetStateMachine::MStateTransition, PdpMCprStates::TContext)
void CPdpErrorRecoveryActivity::TProcessErrorRecoveryReq::DoL()
	{
	TEErrorRecovery::TErrorRecoveryRequest& req = message_cast<TEErrorRecovery::TErrorRecoveryRequest>(iContext.iMessage);
	TInt error = req.iErrContext.iStateChange.iError;

	OstTraceDefExt2(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CPDPERRORRECOVERYACTIVITY_TPROCESSERRORRECOVERYREQ_DOL_1, "Error Recovery Req. Message Id [%d] Error Id [%d]", req.iErrContext.iMessageId.MessageId(), req.iErrContext.iStateChange.iError);

#ifdef SYMBIAN_NETWORKING_CONTENTION_MANAGEMENT
	if (req.iErrContext.iStateChange.iError == KErrUmtsMaxNumOfContextExceededByNetwork ||
	    req.iErrContext.iStateChange.iError == KErrUmtsMaxNumOfContextExceededByPhone)
		{
		// Get sender's priority
		TClientIter<TDefaultClientMatchPolicy> iter = iContext.Node().GetClientIter<TDefaultClientMatchPolicy>(TCFClientType(TCFClientType::EData));
		ESock::RCFNodePriorityInterface* dataClient;
		TBool wasFound = EFalse;
		TUint cprPriority = KMaxTUint;
		while ((dataClient = static_cast<RCFNodePriorityInterface*>(iter++)) != NULL)
			{
			if (dataClient->RecipientId() == req.iErrContext.iOriginator)
				{
				cprPriority = dataClient->Priority();
				wasFound = ETrue;
				}
			}
		if (wasFound)
			{
			TInt err = KErrNone;
			TBool result = EFalse;
			TRAP(err, result = iContext.Node().GetTierManager()->HandleContentionL(&iContext.Node(), req.iErrContext.iOriginator, cprPriority));
			if (err == KErrNone && result)
				{
				CPdpErrorRecoveryActivity& activity = static_cast<CPdpErrorRecoveryActivity&>(*iContext.iNodeActivity);
				activity.iPendingCprId = req.iErrContext.iOriginator;
				activity.iCprActivity = req.iErrContext.iActivitySigId;
				activity.iCprMessageId = req.iErrContext.iMessageId;
				activity.iErrorCode = error;
				// The contention request has been added into query, Reset message error code.
				// TErrResponse will be send from Mcpr's ContentionResolved function.
				error = KErrNone;
				}
			}
		}
#endif

	if (error != KErrNone)
		{
		TErrResponse propagateResp(TErrResponse::EPropagate, error, req.iErrContext.iMessageId);
		TEErrorRecovery::TErrorRecoveryResponse rawResp	(propagateResp);

		TCFSafeMessage::TResponseCarrierWest<TEErrorRecovery::TErrorRecoveryResponse> resp(rawResp, iContext.iSender);
		iContext.PostToSender(resp);
		}
	}

DEFINE_SMELEMENT(CPdpErrorRecoveryActivity::TProcessContentionResult, NetStateMachine::MStateTransition, PdpMCprStates::TContext)
void CPdpErrorRecoveryActivity::TProcessContentionResult::DoL()
	{
	TPDPMessages::TPdpContentionResultMessage* msg = message_cast<TPDPMessages::TPdpContentionResultMessage>(&iContext.iMessage);
	CPdpErrorRecoveryActivity& activity = static_cast<CPdpErrorRecoveryActivity&>(*iContext.iNodeActivity);
	OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CPDPERRORRECOVERYACTIVITY_TPROCESSCONTENTIONRESULT_DOL_1, "Contention Result Code [%d]", msg->iValue);
	if (msg->iValue != 0)
		{
		TErrResponse retryResp(TErrResponse::ERetry, 0, activity.iCprMessageId);
		TEErrorRecovery::TErrorRecoveryResponse rawResp	(retryResp);
		TCFSafeMessage::TResponseCarrierWest<TEErrorRecovery::TErrorRecoveryResponse> resp(rawResp, activity.iOriginators[0].RecipientId());
		iContext.PostToSender(resp);
		}
	else
		{
		TErrResponse propagateResp(TErrResponse::EPropagate, activity.iErrorCode, activity.iCprMessageId);
		TEErrorRecovery::TErrorRecoveryResponse rawResp	(propagateResp);
		TCFSafeMessage::TResponseCarrierWest<TEErrorRecovery::TErrorRecoveryResponse> resp(rawResp, activity.iOriginators[0].RecipientId());
		iContext.PostToSender(resp);
		}
	}
}

