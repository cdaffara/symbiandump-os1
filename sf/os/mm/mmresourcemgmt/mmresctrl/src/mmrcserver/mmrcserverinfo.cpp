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

#include "mmrcserverinfo.h"
#include "mmrcserversession.h"
#include "mmrcservercontroller.h"
#include "mlogicalchain.h"
#include "mmrcutil.h"
#include <a3f/a3f_trace_utils.h>


//------------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------------
/**
 * By default Symbian 2nd phase constructor is private.
 * @param CMMRCServerController& A reference on the MMRC Server controller
 */
CMMRCServerInfo::CMMRCServerInfo(CMMRCServerController& aServerController)
:	iMMRCServerController( aServerController)
	{
	TRACE_CREATE();
	DP_CONTEXT(----> CMMRCServerInfo::CMMRCServerInfo *CD1*, CtxDevSound, DPLOCAL);
	DP_IN();
	
	DP_OUT();
	}


/**
 * destructor
 */
CMMRCServerInfo::~CMMRCServerInfo()
	{
	DP_CONTEXT(----> CMMRCServerInfo::~CMMRCServerInfo *CD1*, CtxDevSound, DPLOCAL);
	DP_IN();
	
	iMMRCAllocatedResourceContextsQueue.ResetAndDestroy();
	iMMRCContextsQueue.ResetAndDestroy();
	iMMRCAPRContextsQueue.ResetAndDestroy();
	iMMRCPausedContextsQueue.ResetAndDestroy();
	
	DP_OUT();
	}

/**
 * Constructs, and returns a pointer to, a new CMMRCServerInfo object.
 * Leaves on failure.
 * @param CMMRCServerController& A reference on the MMRC Server controller
 * @return CMMRCServerInfo* A pointer to newly created utlitly object.
 */	
CMMRCServerInfo* CMMRCServerInfo::NewL(CMMRCServerController& aServerController)
	{
	DP_STATIC_CONTEXT(----> CMMRCServerInfo::NewL *CD1*, CtxDevSound, DPLOCAL);
	DP_IN();
	
	CMMRCServerInfo* self = NewLC(aServerController);
	CleanupStack::Pop(self);

	DP0_RET(self, "self = 0x%x");
	}
	
/**
 * Constructs, leaves object on the cleanup stack, and returns a pointer
 * to, a new CMMRCServerInfo object.
 * Leaves on failure.
 * @param CMMRCServerController& A reference on the MMRC Server controller
 * @return CMMRCServerInfo* A pointer to newly created utlitly object.
 */	
CMMRCServerInfo* CMMRCServerInfo::NewLC(CMMRCServerController& aServerController)
	{
	DP_STATIC_CONTEXT(----> CMMRCServerInfo::NewLC *CD1*, CtxDevSound, DPLOCAL);
	DP_IN();
	
	CMMRCServerInfo* self=new(ELeave) CMMRCServerInfo(aServerController);
	CleanupStack::PushL(self);
	self->ConstructL(  );

	DP0_RET(self, "self = 0x%x");
	}
	
/**
 	CMMRCServerInfo::ConstructL
 	Symbian 2nd phase constructor can leave.
 */
void CMMRCServerInfo::ConstructL()
	{
	DP_CONTEXT(----> CMMRCServerInfo::ConstructL *CD1*, CtxDevSound, DPLOCAL);
	DP_IN();
	
	DP_OUT();
	}
	
/** 
 * Change the state of the server
 * @param TMMRCServerState aNewState
 */
void CMMRCServerInfo::SwitchServerState(TMMRCServerState aNewState, TUint64 aContextId)
	{
	DP_CONTEXT(----> CMMRCServerInfo::SwitchServerState *CD1*, CtxDevSound, DPLOCAL);
	DP_IN();
	
	iMMRCServerInfoState = aNewState;
	iLastTransactionContextId = aContextId;
	
	DP_OUT();
	}

/** 
 * return the current server state
 * @return TMMRCServerState& aServerState
 */
void CMMRCServerInfo::ServerState(TMMRCServerState& aServerState, TUint64& aContextId) const
	{
	DP_CONTEXT(----> CMMRCServerInfo::ServerState *CD1*, CtxDevSound, DPLOCAL);
	DP_IN();
	
	aContextId = iLastTransactionContextId;
	aServerState = iMMRCServerInfoState;
	
	DP_OUT();
	}

