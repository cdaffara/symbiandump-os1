// SensibleServer.cpp
// 
// Copyright (c) 2006 - 2010 Accenture. All rights reserved.
// This component and the accompanying materials are made available
// under the terms of the "Eclipse Public License v1.0"
// which accompanies this distribution, and is available
// at the URL "http://www.eclipse.org/legal/epl-v10.html".
// 
// Initial Contributors:
// Accenture - Initial contribution
//

#include "SensibleServer.h"
#include "SensibleServer_server_specific.h"

#define DebugPanic() User::Panic(KDebugPanic, __LINE__)
#define ClientPanic(aMsg) PanicClient(aMsg, - __LINE__)

inline CShutdown::CShutdown()
	:CTimer(-1)
	{CActiveScheduler::Add(this);}
inline void CShutdown::ConstructL()
	{CTimer::ConstructL();}
inline void CShutdown::Start(TInt aDelay)
	{
	if (aDelay)
		{
		After(aDelay);
		}
	}

CSensibleServer::CSensibleServer()
	:CServerBase(0,ESharableSessions)
	{}

CSensibleSession::CSensibleSession()
	: iCallbackQ(_FOFF(CCallbackContext, iLink))
	{}

inline CSensibleServer& CSensibleSession::Server()
	{return *static_cast<CSensibleServer*>(const_cast<CServerBase*>(CSessionBase::Server()));}

inline CFilteringScheduler* CSensibleServer::Scheduler()
	{return iScheduler;}

#ifdef RUN_SERVER_WITH_EIKONENV

#include <eikenv.h>
#include <eikappui.h>

class CSensibleAppUi : public CEikAppUi
	{
public:
	void ConstructL();
	~CSensibleAppUi();

private:
	CSensibleServer* iServer;
	};

void CSensibleAppUi::ConstructL()
	{
	BaseConstructL(ENoAppResourceFile /*|ENoScreenFurniture*/);

	//
	// create the server
	iServer = new(ELeave) CServer_Class_Name();
	iServer->ConstructL();
	}

CSensibleAppUi::~CSensibleAppUi()
	{
	delete iServer;
	}

// Have to derive from CONE scheduler otherwise ccoeenv complains mightily
#define SCHEDULER_SUPER CCoeScheduler
#define SCHEDULER_CONSTRUCTOR CCoeScheduler(CCoeEnv::Static())
inline void ExitScheduler() { CBaActiveScheduler::Exit(); }

#else

#define SCHEDULER_SUPER CActiveScheduler
#define SCHEDULER_CONSTRUCTOR CActiveScheduler()
inline void ExitScheduler() { CActiveScheduler::Stop(); }

#endif

class CFilteringScheduler : public SCHEDULER_SUPER
	{
public:
	CFilteringScheduler();
	void WaitForAnyRequest();
	//void OnlyRunThisObject(CActive* aActive); // This doesn't work atm!
	void RunEverythingExcept(CActive* aActive, CActive* aActive2);
	void StopFiltering();
	
private:
	struct SObj {
		CActive* iObject;
		TInt iCachedStatus;
		};
	static const TInt KNumFilteredObjects = 2;
	SObj iFilteredObjects[KNumFilteredObjects];
	};

inline TServerStart::TServerStart()
	{}

void TServerStart::SignalL()
//
// Signal the owning thread that the server has started successfully
// This may itself fail
//
	{
#ifdef EKA2
	RProcess::Rendezvous(KErrNone);
#else
	RThread starter;
	User::LeaveIfError(starter.Open(iId));
	starter.RequestComplete(iStatus,KErrNone);
	starter.Close();
#endif
	}


///////////////////////

#ifndef __HIDE_IPC_V1__
void CSensibleSession::CreateL(const CServer& aServer)
//
// 2nd phase construct for sessions - called by the CServer framework
//
	{
	CSharableSession::CreateL(aServer);	// does not leave
	CreateL();
	}
