// Copyright (c) 1999-2010 Nokia Corporation and/or its subsidiary(-ies).
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

#include <s32mem.h>
#include <e32svr.h>
#include "BTManServer.h"


#include "BTSec.h"
#include "BtManServerSecurityPolicy.h"
#include <bluetooth/logger.h>

#ifdef __FLOG_ACTIVE
_LIT8(KLogComponent, LOG_COMPONENT_BT_MANAGER_SERVER);
#endif

static const TInt KMessageArrayGranularity = 4; //< The granularity of the array used to hold TBTManMessage objects
static const TInt KBTManClientServerProtocolSlot = 2; // the slot that points to the struct buf used between client and server

inline CBTManServerShutdown::CBTManServerShutdown()
	:CTimer(-1)
	{
	LOG_FUNC
	CActiveScheduler::Add(this);
	}

inline void CBTManServerShutdown::ConstructL()
	{
	LOG_FUNC
	CTimer::ConstructL();
	}

inline void CBTManServerShutdown::Start()
	{
	LOG_FUNC
	After(KBTManServerShutdownDelay);
	}

//


void CBTManServerShutdown::RunL()
/**
Initiate server exit when the timer expires
**/
	{
	LOG_FUNC
	CActiveScheduler::Stop();
	}

//=====================================================================
// CBTManServer
//=====================================================================

inline CBTManServer::CBTManServer()
	:	CPolicyServer(EPriorityStandard,KBtmanServerPolicy,ESharableSessions)
	{
	LOG_FUNC
	}

CServer2* CBTManServer::NewLC()
	{
	LOG_STATIC_FUNC
	CBTManServer* self=new(ELeave) CBTManServer;
	CleanupStack::PushL(self);
	self->ConstructL();
	return self;
	}

void CBTManServer::ConstructL()
	{
	LOG_FUNC
	StartL(KBTManServerName);
	//Ensure that the server will exit even if the 1st client fails to connect
	iShutdown.ConstructL();
	iShutdown.Start();

	TInt err;
	err = iProperty.Define(KPropertyUidBluetoothCategory,
						   KPropertyKeyBluetoothGetRegistryTableChange,
						   RProperty::EInt,
						   KLOCAL_SERVICES,
						   KBTMAN_SID_PROT_SERV);
						   
	FLOGIFERR(err,_L("CBTManServer::ConstructL() - iProperty.Define Failure"));
		
	err = iProperty.Define(KPropertyUidBluetoothCategory,
						   KPropertyKeyBluetoothCorruptRegistryReset,
						   RProperty::EInt,
						   KLOCAL_SERVICES,
						   KLOCAL_SERVICES_AND_NETWORK_CONTROL);
						   
	FLOGIFERR(err,_L("CBTManServer::ConstructL() - iProperty.Define Failure"));

	//Create the service providers...
	iRegistry = CBTRegistry::NewL();

	iContainerIndex = CObjectConIx::NewL();
	// don't stop the server if we can't provide this service...				   
	
	//Start Backup/Restore Manager and obtain notification of backup/restore operations.
	iBURManager=CBTManServerBURMgr::NewL(*this, this);
	}


CObjectCon* CBTManServer::NewContainerL()
/**
Return a new object container
**/
	{
	LOG_FUNC
	return iContainerIndex->CreateL();
	}

void CBTManServer::DeleteContainer(CObjectCon* aCon)
	{
	LOG_FUNC
	iContainerIndex->Remove(aCon);
	}

CBTManServer::~CBTManServer()
	{
	LOG_FUNC

	// Delete defined properties
	TInt err;
	err = iProperty.Delete(KPropertyUidBluetoothCategory, KPropertyKeyBluetoothGetRegistryTableChange);
	FLOGIFERR(err,_L("CBTManServer::~CBTManServer() - iProperty.Delete Failure"));
	err = iProperty.Delete(KPropertyUidBluetoothCategory, KPropertyKeyBluetoothCorruptRegistryReset);
	FLOGIFERR(err,_L("CBTManServer::~CBTManServer() - iProperty.Delete Failure"));
	// Close the RProperty handle in case it is ever attached.
	iProperty.Close();

	delete iBURManager;
	delete iRegistry;
	delete iContainerIndex;
	}

CSession2* CBTManServer::NewSessionL(const TVersion &aVersion, const RMessage2& aMessage) const
	{
	LOG_FUNC
	TVersion v(KBTManServerMajorVersionNumber,KBTManServerMinorVersionNumber,KBTManServerBuildVersionNumber);
	if (!User::QueryVersionSupported(v,aVersion))
		{
		User::Leave(KErrNotSupported);
		}
	// make new session
	return new(ELeave) CBTManSession(*iRegistry, aMessage);
	}

