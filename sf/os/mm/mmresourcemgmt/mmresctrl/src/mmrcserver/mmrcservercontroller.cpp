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

#include "mmrcservercontroller.h"
#include "mmrcerrorcodes.h"
#include "mmrcserversession.h"
#include "mmrcserverinfo.h"
#include "mmrcserverrulemanager.h"
#include "mmrcadaptationproxy.h"
#include "mlogicalchain.h"
#include <a3f/audioprocessingunittypeuids.h>

/**
@internalComponent

This class implements a timer. 
*/	
class CARNNotificationObject : public CActive
	{
	
public:
	static CARNNotificationObject* NewL(const TInt aPriority, CMMRCServerInfo& aMMRCServerInfo);
	~CARNNotificationObject();
	void After(TTimeIntervalMicroSeconds32 aInterval);
	void Initialize();
	
protected:
	void RunL();
	void DoCancel();

private:
	CARNNotificationObject(const TInt aPriority);
	void ConstructL(CMMRCServerInfo& aMMRCServerInfo);

public:
	TInt iIndex;
	
protected:
	RTimer	iTimer;
	CMMRCServerInfo*  iMMRCServerInfo;
	};
	
//------------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------------

/**
 * Constructor
 */
CMMRCServerController::CMMRCServerController()
	{
	TRACE_CREATE();
	DP_CONTEXT(----> CMMRCServerController::CMMRCServerController *CD1*, CtxDevSound, DPLOCAL);
	DP_IN();
	
	DP_OUT();
	}

/**
 * Destructor
 */
CMMRCServerController::~CMMRCServerController()
	{
	DP_CONTEXT(----> CMMRCServerController::~CMMRCServerController *CD1*, CtxDevSound, DPLOCAL);
	DP_IN();
	
	delete iMMRCServerInfo;
	delete iMMRCServerRuleManager;
	delete iARNNotificationObject;
	
	DP_OUT();
	}

/**
 * Constructs, and returns a pointer to, a new CMMRCServerController object.
 * Leaves on failure.
 * @return CMMRCServerController* A pointer to newly created utlitly object.
 */		
CMMRCServerController* CMMRCServerController::NewL()
	{
	DP_STATIC_CONTEXT(----> CMMRCServerController::NewL *CD1*, CtxDevSound, DPLOCAL);
	DP_IN();
	
	CMMRCServerController* self = NewLC();
	CleanupStack::Pop(self);
	
	DP0_RET(self, "self = 0x%x");
	}
	
/**
 * Constructs, leaves object on the cleanup stack, and returns a pointer
 * to, a new CMMRCServerController object.
 * Leaves on failure.
 * @return CMMRCServerController* A pointer to newly created utlitly object.
 */	
CMMRCServerController* CMMRCServerController::NewLC()
	{
	DP_STATIC_CONTEXT(----> CMMRCServerController::NewLC *CD1*, CtxDevSound, DPLOCAL);
	DP_IN();
	
	CMMRCServerController* self = new(ELeave) CMMRCServerController();
	CleanupStack::PushL(self);
	self->ConstructL();
	
	DP0_RET(self, "self = 0x%x");
	}
	
/**
 * CMMRCServerController::ConstructL
 * Symbian 2nd phase constructor can leave.
 */
void CMMRCServerController::ConstructL()
	{
	DP_CONTEXT(----> CMMRCServerController::ConstructL *CD1*, CtxDevSound, DPLOCAL);
	DP_IN();
	
	iMMRCServerInfo = CMMRCServerInfo::NewL(*this);
	iMMRCServerRuleManager = CMMRCServerRuleManager::NewL(*this);
	iARNNotificationObject = CARNNotificationObject::NewL(0, *iMMRCServerInfo); //Check the value
	
	DP_OUT();
	}

/**
* Process the request pointer by a specific session
* @param CMMRCServerSession& aServerSession
* @return KerrNone in case of success of the process
*/
TInt CMMRCServerController::EnqueueRequestL(CMMRCServerSession& aServerSession)
	{
	DP_CONTEXT(----> CMMRCServerController::ProcessRequestL *CD1*, CtxDevSound, DPLOCAL);
	DP_IN();
	
	//add the request to the queue
	iMMRCServerInfo->PushRequestL(aServerSession);
	TInt err = ProcessNextRequestL();
	
	DP0_RET(err, "err=%d");
	}

