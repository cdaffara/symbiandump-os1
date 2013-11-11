// server.cpp
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

#include "server.h"
#include "pipe.h"
#include "console.h"
#include "file.h"
#include "null.h"
#include "persistentconsole.h"
#include "readwrite.h"
#include "clientserver.h"
#include "session.h"


#ifndef EKA2

//
// TServerStart.
//

TServerStart::TServerStart()
	{
	}

void TServerStart::SignalL()
	{
	RThread starter;
	User::LeaveIfError(starter.Open(iId));
	starter.RequestComplete(iStatus,KErrNone);
	starter.Close();
	}

#endif


//
// CShutdownTimer.
//

CShutdownTimer::CShutdownTimer()
	: CTimer(-1)
	{
	CActiveScheduler::Add(this);
	}

void CShutdownTimer::ConstructL()
	{
	CTimer::ConstructL();
	}

void CShutdownTimer::Start()
	{
	After(KShutdownDelay);
	}

void CShutdownTimer::RunL()
	{
	CActiveScheduler::Stop();
	}


//
// CIoServer.
//

CIoServer::CIoServer()
#ifdef EKA2
	: CServer2(0, ESharableSessions)
#else
	: CServer(0, ESharableSessions)
#endif
	{
	
	}

#ifdef EKA2
CServer2* CIoServer::NewLC()
#else
CServer* CIoServer::NewLC()
#endif
	{
	CIoServer* self=new(ELeave) CIoServer;
	CleanupStack::PushL(self);
	self->ConstructL();
	return self;
	}

CIoServer::~CIoServer()
	{
	if (iIoObjectContainerIndex)
		{
		iIoObjectContainerIndex->Remove(iIoObjectContainer);
		}
	delete iIoObjectContainerIndex;
#ifdef IOSRV_LOGGING
	delete iLog;
#endif
	}

void CIoServer::ConstructL()
	{
#if defined (__WINS__) && !defined (EKA2)
	RThread().SetPriority(EPriorityAbsoluteHigh);
#else
	RProcess().SetPriority(::EPriorityHigh);
#endif
	User::LeaveIfError(iFs.Connect());
#ifdef IOSRV_LOGGING
	iLog = CIoLog::NewL(iFs);
#endif
	iConfig.Init(); // Ignore error (defaults will be used).
	StartL(KIoServerName);
	iIoObjectContainerIndex = CObjectConIx::NewL();
	iIoObjectContainer = iIoObjectContainerIndex->CreateL();
	iShutdownTimer.ConstructL();
	iShutdownTimer.Start();
	}

#ifdef EKA2
	CSession2* CIoServer::NewSessionL(const TVersion&, const RMessage2&) const
#else
	CSharableSession* CIoServer::NewSessionL(const TVersion&) const
#endif
	{
	CIoSession* session = new(ELeave) CIoSession();
	LOG(CIoLog::Printf(_L("CIoSession 0x%08x created\r\n"), session));
	return session;
	}

void CIoServer::AddSession()
	{
	++iSessionCount;
	iShutdownTimer.Cancel();
	}

void CIoServer::DropSession()
	{
	if (--iSessionCount == 0)
		{
		iShutdownTimer.Start();
		}
	}

CIoPipe& CIoServer::CreatePipeLC()
	{
	CIoPipe* pipe = CIoPipe::NewLC();
	iIoObjectContainer->AddL(pipe);
	return *pipe;
	}

CIoConsole& CIoServer::CreateConsoleLC(const TDesC& aImplementation, const TDesC& aTitle, TSize aSize, MIoWriteEndPoint* aUnderlyingConsole, TUint aOptions)
	{
	CIoConsole* underlying = NULL;
	while (aUnderlyingConsole && aUnderlyingConsole->IoepIsType(RIoHandle::EPersistentConsole))
		{
		aUnderlyingConsole = ((CIoPersistentConsole*)aUnderlyingConsole)->TransientWriter();
		}
	if (aUnderlyingConsole && aUnderlyingConsole->IoepIsConsole())
		{
		underlying = (CIoConsole*)aUnderlyingConsole;
		}
		
	CIoConsole* console = CIoConsole::NewLC(aImplementation, aTitle, aSize, iConfig, underlying, aOptions);
	iIoObjectContainer->AddL(console);
	return *console;
	}