void CBTManServer::BUROperationStarted()
/**
A backup or restore operation has started.
Cancel the shutdown timer if it was running
**/
	{
	LOG_FUNC
	__ASSERT_DEBUG(!iBUROperationStarted, PanicServer(EBTManBadState));

	iBUROperationStarted = ETrue;
	iShutdown.Cancel();
	}

void CBTManServer::BUROperationStopped()
/**
A backup or restore operation has finished.
Start the shutdown timer if there are no sessions running.
**/
	{
	LOG_FUNC
	__ASSERT_DEBUG(iBUROperationStarted, PanicServer(EBTManBadState));

	iBUROperationStarted = EFalse;
	TryToStartShutdownTimer();
	}

void CBTManServer::AddSession()
/**
A new session is being created
Cancel the shutdown timer if it was running
**/
	{
	LOG_FUNC
	++iSessionCount;
	if (iSessionCount > iMaxSessionCount)
		{
		iMaxSessionCount = iSessionCount;
		}
	iShutdown.Cancel();
	}

void CBTManServer::DropSession()
/**
A session is being destroyed
Start the shutdown timer if it is the last session.
**/
	{
	LOG_FUNC
	__ASSERT_DEBUG(iSessionCount > 0, PanicServer(EBTManBadState));
	
	--iSessionCount;
	TryToStartShutdownTimer();
	}

void CBTManServer::TryToStartShutdownTimer()
/**
Starts the shutdown timer if the server is able to shutdown
**/
	{
	LOG_FUNC
	
	if (iSessionCount == 0 && !iBUROperationStarted)
		{
		iShutdown.Start();
		}
	}

void CBTManServer::Publish(TUint aKey, TInt aValue)
	{
	LOG_FUNC
	TInt err;
	err = iProperty.Set(KPropertyUidBluetoothCategory,
					    aKey,
						aValue);
	FLOGIFERR(err,_L("CBTManServer::Publish() - iProperty.Set Failure"));
	}

void CBTManServer::NotifyViewChange(CBTManSubSession& aSubSessionViewOwner, const TDesC& aViewDescriptor)
	{
	LOG_FUNC
	// For views owned by subsessions.
	
	// Go through all sessions - they'll dispatch so all the subessions apart from the one calling here
	iSessionIter.SetToFirst();

	while (iSessionIter)
		{
		CBTManSession* s = static_cast<CBTManSession*>(iSessionIter++);
		s->SubSessionHasOverlappingView(aSubSessionViewOwner, aViewDescriptor);
		}
	}

void CBTManServer::NotifyViewChange(const TDesC& aViewDescriptor)
	{
	LOG_FUNC
	// For views not owned by subsessions (e.g. views owned by Backup/Restore classes)

	// Go through all sessions - they'll dispatch so all the subessions apart from the one calling here
	iSessionIter.SetToFirst();

	while (iSessionIter)
		{
		CBTManSession* s = static_cast<CBTManSession*>(iSessionIter++);
		s->SubSessionHasOverlappingView(aViewDescriptor);
		}
	}
	
void PanicClient(const RMessage2& aMessage,TInt aPanic, CBTManSession* aSession)
/**
RMessage2::Panic() also completes the message. This is:
(a) important for efficient cleanup within the kernel
(b) a problem if the message is completed a second time
**/
	{
	LOG_STATIC_FUNC
	LOG1(_L("PanicClient: Reason = %d"), aPanic);
	//remove the message from the message table so we don't complete it again
	//The message may not yet be in the table, so check for NULL before deleting it
	//This code could be more efficient, but hopefully it won't be called enough to make
	//it worthwhile improving it ;-)
	
	__DEBUGGER();

	/*** ORDER MATTERS! ***/
	//First find the message in the session's stored messages
	//This must be done BEFORE panicking the client because
	//panicking the client resets the RMessage2 handle value to
	//zero and so almost always renders the FindMessage method useless!
	CBTManMessage* m = aSession->FindMessage(aMessage);
	
	//Now panic the client - this is safer done before deleting 
	//the message from the message array, just in case the RMessage2
	//object is obtained from that array and not directly from the client
	aMessage.Panic(KBTManPanic,aPanic);
	
	//Now delete the message from the session's queue. This is vital
	//if we are not to attempt to complete on a dead message
	if (m)
		{
		//aMessage will not be usable after the next line if it is obtained from the message array
		//instead of the one directly supplied by the client
		aSession->DeleteMessage(m);
		}
	}

void PanicServer(TInt aPanic)
/**
Panic our own thread
**/
	{
	LOG_STATIC_FUNC
	LOG1(_L("PanicServer: Reason = %d"), aPanic);
	User::Panic(KBTManPanic, aPanic);
	}


