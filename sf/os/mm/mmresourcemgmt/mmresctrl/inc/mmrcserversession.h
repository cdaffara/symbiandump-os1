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

#ifndef MMRCSERVERSESSION_H__
#define MMRCSERVERSESSION_H__

#include <e32std.h>
#include <e32base.h>
#include <a3f/a3fbase.h>
#include <e32msgqueue.h>
#include "mmrcclientserver.h"

/**
* @file
* @internalTechnology
*/

class MMultimediaResourceControlObserver;
class MLogicalChain;
class CFourCCConvertor;
class MAudioContext;
class CMMRCServerController;
class CMMRCServerSession;
class CMMRCClientSideServerRequestToAdaptationAO;
class CMMRCServerSideAdaptationResponseAO;
class CAudioContext;

enum TMMRCServerSessionRequestType
	{
	ENormalRequest, 
	EPreemptionRequest, 
	};

/**
Panic category and codes that MMRCServerSession raises on the client
*/
_LIT(KMMRCServerSessionPanicCategory, "MMRCServerSession");

enum TMMRCServerSessionPanicCodes
	{
	ECloneLeftWhenSendingResponseToClient =1,
	EMsgQueueFailedToSendMsg
	};

/**
@internalComponent
This class implements the MMRC Server Session. 
*/
NONSHARABLE_CLASS(CMMRCServerSession) : public CSession2
	{
	//friend class CTimeoutClientAcknowledment;
public:
	/**
	 * Constructs, and returns a pointer to, a new CMMRCServerSession object.
	 * Leaves on failure.
	 * @param CMMRCServerController& A reference on the MMRC Server controller
	 * @param CFourCCConvertor& A reference to the Fourcc to format converter 	 
	 * @return CMMRCServerSession* A pointer to newly created utlitly object.
	 */
	static CMMRCServerSession* NewL(CMMRCServerController& aMMRCServerController, 
									const CFourCCConvertor& aFourCCConvertor);

	/**
	 * Constructs, leaves object on the cleanup stack, and returns a pointer
	 * to, a new CMMRCServerSession object.
	 * Leaves on failure.
	 * @param CMMRCServerController& A reference on the MMRC Server controller
	 * @param CFourCCConvertor& A reference to the Fourcc to format converter 	 
	 * @return CMMRCServerSession* A pointer to newly created utlitly object.
	 */
	static CMMRCServerSession* NewLC(CMMRCServerController& aMMRCServerController, 
									const CFourCCConvertor& aFourCCConvertor);

	/**
	 * Destructor.
	 */
	~CMMRCServerSession();


	/**
	 * from CSession2
	 * @param aMessage - Function and data for the session
	 */	
	void ServiceL(const RMessage2& aMessage);  
	
	/*
	 * 
	 */
	void StartProcessL();

	/*
	 * 
	 */
	void StartPreemptionL();

	/**
	 * Callback on a resource request
	 * @param TMMRCServerToClientMessageResults aResult the result on the request
	 * @leave
	 */
	void AdaptationRequestAcknowledgment(TMMRCAdaptationToServerRequestResults aResult, TInt aError);

	/**
	 * Notify all the paused client that a resource has been unallocated
	 */
	void NotifyClientResourceUnallocated();

	/**
	 * return the current committed chain (const)
	 * @return the current committed chain
	 */
	const MLogicalChain* LogicalChainRequested() const;
	
	/**
	 * return the current committed chain (const)
	 * @return the current committed chain
	 */
	const MLogicalChain* LogicalChainRequestedStack() const;

	/**
	 * return the last committed chain (const)
	 * @return the last committed chain
	 */
	const MLogicalChain* LogicalChainLastCommited() const;

	/*
	 * 
	 */
	const MAudioContext* AudioContext() const;
	
	/**
	 * return the current committed chain
	 * @return the current committed chain
	 */
	MLogicalChain* LogicalChainRequested();
	
	/**
	 * return the current committed chain
	 * @return the current committed chain
	 */
	MLogicalChain* LogicalChainRequestedStack();

	/**
	 * return the last committed chain
	 * @return the last committed chain
	 */
	MLogicalChain* LogicalChainLastCommited();

	/**
	 * 
	 */		
	CMMRCClientSideServerRequestToAdaptationAO* AdaptationProxy();
	
	
	CMMRCServerSideAdaptationResponseAO* ServerProxy();
	/**
	 * 
	 */		
	TUint64 ContextId();

	/**
	 * 
	 */		
	TBool HasMultimediaCapability() const;
	
	/**
	 * 
	 */		
	void Clean();

	/**
	 * 
	 */		
	TBool IsServerShuttingDown();

	TInt RequestsSent();
	
	TInt SendRequestToAdaptation(TMMRCServerSessionRequestType aRequestType);

	TInt GetPriority() const;

	/**
	 * 
	 */		
	TBool HasUserEnvironmentCapability();
	
	/*
	 * 
	 */
	TInt NotifyClientOfError(TInt aError);
	
protected:

	void IncrementRequestsSent();

	/*
	 * 
	 */
	void Disconnect(const RMessage2& aMessage);
	
private:

	//Constructor
	/**
	 * By default Symbian 2nd phase constructor is private.
	 * @param CMMRCServerController& A reference on the MMRC Server controller
	 * @param CFourCCConvertor& A reference to the Fourcc to format converter 	 
	 */
	CMMRCServerSession(CMMRCServerController& aMMRCServerController, 
						const CFourCCConvertor& aFourCCConvertor);
	
	/**
	 * By default Symbian 2nd phase constructor is private.
	 */
	void ConstructL();
	
	/**
	 * Dispatch the message received from ServeL
	 * EMMRCClientToServerSetMsgQueueHandle - Initialise the queue handler
	 * EMMRCClientToServerMessage - Process a message from the client
	 * EMMRCClientToServerMessageUpdate - Process a message update from the client
	 * EMMRCClientToServerMessageAcknowledgement - Process the acknowldgement from the client
	 * @param aMessage - Function and data for the session
	 * @return error code
	 * @leave in case of writeL leave or request leave
	 */	
	TInt DispatchMessageL(const RMessage2& aMessage);
	
	/*
	 * 
	 */
	TInt EnqueueRequestL(const RMessage2& aMessage);

	/*
	 * 
	 */
	void PreProcessRequestL(const RMessage2& aMessage);
	
	
	/*
	 * 
	 */
	void ProcessResourceDataRequestL(const RMessage2& aMessage);
	
	/*
	 * 
	 */
	void SendRequestResponseToClient(TMMRCQueueItem& messageItem);

	/*
	 * 
	 */
	void SendPreemptionNotificationToClient(TMMRCQueueItem& messageItem);
	
	
	/*
	 * 
	 */
	void Panic(TMMRCServerSessionPanicCodes aCode);

private:
	RMsgQueue<TMMRCQueueItem> iMsgQueue;			//
	CMMRCServerController& iMMRCServerController;	// a reference on the MMRC Server controller
	//RThread iClientThread;
	RThread iServerThread;
	RThread iClientThread;
	TBool iShutdown;
	//owned
	CMMRCServerSideAdaptationResponseAO* iMMRCServerSideAdaptationResponseAO;
	MLogicalChain* iLogicalChainRequestedStack;			// the new committed chain
	MLogicalChain* iLogicalChainLastCommitedStack;		// the last committed
	MLogicalChain* iLogicalChainRequested;			// the new committed chain
	MLogicalChain* iLogicalChainLastCommited;		// the last committed
	MLogicalChain* iLogicalChainRequestResponse;			// A copy of the chain for client response
	MLogicalChain* iLogicalChainPreemptionNotification;	// A copy of the chain for preemption notification to the client

	//not owned (can be null pointers)
	CAudioContext* iAudioContext;
	const CFourCCConvertor* iFourCCConvertor;
	CMMRCClientSideServerRequestToAdaptationAO* iMMRCClientSideServerRequestToAdaptationAO;

	//not own
	TInt32 iContextId;
	TInt32 iRequestsSent;
	TBool iClientHasMMCapability;
	TBool iClientHasUECapability;
	};

#endif //__MMRCSERVERSESSION_H__
