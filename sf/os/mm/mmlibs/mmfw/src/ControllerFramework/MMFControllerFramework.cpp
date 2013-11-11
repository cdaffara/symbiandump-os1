// Copyright (c) 2002-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#include <e32math.h>
#include <mmf/server/mmfdrmpluginserverproxy.h>
#include "mmfcontrollerframework.h"
#include "mmfcontroller.h"
#include "mmfcontrollerheap.h"
#include "mmfcontrollerframeworkpriv.h"
#include "mmfcontrollerpatchdata.h"

#ifdef SYMBIAN_ENABLE_SPLIT_HEADERS
#include <mmf/common/mmfcontrollerextendeddata.h>
#include <mmf/common/mmfcustomcommandparsermanager.h>
#include <mmf/common/mmfcontrollerframeworkclasses.h>
#endif

// Panic
   
enum
	{
	EPanicHeapHalfOpen=1,
	EPanicHeapOpenWithoutTls,
	EPanicReleaseWithoutRegister,
	EPanicBadInvariant
	};
	
#ifdef _DEBUG
static void Panic(TInt aReason)
	{
	_LIT(KControllerFramework, "ControllerFramework");
	User::Panic(KControllerFramework, aReason);
	}
#endif

EXPORT_C RMMFControllerProxy::RMMFControllerProxy() :
	iDestinationPckg(TMMFMessageDestination(KUidInterfaceMMFControllerProxy, KMMFObjectHandleControllerProxy)),
	iLogonAO(NULL), iThreadPriority(static_cast<TThreadPriority>(KDefaultMMFControllerThreadPriority)), iFlags(0)
	{
	iSubThread.Close();//iSubThread is automatically initialised to be a handle to this thread!
	}

TInt RMMFControllerProxy::LoadController(
	TUid aControllerUid, 
 	const CMMFControllerImplementationInformation& aControllerInfo,
 	TBool aUseSharedHeap,
 	TBool aNoDRMCap)
	{
	// First check that we haven't already created the subthread
	if (iSubThread.Handle() != 0)
		return KErrAlreadyExists;
	
#ifdef SYMBIAN_FORCE_USE_SHARED_HEAP
	aUseSharedHeap = ETrue;
#endif

	//determine what maximum heap size this thread should be created with
	TUint maxHeapSize =0;
	TInt error = KErrNone;
	maxHeapSize = aControllerInfo.HeapSpaceRequired();
	TUint stackSize = aControllerInfo.StackSize();
	
	ASSERT(!iLogonAO);
	TRAP(error, iLogonAO = CLogonMonitor::NewL(this));
	
	if (!error)
		{
		if (aNoDRMCap && aControllerInfo.SupportsSecureDRMProcessMode())
			{
			error = DoCreateSessionForNoDRMCapClient(maxHeapSize, aUseSharedHeap, stackSize);
			}
		else
			{
			// server2 will be set in this function call
			error = DoCreateSubThread(&iLogonAO->Server(), maxHeapSize, aUseSharedHeap, stackSize);
		
			// Now create a session with the controller proxy server running in the subthread
			if (!error)
				{
				// create a session with iServer2 (local server)
				error = CreateSession(iLogonAO->Server(), KMMFControllerProxyVersion);
				}
			}
		}

	// Finally, tell the controller proxy server to load the relevant plugin
	if (!error)
		{
		TMMFUidPckg uidPckg(aControllerUid);
		error = SendSync(iDestinationPckg, 
						 EMMFControllerProxyLoadControllerPluginByUid, 
						 uidPckg, 
						 KNullDesC8);
		}

	// If an error occurred with any of the above, close all the handles
	if (error)
		Close();

	return error;
	}


EXPORT_C TInt RMMFControllerProxy::LoadController(TUid aControllerUid, TBool aUseSharedHeap)
	{
 	CMMFControllerImplementationInformation* controllerInfo = NULL;
 	
 	TRAPD(err, controllerInfo = CMMFControllerImplementationInformation::NewL(aControllerUid));
	if (!err && controllerInfo)
		{
		err = LoadController(aControllerUid, *controllerInfo, aUseSharedHeap, EFalse);
		delete controllerInfo;
		}
 	return err;
	}

EXPORT_C TInt RMMFControllerProxy::LoadController(const CMMFControllerImplementationInformation& aControllerInfo, TBool aUseSharedHeap)
	{
	return LoadController(aControllerInfo.Uid(), aControllerInfo, aUseSharedHeap, EFalse);
	}

EXPORT_C TInt RMMFControllerProxy::LoadControllerInSecureDRMProcess(TUid aControllerUid, TBool aUseSharedHeap)
	{
	CMMFControllerImplementationInformation* controllerInfo = NULL;
	
	TRAPD(err, controllerInfo = CMMFControllerImplementationInformation::NewL(aControllerUid));
	if (!err && controllerInfo)
		{
		err = LoadController(aControllerUid, *controllerInfo, aUseSharedHeap, ETrue);
		delete controllerInfo;
		}
	return err;
	}
	
EXPORT_C TInt RMMFControllerProxy::LoadControllerInSecureDRMProcess(const CMMFControllerImplementationInformation& aControllerInfo, TBool aUseSharedHeap)
	{
	return LoadController(aControllerInfo.Uid(), aControllerInfo, aUseSharedHeap, ETrue);
	}

TUint RMMFControllerProxy::ControllersMaxHeapSizeL(TUid aControllerUid)
	{
	CMMFControllerImplementationInformation* controllerInfo = NULL;

	TRAPD(err, controllerInfo = CMMFControllerImplementationInformation::NewL(aControllerUid));


	TUint maxHeapSize = KMMFDefaultControllerThreadHeapSize;
	
	if((err != KErrNone) && (err != KErrCorrupt))
		{
		delete controllerInfo;
		User::Leave(err);
		}


	if(controllerInfo && (err == KErrNone))
		maxHeapSize = controllerInfo->HeapSpaceRequired();

	delete controllerInfo;

	return maxHeapSize;
	}


