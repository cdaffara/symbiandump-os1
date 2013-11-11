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
// mmrcserverphysicaladaptation.cpp
// 
//


#include "mmrcadaptationproxy.h"
#include "mmrcservercontroller.h"
#include "mmrcserversession.h"
#include "mlogicalchain.h"
#include "audiostreamadaptation.h"

#include <a3f/maudiocontext.h>



//------------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------------
/**
 * By default Symbian 2nd phase constructor is private.
 * @param CMMRCServerController& A reference on the MMRC Server controller
 */
CMMRCClientSideServerRequestToAdaptationAO::CMMRCClientSideServerRequestToAdaptationAO()
: CActive(EPriorityHigh)
	{
	TRACE_CREATE();
	DP_CONTEXT(----> CMMRCClientSideServerRequestToAdaptationAO::CMMRCClientSideServerRequestToAdaptationAO *CD1*, CtxDevSound, DPLOCAL);
	DP_IN();
	DP_OUT();
	}

/**
 * Destructor
 */
CMMRCClientSideServerRequestToAdaptationAO::~CMMRCClientSideServerRequestToAdaptationAO()
	{
	DP_CONTEXT(----> CMMRCClientSideServerRequestToAdaptationAO::~CMMRCClientSideServerRequestToAdaptationAO *CD1*, CtxDevSound, DPLOCAL);
	DP_IN();
	
	Cancel();
	TidyUp();
	
	DP_OUT();
	}

/**
 * Constructs, and returns a pointer to, a new CMMRCClientSideServerRequestToAdaptationAO object.
 * Leaves on failure.
 * @param CMMRCServerController& A reference on the MMRC Server controller
 * @return CMMRCClientSideServerRequestToAdaptationAO* A pointer to newly created utlitly object.
 */		
EXPORT_C CMMRCClientSideServerRequestToAdaptationAO* CMMRCClientSideServerRequestToAdaptationAO::NewL()
	{
	DP_STATIC_CONTEXT(----> CMMRCClientSideServerRequestToAdaptationAO::NewL *CD1*, CtxDevSound, DPLOCAL);
	DP_IN();
	
	CMMRCClientSideServerRequestToAdaptationAO* self = new(ELeave) CMMRCClientSideServerRequestToAdaptationAO();
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	
	DP0_RET(self, "self = 0x%x");
	}
	
void CMMRCClientSideServerRequestToAdaptationAO::Initialize(CMMRCServerSession* aServerSession, CMMRCServerSideAdaptationResponseAO* aCallback, 
					RThread* aClientThread, const CFourCCConvertor& aFourCCConvertor)
	{
	DP_CONTEXT(----> CMMRCClientSideServerRequestToAdaptationAO::SetCallback *CD1*, CtxDevSound, DPLOCAL);
	DP_IN();
	
	iMMRCServerSideAdaptationResponseAO = aCallback;
	iServerSession = aServerSession;
	iClientThread = aClientThread;
	
	iStreamAdaptation->SetFourCC(aFourCCConvertor);
	
	DP_OUT();
	}

/**
CMMRCClientSideServerRequestToAdaptationAO::ConstructL
Symbian 2nd phase constructor can leave.
 */
void CMMRCClientSideServerRequestToAdaptationAO::ConstructL()
	{
 	DP_CONTEXT(----> CMMRCClientSideServerRequestToAdaptationAO::ConstructL *CD1*, CtxDevSound, DPLOCAL);
	DP_IN();
	
	CActiveScheduler::Add(this);
	iStreamAdaptation = CAudioStreamAdaptation::NewL(TUid::Uid(0x10283461)) ;
	iStreamAdaptation->RegisterAudioStreamObserver(*this);

	iStatus = KRequestPending;
	SetActive();
	
	DP_OUT();
	}

/**
 * Send a request to the resource manager
 * @param MLogicalChain* aOldChain the last committed chain
 * @param MLogicalChain* aNewChain the current committed chain
 * @param TRequestStatus& aStatus
 * @return KErrNone in case of success of the send
 */
