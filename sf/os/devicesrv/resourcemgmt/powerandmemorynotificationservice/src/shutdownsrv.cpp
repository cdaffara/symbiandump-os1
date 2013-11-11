// Copyright (c) 1997-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef SYMBIAN_ENABLE_SPLIT_HEADERS
#include "shutdownsrv.h"
#else //SYMBIAN_ENABLE_SPLIT_HEADERS
#include "shutdownsess.h"
#endif //SYMBIAN_ENABLE_SPLIT_HEADERS
#include "savenotf.h"
#include "savepriv.h"
#include "shutdowntimer.h"
#include <hal.h>
#include <f32file.h>
#include <bautils.h>

#ifdef SYMBIAN_SSM_GRACEFUL_SHUTDOWN
#include <s32mem.h>
#include <ssm/ssmpatchableconstants.h>
#endif

//The function used to start the profiler, while profiling the shutdown.
#ifdef __PROFILE_SHUTDOWN
_LIT(KProfilerCmd,"start -noui");
void StartProfiler()
	{
	RProcess p;
	TInt r=p.Create(KProfilerName,KProfilerCmd);
	if (r==KErrNone)
		{
		p.Resume();
		p.Close();
		}
	}
#endif

//The function used to unload the profiler, while profiling the shutdown.
#ifdef __PROFILE_SHUTDOWN
void UnloadProfiler()
	{
	TFullName name;
	TFindServer findSvr(KProfilerName);
	Profiler::Unload();
	while(findSvr.Next(name) == KErrNone)
		{
		User::After(1000000);
		findSvr.Find(KProfilerName);
		}
	}
#endif

//
// class CServShutdownServer
//

/**
Standard phase-one factory method for creating CServShutdownServer instances.
@return A newly-constructed shutdown server object.
@leave Some system-wide error codes including KErrNoMemory.
*/
EXPORT_C CServShutdownServer* CServShutdownServer::NewL()
	{ // static
	CServShutdownServer* self=new(ELeave) CServShutdownServer(CActive::EPriorityStandard);
	return self;
	}

/**
Destructor.
Releases CShutdownTimer object if it has been allocated.
*/
EXPORT_C CServShutdownServer::~CServShutdownServer()
	{
	delete iShutdownTimer;
	}

/**
This method iterates through all session instances and checks that all they have pending
requests (which means that all clients have completed their powerdown related processing
and re-registered itself back to the shutdown server).. If that's true (all sessions have
pending requests) and iPowerOff flag is nonzero, the method will call SwitchOff() - the
power will be switched off. 
@see CServShutdownServer::SwitchOff()
@see CServShutdownSession::HasPendingRequest()
*/
EXPORT_C void CServShutdownServer::HandlePowerNotifRequest(const RThread& /*aClient*/)
	{
	// if all sessions have a pending request then they've all saved their data and we can switch off
	TBool completed=ETrue;
	TDblQueIter<CSession2> iter(iSessionIter);
	iter.SetToFirst();

	for (CSession2* session=iter++; session!=NULL; session=iter++)
		{
		if (!static_cast<CServShutdownSession*>(session)->HasPendingRequest())
			{
			completed=EFalse;
			break;
			}
		}
	if (completed && iPowerOff)
		{
		SwitchOff();
		}
	}

/**
This method will call SwitchOff(), if there are no sessions, so - no clients.
If there are registered clients (sessions), the method will call CServShutdownSession::NotifySave()
for each of them.
If the timer object has been initialised, it calculates the shutdown timeout value 
and adds the timer to the active scheduler.
@param aSaveType The action, which will be given to the clients, when calling their 
                 MSaveObserver::SaveL() implementations.
@see CServShutdownServer::SwitchOff()
@see CServShutdownSession::NotifySave()
*/
EXPORT_C void CServShutdownServer::NotifySave(MSaveObserver::TSaveType aSaveType)
	{
	TDblQueIter<CSession2> iter(iSessionIter);
	iter.SetToFirst();
	CSession2* p=iter++;
	if (p==NULL)
		{
		if (iPowerOff)
			{
			SwitchOff();
			}
		}
	else
		{
		TInt numClients = 0;

		iter.SetToFirst();
	    for (CSession2* session=iter++; session!=NULL; session=iter++)
			{
			static_cast<CServShutdownSession*>(session)->NotifySave(aSaveType);
			numClients++;  
			}
			
		if(iShutdownTimer && !iShutdownTimer->IsActive())
			{
			iShutdownTimer->Start(numClients);
			}
		}
	}

