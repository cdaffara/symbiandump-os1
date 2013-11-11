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

#include <comms-infras/ss_log.h>
#include <comms-infras/upsmcpractivities.h>
#include <comms-infras/upsmessages.h>
#include <comms-infras/ss_metaconnprov.h>
#include <comms-infras/netups.h>


#ifdef _DEBUG
// Panic category for "absolutely impossible!" vanilla ASSERT()-type panics from this module
// (if it could happen through user error then you should give it an explicit, documented, category + code)
_LIT(KSpecAssert_ESockUpsCrPrvMAc, "ESockUpsCrPrvMAc");
#endif

using namespace Messages;
using namespace NetUps;
using namespace ESock;
using namespace NetStateMachine;
using namespace UpsMCprActivities;

//
//
// CUpsAuthorisationActivityBase
//
// Support for UPS.
//

EXPORT_C CUpsAuthorisationActivityBase::CUpsAuthorisationActivityBase(const MeshMachine::TNodeActivity& aActivitySig, MeshMachine::AMMNodeBase& aNode, TUint aNextActivityCount)
  : CNodeParallelActivityBase(aActivitySig, aNode, aNextActivityCount), iRequestId(0)
	{
	}

EXPORT_C CUpsAuthorisationActivityBase::~CUpsAuthorisationActivityBase()
	{
	// do not destroy the iNetUps instance here as we do not own it.
	}

EXPORT_C TInt CUpsAuthorisationActivityBase::GetServiceId(const CCommsProviderBase& aNode, TInt32& aServiceId) const
/**
Get the UPS ServiceId for the current activity.

Called before issuing a request to NetUps.  This default implementation returns the TierId in
the Access Point Config structure as the UPS ServiceId.

@param aNode Node corresponding to this activity.
@param aServiceId Variable populated with ServiceId.
@return KErrNone or a system wide error code

@TODO PREQ1116 - should we make this leaving insteadof returning an error?
*/
	{
	const Meta::SMetaData* const extension = aNode.AccessPointConfig().FindExtension(STypeId::CreateSTypeId(CUPSAccessPointConfigExt::EUPSAccessPointConfigUid, CUPSAccessPointConfigExt::ETypeId));
	// UpsExt returns Service Id not Tier Id in AccessPointConfigExt 
	const CUPSAccessPointConfigExt* const upsExt = static_cast<const CUPSAccessPointConfigExt*>(extension);
	aServiceId = upsExt->GetServiceId().iUid;
	return KErrNone;
	}

EXPORT_DEFINE_SMELEMENT(CUpsAuthorisationActivityBase::TIssueRequestToNetUps, NetStateMachine::MStateTransition, CUpsAuthorisationActivityBase::TContext)

void CUpsAuthorisationActivityBase::TIssueRequestToNetUps::DoL()
/**
Issue an authorisation request to the NetUps.  The NetUps will call back to ProcessPolicyCheckResponse()
with the result.
*/
	{
	const UpsMessage::TPolicyCheckRequest* const msg = message_cast<const UpsMessage::TPolicyCheckRequest>(&iContext.iMessage);
	const CMetaConnectionProviderBase& node = iContext.Node();
	CUpsAuthorisationActivityBase* const activity = static_cast<CUpsAuthorisationActivityBase*>(iContext.Activity());

	// Retrieve the ServiceId to use
	TInt32 serviceId;
	User::LeaveIfError(activity->GetServiceId(node, serviceId));

	const TInt KMaxUpsDestinationString = 60;				// @TODO PREQ1116 - move to header file
	TBuf<KMaxUpsDestinationString> upsDestinationString;

	if (msg)
		{
		// If the incoming message is a TPolicyCheckRequest, then issue the request to the NetUps
		// with arguments taken from the message.
		const TPolicyCheckRequestParams* params = &msg->iParams;		// parameters within TPolicyCheckRequest message

		// Get the human readable destination address string
		User::LeaveIfError(activity->GetUpsDestinationString(params->iDestinationAddrType, params->iDestinationAddr, upsDestinationString));

		// Get the human readable access point name
		const TDesC& accessPointName = activity->GetUpsAccessPointNameL();

		Messages::TNodeId commsId = (params->iCommsId != Messages::TNodeId::NullId()) ?
				params->iCommsId : node.Id();

		activity->PerformPolicyCheckRequestActionsL(commsId);		
		
		// Send request towards NetUps
		TPolicyCheckRequestData requestData(params->iProcessId, params->iThreadId,
							params->iPlatSecResult == KErrNone,
							upsDestinationString,
							accessPointName, commsId, *activity);

		__ASSERT_DEBUG(activity->NetUpsReference() == NULL, User::Panic(KSpecAssert_ESockUpsCrPrvMAc, 1));
		// Allocate NetUps instance.  Note that GetNetUpsL() is virtual and could be overridden by the node.
		activity->SetNetUpsReference(activity->GetNetUpsL(serviceId));
		activity->NetUpsReference()->ProcessPolicyCheckRequestL(requestData, activity->RequestId());
		}
	else
		{
		// If the incoming message is not a TPolicyCheckRequest, then issue the request to the NetUps
		// with arguments extracted from the UPS Access Point Config extension structure.
		const Meta::SMetaData* const extension =
			iContext.Node().AccessPointConfig().FindExtension(STypeId::CreateSTypeId(CUPSAccessPointConfigExt::EUPSAccessPointConfigUid, CUPSAccessPointConfigExt::ETypeId));

	 	if (extension)
			{
			const CUPSAccessPointConfigExt* const upsExt = static_cast<const CUPSAccessPointConfigExt*>(extension);

			// Get the human readable destination address string
			User::LeaveIfError(activity->GetUpsDestinationString(upsExt->GetDestinationAddrType(), upsExt->GetDestinationAddr(), upsDestinationString));

			// Get the human readable access point name
			const TDesC& accessPointName = activity->GetUpsAccessPointNameL();

			// Send request towards NetUps
			TPolicyCheckRequestData requestData(upsExt->GetProcessId(), upsExt->GetThreadId(),
							upsExt->GetPolicyCheckResult() == KErrNone,
							upsDestinationString,
							accessPointName, node.Id(), *activity);

			__ASSERT_DEBUG(activity->NetUpsReference() == NULL, User::Panic(KSpecAssert_ESockUpsCrPrvMAc, 2));
			// Allocate NetUps instance.  Note that GetNetUpsL() is virtual and could be overridden by the node.
			activity->SetNetUpsReference(activity->GetNetUpsL(serviceId));

			activity->PerformPolicyCheckRequestActionsL(node.Id());			
			activity->NetUpsReference()->ProcessPolicyCheckRequestL(requestData, activity->RequestId());
			}
		else
			{
			User::Leave(KErrCorrupt);
			}
		}
	}

