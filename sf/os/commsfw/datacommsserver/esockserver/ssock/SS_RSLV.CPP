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
//

#define SYMBIAN_NETWORKING_UPS

#include <comms-infras/ss_roles.h>
#include <comms-infras/ss_log.h>
#include <ss_glob.h>
#include <ss_std.h>
#include <comms-infras/ss_subconnflow.h>
#include "ss_flowrequest.h"
#include <es_prot.h>
#include "SS_rslv.H"
#include <comms-infras/nifif.h>
#include <comms-infras/ss_sapshim.h>
#include <comms-infras/ss_nodemessages_dataclient.h>
#include <es_mbuf.h>

#include <comms-infras/ss_msgintercept.h>


#ifdef _DEBUG
// Panic category for "absolutely impossible!" vanilla ASSERT()-type panics from this module
// (if it could happen through user error then you should give it an explicit, documented, category + code)
_LIT(KSpecAssert_ESockSSockS_RSLV, "ESockSSockS_RSLV");
#endif

#define MSG_PRM(prmIndex)		(prmIndex)

using namespace ESock;
using namespace Messages;

CHostResolver::CHostResolver(CSockSession* aSession, CPlayer* aPlayer, const TSubSessionUniqueId aSubSessionUniqueId)
: CSockSubSession(aSession, aPlayer, aSubSessionUniqueId),
  Messages::ASimpleNodeIdBase(),
  iPtrQryBuf(NULL, 0),
  iPtrQryResBuf(NULL, 0)

/**
Constructor - set up default options.

*/
	{
	LOG_NODE_CREATE(KESockFlowTag, CHostResolver)
    }

CHostResolver::~CHostResolver()
/**
Destructor

*/
	{
   	delete ipQryRespBuf; //-- delete query response buffer
   	delete ipQryBuf;     //-- delete query buffer
	LOG_NODE_DESTROY(KESockFlowTag, CHostResolver)
	}

void CHostResolver::InitiateDestruction()
	{
	// Remove the subsession from the session's subsession list.
    if(iSession)
		{
		iSession->SubSessions().Lock();

		Den::CSubSessionIx::TSubSessionHandle handle;
		if(iSession->SubSessions().Find(this, handle) == KErrNone)
			{
			iSession->PitBoss().RemoveSubSession(handle, iSession);
			}

		iSession->SubSessions().Unlock();
		}

	SetClosing();

    if (iRSP)
        {
//    	ASSERT(iFlowBinderControl);	// in OOM cases may not have been bound
		if(iFlowBinderControl)
			{
	  		iFlowBinderControl->Unbind();
			}
		delete iRSP;
        }

	if (!FlowRequestPending())
		delete this;
	}

CHostResolver * CHostResolver::NewLC(CProtocolRef* aProtRef, CSockSession *aSession, CPlayer* aPlayer, const TSubSessionUniqueId aSubSessionUniqueId)
/**
Create a new CHostresolver

*/
	{
	CHostResolver* h = new(ELeave) CHostResolver(aSession, aPlayer, aSubSessionUniqueId);
	CleanupStack::PushL(h);
	h->CreateL(aProtRef);
	ESOCK_DEBUG_REGISTER_GENERAL_NODE(ESockDebug::KHostResolverNodeUid, h);
	return h;
	}

void CHostResolver::CreateL(CProtocolRef* aProtRef)
	{
	CSockSubSession::ConstructL(aProtRef->Protocol());
	iBusy=EFalse;
	iAwaitingConnection=EFalse;
	iProtocolInfo = &(aProtRef->Info());
	}

void CHostResolver::FinalCompleteAllBlockedMessages(TInt aResult)
	{
	if(iBusy)
		{
		CompleteMessage(iBlockedReq, aResult);
		}
	}

void CHostResolver::ReceivedL(const TRuntimeCtxId& aSender, const TNodeId& aRecipient, TSignatureBase& aMessage)
    {
    (void) aRecipient;
	ESOCK_DEBUG_MESSAGE_INTERCEPT(aSender, aMessage, aRecipient);

    if (aMessage.IsMessage<TEBase::TError>())
    	{
    	//The first error would be from the flow request, any other error iIsFlowRequestPending must be false anyway
		if (FlowRequestPending())
			{
			TEBase::TError& errorMsg(static_cast<TEBase::TError&>(aMessage));
			CompleteFlowRequestMessage(errorMsg.iValue);
			SetFlowRequestPending(EFalse);
			}

		//We are no longer needed and the client has been completed too, tear us down.
		InitiateDestruction();
		}
    else if (aMessage.IsMessage<TCFDataClient::TBindTo>())
		{
		// An error here is propagated back to the waiting CFlowRequest
		TCFDataClient::TBindTo& bindToMsg(static_cast<TCFDataClient::TBindTo&>(aMessage));
		SetFlowRequestPending(EFalse);

		TInt err;
		if (IsClosing())
			{
			err = KErrAbort;
			}
		else
			{
			TRAP(err,BindToL(bindToMsg));
			}

        if(err == KErrNone)
            {
            RClientInterface::OpenPostMessageClose(Id(), aSender, TCFDataClient::TBindToComplete().CRef());
            }
        else
            {
            RClientInterface::OpenPostMessageClose(Id(), aSender, TEBase::TError(aMessage.MessageId(), err).CRef());
            }

        
		CompleteFlowRequestMessage(err);

		if (IsClosing())
			{
			InitiateDestruction();
			}
		}
    else
        {
  		__ASSERT_DEBUG(EFalse, User::Panic(KSpecAssert_ESockSSockS_RSLV, 1));
        }
    }