CIoFile& CIoServer::CreateFileLC(const TDesC& aName, RIoFile::TMode aMode)
	{
	CIoFile* file = CIoFile::NewLC(iFs, aName, aMode);
	iIoObjectContainer->AddL(file);
	return *file;
	}

CIoNull& CIoServer::CreateNullLC()
	{
	CIoNull* null = CIoNull::NewLC();
	iIoObjectContainer->AddL(null);
	return *null;
	}
	
CIoPersistentConsole& CIoServer::CreatePersistentConsoleLC(const TDesC& aName, const TDesC& aTitle, const RMsg& aMessage)
	{
	CIoPersistentConsole* pcons = CIoPersistentConsole::NewLC(aName, aTitle, *this, aMessage);
	iIoObjectContainer->AddL(pcons);
	return *pcons;
	}


CIoReadObject& CIoServer::CreateReadObjLC()
	{
	CIoReadObject* obj = CIoReadObject::NewLC(iNextReadObjId++);
	iIoObjectContainer->AddL(obj);
	return *obj;
	}

CIoWriteObject& CIoServer::CreateWriteObjLC()
	{
	CIoWriteObject* obj = CIoWriteObject::NewLC(iNextWriteObjId++);
	iIoObjectContainer->AddL(obj);
	return *obj;
	}

CIoReadObject* CIoServer::NextReadObj(TThreadId aOwningThread) const
	{
	return (CIoReadObject*)DoFindObj(RIoHandle::EReadObject, aOwningThread, ETrue);
	}

CIoReadWriteObject* CIoServer::DoFindObj(RIoHandle::TType aType, TThreadId aOwningThread, TBool aNext) const
	{
	CIoReadWriteObject* lastMatchingObj = NULL;
	const TInt numObjs = iIoObjectContainer->Count();
	for (TInt i = 0; i < numObjs; ++i)
		{
		CIoObject* obj = (CIoObject*)(*iIoObjectContainer)[i];
		if (obj->IsType(aType))
			{
			CIoReadWriteObject* readWriteObj = (CIoReadWriteObject*)obj;
			if (aNext && readWriteObj->IsOwner(aOwningThread) && !readWriteObj->OpenedByOwner())
				{
				// aNext==ETrue means we return the first matching object that isn't already opened by its owner
				return readWriteObj;
				}
			else if (!aNext && readWriteObj->IsOwner(aOwningThread) && readWriteObj->OpenedByOwner())
				{
				// aNext==EFalse means we return the last matching object that *is* opened
				lastMatchingObj = readWriteObj;
				}
			}
		}
	return lastMatchingObj;
	}

CIoWriteObject* CIoServer::NextWriteObj(TThreadId aOwningThread) const
	{
	return (CIoWriteObject*)DoFindObj(RIoHandle::EWriteObject, aOwningThread, ETrue);
	}

CIoReadObject* CIoServer::LastOpenedReadObj(TThreadId aOwningThread) const
	{
	return (CIoReadObject*)DoFindObj(RIoHandle::EReadObject, aOwningThread, EFalse);
	}

CIoWriteObject* CIoServer::LastOpenedWriteObj(TThreadId aOwningThread) const
	{
	return (CIoWriteObject*)DoFindObj(RIoHandle::EWriteObject, aOwningThread, EFalse);
	}

CIoPersistentConsole& CIoServer::FindPersistentConsoleL(const TDesC& aName)
	{
	const TInt numObjs = iIoObjectContainer->Count();
	for (TInt i=0; i<numObjs; ++i)
		{
		CIoObject* obj = (CIoObject*)(*iIoObjectContainer)[i];
		if (obj->IsType(RIoHandle::EPersistentConsole))
			{
			CIoPersistentConsole& pcons = (CIoPersistentConsole&)*obj;
			if (pcons.Name().Compare(aName)==0)
				{
				return pcons;
				}
			}
		}
	User::Leave(KErrNotFound);
	return *(CIoPersistentConsole*)1; // To keep the compiler happy.
	}

CIoObject* CIoServer::FindObjectByName(RIoHandle::TType aType, TInt& aFindHandle, const TDesC& aMatch, TName& aName) const
	{
	FOREVER
		{
		TInt err = iIoObjectContainer->FindByName(aFindHandle, aMatch, aName);
		if (err == KErrNone)
			{
			CIoObject* obj = (CIoObject*)iIoObjectContainer->At(aFindHandle);
			if (obj->IsType(aType))
				{
				return obj;
				}
			}
		else
			{
			break;
			}
		}
	return NULL;
	}

