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
#include <mmf/server/mmfdatapathproxy.h>
#ifdef SYMBIAN_ENABLE_SPLIT_HEADERS
#include <mmf/server/mmfdatapathproxyserver.h>
#endif

/**
Constructs a datapath event monitor object.

@param  aObserver
        A reference to the observer of the active object. The observer will be notified when an
        event occurs.
@param  aMMFDataPathProxy
        A reference to the client datapath proxy class.

@return A pointer to the new event monitor.

@since 7.0s
*/
EXPORT_C CMMFDataPathEventMonitor* CMMFDataPathEventMonitor::NewL(MMMFDataPathEventMonitorObserver& aObserver,
															 RMMFDataPathProxy& aMMFDataPathProxy)
	{
	return (new(ELeave) CMMFDataPathEventMonitor(aObserver, aMMFDataPathProxy));
	}

CMMFDataPathEventMonitor::CMMFDataPathEventMonitor(MMMFDataPathEventMonitorObserver& aObserver, 
													   RMMFDataPathProxy& aMMFDataPathProxy) :
	CActive(EPriorityStandard),
	iObserver(aObserver), 
	iMMFDataPathProxy(aMMFDataPathProxy)
	{
	CActiveScheduler::Add(this);
	}

/**
Destructor.

Calls Cancel().
*/
EXPORT_C CMMFDataPathEventMonitor::~CMMFDataPathEventMonitor()
	{
	Cancel();
	}

/**
Tells the datapath event monitor to start listening for events.

The datapath proxy must have been opened before this method is called.

@since  7.0s
*/
EXPORT_C void CMMFDataPathEventMonitor::Start()
	{
	iMMFDataPathProxy.ReceiveEvents(iEventPckg, iStatus);
	SetActive();
	}

/**
Internal active object function.

Starts the data path event monitor and handles an event if there is no error status.

Calls HandleEvent on iObserver.

@since  7.0s
*/
EXPORT_C void CMMFDataPathEventMonitor::RunL()
	{
	if (iStatus.Int() == KErrNone)
		{
		iObserver.HandleEvent(iEventPckg());
		Start();
		}
	else
		{
		//something's gone wrong with trying to receive events (e.g. server died etc)
		TMMFEvent event(KMMFErrorCategoryDataPathGeneralError, iStatus.Int());
		iObserver.HandleEvent(event);
		//we don't want to receive events again here...
		}
	}

/**
Cancels the outstanding request on iMMFDataPathProxy.

@since  7.0s
*/
void CMMFDataPathEventMonitor::DoCancel()
	{
	iMMFDataPathProxy.CancelReceiveEvents();
	}

/**
Creates a subthread that will contain the datapath.

@return An error code indicating if the function call was successful. KErrNone on success, otherwise
        another of the system-wide error codes.

@since 7.0s
*/
EXPORT_C TInt RMMFDataPathProxy::CreateSubThread()
	{
	//start the subthread with a unique name
	TName subThreadName(_L("MMFDataPathThread"));
	subThreadName.AppendNum(Math::Random(),EHex);
	
	TInt error = DoCreateSubThread(subThreadName, &CMMFDataPathProxyServer::StartThread);
	if (error)
		return error;

	//now create a session with the subthread
	error = CreateSession(subThreadName, KMMFDataPathProxyVersion);

	return error;
	}

// Note: in the following, we can pass straight addresses for writing since both client and server
// are in the same process.
/**
Load the datapath in the new thread. Use this method if the codec UID is not known
and there is no datapath ambiguity, ie. only one datapath is possible.

@return An error code indicating if the function call was successful. KErrNone on success, otherwise
        another of the system-wide error codes.

@since 7.0s
*/
EXPORT_C TInt RMMFDataPathProxy::LoadDataPath()
	{
	return SendReceive(EMMFDataPathProxyLoadDataPathBy);
	}

/**
Load the datapath in the new thread. 

Use this method if the codec UID is not known but the TMediaId can be used to select which codec 
to use.

@param  aMediaId
	    The type of media to be handled by this datapath.

@return An error code indicating if the function call was successful. KErrNone on success, otherwise
        another of the system-wide error codes.

@since  7.0s
*/
EXPORT_C TInt RMMFDataPathProxy::LoadDataPath(TMediaId aMediaId)
	{
	return SendReceive (EMMFDataPathProxyLoadDataPathByMediaId, TInt(&aMediaId));
	}

