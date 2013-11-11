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


#include "mmrcserversession.h"
#include "mmrcerrorcodes.h"
#include "mmrcservercontroller.h"
#include "mmrcadaptationproxy.h"

#include <a3f/maudiocontext.h>

#include "mlogicalchain.h"
#include "logicalaudiostream.h"
#include "audiocontext.h"

#include <a3f/a3ffourcclookup.h>


//------------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------------
/**
 * By default Symbian 2nd phase constructor is private.
 * @param CMMRCServerController& A reference on the MMRC Server controller
 */
CMMRCServerSession::CMMRCServerSession(CMMRCServerController& aServerController, 
									const CFourCCConvertor& aFourCCConvertor)
: iMMRCServerController(aServerController),
  iShutdown(0),
  iLogicalChainRequested(NULL),
  iLogicalChainLastCommited(NULL),
  iLogicalChainRequestResponse(NULL),
  iLogicalChainPreemptionNotification(NULL)
	{
	TRACE_CREATE();
	DP_CONTEXT(----> CMMRCServerSession::CMMRCServerSession *CD1*, CtxDevSound, DPLOCAL);
	DP_IN();
	iFourCCConvertor = &aFourCCConvertor;
	DP_OUT();
	}

/**
 * Destructor
 */
CMMRCServerSession::~CMMRCServerSession()
	{
 	DP_CONTEXT(----> CMMRCServerSession::~CMMRCServerSession *CD1*, CtxDevSound, DPLOCAL);
	DP_IN();
	//remove any request associated to this session from the controller
	iMMRCServerController.CloseSessionByContextId(iContextId);
	
	if (iMMRCServerSideAdaptationResponseAO)
		{
		delete iMMRCServerSideAdaptationResponseAO;
		iMMRCServerSideAdaptationResponseAO=0;
		};
	
 	if(	iLogicalChainLastCommited )
 		{
		iLogicalChainLastCommited->Release();
 		iLogicalChainLastCommited = NULL;
 		};
 	
 	if(	iLogicalChainRequested )
 		{
		iLogicalChainRequested->Release();
 		iLogicalChainRequested = NULL;
 		}
 	if(	iLogicalChainRequestResponse )
 		{
		iLogicalChainRequestResponse->Release();
		iLogicalChainRequestResponse = NULL;
 		};
 		
 	if(	iLogicalChainPreemptionNotification )
 		{
		iLogicalChainPreemptionNotification->Release();
		iLogicalChainPreemptionNotification = NULL;
 		};

	iMsgQueue.Close();	
	
	DP_OUT();
	}

/**
 * Constructs, and returns a pointer to, a new CMMRCServerSession object.
 * Leaves on failure.
 * @param CMMRCServerController& A reference on the MMRC Server controller
 * @param CFourCCConvertor& A reference to the Fourcc to format converter 
 * @return CMMRCServerSession* A pointer to newly created utlitly object.
 */
CMMRCServerSession* CMMRCServerSession::NewL(CMMRCServerController& aServerController, 
											const CFourCCConvertor& aFourCCConvertor)
	{
	DP_STATIC_CONTEXT(----> CMMRCServerSession::NewL *CD1*, CtxDevSound, DPLOCAL);
	DP_IN();
	
	CMMRCServerSession* self = NewLC(aServerController, aFourCCConvertor);
	CleanupStack::Pop(self);
	
    DP0_RET(self, "self=0x%x");
	}

/**
 * Constructs, leaves object on the cleanup stack, and returns a pointer
 * to, a new CMMRCServerSession object.
 * Leaves on failure.
 * @param CMMRCServerController& A reference on the MMRC Server controller
 * @param CFourCCConvertor& A reference to the Fourcc to format converter  
 * @return CMMRCServerSession* A pointer to newly created utlitly object.
 */