static void RunServerL()
/**
Perform all server initialisation, in particular creation of the
scheduler and server and then run the scheduler
**/
	{
	LOG_STATIC_FUNC
	// create and install the active scheduler we need
	CActiveScheduler* scheduler=new(ELeave) CActiveScheduler;
	CleanupStack::PushL(scheduler);
	CActiveScheduler::Install(scheduler);
	//
	// create the server (leave it on the cleanup stack)
	CServer2* server = CBTManServer::NewLC();
	//
	
	#ifdef __BTMANSERVER_NO_PROCESSES__
	RThread::Rendezvous(KErrNone);
	#else
	// naming the server thread after the server helps to debug panics
	// ignore error - we tried the best we could
	User::RenameThread(KBTManServerName); 
	
	RProcess::Rendezvous(KErrNone);
	#endif
	
	//
	// Ready to run
	CActiveScheduler::Start();
	//
	// Cleanup the server and scheduler
	CleanupStack::PopAndDestroy(server);
	CleanupStack::PopAndDestroy(scheduler);
	}

TInt E32Main()
/**
Main entry-point for the server process
**/
	{
	CONNECT_LOGGER
	LOG_STATIC_FUNC
	LOG(_L("BTManServer RunServer"));

	__UHEAP_MARK;
#ifdef TEST_OOM //define TEST_OOM in preprocessor definitions in project settings
	__UHEAP_SETFAIL(RHeap::ERandom, 100);
#endif
	CTrapCleanup* cleanup=CTrapCleanup::New();
	TInt r=KErrNoMemory;
	if (cleanup)
		{
		TRAP(r,RunServerL());
		delete cleanup;
		}
	//
	__UHEAP_MARKEND;
	CLOSE_LOGGER
	return r;
	}


//=====================================================================
//CBTManMessage
//=====================================================================

// This framework is retained as it allows instrinsically asynchronous actions to be
// looked after easily.  At present such actions are limited to change notifications
// pretty much everything else is handled synchronously once the Server has had
// its ServiceL called.

CBTManMessage* CBTManMessage::NewL(const RMessage2& aMessage)
	{
	LOG_STATIC_FUNC
	CBTManMessage* m = new (ELeave) CBTManMessage(aMessage);
	CleanupStack::PushL(m);
	m->ConstructL();
	CleanupStack::Pop(m);
	return m;
	}

CBTManMessage::CBTManMessage(const RMessage2& aMessage)
: iMessage(aMessage)
	{
	LOG_FUNC
	}

void CBTManMessage::ConstructL()
	{
	LOG_FUNC
	if (iMessage.Ptr2())
		{
		TPckgBuf<TBTManClientServerMessage> msgBuf;
		iMessage.ReadL(KBTManClientServerProtocolSlot, msgBuf);
		
		iCancelPtr = msgBuf().iClientStatusToCancel;
		}
	}


CBTManMessage::~CBTManMessage()
	{
	LOG_FUNC
	//ensure we delete the helper if there is one
	if (iHelper)
		{
		iHelper->Delete();
		}
	}

void CBTManMessage::SetHelper(MBTManHelper* aHelper)
/**
Assigns a helper to the message.
The helper will then be told to delete itself when the message is completed.
This is especially important when cancelling or under leave conditions.
**/
	{
	LOG_FUNC
	__ASSERT_DEBUG(iHelper==NULL, PanicServer(EBTManBadHelper));
	__ASSERT_DEBUG((aHelper->Message()).Ptr2() == iCancelPtr, PanicServer(EBTManBadHelper));
	iHelper = aHelper;
	}

void CBTManMessage::RemoveHelper()
/**
Disassociates a helper with a message.
**/
	{
	LOG_FUNC
	iHelper = NULL;
	}

void CBTManMessage::Complete(TInt aReason)
/**
Completes the RMessage2 and deletes any associated helper.
**/
	{
	LOG_FUNC
	//complete the message
	iMessage.Complete(aReason);
	//delete the helper if there is one
	if (iHelper)
		iHelper->Delete();
	}

const RMessage2& CBTManMessage::Message()
	{
	LOG_FUNC
	return iMessage;
	}

const TAny* CBTManMessage::CancelPtr()
/**
The CancelPtr is the address of the TRequestStatus of the client-side active object
dealing with the request.  When a request is cancelled, it is located server-side using
this address.
**/
	{
	LOG_FUNC
	return iCancelPtr;
	}


MBTManHelper* CBTManMessage::Helper()
	{
	LOG_FUNC
	return iHelper;
	}

TBool CBTManMessage::operator==(CBTManMessage& aMessage) const
	{
	LOG_FUNC
	if (iMessage == aMessage.Message())
		return ETrue;
	return EFalse;
	}

//=====================================================================
//CBTManSession
//=====================================================================

CBTManServer& CBTManSession::Server()
	{
	LOG_FUNC
	return *static_cast<CBTManServer*>(const_cast<CServer2*>(CSession2::Server()));
	}