//------------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------------

/**
 * Push an Context on the Context queue in parameter
 * @param RMMRCContextsQueueTD& aContextsQueue
 * @param CMMRCServerSession& aServerSession
 * @leave in case of memory error
 */
void CMMRCServerInfo::PushContextL(RMMRCContextsQueueTD& aContextsQueue, const CMMRCServerSession& aServerSession )
	{
	DP_CONTEXT(----> CMMRCServerInfo::PushContextL *CD1*, CtxDevSound, DPLOCAL);
	DP_IN();

	aContextsQueue.PushL(&aServerSession);
	
	DP_OUT();
	}

/**
 * Pop and remove the first Context of the highest priority from the Context queue in parameter
 * @param RMMRCContextsQueueTD& aContextsQueue
 * @return CMMRCServerSession* the session of the Context
 */
CMMRCServerSession* CMMRCServerInfo::PopAndRemoveFirstContext(RMMRCContextsQueueTD& aContextsQueue)
	{
	DP_CONTEXT(----> CMMRCServerInfo::PopAndRemoveFirstContext *CD1*, CtxDevSound, DPLOCAL);
	DP_IN();
	
	CMMRCServerSession* serverSession = 0;
	//PopAndRemove return null if the queue is empty;
	serverSession = aContextsQueue.PopAndRemove();
		
	DP0_RET(serverSession, "serverSession = 0x%x");
	}

/**
 * Pop and remove the first Context from the Context queue in parameter by contextId
 * @param RMMRCContextsQueueTD& aContextsQueue
 * @param const TAudioContextId& aContextId
 * @return CMMRCServerSession* the session of the Context
 */
CMMRCServerSession* CMMRCServerInfo::PopAndRemoveFirstContextByContextId(RMMRCContextsQueueTD& aContextsQueue, const TAudioContextId& aContextId )
	{
	DP_CONTEXT(----> CMMRCServerInfo::PopAndRemoveFirstContextByContextId *CD1*, CtxDevSound, DPLOCAL);
	DP_IN();
	
	CMMRCServerSession* serverSession = NULL;
	
	TInt nbContexts = aContextsQueue.Count();
	for( TInt i = 0 ; i < nbContexts ; i++ )
		{
		serverSession = aContextsQueue[i];
		
		TAudioContextId contextId = serverSession->ContextId();
		if( aContextId == contextId )
			{
			aContextsQueue.PopAndRemove(i);
			break;
			}
		else
			{
			serverSession = NULL;
			}
		}
	
	DP0_RET(serverSession, "serverSession = 0x%x");
	}


/**
 * Pop and remove the first Context of the highest priority from the Context queue in parameter
 * @param RMMRCContextsQueueTD& aContextsQueue
 * @return CMMRCServerSession* the session of the Context
 */
CMMRCServerSession* CMMRCServerInfo::PopAndRemoveContextFirstHighestPriority(RMMRCContextsQueueTD& aContextsQueue)
	{
	DP_CONTEXT(----> CMMRCServerInfo::PopAndRemoveContextFirstHighestPriority *CD1*, CtxDevSound, DPLOCAL);
	DP_IN();
	
	TInt nbContexts = aContextsQueue.Count();
	if( 0 == nbContexts )
		{
		return NULL;
		}
	
	TInt highestPriority = 0;
	TInt sessionIdx = 0;
	for( TInt i = 0 ; i < nbContexts ; i++ )
		{
		const CMMRCServerSession* srvSession = aContextsQueue[i];
		// LogicalChainLastCommited or LogicalChainRequested is not null necesseraly
		const MLogicalChain* logicalChain = srvSession->LogicalChainRequested();
		if(!logicalChain)
			{
			logicalChain = srvSession->LogicalChainLastCommited();			
			}
		if(!logicalChain)
			{
			return NULL;
			}
// AM Getting the priority
		TInt currentChainPriority = (const_cast<MLogicalChain*>(logicalChain))->Priority();
		if( currentChainPriority > highestPriority )
			{
			highestPriority = currentChainPriority;
			sessionIdx = i;
			}

			
// JIn edited as these settings have been removed 
/*		TAudioTypeSettings audioTypeSettings = (logicalChain->ChainConfiguration()).iAudioTypeSettings;
		if( audioTypeSettings.iPriority > highestPriority )
			{
			highestPriority = audioTypeSettings.iPriority;
			sessionIdx = i;
			}
*/	

// AM 
	// highestPriority = 5;
	// sessionIdx = i;

//EOJin	
		}
	CMMRCServerSession* serverSession = aContextsQueue.PopAndRemove(sessionIdx);
	
	DP0_RET(serverSession, "serverSession=0x%x");
	}

