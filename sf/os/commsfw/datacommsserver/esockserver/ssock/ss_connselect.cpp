// Copyright (c) 1997-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// SS_CONN.CPP
// 
//
#include "ss_connselect.h"

#include <comms-infras/ss_log.h>
#include "SS_conn.H"
#include <comms-infras/esock_params_internal.h>
#include <comms-infras/ss_nodemessages_selector.h>

#include <elements/nm_messages_base.h>
#include <elements/nm_messages_peer.h>

#include <comms-infras/ss_nodemessages_dataclient.h>
#include <comms-infras/ss_nodemessages_factory.h>
#include <comms-infras/ss_nodemessages_internal.h>


#ifdef _DEBUG
// Panic category for "absolutely impossible!" vanilla ASSERT()-type panics from this module
// (if it could happen through user error then you should give it an explicit, documented, category + code)
_LIT(KSpecAssert_ESockSSockscnslc, "ESockSSockscnslc");
#endif

using namespace ESock;
using namespace Messages;
using namespace MeshMachine;

//
//CSelectionRequest
CSelectionRequest* CSelectionRequest::NewL(const TRuntimeCtxId& aRequestingClient, const TNodeId& aTierManagerId)
	{
	CSelectionRequest* self = new (ELeave) CSelectionRequest(aRequestingClient,aTierManagerId);

	NM_LOG_START_BLOCK(KESockMetaConnectionTag(), _L8("CSelectionRequest::NewL"));
	NM_LOG((KESockMetaConnectionTag(), _L8("[this=0x%08x]"), self));
	NM_LOG_ADDRESS_EXT(KESockMetaConnectionTag(), aRequestingClient, _L8("Client:"));
	NM_LOG_ADDRESS_EXT(KESockMetaConnectionTag(), aTierManagerId, _L8("TierMgr:"));
	NM_LOG_END_BLOCK(KESockMetaConnectionTag(), KNullDesC8());
	return self;
	}

CSelectionRequest::CSelectionRequest(const TRuntimeCtxId& aRequestingClient, const TNodeId& aTierManagerId)
:	Messages::ASimpleNodeIdBase(),
	TIfStaticFetcherNearestInHierarchy(this),
	iTierManagerId(aTierManagerId),
	iSelectionStatus(EIdle),
	iPlatsecApiExt(NULL)
	{
	LOG_NODE_CREATE(KESockMetaConnectionTag, CSelectionRequest);
	iRequestingNode.Open(address_cast<TNodeId>(aRequestingClient));
	iRequest.Open(iRequestingNode, aRequestingClient);
	}

CSelectionRequest::~CSelectionRequest()
	{
	if (iPlatsecApiExt != NULL)
		delete iPlatsecApiExt;
		

	iRequestingNode.Close();

	//Properly handled CSR should either complete the request or be cancelled before being destroyed.
	//If the requesting client wants to cancel its request with this CSR, it sends a cancel message.
	//The cancel message is processed (all active requests are being cancelled) and when all complete
	//with error (or select complete) messages, then the error is being send to the requesting client.
	//The requesting client must wait for this error (confirmation to its cancel message) and then
	//it can destroy this CSR, not earlier.
	//This is why we must make sure that iActiveRequests is empty.
	//If this ASSERT fires in your case, make sure that the requesting client obeys the above mentioned
	//protocol.
	__ASSERT_DEBUG(iActiveRequests.Count()==0, User::Panic(KSpecAssert_ESockSSockscnslc, 1));
	iActiveRequests.Close();

	//The client decided to destroy us.
	//Regardless if we received or didn't receive TSelect, iTopMcprId must be empty now.
	__ASSERT_DEBUG(iTopMcprId.IsNull(), User::Panic(KSpecAssert_ESockSSockscnslc, 2));
	LOG_NODE_DESTROY(KESockMetaConnectionTag, CSelectionRequest);
	}