TInt RMMFControllerProxy::DoCreateSubThread(RServer2* aServer2, TUint aMaxHeapSize, TBool aUseSharedHeap, TUint aStackSize)
	{
	TInt error = KErrNone;

	TControllerProxyServerParams params;
	params.iServer = aServer2;
	params.iUsingSharedHeap = aUseSharedHeap;

#ifdef SYMBIAN_USE_CLIENT_HEAP
	// controller threads share the *client* heap (intended for out of memory testing)
	error = iSubThread.Create(_L(""), &CMMFControllerProxyServer::StartThread, 
			aStackSize, NULL, &params, EOwnerThread);
#else
	if( aUseSharedHeap )
		{
		//controller threads all share a controller heap
		CMMFControllerHeap* contHeap = static_cast<CMMFControllerHeap*>(Dll::Tls());
		if(contHeap == NULL)
			{
			TRAP(error, contHeap = CMMFControllerHeap::NewL());
			if(error)
				{			
				return error;	
				}
				
			Dll::SetTls(contHeap);
			}
			
		__ASSERT_DEBUG((iFlags&EFlagOpenedSharedHeap)==0, Panic(EPanicHeapHalfOpen));
			
		RHeap* sharedHeap = contHeap->RegisterHeap();
		// We've registered, so record the fact so can "unregister" on close or error
		iFlags |= EFlagOpenedSharedHeap; 
		
		error = iSubThread.Create(KNullDesC, &CMMFControllerProxyServer::StartThread,
				aStackSize, sharedHeap, 						
				&params, EOwnerThread);		
		}
	else
		{	
		// Threads create own heap (default behaviour)
		if(aMaxHeapSize < static_cast<TUint>(KMinHeapSize))
			aMaxHeapSize = KMinHeapSize; //else raises a USER 111 panic
		else if(aMaxHeapSize >  static_cast<TUint>(KMMFControllerProxyMaxHeapSize))
			aMaxHeapSize = KMMFControllerProxyMaxHeapSize;
		
		TThreadCreateInfo threadSettings (KNullDesC, &CMMFControllerProxyServer::StartThread,
										  aStackSize, &params);
		threadSettings.SetCreateHeap(KMinHeapSize, aMaxHeapSize);
		threadSettings.SetOwner(EOwnerThread);
		threadSettings.SetPaging(TThreadCreateInfo::EUnpaged);
		
		error = iSubThread.Create(threadSettings);
		}
#endif

	if (error)
		{
		return error;
		}

	TRequestStatus rendezvous;
	iSubThread.Rendezvous(rendezvous);
	if (rendezvous != KRequestPending)
		{
		iSubThread.Kill(0);
		}
	else
		{
		iLogonAO->StartMonitoring(iSubThread);
		if (iLogonAO->iStatus != KRequestPending)
			{
			// Failed to logon
			iSubThread.RendezvousCancel(rendezvous);
			User::WaitForRequest(rendezvous);
			iSubThread.Kill(0);
			iSubThread.Close();
			return iLogonAO->iStatus.Int();
			}
		else
			{
			iSubThread.SetPriority(iThreadPriority);
			iSubThread.Resume();
			}
		}
		
	User::WaitForRequest(rendezvous); // wait for startup or death
	
	if (rendezvous != KErrNone)
		{
		iLogonAO->Cancel();
		iSubThread.Close();
		// if open failed, but we registered the heap, need to release
		if((iFlags&EFlagOpenedSharedHeap))
			{
			ReleaseHeap();
			}		
		}
		
	return rendezvous.Int();
	}

EXPORT_C void RMMFControllerProxy::Close()
	{
#ifdef _DEBUG
	_LIT(KMMFClientThreadPanic, "MMFClientThread");
#endif
	// check if thread was created
	TBool subThreadCreated = EFalse;
	TRequestStatus logoffStatus;
	TBool logoffFailed = EFalse;
	if (iSubThread.Handle() != KNullHandle)
		{
		subThreadCreated = ETrue;
		iLogonAO->Cancel();
		iSubThread.Logon(logoffStatus);
		
		if (logoffStatus == KErrNoMemory && iSubThread.ExitType() == EExitPending)
			{
			// Logon() call has failed because of a lack of memory
			logoffFailed = ETrue;
			}
		}

	// Close the controller and wait for its exit.
	// Close the session to signal the controller proxy server to shut down.
	RHandleBase::Close();
	
	// Now wait for the death of the subthread if we have a valid handle...
	if (subThreadCreated)
		{
		RProcess thisProcess;
		RProcess controllerProcess;
		iSubThread.Process(controllerProcess);	// ignore error, best try
		TBool secureDrmMode = thisProcess.Id() != controllerProcess.Id();
		thisProcess.Close();
		controllerProcess.Close();
				
		RTimer timer;
		TInt err = timer.CreateLocal();
		// If we managed to create the timer and logon to the thread, 
		// wait for both the death and the timeout to minimise the risk of deadlock
		if (!err && !logoffFailed)
			{
			TRequestStatus timeout;
			timer.After(timeout, KMmfControllerThreadShutdownTimeout);
			User::WaitForRequest(logoffStatus, timeout);
			if (logoffStatus == KRequestPending)
				{
				// we have timed out.  Kill the controller thread
				iSubThread.LogonCancel(logoffStatus);
				User::WaitForRequest(logoffStatus);
				
				if (!secureDrmMode)
					{
					// Controller server thread is created in current process
				#ifdef _DEBUG
					iSubThread.Panic(KMMFClientThreadPanic,KErrDied);
				#else
					iSubThread.Kill(KErrDied);
				#endif
					}
				else
					{
					// Controller server thread is created through DRM plugin server
					RMMFDRMPluginServerProxy server;
					// ignore all RMMFDRMPluginServerProxy errors, best try
					err = server.Open();
					if (err == KErrNone)
						{
					#ifdef _DEBUG
						server.PanicControllerThread(iSubThread.Id(), KMMFClientThreadPanic, KErrDied);
					#else
						server.KillControllerThread(iSubThread.Id(), KErrDied);
					#endif
						server.Close();
						}
					}
				}
			else
				{
				// subthread has exited. Cancel the timer.
				timer.Cancel();
				User::WaitForRequest(timeout);
				}
			}
		else
			{
			// We have no timer or we can't logon to the thread so we'll just poll the thread status a maximum
			// of 10 times and kill the thread if it hasn't exited after the polling
			for (TInt i=0; i<10 && iSubThread.ExitType() == EExitPending; ++i)
				{
				User::After(KMmfControllerThreadShutdownTimeout/10);	// wait for a while
				}
				
			if (iSubThread.ExitType() == EExitPending)
				{
				// The polling hasn't been succesful so we kill the thread
				if (!secureDrmMode)
					{
					iSubThread.Kill(KErrDied);
					}
				else
					{
					// Controller server thread is created through DRM plugin server
					RMMFDRMPluginServerProxy server;
					// ignore all RMMFDRMPluginServerProxy errors, best try
					err = server.Open();
					if (err == KErrNone)
						{
						server.KillControllerThread(iSubThread.Id(), KErrDied);
						}
					}
				}
				
			User::WaitForRequest(logoffStatus);
			}
		timer.Close();
		}

	// Close the handle to the controller thread
	iSubThread.Close();
	// Delete the Logon AO
	if (iLogonAO)
		{
		delete iLogonAO;
		iLogonAO = NULL;
		}
	// if this is last thread to be killed delete shared heap
	if((iFlags&EFlagOpenedSharedHeap))
		{
		ReleaseHeap();
		}	
	}