/**
 * Process the first request with the highest priority from the FIFO queue
 * @return KerrNone in case of success of the process
 */
 TInt CMMRCServerController::ProcessNextRequestL()
	{
	DP_CONTEXT(--------> CMMRCServerController::ProcessNextRequestL *CD1*, CtxDevSound, DPLOCAL);
	DP_IN();
	
	TInt err = KErrNone;

	TUint64 contextId;
	TMMRCServerState serverState;
	iMMRCServerInfo->ServerState(serverState, contextId);
#ifdef SYMBIAN_MULTIMEDIA_ENABLE_LOGGING
		RDebug::Print(_L("serverState    = %d"),  serverState);
		RDebug::Print(_L("size of message queue    = %d"),  iMMRCServerInfo->RequestsCount());
		RDebug::Print(_L("size of inprocess queue = %d"),  iMMRCServerInfo->AllocatedResourceContextsCount());
		RDebug::Print(_L("size of paused queue = %d"),  iMMRCServerInfo->AllocatedPausedContextsCount());
#endif
	if(EMMRCServerStateIdle != serverState)
		{
		DP0_RET(KErrNone, "%d");
		}
	
	CMMRCServerSession* serverSession = iMMRCServerInfo->PopAndRemoveFirstRequest(); //PopAndRemoveRequestFirstHighestPriority();
	//Pop the next next commited message with the highest priority
	if( NULL == serverSession )
		{
		DP0_RET(KErrNone, "%d");
		}
	
	if (!serverSession->HasUserEnvironmentCapability())
		{
		/* Check to see if it's a record use case and in IDLE, Active, or PRIME.
		// in which case you should refuse the request for reasons 
		KErrPermissionDenied
		*/
		MLogicalChain* requestedChange = serverSession->LogicalChainRequestedStack();
		if ((requestedChange!=NULL) && (requestedChange->StreamState() > EInitialized)) // trying to play
			{
			TInt count = requestedChange->AudioProcessingUnitsCount();
			for (TInt i=0; i<count; ++i)
				{
				if (KUidAudioDeviceSource == requestedChange->AudioProcessingUnitUid(i))
					{		
					serverSession->NotifyClientOfError(KErrPermissionDenied);
					// The following return value, doesn't actually make it back to the client
					// We do however Notify the client above using NotifyClientOfError()
					DP0_RET(KErrPermissionDenied, "%d");
					}
				}			
			}

		}
	
	//Pop the last commited message in process = the current resource in use
	CMMRCServerSession* serverSessionAllocatedResource = iMMRCServerInfo->PopFirstAllocatedResourceContext();
	//If no resource locked...
	if( NULL == serverSessionAllocatedResource )
		{
		
		err = SendResourceRequest ( *serverSession );
		}
	else //If a resource has been allocated or is used, possibly preemption
		{
		TAudioContextId contextId = serverSession->ContextId( );
		TAudioContextId contextIdAllocatedResource = serverSessionAllocatedResource->ContextId( );
	#ifdef SYMBIAN_MULTIMEDIA_ENABLE_LOGGING	
			RDebug::Print(_L("contextId          = %d"),  contextId.Id());
			RDebug::Print(_L("contextIdAllocatedResource = %d"),  contextIdAllocatedResource.Id());
	#endif

		//in case of processing the same context, remove the last committed request from the queue
		// of in-process request to avoid preemption within the same context.
		if(contextId == contextIdAllocatedResource)
			{
			err = SendResourceRequest ( *serverSession );
			}
		else // maybe preemption
			{
			//Pop the rule
			
			MLogicalChain* logicalChainUsingResource = serverSessionAllocatedResource->LogicalChainLastCommited();
			MLogicalChain* logicalChainNew = serverSession->LogicalChainRequestedStack();
			//Should not/cannot be null! 
			if( NULL==logicalChainUsingResource || NULL==logicalChainNew )
				{
			#ifdef _DEBUG	
					RDebug::Print(_L("!!!!CMMRCServerController::ProcessNextRequestL - logicalChainUsingResource or logicalChainNew NULL"));
					ASSERT(0);
			#endif
				User::Leave(EMMRCErrorLogicalChainNull);
				}
			
			TAudioState audioStateAllocatedResource = logicalChainUsingResource->StreamState();
			TAudioState audioStateNew = logicalChainNew->StreamState ( );
			TMMRCRule rule = DecisionRule(audioStateAllocatedResource, audioStateNew);
			//Process according to the rule
			if( rule == EAlwaysProcess )
				{
				err = SendResourceRequest( *serverSession );
				}
			else // process only in case of higher priority
				{
				TInt currentClientPriority = logicalChainUsingResource->Priority();
				TInt newClientPriority = logicalChainNew->Priority();
				TBool currentClient_HasMMDD = serverSessionAllocatedResource->HasMultimediaCapability();
				TBool newClient_HasMMDD = serverSession->HasMultimediaCapability();

				// If new client has multimedia capability and current doesn't have, 
				// priorities don't matter preempt the current client
				
				// But if both are equal competitors (both has MMDD capability or both hasn't)
				// and the newcomer has higher priority preempt too
				if ( (!currentClient_HasMMDD && newClient_HasMMDD)  || 
					( (currentClient_HasMMDD == newClient_HasMMDD) && 
					  (newClientPriority > currentClientPriority) ) )
					{
					//The process is delayed. First we have to preempt the current inprocess resource
					// so puch back the request (its session)
					iMMRCServerInfo->PushRequestL(*serverSession);
					//
					//send a premption message to the last commited message session
					err = SendPreemptionRequest ( *serverSessionAllocatedResource );
					}
				else 
					{
					// Deny the resource
					TAudioContextId iId = serverSession->ContextId( );
					TBool iIsOnPauseList = iMMRCServerInfo->IsOnPauseList(iId);
					if(!iIsOnPauseList)
						{
						iMMRCServerInfo->PushPausedContextL(*serverSession);
						}
// nothing pending, deny resources, and notify client.						
					serverSession->AdaptationRequestAcknowledgment(EMMRCAdaptationToServerResourceIncomplete, KErrInUse);
// Server is back to idle.
					iMMRCServerInfo->SwitchServerState(EMMRCServerStateIdle, 0);
					}
				}
			}
		}
	
#ifdef SYMBIAN_MULTIMEDIA_ENABLE_LOGGING	
	if(err != KErrNone)
		{
		RDebug::Print(_L("!!!!CMMRCServerController::ProcessNextRequestL - error = %d"), err);
		}
#endif
	
	DP0_RET(err, "err=%d");
	}