CBTManSession::CBTManSession(CBTRegistry& aRegistry, const RMessage2& aMessage)
: iRegistry(aRegistry), iCurrentMessage(aMessage)
	{
	LOG_FUNC
	}

void CBTManSession::CreateL()
/**
2nd phase construct for sessions - called by the CServer2 framework
**/
	{
	LOG_FUNC
	//CSession2::CreateL();	// private and does nowt so removed
	//Add session to server first. If anything leaves, it will be removed by the destructor
	Server().AddSession();
	ConstructL();
	}

void CBTManSession::ConstructL()
	{
	LOG_FUNC
	//Create new object index
	iSubSessions = CObjectIx::NewL();
	iContainer = Server().NewContainerL();

	iMessageArray = new(ELeave) CArrayPtrFlat<CBTManMessage>(KMessageArrayGranularity);
	}

CBTManSession::~CBTManSession()
	{
	LOG_FUNC
	
	delete iSubSessions;
	Server().DeleteContainer(iContainer);
	Server().DropSession();
	
	if (iMessageArray)
		{
		__ASSERT_DEBUG(iMessageArray->Count()== 0, PanicServer(EBTManOutstandingMessagesOnClosedSession));
	
		CompleteOutstandingMessages();
		iMessageArray->ResetAndDestroy();
		}
	delete iMessageArray;
	}

void CBTManSession::CompleteOutstandingMessages()
/**
Completes any messages left in the CBTManMessage array.
**/
	{
	LOG_FUNC
	CBTManMessage* ptr;
	TInt count = iMessageArray->Count();
	LOG1(_L("CBTManSession::CompleteOutstandingMessages(): %d"), count);
	for (TInt i=(count-1); i>=0; i--)
		{
		ptr = iMessageArray->At(i);
		ptr->Complete(KErrDied);
		iMessageArray->Delete(i);
		delete ptr;
		ptr = NULL;
		}
	}


CBTManSubSession* CBTManSession::SubSessionFromHandle(TInt aHandle)
/**
Returns a subsession given a handle

	@param aHandle the handle given by the client
	@param aMessage	only used if we need to panic client's having used a dodgy handle
**/
	{
	LOG_FUNC
	CBTManSubSession* p = static_cast<CBTManSubSession*>(iSubSessions->At(aHandle));
	return p;
	}

TInt CBTManSession::HandleError(TInt aError, const RMessage2 &aMessage)
/**
Handle an error from ServiceL()
A bad descriptor error implies a badly programmed client, so panic it;
otherwise report the error to the client
**/
	{
	LOG_FUNC
	LOG1(_L("CBTManSession::HandleError(): %d"), aError);
	if (aError==KErrBadDescriptor || aError==KErrBadHandle)
		{
#ifdef _DEBUG
		__DEBUGGER(); //******** SERVER STOP ******************
#endif
		TInt convErr = aError==KErrBadDescriptor?EBTManBadDescriptor:EBTManBadSubSessionHandle;
		PanicClient(aMessage, convErr, this);
		}
	else
		{
		CompleteMessage(aMessage, aError);
		}
	Server().ReStart();
	return KErrNone;	// handled the error fully
	}


void CBTManSession::ServiceL(const RMessage2 &aMessage)
/**
Handle a client request.
Leaving is handled by HandleError() which reports the error code
to the client
**/
	{
	LOG_FUNC		
	//We don't need to create a message object if message is handled by session
	//Simply dispatch and complete the message
	TBool handled = ETrue;

	TRAPD(err, handled = DispatchSessMessageL(aMessage));
	if (handled || (err!=KErrNone))
		{
		if (!iClientPanic)
			{
			// Don't complete messages already completed by ClientPanic
			aMessage.Complete(err);
			}
		else 
			{
			//Client Panic Handled - Reset Flag.
			iClientPanic = EFalse;
			}
		return;
		}

	//not handled by session so must be for subsession
	//Create a message object to handle this
	//We need to trap this so a leave doesn't propogate to active scheduler
	err = KErrNone;
	TRAP(err, CreateBTManMessageL(aMessage));
	if (err)
		{
		aMessage.Complete(err);
		return;
		}

	//Trap dispatchSubSessMessage so we can handle error here
	err=KErrNone;
	TRAP(err, DispatchSubSessMessageL(aMessage));
	//if we have an error, try to handle it with handleerror. If this fails, leave
	//and let the active scheduler sort it out.
	if (err != KErrNone)
		{
		// tell subsessions to try to reduce footprint and rollback
		for (TInt i = 0; i<iSubSessions->Count(); i++)
			{
			//check this element is an actual object rather than an empty memory 
			//cell on the free list
			if((*iSubSessions)[i])
				{
				static_cast<CBTManSubSession*>((*iSubSessions)[i])->Cleanup(err);
				}
			}
		// Tidy up of logic and construct
		if (err != KErrNone)
			{
			User::LeaveIfError(HandleError(err, aMessage));
			return;
			}
		}
	}