// Release the shared heap, should only be called if has previously been registered
// by this thread
void RMMFControllerProxy::ReleaseHeap()
	{
	__ASSERT_DEBUG((iFlags&EFlagOpenedSharedHeap), Panic(EPanicReleaseWithoutRegister));

	CMMFControllerHeap* contHeap = static_cast<CMMFControllerHeap*>(Dll::Tls());
	__ASSERT_DEBUG(contHeap!=NULL, Panic(EPanicHeapOpenWithoutTls));
	
	if(contHeap != NULL)
		{
		TInt refCount = contHeap->ReleaseHeap();
		if(refCount == 0)
			{  //no other controllers using the heap
			delete contHeap;
			Dll::SetTls(NULL);
			}
		}
		
	iFlags &= ~EFlagOpenedSharedHeap; // clear flag since we've released the heap
	}

EXPORT_C TInt RMMFControllerProxy::SendSync(const TMMFMessageDestinationPckg& aDestination, TInt aFunction, const TDesC8& aDataTo1, const TDesC8& aDataTo2, TDes8& aDataFrom) const
	{
	// Make sure we have been opened
	if (iSubThread.Handle() == 0)
		{
		return KErrNotReady;
		}
	else
		return SendReceiveResult(aFunction, aDestination, aDataTo1, aDataTo2, aDataFrom);
	}


EXPORT_C TInt RMMFControllerProxy::SendSync(TInt aFunction, const TIpcArgs& aIpcArgs) const
	{
	// Make sure we have been opened
	if (iSubThread.Handle() == 0)
		{
		return KErrNotReady;
		}
	else
		return RSessionBase::SendReceive(aFunction, aIpcArgs);
	}
	
EXPORT_C void RMMFControllerProxy::SendAsync(TInt aFunction, const TIpcArgs& aIpcArgs, TRequestStatus& aStatus) const
	{
	// Make sure we have been opened
	if (iSubThread.Handle() == 0)
		{
		TRequestStatus* status = &aStatus;
		User::RequestComplete(status, KErrNotReady);
		}
	else
		RSessionBase::SendReceive(aFunction, aIpcArgs, aStatus);
	}



EXPORT_C TInt RMMFControllerProxy::SendSync(const TMMFMessageDestinationPckg& aDestination, TInt aFunction, const TDesC8& aDataTo1, const TDesC8& aDataTo2) const
	{
	// Make sure we have been opened
	if (iSubThread.Handle() == 0)
		{
		return KErrNotReady;
		}
	else
		return SendReceive(aFunction, aDestination, aDataTo1, aDataTo2);
	}

EXPORT_C void RMMFControllerProxy::SendAsync(const TMMFMessageDestinationPckg& aDestination, TInt aFunction, const TDesC8& aDataTo1, const TDesC8& aDataTo2, TDes8& aDataFrom, TRequestStatus& aStatus) const
	{
	if (iSubThread.Handle() == 0)
		{
		TRequestStatus* stat = &aStatus;
		User::RequestComplete(stat, KErrNotReady);
		}
	else
		SendReceiveResult(aFunction, aDestination, aDataTo1, aDataTo2, aDataFrom, aStatus);
	}

EXPORT_C void RMMFControllerProxy::SendAsync(const TMMFMessageDestinationPckg& aDestination, TInt aFunction, const TDesC8& aDataTo1, const TDesC8& aDataTo2, TRequestStatus& aStatus) const
	{
	if (iSubThread.Handle() == 0)
		{
		TRequestStatus* stat = &aStatus;
		User::RequestComplete(stat, KErrNotReady);
		}
	else
		SendReceive(aFunction, aDestination, aDataTo1, aDataTo2, aStatus);
	}

EXPORT_C void RMMFControllerProxy::ReceiveEvents(TMMFEventPckg& aEvent, TRequestStatus& aStatus)
	{
	SendAsync(iDestinationPckg, EMMFControllerProxyReceiveEvents, KNullDesC8, KNullDesC8, aEvent, aStatus);
	}

EXPORT_C TInt RMMFControllerProxy::CancelReceiveEvents()
	{
	return SendSync(iDestinationPckg, EMMFControllerProxyCancelReceiveEvents, KNullDesC8, KNullDesC8);
	}