/**
 * Pop the decision rule on the base of the current in-process request and a new committed request
 * @param TAudioState aAudioStateAllocatedResource
 * @param TAudioState aAudioStateRequiringProcess
 * @return TMMRCRule
 */
TMMRCRule CMMRCServerController::DecisionRule(TAudioState aAudioStateAllocatedResource, TAudioState aAudioStateRequiringProcess)
	{
	DP_CONTEXT(----> CMMRCServerController::DecisionRule *CD1*, CtxDevSound, DPLOCAL);
	DP_IN();
	
	TMMRCRule rule = iMMRCServerRuleManager->DecisionRule(aAudioStateAllocatedResource, aAudioStateRequiringProcess);
	
	DP0_RET(rule, "rule=%d");
	}

/**
 * Pop the action reason on the base of the last committed request and the current one
 * @param TAudioState aAudioStateLastCommitted
 * @param TAudioState aAudioStateRequested
 * @return TReason
 */
TReason CMMRCServerController::ActionReason(TAudioState aAudioStateLastCommitted, TAudioState aAudioStateRequested)
	{
	DP_CONTEXT(----> CMMRCServerController::ActionReason *CD1*, CtxDevSound, DPLOCAL);
	DP_IN();
	
	TReason reason = iMMRCServerRuleManager->ActionReason(aAudioStateLastCommitted, aAudioStateRequested);
	
	DP0_RET(reason, "reason=%d");
	}

/**
 * Send a resource request to the adaptation
 * @param MLogicalChain* aOldChain
 * @param MLogicalChain* aNewChain
 * @return KerrNone in case of success of the process
 */
