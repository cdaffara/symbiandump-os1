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

#include <savenotf.h>
#include <e32std.h>
#include <e32math.h>
#include <savepriv.h>
#include <e32svr.h>

const TInt KNumConnectRetries	=10;

const TUid KServerUid3={0x10004902};

#ifdef TEST_SHUTDOWN_SERVER 
_LIT(KTestShutdownServerName,"testshutdownsrvs");
#else
_LIT(KShutdownSrvName,"shutdownsrvs");
#endif	//TEST_SHUTDOWN_SERVER 

//
// class RSaveSession
//
/**
This method will establish the connection with the shutdown server. If the shutdown server
is not started yet, it will be started.
@return KErrNone The connection was established successfully.
@return Some other system-wide error codes
*/
TInt RSaveSession::ConnectL()
	{
	TInt err=KErrNone;
	TInt retry=KNumConnectRetries;
	FOREVER
		{
		#ifdef TEST_SHUTDOWN_SERVER 
		err = CreateSession(__TEST_SHUTDOWN_SERVER_NAME, TVersion(0,0,0));
		#else
		err = CreateSession(__SHUTDOWN_SERVER_NAME, TVersion(KShutdownMajorVN,KShutdownMinorVN,KShutdownBuildVN), KShutdownMessageSlots);
		#endif
		if ((--retry>0) && ((err==KErrNotFound) || (err==KErrServerTerminated)))
			{
			err = StartServerL();
			if ((err!=KErrNone) && (err!=KErrAlreadyExists))
				{
				break;
				}
			}
		else
			{
			break;
			}
		}
	return err;
	}

/**
This method starts the shutdown server.
@return KErrNone The server was startedsuccessfully.
@return KErrAlreadyExists The server is started already.
@return Some other system-wide error codes
*/
TInt RSaveSession::StartServerL()
	{
	TInt error=KErrNone;
	const TUidType serverUid(KNullUid,KNullUid,KServerUid3);

	RProcess server;
	#ifdef TEST_SHUTDOWN_SERVER 
	error = server.Create(KTestShutdownServerName,KNullDesC,serverUid);
	#else
	error = server.Create(KShutdownSrvName,KNullDesC,serverUid);
	#endif
	if(error!=KErrNone)
		return error;
 	TRequestStatus stat;
	server.Rendezvous(stat);
 	if (stat!=KRequestPending)
 		server.Kill(0);		// abort startup
 	else
 		server.Resume();	// logon OK - start the server
 	User::WaitForRequest(stat);		// wait for start or death
 	// we can't use the 'exit reason' if the server panicked as this
 	// is the panic 'reason' and may be '0' which cannot be distinguished
 	// from KErrNone
 	error=(server.ExitType()==EExitPanic) ? KErrGeneral : stat.Int();
	server.Close();
	return error;
	}

/**
This method sends client's registration request to the shutdown server.
This is an asynchronous request. It will be completed by the server later,
at the moment of power down/low memory event.
@param aStatus An output parameter - the request status.
*/
void RSaveSession::NotifySave(TRequestStatus& aStatus)
	{
	aStatus=KRequestPending;
	SendReceive(TSaveOpCodeNotify,aStatus);
	}

/**
This method sends client's cancel registration request to the shutdown server.
This is a synchronous request.
*/
void RSaveSession::NotifySaveCancel()
	{
	SendReceive(TSaveOpCodeNotifyCancel);
	}

/**
With SYSLIBS_TEST macro defined, this method initiates a powerdown sequence. 
Otherwise, it returns with KErrNotSupported.
@param aAction The action, which will be given to the clients, when calling their 
               MSaveObserver::SaveL() implementations.
@param aPowerOff Non-zero if you want to switch the power off.
@return KErrNone, if it sends the powerdown request to the server successfully. KErrServerTerminated, 
if the server no longer present; KErrServerBusy, if there are no message slots available; 
KErrNoMemory, if there is insufficient memory available.
*/
#ifdef SYSLIBS_TEST 
TInt RSaveSession::SwitchOff(MSaveObserver::TSaveType aAction, TBool aPowerOff)
    {
	return SendReceive(TSaveOpCodePowerOff, TIpcArgs(aAction, aPowerOff));
    }
#else
TInt RSaveSession::SwitchOff(MSaveObserver::TSaveType /*aAction*/, TBool /*aPowerOff*/)
    {
	return KErrNotSupported;
    }
#endif    

/**
With SYSLIBS_TEST macro defined, this method sends a request to query the power state of the server. 
Otherwise, it returns with KErrNotSupported.
@param aPowerOff The result of the query. Non-zero if the power state of the server is off.
@return KErrNone, if it gets the power state from the server successfully. KErrServerTerminated, 
if the server no longer present; KErrServerBusy, if there are no message slots available; 
KErrNoMemory, if there is insufficient memory available.
*/
#ifdef SYSLIBS_TEST 
TInt RSaveSession::ServerPowerState(TBool& aPowerOff)
	{
	TPckg<TBool> powerOffPckg(aPowerOff);
	return SendReceive(TSaveOpCodeQueryPowerState, TIpcArgs(&powerOffPckg));
	}
#else
TInt RSaveSession::ServerPowerState(TBool& /*aPowerOff*/)
	{
	return KErrNotSupported;
	}
#endif
//
// class CSaveNotifier : public CActive
//