EXPORT_C TInt RMMFControllerProxy::SetThreadPriority(const TThreadPriority& aPriority) const
	{
	TInt err = KErrNone;

	if (iSubThread.Handle() == 0)
		{
		err = KErrNotReady;
		}
	else
		{
		if (iThreadPriority != aPriority)
			{
			// This is a const method so cast it away to store the priority
			RMMFControllerProxy* nonConstThis = const_cast<RMMFControllerProxy*>(this);
			nonConstThis->iThreadPriority = aPriority;
			}
			
		// check controller thread creation location
		RProcess thisProcess;
		RProcess controllerProcess;
		err = iSubThread.Process(controllerProcess);
		
		if (err == KErrNone)
			{
			if (thisProcess.Id() == controllerProcess.Id())
				{
				// Controller server thread is created in current process
				if (iSubThread.Priority() != iThreadPriority)
					{
					iSubThread.Suspend();
					iSubThread.SetPriority(iThreadPriority);
					iSubThread.Resume();
					}
				}
			else
				{
				// Controller server thread is created through DRM plugin server
				RMMFDRMPluginServerProxy server;
				err = server.Open();
				if (err == KErrNone)
					{
					err = server.SetThreadPriority(iSubThread.Id(), iThreadPriority);
					server.Close();
					}
				}
			}
		thisProcess.Close();
		controllerProcess.Close();
		}
	return err;
	}
	
void RMMFControllerProxy::ThreadTerminated()
	{
	// The controller subthread has died and the controller should be closed
	iSubThread.Close();
	}

TInt RMMFControllerProxy::DoCreateSessionForNoDRMCapClient(TUint aMaxHeapSize, TBool aUseSharedHeap, TUint aStackSize)
	{
	TThreadId tid;
	RMMFDRMPluginServerProxy server;
    TInt error = server.Open();
    
    if(!error)
    	{
    	error = server.LaunchControllerServer(aMaxHeapSize, aUseSharedHeap, tid, aStackSize);
    	}		    
    if(!error)
		{
		error = iSubThread.Open(tid, EOwnerThread);
		if(!error)
			{
			iLogonAO->StartMonitoring(iSubThread);
			if (iLogonAO->iStatus != KRequestPending)
				{
				// Failed to logon
				server.KillControllerThread(tid, 0);
				iSubThread.Close();
				error = iLogonAO->iStatus.Int();
				}
			}
		else
			{
			// Failed to open thread handle
			server.KillControllerThread(tid, 0);
			}
		}
	if(!error)
		{
		error = server.SetThreadPriority(tid, iThreadPriority);
		if(!error)
			{
			error = SetReturnedHandle(server.GetControllerSessionHandle());	
			}
		if(error)
			{
			// Failed to create session with controller
			iLogonAO->Cancel();
			server.KillControllerThread(tid, 0);
			iSubThread.Close();
			}
		}
    server.Close();
    return error;
	}


// RMMFCustomCommandsBase

/**
Constructor.

@param  aController
        A reference to the controller client class that will be used to send
        custom commands to the controller plugin.
@param  aInterfaceId
        The UID of the custom command interface that is provided by this client
        API class.

@since 7.0s
*/
EXPORT_C RMMFCustomCommandsBase::RMMFCustomCommandsBase(RMMFController& aController, TUid aInterfaceId)
	: iController(aController), iDestinationPckg(aInterfaceId)
	{
	}


// TMMFMessageDestination
EXPORT_C TMMFMessageDestination::TMMFMessageDestination()
	: iInterfaceId(KNullUid), iDestinationHandle(KMMFObjectHandleController)
	{
	}

EXPORT_C TMMFMessageDestination::TMMFMessageDestination(TUid aInterfaceId)
	: iInterfaceId(aInterfaceId), iDestinationHandle(KMMFObjectHandleController)
	{
	}

EXPORT_C TMMFMessageDestination::TMMFMessageDestination(TUid aInterfaceId, TInt aDestinationHandle)
	: iInterfaceId(aInterfaceId), iDestinationHandle(aDestinationHandle)
	{
	}

EXPORT_C TMMFMessageDestination::TMMFMessageDestination(const TMMFMessageDestination& aOther)
	: iInterfaceId(aOther.iInterfaceId), iDestinationHandle(aOther.iDestinationHandle)
	{
	}

EXPORT_C TUid TMMFMessageDestination::InterfaceId() const
	{
	return iInterfaceId;
	}

EXPORT_C TInt TMMFMessageDestination::DestinationHandle() const
	{
	return iDestinationHandle;
	}

EXPORT_C TBool TMMFMessageDestination::operator==(const TMMFMessageDestination& aOther) const
	{
	return ((iInterfaceId==aOther.iInterfaceId) && (iDestinationHandle==aOther.iDestinationHandle));
	}



// CMMFControllerProxySession
CMMFControllerProxySession* CMMFControllerProxySession::NewL()
	{
	CMMFControllerProxySession* s = new(ELeave) CMMFControllerProxySession;
	return s;
	}

void CMMFControllerProxySession::CreateL(const CMmfIpcServer& aServer)
	{
	CMmfIpcSession::CreateL(aServer);
	iServer = STATIC_CAST(CMMFControllerProxyServer*, (CONST_CAST(CMmfIpcServer*, &aServer)));
	iServer->SessionCreated();
	}

CMMFControllerProxySession::~CMMFControllerProxySession()
	{
	delete iController;
	delete iEventReceiver;
	iEvents.Close();
	iServer->SessionDestroyed();
	}