TInt CMMRCServerController::SendResourceRequest( CMMRCServerSession& aServerSession )
	{
	DP_CONTEXT(----> CMMRCServerController::SendResourceRequest *CD1*, CtxDevSound, DPLOCAL);
	DP_IN();
	
	iMMRCServerInfo->SwitchServerState( EMMRCServerStateWaitResourceResponseFromAdaptation, aServerSession.ContextId() );

// if entering APR state, remove from Paused list.
	TRAPD(err, aServerSession.StartProcessL());
	{
	MLogicalChain* logicalChainNew = aServerSession.LogicalChainRequested();
	if (logicalChainNew)
		{
		TAudioState audioStateNew = logicalChainNew->StreamState();
		if ( 
			(audioStateNew == EActive ) || 
			(audioStateNew == EPrimed) || 
			(audioStateNew == EIdle) 
			
			)
			{
			TInt contextId = aServerSession.ContextId( );
			iMMRCServerInfo->PopAndRemovePausedContext(contextId);
			}
		}
	}
	if(err == KErrNone)
		{
		//Send request
		if (!aServerSession.SendRequestToAdaptation(ENormalRequest))
			{
// TBD fail at current task.
// return server shutting down			
			}
		}
	
#ifdef SYMBIAN_MULTIMEDIA_ENABLE_LOGGING	
	if(err != KErrNone)
		{
		RDebug::Print(_L("!!!!CMMRCServerController::SendResourceRequest - error = %d"), err);
		}
#endif
	
	DP0_RET(err, "err=%d");
	}

/** 
 * Send a preemption request to the adaptation
 * @param MLogicalChain* aOldChain
 * @param MLogicalChain* aNewChain
 * @return KerrNone in case of success of the process
 */
TInt CMMRCServerController::SendPreemptionRequest( CMMRCServerSession& aServerSession )
	{
	DP_CONTEXT(----> CMMRCServerController::SendPreemptionRequest *CD1*, CtxDevSound, DPLOCAL);
	DP_IN();
	
	iMMRCServerInfo->SwitchServerState( EMMRCServerStateWaitPreemptionResponse, aServerSession.ContextId() );

	//Update logical chains
	TRAPD(err, aServerSession.StartPreemptionL());
		
	
	if(err == KErrNone)
		{
	//Send request
		if (!aServerSession.SendRequestToAdaptation(EPreemptionRequest))
			{
	// TBD fail at current task.
	// return server shutting down/low memory where necessary			
			}
		}
	
#ifdef SYMBIAN_MULTIMEDIA_ENABLE_LOGGING	
	if(err != KErrNone)
		{
		RDebug::Print(_L("!!!!CMMRCServerController::SendPreemptionRequest - error = %d"), err);
		}
#endif

	DP0_RET(err, "err=%d");
	}

/** 
 * Callback from the adaptation
 * @param MLogicalChain* aOldChain
 * @param MLogicalChain* aNewChain
 * @param TMMRCAdaptationToServerRequestResults aResult
 */