#endif

void CSensibleSession::CreateL()
	{
	Server().AddSession();
	}

CSensibleSession::~CSensibleSession()
	{
	Server().DropSession();
	}

static void JustPanic(TAny*)
	{
	User::Panic(_L("Crash!"), 0);
	}

extern void CleanupPanicPushL()
	{
	CleanupStack::PushL(TCleanupItem(&JustPanic, 0));
	}

void CSensibleSession::ServiceL(const RMessage& aMessage)
//
// Handle a client request.
// Leaving is handled by CSensibleServer::RunError() which reports the error code
// to the client
//
	{
	switch (aMessage.Function())
		{
	case ERegisterCallbackNotifier:
		__ASSERT_ALWAYS(!iCallbackPending, ClientPanic(aMessage)); // Can't call Register is there's something already registered
		iCallbackNotifier = aMessage;
		iCallbackPending = ETrue;
		CompleteNextCallback();
		break;
	case EGetCallbackContext:
		{
		__ASSERT_ALWAYS(!iCallbackQ.IsEmpty(), ClientPanic(aMessage));
		CCallbackContext* c = iCallbackQ.First();
		__ASSERT_ALWAYS(c->Flag(EActive), ClientPanic(aMessage));
		__ASSERT_ALWAYS(c->CallbackHasContext(), ClientPanic(aMessage));
		aMessage.WriteL(SLOT(aMessage, 0), *c->Context());
		aMessage.Complete(KErrNone);
		if (!c->CallbackRequiresResult())
			{
			iCallbackQ.Remove(*c);
			delete c;
			}
		break;
		}
	case EWriteCallbackResultAndReregister:
		{
		__ASSERT_ALWAYS(!iCallbackPending, ClientPanic(aMessage)); // Can't call Register is there's something already registered
		__ASSERT_ALWAYS(!iCallbackQ.IsEmpty(), ClientPanic(aMessage));
		CCallbackContext* c = iCallbackQ.First();
		__ASSERT_ALWAYS(c->Flag(EActive), ClientPanic(aMessage));
		__ASSERT_ALWAYS(c->CallbackRequiresResult(), ClientPanic(aMessage));
		
		// Get the reregistering out of the way
		iCallbackNotifier = aMessage;
		iCallbackPending = ETrue;
		
		if (aMessage.Int2() < 0)
			{
			// Leave code
			c->SetFlags(EResultIsLeaveCode);
			c->Result().integer = aMessage.Int2();
			}
		else if (c->Flag(EResultHBufC16))
			{
			HBufC16* result = HBufC16::New(aMessage.Int2());
			if (!result) 
				{
				c->SetFlags(EResultIsLeaveCode);
				c->Result().integer = KErrNoMemory;
				}
			else
				{
				*c->Result().l = result;
				TPtr16 ptr(result->Des());
				aMessage.ReadL(SLOT(aMessage, 1), ptr);
				}
			}
		else if (c->Flag(EResultHBufC8))
			{
			HBufC8* result = HBufC8::New(aMessage.Int2());
			if (!result) 
				{
				c->SetFlags(EResultIsLeaveCode);
				c->Result().integer = KErrNoMemory;
				}
			else
				{
				*c->Result().s = result;
				TPtr8 ptr(result->Des());
				aMessage.ReadL(SLOT(aMessage, 1), ptr);
				}
			}
		else
			{
			// It's a TPkg
			aMessage.ReadL(SLOT(aMessage, 1), *c->Result().pkg);
			}
		CActiveScheduler::Stop();
		break;
		}
	case ECancelCallbackNotifier:
		{
		if (iCallbackPending)
			{
			iCallbackNotifier.Complete(KErrCancel);
			iCallbackPending = EFalse;
			}
		aMessage.Complete(KErrNone);
		break;
		}
	case EDummy:
		aMessage.Complete(KErrNone);
		break;

	default:
		if (!DoServiceL(aMessage))
			{
			PanicClient(aMessage, EPanicIllegalFunction);
			}
		break;
		}
	}