/**
@return Non-zero If the powerdown sequence has been initiated.
*/
EXPORT_C TBool CServShutdownServer::IsPowerOff() const
	{
	return iPowerOff;
	}

/**
Cancels any power off request.
*/
EXPORT_C void CServShutdownServer::CancelPowerOff()
	{
	iPowerOff=EFalse;
	
	// Cancel the timer.
	if (iShutdownTimer)
		{
		iShutdownTimer->Cancel();
		}
	}

/**
@param aPriority The active object priority.
*/
EXPORT_C CServShutdownServer::CServShutdownServer(TInt aPriority):
	CServer2(aPriority), iShutdownTimer(0)
	{
    }

/**
Completes the server construction by adding the server to the active scheduler and initializing the
CShutdownTimer object if applicable.
@leave See CServer2::StartL() leave error codes.
@see CServer2::StartL()
@panic KErrNotSupported Incorrect patchable variables configuration.
*/
EXPORT_C void CServShutdownServer::ConstructL()
	{
	iShutdownTimer = CShutdownTimer::NewL(*this);
	#ifdef TEST_SHUTDOWN_SERVER 
	StartL(__TEST_SHUTDOWN_SERVER_NAME);
	#else
	StartL(__SHUTDOWN_SERVER_NAME);
	#endif
	}

/**
This method switches off the power using the Power API.
If there is a defined SYSLIBS_TEST macro, the method does nothing.
*/
void CServShutdownServer::DoSwitchOff()
	{
	//Finalize the drives before shutting down
	RFs fs;
	TInt r=fs.Connect(); 
	if (r==KErrNone) 
		{
		//Ignore the error code, as it is not a critical call
		r=fs.FinaliseDrives();
		fs.Close();
		}
#ifdef 	SYSLIBS_TEST 
		// Test mode, only prints debug message.
		RDebug::Printf("CServShutdownServer::SwitchOff() gets run in SYSLIBS_TEST mode.\n");
#else 	//SYSLIBS_TEST
	// restart or standby/shutdown using Power API 
	if (Power::EnableWakeupEvents(iPowerEvent) == KErrNone)
		{
		// Prepare to wake up if power event is standby
		TRequestStatus s;
		Power::RequestWakeupEventNotification(s);
		Power::PowerDown(); // if event is Restart, this function should never return
		User::WaitForRequest(s);
		}
#endif //SYSLIBS_TEST
	} 
void CServShutdownServer::SwitchOff()
	{
	// If the timer has applied, cancel any outstanding requests, 
	// no matter SwitchOff() has been triggered by the timer or the shutdown server.
	if (iShutdownTimer)
		{
		iShutdownTimer->Cancel();
		}

	if (iPowerOff)
		{
#ifdef __PROFILE_SHUTDOWN
		UnloadProfiler();
#endif //__PROFILE_SHUTDOWN
		}
	//SSM shutdown is used only when patchable constant KSsmGracefulShutdown is true 
#ifdef SYMBIAN_SSM_GRACEFUL_SHUTDOWN
	if (iPowerOff && !IsSsmGracefulShutdown())
#else
	if (iPowerOff)
#endif
		{
		//Shutdown using old shutdownsrv, not SSM
		DoSwitchOff();
		}
	iPowerOff = EFalse;
	}

/**
This method creates a new server side session object.
@param aVersion Shutdown server version number.
@return A pointer to the created session object.
@leave KErrNotSupported Unknown shutdown server version
@leave Some system-wide error codes including KErrNoMemory.
*/
CSession2* CServShutdownServer::NewSessionL(const TVersion& aVersion,const RMessage2& /*aMessage*/) const
	{
	TVersion v(KShutdownMajorVN,KShutdownMinorVN,KShutdownBuildVN);
	if (!User::QueryVersionSupported(v,aVersion))
		User::Leave(KErrNotSupported);

	CSession2* pSession = CServShutdownSession::NewL();
	return pSession;
	}