CIoObject& CIoServer::OpenObjectLC(TInt aFindHandle)
	{
	CIoObject* obj = (CIoObject*)iIoObjectContainer->At(aFindHandle);
	if (obj)
		{
		User::LeaveIfError(obj->Open());
		CleanupClosePushL(*obj);
		return *obj;
		}
	User::Leave(KErrNotFound);
	return *(CIoObject*)1; // To keep the compiler happy.
	}
	
const TIoConfig& CIoServer::Config()
	{
	return iConfig;
	}
	
void CIoServer::PersistentConsoleAddL(const TDesC16& aName, const CIoPersistentConsole& aCons)
	{
	if (iPersistentConsoleNames.Find(aName)!=NULL)
		{
		User::Leave(KErrAlreadyExists);
		}
	iPersistentConsoleNames.InsertL(&aName, &aCons);
	}
	
void CIoServer::PersistentConsoleRemove(const TDesC16& aName, const CIoPersistentConsole& aCons)
	{
	if (iPersistentConsoleNames.Find(aName) == &aCons)
		{
		iPersistentConsoleNames.Remove(&aName);
		}
	}

TInt CIoServer::RunError(TInt aError)
	{
	if (aError == KErrBadDescriptor)
		{
		PanicClient(Message(), EPanicBadDescriptor);
		}
	else if (aError == KErrBadHandle)
		{
		PanicClient(Message(), EPanicBadHandle);
		}
	else
		{
		LOG(CIoLog::LogCompletion(Message(), aError));
		Complete(Message(), aError);
		}

	ReStart();
	return KErrNone;
	}


//
// Statics.
//

void PanicClient(const RMsg& aMessage, TIoPanicReason aReason)
	{
	aMessage.Panic(KIoServerName, aReason);
	}

#ifdef EKA2
static void RunServerL()
#else
static void RunServerL(TServerStart& aStart)
#endif
	{
	CActiveScheduler* scheduler = new(ELeave) CActiveScheduler;
	CleanupStack::PushL(scheduler);
	CActiveScheduler::Install(scheduler);

	CIoServer::NewLC();
#ifdef EKA2
	User::RenameThread(KIoServerName);
	RProcess::Rendezvous(KErrNone);
#else
	User::LeaveIfError(RThread().Rename(KIoServerName));
	aStart.SignalL();
#endif
	CActiveScheduler::Start();
	CleanupStack::PopAndDestroy(2, scheduler);
	}

#ifdef EKA2
static TInt RunServer()
#else
static TInt RunServer(TServerStart& aStart)
#endif
	{
	__UHEAP_MARK;
	CTrapCleanup* cleanup=CTrapCleanup::New();
	TInt r = KErrNoMemory;
	if (cleanup)
		{
#ifdef EKA2
		TRAP(r, RunServerL());
#else
		TRAP(r, RunServerL(aStart));
#endif
		delete cleanup;
		}
	__UHEAP_MARKEND;
	return r;
	}


#if defined(__WINS__) && !defined(EKA2)

static TInt ThreadFunction(TAny* aParms)
	{
	return RunServer(*static_cast<TServerStart*>(aParms));
	}

IMPORT_C TInt WinsMain();
EXPORT_C TInt WinsMain()
	{
	return reinterpret_cast<TInt>(&ThreadFunction);
	}

TInt E32Dll(TDllReason)
	{
	return KErrNone;
	}

#else
#ifdef EKA2

TInt E32Main()
	{
	return RunServer();
	}

#else

TInt TServerStart::GetCommand()
	{
	RProcess p;
	if (p.CommandLineLength() != (sizeof(TServerStart) / sizeof(TText)))
		{
		return KErrGeneral;
		}
	TPtr ptr(reinterpret_cast<TText*>(this), 0, (sizeof(TServerStart) / sizeof(TText)));
	p.CommandLine(ptr);
	return KErrNone;
	}

TInt E32Main()
	{
	TServerStart start;
	TInt r = start.GetCommand();
	if (r == KErrNone)
		{
		r = RunServer(start);
		}
	return r;
	}

#endif
#endif

#ifndef EKA2
const TAny* MessagePtr(const RMsg& aMessage, TInt aParam)
	{
	const TAny* ptr;
	switch (aParam)
		{
		case 0:
			{
			ptr = aMessage.Ptr0();
			break;
			}
		case 1:
			{
			ptr = aMessage.Ptr1();
			break;
			}
		case 2:
			{
			ptr = aMessage.Ptr2();
			break;
			}
		case 3:
			{
			ptr = aMessage.Ptr3();
			break;
			}
		default:
			{
			ASSERT(EFalse);
			ptr = NULL;
			}
		}
	return ptr;
	}