TBool CSensibleSession::DoServiceL(const RMessage& aMessage)
	{
	// Subclasses override this!
	aMessage.Complete(KErrNone);
	return ETrue;
	}

void CShutdown::RunL()
//
// Initiate server exit when the timer expires
//
	{
	ExitScheduler();
	}

void CSensibleServer::ConstructL()
//
// 2nd phase construction - ensure the timer and server objects are running
//
	{
	StartL(KMyServerName);
	iShutdown.ConstructL();
	// ensure that the server still exits even if the 1st client fails to connect
	iShutdown.Start(TransientServerShutdownTime());

	// Now set up our special scheduler. This is tricky because of good old eikonenv doing stuff differently
	// Basically without eikonenv, RunServer owns the old scheduler so we can't delete it
	// However eikonenv will delete the new one as part of its shutdown!

	iScheduler = new(ELeave) CFilteringScheduler();
	iOldScheduler = CActiveScheduler::Replace(iScheduler);
	
#ifdef RUN_SERVER_WITH_EIKONENV
	DISOWN(iOldScheduler);
#endif
	}

CSensibleServer::~CSensibleServer()
	{
#ifndef RUN_SERVER_WITH_EIKONENV
	DISOWN(iScheduler); // To mimic what CCoeEnv does
#endif
	}

#ifdef __HIDE_IPC_V1__
CSessionBase* CSensibleServer::NewSessionL(const TVersion& /*aVersion*/, const RMessage& /*aMessage*/) const
#else
CSessionBase* CSensibleServer::NewSessionL(const TVersion& /*aVersion*/) const
#endif
//
// Cretae a new client session. This should really check the version number.
//
	{
	return new(ELeave) CSensibleSession();
	}

void CSensibleServer::AddSession()
//
// A new session is being created
// Cancel the shutdown timer if it was running
//
	{
	++iSessionCount;
	iShutdown.Cancel();
	}

void CSensibleServer::DropSession()
//
// A session is being destroyed
// Start the shutdown timer if it is the last session.
//
	{
	if (--iSessionCount==0 && CActiveScheduler::Current()) // Check we have a scheduler, because if the server is being shut down there won't be one (and there'll be no point starting a shutdown timer
		iShutdown.Start(TransientServerShutdownTime());
	}


TInt CSensibleServer::RunError(TInt aError)
//
// Handle an error from CSensibleSession::ServiceL()
// A bad descriptor error implies a badly programmed client, so panic it;
// otherwise report the error to the client
//
	{
	if (aError==KErrBadDescriptor)
		PanicClient(Message(),EPanicBadDescriptor);
	else
		Message().Complete(aError);
	//
	// The leave will result in an early return from CServer::RunL(), skipping
	// the call to request another message. So do that now in order to keep the
	// server running.
	ReStart();
	return KErrNone;	// handled the error fully
	}

/*void CSensibleServer::BlockAllAOsExceptServerRequests()
	{
	Scheduler()->OnlyRunThisObject(this);
	}*/

void CSensibleServer::BlockRequestsFrom(CActive* aActive1, CActive* aActive2)
	{
	Scheduler()->RunEverythingExcept(aActive1, aActive2);
	}

void CSensibleServer::StopBlocking()
	{
	Scheduler()->StopFiltering();
	}

TInt CSensibleServer::TransientServerShutdownTime() const
	{
	return 2000000; // Default to 2 seconds
	}

void PanicClient(const RMessage& aMessage, TInt aPanic)
//
// RMessage::Panic() also completes the message. This is:
// (a) important for efficient cleanup within the kernel
// (b) a problem if the message is completed a second time
//
	{
	__DEBUGGER();
	aMessage.Panic(KDebugPanic, aPanic);
	}

