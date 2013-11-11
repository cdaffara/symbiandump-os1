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

#include "mmrcclientsession.h"
#include "mmrcserverstart.h"
#include "multimediaresourcecontrolobserver.h"
#include "mmrcadaptationproxy.h"
#include "mlogicalchain.h"
#include "audiocontext.h"
/**
 * 
 */
RMMRCClientSession::RMMRCClientSession()
: iLogicalChainLastCommited(NULL),
  iLogicalChainRequested(NULL)
	{
	TRACE_CREATE();
	DP_CONTEXT(----> RMMRCClientSession::RMMRCClientSession *CD1*, CtxDevSound, DPLOCAL);
	DP_IN();
	DP_OUT();
	}

/**
 * 
 */
RMMRCClientSession::~RMMRCClientSession()
	{
	DP_CONTEXT(----> RMMRCClientSession::~RMMRCClientSession *CD1*, CtxDevSound, DPLOCAL);
	DP_IN();
	
	if(iMMRCClientSideServerRequestToAdaptationAO)
		{
		delete iMMRCClientSideServerRequestToAdaptationAO;
		iMMRCClientSideServerRequestToAdaptationAO=0;
		}

	DP_OUT();
	}


/**
// Runs client-side and starts the separate server process
*/
TInt RMMRCClientSession::StartServer()
	{
	DP_CONTEXT(----> RMMRCClientSession::StartServer *CD1*, CtxDevSound, DPLOCAL);
	DP_IN();
	
	TInt err = KErrNone;
	err = StartMMRCServer(iServerThread);
	
	DP0_RET(err, "err = %d");
	}

/**
*/
TInt RMMRCClientSession::Open(MMultimediaResourceControlObserver& aCallback)
	{
	DP_CONTEXT(----> RMMRCClientSession::Open *CD1*, CtxDevSound, DPLOCAL);
	DP_IN();
	
	TInt err = KErrNone;
	iObserver = &aCallback;
	
	err = CreateSession(KMMRCServerName, TVersion(1,0,0));	 
	
	DP0_RET(err, "err = %d");
	}

/**
*/
TUint64 RMMRCClientSession::LogOn(TProcessId aProcessId)
	{
	DP_CONTEXT(----> RMMRCClientSession::LogOn *CD1*, CtxDevSound, DPLOCAL);
	DP_IN();
	iCurrentRequestId = 0;
	TUint64 contextId;
	TPckg<TUint64> contextIdPckg(contextId);
	TPckg<TProcessId> processIdPckg(aProcessId);
	TIpcArgs args(&processIdPckg, &contextIdPckg);
	
	SendReceive(EMMRCClientToServerRequestContextId,args);
	
	DP0_RET(contextId, "contextId = %d");
	}


/**
 */
void RMMRCClientSession::Close()
	{
	DP_CONTEXT(----> RMMRCClientSession::Close *CD1*, CtxDevSound, DPLOCAL);
	DP_IN();

	TRequestStatus RequestStatus;
	SendReceive(EMMRCClientToServerCloseSession, TIpcArgs(0), RequestStatus);
	User::WaitForRequest(RequestStatus); 
	if(iMMRCClientSideServerRequestToAdaptationAO)
		{
		delete iMMRCClientSideServerRequestToAdaptationAO;
		iMMRCClientSideServerRequestToAdaptationAO=0;
		}
	
	RHandleBase::Close();
	iServerThread.Close();
	
	DP_OUT();
	}

/**
*/
void RMMRCClientSession::InitializeServerL(RHandleBase& aMsgQueueHandle)
	{
	DP_CONTEXT(----> RMMRCClientSession::InitializeServerL *CD1*, CtxDevSound, DPLOCAL);
	DP_IN();
	
	//Send the Client ThreadId
	RThread me;
	const TThreadId& id = me.Id();
	SendReceive(EMMRCClientToServerSetClientThreadId, TIpcArgs(&id));
	
	//Create a client side adaptation proxy on the client schudeler 
	iMMRCClientSideServerRequestToAdaptationAO = CMMRCClientSideServerRequestToAdaptationAO::NewL();
	// send synchronously the pointer to the server
	SendReceive(EMMRCClientToServerSetAdaptationProxy, TIpcArgs(iMMRCClientSideServerRequestToAdaptationAO));
	
	//Send synchronously the Message Queue pointer to the server
	SendReceive(EMMRCClientToServerSetMsgQueueHandle, TIpcArgs(aMsgQueueHandle));
	
	DP_OUT();
	}