void CMMFControllerProxySession::ServiceL(const RMmfIpcMessage& aMessage)
	{
	TMMFMessage message(aMessage);
	// Get the destination info from the client.
	message.FetchDestinationL();

	if (message.Destination().InterfaceId() == KUidInterfaceMMFControllerProxy)
		{
		// Message for controller proxy so decode here
		TBool complete = EFalse;
		switch (message.Function())
			{
		case EMMFControllerProxyReceiveEvents:
			complete = ReceiveEventsL(message);
			break;
		case EMMFControllerProxyCancelReceiveEvents:
			complete = CancelReceiveEvents(message);
			break;
		case EMMFControllerProxyLoadControllerPluginByUid:
			complete = LoadControllerL(message);
			break;
		default:
			User::Leave(KErrNotSupported);
			}
		if (complete)
			message.Complete(KErrNone);
		}
	else
		{
		// Message for controller so forward on to controller baseclass
		if (iController)
			iController->HandleRequestL(message);
		else
			User::Leave(KErrNotReady);
		}
	}

CMMFControllerProxySession::CMMFControllerProxySession()
	{
	}

TBool CMMFControllerProxySession::ReceiveEventsL(TMMFMessage& aMessage)
	{
	if (iEventReceiver)
		User::Leave(KErrAlreadyExists);
	iEventReceiver = CMMFEventReceiver::NewL(aMessage);
	//send the next cached event (if any) to the client
	if (iEvents.Count() > 0)
		{
		TMMFEvent& event = iEvents[0];
		iEventReceiver->SendEvent(event);
		delete iEventReceiver;
		iEventReceiver=NULL;
		iEvents.Remove(0);
		}
	return EFalse;
	}

TBool CMMFControllerProxySession::CancelReceiveEvents(TMMFMessage& /*aMessage*/)
	{
	delete iEventReceiver;
	iEventReceiver = NULL;
	return ETrue;
	}

TInt CMMFControllerProxySession::SendEventToClient(const TMMFEvent& aEvent)
	{
	TInt error = KErrNone;
	if (iEventReceiver)
		{
		//send event to client now
		iEventReceiver->SendEvent(aEvent);
		delete iEventReceiver;
		iEventReceiver=NULL;
		error = KErrNone;
		}
	else
		{
		//queue the request for later
		TMMFEvent event(aEvent);
		//if we've exceeded the max number of cached messages, delete the first and append this one to the end
		if (iEvents.Count() >= KMMFControllerProxyMaxCachedMessages)
			iEvents.Remove(0);
		error = iEvents.Append(event);
		}
	return error;
	}

TBool CMMFControllerProxySession::LoadControllerL(TMMFMessage& aMessage)
	{
	TMMFUidPckg pckg;
	aMessage.ReadData1FromClientL(pckg);
	
	RThread clientThread;
	aMessage.iMessage.ClientL(clientThread);
	CleanupClosePushL(clientThread);
	iController = CMMFController::NewL(pckg(), *this, clientThread.Id());
	CleanupStack::PopAndDestroy(&clientThread);
	return ETrue;
	}



CMMFControllerProxyServer* CMMFControllerProxyServer::NewL(RServer2* aServer2)
	{
	CMMFControllerProxyServer* s = new(ELeave) CMMFControllerProxyServer();
	CleanupStack::PushL(s);
	s->ConstructL(aServer2);
	CleanupStack::Pop(s);
	return s;
	}

CMMFControllerProxyServer::CMMFControllerProxyServer() :
	CMmfIpcServer(EPriorityStandard, EGlobalSharableSessions)
	{
	}

void CMMFControllerProxyServer::ConstructL(RServer2* aServer2)
	{
	SetPinClientDescriptors(ETrue);
	
	StartL(KNullDesC);
	*aServer2 = CServer2::Server();
	
	iShutdownTimer = CMMFControllerProxyShutdown::NewL();
	iShutdownTimer->Start();
	}

CMMFControllerProxyServer::~CMMFControllerProxyServer()
	{
	delete iShutdownTimer;
	}

EXPORT_C TInt CMMFControllerProxyServer::StartThread(TAny* aParam)
	{
	TInt err = KErrNone;
	//create cleanupstack
	CTrapCleanup* cleanup=CTrapCleanup::New(); // get clean-up stack
	if (!cleanup)
		err = KErrNoMemory;
	if (!err)
		{
		TRAP(err, DoStartThreadL(aParam));
		}
	delete cleanup;
	return err;
	}
	
void CMMFControllerProxyServer::RenameControllerProxyThread()
	{
	RThread thread;
	TThreadId threadId;
	TName name;
	name.Append(KMMFControllerProxyServerName);
	threadId = thread.Id();
	name.AppendNum(threadId.Id(),EHex);
	User::RenameThread(name);
	}

void CMMFControllerProxyServer::DoStartThreadL(TAny* aParam)
	{
	TControllerProxyServerParams* params = static_cast<TControllerProxyServerParams*>(aParam);
	
	//Rename Current thread to unique name
	RenameControllerProxyThread();
	
#ifndef SYMBIAN_USE_CLIENT_HEAP
	TBool usingSharedHeap = params->iUsingSharedHeap; // take copy since params invalid after Rendezvous
	
	if( ! usingSharedHeap )
		{
		__UHEAP_MARK;
		}

#endif
	// create and install the active scheduler we need
	CActiveScheduler* s=new(ELeave) CActiveScheduler;
	CleanupStack::PushL(s);
	CActiveScheduler::Install(s);
	// create the server (leave it on the cleanup stack)
	CleanupStack::PushL(CMMFControllerProxyServer::NewL( params->iServer ) );
	// Initialisation complete, now signal the client
	RThread::Rendezvous(KErrNone);
	// Ready to run
	CActiveScheduler::Start();
	
	REComSession::FinalClose();
	
	// Cleanup the server and scheduler
	CleanupStack::PopAndDestroy(2, s);
#ifndef SYMBIAN_USE_CLIENT_HEAP
	if( ! usingSharedHeap )
		{
		__UHEAP_MARKEND;
		}
#endif
	}

TInt CMMFControllerProxyServer::RunError(TInt aError)
	{
	// Send error to the client
	Message().Complete(aError);
	ReStart();
	return KErrNone;
	}