/**
Load the datapath in the new thread. 

Use this method if the codec UID is known and there is no datapath ambiguity, ie. only one datapath
is possible.

@param  aCodecUid
        The UID of the codec plugin to be used by the datapath.

@return An error code indicating if the function call was successful. KErrNone on success, otherwise
        another of the system-wide error codes.

@since  7.0s
*/
EXPORT_C TInt RMMFDataPathProxy::LoadDataPath(TUid aCodecUid) 
	{
	return SendReceive(EMMFDataPathProxyLoadDataPathByCodecUid, TInt(&aCodecUid));
	}

/**
Loads the datapath in the new thread. 

Use this method if the codec UID is known and there is datapath ambiguity. The TMediaId will be
used to choose the correct path.

@param  aCodecUid
        The UID of the codec plugin to be used by the datapath.
@param  aMediaId
        The type of media to be handled by this datapath.

@return An error code indicating if the function call was successful. KErrNone on success, otherwise
        another of the system-wide error codes.

@since  7.0s
*/
EXPORT_C TInt RMMFDataPathProxy::LoadDataPath(TUid aCodecUid, TMediaId aMediaId)
	{
	return SendReceive(EMMFDataPathProxyLoadDataPathByMediaIdCodecUid, TInt(&aCodecUid), TInt(&aMediaId));
	}
	
/**
Specify the data source for this datapath.

If the sink already exists, this function tries to establish a connection between the source and 
sink. Note that only one data source can be added to the datapath.

@param  aSource
        A pointer to the data source.

@return An error code indicating if the function call was successful. KErrNone on success, otherwise
        another of the system-wide error codes.

@since 7.0s
*/
EXPORT_C TInt RMMFDataPathProxy::AddDataSource(MDataSource* aSource)
	{
	return SendReceive(EMMFDataPathProxyAddDataSource, TInt(aSource));
	}
	
/**
Specify the data sink for this datapath.  

If the source already exists, this function tries to establish a connection between the source and 
the sink. Note that only one data sink can be added to the datapath.

@param  aSink
        A pointer to the data sink.

@return An error code indicating if the function call was successful. KErrNone on success, otherwise
        another of the system-wide error codes.

@since  7.0s
*/
EXPORT_C TInt RMMFDataPathProxy::AddDataSink(MDataSink* aSink)
	{
	return SendReceive(EMMFDataPathProxyAddDataSink, TInt(aSink));
	}
	
/**
Transfer the datapath from the stopped into the primed state.

This function allocates buffers in preparation to play and must be called before calling PlayL().

@return An error code indicating if the function call was successful. KErrNone on success, otherwise
        another of the system-wide error codes.

@since  7.0s
*/
EXPORT_C TInt RMMFDataPathProxy::Prime()
	{
	return SendReceive(EMMFDataPathProxyPrime);
	}

/**
Transfer the datapath from the primed into the playing state.

This function starts an active scheduler play loop and can only play from the primed state.

@return An error code indicating if the function call was successful. KErrNone on success, otherwise
        another of the system-wide error codes.

@since 7.0s
*/
EXPORT_C TInt RMMFDataPathProxy::Play()
	{
	return SendReceive(EMMFDataPathProxyPlay);
	}

/**
Pauses playing.

This function transfers the datapath from the playing into the primed state and sends 
KMMFErrorCategoryDataPathGeneralError to the client if an error occurs.

@return An error code indicating if the function call was successful. KErrNone on success, otherwise
        another of the system-wide error codes.

@since  7.0s
*/
EXPORT_C TInt RMMFDataPathProxy::Pause()
	{
	return SendReceive(EMMFDataPathProxyPause);
	}
	
/**
Stops playing.

This function transfers the datapath from the primed into the stopped state and resets the data 
path position, but does not clean up buffers. It also sends KMMFErrorCategoryDataPathGeneralError 
to the client if an error occurs.

@return An error code indicating if the function call was successful. KErrNone on success, otherwise
        another of the system-wide error codes.

@since  7.0s
*/
EXPORT_C TInt RMMFDataPathProxy::Stop()
	{
	return SendReceive(EMMFDataPathProxyStop);
	}
	