//The entry point to the selector. It may be replaced by the mesh machine in the future.
void CSelectionRequest::ReceivedL(const TRuntimeCtxId& aSender, const TNodeId& /*aRecipient*/, TSignatureBase& aCFMessage)
    {
	if (TEBase::ERealmId == aCFMessage.MessageId().Realm())
		{
		switch (aCFMessage.MessageId().MessageId())
			{
		case TEBase::TCancel::EId :
			__ASSERT_DEBUG(iRequestingNode == aSender, User::Panic(KSpecAssert_ESockSSockscnslc, 3));
			Cancel();
			break;
		case TEBase::TError::EId:
			{
			TEBase::TError& error = message_cast<TEBase::TError>(aCFMessage);
			if (iSelectionStatus==ERequestingCommsBinder)
			    {
			    CommsBinderRequestError(aSender,error);
			    }
			else if(error.iMsgId == TCFDataClient::TBindToComplete::Id())
			    {
			    if (KErrNone==iOriginalError)
			        {
			        iOriginalError = error.iValue;
			        }
	            // Send TCFServiceProvider::TBindToComplete to complete BinderRequest
	            __ASSERT_DEBUG(!iTopMcprId.IsNull(), User::Panic(KSpecAssert_ESockSSockscnslc, 43)); //iTopMcprId must be selected by now!
	            RNodeInterface::OpenPostMessageClose(Id(), iTopMcprId, TCFServiceProvider::TBindToComplete(error.iValue).CRef());
			    }
			else
			    {
				if (error.iMsgId == TCFSelector::TSimpleSelect::Id()
					|| error.iMsgId == TCFSelector::TSelect::Id())
					{
					//TSelect may be used by CConnection or the TopMcpr
					//In both cases the control client must leave on an error.
					TInt idx = FindActiveRequest(aSender);
					if (idx != KErrNotFound && !(iActiveRequests[idx].Flags() & TClientType::ELeaving))
						{
						RNodeInterface::OpenPostMessageClose(Id(), aSender, TEChild::TLeft().CRef());
						iActiveRequests[idx].SetFlags(TClientType::ELeaving);
	                    if (aSender == iTopMcprId)
	                        {
	                        iTopMcprId.SetNull();
	                        }						
						}
					}
				SelectionError(aSender, error.iValue);
				}
			}
			break;
		default:
//TODO - logging
			NM_LOG_START_BLOCK(KESockMetaConnectionTag(), _L8("CSelectionRequest:ReceivedL"));
			NM_LOG((KESockMetaConnectionTag(), _L8("[this=0x%08x] ERROR: KErrNotSupported "), this));
			NM_LOG_MESSAGE(KESockMetaConnectionTag(), aCFMessage);
			NM_LOG_END_BLOCK(KESockMetaConnectionTag(), KNullDesC8());
	    		__ASSERT_DEBUG(iRequestingNode==aSender, User::Panic(KSpecAssert_ESockSSockscnslc, 4));

			__ASSERT_DEBUG(EFalse, User::Panic(KSpecAssert_ESockSSockscnslc, 5)); //For debug configurations
			User::Leave(KErrNotSupported); //For release configurations
			}
		}
	else if ( aCFMessage.IsMessage<TEChild::TDestroy>() )
		{
		__ASSERT_DEBUG(iRequestingNode==aSender, User::Panic(KSpecAssert_ESockSSockscnslc, 6));
		InitialiseDestroy();
		}
	else if (TEPeer::ERealmId == aCFMessage.MessageId().Realm())
		{
		switch (aCFMessage.MessageId().MessageId())
			{
		case TEPeer::TLeaveComplete::EId:
			__ASSERT_DEBUG(aSender==iTopMcprId || iTopMcprId.IsNull(), User::Panic(KSpecAssert_ESockSSockscnslc, 7));
			iTopMcprId.SetNull();
			delete this;
			break;
		default:
//TODO - logging
			NM_LOG_START_BLOCK(KESockMetaConnectionTag(), _L8("CSelectionRequest:ReceivedL"));
			NM_LOG((KESockMetaConnectionTag(), _L8("[this=0x%08x] ERROR: KErrNotSupported "), this));
			NM_LOG_MESSAGE(KESockMetaConnectionTag(), aCFMessage);
			NM_LOG_END_BLOCK(KESockMetaConnectionTag(), KNullDesC8());

			__ASSERT_DEBUG(EFalse, User::Panic(KSpecAssert_ESockSSockscnslc, 8)); //For debug configurations
			User::Leave(KErrNotSupported); //For release configurations
			}
		}
	else if (TCFSelector::ERealmId == aCFMessage.MessageId().Realm())
		{
		switch (aCFMessage.MessageId().MessageId())
			{
    	case TCFSelector::TSelect::EId:
    		__ASSERT_DEBUG(aSender == iRequestingNode.RecipientId(), User::Panic(KSpecAssert_ESockSSockscnslc, 9));
    		SelectConnPrefList(message_cast<const TCFSelector::TSelect>(aCFMessage).iConnPrefList);
    		break;

		case TCFSelector::TSimpleSelect::EId:
			__ASSERT_DEBUG(iRequestingNode == aSender, User::Panic(KSpecAssert_ESockSSockscnslc, 10));
			iRequest.Open(iRequestingNode, aSender);
			Select(message_cast<const TCFSelector::TSimpleSelect>(aCFMessage).iSelectionPrefs);
			break;
		case TCFSelector::TSelectComplete::EId:
			{
			TCFSelector::TSelectComplete& msg = message_cast<TCFSelector::TSelectComplete>(aCFMessage);
			SelectComplete(address_cast<TNodeId>(aSender), msg.iNodeId, msg.iProviderInfo);
			}
			break;
		default:
//TODO - logging
			NM_LOG_START_BLOCK(KESockMetaConnectionTag(), _L8("CSelectionRequest:ReceivedL"));
			NM_LOG((KESockMetaConnectionTag(), _L8("[this=0x%08x] ERROR: KErrNotSupported "), this));
			NM_LOG_MESSAGE(KESockMetaConnectionTag(), aCFMessage);
			NM_LOG_END_BLOCK(KESockMetaConnectionTag(), KNullDesC8());

			__ASSERT_DEBUG(EFalse, User::Panic(KSpecAssert_ESockSSockscnslc, 11)); //For debug configurations
			User::Leave(KErrNotSupported); //For release configurations
			}
		}
	else if ( aCFMessage.IsMessage<TCFServiceProvider::TCommsBinderResponse>() )
		{
		CommsBinderResponse(message_cast<TCFServiceProvider::TCommsBinderResponse>(aCFMessage));
		}
	else if ( aCFMessage.IsMessage<TCFDataClient::TBindToComplete>() )
		{
		__ASSERT_DEBUG(iRequestingNode==aSender, User::Panic(KSpecAssert_ESockSSockscnslc, 12));
		__ASSERT_DEBUG(iSelectionStatus==ERequestingCommsBinder || iSelectionStatus==EIdle, User::Panic(KSpecAssert_ESockSSockscnslc, 13));
		__ASSERT_DEBUG(!iTopMcprId.IsNull(), User::Panic(KSpecAssert_ESockSSockscnslc, 14)); //iTopMcprId must be selected by now!

		// Notify the service provider to complete binding activity (TCFServiceProvider::TCommsBinderRequest)
		RNodeInterface::OpenPostMessageClose(Id(), iTopMcprId, TCFServiceProvider::TBindToComplete().CRef());

		}
	else if ( aCFMessage.IsMessage<TCFMessage::TStateChange>() )
		{
		if (!iDestroying)
			{
			// When destroying prevent the status change being posted to
			// the Implicit Flow Request which may already have been deleted.
			iRequestingNode.PostMessage(Id(), aCFMessage);
			}
		}
	else if ( aCFMessage.IsMessage<TCFMessage::TProvisionConnectionInfo>() )
		{
		__ASSERT_DEBUG(iRequestingNode == aSender, User::Panic(KSpecAssert_ESockSSockscnslc, 15));
		Provision(message_cast<const TCFMessage::TProvisionConnectionInfo>(aCFMessage).iPtr);
		}
	else if ( aCFMessage.IsMessage<TCFPeer::TJoinComplete>() )
		{
		JoinComplete(address_cast<TNodeId>(aSender), message_cast<TCFPeer::TJoinComplete>(aCFMessage));
		}
	else
		{
//TODO - logging
		NM_LOG_START_BLOCK(KESockMetaConnectionTag(), _L8("CSelectionRequest:ReceivedL"));
		NM_LOG((KESockMetaConnectionTag(), _L8("[this=0x%08x] ERROR: KErrNotSupported "), this));
		NM_LOG_MESSAGE(KESockMetaConnectionTag(), aCFMessage);
		NM_LOG_END_BLOCK(KESockMetaConnectionTag(), KNullDesC8());

		__ASSERT_DEBUG(EFalse, User::Panic(KSpecAssert_ESockSSockscnslc, 16)); //For debug configurations
		User::Leave(KErrNotSupported); //For release configurations
		}
	}