EXPORT_DEFINE_SMELEMENT(CUpsAuthorisationActivityBase::TForwardCompletionPolicyCheckResponseToOriginators, NetStateMachine::MStateTransition, CUpsAuthorisationActivityBase::TContext)

void CUpsAuthorisationActivityBase::TForwardCompletionPolicyCheckResponseToOriginators::DoL()
	{
	UpsMessage::TPolicyCheckResponse outMsg(KErrNone);
	
	CUpsAuthorisationActivityBase* const activity = static_cast<CUpsAuthorisationActivityBase*>(iContext.Activity());
	
	activity->PostToOriginators(outMsg);
    activity->SetPostedTo(TNodeId::NullId());
	}

EXPORT_DEFINE_SMELEMENT(CUpsAuthorisationActivityBase::TAwaitingPolicyCheckResponseOrCancel, NetStateMachine::MState, CUpsAuthorisationActivityBase::TContext)

TBool CUpsAuthorisationActivityBase::TAwaitingPolicyCheckResponseOrCancel::Accept()
/**
State that waits for TPolicyCheckResponse or TCancel.

If TCancel is received, then cancel the NetUps request.
*/
	{
	// @TODO PREQ1116 - do we even need CoreNetStates::TAwaitingPolicyCheckResponse anymore ?

	if (iContext.iMessage.IsMessage<UpsMessage::TPolicyCheckResponse>())
		{
		return ETrue;
		}
	else
	if (iContext.iMessage.IsMessage<Messages::TEBase::TCancel>())
		{
		CancelRequest();
		// Clear the message so that MState::Cancel() is not subsequently called.  This is because
		// SetIdle() is called after Cancel(), whereas we must wait for the TPolicyCheckResponse
		// message in response to the cancellation.  Hence CancelRequest() is used instead of overriding
		// Cancel().
		iContext.iMessage.ClearMessageId();
		return EFalse;
		}
	return EFalse;
	}

void CUpsAuthorisationActivityBase::TAwaitingPolicyCheckResponseOrCancel::CancelRequest()
/**
Cancel the outstanding request to the NetUps.
*/
	{
	const CMetaConnectionProviderBase& node = iContext.Node();
	CUpsAuthorisationActivityBase* const activity = static_cast<CUpsAuthorisationActivityBase*>(iContext.Activity());

	__ASSERT_DEBUG(activity->NetUpsReference(), User::Panic(KSpecAssert_ESockUpsCrPrvMAc, 3));
	(void) activity->NetUpsReference()->CancelRequest(node.Id(), activity->RequestId());
	}