static void RunServerL(TServerStart& aStart)
//
// Perform all server initialisation, in particular creation of the
// scheduler and server and then run the scheduler
//
	{

#ifndef RUN_SERVER_WITH_EIKONENV
	// create and install the active scheduler we need
	CActiveScheduler* s=new(ELeave) CActiveScheduler;
	CleanupStack::PushL(s);
	CActiveScheduler::Install(s);
#endif

	//
	// naming the server thread after the server helps to debug panics
#ifdef __SECURE_API__
	User::LeaveIfError(User::RenameThread(KMyServerName));
#else
	User::LeaveIfError(RThread().Rename(KMyServerName));
#endif

#ifdef RUN_SERVER_WITH_EIKONENV
	// In this case, the server creation/destruction is pushed into CSensibleAppUi

	// Give ourselves a eikonenv
	CEikonEnv* env = new CEikonEnv;
	CEikAppUi* appui = NULL;
	TInt err = KErrNone;
	if (env != NULL)
		{
		TRAP(err,
			env->ConstructL(EFalse);
			appui = new (ELeave)CSensibleAppUi();
			appui->ConstructL();
			env->SetAppUi(appui);
			);
		}
	if (err == KErrNone)
		{
		//
		// Initialisation complete, now signal the client
		aStart.SignalL();
		env->ExecuteD();
		}
	else
		{
		if (env != NULL)
			{
			env->DestroyEnvironment();
			}
		User::Leave(err); // This will tell the client that something's gone wrong
		}
#else	

	//
	// create the server
	CSensibleServer* server = new(ELeave) CServer_Class_Name();
	CleanupStack::PushL(server);
	server->ConstructL();
	//
	// Initialisation complete, now signal the client
	aStart.SignalL();
	//
	// Ready to run
	CActiveScheduler::Start();

	CleanupStack::PopAndDestroy(2, s); // server, scheduler

#endif
	}

static TInt RunServer(TServerStart& aStart)
//
// Main entry-point for the server thread
//
	{
	__UHEAP_MARK;
	//
	CTrapCleanup* cleanup=CTrapCleanup::New();
	TInt r=KErrNoMemory;
	if (cleanup)
		{
//#ifdef _DEBUG
//		TRAP(r, CleanupPanicPushL(); RunServerL(aStart); CleanupStack::Pop());
//#else
		TRAP(r,RunServerL(aStart));
//#endif
		delete cleanup;
		}
	//
	__UHEAP_MARKEND;
	return r;
	}


#ifndef EKA2

// The server binary is an "EPOCEXE" target type
// Thus the server parameter passing and startup code for WINS and EPOC are
// significantly different.

#ifdef __WINS__

// In WINS, the EPOCEXE target is a DLL with an entry point called WinsMain,
// taking no parameters and returning TInt. This is not really valid as a thread
// function which takes a TAny* parameter which we need.
//
// So the DLL entry-point WinsMain() is used to return a TInt representing the
// real thread function within the DLL. This is good as long as
// sizeof(TInt)>=sizeof(TThreadFunction).
//

static TInt ThreadFunction(TAny* aParms)
//
// WINS thread entry-point function.
// The TServerStart objects is passed as the thread parameter
//
	{
	return RunServer(*static_cast<TServerStart*>(aParms));
	}

IMPORT_C TInt WinsMain();
EXPORT_C TInt WinsMain()
//
// WINS DLL entry-point. Just return the real thread function 
// cast to TInt
//
	{
	return reinterpret_cast<TInt>(&ThreadFunction);
	}

TInt E32Dll(TDllReason)
	{
	return KErrNone;
	}

#else

//
// In EPOC, the EPOCEXE target is a process, and the server startup
// parameters are encoded in the command line
//

TInt TServerStart::GetCommand()
	{
	RProcess p;
	if (p.CommandLineLength()!=sizeof(TServerStart)/sizeof(TText))
		return KErrGeneral;
	TPtr ptr(reinterpret_cast<TText*>(this),0,sizeof(TServerStart)/sizeof(TText));
	p.CommandLine(ptr);
	return KErrNone;
	}