void CMMRCServerController::AdaptationRequestAcknowledgment(CMMRCServerSession& aServerSession, TMMRCAdaptationToServerRequestResults aResult, TInt /*aError*/)
	{
	DP_CONTEXT(--------> CMMRCServerController::AdaptationRequestAcknowledgment *CD1*, CtxDevSound, DPLOCAL);
	DP_IN();
#ifdef SYMBIAN_MULTIMEDIA_ENABLE_LOGGING	
		RDebug::Print(_L("size of message queue    = %d"),  iMMRCServerInfo->RequestsCount());
		RDebug::Print(_L("size of inprocess queue = %d"),  iMMRCServerInfo->AllocatedResourceContextsCount());
		RDebug::Print(_L("size of paused queue = %d"),  iMMRCServerInfo->AllocatedPausedContextsCount());
#endif
	
	TUint64 contextId;
	TMMRCServerState serverState;
	TInt err = KErrNone;
	iMMRCServerInfo->ServerState(serverState, contextId);
	switch(serverState)
		{
		case EMMRCServerStateWaitPreemptionResponse: // here we process the response
			{
			switch(aResult)
				{
				case EMMRCAdaptationToServerResourceComplete:
					{
					//we remove the request in process from the queue
					CMMRCServerSession* serverSession = iMMRCServerInfo->PopAndRemoveFirstAllocatedResourceContext();
					if(serverSession)
						{
						TRAP(err, iMMRCServerInfo->PushPausedContextL(*serverSession));
						DP1(DLERR,"EPushPauseContextLeft %d", err);
						__ASSERT_DEBUG(err == KErrNone, Panic(EPushPauseContextLeft));
						}
					//Do not wait for a client acknowledgment
					iMMRCServerInfo->SwitchServerState(EMMRCServerStateIdle, 0);
					}
				break;
				case EMMRCAdaptationToServerResourceTimeout:
				default:
					{
					//jjpy TODO: What if the preemption request failed?!
					// That will be specified/implemented during the integration with the resource manager
					//we remove the request in process from the queue
					iMMRCServerInfo->PopAndRemoveFirstAllocatedResourceContext();
					}
				} // end switch(aResult)
			} //end case EMMRCServerStateWaitPreemptionResponse
		break;
		case EMMRCServerStateWaitResourceResponseFromAdaptation: // here we process the response
			{
			switch(aResult)
				{
				case EMMRCAdaptationToServerResourceComplete:
					{
					CMMRCServerSession* serverSessionAllocatedResource = iMMRCServerInfo->PopFirstAllocatedResourceContext();
					if( serverSessionAllocatedResource ) //If some previously some context has allocated a resource
						{
					 	//check if same context, if not it might be an error
					 	TAudioContextId contextId = aServerSession.ContextId( );
					 	TAudioContextId contextIdAllocatedResource = serverSessionAllocatedResource->ContextId( );
					#ifdef SYMBIAN_MULTIMEDIA_ENABLE_LOGGING	
							RDebug::Print(_L("contextId          = %d"),  contextId.Id());
							RDebug::Print(_L("contextIdAllocatedResource = %d"),  contextIdAllocatedResource.Id());
					#endif
						MLogicalChain* logicalChainNew = aServerSession.LogicalChainRequested();
						MLogicalChain* logicalChainOld = aServerSession.LogicalChainLastCommited();
						TAudioState audioStateNew = logicalChainNew->StreamState ( );
					 	if(contextId != contextIdAllocatedResource)
					 		{
					 		//it is ok if the rule said process always
							MLogicalChain* logicalChainUsingResource = serverSessionAllocatedResource->LogicalChainLastCommited();
							TAudioState audioStateAllocatedResource = logicalChainUsingResource->StreamState();
							TAudioState audioStateNew = logicalChainNew->StreamState ( );
							TMMRCRule rule = DecisionRule(audioStateAllocatedResource, audioStateNew);
					 		if( rule != EAlwaysProcess )
					 			{
					 			//not possible in theory... this is an error
							#ifdef SYMBIAN_MULTIMEDIA_ENABLE_LOGGING	
									RDebug::Print(_L("!!!!CMMRCServerController::AdaptationRequestAcknowledgment - contextId allocating a resource and new request different. Should be the same!"),  iMMRCServerInfo->AllocatedResourceContextsCount());
							#endif				 		
					 			}
					 		}
					 	else //same context here, so check if the resource has been deallocated
					 		{
						 	// in case of stop etc. remove from in process request
						 	//now check the reason
							TAudioState audioStateOld = EUninitialized;
							if(logicalChainOld)
								{
								audioStateOld = logicalChainOld->StreamState();
								}
							TReason reason = ActionReason(audioStateOld, audioStateNew);
								
							if (reason == ELoad)
								{
								iARNNotificationObject->Cancel();
								iMMRCServerInfo->PopAndRemovePausedContext(contextId);
								}
							else if (reason == EUnload)
								{
								iARNNotificationObject->Initialize();
								iMMRCServerInfo->PopAndRemoveFirstAllocatedResourceContextByContextId(contextId);			
								}
					 		}
					 	}
					else //no in process request locking a resource
						{
						MLogicalChain* logicalChainNew = aServerSession.LogicalChainRequested();
						MLogicalChain* logicalChainOld = aServerSession.LogicalChainLastCommited();
						TAudioState audioStateNew = logicalChainNew->StreamState ( );
						TAudioState audioStateOld = EUninitialized;
						if(logicalChainOld)
							{
							audioStateOld = logicalChainOld->StreamState();
							}
						TReason reason = ActionReason(audioStateOld, audioStateNew);
						if( reason == ELoad ||
			 					reason == EPrimeReason ||
			 					reason == EActivate )
			 					{
			 					TRAP(err, iMMRCServerInfo->PushAllocatedResourceContextL( aServerSession ));
			 					DP1(DLERR,"EPushAllocatedResourceContextFailed %d", err);
								__ASSERT_DEBUG(err == KErrNone, Panic(EPushAllocatedResourceContextFailed));
			 					}					
						}
					iMMRCServerInfo->SwitchServerState(EMMRCServerStateIdle, 0);
					}
				break;
	 			case EMMRCAdaptationToServerResourceTimeout:
	 			default:
	 				{
					//Just switch the state
					iMMRCServerInfo->SwitchServerState(EMMRCServerStateIdle, 0);
					}
	 			break;
	 			} //end switch switch(aResult)
	 		} // end case EMMRCServerStateWaitResourceResponseFromAdaptation
	 	break;
		case EMMRCServerStateIdle: //not possible....
	 	default:
 	 	break;
	 	}
 	
	TRAP(err, ProcessNextRequestL());
	DP1(DLERR,"EProcessNextRequestLeft %d", err);
	__ASSERT_DEBUG(err == KErrNone, Panic(EProcessNextRequestLeft));
	DP_OUT();
	}

