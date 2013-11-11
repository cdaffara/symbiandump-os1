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

/**
 @file
 @deprecated since v9.1. Functionality is replaced with commsdat.
*/

#include <c32comm.h>
#include <e32svr.h>

#include <commsdat.h>
#include "NotifierServ.H"

const TInt noPendingEvents = -1;

void InitL()
	{
	RProcess().SetPriority(EPriorityHigh);
	CCommsdatNotifierScheduler* pA = new(ELeave) CCommsdatNotifierScheduler();
	CActiveScheduler::Install(pA);
	pA->iServer = CCommsdatNotifierServer::NewL();	
	}

/**
CCommsdatNotifier server process entry point
*/
TInt E32Main()
	{
	__UHEAP_MARK;	
	TInt ret = KErrNone;
	
	CTrapCleanup* cleanup=CTrapCleanup::New();
	if (cleanup==NULL)
		{
		ret=KErrNoMemory;	
		}		

	if (ret==KErrNone)
		{
		TRAP(ret,InitL());	
		}		

	RProcess::Rendezvous(ret);
	
	if (ret==KErrNone)
		{
		CActiveScheduler::Start();	
		}
		

	delete CActiveScheduler::Current();
	delete cleanup;
	
	__UHEAP_MARKEND;

	return ret;
	}
	

	
//**********************************
//CCommsdatNotifierServer
//**********************************

CCommsdatNotifierServer::CCommsdatNotifierServer()
	: CServer2(EPriorityHigh, EGlobalSharableSessions)
	{
	}

CCommsdatNotifierServer* CCommsdatNotifierServer::NewL()
// Create and start a new count server
	{
	CCommsdatNotifierServer* self = new(ELeave) CCommsdatNotifierServer;
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;
	}

void CCommsdatNotifierServer::ConstructL()
	{
	iShutdownWatchDog = CShutdownWatchDog::NewL(*this);
	StartL(COMMSDATNOTIFIER_SERVER_NAME);
	}

CCommsdatNotifierServer::~CCommsdatNotifierServer()
	{
	delete iShutdownWatchDog;
	}

void CCommsdatNotifierServer::IncrementSessionCount()
	{
	iSessionCount++;
	}

void CCommsdatNotifierServer::DecrementSessionCount()
	{
	iSessionCount--;
	if (iSessionCount<=0)
		{
		iShutdownWatchDog->Cancel();	//Cancel any outstanding request and start again
		iShutdownWatchDog->StartShutdown();
		}		
	}
	
void CCommsdatNotifierServer::MaybeShutDown()
     {
      if (iSessionCount<=0)
         {
      	 CActiveScheduler::Stop();
      	 }
     }
     
CSession2* CCommsdatNotifierServer::NewSessionL(const TVersion &aVersion, const RMessage2& /*aMessage*/) const
// Create a new server session - check we're the right version and make session
	{
	TVersion v(KCommsdatNotifierMajorVersionNumber,KCommsdatNotifierMinorVersionNumber,KCommsdatNotifierBuildVersionNumber);
	
	TBool r=User::QueryVersionSupported(v,aVersion);
	if (r==EFalse)
		{
		User::Leave(KErrNotSupported);
		}		
	
	TBool clientSpecialVersion = (aVersion.iBuild==KCommsdatNotifierSpecialBuildVersionNumber);
	
	return CCommsdatNotifierSession::NewL((CCommsdatNotifierServer*)this, clientSpecialVersion);
	}
	

//**********************************
//CCommsdatNotifierSession
//**********************************

CCommsdatNotifierSession* CCommsdatNotifierSession::NewL(CCommsdatNotifierServer* aServer, TBool aClientSpecialVersion)
	{
	CCommsdatNotifierSession* thisptr = new(ELeave) CCommsdatNotifierSession(aServer);
	CleanupStack::PushL(thisptr);
	thisptr->ConstructL(aClientSpecialVersion);
	CleanupStack::Pop(thisptr);
	return thisptr;
	}
	
void CCommsdatNotifierSession::ConstructL(TBool aClientSpecialVersion)
	{
	if (aClientSpecialVersion)
		iNotifierHandler = CCommsdatNotifierSessionHandler2::NewL();	
	else
		iNotifierHandler = CCommsdatNotifierSessionHandler::NewL();		
	}	
		
CCommsdatNotifierSession::CCommsdatNotifierSession(CCommsdatNotifierServer* aServer)
	: iNotifierSvr(aServer)
	{
	iNotifierSvr->IncrementSessionCount();
	}

CCommsdatNotifierSession::~CCommsdatNotifierSession()
	{
	iNotifierSvr->DecrementSessionCount();
	iNotifierHandler->Cancel();	
	delete iNotifierHandler;
	}