EXPORT_C TInt CUpsAuthorisationActivityBase::ProcessPolicyCheckResponse(NetUps::TNetUpsDecision aNetUpsDecision, const TNodeId& aCommsId)
/**
Callback from NetUps with the result of a previous authorisation request.

@param aNetUpsDecision Authorisation decision from UPS
@param aCommsId CommsId passed in the original NetUps request
*/
	{
	PerformPolicyCheckResponseActions(aNetUpsDecision, aCommsId);
	
	const TNodeId& self = iNode.Id();

	const TInt ret = (aNetUpsDecision == EYes || aNetUpsDecision == ESessionYes) ? KErrNone : KErrPermissionDenied;
	// Post a TPolicyCheckResponse message to self only in order to restart execution of the activity
	// (which is assumed to be in a state waiting for a TPolicyCheckResponse).
	iNode.SelfInterface().PostMessage(self, UpsMessage::TPolicyCheckResponse(ret).CRef());
	ResetRequestId();
	return KErrNone;
	}

EXPORT_C TInt CUpsAuthorisationActivityBase::ProcessPolicyCheckResponse(TInt aError, const TNodeId& aCommsId)
/**
Callback from NetUps if an error occurred in the processing of a previous authorisation request (note that this
is not the same as a successfully completed authorisation request that resulted in denial).

@param aError error return from NetUps
*/
	{
	const TNodeId& self = iNode.Id();
	(void) aCommsId;			// for urel
	// Post a TPolicyCheckResponse message to self in order to continue the activity.
	iNode.SelfInterface().PostMessage(self, UpsMessage::TPolicyCheckResponse(aError).CRef());
	ResetRequestId();
	return KErrNone;
	}

EXPORT_DEFINE_SMELEMENT(CUpsAuthorisationActivityBase::TForwardPolicyCheckResponseToOriginators, NetStateMachine::MStateTransition, CUpsAuthorisationActivityBase::TContext)
void CUpsAuthorisationActivityBase::TForwardPolicyCheckResponseToOriginators::DoL()
/**
Forward to our originators the TPolicyCheckResponse that we posted to ourself.
*/
	{
  	CUpsAuthorisationActivityBase* const activity = static_cast<CUpsAuthorisationActivityBase*>(iContext.Activity());

	const UpsMessage::TPolicyCheckResponse& inMsg = message_cast<const UpsMessage::TPolicyCheckResponse>(iContext.iMessage);

	//
	// Errors other than KErrNone and KErrPermissionDenied will result in a TError being
	// sent back to the originator.  KErrNone and KErrPermission denied are taken as
	// authorisation granted/denied rather than a "real" error, and so will result
	// in a TPolicyCheckResponse.  This logic also ensures that KErrCancel from a prior
	// TCancel appropriately results in a TError.
	//
	if (inMsg.iValue != KErrNone && inMsg.iValue != KErrPermissionDenied)
		{
		activity->SetError(inMsg.iValue);
		}
	else
		{
		// (ECFActivityNull because PostToOriginators() sets activity id anyway)
		UpsMessage::TPolicyCheckResponse outMsg(inMsg.iValue);
		activity->PostToOriginators(outMsg);
	    activity->SetPostedTo(TNodeId::NullId());
		}
	}

EXPORT_DEFINE_SMELEMENT(CUpsAuthorisationActivityBase::TNoTagOrError, NetStateMachine::MStateFork, CUpsAuthorisationActivityBase::TContext)
EXPORT_C TInt CUpsAuthorisationActivityBase::TNoTagOrError::TransitionTag()
	{
	// Retrieve error code from TPolicyCheckResponse message posted to self.
	const UpsMessage::TPolicyCheckResponse& msg = message_cast<const UpsMessage::TPolicyCheckResponse>(iContext.iMessage);
	if (msg.iValue == KErrNone)
		{
		return MeshMachine::KNoTag;
		}
	else
		{
		iContext.Activity()->SetError(msg.iValue);
		return MeshMachine::KErrorTag;
		}
	}

EXPORT_DEFINE_SMELEMENT(CUpsAuthorisationActivityBase::TNoTagOrCompletion, NetStateMachine::MStateFork, CUpsAuthorisationActivityBase::TContext)
EXPORT_C TInt CUpsAuthorisationActivityBase::TNoTagOrCompletion::TransitionTag()
	{
	// Determine whether the error code is completion
	const UpsMessage::TPolicyCheckRequest* const msg = message_cast<const UpsMessage::TPolicyCheckRequest>(&iContext.iMessage);

	if (msg->iParams.iPlatSecResult == KErrCompletion)
		{
		return KUpsCompletionTag;
		}
	return MeshMachine::KNoTag;	
	}

EXPORT_C void CUpsAuthorisationActivityBase::PerformPolicyCheckResponseActions(NetUps::TNetUpsDecision aNetUpsDecision, const Messages::TNodeId& aCommsId)
	{
	(void)aNetUpsDecision;
	(void)aCommsId;
	}

EXPORT_C void CUpsAuthorisationActivityBase::PerformPolicyCheckRequestActionsL(const Messages::TNodeId& aCommsId) 
	{
	(void) aCommsId;
	}