/*
 * 
 */
void CMMRCServerController::NotifyPausedClientsResourceUnallocated()
	{
 	DP_CONTEXT(----> CMMRCServerController::NotifyClientsResourceUnallocated *CD1*, CtxDevSound, DPLOCAL);
	DP_IN();
	
	CMMRCServerSession* serverSession  = iMMRCServerInfo->PopAndRemoveFirstPausedContext( );
	while( serverSession )
		{
		serverSession->NotifyClientResourceUnallocated();
		serverSession  = iMMRCServerInfo->PopAndRemoveFirstPausedContext( );
		}
	
	DP_OUT();
	}

/*
 * 
 */
void CMMRCServerController::CloseSessionByContextId(TInt32 aContextId)
	{
 	DP_CONTEXT(----> CMMRCServerController::CloseSessionByContextId *CD1*, CtxDevSound, DPLOCAL);
	DP_IN();
	
	while( iMMRCServerInfo->PopAndRemoveFirstAllocatedResourceContextByContextId( aContextId ) ){}
	while( iMMRCServerInfo->PopAndRemoveFirstRequestByContextId( aContextId ) ){}
	while( iMMRCServerInfo->PopAndRemovePausedContext( aContextId ) ){}
	while( iMMRCServerInfo->PopAndRemoveFirstAPRContextByContextId( aContextId ) ){}
	TUint64 contextId;
	TMMRCServerState serverState;
	iMMRCServerInfo->ServerState(serverState, contextId);
	if( contextId == aContextId)
		{
		iMMRCServerInfo->SwitchServerState(EMMRCServerStateIdle, 0);
		}
#ifdef SYMBIAN_MULTIMEDIA_ENABLE_LOGGING	
	RDebug::Print(_L("size of message queue    = %d"),  iMMRCServerInfo->RequestsCount());
	RDebug::Print(_L("size of inprocess queue = %d"),  iMMRCServerInfo->AllocatedResourceContextsCount());
	RDebug::Print(_L("size of paused queue = %d"),  iMMRCServerInfo->AllocatedPausedContextsCount());
	RDebug::Print(_L("size of APR queue  = %d"),  iMMRCServerInfo->AllocatedAPRContextsCount());
#endif
	
	DP_OUT();
	}

/*
 *
 */
void CMMRCServerController::ServerState(TMMRCServerState& aServerState, TUint64& aContextId) const
	{
	iMMRCServerInfo->ServerState(aServerState, aContextId);
	}
	
/**

*/
TInt CMMRCServerController::EnqueueRequestForAPRL(CMMRCServerSession& aServerSession)
	{
	DP_CONTEXT(----> CMMRCServerController::EnqueueRequestForAPRL *CD1*, CtxDevSound, DPLOCAL);
	DP_IN();
	
	//add the request to the queue
	TInt err = KErrNone;
	//if already there, should return KErrAlreadyExists

	if (iMMRCServerInfo->IsOnAPRList(aServerSession.ContextId()))
		{
		err = KErrAlreadyExists;
		}
	else
		{
		iMMRCServerInfo->PushAPRContextL(aServerSession);
		}	
	DP0_RET(err, "err=%d");
	}
	