TInt E32Main()
//
// Server process entry-point
// Recover the startup parameters and run the server
//
	{
	TServerStart start;
	TInt r=start.GetCommand();
	if (r==KErrNone)
		r=RunServer(start);
	return r;
	}

#endif

#else

TInt E32Main()
//
// Server process entry-point
//
	{
	TServerStart start;
	TInt r = RunServer(start);
	return r;
	}

#endif

//// CCallbackContext ////

CCallbackContext::CCallbackContext(TCallbackCode aCode)
	: iCallback(aCode)
	{
	iCallback.iCode = aCode;
	}

CCallbackContext::~CCallbackContext()
	{
	__ASSERT_DEBUG(!Flag(EActive), DebugPanic());
	delete iContext;
	}

void CCallbackContext::SetFlags(TInt aFlags)
	{
	iFlags |= aFlags;
	}

TBool CCallbackContext::Flag(TInt aFlags) const
	{
	return iFlags & aFlags;
	}

void CCallbackContext::ClearFlags(TInt aFlags)
	{
	iFlags = iFlags & ~aFlags;
	}

TBool CCallbackContext::CallbackRequiresResult() const
	{
	//TODO
	return EFalse;
	}

TBool CCallbackContext::CallbackHasContext() const
	{
	return (iContext != NULL);
	}

void CCallbackContext::SetResult(TDes8& aPkg)
	{
	iResult.pkg = &aPkg;
	ClearFlags(EResultHBufC8 | EResultHBufC16);
	}

void CCallbackContext::SetResult(HBufC8*& aResult)
	{
	iResult.s = &aResult;
	SetFlags(EResultHBufC8);
	ClearFlags(EResultHBufC16);
	}

void CCallbackContext::SetResult(HBufC16*& aResult)
	{
	iResult.l = &aResult;
	SetFlags(EResultHBufC16);
	ClearFlags(EResultHBufC8);
	}

HBufC8* CCallbackContext::Context()
	{
	return iContext;
	}

CCallbackContext::TResult& CCallbackContext::Result()
	{
	return iResult;
	}

TServerCallback& CCallbackContext::Callback()
	{
	return iCallback;
	}

TCallbackWriter CCallbackContext::Writer()
	{
	TCallbackWriter res(iCallback, &iContext);
	return res;
	}

//// CFilteringScheduler ////

CFilteringScheduler::CFilteringScheduler()
: SCHEDULER_CONSTRUCTOR
	{}


/*
void CFilteringScheduler::OnlyRunThisObject(CActive* aActive)
	{
	__ASSERT_ALWAYS(!iObject, DebugPanic());
	iObject = &aActive->iStatus;
	iOnlyRunThisObject = ETrue;
	User::Panic(_L("OnlyRunThisObject doesn't work yet!"), 0);
	}
*/

void CFilteringScheduler::RunEverythingExcept(CActive* aActive, CActive* aActive2)
	{
	__ASSERT_ALWAYS(!iFilteredObjects[0].iObject, DebugPanic());
	iFilteredObjects[0].iObject = aActive;
	iFilteredObjects[1].iObject = aActive2;

	iFilteredObjects[0].iCachedStatus = KRequestPending;
	iFilteredObjects[1].iCachedStatus = KRequestPending;
	}

void CFilteringScheduler::StopFiltering()
	{
	for (TInt i = 0; i < KNumFilteredObjects; i++)
		{
		SObj& obj = iFilteredObjects[i];
		if (obj.iObject && obj.iCachedStatus != KRequestPending)
			{
			TRequestStatus* stat = &obj.iObject->iStatus;
			User::RequestComplete(stat, obj.iCachedStatus); // Since we consumed the signal from the previous complete, we need to re-signal by calling RequestComplete rather than just updating the object status ourselves
			}
		obj.iObject = NULL;
		}
	}