void CMMRCClientSideServerRequestToAdaptationAO::Service(TUint aRequestType)
	{
	DP_CONTEXT(----> CMMRCClientSideServerRequestToAdaptationAO::Service *CD1*, CtxDevSound, DPLOCAL);
	DP_IN();
	iRequestType = aRequestType;
	iAudioContext = const_cast<MAudioContext*>(iServerSession->AudioContext());
	iLogicalChainLastCommited  = iServerSession->LogicalChainLastCommited();
	iLogicalChainRequested = iServerSession->LogicalChainRequested();
	
	// Simulate an asyncronous response
/*
unsafe as checking a client-thread object from server thread.	
if (IsActive())

Rely on serversession making this call safe.
*/
	
		{
		TRequestStatus* status = &iStatus;
DP1(2,"clientsideAO event",status);
		iClientThread->RequestComplete(status, KErrNone);
		}
/*	else
		{
		DP0(2,"shouldneverhappen!!");
		ASSERT(0);
		}
*/
	DP_OUT();
	}

/**
 * Callback in case of timeout
 */
void CMMRCClientSideServerRequestToAdaptationAO::ResourceRequestAcknowledgmentTimeout( )
	{
	DP_CONTEXT(----> CMMRCClientSideServerRequestToAdaptationAO::ResourceRequestAcknowledgmentTimeout *CD1*, CtxDevSound, DPLOCAL);
	DP_IN();
	
	DP_OUT();
	}

void CMMRCClientSideServerRequestToAdaptationAO::TidyUp()
	{
/*
This is a place to put synchronous tidyup code.
See below first attempt.
*/	
	
/*	if(iStreamAdaptation)
		{
		iStreamAdaptation->UnregisterAllAudioStreamObserver();

		if (iLogicalChainLastCommited)
			{
// never got past uninitialised.  delete.			
			CLogicalAudioStream* audiostream = static_cast<CLogicalAudioStream*>(iLogicalChainLastCommited);
			TAudioState state = audiostream->StreamState();
			if (state != EUninitialized) && (iLogicalChainLastCommited)) 
				{
				CLogicalAudioStream* audiostream = static_cast<CLogicalAudioStream*>(iLogicalChainRequested);
				TAudioState state = audiostream->StreamState();
				audiostream->ResetMessage();
				audiostream->SetMessageType(EComponentDestruction);
				switch(state)
					{
					case EActive:
					case EPrimed:
						{
						audiostream->Stop();
						} // deliberate fallthru'.
					case EIdle:
						{
						audiostream->Unload();
						} // deliberate fallthru'.
					case EInitialized:
						{
						audiostream->Uninitialize();
						iStreamAdaptation->Message(*iLogicalChainLastCommited, *iLogicalChainRequested, *iAudioContext, ETrue);	
						waitForAdaptationResponse=EFalse;
		}
						break;
					default:
			{
			// No need to wait until adaptation responds... just kill.
						waitForAdaptationResponse=EFalse;
						}
						break;
					}	
			}
		}
*/	delete iStreamAdaptation;
	};
	

/**
 * CMMRCClientSideServerRequestToAdaptationAO::RunL
 */

void CMMRCClientSideServerRequestToAdaptationAO::RunL()
	{
	DP_CONTEXT(--------> CMMRCClientSideServerRequestToAdaptationAO::RunL *CD1*, CtxDevSound, DPLOCAL);
	DP_IN();

/*
flag is safe to read in A.O as it is set synchronously by an A.O running
in the same Active scheduler.
*/
	// no shut down... regular call.	
	iStatus = KRequestPending;
	SetActive();
	iRequestsProcessed++;
	
	TUint flags = 0;
	if(iRequestType == EPreemptionRequest)
		{
		flags |= KPreemptionRequest;
		}
	
	iStreamAdaptation->Message(*iLogicalChainLastCommited, *iLogicalChainRequested, *iAudioContext, flags);
	DP_OUT();
	}

/**
 	CMMFDevSoundServer::CDelayDevSoundShutDown::DoCancel
 	Called by Active object framework when user cancels active object.
 	(other items were commented in a header).
 */