void CBTManSession::CreateBTManMessageL(const RMessage2& aMessage)
/**
Creates a CBTManMessage and appends it to iMessageArray.
These are used for the BTMan server to conduct asynchronous activities itself

Many activities that the BTMan Server does at present are synchronously handled; but
for conformity all the subsessions use this framework so that any asynchronous
tasks can be added later

	@param	aMessage	The original client message
**/
	{
	LOG_FUNC
	CBTManMessage* m = CBTManMessage::NewL(aMessage);
	CleanupStack::PushL(m);
	iMessageArray->AppendL(m);
	CleanupStack::Pop();
	}


TBool CBTManSession::DispatchSessMessageL(const RMessage2 &aMessage)
/**
Handles the request - it may not actually be for the session

@param aMessage    The current message being handled.
@return			   ETrue: The message was meant for the session.
@return            EFalse: The message was meant for a subsession.
**/
	{
	LOG_FUNC
	switch (aMessage.Function())
		{
	case EBTManCreateRegistrySubSession:
		NewSubSessionL(ERegistry, aMessage);
		return ETrue;
	case EBTManCreateLocalDeviceSubSession:
		NewSubSessionL(ELocalDevice, aMessage);
		return ETrue;
	case EBTManCreateCommPortSettingsSubSession:
		NewSubSessionL(ECommPortSettings, aMessage);
		return ETrue;
	case EBTManCreateHostResolverSubSession:
		NewSubSessionL(EHostResolver, aMessage);
		return ETrue;
	case EBTManCancelRequest:
		CancelRequest(aMessage);
		return ETrue;
	case EBTManCloseSubSession:
		CloseSubSession(aMessage);
		return ETrue;
	case EBTManSetHeapFailure:
#ifdef _DEBUG
		User::__DbgSetAllocFail(RHeap::EUser,RHeap::TAllocFail(aMessage.Int0()),aMessage.Int1());
#endif
		return ETrue;
	case EBTManSubSessionCount:
		{
#ifdef _DEBUG
		TPckgBuf<TInt> pckg(iSubSessions->Count());
		aMessage.WriteL(0, pckg);
#endif
		return ETrue;
		}
	default:
		//not handled here so must be for subsession
		return EFalse;
		}
	}