void CFilteringScheduler::WaitForAnyRequest()
	{

	if (!iFilteredObjects[0].iObject)
		{
		SCHEDULER_SUPER::WaitForAnyRequest();		
		return;
		}

	for (;;)
		{
		SCHEDULER_SUPER::WaitForAnyRequest();
		TBool found = EFalse;
		for (TInt i = 0; i < KNumFilteredObjects; i++)
			{
			SObj& obj = iFilteredObjects[i];
			TBool isReadyToRun = obj.iObject && obj.iObject->IsActive() && obj.iObject->iStatus != KRequestPending;
			if (isReadyToRun)
				{
				// Our target object has completed, so mark it back as pending, and consume the signal
				ASSERT(obj.iCachedStatus == KRequestPending); // If this is already set something has gone quite wrong with our logic
				obj.iCachedStatus = obj.iObject->iStatus.Int();
				*((TInt*)&obj.iObject->iStatus) = KRequestPending; // Cast this to a TInt* to prevent TRequestStatus::operator= changing the flags
				found = ETrue;
				break;
				}
			}
		if (!found) break; // It wasn't one of our objects that completed so no need to go round the loop again
		}
	}

//// TCallbackWriter ////

TCallbackWriter::TCallbackWriter(TServerCallback& aCallback, HBufC8** aContext)
: iCallback(aCallback), iContext(aContext), iBuf((TUint8*)aCallback.iData.Ptr(), aCallback.iData.MaxLength()), iInContext(EFalse)
	{
	if (iContext) *iContext = NULL;
	}

void TCallbackWriter::AddL(const TDesC8& aData, char* aType)
	{
	__ASSERT_DEBUG(aData.Length(), DebugPanic());
	__ASSERT_DEBUG(aType, DebugPanic());
	TInt bytesRemaining = iBuf.MaxSize() - iBuf.Size();

	TInt typeSize = 1;
	if (*aType == 'D')
		{
		// TDesC16s need to be 2-byte aligned, so make sure the data after the type byte will be
		if (!(iBuf.Length() & 1)) typeSize = 2;
		}

	if (aData.Size() + typeSize > bytesRemaining)
		{
		// No room for arg and type
 		if (!iContext) User::Leave(KErrNoMemory);
		if (!iInContext)
			{
			// so construct context
			if (*aType == 'D') typeSize = 2;
			*iContext = HBufC8::NewL(aData.Length() + typeSize);
			iInContext = ETrue;
			}
		else
			{
			// realloc
			HBufC8* newContext = (*iContext)->ReAlloc(Max(iBuf.MaxSize() * 2, iBuf.MaxSize() + aData.Size() + typeSize));
			if (!newContext)
				{
				delete *iContext;
				*iContext = NULL;
				User::Leave(KErrNoMemory);
				}
			*iContext = newContext;
			}
		iBuf.Set((*iContext)->Des());
		}
	iBuf.Append(*aType);
	if (typeSize == 2) iBuf.Append('-'); // Padding
	iBuf.Append(aData);
	if (iInContext) 
		{
		iCallback.iContextLength = iBuf.Length();
		}
	else
		{
		iCallback.iData.SetLength(iBuf.Length()); // Because a TPtr pointing to a buf doesn't behave like one pointing to an HBufC, sigh...
		}

	}

#define ADD(T, arg, type) { TPckg<T> x(arg); AddL(x, #type); }

void TCallbackWriter::AddL(TInt aInt)		{ ADD(TInt, aInt, i); }
void TCallbackWriter::AddL(TUint aInt)		{ ADD(TUint, aInt, u); }
void TCallbackWriter::AddL(TPoint aPoint)	{ ADD(TPoint, aPoint, P); }
void TCallbackWriter::AddL(TSize aSize)		{ ADD(TSize, aSize, S); }
void TCallbackWriter::AddL(TRgb aRgb)		{ ADD(TRgb, aRgb, G); }
void TCallbackWriter::AddL(TRect aRect)		{ ADD(TRect, aRect, R); }