/**
*/
TInt RMMRCClientSession::SendResourceRequest(MLogicalChain* aLogicalChainLastCommited, MLogicalChain* aLogicalChainRequested, CAudioContext* aContext)
	{
	DP_CONTEXT(----SSSS> RMMRCClientSession::SendResourceRequest *CD1*, CtxDevSound, DPLOCAL);
	DP_IN();

	TInt err = KErrNone;
	//Only one request at a time!!!
	if(iLogicalChainLastCommited || iLogicalChainRequested)
		{
		err = KErrServerBusy;
		#ifdef _DEBUG	
			RDebug::Print(_L("!!RMMRCClientSession::SendResourceRequest -  Second request but server busy"));
		#endif
		DP0_RET(err, "err = %d");
		}
	
	//Clone the chains; they are realesed on response
	if( err == KErrNone )
		{
		TRAP(err, iLogicalChainLastCommited = aLogicalChainLastCommited->CloneL());
		if( err == KErrNone )
			{
			TRAP(err, iLogicalChainRequested = aLogicalChainRequested->CloneL());
			}
		}
		
	//Send the request
	if ( err == KErrNone )
		{
		// AM To determine when we reached the limit of messages
		err = Send(EMMRCClientToServerRequestResource, TIpcArgs(iLogicalChainLastCommited, iLogicalChainRequested, aContext, aContext->MsgVersion()));
		}
	#ifdef _DEBUG	
	else
		{
		RDebug::Print(_L("!!!!RMMRCClientSession::SendResourceRequest - Error = %d"), err);
		}
	#endif
	
	DP0_RET(err, "err = %d");
	}

/**
 */
 void RMMRCClientSession::ResourceRequestResponse(MLogicalChain* aChainResponse, TMMRCServerToClientMessageResults /*aResult*/, TInt aError)
	{
	DP_CONTEXT(--------> RMMRCClientSession::ResourceRequestResponseL *CD1*, CtxDevSound, DPLOCAL);
	DP_IN();
	
	//Check if the response is not null... Should never happen
	if( aChainResponse == NULL && aError == KErrNone)
	//coverity[var_compare_op]	
		{
		RDebug::Print(_L("!!!!RMMRCClientSession::ResourceRequestResponseL -  Chain in input NULL - KErrArgument"));
		__ASSERT_ALWAYS(0, User::Panic(_L("MMRCClientSessionNullResponse"), 1));
		}
	
	iObserver->ReceiveResourceResponse(aChainResponse, aError);
	//coverity[var_deref_model]
	// Coverity incorrectly assumes that aChainResponse is always NULL
	DP_OUT();
	}

/**
*/
TInt RMMRCClientSession::RegisterAsClient(TUid aEventType, const TDesC8& aNotificationRegistrationData)
	{
	DP_CONTEXT(----> RMMRCClientSession::RegisterAsClient *CD1*, CtxDevSound, DPLOCAL);
	DP_IN();
	
	TInt err (KErrNone);
	err = SendReceive(EMMRCClientToServerRegisterAsClient, TIpcArgs(&aEventType, &aNotificationRegistrationData));

	
	DP0_RET(err, "err = %d");
	}
	
/**
*/	
TInt RMMRCClientSession::CancelRegisterAsClient(TUid aEventType)
	{
	DP_CONTEXT(----> RMMRCClientSession::CancelRegisterAsClient *CD1*, CtxDevSound, DPLOCAL);
	DP_IN();
	
	TInt err (KErrNone);
	err = SendReceive(EMMRCClientToServerCancelRegisterAsClient, TIpcArgs(&aEventType));

	
	DP0_RET(err, "err = %d");
	}

/**
*/	
TInt RMMRCClientSession::WillResumePlay()
	{
	DP_CONTEXT(----> RMMRCClientSession::WillResumePlay *CD1*, CtxDevSound, DPLOCAL);
	DP_IN();
	
	TInt err (KErrNone);
	err = SendReceive(EMMRCClientToServerWillResumePlay, TIpcArgs(0));

	
	DP0_RET(err, "err = %d");
	}
	
void RMMRCClientSession::ResetMessages()
	{
	DP_CONTEXT(----> RMMRCClientSession::ResetMessages *CD1*, CtxDevSound, DPLOCAL);
	DP_IN();
	
	//Clean the logical chains
	if(iLogicalChainLastCommited)
		{
		iLogicalChainLastCommited->Release();
		iLogicalChainLastCommited = NULL;
		}
	if(iLogicalChainRequested)
		{
		iLogicalChainRequested->Release();
		iLogicalChainRequested = NULL;
		}
	
	DP_OUT();
	}		
//EOF