enum TFlags
	{
	EDelayRequeue	=0x03,
	};

/**
@param aObserver A reference to the client's object, which implements MSaveObserver::SaveL() method.
@see MSaveObserver class.
*/
inline CSaveNotifier::CSaveNotifier(MSaveObserver& aObserver): 
	CActive(CActive::EPriorityStandard),
	iSaveObserver(aObserver)
	{
    }

/**
Standard phase one factory method for creating CSaveNotifier instances.
The caller of this method will be automatically registered to receive powerdown/low memory
notifications from the shutdown server.
@param aObserver A reference to the client's object, which implements MSaveObserver::SaveL() method.
@return A pointer to the created CSaveNotifier instance.
@see MSaveObserver class.
*/
EXPORT_C CSaveNotifier* CSaveNotifier::NewL(MSaveObserver& aObserver)
	{ // static
	CSaveNotifier* self=new(ELeave) CSaveNotifier(aObserver);
	CleanupStack::PushL(self);
	self->ConstructL();
	self->Start();
	CleanupStack::Pop();
	return self;
	}

/**
*/
EXPORT_C CSaveNotifier::~CSaveNotifier()
	{
	Cancel();
	iSaveSession.Close();
	}

/**
This method can delay/stop the powerdown sequence, if called after the client got the
powerdown notification (inside MSaveObserver::SaveL() implementation).
@see CSaveNotifier::Queue()
*/
EXPORT_C void CSaveNotifier::DelayRequeue()
	{
	iFlags|=EDelayRequeue;
	}

/**
This method re-register the client to receive powerdown/low memory notifications from the
shutdown server. It clears the effect of CSaveNotifier::DelayRequeue() call.
@see CSaveNotifier::DelayRequeue()
*/
EXPORT_C void CSaveNotifier::Queue()
	{
	iFlags&=~EDelayRequeue;
	Start();
	}

/**
The client will call this method if an error occurred during the powerdown processing.
But it ignores the error code supplied and re-queues for further events.
@param aError The error code.
@deprecated The server will not handle the error message from the client.
*/
EXPORT_C void CSaveNotifier::HandleError(TInt /*aError*/)
	{
  	if (!IsActive())
  		{
  		Queue();		
		}
	}

/**
Standard phase-two construction method for creating CSaveNotifier instances.
It creates a session with the shutdown server.
*/
void CSaveNotifier::ConstructL()
	{
	User::LeaveIfError(iSaveSession.ConnectL());
	CActiveScheduler::Add(this);
	}

/**
The method notifies the shutdown server tha the client wants to be notified when there is an
event like powerdown/low memory (client registration).
*/
void CSaveNotifier::Start()
	{
	if (!(iFlags&EDelayRequeue))
		{
		iStatus=KRequestPending;
		SetActive();
		iSaveSession.NotifySave(iStatus);
		}
	}

/**
The method cancels the client's registration.
*/
void CSaveNotifier::DoCancel()
	{
	iSaveSession.NotifySaveCancel();
	}

/**
This method gets called when the shutdown server completes the client request for notification,
registered when NotifySave() was called.
It calls the implementation of MSaveObserver::SaveL().
*/
void CSaveNotifier::RunL()
	{
	const TInt status=iStatus.Int();
	if (status<0)
		{
		User::Leave(status);
		}
	else
		{
		MSaveObserver::TSaveType saveType=(MSaveObserver::TSaveType)status;
		TRAPD(err,iSaveObserver.SaveL(saveType));
		Start();
		User::LeaveIfError(err);
		}
	}

/**
With SYSLIBS_TEST macro defined, this method initiates a powerdown sequence. 
Otherwise, it returns with KErrNotSupported.
@param aAction The action, which will be given to the clients, when calling their 
               MSaveObserver::SaveL() implementations.
@param aPowerOff Non-zero if you want to switch the power off.
@return KErrNone, if it sends the powerdown request to the server successfully. KErrServerTerminated, 
if the server no longer present; KErrServerBusy, if there are no message slots available; 
KErrNoMemory, if there is insufficient memory available.
@internalComponent
*/
#ifdef SYSLIBS_TEST 
EXPORT_C TInt CSaveNotifier::SwitchOff(MSaveObserver::TSaveType aAction, TBool aPowerOff)
    {
    return iSaveSession.SwitchOff(aAction, aPowerOff);
    }
#else
EXPORT_C TInt CSaveNotifier::SwitchOff(MSaveObserver::TSaveType /*aAction*/, TBool /*aPowerOff*/)
    {
    return KErrNotSupported;
    }
#endif

/**
With SYSLIBS_TEST macro defined, this method sends a request to query the power state of the server. 
Otherwise, it returns with KErrNotSupported.
@param aPowerOff The result of the query. Non-zero if the power state of the server is off.
@return KErrNone, if it gets the power state from the server successfully. KErrServerTerminated, 
if the server no longer present; KErrServerBusy, if there are no message slots available; 
KErrNoMemory, if there is insufficient memory available.
@internalComponent
*/
#ifdef SYSLIBS_TEST 
EXPORT_C TInt CSaveNotifier::ServerPowerState(TBool& aPowerOff)
	{
	return iSaveSession.ServerPowerState(aPowerOff);
	}
#else
EXPORT_C TInt CSaveNotifier::ServerPowerState(TBool& /*aPowerOff*/)
	{
    return KErrNotSupported;
    }
#endif