void TCallbackWriter::AddL(const TDesC16& aDesc)
	{
	ADD(TInt, aDesc.Length(), i);
	TPtrC8 x((TUint8*)aDesc.Ptr(), aDesc.Size());
	AddL(x, "D");
	}

void TCallbackWriter::AddL(const TDesC8& aDesc)
	{
	ADD(TInt, aDesc.Length(), i);
	AddL(aDesc, "8");
	}

//// CSensibleSession ////

void CSensibleSession::QueueCallbackL(CCallbackContext* aContext)
	{
	iCallbackQ.AddLast(*aContext);
	if (aContext->CallbackRequiresResult())
		{
		if (aContext->Flag(EBlockServer))
			{
			//Server().Scheduler()->OnlyRunThisObject(&Server()); //TODO fix this at some point!
			}
		__ASSERT_ALWAYS(!iWaitingForCallbackResult, DebugPanic()); // This means someone queued a callback that required a result without specifying EBlockServer, and in the meantime someone else has queued another callback requiring a result. This isn't supported! If there is the remotest chance this could happen, then all must specify EBlockServer.
		iWaitingForCallbackResult = ETrue;
		CompleteNextCallback();
		CActiveScheduler::Start();
		// When we reach here, the client stuff has finished and aContext has our result in
		// We call CompleteNextCallback again here since to reach this point the server must have received a EWriteCallbackResultAndReregister, which means it's ready for another callback
		CompleteNextCallback();
		if (aContext->Flag(EBlockServer))
			{
			Server().Scheduler()->StopFiltering();
			}
		iWaitingForCallbackResult = EFalse;
		if (aContext->Flag(EResultIsLeaveCode))
			{
			iCallbackQ.Remove(*aContext);
			TInt err = aContext->Result().integer;
			delete aContext;
			User::Leave(err);
			}
		// Nothing else needed
		}
	else
		{
		CompleteNextCallback();
		}
	}

TBool CSensibleSession::DispatchCallback(TServerCallback& aCallback)
	{
	if (!iCallbackPending)
		{
		// Client not ready to be notified
		return EFalse;
		}

	TPckg<TServerCallback> pkg(aCallback);
	TRAPD(err, iCallbackNotifier.WriteL(SLOT(iCallbackNotifier, 0), pkg));
	if (err)
		{
		PanicClient(iCallbackNotifier, EPanicBadDescriptor);
		iCallbackPending = EFalse;
		return EFalse;
		}
	
	iCallbackNotifier.Complete(KErrNone);
	iCallbackPending = EFalse;
	return ETrue;
	}

void CSensibleSession::CompleteNextCallback()
	{
	if (!iCallbackPending)
		{
		// Client not ready to be notified
		return;
		}
	else if (iCallbackQ.IsEmpty())
		{
		// Nothing to complete yet
		return;
		}
	
	CCallbackContext* c = iCallbackQ.First();
	TPckg<TServerCallback> pkg(c->Callback());
	#ifdef __HIDE_IPC_V1__
	TRAPD(err, iCallbackNotifier.WriteL(0, pkg));
	#else
	TRAPD(err, iCallbackNotifier.WriteL(iCallbackNotifier.Ptr0(), pkg));
	#endif
	if (err)
		{
		PanicClient(iCallbackNotifier, EPanicBadDescriptor);
		iCallbackPending = EFalse;
		return;
		}
	
	iCallbackNotifier.Complete(KErrNone);
	iCallbackPending = EFalse;
	
	if (!c->CallbackRequiresResult() && !c->CallbackHasContext())
		{
		iCallbackQ.Remove(*c);
		delete c;
		}
	else
		{
		c->SetFlags(EActive);
		}
	}