CMMRCServerSession* CMMRCServerSession::NewLC(CMMRCServerController& aServerController, 
											const CFourCCConvertor& aFourCCConvertor)
	{
	DP_STATIC_CONTEXT(----> CMMRCServerSession::NewLC *CD1*, CtxDevSound, DPLOCAL);
	DP_IN();
	
	CMMRCServerSession* self = new(ELeave) CMMRCServerSession(aServerController, aFourCCConvertor);
	CleanupStack::PushL( self );
	self->ConstructL();
	
	DP0_RET(self, "self=0x%x");
	}

/**
 	CMMRCServerSession::ConstructL
 	Symbian 2nd phase constructor can leave.
 */
void CMMRCServerSession::ConstructL()
	{
 	DP_CONTEXT(----> CMMRCServerSession::ConstructL *CD1*, CtxDevSound, DPLOCAL);
	DP_IN();
	
	iContextId = reinterpret_cast<TUint32>(this);
	iMMRCServerSideAdaptationResponseAO = CMMRCServerSideAdaptationResponseAO::NewL();
	iMMRCServerSideAdaptationResponseAO->Initialize(this, &iServerThread);

	DP_OUT();
	}

/**
 * from CSession2
 * @param aMessage - Function and data for the session
 */	
void CMMRCServerSession::ServiceL(const RMessage2& aMessage)
	{
 	DP_CONTEXT(----> CMMRCServerSession::ServiceL *CD1*, CtxDevSound, DPLOCAL);
	DP_IN();

	TRAPD( errL, DispatchMessageL(aMessage) );

	if( errL != KErrNone )
		{
		#ifdef SYMBIAN_MULTIMEDIA_ENABLE_LOGGING	
			RDebug::Print(_L("CMMRCServerSession::ServiceL - err %d"), errL);
		#endif
		}
	
	DP_OUT();
	}	

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
TInt CMMRCServerSession::DispatchMessageL(const RMessage2& aMessage)
	{
 	DP_CONTEXT(----> CMMRCServerSession::DispatchMessageL *CD1*, CtxDevSound, DPLOCAL);
	DP_IN();
	
	TInt error(KErrNone);

	TMMRCClientToServerMessages messageFunction = static_cast<TMMRCClientToServerMessages>(aMessage.Function());
	switch(messageFunction)
		{
		
		case EMMRCClientToServerSetClientThreadId:
			{
			TThreadId* id = static_cast<TThreadId*>(const_cast<TAny *>(aMessage.Ptr0()));
			iClientThread.Open(*id);
			RThread me;
			iServerThread.Open(me.Id());
			aMessage.Complete(error);  
			}
		break;

		case EMMRCClientToServerSetAdaptationProxy:
			{
			iMMRCClientSideServerRequestToAdaptationAO = static_cast<CMMRCClientSideServerRequestToAdaptationAO*>(const_cast<TAny *>(aMessage.Ptr0()));

			if(iMMRCClientSideServerRequestToAdaptationAO)
				{
				iMMRCClientSideServerRequestToAdaptationAO->Initialize(this, 
															iMMRCServerSideAdaptationResponseAO, 
															&iClientThread, 
															*iFourCCConvertor);
				}
			aMessage.Complete(error);  
			}
		break;

		case EMMRCClientToServerSetMsgQueueHandle:
			{
			error = iMsgQueue.Open( aMessage, 0 ); 
			aMessage.Complete(error);  
			}
		break;

		case EMMRCClientToServerCloseSession:
			{
			//set a flag so that any subsequent acknowledgment will be ignored...
			iShutdown=ETrue;
			aMessage.Complete(error);  
			}
		break;
		
		case EMMRCClientToServerRequestResource:
			{
			error = EnqueueRequestL(aMessage);
			// AM this is needed since the request is not asynchronous.
			// It is processed asynchronously which is different
			aMessage.Complete(error);
			}
		break;

		case EMMRCClientToServerRequestContextId:
			{
			// CDS This code does not appear to be handling PlatSec functionality well

			TProcessId PID;
			TPckg<TProcessId> processIdPckg(PID);
			TPckgBuf<TUint64> contextIdPckg;
						
			aMessage.ReadL( 0, processIdPckg );			
			
			RProcess client;
			TInt err = client.Open(PID);
			if(err == KErrNone)
				{
				iClientHasMMCapability = client.HasCapability(ECapabilityMultimediaDD, KSuppressPlatSecDiagnostic);
				iClientHasUECapability = client.HasCapability(ECapabilityUserEnvironment, KSuppressPlatSecDiagnostic);
				client.Close();
				}
			aMessage.WriteL( 1, contextIdPckg );
			aMessage.Complete(error);  
			}
		break;
		
		
		case EMMRCClientToServerRegisterAsClient:
			{
			// ignoring arguments
			error = iMMRCServerController.EnqueueRequestForAPRL(*this);
			aMessage.Complete(error);
			}
		break;
		
		case EMMRCClientToServerCancelRegisterAsClient:
			{
			// ignoring arguments
			error = iMMRCServerController.RemoveRequestForAPR(*this);
			aMessage.Complete(error);
			}
		break;

		case EMMRCClientToServerWillResumePlay:
			{
			error = iMMRCServerController.WillResumePlay();
			aMessage.Complete(error);  
			}
		break;		
		
		default:
			{
			#ifdef SYMBIAN_MULTIMEDIA_ENABLE_LOGGING	
				RDebug::Print(_L("!!!!CMMRCServerSession::DispatchMessageL - unknown message"));
			#endif
			// Unknown Message
			error = KErrNotSupported;
			}
		break;
		}

	DP0_RET(error, "err=%d");
	}	