CMmfIpcSession* CMMFControllerProxyServer::NewSessionL(const TVersion& aVersion) const
	{
	// Only one session is allowed to connect to us
	if (iHaveSession)
		User::Leave(KErrAlreadyExists);
	// Check that the version number of the client is correct
	if (!User::QueryVersionSupported(KMMFControllerProxyVersion, aVersion))
		User::Leave(KErrNotSupported);

	return CMMFControllerProxySession::NewL();
	}

void CMMFControllerProxyServer::SessionCreated()
	{
	iHaveSession = ETrue;
	// Stop the shutdown timer
	iShutdownTimer->Cancel();
	}

void CMMFControllerProxyServer::SessionDestroyed()
	{
	// Need to shut down
	iShutdownTimer->ShutdownNow();
	}

CMMFControllerProxyShutdown* CMMFControllerProxyShutdown::NewL() 
	{
	CMMFControllerProxyShutdown* s = new(ELeave) CMMFControllerProxyShutdown();
	CleanupStack::PushL(s);
	s->ConstructL();
	CleanupStack::Pop(s);
	return s;
	}

CMMFControllerProxyShutdown::CMMFControllerProxyShutdown() : 
	CTimer(EPriorityLow)
	{
	CActiveScheduler::Add(this);
	}

void CMMFControllerProxyShutdown::ConstructL()
	{
	CTimer::ConstructL();
	}

void CMMFControllerProxyShutdown::Start()
	{
	After(EMMFControllerProxyShutdownDelay);
	}

void CMMFControllerProxyShutdown::RunL()
	{
	ShutdownNow();
	}

void CMMFControllerProxyShutdown::ShutdownNow()
	{
	CActiveScheduler::Stop();
	}







CMMFEventReceiver* CMMFEventReceiver::NewL(const TMMFMessage& aMessage)
	{
	return new(ELeave) CMMFEventReceiver(aMessage);
	}

CMMFEventReceiver::~CMMFEventReceiver()
	{
	if (!(iMessage.IsCompleted()))
		iMessage.Complete(KErrDied);
	}

void CMMFEventReceiver::SendEvent(const TMMFEvent& aEvent)
	{
	TMMFEventPckg eventpckg(aEvent);
	TInt err = iMessage.WriteDataToClient(eventpckg);
	iMessage.Complete(err);
	}

CMMFEventReceiver::CMMFEventReceiver(const TMMFMessage& aMessage) : iMessage(aMessage)
	{
	}



// TMMFMessage
EXPORT_C TMMFMessage::TMMFMessage(const TMMFMessage& aMessage) :
	iMessage(aMessage.iMessage), 
#ifdef __MMF_USE_IPC_V2__
	iFunction(aMessage.iFunction),
#endif  // __MMF_USE_IPC_V2__
	iDestination(aMessage.iDestination), 
	iAmCompleted(aMessage.iAmCompleted)
	{
	}

EXPORT_C const TMMFMessageDestination& TMMFMessage::Destination()
	{
	return iDestination;
	}

EXPORT_C TInt TMMFMessage::Function()
	{
#ifdef __MMF_USE_IPC_V2__
	return iFunction;
#else
	return iMessage.Function();
#endif // __MMF_USE_IPC_V2__
	}

EXPORT_C TInt TMMFMessage::SizeOfData1FromClient()
	{
#ifdef __MMF_USE_IPC_V2__
	return iMessage.GetDesLength(1);
#else
	return iMessage.Client().GetDesLength(iMessage.Ptr1());
#endif // __MMF_USE_IPC_V2__
	}

EXPORT_C void TMMFMessage::ReadData1FromClientL(TDes8& aDes)
	{
#ifdef __MMF_USE_IPC_V2__
	iMessage.ReadL(1, aDes);
#else
	iMessage.ReadL(iMessage.Ptr1(), aDes);
#endif // __MMF_USE_IPC_V2__
	}

EXPORT_C TInt TMMFMessage::ReadData1FromClient(TDes8& aDes)
	{
	TRAPD(err, ReadData1FromClientL(aDes));
	return err;
	}

EXPORT_C TInt TMMFMessage::SizeOfData2FromClient()
	{
#ifdef __MMF_USE_IPC_V2__
	return iMessage.GetDesLength(2);
#else
	return iMessage.Client().GetDesLength(iMessage.Ptr2());
#endif // __MMF_USE_IPC_V2__
	}

EXPORT_C void TMMFMessage::ReadData2FromClientL(TDes8& aDes)
	{
#ifdef __MMF_USE_IPC_V2__
	iMessage.ReadL(2, aDes);
#else
	iMessage.ReadL(iMessage.Ptr2(), aDes);
#endif // __MMF_USE_IPC_V2__
	}

EXPORT_C TInt TMMFMessage::ReadData2FromClient(TDes8& aDes)
	{
	TRAPD(err, ReadData2FromClientL(aDes));
	return err;
	}

EXPORT_C void TMMFMessage::WriteDataToClientL(const TDesC8& aDes)
	{
#ifdef __MMF_USE_IPC_V2__
	iMessage.WriteL(3, aDes);
#else
	iMessage.WriteL(iMessage.Ptr3(), aDes);
#endif // __MMF_USE_IPC_V2__
	}

EXPORT_C TInt TMMFMessage::WriteDataToClient(const TDesC8& aDes)
	{
	TRAPD(err, WriteDataToClientL(aDes));
	return err;
	}

EXPORT_C void TMMFMessage::Complete(TInt aReason)
	{
	iMessage.Complete(aReason);
	iAmCompleted = ETrue;
	}

EXPORT_C TBool TMMFMessage::IsCompleted()
	{
	return iAmCompleted;
	}
	
EXPORT_C void TMMFMessage::AdoptFileHandleFromClientL(TInt aFsHandleIndex, TInt aFileHandleIndex, RFile& aFile)
	{
	User::LeaveIfError(aFile.AdoptFromClient(RMessage2(iMessage) , aFsHandleIndex, aFileHandleIndex));
	}

