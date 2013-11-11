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

#include "mmrcclientimplementation.h"
#include "mmrcclientmsgqueuehandler.h"
#include "multimediaresourcecontrolobserver.h"
#include "audiocontext.h"

#include <a3f/maudiocontextobserver.h>


const TInt KMsgQueueSlots = 8;

 
/**
 */
CMMRCClientImplementation::CMMRCClientImplementation(MMultimediaResourceControlObserver& aCallback) 
: iObserver(aCallback)
	{
	TRACE_CREATE();
	DP_CONTEXT(----> CMMRCClientImplementation::CMMRCClientImplementation *CD1*, CtxDevSound, DPLOCAL);
	DP_IN();
	DP_OUT();
	}

/**
 */
CMMRCClientImplementation::~CMMRCClientImplementation()
	{
	DP_CONTEXT(----> CMMRCClientImplementation::~CMMRCClientImplementation *CD1*, CtxDevSound, DPLOCAL);
	DP_IN();

	if(iMsgClientQueueHandler)
		{
		delete iMsgClientQueueHandler;
		}
	iMsgQueue.Close();

	DP_OUT();
	}

/**
 */
CMMRCClientImplementation* CMMRCClientImplementation::NewL(MMultimediaResourceControlObserver& aCallback)
	{
	DP_STATIC_CONTEXT(----> CMMRCClientImplementation::NewL *CD1*, CtxDevSound, DPLOCAL);
	DP_IN();
	
	CMMRCClientImplementation* self = new (ELeave) CMMRCClientImplementation(aCallback);
	CleanupStack::PushL (self);
	self->ConstructL ();
	CleanupStack::Pop (self);

	DP0_RET(self, "0x%x");
	}

/**
 */
void CMMRCClientImplementation::ConstructL()
	{
	DP_CONTEXT(----> CMMRCClientImplementation::ConstructL *CD1*, CtxDevSound, DPLOCAL);
	DP_IN();
	
	TInt error = iMMRCClientSession.Open (iObserver);
	if ( KErrNotFound == error)
		{
		//The server is not found, create it
		error = iMMRCClientSession.StartServer ();
		User::LeaveIfError (error);

		//Try again to create a connection to it
		error = iMMRCClientSession.Open (iObserver);
		#ifdef _DEBUG	
		if(error != KErrNone)
			{
			RDebug::Print(_L("!!!!CMMRCClientImplementation::ConstructL - Open session 2 - error = %d"), error);
			}
		#endif
		User::LeaveIfError (error);
		}
	#ifdef _DEBUG	
	if(error != KErrNone)
		{
		RDebug::Print(_L("!!!!CMMRCClientImplementation::ConstructL - Open session 1 - error = %d"), error);
		}
	#endif
	User::LeaveIfError (error);

	//Create an un-named msg queue, a handle is needed to connect to it.
	error = iMsgQueue.CreateGlobal (KNullDesC, KMsgQueueSlots,	EOwnerProcess);
	#ifdef _DEBUG	
	if(error != KErrNone)
		{
		RDebug::Print(_L("!!!!CMMRCClientImplementation::ConstructL - CreateGlobal - error = %d"), error);
		}
	#endif
	User::LeaveIfError (error);
	iMsgClientQueueHandler = CMMRCClientMsgQueueHandler::NewL (iMsgQueue, *this);

	//Forward the message queue handle to the server, which will connect to it.
	iMMRCClientSession.InitializeServerL(iMsgQueue);
	
	DP_OUT();
	}
/**
 */
TUint64 CMMRCClientImplementation::LogOn(TProcessId aProcessId)
	{
	DP_CONTEXT(----> CMMRCClientImplementation::LogOn *CD1*, CtxDevSound, DPLOCAL);
	DP_IN();
	
	TUint64 contextId = iMMRCClientSession.LogOn(aProcessId);
	
	DP0_RET(contextId, "contextId = %d");
	}

/**
 */