/**
 * Pop and remove the first Context of the highest priority and more than aPriority 
 * from the Context queue in parameter
 * @param RMMRCContextsQueueTD& aContextsQueue
 * @param TInt aPriority
 * @return CMMRCServerSession* the session of the Context
 */
CMMRCServerSession* CMMRCServerInfo::PopAndRemoveContextFirstHighestPriority(RMMRCContextsQueueTD& aContextsQueue, TInt /*aPriority*/ )
	{
	DP_CONTEXT(----> CMMRCServerInfo::PopAndRemoveContextFirstHighestPriority *CD1*, CtxDevSound, DPLOCAL);
	DP_IN();
	
	CMMRCServerSession* serverSession = NULL;
	TInt nbContexts = aContextsQueue.Count();
	if( 0 == nbContexts )
		{
		return NULL;
		}
	
	TInt highestPriority = 0;
	TInt sessionIdx = 0;
	for( TInt i = 0 ; i < nbContexts ; i++ )
		{
		const CMMRCServerSession* srvSession = aContextsQueue[i];
		// LogicalChainLastCommited or LogicalChainRequested is not null necesseraly
		const MLogicalChain* logicalChain = srvSession->LogicalChainRequested();
		if(!logicalChain)
			{
			return NULL;
			}

// AM Getting the priority
		TInt currentChainPriority = (const_cast<MLogicalChain*>(logicalChain))->Priority();
		if( currentChainPriority > highestPriority )
			{
			highestPriority = currentChainPriority;
			sessionIdx = i;
			}



// EOJin 
		}
	
	if( sessionIdx < nbContexts )
		{
		serverSession = aContextsQueue.PopAndRemove(sessionIdx);
		}
	
DP0_RET(serverSession, "serverSession=0x%x");
	}

/**
 * Pop the first Context from the Context queue in parameter
 * @param RMMRCContextsQueueTD& aContextsQueue
 * @return CMMRCServerSession* the session of the Context
 */
CMMRCServerSession* CMMRCServerInfo::PopFirstContext(const RMMRCContextsQueueTD& aContextsQueue) const
	{
	DP_CONTEXT(----> CMMRCServerInfo::PopFirstContext *CD1*, CtxDevSound, DPLOCAL);
	DP_IN();
	
	CMMRCServerSession* serverSession = 0;
	//PopAndRemove return null if the queue is empty;
	serverSession = aContextsQueue.Pop();
		
	DP0_RET(serverSession, "serverSession=0x%x");
	}

/**
 * Pop the first Context of the highest priority from the Context queue in parameter
 * @param RMMRCContextsQueueTD& aContextsQueue
 * @return CMMRCServerSession* the session of the Context 
 */