void CSelectionRequest::InitialiseDestroy()
	{
	__ASSERT_DEBUG(!iDestroying, User::Panic(KSpecAssert_ESockSSockscnslc, 17));
	iDestroying = ETrue;

    if (iActiveRequests.Count()!=0)
    	{
      /*
      Unfortunatelly today the parent of 'this' (CConnection or CFlowRequest) doesn't handle ownership
      with 'this' very well and it may (in cancelation scenarios it will) bail out right after posting TDestroy
      to 'this'. This is not good because the TSelect protocol carries data owned by the parent (selection preferences)
      and MCPRs may attempt to access them after the owner/parent has gone away. We're making it illegal for the
      parent to post TDestroy before all requests are completed. Consequently before posting TDestroy the parent 
      must make sure the selection is complete by awaiting response to the TSelect protocol. If it wants to
      terminate selection it _must_ send TCancel (and await responses) before posting TDestroy.
      */
        __ASSERT_DEBUG(EFalse, User::Panic(KSpecAssert_ESockSSockscnslc, 44)); 
    	return;
    	}

	//We have never joined the top mcpr, clear it now
	if (!iJoined)
		{
		iTopMcprId.SetNull();
		}

	//We are ready but we have to leave the iTopMcprId first
	if (!iTopMcprId.IsNull())
    	{
    	RNodeInterface::OpenPostMessageClose(Id(), iTopMcprId, TEPeer::TLeaveRequest().CRef());
		return;
    	}

   	//We are ready to be deleted
   	delete this;
   	return;
	}