void CMMRCClientSideServerRequestToAdaptationAO::DoCancel()
	{
	DP_CONTEXT(----> CMMRCClientSideServerRequestToAdaptationAO::DoCancel *CD1*, CtxDevSound, DPLOCAL);
	DP_IN();
/* This is "rather complicated" :-)
	
This AO always sets itself active, and hence this method will be called
under all deletion conditions.

As AO and signaller are in different threads, and in Symbian there is
no official manner by which to ascertain if you have been signalled, 
we have a synchronous call (across client and server threads) which
simply sets a flag and completes.  The flag prevents any further
requests being sent to the adaptation.

By this mechanism, it is possible to read the number of requests
sent in the server thread as the number does not change after the
sync call.  This is necessary as if there was no outstanding request
this A.O needs to self-signal.
*/	

/* 
This assert ensures that the synchronous client->server call occurred
and therefore it is safe to do the next comparison.  Necessary in 
case someone re-orders the MMRC client deletion later.
*/
	ASSERT(iServerSession->IsServerShuttingDown()); 
	
/* check to see if we have processed all of the requests that 
have been sent by the server - i.e. are there any pending requests

if (no outstanding requests)
	we need to self-signal or this would hang.
otherwise
	we need to signal the server-side A.O to terminate that.
*/
	if (iServerSession->RequestsSent() != iRequestsProcessed)
		{
// should throw an error, rather than KErrNone	
//		iMMRCServerSideAdaptationResponseAO->ServiceL( EMMRCAdaptationToServerShutdown, KErrServerTerminated);		
		iMMRCServerSideAdaptationResponseAO->Service( EMMRCAdaptationToServerShutdown, KErrNone);		
		}
	else
		{
/* no more requests will ever come... self signal.
Don't notify MMRC server A.O. - it won't be expecting one
*/
		TRequestStatus* status(&iStatus);
		User::RequestComplete(status, KErrNone);
		}
	DP_OUT();
	}

/**
 * 
 */
void CMMRCClientSideServerRequestToAdaptationAO::PhysicalAdaptationEvent(TPhysicalEvent aEvent, TInt aError)
	{
	DP_CONTEXT(----> CMMRCClientSideServerRequestToAdaptationAO::PhysicalAdaptationEvent *CD1*, CtxDevSound, DPLOCAL);
	DP_IN();
	
	switch (aEvent)
		{
		case EOperationComplete:
			if(iMMRCServerSideAdaptationResponseAO)
				{
				iMMRCServerSideAdaptationResponseAO->Service( EMMRCAdaptationToServerResourceComplete, aError);
				}
			break;
			
		case EOperationIncomplete:
			if(iMMRCServerSideAdaptationResponseAO)
				{
				iMMRCServerSideAdaptationResponseAO->Service( EMMRCAdaptationToServerResourceIncomplete, aError);
				}
			break;
			
		default:
			break;
		}
	
	DP_OUT();
	}

/**
 * 
 */
void CMMRCClientSideServerRequestToAdaptationAO::StateEvent(TInt /*aReason*/, TAudioState /*aNewState*/)
	{
	DP_CONTEXT(----> CMMRCClientSideServerRequestToAdaptationAO::StateEvent *CD1*, CtxDevSound, DPLOCAL);
	DP_IN();
	
	DP_OUT();
	}

/**
 * 
 */
void CMMRCClientSideServerRequestToAdaptationAO::AddProcessingUnitComplete(TUid /*aType*/, TInt /*aError*/)
	{
	DP_CONTEXT(----> CMMRCClientSideServerRequestToAdaptationAO::AddProcessingUnitComplete *CD1*, CtxDevSound, DPLOCAL);
	DP_IN();
	
	DP_OUT();
	}

/**
 * 
 */
void CMMRCClientSideServerRequestToAdaptationAO::RemoveProcessingUnitComplete(TUid /*aType*/, TInt /*aError*/)
	{
	DP_CONTEXT(----> CMMRCClientSideServerRequestToAdaptationAO::RemoveProcessingUnitComplete *CD1*, CtxDevSound, DPLOCAL);
	DP_IN();
	
	DP_OUT();
	}

/**
 * 
 */
void CMMRCClientSideServerRequestToAdaptationAO::ProcessingFinished()
	{
	DP_CONTEXT(----> CMMRCClientSideServerRequestToAdaptationAO::ProcessingFinished *CD1*, CtxDevSound, DPLOCAL);
	DP_IN();
	
	DP_OUT();
	}

/**
 * 
 */
void CMMRCClientSideServerRequestToAdaptationAO::FlushComplete(TInt /*aError*/)
	{
	DP_CONTEXT(----> CMMRCClientSideServerRequestToAdaptationAO::FlushComplete *CD1*, CtxDevSound, DPLOCAL);
	DP_IN();
	
	DP_OUT();
	}