CMMRCServerSession* CMMRCServerInfo::PopContextFirstHighestPriority(const RMMRCContextsQueueTD& aContextsQueue) const
	{
	DP_CONTEXT(----> CMMRCServerInfo::PopContextFirstHighestPriority *CD1*, CtxDevSound, DPLOCAL);
	DP_IN();
	
	TInt nbContexts = aContextsQueue.Count();
	if( 0 == nbContexts )
		{
		return NULL;
		}
	
	TInt highestPriority = 0;
	TInt sessionIdx = 0;
	for( TInt i = 0 ; i < nbContexts ; i++ )
		{
		const CMMRCServerSession* srvSession = aContextsQueue[i];
		// LogicalChainLastCommited or LogicalChainRequested is not null necesseraly
		const MLogicalChain* logicalChain = srvSession->LogicalChainRequested();
		if(!logicalChain)
			{
			logicalChain = srvSession->LogicalChainLastCommited();			
			}
		if(!logicalChain)
			{
			return NULL;
			}

// Jin edited to remove dependency on missing element
/*		
		TAudioTypeSettings audioTypeSettings = (logicalChain->ChainConfiguration()).iAudioTypeSettings;
		if( audioTypeSettings.iPriority > highestPriority )
			{
			highestPriority = audioTypeSettings.iPriority;
			sessionIdx = i;
			}
*/
// AM
/*	highestPriority = 5;
	sessionIdx = i;
	
*/	
// AM Getting the priority
		TInt currentChainPriority = (const_cast<MLogicalChain*>(logicalChain))->Priority();
		if( currentChainPriority > highestPriority )
			{
			highestPriority = currentChainPriority;
			sessionIdx = i;
			}

// EOJIn
		}
	CMMRCServerSession* serverSession = aContextsQueue.Pop(sessionIdx);
	
	DP0_RET(serverSession, "serverSession=0x%x");
	}

/**
 * Pop the first Context of the highest priority and more than aPriority 
 * from the Context queue in parameter
 * @param RMMRCContextsQueueTD& aContextsQueue
 * @param TInt aPriority
 * @return CMMRCServerSession* the session of the Context
 */
CMMRCServerSession* CMMRCServerInfo::PopContextFirstHighestPriority(const RMMRCContextsQueueTD& aContextsQueue, TInt /*aPriority*/ ) const
	{
	DP_CONTEXT(----> CMMRCServerInfo::PopContextFirstHighestPriority *CD1*, CtxDevSound, DPLOCAL);
	DP_IN();
	
	TInt nbContexts = aContextsQueue.Count();
	if( 0 == nbContexts )
		{
		return NULL;
		}
	
	TInt highestPriority = 0;
	TInt sessionIdx = 0;
	for( TInt i = 0 ; i < nbContexts ; i++ )
		{
		const CMMRCServerSession* srvSession = aContextsQueue[i];
		// LogicalChainLastCommited or LogicalChainRequested is not null necesseraly
		const MLogicalChain* logicalChain = srvSession->LogicalChainRequested();
		if(!logicalChain)
			{
			logicalChain = srvSession->LogicalChainLastCommited();			
			}
		if(!logicalChain)
			{
			return NULL;
			}
		
// JIn edited to remove dependency
/*
		TAudioTypeSettings audioTypeSettings = (logicalChain->ChainConfiguration()).iAudioTypeSettings;
		if( audioTypeSettings.iPriority > aPriority && audioTypeSettings.iPriority > highestPriority )
			{
			highestPriority = audioTypeSettings.iPriority;
			sessionIdx = i;
			}
*/

// AM New way to access 
//	highestPriority = 5;
//	sessionIdx = i;

// AM Getting the priority
		TInt currentChainPriority = (const_cast<MLogicalChain*>(logicalChain))->Priority();
		if( currentChainPriority > highestPriority )
			{
			highestPriority = currentChainPriority;
			sessionIdx = i;
			}
// EOJIn
		}
	
	CMMRCServerSession* serverSession = NULL;
	if( sessionIdx < nbContexts )
		{
		serverSession = aContextsQueue.Pop(sessionIdx);
		}
	
	DP0_RET(serverSession, "serverSession=0x%x");
	}

//------------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------------

/** 
 * Push an Context on the Context queue
 * @param CMMRCServerSession& aServerSession
 * @leave in case of memory error
 */
void CMMRCServerInfo::PushRequestL(const CMMRCServerSession& aServerSession )
	{
	DP_CONTEXT(----> CMMRCServerInfo::PushRequestL *CD1*, CtxDevSound, DPLOCAL);
	DP_IN();
	
	PushContextL(iMMRCContextsQueue, aServerSession);

	DP_OUT();
	}

/** 
 * Pop and remove the first Context from the Context queue
 * @return CMMRCServerSession* the session of the Context
 */