/**
This method has to be called, when the registered clients have to be notified that a 
particular action has to be done, such as MSaveObserver::ESaveData, MSaveObserver::ESaveAll, 
MSaveObserver::EReleaseRAM,...
If this is a beginning of a powerdown sequence, the method will store the locales and the HAL
properties.
If the requested action is not MSaveObserver::ESaveNone, the method will call 
CServShutdownServer::NotifySave().
@param aAction The type of the requested action
@param aPowerOff If it is non-zero, this is the beginning of a powerdown sequence.
@param aEvent The type of the powerdown event (power off or restart)
@leave KErrNotSupported Leaves if aEvent is invalid  
@see CServShutdownServer::NotifySave()
@see TPowerState
*/
EXPORT_C void CServShutdownServer::HandleShutdownEventL(MSaveObserver::TSaveType aAction,TBool aPowerOff, TPowerState aEvent)
	{
	if( aPowerOff )
		{
		__ASSERT_ALWAYS((aEvent>EPwActive)&&(aEvent<EPwLimit), User::Leave(KErrNotSupported));
#ifdef __PROFILE_SHUTDOWN
		StartProfiler();
#endif

#ifdef SYMBIAN_SSM_GRACEFUL_SHUTDOWN
		if (!IsSsmGracefulShutdown())
			{
			// Save the state of the HAL
			// The state of the locale should be saved before the HAL
			BaflUtils::PersistLocale();
			BaflUtils::PersistHAL();
			}
#else
		// Save the state of the HAL
		// The state of the locale should be saved before the HAL
		BaflUtils::PersistLocale();
		BaflUtils::PersistHAL();
#endif
		}

	iPowerOff=aPowerOff;
	iPowerEvent = aEvent;
	// add LAF setting for powering down screen (& keyboard??)
	if (aAction != MSaveObserver::ESaveNone)
		{
		NotifySave(aAction);
		}
	}

/**
This method returns an information about the shutdown status.
@param aPowerOff An output parameter, where iPowerOff value will be stored. 
                 It will be non-zero, if a powerdown sequence has been initiated.
@param aAllSessionsHavePendingRequest An output parameter. It will be non-zero, if
                 all clients has pending requests.
*/
EXPORT_C void CServShutdownServer::GetShutdownState(TBool& aPowerOff, TBool& aAllSessionsHavePendingRequest) const
	{
	aAllSessionsHavePendingRequest = AllSessionsHavePendingRequest();
	aPowerOff = iPowerOff;
	}

/**
This method creates an array of CArrayFix<TThreadId> type and appends to it the
thread id-s of the all registered clients.
The created CArrayFix<TThreadId> instance will be pushed on the cleanup stack.
@return A pointer to a CArrayFix<TThreadId> array with the client thread id-s.
@leave Some system-wide error codes including KErrNoMemory.
*/
EXPORT_C CArrayFix<TThreadId>* CServShutdownServer::ClientArrayLC()
	{
	CArrayFix<TThreadId>* clientArray=new(ELeave) CArrayFixFlat<TThreadId>(2);
	CleanupStack::PushL(clientArray);
	TDblQueIter<CSession2> iter(iSessionIter);
	iter.SetToFirst();
	TKeyArrayFix key(0,ECmpTInt);
    for (CSession2* session=iter++; session!=NULL; session=iter++)
		{
		TThreadId id=static_cast<CServShutdownSession*>(session)->ClientThreadId();
		TInt pos;
		if (clientArray->Find(id,key,pos)!=0)
			{
			clientArray->AppendL(id);
			}
		}
	return clientArray;
	
	
	}