/**
 * Process a message that can be
 *     EMMRCMessageStateAffecting a state-effecting message
 *     EMMRCMessageCommit a resource request
 * @param const RMessage2& aMessage
 */	
void CMMRCServerSession::PreProcessRequestL(const RMessage2& aMessage)
 	{
 	DP_CONTEXT(----> CMMRCServerSession::PreprocessMessageL *CD1*, CtxDevSound, DPLOCAL);
	DP_IN();
	
	MLogicalChain* logicalChainRequested = static_cast<MLogicalChain*>(const_cast<TAny *>(aMessage.Ptr1()));
	if( logicalChainRequested == NULL )
		{
		#ifdef _DEBUG	
			RDebug::Print(_L("!!!!CMMRCServerSession::PreProcessRequestL - logicalChainRequested NULL"));
			ASSERT(0);
		#endif
		User::Leave(KErrArgument);	
		}
	
	TInt err1 = KErrNone, err2 = KErrNone;
	iAudioContext = static_cast<CAudioContext*>(const_cast<TAny *>(aMessage.Ptr2()));
	MLogicalChain* logicalChainLastCommited = static_cast<MLogicalChain*>(const_cast<TAny *>(aMessage.Ptr0()));
	
	if(iLogicalChainLastCommitedStack)
		{
		iLogicalChainLastCommitedStack->Release();
		iLogicalChainLastCommitedStack = NULL;
		}
	if(iLogicalChainRequestedStack)
		{
		iLogicalChainRequestedStack->Release();
		iLogicalChainRequestedStack = NULL;
		}
	
	iAudioContext->CriticalSection().Wait();
	TInt msgVersion = iAudioContext->MsgVersion();
	if(msgVersion == aMessage.Int3())
		{
		if(logicalChainLastCommited)
			{
			TRAP(err1, iLogicalChainLastCommitedStack = logicalChainLastCommited->CloneL());
			}
	
		if(logicalChainRequested)
			{
			TRAP(err2, iLogicalChainRequestedStack = logicalChainRequested->CloneL());
			}
		}
	iAudioContext->CriticalSection().Signal();
	
	if(msgVersion != aMessage.Int3())
		{
		User::Leave(KErrCompletion);
		}
	
	if(err1!=KErrNone)
		{
		#ifdef SYMBIAN_MULTIMEDIA_ENABLE_LOGGING
			RDebug::Print(_L("!!!!CMMRCServerSession::PreProcessRequestL - logicalChainLastCommited->CloneL - error = %d"), err1);
		#endif
		ASSERT(0);
		User::Leave(err1);
		}
		
	if(err2!=KErrNone)
		{
		#ifdef SYMBIAN_MULTIMEDIA_ENABLE_LOGGING	
			RDebug::Print(_L("!!!!CMMRCServerSession::PreProcessRequestL - logicalChainRequested->CloneL - error = %d"), err2);
		#endif
		if(iLogicalChainLastCommitedStack)
			{
			iLogicalChainRequestedStack->Release();
			iLogicalChainRequestedStack = NULL;
			}
		ASSERT(0);
		User::Leave(err2);
		}
		
	DP_OUT();
	}