CMMRCServerSession* CMMRCServerInfo::PopAndRemoveFirstRequest()
	{
	DP_CONTEXT(----> CMMRCServerInfo::PopAndRemoveFirstRequest *CD1*, CtxDevSound, DPLOCAL);
	DP_IN();
	
	CMMRCServerSession* mmrcSession =  PopAndRemoveFirstContext(iMMRCContextsQueue);

	DP0_RET(mmrcSession, "mmrcSession = 0x%x");
	}

/** 
 * Pop and remove the first Context from the Context queue by contextId
 * @param const TAudioContextId& aContextId
 * @return CMMRCServerSession* the session of the Context
 */
CMMRCServerSession* CMMRCServerInfo::PopAndRemoveFirstRequestByContextId(const TAudioContextId& aContextId )
	{
	DP_CONTEXT(----> CMMRCServerInfo::PopAndRemoveFirstRequestByContextId *CD1*, CtxDevSound, DPLOCAL);
	DP_IN();
	
	CMMRCServerSession* mmrcSession = PopAndRemoveFirstContextByContextId(iMMRCContextsQueue, aContextId);

	DP0_RET(mmrcSession, "mmrcSession = 0x%x");
	}


/** 
 * Pop and remove the first Context of the highest priority from the Context queue
 * @param TInt aPriority
 * @return CMMRCServerSession* the session of the Context
 */
CMMRCServerSession* CMMRCServerInfo::PopAndRemoveRequestFirstHighestPriority()
	{
	DP_CONTEXT(----> CMMRCServerInfo::PopAndRemoveRequestFirstHighestPriority *CD1*, CtxDevSound, DPLOCAL);
	DP_IN();
	
	CMMRCServerSession* mmrcSession = PopAndRemoveContextFirstHighestPriority(iMMRCContextsQueue);

	DP0_RET(mmrcSession, "mmrcSession = 0x%x");
	}

/** 
 * Pop and remove the first Context of the highest priority and more than aPriority 
 * from the Context queue
 * @param TInt aPriority
 * @return CMMRCServerSession* the session of the Context
 */
CMMRCServerSession* CMMRCServerInfo::PopAndRemoveRequestFirstHighestPriority( TInt aPriority )
	{
 	DP_CONTEXT(----> CMMRCServerInfo::PopAndRemoveRequestFirstHighestPriority *CD1*, CtxDevSound, DPLOCAL);
	DP_IN();
	
	CMMRCServerSession* mmrcSession = PopAndRemoveContextFirstHighestPriority(iMMRCContextsQueue, aPriority);

	DP0_RET(mmrcSession, "mmrcSession = 0x%x");
	}

/** 
 * Pop the first Context from the Context queue
 * @return CMMRCServerSession* the session of the Context
 */
CMMRCServerSession* CMMRCServerInfo::PopFirstRequest() const
	{
	DP_CONTEXT(----> CMMRCServerInfo::PopFirstRequest *CD1*, CtxDevSound, DPLOCAL);
	DP_IN();
	
	CMMRCServerSession* mmrcSession = PopFirstContext(iMMRCContextsQueue);

	DP0_RET(mmrcSession, "mmrcSession = 0x%x");
	}

/** 
 * Pop the first Context of the highest priority from the Context queue
 * @return CMMRCServerSession* the session of the Context
 */
CMMRCServerSession* CMMRCServerInfo::PopRequestFirstHighestPriority() const
	{
	DP_CONTEXT(----> CMMRCServerInfo::PopRequestFirstHighestPriority *CD1*, CtxDevSound, DPLOCAL);
	DP_IN();
	
	CMMRCServerSession* mmrcSession = PopContextFirstHighestPriority(iMMRCContextsQueue);

	DP0_RET(mmrcSession, "mmrcSession = 0x%x");
	}

/** 
 * Pop the first Context of the highest priority and more than aPriority 
 * from the Context queue
 * @param TInt aPriority
 * @return CMMRCServerSession* the session of the Context
 */
CMMRCServerSession* CMMRCServerInfo::PopRequestFirstHighestPriority(TInt aPriority ) const
	{
	DP_CONTEXT(----> CMMRCServerInfo::PopRequestFirstHighestPriority *CD1*, CtxDevSound, DPLOCAL);
	DP_IN();
	
	CMMRCServerSession* mmrcSession = PopContextFirstHighestPriority(iMMRCContextsQueue, aPriority);

	DP0_RET(mmrcSession, "mmrcSession = 0x%x");
	}