//------------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------------
/**
 */
CMMRCServerSideAdaptationResponseAO::CMMRCServerSideAdaptationResponseAO()
: CActive(EPriorityHigh)
	{
	TRACE_CREATE();
	DP_CONTEXT(----> CMMRCServerSideAdaptationResponseAO::CMMRCServerSideAdaptationResponseAO *CD1*, CtxDevSound, DPLOCAL);
	DP_IN();
	DP_OUT();
	}

/**
 */
CMMRCServerSideAdaptationResponseAO::~CMMRCServerSideAdaptationResponseAO()
	{
	DP_CONTEXT(----> CMMRCServerSideAdaptationResponseAO::~CMMRCServerSideAdaptationResponseAO *CD1*, CtxDevSound, DPLOCAL);
	DP_IN();
	
	Cancel();
	
	DP_OUT();
	}

/**
 */
CMMRCServerSideAdaptationResponseAO* CMMRCServerSideAdaptationResponseAO::NewL()
	{
	DP_STATIC_CONTEXT(----> CMMRCServerSideAdaptationResponseAO::NewL *CD1*, CtxDevSound, DPLOCAL);
	DP_IN();
	
	CMMRCServerSideAdaptationResponseAO* self = new(ELeave) CMMRCServerSideAdaptationResponseAO();
	CleanupStack::PushL( self );
	self->ConstructL();
	CleanupStack::Pop(self);
	
	DP0_RET(self, "self = 0x%x");
	}

/**
 */
void CMMRCServerSideAdaptationResponseAO::ConstructL()
	{
	DP_CONTEXT(----> CMMRCServerSideAdaptationResponseAO::ConstructL *CD1*, CtxDevSound, DPLOCAL);
	DP_IN();
	
	CActiveScheduler::Add (this );
	
	DP_OUT();
	}

/**
 */
void CMMRCServerSideAdaptationResponseAO::Initialize(CMMRCServerSession* aServerSession, RThread* aServerThread )
	{
	DP_CONTEXT(----> CMMRCServerSideAdaptationResponseAO::SetServerSession *CD1*, CtxDevSound, DPLOCAL);
	DP_IN();
	
	iMMRCServerSession = aServerSession;
	ASSERT(iMMRCServerSession!=0);
	iServerThread = aServerThread;
	ASSERT(iServerThread!=0);
	DP_OUT();
	}

/**
These are rather misleading convenience functions which are called
from the context of the caller (i.e. a different thread context)

Be very cautious with what you rely on.
 */
void CMMRCServerSideAdaptationResponseAO::Service(TMMRCAdaptationToServerRequestResults aEvent, TInt aError)
	{
	DP_CONTEXT(----> CMMRCServerSideAdaptationResponseAO::Service *CD1*, CtxDevSound, DPLOCAL);
	DP_IN();
	
	iEvent = aEvent;
	iError = aError;
	
	// Simulate an asynchronous response on correct thread.
		TRequestStatus* status = &iStatus;
		iServerThread->RequestComplete(status, KErrNone);
	DP_OUT();
	}
/**
 */
void CMMRCServerSideAdaptationResponseAO::DoCancel( )
	{
	DP_CONTEXT(----> CMMRCServerSideAdaptationResponseAO::DoCancel *CD1*, CtxDevSound, DPLOCAL);
	DP_IN();
/* this should only happen if there is a pending request.
    client-side will send a response before closing.		

 See DoCancel in CMMRCClientSideServerRequestToAdaptationAO for details.   
    */
	DP_OUT();
	}

/**
server-side A.O. used to observe the outcome.

Calls back AdaptationRequestAcknowledgment on MMRC server session
 from server thread context.
 */
void CMMRCServerSideAdaptationResponseAO::RunL()
	{
	DP_CONTEXT(--------> CMMRCServerSideAdaptationResponseAO::RunL *CD1*, CtxDevSound, DPLOCAL);
	DP_IN();
	iMMRCServerSession->AdaptationRequestAcknowledgment( iEvent, iError );
	DP_OUT();
		}

void CMMRCServerSideAdaptationResponseAO::StartAdaptationListening()
		{
/* wrapper functio to set up the A.O */	
		iStatus = KRequestPending;
		SetActive();
		}
//EOF