//Selection requests start here.
void CSelectionRequest::Select(const TSelectionPrefs& aSelectionPreferences)
	{
    NM_LOG_START_BLOCK(KESockMetaConnectionTag(), _L8("CSelectionRequest::Select"));
	NM_LOG_ADDRESS_EXT(KESockMetaConnectionTag(), iRequestingNode.RecipientId(), _L8("Client:"));
	NM_LOG((KESockMetaConnectionTag(), _L8("[this=0x%08x]"), this));
    NM_LOG_STMT(TUint selectionScope = aSelectionPreferences.Scope());
	NM_LOG((KESockMetaConnectionTag(), _L8("%s"), (selectionScope & TSelectionPrefs::EExplicitConnection)?_S8("EExplicitConnection"):_S8("SelectTopProviderOnly")));

	LOG( ESockLog::Printf(KESockMetaConnectionTag, _L8("CSelectionRequest %08x:\tSelect: %s."), this, (selectionScope&TSelectionPrefs::ESelectFromExisting)?_S8("ESelectFromExisting"):_S8("SelectAndCreate")));
	LOG( ESockLog::Printf(KESockMetaConnectionTag, _L8("CSelectionRequest %08x:\tSelect: %s."), this, (selectionScope&TSelectionPrefs::ERequestCommsBinder)?_S8("ERequestCommsBinder"):_S8("DoNotRequestCommsBinder")));

    NM_LOG_END_BLOCK(KESockMetaConnectionTag(), KNullDesC8());

	//The requesting client may only request selection once.
	//If this assert fires in your case, please make sure the requesting client obeys the protocol.
	__ASSERT_DEBUG(iSelectionStatus==EIdle, User::Panic(KSpecAssert_ESockSSockscnslc, 18));
	__ASSERT_DEBUG(iTopMcprId.IsNull(), User::Panic(KSpecAssert_ESockSSockscnslc, 19));
	iSelectionStatus = ESelecting;
	iSelectionPreferences.Copy(aSelectionPreferences);
	__ASSERT_DEBUG(iActiveRequests.Count()==0, User::Panic(KSpecAssert_ESockSSockscnslc, 20));
	if (KErrNone==iActiveRequests.Append(RNodeInterface()))
		{
		RNodeInterface& tierManager = iActiveRequests[0];
		tierManager.Open(iTierManagerId);
		tierManager.PostMessage(Id(), TCFPeer::TJoinRequest(Id(), TClientType(TCFClientType::ECtrl)).CRef());
		}
	else
		{
		//We have an OOM condition here. Since we have just started serving this request,
		//we must report this error to the requesting client (see SelectError()).
		SelectionError(Id(), KErrNoMemory);
		}
	}

void CSelectionRequest::SelectConnPrefList(const RConnPrefList& aConnPrefList)
	{
	//The requesting client may only request selection once.
	//If this assert fires in your case, please make sure the requesting client obeys the protocol.
	__ASSERT_DEBUG(iSelectionStatus == EIdle, User::Panic(KSpecAssert_ESockSSockscnslc, 21));
	__ASSERT_DEBUG(iTopMcprId == TNodeId::NullId(), User::Panic(KSpecAssert_ESockSSockscnslc, 22));
	iSelectionStatus = ESelecting;
	iConnPrefList = aConnPrefList;

	ESock::RConnPrefList::TIter<TConnCSRPref> iterCSR = iConnPrefList.getIter<TConnCSRPref>();
	//There should be one and only one CSR pref
	__ASSERT_DEBUG(iterCSR[0] != NULL || iterCSR[1] == NULL , User::Panic(KSpecAssert_ESockSSockscnslc, 23));
	TConnCSRPref* pref = iterCSR[0];
	iSelectionPreferences.SetScope(pref->Scope());
	iSelectionPreferences.SetFlags(pref->Flags());
	iSelectionPreferences.SetSubSessionUniqueId(pref->Scope());

	__ASSERT_DEBUG(iActiveRequests.Count()==0, User::Panic(KSpecAssert_ESockSSockscnslc, 24));
	if (KErrNone==iActiveRequests.Append(RNodeInterface()))
		{
		RNodeInterface& tierManager = iActiveRequests[iActiveRequests.Count()-1];
		tierManager.Open(iTierManagerId);
		tierManager.PostMessage(
			Id(),
			TCFPeer::TJoinRequest(Id(), TClientType(TCFClientType::ECtrl)).CRef()
			);
		}
	else
		{
		//We have an OOM condition here. Since we have just started serving this request,
		//we must report this error to the requesting client (see SelectError()).
		SelectionError(Id(), KErrNoMemory);
		}
	}