//------------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------------

/** 
 * Push an Context on the in-process Context queue
 * @param CMMRCServerSession& aServerSession
 * @leave in case of memory error
 */
void CMMRCServerInfo::PushAllocatedResourceContextL(const CMMRCServerSession& aServerSession )
	{
	DP_CONTEXT(----> CMMRCServerInfo::PushAllocatedResourceContextL *CD1*, CtxDevSound, DPLOCAL);
	DP_IN();

	PushContextL(iMMRCAllocatedResourceContextsQueue, aServerSession);

	DP_OUT();
	}

/** 
 * Pop and remove the first Context of the highest priority from the in-process Context queue
 * @return CMMRCServerSession* the session of the Context
 */
CMMRCServerSession* CMMRCServerInfo::PopAndRemoveFirstAllocatedResourceContext()
	{
	DP_CONTEXT(----> CMMRCServerInfo::PopAndRemoveFirstAllocatedResourceContext *CD1*, CtxDevSound, DPLOCAL);
	DP_IN();
	
	CMMRCServerSession* mmrcSession = PopAndRemoveFirstContext(iMMRCAllocatedResourceContextsQueue);

	DP0_RET(mmrcSession, "mmrcSession = 0x%x");
	}

/** 
 * Pop and remove the first Context from the in-process Context queue by contextId
 * @param const TAudioContextId& aContextId
 * @return CMMRCServerSession* the session of the Context
 */
CMMRCServerSession* CMMRCServerInfo::PopAndRemoveFirstAllocatedResourceContextByContextId(const TAudioContextId& aContextId )
	{
	DP_CONTEXT(----> CMMRCServerInfo::PopAndRemoveFirstAllocatedResourceContextByContextId *CD1*, CtxDevSound, DPLOCAL);
	DP_IN();
	
	CMMRCServerSession* mmrcSession = PopAndRemoveFirstContextByContextId(iMMRCAllocatedResourceContextsQueue, aContextId);

	DP0_RET(mmrcSession, "mmrcSession = 0x%x");
	}

/** 
 * Pop the first Context from the in-process Context queue
 * @return CMMRCServerSession* the session of the Context
 */
CMMRCServerSession* CMMRCServerInfo::PopFirstAllocatedResourceContext() const
	{
	DP_CONTEXT(----> CMMRCServerInfo::PopFirstAllocatedResourceContext *CD1*, CtxDevSound, DPLOCAL);
	DP_IN();
	
	CMMRCServerSession* mmrcSession = PopFirstContext(iMMRCAllocatedResourceContextsQueue);

	DP0_RET(mmrcSession, "mmrcSession = 0x%x");
	}

//------------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------------

/** 
 * Push an Context on the in-process Context queue
 * @param CMMRCServerSession& aServerSession
 * @leave in case of memory error
 */
void CMMRCServerInfo::PushPausedContextL(const CMMRCServerSession& aServerSession )
	{
	DP_CONTEXT(----> CMMRCServerInfo::PushPausedContextL *CD1*, CtxDevSound, DPLOCAL);
	DP_IN();
	
	PushContextInOrderL(iMMRCPausedContextsQueue, aServerSession);

	DP_OUT();
	}

/** 
 * Pop and remove the first Context of the highest priority from the in-process Context queue
 * @return CMMRCServerSession* the session of the Context
 */
CMMRCServerSession* CMMRCServerInfo::PopAndRemoveFirstPausedContext()
	{
	DP_CONTEXT(----> CMMRCServerInfo::PopAndRemoveFirstPausedContext *CD1*, CtxDevSound, DPLOCAL);
	DP_IN();
	
	CMMRCServerSession* mmrcSession = PopAndRemoveFirstContextForPause(iMMRCPausedContextsQueue);

	DP0_RET(mmrcSession, "mmrcSession = 0x%x");
	}
/** 
 * Pop and remove the first Context from the in-process Context queue by contextId
 * @param const TAudioContextId& aContextId
 * @return CMMRCServerSession* the session of the Context
 */