/**

*/
TInt CMMRCServerController::RemoveRequestForAPR(CMMRCServerSession& aServerSession)
	{
	DP_CONTEXT(----> CMMRCServerController::RemoveRequestForAPR *CD1*, CtxDevSound, DPLOCAL);
	DP_IN();
	
	TInt err = KErrNone;
	TInt32 iContextId = aServerSession.ContextId();
	TBool iIsOnAPRList = iMMRCServerInfo->IsOnAPRList(iContextId);
	if(iIsOnAPRList)
		{

		// Its possible that the remove request has occured whilst clients are being notified that the resource is available
		// Check if a notification sequence is currently under way
		if ( iARNNotificationObject->IsActive() )
			{
			
			// Iterate through each paused client 
			for ( TInt index = 0 ; index < iMMRCServerInfo->AllocatedPausedContextsCount() ; index++ )
				{
				
				// Find the client that has requested removal in the list
				TAudioContextId id = iMMRCServerInfo->PausedContextByIndex(index)->ContextId();
				if ( aServerSession.ContextId() == id )
					{
					// Object has already received a notification
					if ( index < iARNNotificationObject->iIndex )
						{
						// decrement the notification objects list index to account for the item that will be removed from the queue
						--iARNNotificationObject->iIndex;
						}
						
					// no action needs to be taken if 
					// index >= iMMRCServerInfo.iIndex
					
					break;
					}
				}
			
			}

		iMMRCServerInfo->PopAndRemoveFirstAPRContextByContextId(iContextId);
		iMMRCServerInfo->PopAndRemovePausedContext(iContextId); //removing this session from the list of paused too.
		}
	else
		{
		err = KErrCancel;
		}
	
	DP0_RET(err, "err=%d");
	}

/**

*/
TInt CMMRCServerController::WillResumePlay()
	{
	DP_CONTEXT(----> CMMRCServerController::WillResumePlay *CD1*, CtxDevSound, DPLOCAL);
	DP_IN();
	
	TInt err = KErrNone;
	iARNNotificationObject->Cancel();
	
	
	DP0_RET(err, "err=%d");
	}


//From CActive
/**
 * Constructor.
 */
CARNNotificationObject::CARNNotificationObject(const TInt aPriority)
:CActive(aPriority)
	{
	TRACE_CREATE();
	DP_CONTEXT(----> CARNNotificationObject::CARNNotificationObject *CD1*, CtxDevSound, DPLOCAL);
	DP_IN();
	
	
	DP_OUT();
	}


/**
 * Destructor.
 */
CARNNotificationObject::~CARNNotificationObject()
	{
	DP_CONTEXT(----> CARNNotificationObject::~CARNNotificationObject *CD1*, CtxDevSound, DPLOCAL);
	DP_IN();

	Cancel();
	iTimer.Close();
	delete iMMRCServerInfo;
	
	DP_OUT();
	}


/**
 	CARNNotificationObject::ConstructL
 	Symbian 2nd phase constructor can leave.
 */
void CARNNotificationObject::ConstructL(CMMRCServerInfo& aMMRCServerInfo)
	{
	DP_CONTEXT(----> CARNNotificationObject::ConstructL *CD1*, CtxDevSound, DPLOCAL);
	DP_IN();
	
	iMMRCServerInfo = &aMMRCServerInfo; 
	CActiveScheduler::Add(this);
	User::LeaveIfError(iTimer.CreateLocal());
	
	
	DP_OUT();
	}

// ---------------------------------------------------------------------------
// CARNNotificationObject::NewL
// ---------------------------------------------------------------------------
CARNNotificationObject* CARNNotificationObject::NewL(const TInt aPriority, CMMRCServerInfo& aMMRCServerInfo)
	{
	DP_STATIC_CONTEXT(----> CARNNotificationObject::NewL *CD1*, CtxDevSound, DPLOCAL);
	DP_IN();
	
	CARNNotificationObject* self = new(ELeave)CARNNotificationObject(aPriority);
	CleanupStack::PushL(self);
	self->ConstructL(aMMRCServerInfo);
	CleanupStack::Pop(self);
	
	DP0_RET(self, "0x%x");
	} 