/**
Gets the current position of the datapath in units of time.

@param  aPosition
        The current position in micro seconds will be copied into this variable.

@return An error code indicating if the function call was successful. KErrNone on success, otherwise
        another of the system-wide error codes.

@since  7.0s
*/
EXPORT_C TInt RMMFDataPathProxy::GetPosition(TTimeIntervalMicroSeconds& aPosition) const
	{
	return SendReceive(EMMFDataPathProxyGetPosition, TInt(&aPosition));
	}

/**
Sets the current position of the datapath in units of time.

@param  aPosition
        The required position in micro seconds.

@return An error code indicating if the function call was successful. KErrNone on success, otherwise
        another of the system-wide error codes.

@since 7.0s
**/
EXPORT_C TInt RMMFDataPathProxy::SetPosition(const TTimeIntervalMicroSeconds& aPosition)
	{
	return SendReceive(EMMFDataPathProxySetPosition, TInt(&aPosition));
	}
	
/**
Sets the play window relative to the start of the entire clip.

@param  aStart
        The start position in micro seconds relative to the start of the clip.
@param  aEnd
        The end position in micro seconds relative to the start of the clip.

@return An error code indicating if the function call was successful. KErrNone on success, otherwise
        another of the system-wide error codes.

@since  7.0s
*/
EXPORT_C TInt RMMFDataPathProxy::SetPlayWindow( const TTimeIntervalMicroSeconds& aStart, const TTimeIntervalMicroSeconds& aEnd )
	{
	return SendReceive( EMMFDataPathProxySetPlayWindow, TInt(&aStart), TInt(&aEnd)) ;
	}
	
/**
Removes a previously defined play window.

@return An error code indicating if the function call was successful. KErrNone on success, otherwise
        another of the system-wide error codes.

@since  7.0s
*/
EXPORT_C TInt RMMFDataPathProxy::ClearPlayWindow()
	{
	return SendReceive( EMMFDataPathProxyClearPlayWindow) ;
	}
	
/**
Gets the current datapath state.

@param  aState
        The current state of the datapath will be copied into this variable.

@return An error code indicating if the function call was successful. KErrNone on success, otherwise
        another of the system-wide error codes.

@since  7.0s
*/
EXPORT_C TInt RMMFDataPathProxy::State( TInt& aState )
	{
	return SendReceive( EMMFDataPathProxyState, TInt(&aState) ) ; // pass address, not value
	}

/**
Deletes the datapath and shuts down the datapath proxy thread.

Calls RMMFSubThreadBase::Shutdown(). This function will not return until the subthread has 
exited, or a timeout has occurred.

@since  7.0s
*/
EXPORT_C void RMMFDataPathProxy::Close()
	{
	Shutdown();
	}






CMMFDataPathProxyServer* CMMFDataPathProxyServer::NewL()
	{
	CMMFDataPathProxyServer* s = new(ELeave) CMMFDataPathProxyServer();
	CleanupStack::PushL(s);
	s->ConstructL();
	CleanupStack::Pop();
	return s;
	}

CMMFDataPathProxyServer::CMMFDataPathProxyServer() :
	CMMFSubThreadServer(EPriorityStandard)
	{
	}

void CMMFDataPathProxyServer::ConstructL()
	{
	//just need to call baseclass's constructL here
	CMMFSubThreadServer::ConstructL();
	}

CMMFDataPathProxyServer::~CMMFDataPathProxyServer()
	{
	}

CMmfIpcSession* CMMFDataPathProxyServer::NewSessionL(const TVersion& aVersion) const
	{
	if (!User::QueryVersionSupported(KMMFDataPathProxyVersion, aVersion))
		User::Leave(KErrNotSupported);

	return CMMFDataPathProxySession::NewL();
	}

TInt CMMFDataPathProxyServer::StartThread(TAny*)
	{
	TInt err = KErrNone;
	//create cleanupstack
	CTrapCleanup* cleanup=CTrapCleanup::New(); // get clean-up stack
	if (!cleanup)
		err = KErrNoMemory;
	if (!err)
		{
		TRAP(err, DoStartThreadL());
		}
	delete cleanup;
	return err;
	}