CMMRCServerSession* CMMRCServerInfo::PopAndRemovePausedContext(const TAudioContextId& aContextId )
	{
	DP_CONTEXT(----> CMMRCServerInfo::PopAndRemovePausedContext *CD1*, CtxDevSound, DPLOCAL);
	DP_IN();
	
	CMMRCServerSession* mmrcSession = PopAndRemoveContextIdForPause(iMMRCPausedContextsQueue, aContextId);

	DP0_RET(mmrcSession, "mmrcSession = 0x%x");
	}
/** 
 * Pop the Context from the in-process Context queue
 * @param const TAudioContextId& aContextId
 * @return CMMRCServerSession* the session of the Context
 */
CMMRCServerSession* CMMRCServerInfo::PausedContextByIndex(TInt aIndex)
	{
	DP_CONTEXT(----> CMMRCServerInfo::PausedContextByIndex *CD1*, CtxDevSound, DPLOCAL);
	DP_IN();
	
	CMMRCServerSession* mmrcSession = PausedContextByIndexForPause(iMMRCPausedContextsQueue, aIndex);

	DP0_RET(mmrcSession, "mmrcSession = 0x%x");
	}


//------------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------------

/** 
 * Push an Context on the in-process Context queue
 * @param CMMRCServerSession& aServerSession
 * @leave in case of memory error
 */
void CMMRCServerInfo::PushAPRContextL(const CMMRCServerSession& aServerSession )
	{
	DP_CONTEXT(----> CMMRCServerInfo::PushAPRContextL *CD1*, CtxDevSound, DPLOCAL);
	DP_IN();
	
	PushContextL(iMMRCAPRContextsQueue, aServerSession);
	
	DP_OUT();
	}

/** 
 * Pop and remove the first Context of the highest priority from the in-process Context queue
 * @return CMMRCServerSession* the session of the Context
 */
CMMRCServerSession* CMMRCServerInfo::PopAndRemoveFirstAPRContext()
	{
	DP_CONTEXT(----> CMMRCServerInfo::PopAndRemoveFirstAPRContext *CD1*, CtxDevSound, DPLOCAL);
	DP_IN();
	
	CMMRCServerSession* mmrcSession = PopAndRemoveFirstContext(iMMRCAPRContextsQueue);

	DP0_RET(mmrcSession, "mmrcSession = 0x%x");
	}
/** 
 * Pop and remove the first Context from the in-process Context queue by contextId
 * @param const TAudioContextId& aContextId
 * @return CMMRCServerSession* the session of the Context
 */
CMMRCServerSession* CMMRCServerInfo::PopAndRemoveFirstAPRContextByContextId(const TAudioContextId& aContextId )
	{
	DP_CONTEXT(----> CMMRCServerInfo::PopAndRemoveFirstAPRContextByContextId *CD1*, CtxDevSound, DPLOCAL);
	DP_IN();
	
	CMMRCServerSession* mmrcSession = PopAndRemoveFirstContextByContextId(iMMRCAPRContextsQueue, aContextId);

	DP0_RET(mmrcSession, "mmrcSession = 0x%x");
	}
/** 
 * Pop the first Context from the in-process Context queue
 * @return CMMRCServerSession* the session of the Context
 */
CMMRCServerSession* CMMRCServerInfo::PopFirstAPRContext() const
	{
	DP_CONTEXT(----> CMMRCServerInfo::PopFirstAPRContext *CD1*, CtxDevSound, DPLOCAL);
	DP_IN();
	
	CMMRCServerSession* mmrcSession = PopFirstContext(iMMRCAPRContextsQueue);

	DP0_RET(mmrcSession, "mmrcSession = 0x%x");
	}

/**

 */
TBool CMMRCServerInfo::IsOnAPRList(TAudioContextId aId)
	{
	DP_CONTEXT(----> CMMRCServerInfo::IsOnAPRList *CD1*, CtxDevSound, DPLOCAL);
	DP_IN();
	TBool iIsContain = EFalse;
	CMMRCServerSession* serverSession = NULL;
	
	TInt nbContexts = iMMRCAPRContextsQueue.Count();
	for( TInt i = 0 ; i < nbContexts ; i++ )
		{
		serverSession = iMMRCAPRContextsQueue[i];
		
		TAudioContextId contextId = serverSession->ContextId();
		if( aId == contextId )
			{
			iIsContain = ETrue;
			break;
			}
		}

	DP0_RET(iIsContain, "%d");
	}