void CBTManSession::DispatchSubSessMessageL(const RMessage2& aMessage)
/**
Handles subsession requests.

	@param aMessage    The current message being handled.
**/
	{
	LOG_FUNC
	CBTManSubSession& ss = *SubSessionFromHandle(aMessage.Int3());
	if (&ss == NULL)
		{
		User::Leave(KErrBadHandle);
		}

	// Get the status of the client request (sent on all msgs in slot2)
	switch (aMessage.Function())
		{
	case EBTManRegistrySearch:
		{
		TBTRegistrySearchPckgBuf pckg;
		aMessage.ReadL(0, pckg);
		static_cast<CBTRegistrySubSession*>(&ss)->OpenViewL(pckg(), aMessage);
		break;
		}
	case EBTRegistryAddDevice:
		{
		// create a new device subsession and add the device - may change API to require
		// previously opened subsession??
		// need to internalise a contiguated buffer
	
		TInt len = aMessage.GetDesLengthL(0);
		HBufC8* buffer = HBufC8::NewLC(len);
		TPtr8 ptr(buffer->Des());

		aMessage.ReadL(0, ptr);
		RDesReadStream stream;
		stream.Open(ptr);
		CleanupClosePushL(stream);

		CBTDevice* addDetails = CBTDevice::NewLC();
		addDetails->InternalizeL(stream);

		static_cast<CBTRegistrySubSession*>(&ss)->AddDeviceL(*addDetails, aMessage);

		CleanupStack::PopAndDestroy(3); //addDetails, stream, buffer
		break;
		}
	case EBTRegistryModifyNamelessDevice:
		{
		LOG(_L("CBTManSession -> ModifyNamelessDevice"));
		TBTNamelessDevicePckgBuf pckg;
		aMessage.ReadL(0, pckg);
		static_cast<CBTRegistrySubSession*>(&ss)->ModifyL(pckg(), aMessage);
		break;
		}
	case EBTRegistryGetNamelessDevice:
		{
		TBTNamelessDevicePckgBuf pckg;
		aMessage.ReadL(0, pckg);
		static_cast<CBTRegistrySubSession*>(&ss)->GetDeviceL(pckg(), aMessage);
		break;
		}
	case EBTManExtractRegistryDataIntoServer:
		{
		static_cast<CBTRegistrySubSession*>(&ss)->PreLoadL(aMessage);
		break;
		}

	case EBTManRetrieveRegistryData:
		{
		static_cast<CBTRegistrySubSession*>(&ss)->RetrieveL(aMessage);
		break;
		}

	case EBTRegistryDeleteLinkKey:
		{
		TBTDevAddrPckgBuf pckg;
		aMessage.ReadL(0, pckg);
		static_cast<CBTRegistrySubSession*>(&ss)->UnpairL(pckg(), aMessage);
		break;
		}

	case EBTRegistryDeleteDevices:
		{
		//nothing to read
		static_cast<CBTRegistrySubSession*>(&ss)->DeleteViewL(aMessage);
		break;
		}

	case EBTRegistryModifyBluetoothName:
	case EBTRegistryModifyFriendlyName:
		{
		// read the address - this could move into the handler
		TBTDevAddrPckgBuf addr;
		aMessage.ReadL(0, addr);
		static_cast<CBTRegistrySubSession*>(&ss)->ModifyNameL(addr(), aMessage);
		break;
		}

	case EBTRegistryGetLocalDevice:
		{
		//nothing to read
		static_cast<CBTLocalDeviceSubSession*>(&ss)->GetL(aMessage);
		break;
		}

	case EBTRegistryUpdateLocalDevice:
		{
		TPckgBuf<TBTLocalDevice> pckg;
		aMessage.ReadL(0, pckg);
		static_cast<CBTLocalDeviceSubSession*>(&ss)->UpdateL(pckg(), aMessage);
		break;
		}

	case EBTRegistryGetCommPortSettings:
		{
		TPckgBuf<TBTCommPortSettings> pckg;
		aMessage.ReadL(0, pckg);
		static_cast<CBTCommPortSettingsSubSession*>(&ss)->GetL(pckg(), aMessage);
		break;
		}

	case EBTRegistryUpdateCommPortSettings:
		{
		TPckgBuf<TBTCommPortSettings> pckg;
		aMessage.ReadL(0, pckg);
		static_cast<CBTCommPortSettingsSubSession*>(&ss)->UpdateL(pckg(), aMessage);
		break;
		}

	case EBTRegistryDeleteCommPortSettings:
		{
		TPckgBuf<TBTCommPortSettings> pckg;
		aMessage.ReadL(0, pckg);
		static_cast<CBTCommPortSettingsSubSession*>(&ss)->DeleteL(pckg(), aMessage);
		break;
		}

	case EBTRegistryUnpairView:
		{
		static_cast<CBTRegistrySubSession*>(&ss)->UnpairViewL(aMessage);
		break;
		}
		
	case EBTRegistryCloseView:
		{
		static_cast<CBTRegistrySubSession*>(&ss)->CloseView(aMessage);
		break;
		}	
	
	case EBTRegistryNotifyViewChange:
		{
		ss.SetViewChangeNotificationMessage(aMessage);
		break;
		}
		
	case EBTHostResolverDeviceRequest:
	case EBTHostResolverDeviceModifyDevice:
	default:
		PanicClient(aMessage, EBTManBadRequest, this);
		return;
		}
	}

void CBTManSession::NewSubSessionL(TSubSessionType aType, const RMessage2 &aMessage)
/**
Create a subsession of type aType and add it to the object index.
**/
	{
	LOG_FUNC
	//Make new counter object
	CBTManSubSession* ss = NULL;
	switch (aType)
		{
	case ERegistry:
		ss = CBTRegistrySubSession::NewL(*this, iRegistry);
		break;
	case ELocalDevice:
		ss = CBTLocalDeviceSubSession::NewL(*this, iRegistry);
		break;
	case ECommPortSettings:
		ss = CBTCommPortSettingsSubSession::NewL(*this, iRegistry);
		break;
	case EHostResolver:
	default:
		PanicServer(EBTManBadSubSessionType);
		}

	//add to container to generate unique id
	//If anything goes wrong, make sure we close the subsession.
	CleanupClosePushL(*ss);

	iContainer->AddL(ss);

	//add to object index - this returns a unique handle
	// From inspection of CObjectIx::AddL(), it will can only leave BEFORE ss is added to the index.
	// Therefore, we must close the subsession and leave if anything goes wrong, 
	// and NOT call CObjectIx::Remove().
	TInt handle = 0;
	handle = iSubSessions->AddL(ss);

	//ss now has an owner so we can pop it off the cleanupstack
	CleanupStack::Pop();	//ss

	//Write handle to client
	//If anything goes wrong, call CObjectIx::Remove(). This will close the subsession for us.
	TPckg<TInt> pckg(handle);
	TRAPD(err, aMessage.WriteL(3, pckg));
	if (err!=KErrNone)
		{
		iSubSessions->Remove(handle);
		User::Leave(err);
		}
	//increase resource count
	iResourceCount++;
	}