void CSelectionRequest::Provision(const Meta::SMetaData* aProvisionConfig)
	{
/*	if (!aProvisionConfig->IsTypeOf(STypeId::CreateSTypeId(CConnectionInfo::EUid, CConnectionInfo::ETypeId)))
	return;*/
	iProvision = aProvisionConfig;
	}

void CSelectionRequest::Cancel()
	{
	NM_LOG_START_BLOCK(KESockMetaConnectionTag(), _L8("CSelectionRequest::Cancel"));
	NM_LOG((KESockMetaConnectionTag, _L8("[this=0x%08x]"), this));
	NM_LOG_ADDRESS_EXT(KESockMetaConnectionTag(), iRequestingNode.RecipientId(), _L8("Client:"));
	NM_LOG_END_BLOCK(KESockMetaConnectionTag(), KNullDesC8());

	//Is the requesting client trying to cancel its request again?
	__ASSERT_DEBUG(iSelectionStatus!=ECancelling, User::Panic(KSpecAssert_ESockSSockscnslc, 25));
	if (iSelectionStatus!=EIdle)
		{
		iSelectionStatus = ECancelling;
		iOriginalError = KErrCancel;

		TEBase::TCancel cancel;
		for (TInt i = iActiveRequests.Count()-1; i >= 0; i--)
			{
			//Send cancel and wait for error messages before destructing this object.
			iActiveRequests[i].PostMessage(Id(), cancel);
			}
		//We have not completed the original request yet, do not send
		//an error message to the requesting client from here.
		//When the last error (or select complete) message has been returned
		//from the last MCpr we have cancelled, then we send our cancel's
		//confirmation from SelectionFinished().
		}
	}

void CSelectionRequest::JoinComplete(const TNodeId& aSender, TCFPeer::TJoinComplete& /*aCFMessage*/)
	{
	if (KErrNone==iOriginalError)
		{
		if (iTopMcprId.IsNull())
			{
			__ASSERT_DEBUG(iActiveRequests[0].RecipientId()==aSender && iTierManagerId==aSender, User::Panic(KSpecAssert_ESockSSockscnslc, 26)); //It has surely been sent by the tier manager.

			if(iConnPrefList.Count() < 1)
				{
				iActiveRequests[0].PostMessage(Id(), TCFSelector::TSimpleSelect(iSelectionPreferences).CRef());
				}
			else
				{
				iActiveRequests[0].PostMessage(Id(), TCFSelector::TSelect(iConnPrefList).CRef());
				}
			}
		else
			{
			if (aSender==iTopMcprId)
				{
				iJoined = ETrue;
				
				TUint selectionScope = iSelectionPreferences.Scope();
				if (!(selectionScope & TSelectionPrefs::EExplicitConnection))
					{
					return;
					}
				}

			if(iConnPrefList.Count() < 1)
				{
				RNodeInterface::OpenPostMessageClose(Id(), aSender, TCFSelector::TSimpleSelect(iSelectionPreferences).CRef());
				}
			else
				{
				RNodeInterface::OpenPostMessageClose(
					Id(),
					aSender,
					TCFSelector::TSelect(iConnPrefList).CRef()
					);
				}
			}
		}
	else
		{
		TInt idx = FindActiveRequest(aSender);
		if (idx != KErrNotFound && !(iActiveRequests[idx].Flags() & TClientType::ELeaving))
			{
			RNodeInterface::OpenPostMessageClose(Id(), aSender, TEChild::TLeft().CRef());
			iActiveRequests[idx].SetFlags(TClientType::ELeaving);
			}
		
		ProviderSelectionFinished(aSender);
		
		if (iActiveRequests.Count()==0)
			{ //The active list is empty which means that we have finished serving the selection request.
			SelectionFinished();
			}
		}
	}

void CSelectionRequest::CommsBinderResponse(const TCFServiceProvider::TCommsBinderResponse& aMsg)
	{
	iRequest.ReplyTo(Id(), TCFDataClient::TBindTo(aMsg.iNodeId).CRef());
	iSelectionStatus = EIdle;
	}