void CHostResolver::BindToL(const TCFDataClient::TBindTo& aBindTo)
    {
    __ASSERT_DEBUG(iLowerControl==NULL, User::Panic(KSpecAssert_ESockSSockS_RSLV, 2));
#if defined(__GCCXML__)
    CSubConnectionFlowBase* flow = reinterpret_cast<CSubConnectionFlowBase*>(reinterpret_cast<Messages::ANode*>(aBindTo.iNodeId.Ptr()));
#else
    CSubConnectionFlowBase* flow = mcfnode_cast<CSubConnectionFlowBase>(reinterpret_cast<Messages::ANode*>(aBindTo.iNodeId.Ptr()));
#endif
    if (flow==NULL)
        {
        User::Leave(KErrArgument);
        }

    NM_LOG((KESockServerTag, _L8("CHostResolver %08x:\tSynchronous call: From=%08x To=%08x Func=BindToL"),
			this, static_cast<Messages::ANode*>(this), &aBindTo.iNodeId.Node()) )

	iFlowBinderControl = flow->GetBinderControlL();
	iLowerControl = iFlowBinderControl->GetControlL(KNullDesC8);
	iLowerDataSender = iFlowBinderControl->BindL(KNullDesC8, NULL, this);


#ifdef SYMBIAN_NETWORKING_UPS
	LOG(ESockLog::Printf(KESockConnectionTag, _L8("CHostResolver %08x:\tBindToL(%08x)"), this, &aBindTo.iNodeId.Node()) );
	LOG(ESockLogExternal::Printf(KCFNodeTag, KESockFlowTag, _L8("CHostResolver %08x:\tSynchronous call: Sender=%08x, Recipient=%08x, Function=BindToL"), this, static_cast<ANode*>(this), &aBindTo.iNodeId.Node()) );

	// UPS support
	InitialiseFlow(flow);
#endif

    LockToConnectionInfo();
    }

#ifdef SYMBIAN_NETWORKING_UPS

void CHostResolver::InitialiseFlow(CSubConnectionFlowBase* aFlow)
/**
Initialise the flow for Host Resolver UPS operation

@param aFlow Underlying flow to initialise
*/
	{
	CTransportFlowShim* shimFlow = Factories::factoryobject_cast<CTransportFlowShim>(aFlow);
    if (shimFlow)
    	{
    	// Pass the MProvdSecurityChecker down to the CTransportFlowShim so that it can perform
    	// the Platform Security check.
    	shimFlow->SecurityCheck(Session());

    	// Pass the thread/process id down to the CTransportFlowShim as these are required
    	// to perform UPS authorisation.
		TSoOwnerInfo info;
		GetOwnerInfo(info.iProcessId, info.iUid, info.iThreadId);

		shimFlow->SetOption(KSOLProvider, KSoOwnerInfo, TPckgC<TSoOwnerInfo>(info));

		//
		// In addition to KSoOwnerInfo, communicate an MPlatSecApiExt instance downwards in order
		// to allow dynamic retrieval of process and thread id (rather than the process/thread id
		// at the time of the subsession open.  It is not certain that our MPlatsecApiExt can
		// actually provide a valid process/thread id all the time, so the KSoOwnerInfo gives
		// default values for circumstances where MPlatsecApiExt).
		//
		// @TODO REQ1116 - clean this whole area up - especially the use of two mechanisms
		// "one in case the other doesn't always work".  Can we just use the dynamic one?
		//
		shimFlow->SetOption(KSOLProvider, KSoSetPlatSecApi, TPckgC<const MPlatsecApiExt*>(this));
    	}
	}
#endif

TBool CHostResolver::GetFlowAndSCPR(Messages::TNodeId& aFlow, Messages::TNodeId& aSCPR) const
    {
    if (iFlowBinderControl)
        {
    	aFlow = iFlowBinderControl->Flow()->Id();
    	aSCPR = iFlowBinderControl->Flow()->ControlProvider().RecipientId();
    	return ETrue;
        }
    return EFalse;
    }