void CBTManSession::DeleteSubsession(TInt aHandle, const RMessage2 &aMessage)
	{
	LOG_FUNC
	//panic client if bad handle
	CBTManSubSession* ss = (CBTManSubSession*)iSubSessions->At(aHandle);
	if (ss == NULL)
		{
		PanicClient(aMessage, EBTManBadSubSessionRemove, this);
		//Flag that Client has been panic'd 
		//so that the message is not completed twice
		iClientPanic = ETrue;
		}
	else
		{
		iSubSessions->Remove(aHandle);
		//decrement resource count
		iResourceCount--;
		}
	}

void CBTManSession::CloseSubSession(const RMessage2 &aMessage)
	{
	LOG_FUNC
	// handle for subsession is in slot3
	DeleteSubsession(aMessage.Int3(), aMessage);
	}

void CBTManSession::CompleteMessage(const RMessage2& aMessage, TInt aReason)
/**
Finds a message based on aMessage and completes it.
**/
	{
	LOG_FUNC
	CBTManMessage* m = FindMessage(aMessage);
	__ASSERT_DEBUG(m!=NULL, PanicServer(EBTManBadBTManMessage));
	DoCompleteMessage(*m, aReason);
	}

void CBTManSession::CompleteMessage(MBTManHelper* aHelper, TInt aReason)
/**
Finds a message based on aHelper and completes it.
**/
	{
	LOG_FUNC
	CBTManMessage* m = FindMessage(aHelper);
	__ASSERT_DEBUG(m!=NULL, PanicServer(EBTManBadBTManMessage));
	DoCompleteMessage(*m, aReason);
	}

void CBTManSession::DoCompleteMessage(CBTManMessage& aMessage, TInt aReason)
/**
	Actually does the completion
**/
	{
	LOG_FUNC
	if (aMessage.Message().Ptr2())
		{
		// notify clientAPI that we've serviced it - not needed if the client has
		// not asked us to (typically for synchronous calls)
		TBTManClientServerMessage clientMsg;
		TPckg<TBTManClientServerMessage> clientMsgBuf(clientMsg);
		TRAPD(err_read, aMessage.Message().ReadL(KBTManClientServerProtocolSlot, clientMsgBuf));
		clientMsg.iClientBusy=EFalse;
		TRAPD(err_write, aMessage.Message().WriteL(KBTManClientServerProtocolSlot, clientMsgBuf));
		
		if(err_read != KErrNone || err_write != KErrNone)
			{
			PanicClient(aMessage.Message(),EBTManBadBTManMessage, this);
			return;
			}
		}
	// now tell the kernel
	aMessage.Complete(aReason);
	DeleteMessage(&aMessage);
	}

CBTManMessage* CBTManSession::FindMessage(const RMessage2& aMessage)
/**
Searches the array of CBTManMessages for the one dealing with aMessage.
**/
	{
	LOG_FUNC
	CBTManMessage* ptr;
	for (TInt i=0; i<iMessageArray->Count(); i++)
		{
		ptr = iMessageArray->At(i);
		if(ptr->Message() == aMessage)
			{
			return ptr;
			}
		}
	return NULL;
	}

CBTManMessage* CBTManSession::FindMessage(MBTManHelper* aHelper)
/**
Searches the array of CBTManMessages for the one containing a pointer to aHelper.
**/
	{
	LOG_FUNC
	CBTManMessage* ptr;
	for (TInt i=0; i<iMessageArray->Count(); i++)
		{
		ptr = iMessageArray->At(i);
		if(ptr->Helper() == aHelper)
			{
			return ptr;
			}
		}
	return NULL;
	}

void CBTManSession::DeleteMessage(CBTManMessage* aMessage)
/**
Find the CBTManMessage in the message array and delete it.
**/
	{
	LOG_FUNC
	CBTManMessage* ptr;
	TInt count = iMessageArray->Count();
	for (TInt i=(count-1); i>=0; i--)
		{
		ptr = iMessageArray->At(i);
		if(ptr == aMessage)
			{
			//Delete the message first before removing from the array since a helper associated
			//with the message will try to find the message by parsing the array as part of the
			//destruction the message.
			delete ptr;
			iMessageArray->Delete(i);
			ptr = NULL;
			break;
			}
		}
	//compress the array if the count is less than the length - granularity AND if the count != 0
	if (iMessageArray->Count())
		{
		if (iMessageArray->Length() - iMessageArray->Count() >= KMessageArrayGranularity)
			{
			iMessageArray->Compress();
			}
		}
	}