#endif
	
TInt DesLengthL(const RMsg& aMessage, TInt aParam)	
	{
#ifdef EKA2
	return aMessage.GetDesLengthL(aParam);
#else
	return aMessage.Client().GetDesLength(MessagePtr(aMessage, aParam));
#endif
	}
	
TInt MaxDesLengthL(const RMsg& aMessage, TInt aParam)
	{
#ifdef EKA2
	return aMessage.GetDesMaxLengthL(aParam);
#else
	return aMessage.Client().GetDesMaxLength(MessagePtr(aMessage, aParam));
#endif
	}

void MessageReadL(const RMsg& aMessage, TInt aParam, TDes8& aDes)
	{
#ifdef EKA2
	aMessage.ReadL(aParam, aDes);
#else
	aMessage.ReadL(MessagePtr(aMessage, aParam), aDes);
#endif
	}

void MessageReadL(const RMsg& aMessage, TInt aParam, TDes8& aDes, TInt aOffset)
	{
#ifdef EKA2
	aMessage.ReadL(aParam, aDes, aOffset);
#else
	aMessage.ReadL(MessagePtr(aMessage, aParam), aDes, aOffset);
#endif
	}

void MessageReadL(const RMsg& aMessage, TInt aParam, TDes16& aDes)
	{
#ifdef EKA2
	aMessage.ReadL(aParam, aDes);
#else
	aMessage.ReadL(MessagePtr(aMessage, aParam), aDes);
#endif
	}

void MessageReadL(const RMsg& aMessage, TInt aParam, TDes16& aDes, TInt aOffset)
	{
#ifdef EKA2
	aMessage.ReadL(aParam, aDes, aOffset);
#else
	aMessage.ReadL(MessagePtr(aMessage, aParam), aDes, aOffset);
#endif
	}

void MessageWriteL(const RMsg& aMessage, TInt aParam, const TDesC8& aDes)
	{
#ifdef EKA2
	aMessage.WriteL(aParam, aDes);
#else
	aMessage.WriteL(MessagePtr(aMessage, aParam), aDes);
#endif
	}

void MessageWriteL(const RMsg& aMessage, TInt aParam, const TDesC16& aDes)
	{
#ifdef EKA2
	aMessage.WriteL(aParam, aDes);
#else
	aMessage.WriteL(MessagePtr(aMessage, aParam), aDes);
#endif
	}
	
TInt MessageWrite(const RMsg& aMessage, TInt aParam, const TDesC8& aDes)
	{
#ifdef EKA2
	return aMessage.Write(aParam, aDes);
#else
	return aMessage.Write(MessagePtr(aMessage, aParam), aDes);
#endif
	}

TInt MessageWrite(const RMsg& aMessage, TInt aParam, const TDesC16& aDes)
	{
#ifdef EKA2
	return aMessage.Write(aParam, aDes);
#else
	return aMessage.Write(MessagePtr(aMessage, aParam), aDes);
#endif
	}

TBool MessagePending(const RMsg& aMessage)
	{
#ifdef EKA2
	return !aMessage.IsNull();
#else
	return aMessage != RMessagePtr();
#endif
	}

TThreadId ClientThreadIdL(const RMsg& aMessage)
	{
	TThreadId clientThreadId;
#ifdef EKA2
	RThread clientThread;
	aMessage.ClientL(clientThread);
	clientThreadId = clientThread.Id();
	clientThread.Close();
#else
	clientThreadId = aMessage.Client().Id();
#endif
	return clientThreadId;
	}

TFullName ClientNameL(const RMsg& aMessage)
	{
#ifdef EKA2
	RThread clientThread;
	aMessage.ClientL(clientThread);
	TFullName clientName(clientThread.FullName());
	clientThread.Close();
	return clientName;
#else
	return aMessage.Client().FullName();
#endif
	}

void Complete(const RMsg& aMessage, TInt aError)
	{
	LOG(CIoLog::LogCompletion(aMessage, aError));
	aMessage.Complete(aError);
	}

void CompleteIfPending(const RMsg& aMessage, TInt aError)
	{
	if (MessagePending(aMessage))
		{
		Complete(aMessage, aError);
		}
	}