HBufC8* CHostResolver::GetBuffer(HBufC8* apBuf, TInt aBufLenRequired)
/**
Allocate buffer on heap or reallocate it if required size is more than existing

@param    apBuf           pointer to the existing buffer. If NULL new bufeer will be allocated.
@param    aBufLenRequired reuired buffer length.
@return   pointer to the new buffer
@return   NULL in case of memory problems or other errors
*/
	{
    HBufC8* pBufResult = NULL;

    if(aBufLenRequired <= 0 )
		{
		PanicClient(KESockClientPanic, EBadDescriptorLength);
        return NULL; //-- invalid argument
		}

    if(!apBuf)
		{//-- the buffer is not allocated at all, try allocate it
		pBufResult = HBufC8::New(aBufLenRequired);
		}
    else
		{//-- the buffer is already allocated. Check its size and try to reallocate if
		//-- required size is bigger than existing. Otherwise do nothing.

		if(apBuf->Des().MaxSize() >= aBufLenRequired)
			pBufResult = apBuf; //-- do nothing, existing buffer is big enough
		else
			{  //-- delete and allocate new buffer to avoid data copying
			delete apBuf;
			pBufResult = HBufC8::New(aBufLenRequired);
			}

		}

    return pBufResult;
	}


TBool CHostResolver::GetQueryRespBuffer(void)
/**
Allocate or reallocate buffer for CHostResolver::Query() response data.
The length of the buffer obtained from Message().Ptr1(), as it is the 2nd parameter in RHostResolver::Query() that
shall contain pointer to the client's TPckgBuf query response data.
ipQryRespBuf is used for buffer pointer.

@return ETrue if the buffer has been successfully allocated (or increased)
*/
	{
    //-- obtain the length of the client's buffer and allocate the same buffer on our(server) side.
    //-- data there will be put by the protocol and then copied to the client side in QueryComplete.
    //-- we need to append sizeof(TDes8) to the buffer lenght because from client point of view the buffer is TPckgBuf<...>
    TInt len = iBlockedReq.GetDesLength(1) + sizeof(TDes8);

    ipQryRespBuf = GetBuffer(ipQryRespBuf, len); //-- claim buffer big enough for query response data

    if(ipQryRespBuf)
		{//-- memory allocated, store TPtr8 associated with buffer on the heap, which
		//-- can be used later in asynchonous environment by the protocol.
        iPtrQryResBuf.Set(ipQryRespBuf->Des());
        return ETrue;
		}
    else
		{//-- memory alocation failure
        iPtrQryResBuf.Set(NULL, 0, 0);
        return EFalse;
		}
	}


TBool CHostResolver::GetQueryBuffer(void)
/**
Allocate or reallocate buffer for CHostResolver::Query() data.
The length of the buffer obtained from Message().Ptr0(), as it is the 1st parameter in RHostResolver::Query() that
shall contain pointer to the client's TPckgBuf query data.

@return ETrue if the buffer has been successfully allocated (or increased)
*/
{
    TInt len = iBlockedReq.GetDesLength(0) + sizeof(TDes8);
//    TInt len = iBlockedReq.Client().GetDesLength(Message().Ptr0()) + sizeof(TDes8);

    ipQryBuf = GetBuffer(ipQryBuf, len); //-- claim buffer big enough for query data

    if(ipQryBuf)
    {//-- memory allocated, store TPtr8 associated with buffer on the heap, which
     //-- can be used later in asynchonous environment by the protocol.
        iPtrQryBuf.Set(ipQryBuf->Des());
        return ETrue;
    }
    else
    {//-- memory alocation failure
        iPtrQryBuf.Set(NULL, 0, 0);
        return EFalse;
    }
}

void CHostResolver::QueryL(void)
/**
Host Resolver Query implementation
Query the current protocol and obtain the result

*/
	{
	if (iBusy)
		{
		PanicClient(KESockClientPanic, ETwice);
		SetReturn(KErrInUse);
		return;
		}

	iBlockedReq=Message();

    //-- allocate or reallocate buffer for query and query  response.
    if(! GetQueryRespBuffer() || ! GetQueryBuffer())
		{//-- buffer hasn't been allocated
        SetReturn(KErrNoMemory);
		return;
		}

	//-- read query data from client's buffer to the CHostResolver's internal one
    //-- Ptr0() points to client's Query data buffer, Ptr1() - to the client's query response
   	SafeMessage().ReadL(0, iPtrQryBuf);

	iBusy=ETrue;
	iAwaitingConnection = EFalse;
	DontCompleteCurrentRequest();

	iCurrentOp=EHrQuery;


	iCount=0;

    //-- call CHostResolvProvdBase Query method implemented in appropriate protocol
    iRSP->Query(iPtrQryBuf, iPtrQryResBuf, iCount);
	}