void CCommsdatNotifierSession::ServiceL(const RMessage2& aMessage)
	{
	switch (aMessage.Function())
        {
		case ERequestNotify:
			iNotifierHandler->RegisterNotifyL(aMessage);
			break;
		case ECancelNotify:
			iNotifierHandler->CancelNotify(aMessage);
			break;
		case ECancelAllNotify:
			iNotifierHandler->CancelAllNotify(aMessage);
			break;			
		default:
			PanicClient(KErrArgument,aMessage);
			break;
        }
	}
void CCommsdatNotifierSession::PanicClient(TInt aPanic,const RMessage2& aMessage) const
// Panic the client
	{
	_LIT(KClientFault,"Commdb Shim Notifier Server");
	aMessage.Panic(KClientFault,aPanic);
	}
	
//**********************************
//CCommsdatNotifierSessionHandlerBase
//**********************************

void CCommsdatNotifierSessionHandlerBase::ConstructL()
	{
	TInt err = iProperty.Attach(KUidSystemCategory, KUidCommsDatNotificationEvent.iUid);
	User::LeaveIfError(err);
	}	
	
CCommsdatNotifierSessionHandlerBase::CCommsdatNotifierSessionHandlerBase()
	: CActive(EPriorityHigh)
	{
	CActiveScheduler::Add(this);
	}

CCommsdatNotifierSessionHandlerBase::~CCommsdatNotifierSessionHandlerBase()
	{		
	iProperty.Close();
	}	
	
void CCommsdatNotifierSessionHandlerBase::DoCancel()		
	{
	iProperty.Cancel();
	}

void CCommsdatNotifierSessionHandlerBase::CompleteMessage(const RMessage2& aMessage,TInt aVal)
     {
     if(!aMessage.IsNull())
           {	
           aMessage.Complete(aVal);	
           }
     }	

//**********************************
//CCommsdbNotifierSessionHandler
//**********************************

CCommsdatNotifierSessionHandler* CCommsdatNotifierSessionHandler::NewL()
	{
	CCommsdatNotifierSessionHandler* self = new(ELeave) CCommsdatNotifierSessionHandler;
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;
	}
	
void CCommsdatNotifierSessionHandler::ConstructL()
	{
	CCommsdatNotifierSessionHandlerBase::ConstructL();	
	}	
	
void CCommsdatNotifierSessionHandler::RegisterNotifyL(const RMessage2& aMessage)
	{
	// if there is already an outstanding request, complete
	// immediately with an error
	
	if (! iMessage.IsNull())
		{
		aMessage.Complete(KErrServerBusy);
		return;
		}

	// there are no outstanding requests

	// if there is already a pending event, complete immediately
	if (iPendingEvent > noPendingEvents)
		{
		aMessage.Complete(iPendingEvent);
		// reset iPendingEvent
		iPendingEvent = noPendingEvents;
		return;
		}

	// there are no outstanding events (iPendingEvent == noPendingEvents)

	iMessage = aMessage;

	if (!IsActive())
	    {
	    iProperty.Subscribe(iStatus);
	    SetActive();
	    }
	}

CCommsdatNotifierSessionHandler::CCommsdatNotifierSessionHandler()
	: CCommsdatNotifierSessionHandlerBase(), iPendingEvent(noPendingEvents)
	{
	}

CCommsdatNotifierSessionHandler::~CCommsdatNotifierSessionHandler()
	{		
	}	
	
void CCommsdatNotifierSessionHandler::CancelNotify(const RMessage2& aMessage)
	{
	Cancel();
	iPendingEvent = noPendingEvents;			
	
	// Complete outstanding async request for notification
	CompleteMessage(iMessage, KErrCancel);
	
	// Complete the cancel message
	CompleteMessage(aMessage, KErrNone);
	}
	
void CCommsdatNotifierSessionHandler::CancelAllNotify(const RMessage2& aMessage)
	{
		CancelNotify(aMessage);
	}		
	
void CCommsdatNotifierSessionHandler::RunL()
	{
	TInt stat = iStatus.Int();
// according to the present Symbian Publish and Subscribe API
// documentation and implementation, the following code should ensure
// that further events are not missed during RunL
	iProperty.Subscribe(iStatus);
	SetActive();

	if (stat == KErrNone)
		{
		// use stat for storing the new event
		User::LeaveIfError(iProperty.Get(KUidSystemCategory, KUidCommsDatNotificationEvent.iUid, stat));

		if (iMessage.IsNull())
			{
			// if the client is not ready to receive notifications,
			// store the new event in iPendingEvent,
			//but check first if it is more important than the current
			//pending event (if there is one)
			if (stat > iPendingEvent)
                {
				iPendingEvent = stat;
                }
			}
		else
			{
			// if the client is ready to receive notifications, complete

			if (iPendingEvent > noPendingEvents)
				{
				// if there is a pending event

				// complete with the new event, if it is more
				// important than the pending event
				if (stat > iPendingEvent)
					{
					iMessage.Complete(stat);
					iPendingEvent = stat;
					}
				// otherwise complete with the pending event
				else
					{
					iMessage.Complete(iPendingEvent);
					}
				}
			else
				{
				iMessage.Complete(stat);
				iPendingEvent = stat;
				}
			}
		}
	else
		{
		if (!iMessage.IsNull())
			{
			// if there is a pending event, complete immediately
			if (iPendingEvent > noPendingEvents)
				{
				iMessage.Complete(iPendingEvent);
				}
			else
				{
				// otherwise, complete with error
				iMessage.Complete(stat);
				}
			}
		}
	}

