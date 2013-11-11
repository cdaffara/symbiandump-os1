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

#ifndef MMRCSERVERINFO_H__
#define MMRCSERVERINFO_H__

#include <e32base.h>
#include <e32msgqueue.h>
#include "mmrcutil.h"
#include "mmrcclientserver.h"
#include "mmrcserversession.h" 

class CMMRCServerController;

/**
@internalComponent

This class implements the MMRC Server Info. 
The main purpose of this class is to contain the different information
  about the MMRC Server.
  The server state
  The queue of resource Context
  The queue of resource in process
*/	
NONSHARABLE_CLASS( CMMRCServerInfo ): public CBase
	{
	typedef RMMRCFifoQueue<CMMRCServerSession> RMMRCContextsQueueTD;
	typedef RMMRCFifoOrderQueue<CMMRCServerSession> RMMRCContextsQueueOrderTD;

public:
	/**
	 * Constructs, and returns a pointer to, a new CMMRCServerInfo object.
	 * Leaves on failure.
	 * @param CMMRCServerController& A reference on the MMRC Server controller
	 * @return CMMRCServerInfo* A pointer to newly created utlitly object.
	 */	
	static CMMRCServerInfo* NewL(CMMRCServerController& aServerController); 
	
	/**
	 * Constructs, leaves object on the cleanup stack, and returns a pointer
	 * to, a new CMMRCServerInfo object.
	 * Leaves on failure.
	 * @param CMMRCServerController& A reference on the MMRC Server controller
	 * @return CMMRCServerInfo* A pointer to newly created utlitly object.
	 */	
	 static CMMRCServerInfo* NewLC(CMMRCServerController& aServerController);
 	
	/**
	 * Destructor.
	 */
	~CMMRCServerInfo();

public:

	TInt AllocatedPausedContextsCount();
	TInt RequestsCount();
	TInt AllocatedResourceContextsCount();
	TInt AllocatedAPRContextsCount();


	/** 
	 * Change the state of the server
	 * @param TMMRCServerState aNewState
	 * @param TUint64 aContextId
	 */
	void SwitchServerState(TMMRCServerState aNewState, TUint64 aContextId);
	
	
	/** 
	 * get the state of the server
	 * @param TMMRCServerState the state of the server
	 * @param TUint64 aContextId
	 */
	void ServerState(TMMRCServerState& aServerState, TUint64& aContextId) const;
	
	/** 
	 * Push a Context on the Context queue
	 * @param CMMRCServerSession& aServerSession
	 * @leave in case of memory error
	 */
	void PushRequestL(const CMMRCServerSession& aServerSession);
	
	/** 
	 * Pop and remove the first Context from the Context queue
	 * @return CMMRCServerSession* the session of the Context
	 */
	CMMRCServerSession* PopAndRemoveFirstRequest();
	
	/** 
	 * Pop and remove the first Context from the Context queue by contextId
	 * @param const TAudioContextId& aContextId
	 * @return CMMRCServerSession* the session of the Context
	 */
	CMMRCServerSession* PopAndRemoveFirstRequestByContextId( const TAudioContextId& aContextId );

	/** 
	 * Pop and remove the first Context of the highest priority from the Context queue
	 * @param TInt aPriority
	 * @return CMMRCServerSession* the session of the Context
	 */
	CMMRCServerSession* PopAndRemoveRequestFirstHighestPriority( );

	/** 
	 * Pop and remove the first Context of the highest priority and more than aPriority 
	 * from the Context queue
	 * @param TInt aPriority
	 * @return CMMRCServerSession* the session of the Context
	 */
	CMMRCServerSession* PopAndRemoveRequestFirstHighestPriority( TInt aPriority );

	/** 
	 * Pop the first Context from the Context queue
	 * @return CMMRCServerSession* the session of the Context
	 */
	CMMRCServerSession* PopFirstRequest( ) const;

	/** 
	 * Pop the first Context of the highest priority from the Context queue
	 * @return CMMRCServerSession* the session of the Context
	 */
	CMMRCServerSession* PopRequestFirstHighestPriority( ) const;

	/** 
	 * Pop the first Context of the highest priority and more than aPriority 
	 * from the Context queue
	 * @param TInt aPriority
	 * @return CMMRCServerSession* the session of the Context
	 */
	CMMRCServerSession* PopRequestFirstHighestPriority( TInt aPriority ) const;

	/** 
	 * Push an Context on the in-process Context queue
	 * @param CMMRCServerSession& aServerSession
	 * @leave in case of memory error
	 */
	void PushAllocatedResourceContextL(const CMMRCServerSession& aServerSession);
	
	/** 
	 * Pop and remove the first Context of the highest priority from the in-process Context queue
	 * @return CMMRCServerSession* the session of the Context
	 */
	CMMRCServerSession* PopAndRemoveFirstAllocatedResourceContext();
	
	/** 
	 * Pop and remove the first Context from the in-process Context queue by contextId
	 * @param const TAudioContextId& aContextId
	 * @return CMMRCServerSession* the session of the Context
	 */
	CMMRCServerSession* PopAndRemoveFirstAllocatedResourceContextByContextId( const TAudioContextId& aContextId );


	/** 
	 * Pop the first Context from the in-process Context queue
	 * @return CMMRCServerSession* the session of the Context
	 */
	CMMRCServerSession* PopFirstAllocatedResourceContext( ) const;

	void PushPausedContextL(const CMMRCServerSession& aServerSession );
	CMMRCServerSession* PopAndRemoveFirstPausedContext();
	CMMRCServerSession* PopAndRemovePausedContext(const TAudioContextId& aContextId );
	CMMRCServerSession* PausedContextByIndex(TInt aIndex);
	
	void PushAPRContextL(const CMMRCServerSession& aServerSession );
	CMMRCServerSession* PopAndRemoveFirstAPRContext();
	CMMRCServerSession* PopAndRemoveFirstAPRContextByContextId(const TAudioContextId& aContextId );
	CMMRCServerSession* PopFirstAPRContext() const;
	
	TBool IsOnAPRList(TAudioContextId aId);
	TBool IsOnPauseList(TAudioContextId aId);


private:	
	/**
	 * Push an Context on the Context queue in parameter
	 * @param RMMRCContextsQueueTD& aContextsQueue
	 * @param CMMRCServerSession& aServerSession
	 * @leave in case of memory error
	 */
	void PushContextL(RMMRCContextsQueueTD& aContextsQueue, const CMMRCServerSession& aServerSession);
	
	/**
	 * Pop and remove the first Context of the highest priority from the Context queue in parameter
	 * @param RMMRCContextsQueueTD& aContextsQueue
	 * @return CMMRCServerSession* the session of the Context
	 */
	CMMRCServerSession* PopAndRemoveFirstContext(RMMRCContextsQueueTD& aContextsQueue);
	
	/**
	 * Pop and remove the first Context from the Context queue in parameter by contextId
	 * @param RMMRCContextsQueueTD& aContextsQueue
	 * @param const TAudioContextId& aContextId
	 * @return CMMRCServerSession* the session of the Context
	 */
	CMMRCServerSession* PopAndRemoveFirstContextByContextId(RMMRCContextsQueueTD& aContextsQueue, const TAudioContextId& aContextId );
	
	/**
	 * Pop and remove the first Context of the highest priority from the Context queue in parameter
	 * @param RMMRCContextsQueueTD& aContextsQueue
	 * @return CMMRCServerSession* the session of the Context
	 */
	CMMRCServerSession* PopAndRemoveContextFirstHighestPriority(RMMRCContextsQueueTD& aContextsQueue);
	
	/**
	 * Pop and remove the first Context of the highest priority and more than aPriority 
	 * from the Context queue in parameter
	 * @param RMMRCContextsQueueTD& aContextsQueue
	 * @param TInt aPriority
	 * @return CMMRCServerSession* the session of the Context
	 */
	CMMRCServerSession* PopAndRemoveContextFirstHighestPriority(RMMRCContextsQueueTD& aContextsQueue, TInt aPriority );
	
	/**
	 * Pop the first Context from the Context queue in parameter
	 * @param RMMRCContextsQueueTD& aContextsQueue
	 * @return CMMRCServerSession* the session of the Context
	 */
	CMMRCServerSession* PopFirstContext(const RMMRCContextsQueueTD& aContextsQueue) const;

	/**
	 * Pop the first Context of the highest priority from the Context queue in parameter
	 * @param RMMRCContextsQueueTD& aContextsQueue
	 * @return CMMRCServerSession* the session of the Context 
	 */
	CMMRCServerSession* PopContextFirstHighestPriority(const RMMRCContextsQueueTD& aContextsQueue) const;

	/**
	 * Pop the first Context of the highest priority and more than aPriority 
	 * from the Context queue in parameter
	 * @param RMMRCContextsQueueTD& aContextsQueue
	 * @param TInt aPriority
	 * @return CMMRCServerSession* the session of the Context
	 */
	CMMRCServerSession* PopContextFirstHighestPriority(const RMMRCContextsQueueTD& aContextsQueue, TInt aPriority ) const;

	void PushContextInOrderL(RMMRCContextsQueueOrderTD& aContextsQueue, const CMMRCServerSession& aServerSession);
	CMMRCServerSession* PopAndRemoveFirstContextForPause(RMMRCContextsQueueOrderTD& aContextsQueue);
	CMMRCServerSession* PopAndRemoveContextIdForPause(RMMRCContextsQueueOrderTD& aContextsQueue, const TAudioContextId& aContextId );
	CMMRCServerSession* PausedContextByIndexForPause(RMMRCContextsQueueOrderTD& aContextsQueue, TInt aIndex);

	
private:	
	/**
	 * By default Symbian 2nd phase constructor is private.
	 * @param CMMRCServerController& A reference on the MMRC Server controller
	 */
	CMMRCServerInfo(CMMRCServerController& aServerController);
	
	/**
	 * By default Symbian 2nd phase constructor is private.
	 */
	void ConstructL();

private:
	CMMRCServerController& iMMRCServerController;
	TMMRCServerState iMMRCServerInfoState;
	RMMRCContextsQueueTD iMMRCContextsQueue; 					//0 or more context(s)
	RMMRCContextsQueueTD iMMRCAllocatedResourceContextsQueue; 
	RMMRCContextsQueueOrderTD iMMRCPausedContextsQueue;
	RMMRCContextsQueueTD iMMRCAPRContextsQueue;
	TUint64	iLastTransactionContextId;
	};

#endif //__MMRCSERVERINFO_H__