void CHostResolver::QueryGetNext(void)
/**
Host Resolver QueryGetNext implementation.
Obtain the next query result.

*/
	{
	if (iBusy)
		{
		PanicClient(KESockClientPanic, ETwice);
		SetReturn(KErrInUse);
		return;
		}

	if(iCount<1)
		{//-- tried to call QueryGetNext() before Query()
		PanicClient(KESockClientPanic, ENoQueryFirst);
		SetReturn(KErrNotFound);
		return;
		}

	iBusy=ETrue;
	iBlockedReq=Message();
	DontCompleteCurrentRequest();

	//-- call CHostResolvProvdBase QueryGetNext method implemented in appropriate protocol
	if(iCurrentOp == EHrQuery)
		{	//-- iCount is incremented in QueryComplete,
        //-- iQryBuf isn't supposed to be in use here.
        //-- buffer for response ipQryRespBuf must have been allocated by CHostResolver::Query
        if(!ipQryRespBuf)
			{//-- buffer hasn't been allocated, it means memory problems
            SetReturn(KErrNoMemory);
			return;
			}

        iRSP->Query(iPtrQryBuf, iPtrQryResBuf, iCount);
		}

	}


void CHostResolver::GetByNameL()
/**
Start a get by name query

*/
	{
	if (iBusy)
		{
		PanicClient(KESockClientPanic, ETwice);
		SetReturn(KErrInUse);
		return;
		}

	SafeMessage().ReadL(MSG_PRM(0),iNameRec.iName);

	iBusy=ETrue;
	iAwaitingConnection = EFalse;
	iBlockedReq=Message();
	DontCompleteCurrentRequest();

	iCurrentOp=EHRGetByName;
	iCount=0;
	iNameRec.iFlags=iCount;
	iRSP->GetByName(iNameRec);
	}

void CHostResolver::Next()
/**
Retrieve next record

*/
	{
	if (iBusy)
		{
		PanicClient(KESockClientPanic, ETwice);
		SetReturn(KErrInUse);
		return;
		}

	if(iCount<1)
		{
		PanicClient(KESockClientPanic, ENoResolveFirst);
		SetReturn(KErrNotFound);
		return;
		}

	iBusy=ETrue;
	iBlockedReq=Message();
	DontCompleteCurrentRequest();

	iNameRec.iFlags=iCount;
	if(iCurrentOp==EHRGetByName)
		iRSP->GetByName(iNameRec);
	else
		iRSP->GetByAddress(iNameRec);
	}

void CHostResolver::GetByAddressL()
/**
Start a get by address query

*/
	{
	if (iBusy)
		{
		PanicClient(KESockClientPanic, ETwice);
		SetReturn(KErrInUse);
		return;
		}

	SafeMessage().ReadL(MSG_PRM(0),iNameRec.iAddr);

	iBusy=ETrue;
	iBlockedReq=Message();
	DontCompleteCurrentRequest();

	iCurrentOp=EHRGetByAddress;

	iCount=0;
	iNameRec.iFlags=iCount;
	iRSP->GetByAddress(iNameRec);
	}

void CHostResolver::GetHostName()
/**
Get the name of the local host

*/
	{
    iCurrentOp = EHRGetHostName;
	if (iBusy)
		{
		PanicClient(KESockClientPanic, ETwice);
		SetReturn(KErrInUse);
		return;
		}

	iBusy=ETrue;
	iBlockedReq=Message();
	DontCompleteCurrentRequest();

	iRSP->GetHostName(iNameRec.iName);
	}

void CHostResolver::SetHostNameL()
/**
Set the name of the host machine

*/
	{
   iCurrentOp = EHRSetHostName;
	if (iBusy)
		{
		PanicClient(KESockClientPanic, ETwice);
		SetReturn(KErrInUse);
		return;
		}
	THostName n;
	SafeMessage().ReadL(MSG_PRM(0),n);

	iBusy=ETrue;
	iBlockedReq=Message();
	DontCompleteCurrentRequest();

	iRSP->SetHostName(n);
	}

void CHostResolver::SetOptionL()
	{
	iCurrentOp = EHRSetOpt;
	if (iBusy)
		{
		PanicClient(KESockClientPanic, ETwice);
		SetReturn(KErrInUse);
		return;
		}

	ReadAndSetOptionL();
	
	}


void CHostResolver::Cancel()
/**
Cancel any pending request

*/
	{
	if (!iBusy)
		return;

	iBusy=EFalse;
	iCount=0;

	CompleteMessage(iBlockedReq, KErrCancel);
	iRSP->CancelCurrentOperation();
	}