#ifdef SYMBIAN_SSM_GRACEFUL_SHUTDOWN
/**
This method will write thread id-s of all clients that are registered for shutdown notification in to a streem.
@param aMessage consists of buffer, clientside array count and server side array count 
*/
void CServShutdownServer::ClientArrayL(const RMessage2& aMessage)
	{
	const TInt arrayItemCount = ClientArrayCount();
	//write all registered client array in a buffer only when client side 
	//array count is equal to server side array count
	if(arrayItemCount == aMessage.Int1())
		{
		const TInt sizeRequired = arrayItemCount * sizeof(TThreadId);
		CBufFlat* const buf=CBufFlat::NewL(sizeRequired);
		CleanupStack::PushL(buf);
		RBufWriteStream writeStream(*buf);
		CleanupClosePushL(writeStream); 
		TDblQueIter<CSession2> iter(iSessionIter);
		iter.SetToFirst();
    	for (CSession2* session=iter++; session!=NULL; session=iter++)
			{
			TThreadId id=static_cast<CServShutdownSession*>(session)->ClientThreadId();
			//Thread id (which is TUint64) is broken in to two TUint32 and written as RWriteStream doesnt 
			//support TUint64. Client API will recreate TUint64 value from these TUint32 values.
			if(id.Id())
				{
				//RWriteStream there is no API to writes a TUint64 value as a 64 bit value to stream
				writeStream.WriteUint32L(I64HIGH(id.Id()));
				writeStream.WriteUint32L(I64LOW(id.Id()));
				}
			}
		writeStream.CommitL();
		aMessage.WriteL(0, buf->Ptr(0));
		CleanupStack::PopAndDestroy(2,buf);	//writeStream, buf 
		}
	else
		{
		aMessage.Write(2, TPckg<TInt>(arrayItemCount));
		}	
	
	}
		
/**
This method will return the number of client that are registered for Shutdown notification with ShutDown server.
@return Number of registered clients.
*/
TInt CServShutdownServer::ClientArrayCount()
	{
	TDblQueIter<CSession2> iter(iSessionIter);
	iter.SetToFirst();
	TInt count =0;
    for (CSession2* session=iter++; session!=NULL; session=iter++)
		{
		TThreadId id=static_cast<CServShutdownSession*>(session)->ClientThreadId();
		//Clients which are not registered for notification will not have RMessage in the session and hence 
		//thread id will be NULL(e.g.CLafShutdownEventObserverAdaptor). Avoid such clients. 		
		if(id.Id())
			{
			++count;
			}
		}
	return count;
	}
#endif	//SYMBIAN_SSM_GRACEFUL_SHUTDOWN	
/**
@return Non-zero, if all registered clients have pending requests.
*/
TBool CServShutdownServer::AllSessionsHavePendingRequest() const
	{
	TBool ret=ETrue;
	TDblQueIter<CSession2> iter(iSessionIter);
	iter.SetToFirst();
	for (CSession2* session=iter++; session!=NULL; session=iter++)
		{
		const CServShutdownSession* mySession=static_cast<CServShutdownSession*>(session);
		if (!mySession->HasPendingRequest())
			{
			ret=EFalse;
			break;
			}
		}
	return ret;
	}


/**
@param aId Client's thread id.
@return Non-zero if the client with this thread id has no pending request.
*/
EXPORT_C TBool CServShutdownServer::IsClientHung(TThreadId aId) const
	{
	TBool ret=EFalse;
	TDblQueIter<CSession2> iter(iSessionIter);
	iter.SetToFirst();
	for (CSession2* session=iter++; session!=NULL; session=iter++)
		{
		const CServShutdownSession* mySession=static_cast<CServShutdownSession*>(session);

		if (mySession->ClientThreadId()==aId)
			{
			if (!mySession->HasPendingRequest())
				{
				ret=ETrue;
				break;
				}
			}
		}
	return ret;
	}

//
// class CServShutdownSession
//

/**
*/
EXPORT_C CServShutdownSession::CServShutdownSession()
	: CSession2(),
	iCurrentEvent(-1), iOutstandingEvent(-1)
	{}

/**
*/
EXPORT_C CServShutdownSession::~CServShutdownSession()
	{
	}

/**
Standard phase-one factor method for creating CServShutdownSession instances.
@return A pointer to the created CServShutdownSession instance.
@leave KErrNoMemory Not enough memory to complete the operation.
*/
CServShutdownSession* CServShutdownSession::NewL()
	{
	return new (ELeave) CServShutdownSession;
	}

/**
@return Non-zero, if the client has a pending request.
*/
TBool CServShutdownSession::HasPendingRequest() const
	{
	return !iPtr.IsNull();
	}

/**
This method will complete the pending asychronous client request, effectivelly notifying it
about the action, which the client has to do.
@param aSaveType The type of the requested save action.
*/
void CServShutdownSession::NotifySave(MSaveObserver::TSaveType aSaveType)
	{
	const TInt saveType=(TInt)aSaveType;
	if (HasPendingRequest())
		{
		iPtr.Complete(saveType);
		iCurrentEvent=saveType;
		}
	else
		{
		if (iCurrentEvent==(TInt)MSaveObserver::ESaveAll || 
			(iCurrentEvent==(TInt)MSaveObserver::ESaveQuick && 
			 aSaveType==MSaveObserver::ESaveData))
            {
			iOutstandingEvent=saveType;
			}
		}
	}