void CMMRCClientImplementation::Close()
	{
	DP_CONTEXT(----> CMMRCClientImplementation::Close *CD1*, CtxDevSound, DPLOCAL);
	DP_IN();

	iMMRCClientSession.Close ();

	DP_OUT();
	}

/**
 */
TInt CMMRCClientImplementation::SendResourceRequest(MLogicalChain* aLogicalChainLastCommited, MLogicalChain* aLogicalChainRequested, CAudioContext* aContext)
	{
	DP_CONTEXT(----> CMMRCClientImplementation::SendResourceRequest *CD1*, CtxDevSound, DPLOCAL);
	DP_IN();
	
	TInt err = iMMRCClientSession.SendResourceRequest(aLogicalChainLastCommited, aLogicalChainRequested, aContext);
	
	DP0_RET(err, "err = %d");
	}

/**y
 */
void CMMRCClientImplementation::HandleMessage(TMMRCQueueItem& aMessage)
	{
 	DP_CONTEXT(----> CMMRCClientImplementation::HandleMessage *CD1*, CtxDevSound, DPLOCAL);
	DP_IN();
	
	#ifdef _DEBUG	
		RDebug::Print(_L("RMMRCClientSession::ResourceRequestResponseL aMessage.iRequestType = %d, aMessage.iResult = %d, aMessage.iErrorCode = %d"), aMessage.iRequestType, aMessage.iResult, aMessage.iErrorCode);
	#endif
	
	switch(aMessage.iRequestType)
	{
	
	case EMMRCRequestCommitUpdate:
		{
		// Ignore this, now this will come directly from adaptation
		// iObserver.ReceiveResourceUpdate(aMessage.iData, aMessage.iErrorCode);
		}
		break;

	case EMMRCRequestCommitResponse:
		{
		// This would remain coming throug MMRC to update client side
		iMMRCClientSession.ResourceRequestResponse(aMessage.iData, aMessage.iResult, aMessage.iErrorCode);
		}
		break;

	case EMMRCRequestResourceUnallocated:
		{
		// Notification used for APR.
		iObserver.CanResume();
		}
		break;
		
	default: //unknown message
		#ifdef _DEBUG	
			ASSERT(0);
			RDebug::Print(_L("!!!!CMMRCClientImplementation::HandleMessage - Unknown message"));
		#endif
		break;
	}
	
	DP_OUT();
	}

/**
 */
TInt CMMRCClientImplementation::RegisterAsClient(TUid aEventType, const TDesC8& aNotificationRegistrationData)
	{
	DP_CONTEXT(----> CMMRCClientImplementation::RegisterAsClient *CD1*, CtxDevSound, DPLOCAL);
	DP_IN();
	
	TInt err = iMMRCClientSession.RegisterAsClient(aEventType, aNotificationRegistrationData);

	DP0_RET(err, "err = %d");
	}
	
	
/**
 */
TInt CMMRCClientImplementation::CancelRegisterAsClient(TUid aEventType)
	{
	DP_CONTEXT(----> CMMRCClientImplementation::CancelRegisterAsClient *CD1*, CtxDevSound, DPLOCAL);
	DP_IN();

	TInt err = iMMRCClientSession.CancelRegisterAsClient(aEventType);

	DP0_RET(err, "err = %d");
	}
	

/**
 */
TInt CMMRCClientImplementation::WillResumePlay()
	{
	DP_CONTEXT(----> CMMRCClientImplementation::WillResumePlay *CD1*, CtxDevSound, DPLOCAL);
	DP_IN();

	TInt err = iMMRCClientSession.WillResumePlay();

	DP0_RET(err, "err = %d");
	}
	
void CMMRCClientImplementation::ResetMessages()
	{
	DP_CONTEXT(----> CMMRCClientImplementation::ResetMessages *CD1*, CtxDevSound, DPLOCAL);
	DP_IN();

	iMMRCClientSession.ResetMessages();

	DP_OUT();
	}
//EOF