EXPORT_C TMMFMessage::TMMFMessage(const RMmfIpcMessage& aMessage) :
	iMessage(aMessage), 
#ifdef __MMF_USE_IPC_V2__
	iFunction(aMessage.Function()),
#endif // __MMF_USE_IPC_V2__
	iAmCompleted(EFalse)
	{
	}

EXPORT_C void TMMFMessage::FetchDestinationL()
	{
	// Read the destination info from the client
	TMMFMessageDestinationPckg pckg;
#ifdef __MMF_USE_IPC_V2__
	iMessage.ReadL(0, pckg);
#else
	iMessage.ReadL(iMessage.Ptr0(), pckg);
#endif // __MMF_USE_IPC_V2__
	iDestination = pckg();
	}


// CMMFObject

/**
Constructor.

@param  aInterfaceId
        The UID of the interface provided by this object.
@since 7.0s
*/
EXPORT_C CMMFObject::CMMFObject(TUid aInterfaceId)
	: iHandle(aInterfaceId, KMMFObjectHandleNull)
	{
	}

/**
Destructor.
*/
EXPORT_C CMMFObject::~CMMFObject()
	{
	}

/**
Returns the handle of the object.

@return The handle of this object.

@since  7.0s
*/
EXPORT_C const TMMFMessageDestination& CMMFObject::Handle()
	{
	return iHandle;
	}

/**
Compares two CMMFObjects by comparing their handles.

@param  aOther
        The object to be compared with this object.

@return A boolean indicating if the two CMMFObjects are the same. ETrue if they are the same,
        EFalse if the objects are different.
@since  7.0s
*/
EXPORT_C TBool CMMFObject::operator==(const CMMFObject& aOther)
	{
	return (iHandle==aOther.iHandle);
	}

void CMMFObject::SetHandle(const TMMFMessageDestination& aNewHandle)
	{
	iHandle = aNewHandle;
	}


// CMMFDataSourceHolder

/**
Constructor.

@param  aDataSource
        The data source to be wrapped.

@since  7.0s
*/
EXPORT_C CMMFDataSourceHolder::CMMFDataSourceHolder(MDataSource& aDataSource)
	: CMMFObject(KUidInterfaceMMFDataSourceHolder), iDataSource(&aDataSource)
	{
	}

/**
Destructor.

Note: This deletes the data source.

@since 7.0s
*/
EXPORT_C CMMFDataSourceHolder::~CMMFDataSourceHolder()
	{
	delete iDataSource;
	}

/**
Returns a reference to the data source.

@return The data source.

@since  7.0s
*/
EXPORT_C MDataSource& CMMFDataSourceHolder::DataSource()
	{
	return *iDataSource;
	}

/**
Implementation of the pure virtual function inherited from CMMFObject.

Passes the request directly to the data source.

@param  aMessage
        The message to be handled.

@since  7.0s
*/
EXPORT_C void CMMFDataSourceHolder::HandleRequest(TMMFMessage& aMessage)
	{
	iDataSource->SourceCustomCommand(aMessage);
	}


// CMMFDataSinkHolder

/**
Constructor.

@param  aDataSink
        The data sink to be wrapped.
@since 7.0s
*/
EXPORT_C CMMFDataSinkHolder::CMMFDataSinkHolder(MDataSink& aDataSink)
	: CMMFObject(KUidInterfaceMMFDataSinkHolder), iDataSink(&aDataSink)
	{
	}

/**
Destructor.

Note: This deletes the data sink.

@since  7.0s
*/
EXPORT_C CMMFDataSinkHolder::~CMMFDataSinkHolder()
	{
	delete iDataSink;
	}

/**
Returns a reference to the data sink.

@return The data sink.
@since 7.0s
*/
EXPORT_C MDataSink& CMMFDataSinkHolder::DataSink()
	{
	return *iDataSink;
	}

/**
Implementation of the pure virtual function inherited from CMMFObject.

Passes the request directly to the data sink.

@param  aMessage
        The message to be handled.

@since  7.0s
*/
EXPORT_C void CMMFDataSinkHolder::HandleRequest(TMMFMessage& aMessage)
	{
	iDataSink->SinkCustomCommand(aMessage);
	}


// CMMFCustomCommandParserBase
EXPORT_C TUid CMMFCustomCommandParserBase::InterfaceId()
	{
	return iInterfaceId;
	}

EXPORT_C CMMFCustomCommandParserBase::~CMMFCustomCommandParserBase()
	{
	}

EXPORT_C CMMFCustomCommandParserBase::CMMFCustomCommandParserBase(TUid aInterfaceId)
	: iInterfaceId(aInterfaceId)
	{
	}


// CMMFObjectContainer

/**
Constructor.

@since  7.0s
*/
EXPORT_C CMMFObjectContainer::CMMFObjectContainer()
	{
	iNextObjectHandle = KMMFObjectHandleFirstValid;
	}

/**
Destructor.

Deletes all objects owned by the container.

@since  7.0s
*/
EXPORT_C CMMFObjectContainer::~CMMFObjectContainer()
	{
	iObjects.ResetAndDestroy();
	iObjects.Close();
	}

/**
Add an object to the container.

Once the object has been added, its ownership is transferred to the container.

@param  aObject
        A reference to the object to be added to the container.

@return	An error code indicating if the function call was successful. If the return code is not KErrNone, then
		ownership of the object still remains with the caller.
@since  7.0s
*/
EXPORT_C TInt CMMFObjectContainer::AddMMFObject(CMMFObject& aObject)
	{
	// Check the object isn't already added
	if (aObject.Handle().DestinationHandle() != KMMFObjectHandleNull)
		return KErrAlreadyExists;

	TMMFMessageDestination newHandle(aObject.Handle().InterfaceId(), GenerateObjectHandle());
	// Set the object's handle
	aObject.SetHandle(newHandle);
	// Append the object to the array
	TInt error = iObjects.Append(&aObject);
	// If error occurred, reset object handle to NULL
	if (error)
		{
		TMMFMessageDestination evenNewerHandle(aObject.Handle().InterfaceId(), KMMFObjectHandleNull);
		aObject.SetHandle(evenNewerHandle);
		}
	return error;
	}