void CBTManSession::CancelRequest(const RMessage2& aMessage)
/**
Cancels an asyncronous request.
 We need to find the asynchronous message we are cancelling
 - we can do this by comparing the TRequestStatus held
 in Ptr2() of the message.  This is ok since ALL asynchronous
 requests to this server have the TRequestStatus passed over
 in Ptr2() and only these may be cancelled. The TRequestStatus
 is sent over in ptr0() of the cancel request so we don't confuse
 the cancel message with the one we are trying to cancel.
**/
	{
	LOG_FUNC
	CBTManMessage* m;
	TInt count = iMessageArray->Count();
	for (TInt i=(count-1); i>=0; i--)
		{
		m = iMessageArray->At(i);
		if (m->CancelPtr() == aMessage.Ptr0())
			{
			//Complete the message
			m->Complete(KErrCancel);
			//delete the message from the array
			iMessageArray->Delete(i);
			delete m;
			m = NULL;
			break;
			}
		}
	//aMessage will be completed by CBTManSession::ServiceL()
	}

TBool CBTManSession::SubSessionHasOverlappingView(CBTManSubSession& aSubSessionViewOwner, const TDesC& aViewDescriptor)
	{
	LOG_FUNC
	// Iterate over all subsessions apart from the view owner
	TBool overlapFound = EFalse;
	for (TInt i = 0; i < iContainer->Count(); i++)
		{
		CBTManSubSession* ss = static_cast<CBTManSubSession*>((*iContainer)[i]);
		if (&aSubSessionViewOwner != ss)
			{
			if (ss->IsOverlappingView(aViewDescriptor))
				{
				// Overlaps - subsession will have completed the Notify message
				// With bool return, we can test if indeed it did.
				overlapFound = ETrue;
				}
			}
		}

	return overlapFound;
	}

TBool CBTManSession::SubSessionHasOverlappingView(const TDesC& aViewDescriptor)
	{
	LOG_FUNC
	// Iterate over all subsessions (view owner is not a subsession)
	TBool overlapFound = EFalse;
	for (TInt i = 0; i < iContainer->Count(); i++)
		{
		CBTManSubSession* ss = static_cast<CBTManSubSession*>((*iContainer)[i]);
		if (ss->IsOverlappingView(aViewDescriptor))
			{
			// Overlaps - subsession will have completed the Notify message
			// With bool return, we can test if indeed it did.
			overlapFound = ETrue;
			}
		}

	return overlapFound;
	}


//=====================================================================
//	CBTManSubSession
//=====================================================================

CBTManSubSession::CBTManSubSession(CBTManSession& aSession, CBTRegistry& aRegistry) :
	iSession(aSession), iRegistry(aRegistry)
	{
	LOG_FUNC
	}

void CBTManSubSession::NotifyChange(TUint aTableChanged)
	{
	LOG_FUNC
	iSession.Server().Publish(KPropertyKeyBluetoothGetRegistryTableChange,
							  aTableChanged);
	}

void CBTManSubSession::NotifyChange(TUint aTableChanged, CBTManSubSession& aSubSessionViewOwner, const TDesC& aViewDescriptor)
	{
	LOG_FUNC
	NotifyChange(aTableChanged);
	iSession.Server().NotifyViewChange(aSubSessionViewOwner, aViewDescriptor);
	}	

/*virtual*/ TBool CBTManSubSession::IsOverlappingView(const TDesC& /*aViewDescriptor*/)
	{
	LOG_FUNC
	// By default, not supported - so no overlapping view.
	return EFalse;
	}

/*virtual*/ void CBTManSubSession::SetViewChangeNotificationMessage(const RMessage2& aMessage)
	{
	LOG_FUNC
	// By default, not supported
	iSession.CompleteMessage(aMessage, KErrNotSupported);
	}

#ifdef __BTMANSERVER_NO_PROCESSES__

// The server binary is an "EPOCEXE" target type
// Thus the server parameter passing and startup code for WINS and EPOC are
// significantly different.
//
// In EKA1 WINS, the EPOCEXE target is a DLL with an entry point called WinsMain,
// taking no parameters and returning TInt. This is not really valid as a thread
// function which takes a TAny* parameter which we need.
//
// So the DLL entry-point WinsMain() is used to return a TInt representing the
// real thread function within the DLL. This is good as long as
// sizeof(TInt)>=sizeof(TThreadFunction).
//

static TInt ThreadFunction(TAny*)
//
// WINS thread entry-point function.
//
	{
	LOG_FUNC
	return E32Main();
	}

IMPORT_C TInt WinsMain();
EXPORT_C TInt WinsMain()
//
// WINS DLL entry-point. Just return the real thread function 
// cast to TInt
//
	{
	LOG_FUNC
	return reinterpret_cast<TInt>(&ThreadFunction);
	}

TInt E32Dll(TDllReason)
	{
	LOG_FUNC
	return KErrNone;
	}

#endif