/**
This method dispatches all client requests to the appropriate method calls.
@param aMessage The client's message
@param aCompleteRequest An output parameter. If zero, the client request
                will be completed later.
*/
void CServShutdownSession::DoServiceL(const RMessage2& aMessage, TBool& aCompleteRequest)
	{
	switch (aMessage.Function())
		{
	case TSaveOpCodeNotify:
		RequestNotifyPowerDown(aMessage);
		// don't complete async message yet
		aCompleteRequest=EFalse;
		break;
	case TSaveOpCodeNotifyCancel:
		RequestNotifyPowerDownCancel();
		break;
	case TSaveOpCodeHandleError:
		User::Leave(KErrNotSupported);
		break;
	case TSaveOpCodePowerOff:
		PowerOffL(aMessage);
		break;
	case TSaveOpCodeQueryPowerState:
		PowerStateL(aMessage);
		break;
#ifdef SYMBIAN_SSM_GRACEFUL_SHUTDOWN
	case EEventObsAdaptHandleShutdown:
	case EEventObsAdaptClientArrayCount:
	case EEventObsAdaptClientArray:
	case EEventObsAdaptIsClientHung:
	case EEventObsAdaptGetShutdownState:
		{
		if (IsSsmGracefulShutdown())	// SSM should be used for device shutdown
			{
			switch(aMessage.Function())
				{
				case EEventObsAdaptHandleShutdown:
					HandleShutdownEventL(aMessage);
					break;
				case EEventObsAdaptClientArrayCount:
					ClientArrayCount(aMessage);
					break;
				case EEventObsAdaptClientArray:
					ClientArrayL(aMessage);
					break;
				case EEventObsAdaptIsClientHung:
					IsClientHung(aMessage);
					break;
				case EEventObsAdaptGetShutdownState:
					GetShutdownState(aMessage);
					break;			
				}
			}
		break;	
		}
#endif // SYMBIAN_SSM_GRACEFUL_SHUTDOWN
	default:
		User::Leave(KErrNotSupported);
		break;
		}
	}

#ifdef SYMBIAN_SSM_GRACEFUL_SHUTDOWN

void CServShutdownSession::HandleShutdownEventL(const RMessage2& aMessage)
	{
	MSaveObserver::TSaveType action = static_cast <MSaveObserver::TSaveType> (aMessage.Int0());
	TBool powerOff = static_cast <TBool> (aMessage.Int1());
	TPowerState powerEvent = static_cast <TPowerState> (aMessage.Int2());

	CServShutdownServer* server = static_cast<CServShutdownServer*>(const_cast<CServer2*>(Server()));
	server->HandleShutdownEventL(action, powerOff, powerEvent);
	}

void CServShutdownSession::ClientArrayCount(const RMessage2& aMessage) const
	{
	CServShutdownServer* server = static_cast<CServShutdownServer*>(const_cast<CServer2*>(Server()));
	const TInt count = server->ClientArrayCount();
	aMessage.Write(0, TPckg<TInt>(count));
	}
/*
This function will write all registered client array in a buffer only when client side array count is equal to server side array count
else aMessage will contain an empty buffer and server side array count .
@param aMessage consists of buffer, clientside array count and server side array count 
*/
void CServShutdownSession::ClientArrayL(const RMessage2& aMessage)
	{
	CServShutdownServer* server = static_cast<CServShutdownServer*>(const_cast<CServer2*>(Server()));
	server->ClientArrayL(aMessage);
	}

void CServShutdownSession::IsClientHung(const RMessage2& aMessage) const
	{
	CServShutdownServer* server = static_cast<CServShutdownServer*>(const_cast<CServer2*>(Server()));
	TThreadId threadId = static_cast <TThreadId>(aMessage.Int0());
	TBool clientHung = server->IsClientHung(threadId);

	aMessage.Write(1, TPckg<TBool>(clientHung));
	}