/**
Removes and destroys an object from the container.

This method ensures that the object is no longer in the container, and that it gets deleted.
Even if the object is not found in the container's array of objects, it will be deleted.

@param  aObject
        A reference to the object to be deleted.

@since  7.0s
*/
EXPORT_C void CMMFObjectContainer::RemoveAndDestroyMMFObject(CMMFObject& aObject)
	{
	TInt positionOfObjectInArray;
	TInt error = FindMMFObject(aObject, positionOfObjectInArray);
	if (!error)
		{
		iObjects.Remove(positionOfObjectInArray);
		}
	//else, we don't care since we're just saying we'll make sure the object
	//isn't in the array, and that it gets deleted.
	delete (&aObject);
	}

/**
Removes and destroys all objects from the container.

@since	7.0s
*/
EXPORT_C void CMMFObjectContainer::DeleteAllObjects()
	{
	iObjects.ResetAndDestroy();
	}

/**
Finds an object in the container using a handle.

@param  aObjectHandle
        The handle of the object to be located.
@param  aObjectFound
        A reference to a pointer to the object found in the container.

@return An error code indicating if the function call was successful. KErrNone on success, otherwise
        another of the system-wide error codes.
@since	7.0s
*/
EXPORT_C TInt CMMFObjectContainer::FindMMFObject(const TMMFMessageDestination& aObjectHandle, CMMFObject*& aObjectFound)
	{
	// Need to find the appropriate object in the array of CMMFObjects
	TInt error = KErrNotFound;
	for (TInt i=0; i<iObjects.Count(); i++)
		{
		CMMFObject* obj = iObjects[i];
		if (obj->Handle() == aObjectHandle)
			{
			error = KErrNone;
			aObjectFound = obj;
			break;
			}
		}
	return error;
	}

const RPointerArray<CMMFObject>& CMMFObjectContainer::MMFObjects()
	{
	return iObjects;
	}
	
TInt CMMFObjectContainer::FindMMFObject(const CMMFObject& aObject, TInt& aPositionInArray)
	{
	TInt error = KErrNotFound;
	for (TInt i=0; i<iObjects.Count(); i++)
		{
		CMMFObject* obj = iObjects[i];
		if (*obj == aObject)
			{
			error = KErrNone;
			aPositionInArray = i;
			break;
			}
		}
	return error;
	}

TInt CMMFObjectContainer::GenerateObjectHandle()
	{
	return iNextObjectHandle++;
	}


CLogonMonitor::CLogonMonitor(MLogonMonitorObserver* aLogonMonitorObserver) 
: CActive(EPriorityStandard)
	{
	iLogonMonitorObserver = aLogonMonitorObserver;
	}
	
void CLogonMonitor::ConstructL()
	{
	if (CActiveScheduler::Current()==0)
		{
		iScheduler = new (ELeave) CActiveScheduler;
		CActiveScheduler::Install(iScheduler);
		}
		
	CActiveScheduler::Add(this);
	}
	
CLogonMonitor* CLogonMonitor::NewL(MLogonMonitorObserver* aLogonMonitorObserver)
	{
	CLogonMonitor* self = new (ELeave) CLogonMonitor(aLogonMonitorObserver);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;
	}

CLogonMonitor::~CLogonMonitor()
	{
	Cancel();
	
	if (iScheduler)
		{
		CActiveScheduler::Install(NULL);
		delete iScheduler;
		}
	}

void CLogonMonitor::StartMonitoring(RThread& aThread)
	{
	ASSERT(!iThread);
	
	iThread = &aThread;
	iThread->Logon(iStatus);
	SetActive();
	}

void CLogonMonitor::RunL()
	{
	iServer.Close();
	iLogonMonitorObserver->ThreadTerminated();
	}

void CLogonMonitor::DoCancel()
	{
	ASSERT(iThread);
	
	iThread->LogonCancel(iStatus);
	iThread = NULL;
	}

RServer2& CLogonMonitor::Server()
	{
	return iServer;
	}
	
CMMFControllerExtendedData::CMMFControllerExtendedData()
	: CMMFObject(KUidMMFControllerExtendedDataHolder), iSourceSinkInitData(NULL)
	{
	iClientThreadId = 0;
	iSecureDrmMode = EFalse;
	}

CMMFControllerExtendedData::~CMMFControllerExtendedData()
	{
	ResetSourceSinkInitData();
	}
	
void CMMFControllerExtendedData::SetSourceSinkInitData(HBufC8* aSourceSinkInitData)
	{
	ResetSourceSinkInitData();
	iSourceSinkInitData = aSourceSinkInitData;
	}

HBufC8* CMMFControllerExtendedData::SourceSinkInitData() const
	{
	return iSourceSinkInitData;
	}

void CMMFControllerExtendedData::ResetSourceSinkInitData()
	{
	if (iSourceSinkInitData)
		{
		delete iSourceSinkInitData;
		iSourceSinkInitData = NULL;
		}
	}

void CMMFControllerExtendedData::SetClientThreadId(TThreadId aClientThreadId)
	{
	iClientThreadId = aClientThreadId;
	}

TThreadId CMMFControllerExtendedData::ClientThreadId() const
	{
	return iClientThreadId;
	}

void CMMFControllerExtendedData::SetSecureDrmMode(TBool aSecureDrmMode)
	{
	iSecureDrmMode = aSecureDrmMode;
	}

TBool CMMFControllerExtendedData::SecureDrmMode() const
	{
	return iSecureDrmMode;
	}

void CMMFControllerExtendedData::HandleRequest(TMMFMessage& /*aMessage*/)
	{
	// This function is not suppose to be called.
#ifdef _DEBUG
	Panic(EPanicBadInvariant);
#endif
	}