/**

 */
void CMMRCServerInfo::PushContextInOrderL(RMMRCContextsQueueOrderTD& aContextsQueue, const CMMRCServerSession& aServerSession )
	{
	DP_CONTEXT(----> CMMRCServerInfo::PushContextInOrderL *CD1*, CtxDevSound, DPLOCAL);
	DP_IN();

	aContextsQueue.PushL(&aServerSession);
	
	DP_OUT();
	}


/**

 */
CMMRCServerSession* CMMRCServerInfo::PopAndRemoveFirstContextForPause(RMMRCContextsQueueOrderTD& aContextsQueue)
	{
	DP_CONTEXT(----> CMMRCServerInfo::PopAndRemoveFirstContextForPause *CD1*, CtxDevSound, DPLOCAL);
	DP_IN();
	
	CMMRCServerSession* serverSession = 0;
	//PopAndRemove return null if the queue is empty;
	serverSession = aContextsQueue.PopAndRemoveForPause(0);
		
	DP0_RET(serverSession, "serverSession = 0x%x");
	}

/**

 */
CMMRCServerSession* CMMRCServerInfo::PopAndRemoveContextIdForPause(RMMRCContextsQueueOrderTD& aContextsQueue, const TAudioContextId& aContextId )
	{
	DP_CONTEXT(----> CMMRCServerInfo::PopAndRemoveContextIdForPause *CD1*, CtxDevSound, DPLOCAL);
	DP_IN();
	
	CMMRCServerSession* serverSession = NULL;
	
	TInt nbContexts = aContextsQueue.Count();
	for( TInt i = 0 ; i < nbContexts ; i++ )
		{
		serverSession = aContextsQueue[i];
		
		TAudioContextId contextId = serverSession->ContextId();
		if( aContextId == contextId )
			{
			aContextsQueue.PopAndRemoveForPause(i);
			break;
			}
		else
			{
			serverSession = NULL;
			}
		}
	
	DP0_RET(serverSession, "serverSession = 0x%x");
	}


/**

 */
CMMRCServerSession* CMMRCServerInfo::PausedContextByIndexForPause(RMMRCContextsQueueOrderTD& aContextsQueue, TInt aIndex) 
	{
	DP_CONTEXT(----> CMMRCServerInfo::PausedContextByIndexForPause *CD1*, CtxDevSound, DPLOCAL);
	DP_IN();
	
	CMMRCServerSession* serverSession = 0;
	//PopAndRemove return null if the queue is empty;
	serverSession = aContextsQueue[aIndex];
		
	DP0_RET(serverSession, "serverSession = 0x%x");
	}



/**

 */
TBool CMMRCServerInfo::IsOnPauseList(TAudioContextId aId)
	{
	DP_CONTEXT(----> CMMRCServerInfo::IsOnAPRList *CD1*, CtxDevSound, DPLOCAL);
	DP_IN();
	TBool iIsContain = EFalse;
	CMMRCServerSession* serverSession = NULL;
	
	TInt nbContexts = iMMRCPausedContextsQueue.Count();
	for( TInt i = 0 ; i < nbContexts ; i++ )
		{
		serverSession = iMMRCPausedContextsQueue[i];
		
		TAudioContextId contextId = serverSession->ContextId();
		if( aId == contextId )
			{
			iIsContain = ETrue;
			break;
			}
		}

	DP0_RET(iIsContain, "%d");
	}



		
TInt CMMRCServerInfo::RequestsCount()
	{	return iMMRCContextsQueue.Count();	}
TInt CMMRCServerInfo::AllocatedResourceContextsCount()
	{	return iMMRCAllocatedResourceContextsQueue.Count();		}
TInt CMMRCServerInfo::AllocatedAPRContextsCount()
	{	return iMMRCAPRContextsQueue.Count();	}
TInt CMMRCServerInfo::AllocatedPausedContextsCount()
	{	return iMMRCPausedContextsQueue.Count();	}	

//EOF