// ---------------------------------------------------------------------------
// CARNNotificationObject::RunL
// ---------------------------------------------------------------------------
void CARNNotificationObject::RunL()
	{
	DP_CONTEXT(CARNNotificationObject::RunL *CD1*, CtxDevSound, DPLOCAL);
	DP_IN();
	
	#if defined(_DEBUG) && defined(__WINS__) 
	const TTimeIntervalMicroSeconds32 KTimer = 1.8*1000000; // 1.8 seconds
	#else
	const TTimeIntervalMicroSeconds32 KTimer = 800000; // 0.8 seconds
	#endif
	
	TInt numPausedContext = iMMRCServerInfo->AllocatedPausedContextsCount();

	// The list may have been modified whilst the AO was asleep, need to check that:
	// 1) iIndex is still in range
	// 2) the object at that location should still receive a notification
	
	if ( (iIndex > -1) && (iIndex < numPausedContext) )
		{
		CMMRCServerSession* mmrcSession = iMMRCServerInfo->PausedContextByIndex(iIndex);
		
		TAudioContextId id = mmrcSession->ContextId( );
		TBool isOnAPRList = iMMRCServerInfo->IsOnAPRList(id);
		if(isOnAPRList)
			{
			mmrcSession->NotifyClientResourceUnallocated();
			}

		
		for(TInt aux = iIndex + 1; aux < numPausedContext ; aux++)
			{
			mmrcSession = iMMRCServerInfo->PausedContextByIndex(aux);
			id = mmrcSession->ContextId( );
			isOnAPRList = iMMRCServerInfo->IsOnAPRList(id);
			if(isOnAPRList)
				{
				iIndex = aux;		
				After(KTimer);
				break;
				}
			}
		}
		
	else
		{
		iIndex = 0;
		}
		
	DP_OUT();
	}

// ---------------------------------------------------------------------------
// CARNNotificationObject::After
// ---------------------------------------------------------------------------
void CARNNotificationObject::After(TTimeIntervalMicroSeconds32 aInterval)
	{
	DP_CONTEXT(CARNNotificationObject::After *CD1*, CtxDevSound, DPLOCAL);
	DP_IN();

	Cancel();
	iTimer.After(iStatus,aInterval);
	SetActive();

	DP_OUT();
	}

// ---------------------------------------------------------------------------
// CARNNotificationObject::Initialize
// ---------------------------------------------------------------------------
void CARNNotificationObject::Initialize()
	{
	DP_CONTEXT(CARNNotificationObject::Initialize *CD1*, CtxDevSound, DPLOCAL);
	DP_IN();

	#if defined(_DEBUG) && defined(__WINS__)
	const TTimeIntervalMicroSeconds32 KTimer = 1.8*1000000; // 1.8 seconds
	#else
	const TTimeIntervalMicroSeconds32 KTimer = 800000; // 0.8 seconds
	#endif

	TInt numPausedContext = iMMRCServerInfo->AllocatedPausedContextsCount();
	iIndex = 0;
	CMMRCServerSession* mmrcSession = NULL;
	
	if(numPausedContext) // don't bother if zero.
		{
		for(TInt i(0); i<numPausedContext ; ++i)
			{
			mmrcSession = iMMRCServerInfo->PausedContextByIndex(i);
			TAudioContextId id = mmrcSession->ContextId( );
			TBool isOnAPRList = iMMRCServerInfo->IsOnAPRList(id);
			if(isOnAPRList)
				{
				iIndex = i;
				After(KTimer);
				break;
				}
			}
		}

	DP_OUT();
	}

// ---------------------------------------------------------------------------
// CMMRCServer::DoCancel
// ---------------------------------------------------------------------------
void CARNNotificationObject::DoCancel()
	{
	DP_CONTEXT(CARNNotificationObject::DoCancel *CD1*, CtxDevSound, DPLOCAL);
	DP_IN();
	
	iTimer.Cancel();
	
	DP_OUT();
	}


void CMMRCServerController::Panic(TMMRCServerControllerPanicCodes aCode)
	{
	User::Panic(KMMRCServerControllerPanicCategory, aCode);
	}	
//EOF