void CMMFDataPathProxyServer::DoStartThreadL()
	{
	// create and install the active scheduler we need
	CActiveScheduler* s=new(ELeave) CActiveScheduler;
	CleanupStack::PushL(s);
	CActiveScheduler::Install(s);
	// create the server (leave it on the cleanup stack)
	CleanupStack::PushL(CMMFDataPathProxyServer::NewL());
	// Initialisation complete, now signal the client
	RThread::Rendezvous(KErrNone);
	// Ready to run
	CActiveScheduler::Start();
	// Cleanup the server and scheduler
	CleanupStack::PopAndDestroy(2);
	REComSession::FinalClose(); // fix 047933
	}





CMMFDataPathProxySession* CMMFDataPathProxySession::NewL()
	{
	return new(ELeave) CMMFDataPathProxySession();
	}

CMMFDataPathProxySession::CMMFDataPathProxySession()
	{
	}


CMMFDataPathProxySession::~CMMFDataPathProxySession()
	{
	delete iDataPath;
	}

void CMMFDataPathProxySession::ServiceL(const RMmfIpcMessage& aMessage)
	{
	TBool complete = EFalse;
	switch(aMessage.Function())
		{
	case EMMFDataPathProxyLoadDataPathBy:
		complete = LoadDataPathByL(aMessage);
		break;
	case EMMFDataPathProxyLoadDataPathByMediaId:
		complete = LoadDataPathByMediaIdL(aMessage);
		break;
	case EMMFDataPathProxyLoadDataPathByCodecUid:
		complete = LoadDataPathByCodecUidL(aMessage);
		break;
	case EMMFDataPathProxyLoadDataPathByMediaIdCodecUid:
		complete = LoadDataPathByMediaIdCodecUidL(aMessage);
		break;
	case EMMFDataPathProxyAddDataSource:
		complete = AddDataSourceL(aMessage);
		break;
	case EMMFDataPathProxyAddDataSink:
		complete = AddDataSinkL(aMessage);
		break;
	case EMMFDataPathProxyPrime:
		complete = PrimeL(aMessage);
		break;
	case EMMFDataPathProxyPlay:
		complete = PlayL(aMessage);
		break;
	case EMMFDataPathProxyPause:
		complete = PauseL(aMessage);
		break;
	case EMMFDataPathProxyStop:
		complete = StopL(aMessage);
		break;
	case EMMFDataPathProxyGetPosition:
		complete = GetPositionL(aMessage);
		break;
	case EMMFDataPathProxySetPosition:
		complete = SetPositionL(aMessage);
		break;
	case EMMFDataPathProxySetPlayWindow :
		complete = SetPlayWindowL(aMessage) ;
		break;
	case EMMFDataPathProxyClearPlayWindow:
		complete = ClearPlayWindowL(aMessage);
		break;
	case EMMFDataPathProxyState:
		complete = StateL(aMessage);
		break;
	case EMMFSubThreadReceiveEvents:
		complete = ReceiveEventsL(aMessage);//provided by baseclass
		break;
	case EMMFSubThreadCancelReceiveEvents:
		complete = CancelReceiveEvents();//provided by baseclass
		break;
	case EMMFSubThreadShutdown:
		complete = ShutDown();//provided by baseclass
		break;
	default:
		User::Leave(KErrNotSupported);
		break;
		}

	if (complete)
		aMessage.Complete(KErrNone);
	}


TBool CMMFDataPathProxySession::LoadDataPathByL(const RMmfIpcMessage& /*aMessage*/)
	{
	if (iDataPath)
		User::Leave(KErrAlreadyExists);
	iDataPath = CMMFDataPath::NewL(*this);
	return ETrue;
	}

TBool CMMFDataPathProxySession::LoadDataPathByMediaIdL(const RMmfIpcMessage& aMessage)
	{
	if (iDataPath)
		User::Leave(KErrAlreadyExists);
	TMediaId* mediaId = REINTERPRET_CAST(TMediaId*, aMessage.Int0());
	iDataPath = CMMFDataPath::NewL(*mediaId, *this);
	return ETrue;
	}