void CHostResolver::QueryComplete(TInt anError)
/**
outstanding request has completed

*/
	{
	LOG(ESockLog::Printf(KESockConnectionTag, _L8("CHostResolver %08x:\tQueryComplete(TInt anError)\taError %d"), this, anError) );

	if(!iBusy)
		return;

	Den::RSafeMessage& message = static_cast<Den::RSafeMessage&>(const_cast<RMessage2&>(iBlockedReq));
	if (anError == KErrCompletion && RequiresConnectionStartup())
		{
		//
		// KErrCompletion from Host Resolvers that require connection startup indicates that
		// the name was not found locally, a connection needs to be started, and the
		// GetByName() re-issued.   In some ways, this is the equivalent of CSocket::NoBearer().
		//
 		TRAP(anError,HandleConnectionSetupL());
 		if ( anError == KErrNone )
 			{
			return;
 			}
#ifdef SYMBIAN_NETWORKING_UPS
 		else
 		if (anError == KErrPermissionDenied)
			{
			// This code path supports the UPS "Disabled" configuration.  A KErrPermissionDenied error
			// is returned if the platsec capability check failed.  See if the provider has an error
			// code that it prefers to complete the request with.  This supports the behaviour in the
			// TCP provider whereby the lack of capabilities means that the request is looked up locally
			// without using the network and, if not found, a provider specific error code is returned
			// rather than KErrPermissionDenied.
			TInt err = iRSP->SetOption(KSOLProvider, KSoGetErrorCode, KNullDesC8());
			if (err > 0)
				{
				anError = -err;
				}
			}
#endif //SYMBIAN_NETWORKING_UPS
		}
	else
	if (anError==KErrNone)
		{
		switch (iBlockedReq.Function())
			{
		case EHRNext:
		case EHRGetByName:
		case EHRGetByAddress:
			{
			TInt ret = message.Write(MSG_PRM(1),TPtr8((TUint8 *)&iNameRec,sizeof(iNameRec),sizeof(iNameRec)));
			if(ret!=KErrNone)
				{
				iBusy=EFalse;
				return;
				}
			++iCount;
			break;
			}
		case EHRGetHostName:
			{
			TInt ret = message.Write(MSG_PRM(0),iNameRec.iName);
			if(ret!=KErrNone)
				{
				iBusy=EFalse;
				return;
				}
			break;
			}
		case EHRSetHostName:
		case EHRSetOpt:
			break;

        //-- protocol has completed Query or QueryNext write query response data into the client's buffer
        case EHrQuery:
            if(ipQryRespBuf)
            {
                TInt ret = message.Write(1, iPtrQryResBuf);
				if(ret!=KErrNone)
					{
					iBusy=EFalse;
					return;
					}
				++iCount;
			}
            else
              Panic(EBadErrorCall);
        break;

        case EHrQueryNext:
            if(ipQryRespBuf)
            {
				TInt ret = message.Write(0, iPtrQryResBuf);
				if(ret!=KErrNone)
					{
					iBusy=EFalse;
					return;
					}
				++iCount;
			}
            else
                Panic(EBadErrorCall);
        break;

    	default:
			Panic(ENothingToComplete);
			}
		}
	else
		iCount=0;

	iBusy=EFalse;
	iAwaitingConnection = EFalse;
	message.Complete(anError);
	}

//
//TBool CHostResolver::FetchOwnerInfo(CConnectionProviderBase::TEnumClients aClientType, TProcessId& aProcessId, TUidType& aUidType, TThreadId& aThreadId)
//	{
//	TBool b = aClientType == CConnectionProviderBase::EAll || aClientType == CConnectionProviderBase::EHostResolver;
//	if ( b )
//		{
//		GetOwnerInfo( aProcessId, aUidType, aThreadId);
//		}
//	return b;
//	}
//

void CHostResolver::StartSending()
/**
Called when we have got a valid connection info that we can set

*/
    {
	LOG(ESockLog::Printf(KESockConnectionTag, _L8("CHostResolver %08x:\tStartSending() iBusy %d, iAwaitingConnection %d"), this,iBusy,iAwaitingConnection) );
	LockToConnectionInfo();
	if (iBusy && iAwaitingConnection)
		{
		switch (iCurrentOp)
			{
		case EHRGetByName:
			iRSP->GetByName(iNameRec);
			break;
		case EHRGetByAddress:
			iRSP->GetByAddress(iNameRec);
			break;

        case EHrQuery:

            if(! GetQueryRespBuffer())
	        {//-- buffer hasn't been allocated
                SetReturn(KErrNoMemory);
		        break;
	        }
            else
            {
                //-- call CHostResolvProvdBase Query method implemented in appropriate protocol
               iRSP->Query(iPtrQryBuf, iPtrQryResBuf, iCount);
            }
			break;

		case EHRGetHostName:
			iRSP->GetHostName(iNameRec.iName);
			break;

		case EHRSetHostName:
			{
			THostName n;
			SafeMessage().ReadL(MSG_PRM(0),n);
			iRSP->SetHostName(n);
			}
			break;
			

		case EHRSetOpt:
			{
			TInt err;
			TRAP(err,ReadAndSetOptionL());	
			if(err != KErrNone)
				{
				SetReturn(err);
				}
			}
			break;

		default:
			Panic(ENothingToComplete);
			}
		}
	iAwaitingConnection = EFalse;
    }