//This fn is called when a provider has been successfuly selected.
void CSelectionRequest::SelectComplete(const TNodeId& aSenderId, const TNodeId& aMcprId, const TProviderInfo& aMcprInfo)
	{
	NM_LOG_START_BLOCK(KESockMetaConnectionTag(), _L8("CSelectionRequest::SelectComplete"));
	NM_LOG((KESockMetaConnectionTag(), _L8("[this=0x%08x]"), this));
	NM_LOG_ADDRESS(KESockMetaConnectionTag(), aSenderId);
	NM_LOG_ADDRESS(KESockMetaConnectionTag(), aMcprId);
	NM_LOG_END_BLOCK(KESockMetaConnectionTag(), KNullDesC8());

	__ASSERT_DEBUG(!aSenderId.IsNull(), User::Panic(KSpecAssert_ESockSSockscnslc, 27)); //The sender must exist and be in the active list

	//The sender must be in the active list (or it is the tier manager)
	__ASSERT_DEBUG(KErrNotFound!=FindActiveRequest(aSenderId), User::Panic(KSpecAssert_ESockSSockscnslc, 28));

	if (aMcprId.IsNull())
		{ //Last SelectComplete from aSenderId. Remove from the active list.
		ProviderSelectionFinished(aSenderId);
		}
	else
		{
		HandleProviderSelection(aMcprId);
		}

	if (iTopMcprId.IsNull())
		{
		//Top provider has been selected (it must not be null since we have received SelectComplete and not Error).

    	NM_LOG_ADDRESS_EXT(KESockMetaConnectionTag, aMcprId, _L8("Top provider selected: "));

		//We should have never reveive SelectComplete(NULL) as the only select complete.
		//If even one provider cannot be selected we should have received an error message!
		__ASSERT_DEBUG(!aMcprId.IsNull(), User::Panic(KSpecAssert_ESockSSockscnslc, 30));
		__ASSERT_DEBUG(iActiveRequests[0].RecipientId()==aSenderId, User::Panic(KSpecAssert_ESockSSockscnslc, 29)); //It has surely been sent by the tier manager.
		iTopMcprId = aMcprId;
		iTopMcprInfo = aMcprInfo;
		}
	else if (iActiveRequests.Count()==0)
		{ //The active list is empty which means that we have finished serving the selection request.
		SelectionFinished();
		}
	}

//This fn is called when we receive or generate an error during selection.
void CSelectionRequest::SelectionError(const TRuntimeCtxId& aSenderId, TInt aError)
	{
	//Remember the error because for legacy selection we need to use it
	if (KErrNone==iOriginalError)
		{
		iOriginalError = aError;
		}

	NM_LOG_START_BLOCK(KESockMetaConnectionTag(), _L8("CSelectionRequest::SelectionError"));
	NM_LOG((KESockMetaConnectionTag(), _L8("[this=0x%08x] [aError=%d]"), this, aError));
	NM_LOG_ADDRESS_EXT(KESockMetaConnectionTag(), iRequestingNode.RecipientId(), _L8("Client:"));
	NM_LOG_END_BLOCK(KESockMetaConnectionTag(), KNullDesC8());

	__ASSERT_DEBUG(!aSenderId.IsNull(), User::Panic(KSpecAssert_ESockSSockscnslc, 31)); //The sender must exist and be in the active list
	__ASSERT_DEBUG(iSelectionStatus!=ERequestingCommsBinder, User::Panic(KSpecAssert_ESockSSockscnslc, 32));

	TInt idx = FindActiveRequest(aSenderId);
	if(KErrNotFound != idx)
		{
		//Rather than leaving the service provider (sender) that has send as the error message,
		//we are removing it from our list. Any further comunication with the node could result
		//in further errors.
		iActiveRequests[idx].Close();
		iActiveRequests.Remove(idx);
		}

	//If the active list is empty, we have unsuccessfuly finished the whole selection.
	if (iActiveRequests.Count()==0)
		{
		SelectionFinished();
		}
	}

//This fn is called when we receive an error instead of CommsBinder.
void CSelectionRequest::CommsBinderRequestError(const TRuntimeCtxId& aSenderId, TEBase::TError& aCFMessage)
	{
	// Preventing unused variable warnings.
	#ifndef _DEBUG
		(void)aSenderId;
	#endif

	//Remember the error because for legacy selection we need to use it
	if (KErrNone==iOriginalError)
		{
		iOriginalError = aCFMessage.iValue;
		}

	NM_LOG_START_BLOCK(KESockMetaConnectionTag(), _L8("CSelectionRequest::CommsBinderRequestError"));
	NM_LOG((KESockMetaConnectionTag(), _L8("[this=0x%08x]"), this));
	NM_LOG_ADDRESS_EXT(KESockMetaConnectionTag(), iRequestingNode.RecipientId(), _L8("Client:"));
	NM_LOG_MESSAGE(KESockMetaConnectionTag(), aCFMessage);
	NM_LOG_END_BLOCK(KESockMetaConnectionTag(), KNullDesC8());


	__ASSERT_DEBUG(!aSenderId.IsNull(), User::Panic(KSpecAssert_ESockSSockscnslc, 33)); //The sender must exist and be in the active list
	__ASSERT_DEBUG(iSelectionStatus==ERequestingCommsBinder, User::Panic(KSpecAssert_ESockSSockscnslc, 34));
	__ASSERT_DEBUG(aSenderId==iTopMcprId, User::Panic(KSpecAssert_ESockSSockscnslc, 35)); //CommsBinder error, this can only come from the iTopMcprId
	//This is an error that needs to be reported to the requesting client.
	PostError(TCFServiceProvider::TCommsBinderRequest::Id(), iOriginalError);
	}