TInt CCommsdatNotifierSessionHandler::RunError(TInt aError)		
	{
	CompleteMessage(iMessage,aError);
	return KErrNone;
	}

//**********************************
//CCommsdatNotifierSessionHandler2
//**********************************

CCommsdatNotifierSessionHandler2* CCommsdatNotifierSessionHandler2::NewL()
	{
	CCommsdatNotifierSessionHandler2* self = new(ELeave) CCommsdatNotifierSessionHandler2;
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;
	}
	
void CCommsdatNotifierSessionHandler2::ConstructL()
	{	
	CCommsdatNotifierSessionHandlerBase::ConstructL();	
	
	iMessagePool = new(ELeave)CArrayFixFlat<RMessage2>(5);
	}	
	
void CCommsdatNotifierSessionHandler2::CompleteRequests(TInt aErrorCode)
	{
	for (TInt i = iMessagePool->Count() ; i > 0 ; i--)
		{
		CompleteMessage(iMessagePool->At(i - 1), aErrorCode);
		}
	iMessagePool->Reset();
	}


void CCommsdatNotifierSessionHandler2::ResetRequestVector()
	{
	iMessagePool->Reset();
	}	
	
	
void CCommsdatNotifierSessionHandler2::RegisterNotifyL(const RMessage2& aMessage)
	{
	for (TInt i = iMessagePool->Count() ; i > 0 ; i--)
		{
		if (iMessagePool->At(i - 1).ClientStatus()==aMessage.ClientStatus())
		{
			// do nothing we already have this outstanding request 
			return;
		}
		}	
	
	iMessagePool->AppendL(aMessage);

	if (!IsActive())
	    {
	    iProperty.Subscribe(iStatus);
	    SetActive();
	    }
	}

CCommsdatNotifierSessionHandler2::CCommsdatNotifierSessionHandler2()
	: 	CCommsdatNotifierSessionHandlerBase()
	{
	}

CCommsdatNotifierSessionHandler2::~CCommsdatNotifierSessionHandler2()
	{		
	ResetRequestVector();
	delete iMessagePool;
	}	
	
void CCommsdatNotifierSessionHandler2::CancelNotify(const RMessage2& aMessage)
/**
Cancel a previously registered notification.

@param aMessage the cancel notification request.  Ptr0() identifies the notification to
cancel and contains the address of its TRequestStatus.
*/

	{
	for (TInt i = iMessagePool->Count() ; i > 0 ; i--)
		{
		if (iMessagePool->At(i - 1).ClientStatus() == aMessage.Ptr0())
			{
			CompleteMessage(iMessagePool->At(i - 1), KErrCancel);	// complete the registered notification
			iMessagePool->Delete(i - 1);
			break;
			}
		}
		
	aMessage.Complete(KErrNone); // complete the cancel notification
	}
	
void CCommsdatNotifierSessionHandler2::CancelAllNotify(const RMessage2& aMessage)
	{
	iProperty.Cancel();
	CompleteRequests(KErrCancel);
	aMessage.Complete(KErrCancel);
	}	
	
void CCommsdatNotifierSessionHandler2::RunL()
	{
	TInt stat = iStatus.Int();
	iProperty.Subscribe(iStatus);
	SetActive();
	
	CompleteRequests(stat);
	}
	
TInt CCommsdatNotifierSessionHandler2::RunError(TInt aError)		
	{
	CompleteRequests(aError);
	return KErrNone;
	}	

//**********************************
//CShutdownWatchDog
//**********************************
const TInt KNotifierServerShutdownLatencyMicroSeconds = 30 * 1000 * 1000;

CShutdownWatchDog::CShutdownWatchDog(CCommsdatNotifierServer& aNotifierServer)
	:CTimer(EPriorityStandard),iNotifierServer(aNotifierServer)
	{
	}

void CShutdownWatchDog::StartShutdown()
/**
Queue ourselves to shut this server down after a suitable delay.
*/
	{
	if (!IsAdded())
		{
		CActiveScheduler::Add(this);
		}
	if(!IsActive())
		{
		After(KNotifierServerShutdownLatencyMicroSeconds);
		}
	}

void CShutdownWatchDog::RunL()
/**
Call the server to shut itself down - this will do nothing if there are active sessions.
*/
	{
	iNotifierServer.MaybeShutDown();
	}

CShutdownWatchDog* CShutdownWatchDog::NewL(CCommsdatNotifierServer& aNotifierServer)
	{
	CShutdownWatchDog* self=new CShutdownWatchDog(aNotifierServer);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;
	}
        
        
        	