void CHostResolver::Error(TInt aError)
	{
	LOG(ESockLog::Printf(KESockConnectionTag, _L8("CHostResolver %08x:\tError(aError %d)"), this, aError) );

#ifdef SYMBIAN_NETWORKING_UPS
	if (aError == KErrPermissionDenied)
		{
		// UPS support.  We received a TError in response to a TNoBearer sent previously, indicating
		// UPS authorisation failure.
		//
		// ASSERT(incoming message is a TError with iMsgId set to TNoBearer)
		//
		// See if the provider has an error code that it prefers to complete the request with.
		// This supports the behaviour in the TCP provider whereby the lack of capabilities
		// (lack of authorisation when using UPS) means that the request is looked up locally
		// without using the network and, if not found, a provider specific error code is returned.
		//
		// The SetOption() interface towards Host Resolver SAPs does not allow for the
		// retrieval of a TInt value (being a const interface).  So take a positive return value
		// to represent a valid error code to use.

		TInt err = iRSP->SetOption(KSOLProvider, KSoGetErrorCode, KNullDesC8());
		if (err > 0)
			{
			aError = -err;
			}
		}
#endif //SYMBIAN_NETWORKING_UPS

	QueryComplete(aError);
	iAwaitingConnection = EFalse;
	TBuf8<sizeof(TConnectionInfo)> buf;
	if (aError == KErrBindersInvalid)
	    {
        iLowerControl = NULL;
    	iFlowBinderControl = NULL;
	    iLowerDataSender = NULL;
	    }
	else if (aError == KErrDisconnected || iLowerControl->Control((TUint)KSOLProvider, (TUint)KSoConnectionInfo, buf)!=KErrNone)
	    {
    	//reset the scope
    	TConnectionInfo info(0,0);
    	TPckg<TConnectionInfo> infoBuf(info);

    	LockToConnectionInfo(infoBuf);
	    }
    }

void CHostResolver::HandleConnectionSetupL()
/**
Handle any implicit connection setup which is required as a result of a Host Resolver request.

*/
	{
	LOG(ESockLog::Printf(KESockConnectionTag, _L8("CHostResolver %08x:\tHandleConnectionSetup selecting....."), this) );

	iAwaitingConnection = ETrue;

	// Workaround to kick off the flow set-up
	RMBufChain dummy;
#ifdef SYMBIAN_NETWORKING_UPS
	User::LeaveIfError(iLowerDataSender->Send(dummy));
#else
	iLowerDataSender->Send(dummy);
#endif
	}

void CHostResolver::LockToConnectionInfo()
    {
    // retrieve default connection info
    TPckgBuf<TConnectionInfo> info;
    __ASSERT_DEBUG(iLowerControl!=NULL, User::Panic(KSpecAssert_ESockSSockS_RSLV, 3));
    iLowerControl->Control((TUint)KSOLProvider, (TUint)KSoConnectionInfo, info);

    // set host resolver connection info
	TPckgBuf<TSoIfConnectionInfo> ifInfo;
    ifInfo().iIAPId = info().iIapId;
	ifInfo().iNetworkId = info().iNetId;
    LockToConnectionInfo(ifInfo);
    }

void CHostResolver::LockToConnectionInfo(const TDesC8& aConnectionInfo)
/**
Called when interface selection has completed in order to communicate the connection
information to the Host Resolver.

@param aConnectionInfo Connection information identifying the interface
*/
	{
	if (aConnectionInfo.Length() > 0)
		{
		LOG( ESockLog::ConnectionInfoPrintf(aConnectionInfo, _L("CHostResolver %08x:\tLockToConnectionInfo()"), this) );
		iRSP->SetOption(KSOLProvider, static_cast<TUint>(KSoConnectionInfo), aConnectionInfo);
		}
	}


void CHostResolver::ProcessMessageL()
	{
	switch(Message().Function())
		{
	   case EHRGetByName:
			GetByNameL();
			break;

		case EHRNext:
			Next();
			break;

		case EHRGetByAddress:
			GetByAddressL();
			break;

		case EHRGetHostName:
			GetHostName();
			break;

		case EHRSetHostName:
			SetHostNameL();
			break;

		case EHRCancel:
			Cancel();
			break;

		case EHrQuery:
			QueryL();
			break;

		case EHrQueryNext:
			QueryGetNext();
			break;

	    case EHRClose:
	    	InitiateDestruction();
			break;
			
	    case EHRSetOpt:
	    	SetOptionL();
	    	break;
			
		default:
			SetReturn(KErrNotSupported);
		}
	}