void CServShutdownSession::GetShutdownState(const RMessage2& aMessage) const
	{
	TBool powerOff;
	TBool allSessionsHavePendingRequest;

	CServShutdownServer* server = static_cast<CServShutdownServer*>(const_cast<CServer2*>(Server()));
	server->GetShutdownState(powerOff, allSessionsHavePendingRequest);
	aMessage.Write(0, TPckg<TBool>(powerOff));
	aMessage.Write(1, TPckg<TBool>(allSessionsHavePendingRequest));
	}

#endif 	//SYMBIAN_SSM_GRACEFUL_SHUTDOWN

/**
Handles the servicing of client requests passed to the shutdown server.
@param	aMessage The message containing the client request.
*/
EXPORT_C void CServShutdownSession::ServiceL(const RMessage2& aMessage)
	{
	TBool aCompleteRequest=ETrue;
	TRAPD(error, DoServiceL(aMessage, aCompleteRequest));
	if (aCompleteRequest)
		{
		aMessage.Complete(error);
		}
	}

/**
This method processes a client-side registration request. It is an asynchronous request,
which will be completed later, when powerdown/low memory event occurs.
@param	aMessage The message containing the client request.
*/
void CServShutdownSession::RequestNotifyPowerDown(const RMessage2& aMessage)
	{
	iCurrentEvent=-1;
	iPtr = aMessage;

	if (iOutstandingEvent!=-1)
		{
		NotifySave((MSaveObserver::TSaveType)iOutstandingEvent);
		iOutstandingEvent=-1;
		}
	//The thread variable is just a dummy variable(input for CServShutdownServer::HandlePowerNotifRequest)
	//and is not used inside the function at all, it is there to preserve BC.
	RThread thread;
	static_cast<CServShutdownServer*>(const_cast<CServer2*>(Server()))->HandlePowerNotifRequest(thread);
	}

/**
This method cancels the client registration, completing the requests with 
KErrCancel error code.
*/
void CServShutdownSession::RequestNotifyPowerDownCancel()
	{
	if (HasPendingRequest())
		iPtr.Complete(KErrCancel);
	}

/**
@return The client's thread id.
*/
TThreadId CServShutdownSession::ClientThreadId() const
    {
    TThreadId id = NULL;
    RThread clientThread;
    //Clients which are not registered for notification will not have RMessage in the session and hence 
	//thread id will be NULL(e.g.CLafShutdownEventObserverAdaptor). Avoid such clients. 
    if(!iPtr.IsNull() && KErrNone == iPtr.Client(clientThread))
    	{	
		id = clientThread.Id();	
    	}
    clientThread.Close();	
    return id;
    }


/**
This method should be used only with  SYSLIBS_TEST macro defined and can be used to 
initiate a powerdown sequence.
Without SYSLIBS_TEST macro defined the method will panic the client with 
KErrNotSupported error code.
@param	aMessage The message containing the client request.
*/
#ifdef SYSLIBS_TEST
void CServShutdownSession::PowerOffL(const RMessage2& aMessage)
    {
	MSaveObserver::TSaveType action = static_cast <MSaveObserver::TSaveType> (aMessage.Int0());
    TBool powerOff = static_cast <TBool> (aMessage.Int1());
	CServShutdownServer* server = static_cast<CServShutdownServer*>(const_cast<CServer2*>(Server()));
    server->HandleShutdownEventL(action, powerOff);
    }
#else
void CServShutdownSession::PowerOffL(const RMessage2& aMessage)
    {
    aMessage.Panic(__SHUTDOWN_SERVER_NAME, KErrNotSupported);
    }
#endif//SYSLIBS_TEST

/**
This method should be used only with SYSLIBS_TEST macro defined and can be used to 
get the power state of the server.
Without SYSLIBS_TEST macro defined the method will panic the client with 
KErrNotSupported error code.
@param	aMessage The message containing the client request.
*/
#ifdef SYSLIBS_TEST
void CServShutdownSession::PowerStateL(const RMessage2& aMessage) const
    {
    CServShutdownServer* server = static_cast<CServShutdownServer*>(const_cast<CServer2*>(Server()));
    TBool powerOff = server->IsPowerOff();
    TPckg<TBool> powerOffPckg(powerOff);
    aMessage.WriteL(0,powerOffPckg);
    }
#else
void CServShutdownSession::PowerStateL(const RMessage2& aMessage) const
    {
    aMessage.Panic(__SHUTDOWN_SERVER_NAME, KErrNotSupported);
    }
#endif//SYSLIBS_TEST