//We call this fn when we need to report the selection error to the originating client,
//but after the selection is finished
void CSelectionRequest::PostError(const TNodeSignal::TMessageId& aMessageId, TInt aError)
	{
	//Some non-recoverable error has occured and we want to report it back to the requesting client.

	//Because the selection is finished now, there shouldn'd be any more outstanding
	//selections going on below.
	__ASSERT_DEBUG(iActiveRequests.Count()==0, User::Panic(KSpecAssert_ESockSSockscnslc, 36));

	iRequest.ReplyTo(Id(), TEBase::TError(aMessageId,aError).CRef());
	}

void CSelectionRequest::HandleProviderSelection(const TNodeId& aMcprId)
	{
	NM_LOG_START_BLOCK(KESockMetaConnectionTag(), _L8("CSelectionRequest::HandleProviderSelection"));
	NM_LOG((KESockMetaConnectionTag(), _L8("[this=0x%08x]"), this));
	NM_LOG_ADDRESS(KESockMetaConnectionTag(), aMcprId);
	NM_LOG_END_BLOCK(KESockMetaConnectionTag(), KNullDesC8());


	// Select next layer provider only if the selection mode is EExplicitConnection
	TUint selectionScope = iSelectionPreferences.Scope();
	if (selectionScope & TSelectionPrefs::EExplicitConnection)
		{
		//Are we currently waiting for this provider to complete our selection request?
		if (KErrNotFound==FindActiveRequest(aMcprId))
			{
			if (KErrNone==iActiveRequests.Append(RNodeInterface()))
				{
				
				 RNodeInterface& mcpr = iActiveRequests[iActiveRequests.Count()-1];
				 mcpr.Open(aMcprId, TClientType(TCFClientType::EServProvider));
				 mcpr.PostMessage(Id(), TCFServiceProvider::TJoinRequest(Id(), TClientType::EAdministrative).CRef()); //Join the provider

				}
			else
				{
				//We have an OOM condition here.
				SelectionError(Id(), KErrNoMemory);
				}
			}
		}
	else if (selectionScope & TSelectionPrefs::ERequestCommsBinder && KErrNone==iOriginalError)
		{
		__ASSERT_DEBUG(iTopMcprId.IsNull(), User::Panic(KSpecAssert_ESockSSockscnslc, 37));

		RNodeInterface::OpenPostMessageClose(Id(), aMcprId, TCFServiceProvider::TJoinRequest(Id(), TCFClientType::EAdministrative).CRef());
		}
	}

void CSelectionRequest::ProviderSelectionFinished(const TRuntimeCtxId& aMcprId)
	{
	NM_LOG_START_BLOCK(KESockMetaConnectionTag(), _L8("CSelectionRequest::ProviderSelectionFinished"));
	NM_LOG((KESockMetaConnectionTag(), _L8("[this=0x%08x]"), this));
	NM_LOG_ADDRESS_EXT(KESockMetaConnectionTag(), aMcprId, _L8("aMcprId:"));
	NM_LOG_END_BLOCK(KESockMetaConnectionTag(), KNullDesC8());

	TInt idx = FindActiveRequest(aMcprId);
	__ASSERT_DEBUG(KErrNotFound!=idx, User::Panic(KSpecAssert_ESockSSockscnslc, 38)); //Sender must be in the active list

	//Leave the providers but do not leave:
	//1) The tier manager, as it has never been joined.
	//2) The top provider, as it needs to be alive for as long as the requestor is happy to delete us
	const TNodeId& selectionProvider = iActiveRequests[idx].RecipientId();
	if (iTopMcprId!=selectionProvider) //Do not leave the top MCpr yet
		{
		if (idx != KErrNotFound && !(iActiveRequests[idx].Flags() & TClientType::ELeaving))
			{
			RNodeInterface::OpenPostMessageClose(Id(), aMcprId, TEChild::TLeft().CRef());
			iActiveRequests[idx].SetFlags(TClientType::ELeaving);
			}
		}

	//Remove sender from the active list
	iActiveRequests[idx].Close();
	iActiveRequests.Remove(idx);
	}