/**
 * Process a message that can be
 *     EMMRCMessageStateAffecting a state-effecting message
 *     EMMRCMessageCommit a resource request
 * @param const RMessage2& aMessage
 */	
TInt CMMRCServerSession::EnqueueRequestL(const RMessage2& aMessage)
 	{
 	DP_CONTEXT(----####> CMMRCServerSession::ProcessRequestL *CD1*, CtxDevSound, DPLOCAL);
	DP_IN();
	
	TInt err = KErrNone;
	TRAP(err, PreProcessRequestL(aMessage));
	if( err == KErrNone )
		{
		err = iMMRCServerController.EnqueueRequestL(*this);
		}
	
	DP0_RET(err, "err = %d");
	}

/*
 * 
 */
void CMMRCServerSession::StartProcessL()
	{
 	DP_CONTEXT(----> CMMRCServerSession::StartProcessL *CD1*, CtxDevSound, DPLOCAL);
	DP_IN();

 	if( iLogicalChainLastCommited )
		{
		//jjpy TODO should compare with iLogicalChainRequestedStack
		iLogicalChainLastCommited->Release();
		}
		
	if(iLogicalChainRequested)
		{
		#ifdef _DEBUG	
			RDebug::Print(_L("!!!!CMMRCServerSession::StartProcessL - iLogicalChainRequested not NULL"));
			ASSERT(0);
		#endif
		iLogicalChainRequested->Release();
		}
		
	iLogicalChainLastCommited = iLogicalChainLastCommitedStack;
	iLogicalChainRequested = iLogicalChainRequestedStack;
	
	iLogicalChainLastCommitedStack = 0;
	iLogicalChainRequestedStack = 0;
	
	//Send the CommitUpdate to the client
 	TMMRCQueueItem messageItem;
 	messageItem.iRequestType = EMMRCRequestCommitUpdate;
	messageItem.iResult = EMMRCServerToClientCommitUpdate;
	messageItem.iErrorCode = KErrNone;
 	TInt errSend = iMsgQueue.Send(messageItem);
	#ifdef _DEBUG	
		if(errSend!=KErrNone)
 		{
		RDebug::Print(_L("!!!!CMMRCServerSession::PreProcessRequestL - error = %d"), errSend);
		ASSERT(0);
 		}
	#endif
	User::LeaveIfError(errSend);
	
	DP_OUT();
	}

/*
 * 
 */
void CMMRCServerSession::StartPreemptionL()
	{
 	DP_CONTEXT(----> CMMRCServerSession::StartPreemptionL *CD1*, CtxDevSound, DPLOCAL);
	DP_IN();

	//Should be NULL
	if(iLogicalChainRequested)
		{
		#ifdef _DEBUG	
			RDebug::Print(_L("!!!!CMMRCServerSession::StartPreemptionL - iLogicalChainRequested not NULL"));
			ASSERT(0);
		#endif
		iLogicalChainRequested->Release();
		}

	//Should NOT be NULL.
	if( iLogicalChainLastCommited )
		{
		iLogicalChainRequested = iLogicalChainLastCommited->CloneL();
		CLogicalAudioStream* audiostream = static_cast<CLogicalAudioStream*>(iLogicalChainRequested);
		TAudioState state = audiostream->StreamState();
		
		// MMRC wont't support demote to EIdle anymore
		// All stream are demote to EInitialised
		switch(state)
			{
			case EIdle:
				{
				audiostream->Unload();
				}
				break;
				
			case EPrimed:
			case EActive:
				{
				User::LeaveIfError(audiostream->Stop());
				audiostream->Unload();
				}
				break;
			default:
				#ifdef _DEBUG	
					RDebug::Print(_L("!!!!CMMRCServerSession::StartPreemptionL - wrong state "));
					ASSERT(0);
				#endif
				break;
			}
		}
	
	DP_OUT();
	}

/**
 * Callback on a resource request
 * @param TMMRCServerToClientMessageResults aResult the result on the request
 * @leave
 */
void CMMRCServerSession::AdaptationRequestAcknowledgment(TMMRCAdaptationToServerRequestResults aResult, TInt aError)
 	{
 	DP_CONTEXT(----@@@@> CMMRCServerSession::AdaptationRequestAcknowledgment *CD1*, CtxDevSound, DPLOCAL);
	DP_IN();
	
 	TMMRCQueueItem messageItem;
 	messageItem.iRequestType = EMMRCRequestCommitResponse;
 	
 	switch(aResult)
 		{
  		case EMMRCAdaptationToServerResourceComplete:
 			{
 			TUint64 contextId;
 			TMMRCServerState serverState;
 			iMMRCServerController.ServerState(serverState, contextId);
			switch(serverState)
				{
 				case EMMRCServerStateWaitResourceResponseFromAdaptation: // here we process the response
		 	  		{
		 	  		messageItem.iResult = EMMRCServerToClientResourceAgreed;
		 	 	 	messageItem.iErrorCode = aError;
		 	 		iMMRCServerController.AdaptationRequestAcknowledgment(*this, aResult, aError);
		 	 		SendRequestResponseToClient(messageItem);
 					} //end case EMMRCServerStateWaitResourceResponseFromAdaptation
 	 				break;
				case EMMRCServerStateWaitPreemptionResponse: // here we process the response
					{
			 	 	messageItem.iResult = EMMRCServerToClientResourcePreempt;
		 	 	 	messageItem.iErrorCode = KErrCompletion;
					iMMRCServerController.AdaptationRequestAcknowledgment(*this, aResult, KErrCompletion);
					SendPreemptionNotificationToClient(messageItem);
					}
					break;
				default:
					{
					#ifdef _DEBUG	
						RDebug::Print(_L("!!!!CMMRCServerSession::AdaptationRequestAcknowledgment - Unknown message"));
						ASSERT(0);
					#endif
					}
					break;
				} //end case EMMRCServerStateWaitPreemptionRespons
			}
 			break;
 			
 		case EMMRCAdaptationToServerResourceIncomplete:
 			{
	 	  		messageItem.iResult = EMMRCServerToClientResourceDenied;
	 	 	 	messageItem.iErrorCode = aError;
	 	 		iMMRCServerController.AdaptationRequestAcknowledgment(*this, aResult, aError);
	 	 		SendRequestResponseToClient(messageItem);
			}
 			break;
 			
 		case EMMRCAdaptationToServerResourceTimeout:
 			{
 	 	 	messageItem.iResult = EMMRCServerToClientRequestError;
 	 	 	messageItem.iErrorCode = KErrCompletion;
			iMMRCServerController.AdaptationRequestAcknowledgment(*this, EMMRCAdaptationToServerResourceTimeout, KErrCompletion);
			SendRequestResponseToClient(messageItem);
 			}
 			break;
 			
		default:
 			break;
 		}
 	
  	DP_OUT();
  	}

/**
 * 
 * 
 */	
void CMMRCServerSession::SendRequestResponseToClient(TMMRCQueueItem& messageItem)
	{
 	DP_CONTEXT(----====> CMMRCServerSession::SendRequestResponseToClient *CD1*, CtxDevSound, DPLOCAL);
	DP_IN();
	if(messageItem.iErrorCode == KErrNone)
		{
		//1) Set the iLogicalChainRequestResponse
		if(iLogicalChainRequestResponse)
			{
			iLogicalChainRequestResponse->Release();
			iLogicalChainRequestResponse = NULL;
			}
		if(iLogicalChainRequested)
			{
			TInt errT;
			TRAP(errT, iLogicalChainRequestResponse = iLogicalChainRequested->CloneL());
			DP1(DLERR,"ECloneLeftWhenSendingResponseToClient %d", errT);
			__ASSERT_DEBUG(errT == KErrNone, Panic(ECloneLeftWhenSendingResponseToClient));
			}
		//2) Set the last commited chain et set the desired one to NULL
		if( iLogicalChainLastCommited )
			{
			iLogicalChainLastCommited->Release();
			iLogicalChainLastCommited = NULL;
			}
		iLogicalChainLastCommited = iLogicalChainRequested;
		iLogicalChainRequested = NULL;
		}
	else //In case of ERROR
		{
		//1) Set the iLogicalChainRequestResponse
		if(iLogicalChainRequestResponse)
			{
			iLogicalChainRequestResponse->Release();
			iLogicalChainRequestResponse = NULL;
			}
		if(iLogicalChainRequested)
			{
			TInt errT;
			TRAP(errT, iLogicalChainRequestResponse = iLogicalChainLastCommited->CloneL());
			DP1(DLERR,"ECloneLeftWhenSendingResponseToClient %d", errT);
			__ASSERT_DEBUG(errT == KErrNone, Panic(ECloneLeftWhenSendingResponseToClient));
			}
		//2) Set the last commited chain et set the desired one to NULL
		if( iLogicalChainRequested )
			{
			iLogicalChainRequested->Release();
			iLogicalChainRequested = NULL;
			}
			
		if(iLogicalChainRequestedStack)
			{
			CLogicalAudioStream* lChain = static_cast <CLogicalAudioStream*>(iLogicalChainLastCommited);
			TAudioTypeSettings settings;	
			settings.iPriority = iLogicalChainRequestedStack->Priority();
			settings.iPref = 0;
			lChain->SetAudioType(settings);
			}
		}
	// Set the data for the client
	messageItem.iData = iLogicalChainRequestResponse;

 	//send the response to the client
 	TInt errSend = iMsgQueue.Send(messageItem);
	DP1(DLERR,"EMsgQueueFailedToSendMsg %d", errSend);
	__ASSERT_DEBUG(errSend == KErrNone, Panic(EMsgQueueFailedToSendMsg));
	
  	DP_OUT();
	}

/**
 * 
 * 
 */	
void CMMRCServerSession::SendPreemptionNotificationToClient(TMMRCQueueItem& messageItem)
	{
 	DP_CONTEXT(----~~~~> CMMRCServerSession::SendRequestResponseToClient *CD1*, CtxDevSound, DPLOCAL);
	DP_IN();
	//1) Set the iLogicalChainPreemptionNotification
	if(iLogicalChainPreemptionNotification)
		{
		iLogicalChainPreemptionNotification->Release();
		iLogicalChainPreemptionNotification = NULL;
		}
	if(iLogicalChainRequested)
		{
		TInt errT;
		TRAP(errT, iLogicalChainPreemptionNotification = iLogicalChainRequested->CloneL());
		DP1(DLERR,"ECloneLeftWhenSendingResponseToClient %d", errT);
		__ASSERT_DEBUG(errT == KErrNone, Panic(ECloneLeftWhenSendingResponseToClient));

		iLogicalChainRequested->Release();
		iLogicalChainRequested = NULL;
		}

	messageItem.iData = iLogicalChainPreemptionNotification;

 	//send the response to the client
 	TInt errSend = iMsgQueue.Send(messageItem);
	DP1(DLERR,"EMsgQueueFailedToSendMsg %d", errSend);
	__ASSERT_DEBUG(errSend == KErrNone, Panic(EMsgQueueFailedToSendMsg));
	
  	DP_OUT();
	}

/**
 * Notify all the paused client that a resource has been unallocated
 */
void CMMRCServerSession::NotifyClientResourceUnallocated()
	{
	DP_CONTEXT(----****> CMMRCServerSession::NotifyClientsResourceUnallocated *CD1*, CtxDevSound, DPLOCAL);
	DP_IN();
	
 	TMMRCQueueItem messageItem;
 	messageItem.iRequestType = EMMRCRequestResourceUnallocated;
 	TInt errSend = iMsgQueue.Send(messageItem);
	DP1(DLERR,"EMsgQueueFailedToSendMsg %d", errSend);
	__ASSERT_DEBUG(errSend == KErrNone, Panic(EMsgQueueFailedToSendMsg));
	
	DP_OUT();
	}


/**
 * return the current committed chain (const)
 * @return the current committed chain
 */
const MLogicalChain* CMMRCServerSession::LogicalChainRequested() const
 	{
 	DP_CONTEXT(----> CMMRCServerSession::LogicalChainRequested const *CD1*, CtxDevSound, DPLOCAL);
	DP_IN();
	
	DP0_RET(iLogicalChainRequested, "iLogicalChainRequested=0x%x");
	}

/**
 * return the current committed chain (const)
 * @return the current committed chain
 */
const MLogicalChain* CMMRCServerSession::LogicalChainRequestedStack() const
 	{
 	DP_CONTEXT(----> CMMRCServerSession::LogicalChainRequestedStack const *CD1*, CtxDevSound, DPLOCAL);
	DP_IN();
	
	DP0_RET(iLogicalChainRequestedStack, "iLogicalChainRequestedStack=0x%x");
	}


/**
 * return the last committed chain (const)
 * @return the last committed chain
 */
const MLogicalChain* CMMRCServerSession::LogicalChainLastCommited() const
 	{
 	DP_CONTEXT(----> CMMRCServerSession::LogicalChainLastCommited const *CD1*, CtxDevSound, DPLOCAL);
	DP_IN();
	
    DP0_RET(iLogicalChainLastCommited, "iLogicalChainLastCommited=0x%x");
 	}

/**
 * return the Audio Context (const)
 * @return the Audio Context
 */
const MAudioContext* CMMRCServerSession::AudioContext() const
	{
 	DP_CONTEXT(----> CMMRCServerSession::AudioContext *CD1*, CtxDevSound, DPLOCAL);
	DP_IN();
	
    DP0_RET(iAudioContext, "iAudioContext=0x%x");
	}
 	
/**
 * return the current committed chain
 * @return the current committed chain
 */
MLogicalChain* CMMRCServerSession::LogicalChainRequested()
  	{
 	DP_CONTEXT(----> CMMRCServerSession::LogicalChainRequested *CD1*, CtxDevSound, DPLOCAL);
	DP_IN();
	
    DP0_RET(iLogicalChainRequested, "iLogicalChainRequested=0x%x");
 	}

/**
 * return the current committed chain
 * @return the current committed chain
 */
MLogicalChain* CMMRCServerSession::LogicalChainRequestedStack()
  	{
 	DP_CONTEXT(----> CMMRCServerSession::LogicalChainRequestedStack *CD1*, CtxDevSound, DPLOCAL);
	DP_IN();
	
    DP0_RET(iLogicalChainRequestedStack, "iLogicalChainRequestedStack=0x%x");
 	}

/**
 * return the last committed chain
 * @return the last committed chain
 */
MLogicalChain* CMMRCServerSession::LogicalChainLastCommited()
  	{
 	DP_CONTEXT(----> CMMRCServerSession::LogicalChainLastCommited *CD1*, CtxDevSound, DPLOCAL);
	DP_IN();
	
	DP0_RET(iLogicalChainLastCommited, "iLogicalChainLastCommited=0x%x");
 	}

/**
 * 
 * @param 
 * @return
 */
CMMRCClientSideServerRequestToAdaptationAO* CMMRCServerSession::AdaptationProxy()
{
	DP_CONTEXT(----> CMMRCServerSession::AdaptationProxy *CD1*, CtxDevSound, DPLOCAL);
	DP_IN();
	
    DP0_RET(iMMRCClientSideServerRequestToAdaptationAO, "iMMRCServerAdaptationProxy=0x%x");
}

/**
 * 
 * @param 
 * @return
 */
TUint64 CMMRCServerSession::ContextId()
	{
	DP_CONTEXT(----> CMMRCServerSession::ContextId *CD1*, CtxDevSound, DPLOCAL);
	DP_IN();
	
    DP0_RET(iContextId, "iContextId=%d");
	}

/**
 * 
 */
TBool CMMRCServerSession::HasMultimediaCapability() const
	{
	return iClientHasMMCapability;
	}


/**
 * 
 */
void CMMRCServerSession::Clean()
	{
	DP_CONTEXT(----> CMMRCServerSession::Clean *CD1*, CtxDevSound, DPLOCAL);
	DP_IN();
	DP_OUT();
	}

/**
 * 
 * @param 
 */
void CMMRCServerSession::Disconnect(const RMessage2& aMessage)
	{
	DP_CONTEXT(----> CMMRCServerSession::Disconnect *CD1*, CtxDevSound, DPLOCAL);
	DP_IN();

	CSession2::Disconnect(aMessage);
	
	DP_OUT();
	}

/**
 * 
 * @return 
 */
TBool CMMRCServerSession::IsServerShuttingDown()
	{
	DP_CONTEXT(----> CMMRCServerSession::IsServerShuttingDown *CD1*, CtxDevSound, DPLOCAL);
	DP_IN();

	DP0_RET(iShutdown, "state(bool) = %d");
	}

TInt CMMRCServerSession::RequestsSent()
	{
	return iRequestsSent;
	};

void CMMRCServerSession::IncrementRequestsSent()
	{
	iRequestsSent++;
	};
	
CMMRCServerSideAdaptationResponseAO* CMMRCServerSession::ServerProxy()
	{
	return iMMRCServerSideAdaptationResponseAO;
	};
	
TInt CMMRCServerSession::SendRequestToAdaptation(TMMRCServerSessionRequestType aRequestType)
	{
	TInt err=KErrNone;
	if (!IsServerShuttingDown())
		{
		IncrementRequestsSent();
		ServerProxy()->StartAdaptationListening();
		AdaptationProxy()->Service(aRequestType);
		}
	else
		{
// send some server shutting down error code.		
		}
	return err;
	};	

TInt CMMRCServerSession::GetPriority() const
	{
	DP_CONTEXT(----> CMMRCServerSession::GetPriority *CD1*, CtxDevSound, DPLOCAL);
	DP_IN();
	TInt priority = 0;
	if(LogicalChainLastCommited() != NULL)
		{
		priority = const_cast<MLogicalChain*>( LogicalChainLastCommited() )->Priority();
		}
	DP0_RET(priority, "priority = %d");
	}

void CMMRCServerSession::Panic(TMMRCServerSessionPanicCodes aCode)
	{
	User::Panic(KMMRCServerSessionPanicCategory, aCode);
	}

/**
 * @ return Boolean value specifying whether Client has UserEnvironment Capability.
 */
TBool CMMRCServerSession::HasUserEnvironmentCapability()
	{
	return iClientHasUECapability;
	}

/**
 * @ param Error Value to send
 * @ return Error Value specifying any error in adding the message to the message queue.
 */

TInt CMMRCServerSession::NotifyClientOfError(TInt aError)
	{
	// Create message and send to Client
	TMMRCQueueItem messageItem;
 	messageItem.iRequestType = EMMRCRequestCommitUpdate;
	messageItem.iResult = EMMRCServerToClientCommitUpdate;
	messageItem.iErrorCode = KErrNone;
 	TInt errSend = iMsgQueue.Send(messageItem);
	
	// Create a message with the Error and send to Client
 	messageItem.iRequestType = EMMRCRequestCommitResponse;
	messageItem.iResult = EMMRCServerToClientResourceAgreed;
 	messageItem.iErrorCode = aError;
 	TInt errSend2 = iMsgQueue.Send(messageItem);
 	if (errSend!=KErrNone)
 		{
 		return errSend;
 		}
 	else
 		{
 		return errSend2;
 		}
	}
//EOF