#ifdef SYMBIAN_NETWORKING_UPS
TInt CHostResolver::GetProcessAndThreadId(TProcessId& aProcessId, TThreadId& aThreadId) const
	{
	if (iBusy && !iBlockedReq.IsNull())
		{
		return ASockSubSessionPlatsecApiExt::GetProcessAndThreadIdFromRMessage(iBlockedReq, aProcessId, aThreadId);
		}
	else
		{
		return KErrNotFound;
		}
	}
#endif

void CHostResolver::ReadAndSetOptionL()
	{

	TInt optionName=Message().Int0();
	TInt optionLevel=Message().Int2();

	RBuf8 setOptionBuf;  
	setOptionBuf.CreateL(Message().GetDesLengthL(1)); //1 - is the option that is passed 
	CleanupClosePushL(setOptionBuf);
	SafeMessage().ReadL(MSG_PRM(1),setOptionBuf);

	TPtrC8 setOptionPtr(setOptionBuf);
 	SetReturn(iRSP->SetOption(optionLevel,optionName,setOptionPtr));
	
	CleanupStack::PopAndDestroy();

	}


// ============================================================================================

CServiceResolver::CServiceResolver(CSockSession* aSession, CPlayer* aPlayer, const TSubSessionUniqueId aSubSessionUniqueId)
/**
Constructor - set up default options.

*/
: CSockSubSession(aSession, aPlayer, aSubSessionUniqueId)
	{
	}

CServiceResolver * CServiceResolver::NewLC(CProtocolRef* aProtRef, CSockSession *aSession, CPlayer* aPlayer, const TSubSessionUniqueId aSubSessionUniqueId)
/**
Create a new CService resolver
*/
	{
	CServiceResolver* r = new(ELeave) CServiceResolver(aSession, aPlayer, aSubSessionUniqueId);
	CleanupStack::PushL(r);
	r->ConstructL(aProtRef->Protocol());
	r->iSession=aSession;
	return r;
	}

void CServiceResolver::FinalCompleteAllBlockedMessages(TInt aResult)
	{
	if(iBusy)
		{
		CompleteMessage(iBlockedReq, aResult);
		}
	}

void CServiceResolver::GetByNameL()
/**
Start a get by name query

*/
	{
	if (iBusy)
		{
		PanicClient(KESockClientPanic, ETwice);
		SetReturn(KErrInUse);
		return;
		}

	SafeMessage().ReadL(MSG_PRM(0),iNameBuf);

	iBusy=ETrue;
	iBlockedReq=Message();
	DontCompleteCurrentRequest();

	iRSP->GetByName(iNameBuf,iPortNum);
	}

void CServiceResolver::GetByNumber()
/**
Start a get by number query

*/
	{
	if (iBusy)
		{
		PanicClient(KESockClientPanic, ETwice);
		SetReturn(KErrInUse);
		return;
		}

	iBusy=ETrue;
	iBlockedReq=Message();
	DontCompleteCurrentRequest();

	iRSP->GetByNumber(iNameBuf,Message().Int1());
	}

void CServiceResolver::RegisterServiceL()
/**
Register a new service

*/
	{
	if (iBusy)
		{
		PanicClient(KESockClientPanic, ETwice);
		SetReturn(KErrInUse);
		return;
		}


	SafeMessage().ReadL(MSG_PRM(0),iNameBuf);

	iBusy=ETrue;
	iBlockedReq=Message();
	DontCompleteCurrentRequest();

	iPortNum=Message().Int1();
	iRSP->RegisterService(iNameBuf,iPortNum);
	}

void CServiceResolver::RemoveServiceL()
/**
delete an entry in the services database
*/
	{
	if (iBusy)
		{
		PanicClient(KESockClientPanic, ETwice);
		SetReturn(KErrInUse);
		return;
		}

	SafeMessage().ReadL(MSG_PRM(0),iNameBuf);
	iBusy=ETrue;
	iBlockedReq=Message();
	DontCompleteCurrentRequest();

	iPortNum=Message().Int1();
	iRSP->RemoveService(iNameBuf,iPortNum);
	}

void CServiceResolver::Cancel()
/**
Cancel any pending request

*/
	{
	if (!iBusy)
		return;

	iBusy=EFalse;
	CompleteMessage(iBlockedReq, KErrCancel);
	iRSP->CancelCurrentOperation();
	}