//This fn is called when the whole plane (or just one provider) selection is finished.
//Now we can either start the selected (top) provider or reply to the requesting client
//or just cleanup if the client has cancelled us in the meantime.
void CSelectionRequest::SelectionFinished()
	{
	LOG(ESockLog::Printf(KESockMetaConnectionTag, _L8("CSelectionRequest::SelectionFinished [this=0x%08x]"), this));

	if (iSelectionStatus==ECancelling)
		{ //The requesting client has cancelled us.
		LOG(ESockLog::Printf(KESockMetaConnectionTag, _L8("CSelectionRequest::SelectionFinished - cancelled"), this));

		PostError(TEBase::TCancel::Id(),KErrCancel);
		if (!iTopMcprId.IsNull() && iJoined)
	    	{
	    	//We are ready to destroy ourselves now but we have the iTopMcprId to leave first
	    	RNodeInterface::OpenPostMessageClose(Id(), iTopMcprId, TEPeer::TLeaveRequest().CRef());
	    	}
		return;
		}

	//We can continue with Starting.
	if (KErrNone!=iOriginalError || iTopMcprId.IsNull())
		{
		LOG(ESockLog::Printf(KESockMetaConnectionTag, _L8("CSelectionRequest::SelectionFinished - no top provider found"), this));

		//We don't even have the top provider selected.
		//This is an error that needs to be reported to the requesting client.
		PostError(TCFSelector::TSimpleSelect::Id(),iOriginalError);
		return;
		}

	//We can continue with Requesting Comms Binder.
	TUint selectionScope = iSelectionPreferences.Scope();
	if (selectionScope & TSelectionPrefs::ERequestCommsBinder)
		{
		LOG(ESockLog::Printf(KESockMetaConnectionTag, _L8("CSelectionRequest::SelectionFinished - binding to provider (MCpr Tier Id: %X, AP Id: %d)"), this, iTopMcprInfo.TierId().iUid, iTopMcprInfo.APId()));

		TNodeCtxId sender(ECFActivityBinderRequest, Id()); //TODO[PROD]: Does this make sense to use ECFActivityBinderRequest here?
		RNodeInterface::OpenPostMessageClose(sender, iTopMcprId, TCFServiceProvider::TCommsBinderRequest().CRef());
		iSelectionStatus = ERequestingCommsBinder;
		}
	else
		{
		LOG(ESockLog::Printf(KESockMetaConnectionTag, _L8("CSelectionRequest::SelectionFinished - selected provider (MCpr Tier Id: %X, AP Id: %d)"), this, iTopMcprInfo.TierId().iUid, iTopMcprInfo.APId()));

		//Finish the selection
		//We will leave the top provider in ~CSelectionRequest()
		iRequest.PostMessage(Id(), TCFSelector::TSelectComplete(iTopMcprId,iTopMcprInfo).CRef());
		}
	}

TInt CSelectionRequest::FindActiveRequest(const TRuntimeCtxId& aMcprId)
	{
	for (TInt i = iActiveRequests.Count()-1; i >= 0; i--)
		{
		if (iActiveRequests[i] == aMcprId)
			{
			return i;
			}
		}

	return KErrNotFound;
	}

TInt CSelectionRequest::SecureId(TSecureId& aResult) const
	{
	__ASSERT_DEBUG(iPlatsecApiExt, User::Panic(KSpecAssert_ESockSSockscnslc, 39));
	return iPlatsecApiExt->SecureId(aResult);
	}

TInt CSelectionRequest::VendorId(TVendorId& aResult) const
	{
	__ASSERT_DEBUG(iPlatsecApiExt, User::Panic(KSpecAssert_ESockSSockscnslc, 40));
	return iPlatsecApiExt->VendorId(aResult);
	}

TBool CSelectionRequest::HasCapability(const TCapability aCapability) const
	{
	__ASSERT_DEBUG(iPlatsecApiExt, User::Panic(KSpecAssert_ESockSSockscnslc, 41));
	return iPlatsecApiExt->HasCapability(aCapability);
	}

TInt CSelectionRequest::CheckPolicy(const TSecurityPolicy& aPolicy) const
	{
	__ASSERT_DEBUG(iPlatsecApiExt, User::Panic(KSpecAssert_ESockSSockscnslc, 42));
	return iPlatsecApiExt->CheckPolicy(aPolicy);
	}

NetInterfaces::TInterfaceControl* CSelectionRequest::FetchNodeInterfaceControlL(TInt aInterfaceId)
	{
	if (aInterfaceId == MPlatsecApiExt::KInterfaceId)
    	{
    	if (iPlatsecApiExt)
        	{
        	return this;
        	}
    	
    	if (iProvision && iProvision->GetTypeId() == Meta::STypeId::CreateSTypeId(CConnectionInfo::EUid, CConnectionInfo::ETypeId))
        	{
            iPlatsecApiExt = new(ELeave) ASubSessionPlatsecApiExt(static_cast<const CConnectionInfo*>(iProvision)->SubSessionId());
            return this;
        	}
    	}

	return Messages::ANode::FetchNodeInterfaceControlL(aInterfaceId);
	}

void CSelectionRequest::ReturnInterfacePtrL(MPlatsecApiExt*& aInterface)
	{
	if (iPlatsecApiExt)
		aInterface = this;
	else
		aInterface = NULL;
	}

