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

#ifndef MMRCSERVERCONTROLLER_H__
#define MMRCSERVERCONTROLLER_H__

#include <e32std.h>
#include <e32base.h>
#include <a3f/a3fbase.h>
#include "mmrcclientserver.h"

class CMMRCServerInfo;
class MMMRCServerInfoState;
class CMMRCServerSession;
class CMMRCServerRuleManager;
class CARNNotificationObject;


/**
Panic category and codes that MMRCServerController raises on the client
*/
_LIT(KMMRCServerControllerPanicCategory, "MMRCServerController");

enum TMMRCServerControllerPanicCodes
	{
	EProcessNextRequestLeft = 1,
	EPushPauseContextLeft,
	EPushAllocatedResourceContextFailed
	};


/**
@internalComponent

This class implements the MMRC Server Controller. 
The main purpose of this class is to control the different transactions
*/	
NONSHARABLE_CLASS( CMMRCServerController ): public CBase
	{
public:
	/**
	 * Constructs, and returns a pointer to, a new CMMRCServerController object.
	 * Leaves on failure.
	 * @return CMMRCServerController* A pointer to newly created utlitly object.
	 */		
	static CMMRCServerController* NewL(); 
	
	/**
	 * Constructs, leaves object on the cleanup stack, and returns a pointer
	 * to, a new CMMRCServerController object.
	 * Leaves on failure.
	 * @return CMMRCServerController* A pointer to newly created utlitly object.
	 */	
	static CMMRCServerController* NewLC();
	
	/**
	 * Destructor.
	 */
	~CMMRCServerController();
	
public:

	/**
	 * Process the request pointer by a specific session
	 * @param CMMRCServerSession& aServerSession
	 * @return KerrNone in case of success of the process
	 */
	TInt EnqueueRequestL(CMMRCServerSession& aServerSession );
	
	/** 
	 * Callback from the adaptation
	 * @param MLogicalChain* aOldChain
	 * @param MLogicalChain* aNewChain
	 * @param TMMRCAdaptationToServerRequestResults aResult
	 */
	void AdaptationRequestAcknowledgment(CMMRCServerSession& aServerSessionAllocatedResource, TMMRCAdaptationToServerRequestResults aResult, TInt aError);
	
	/*
	 * 
	 */
	void CloseSessionByContextId(TInt32 aContextId);
	
	/*
	 * 
	 */
	void ClientAcknowledgment(CMMRCServerSession& aServerSession);
	
	/*
	 * 
	 */
	void ServerState(TMMRCServerState& aServerState, TUint64& aContextId) const;
	
	/**
	 * 
	 * @param CMMRCServerSession& aServerSession
	 * @return KerrNone in case of success of the process
	 */	
	TInt EnqueueRequestForAPRL(CMMRCServerSession& aServerSession );
	
	/**
	 * 
	 * @param CMMRCServerSession& aServerSession
	 * @return KerrNone in case of success of the process
	 */	
	TInt RemoveRequestForAPR(CMMRCServerSession& aServerSession );	
	
	/**
	 * 
	 * @return KerrNone in case of success of the process
	 */	
	TInt WillResumePlay();
	

private:
	/**
	 * Process the first request with the highest priority from the FIFO queue
	 * @return KerrNone in case of success of the process
	 */
	TInt ProcessNextRequestL();
	
	/**
	 * Send a resource request to the adaptation
	 * @param CMMRCServerSession& aServerSession 
	 * @return KerrNone in case of success of the process
	 */
	TInt SendResourceRequest( CMMRCServerSession& aServerSession );
	
	/** 
	 * Send a preemption request to the adaptation
	 * @param CMMRCServerSession& aServerSession 
	 * @return KerrNone in case of success of the process
	 */
	TInt SendPreemptionRequest( CMMRCServerSession& aServerSession );
	
	/**
	 * Pop the decision rule on the base of the current in-process request and a new committed request
	 * @param TAudioState aAudioStateAllocatedResource
	 * @param TAudioState aAudioStateRequiringProcess
	 * @return TMMRCRule
	 */
	TMMRCRule DecisionRule(TAudioState aAudioStateAllocatedResource, TAudioState aAudioStateRequiringProcess);
	
	/**
	 * Pop the action reason on the base of the last committed request and the current one
	 * @param TAudioState aAudioStateLastCommitted
	 * @param TAudioState aAudioStateRequested
	 * @return TReason
	 */
	TReason ActionReason(TAudioState aAudioStateLastCommitted, TAudioState aAudioStateRequested);
	
	/**
	 * Notify all the paused client that a resource has been unallocated
	 */
	void NotifyPausedClientsResourceUnallocated();

	
	/*
	 * 
	 */
	void Panic(TMMRCServerControllerPanicCodes aCode);

private:
		
	/**
	 * By default Symbian 2nd phase constructor is private.
	 */
	CMMRCServerController();
	
	/**
	 * By default Symbian 2nd phase constructor is private.
	 */
	void ConstructL();

private:
	//owned
	CMMRCServerInfo* iMMRCServerInfo;
	CMMRCServerRuleManager* iMMRCServerRuleManager;
	CARNNotificationObject* iARNNotificationObject;
	};

#endif //__MMRCSERVERCONTROLLER_H__