void CServiceResolver::QueryComplete(TInt anError)
/**
a request has completed
*/
	{

	//__ASSERT_DEBUG(iBusy,PanicClient(Message(), EBadQueyComplete));
	if(!iBusy)
		return;

	Den::RSafeMessage& message = static_cast<Den::RSafeMessage&>(const_cast<RMessage2&>(iBlockedReq));

	LOG(ESockLog::Printf(KESockConnectionTag, _L8("CServiceResolver %08x:\tQueryComplete(TInt anError)\t aError %d, message %08X"), this, anError, message.Handle()) );

	TPortNum num;

	if (anError==KErrNone)
		switch (iBlockedReq.Function())
			{
		case ESRGetByName:
			{
			num=iPortNum;
			TInt ret = message.Write(MSG_PRM(1),num);
			if(ret!=KErrNone)
				{
				iBusy=EFalse;
				return;
				}
			break;
			}
		case ESRGetByNumber:
			{
			TInt ret = message.Write(MSG_PRM(0),iNameBuf);
			if(ret!=KErrNone)
				{
				iBusy=EFalse;
				return;
				}
			break;
			}
		case ESRRegisterService:
		case ESRRemoveService:
			break;
		default:
			Panic(ENothingToComplete);
			}

	iBusy=EFalse;
	message.Complete(anError);
	}

CServiceResolver::~CServiceResolver()
	{
	delete iRSP;
	}

void CServiceResolver::ProcessMessageL()
	{
	switch(Message().Function())
		{
		case ESRGetByName:
			GetByNameL();
			break;

		case ESRGetByNumber:
			GetByNumber();
			break;

		case ESRRegisterService:
			RegisterServiceL();
			break;

		case ESRRemoveService:
			RemoveServiceL();
			break;

		case ESRCancel:
			Cancel();
			break;

		case ESRClose:
			delete this;
			break;

		default:
			SetReturn(KErrNotSupported);
		}
	}
CNetDatabase* CNetDatabase::NewLC(CProtocolRef* aProtRef, CSockSession *aSession, CPlayer* aPlayer, const TSubSessionUniqueId aSubSessionUniqueId)
/**
Create a new CNetDatabase
*/
	{
	CNetDatabase* n = new(ELeave) CNetDatabase(aSession, aPlayer, aSubSessionUniqueId);
	CleanupStack::PushL(n);
	n->ConstructL(aProtRef->Protocol());
	n->iSession=aSession;
	return n;
	}


CNetDatabase::CNetDatabase(CSockSession *aSession, CPlayer* aPlayer, const TSubSessionUniqueId aSubSessionUniqueId)
/**
Constructor - set up default options.
*/
: CSockSubSession(aSession, aPlayer, aSubSessionUniqueId)
	{
	}

void CNetDatabase::FinalCompleteAllBlockedMessages(TInt aResult)
	{
	if(iBusy)
		{
		CompleteMessage(iBlockedReq, aResult);
		}
	}

void CNetDatabase::RequestL()
/**
Perform a query
*/
	{
	if (iBusy)
		{
		PanicClient(KESockClientPanic, ETwice);
		SetReturn(KErrInUse);
		return;
		}

	delete iBuf;
	iBuf=0;
	iBuf=HBufC8::NewMaxL(Message().Int1());

	delete iBufPtr;
	iBufPtr=0;
	iBufPtr=new(ELeave) TPtr8(iBuf->Des());

	SafeMessage().ReadL(MSG_PRM(0),*iBufPtr);

	iBusy=ETrue;
	iBlockedReq=Message();
	DontCompleteCurrentRequest();


	switch (Message().Function())
		{
	case ENDQuery:
		iRSP->Query(*iBufPtr);
		break;
	case ENDAdd:
		iRSP->Add(*iBufPtr);
		break;
	case ENDRemove:
		iRSP->Remove(*iBufPtr);
		break;
		}
	}

void CNetDatabase::Cancel()
/**
Cancel any pending request
*/
	{
	if (!iBusy)
		return;

	iBusy=EFalse;
	CompleteMessage(iBlockedReq, KErrCancel);
	iRSP->CancelCurrentOperation();
	}

void CNetDatabase::QueryComplete(TInt anError)
/**
An outstanding query had completed
*/
	{

	if(!iBusy)
		return;
	Den::RSafeMessage& message = static_cast<Den::RSafeMessage&>(const_cast<RMessage2&>(iBlockedReq));

	if (anError==KErrNone && iBlockedReq.Function()==ENDQuery)
		{
		TInt ret = message.Write(MSG_PRM(2),*iBufPtr);
		if(ret!=KErrNone)
			{
			iBusy=EFalse;
			return;
			}
		}
	iBusy=EFalse;
	message.Complete(anError);
	}

CNetDatabase::~CNetDatabase()
	{
	delete iRSP;
	delete iBuf;
	delete iBufPtr;
	}

void CNetDatabase::ProcessMessageL()
	{
	switch(Message().Function())
		{
		case ENDQuery:
		case ENDAdd:
		case ENDRemove:
			RequestL();
			break;

		case ENDCancel:
			Cancel();
			break;

		case ENDClose:
			delete this;
			break;

		default:
			SetReturn(KErrNotSupported);
		}
	}