TBool CMMFDataPathProxySession::LoadDataPathByCodecUidL(const RMmfIpcMessage& aMessage)
	{
	if (iDataPath)
		User::Leave(KErrAlreadyExists);
	TUid* uid = REINTERPRET_CAST(TUid*, aMessage.Int0());
	iDataPath = CMMFDataPath::NewL(*uid, *this);
	return ETrue;
	}

TBool CMMFDataPathProxySession::LoadDataPathByMediaIdCodecUidL(const RMmfIpcMessage& aMessage)
	{
	if (iDataPath)
		User::Leave(KErrAlreadyExists);
	TUid* uid = REINTERPRET_CAST(TUid*, aMessage.Int0());
	TMediaId* mediaId = REINTERPRET_CAST(TMediaId*, aMessage.Int1());
	iDataPath = CMMFDataPath::NewL(*uid, *mediaId, *this);
	return ETrue;
	}

TBool CMMFDataPathProxySession::AddDataSourceL(const RMmfIpcMessage& aMessage)
	{
	CheckDataPathExistsL();
	MDataSource* source = REINTERPRET_CAST(MDataSource*, aMessage.Int0());
	iDataPath->AddDataSourceL(source);
	return ETrue;
	}

TBool CMMFDataPathProxySession::AddDataSinkL(const RMmfIpcMessage& aMessage)
	{
	CheckDataPathExistsL();
	MDataSink* sink = REINTERPRET_CAST(MDataSink*, aMessage.Int0());
	iDataPath->AddDataSinkL(sink);
	return ETrue;
	}

TBool CMMFDataPathProxySession::PrimeL(const RMmfIpcMessage& /*aMessage*/)
	{
	CheckDataPathExistsL();
	iDataPath->PrimeL();
	return ETrue;
	}

TBool CMMFDataPathProxySession::PlayL(const RMmfIpcMessage& /*aMessage*/)
	{
	CheckDataPathExistsL();
	iDataPath->PlayL();
	return ETrue;
	}

TBool CMMFDataPathProxySession::PauseL(const RMmfIpcMessage& /*aMessage*/)
	{
	CheckDataPathExistsL();
	iDataPath->Pause();
	return ETrue;
	}

TBool CMMFDataPathProxySession::StopL(const RMmfIpcMessage& /*aMessage*/)
	{
	CheckDataPathExistsL();
	iDataPath->Stop();
	return ETrue;
	}

TBool CMMFDataPathProxySession::GetPositionL(const RMmfIpcMessage& aMessage) const
	{
	CheckDataPathExistsL();
	TTimeIntervalMicroSeconds* t = REINTERPRET_CAST(TTimeIntervalMicroSeconds*, aMessage.Int0());
	*t = iDataPath->Position();
	return ETrue;
	}

TBool CMMFDataPathProxySession::SetPositionL(const RMmfIpcMessage& aMessage)
	{
	CheckDataPathExistsL();
	TTimeIntervalMicroSeconds* t = REINTERPRET_CAST(TTimeIntervalMicroSeconds*, aMessage.Int0());
	iDataPath->SetPositionL(*t);
	return ETrue;
	}

TBool CMMFDataPathProxySession::SetPlayWindowL(const RMmfIpcMessage& aMessage) 
	{
	CheckDataPathExistsL() ;
	TTimeIntervalMicroSeconds* start = REINTERPRET_CAST( TTimeIntervalMicroSeconds*, aMessage.Int0() ) ;
	TTimeIntervalMicroSeconds* end = REINTERPRET_CAST( TTimeIntervalMicroSeconds*, aMessage.Int1() ) ;
	iDataPath->SetPlayWindowL( *start, *end ) ;
	return ETrue;
	}

TBool CMMFDataPathProxySession::ClearPlayWindowL(const RMmfIpcMessage& /*aMessage*/)
	{
	CheckDataPathExistsL() ;
	iDataPath->ClearPlayWindowL() ;
	return ETrue ;
	}

TBool CMMFDataPathProxySession::StateL(const RMmfIpcMessage& aMessage)
	{
	CheckDataPathExistsL() ;
	TInt* state = REINTERPRET_CAST( TInt*, aMessage.Int0() ) ;
	*state = iDataPath->State();
	return ETrue ;
	}

