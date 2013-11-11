// wsp_resolver.cpp
// 
// Copyright (c) 2002 - 2010 Accenture. All rights reserved.
// This component and the accompanying materials are made available
// under the terms of the "Eclipse Public License v1.0"
// which accompanies this distribution, and is available
// at the URL "http://www.eclipse.org/legal/epl-v10.html".
// 
// Initial Contributors:
// Accenture - Initial contribution
//

#include <winsock2.h>
// Prevent e32std.inl from failing to compile with "ambiguous access to overloaded function".
#define memmove _memmove
#define memset _memset
//
#include <e32math.h>
#include <in_sock.h>
#include <utf.h>
#include "wsp_resolver.h"
#include "wsp_session.h"
#include "wsp_factory.h"
#include "wsp_panic.h"
#include "wsp_log.h"


//
// Constants.
//

_LIT(KWspResolverWorkerThreadNameFormat, "WinSockPrtResolverWorkerThread_%x_%d");
const TInt  KWspResolverWorkerThreadNameLength =  64;
const TInt KWspResolverWorkerThreadStackSize = KDefaultStackSize;


//
// RWin32Socket.
//

RWin32Resolver::RWin32Resolver()
	: iNameRecordPckg(NULL, 0, 0)
	{
	}

TInt RWin32Resolver::Open(RWin32Factory& aFactory)
	{
	WSP_LOG(WspLog::Printf(_L("RWin32Resolver::Open: this: 0x%x"), this));
	return CreateSubSession(aFactory, CWin32Factory::ENewResolver);
	}

void RWin32Resolver::Close()
	{
	WSP_LOG(WspLog::Printf(_L("RWin32Resolver::Close: this: 0x%x"), this));
	RWin32SubSession::Close();
	}

TInt RWin32Resolver::GetByName(TNameRecord& aName, TRequestStatus& aStatus)
	{
	WSP_LOG(WspLog::Printf(_L("RWin32Resolver::GetByName: this: 0x%x"), this));
	iNameRecordPckg.Set(reinterpret_cast<TUint8*>(&aName), sizeof(TNameRecord),sizeof(TNameRecord));
	iWin32Message.Set(CWin32Resolver::EGetByName, iNameRecordPckg, aStatus);
	return MakeRequest(iWin32Message);
	}

TInt RWin32Resolver::GetByAddress(TNameRecord& aName, TRequestStatus& aStatus)
	{
	WSP_LOG(WspLog::Printf(_L("RWin32Resolver::GetByAddress: this: 0x%x"), this));
	iNameRecordPckg.Set(reinterpret_cast<TUint8*>(&aName), sizeof(TNameRecord),sizeof(TNameRecord));
	iWin32Message.Set(CWin32Resolver::EGetByAddress, iNameRecordPckg, aStatus);
	return MakeRequest(iWin32Message);
	}

void RWin32Resolver::Cancel()
	{
	WSP_LOG(WspLog::Printf(_L("RWin32Resolver::Cancel: this: 0x%x"), this));
	TWin32Message cancelWin32Message;
	cancelWin32Message.Set(CWin32Resolver::ECancel);
	MakeRequest(cancelWin32Message); // Throw away return code - can't do anything useful with it.
	}


//
// CWin32Resolver.
//

CWin32Resolver* CWin32Resolver::NewL(CWin32Scheduler& aScheduler)
	{
	CWin32Resolver* self = new(ELeave) CWin32Resolver(aScheduler);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;
	}

CWin32Resolver::CWin32Resolver(CWin32Scheduler& aScheduler)
	: CWin32SubSession(aScheduler)
	{
	}

void CWin32Resolver::ConstructL()
	{
	CWin32SubSession::ConstructL();
	}

CWin32Resolver::~CWin32Resolver()
	{
	}

void CWin32Resolver::Run()
	{
	iMessage->Complete(iError);
	iMessage = NULL;
	iWorkerThread.Close();
	}

void CWin32Resolver::ServiceL(TWin32Message& aMessage)
	{
	switch (aMessage.OppCode())
		{
		case EGetByName:
		case EGetByAddress:
			{
			__ASSERT_DEBUG(iMessage == NULL, Panic(EWinSockPrtCWin32ResolverMultipleRequests));
			iMessage = &aMessage;
			CreateWorkerThread();
			break;
			}
		case ECancel:
			{
			Cancel();
			break;
			}
		default:
			{
			__ASSERT_DEBUG(EFalse, Panic(EWinSockPrtInvalidResolverOppCode));
			}
		}
	}

void CWin32Resolver::Cancel()
	{
	if (iWorkerThread.Handle() && (iWorkerThread.ExitType() == EExitPending))
		{
		iWorkerThread.Kill(KErrCancel);
		iMessage->Complete(KErrCancel);
		iMessage = NULL;
		iWorkerThread.Close();
		}
	}

void CWin32Resolver::CreateWorkerThread()
	{
	TBuf<KWspResolverWorkerThreadNameLength> threadName;
	threadName.Format(KWspResolverWorkerThreadNameFormat, this, Math::Rand(iRandSeed));
	TInt err = iWorkerThread.Create(threadName, WorkerThreadStart, KWspResolverWorkerThreadStackSize, NULL, this, EOwnerThread);
	if (err == KErrNone)
		{
		iWorkerThread.Resume();
		}
	else
		{
		iMessage->Complete(err);
		}
	}


//
// CWin32Resolver worker thread methods.
//

TInt CWin32Resolver::WorkerThreadStart(TAny* aPtr)
	{
	CWin32Resolver* self = reinterpret_cast<CWin32Resolver*>(aPtr);

	switch (self->iMessage->OppCode())
		{
		case EGetByName:
			{
			self->GetByName();
			break;
			}
		case EGetByAddress:
			{
			self->GetByAddress();
			break;
			}
		case ECancel:
		default:
			{
			__ASSERT_DEBUG(EFalse, Panic(EWinSockPrtResolverInvalidWorkerThreadRequest));
			}
		}

	return KErrNone;
	}

void CWin32Resolver::GetByName()
	{
	TNameRecord& nameRecord = (TNameRecord&)*iMessage->WriteBuffer().Ptr();
	struct hostent* hp;
	HBufC8* narrowHostNameBuf = HBufC8::New(nameRecord.iName.MaxLength() + 1); // Note, this is one bigger than the size of THostName to allow for a null termination character.
	if (narrowHostNameBuf == NULL)
		{
		iError = KErrNoMemory;
		}
	else
		{
		TPtr8 narrowHostNamePtr(narrowHostNameBuf->Des());
		CnvUtfConverter::ConvertFromUnicodeToUtf8(narrowHostNamePtr, nameRecord.iName);
		hp = gethostbyname(reinterpret_cast<const char*>(narrowHostNamePtr.PtrZ()));
		delete narrowHostNameBuf;
		if (hp)
			{
			TInetAddr& addr = reinterpret_cast<TInetAddr&>(nameRecord.iAddr);
			__ASSERT_DEBUG(hp->h_addrtype == AF_INET, Panic(EWinSockPrtResolverInvalidFamily));
			addr.SetFamily(KAfInet);
			__ASSERT_DEBUG(hp->h_length == 4, Panic(EWinSockPrtResolverInvalidAddressLength));
			TUint32* address = (TUint32*)hp->h_addr_list[0];
			addr.SetAddress(htonl(*address));
			iError = KErrNone;
			}
		else
			{
			iError = KErrNotFound;
			}
		}
	
	if (!SetEvent(iEvent))
		{
		__ASSERT_DEBUG(EFalse, Panic(EWinSockPrtResolverUnexpectedSetEventError));
		}
	}

void CWin32Resolver::GetByAddress()
	{
	TNameRecord& nameRecord = (TNameRecord&)*iMessage->WriteBuffer().Ptr();
	TInetAddr& addr = reinterpret_cast<TInetAddr&>(nameRecord.iAddr);
	struct hostent* hp;
	char addressBuf[4];
	*((TUint32*)addressBuf) = htonl(addr.Address());
	hp = gethostbyaddr(addressBuf, 4, AF_INET);
	if (hp)
		{
		TPtrC8 narrowAddressPtr((TUint8*)hp->h_name, User::StringLength((TUint8*)hp->h_name));
		CnvUtfConverter::ConvertToUnicodeFromUtf8(nameRecord.iName, narrowAddressPtr);
		iError = KErrNone;
		}
	else
		{
		iError = KErrNotFound;
		}
	
	if (!SetEvent(iEvent))
		{
		__ASSERT_DEBUG(EFalse, Panic(EWinSockPrtResolverUnexpectedSetEventError));
		}
	}


//
// CWin32ResolverWrapper.
//

CWin32ResolverWrapper* CWin32ResolverWrapper::NewL(MWin32ResolverObserver& aObserver, RWin32Factory& aWin32Factory)
	{
	CWin32ResolverWrapper* self = new(ELeave) CWin32ResolverWrapper(aObserver);
	CleanupStack::PushL(self);
	self->ConstructL(aWin32Factory);
	CleanupStack::Pop(self);
	return self;
	}

CWin32ResolverWrapper::~CWin32ResolverWrapper()
	{
	Cancel();
	iWin32Resolver.Close();
	}

CWin32ResolverWrapper::CWin32ResolverWrapper(MWin32ResolverObserver& aObserver)
	: CActive(CActive::EPriorityStandard), iObserver(aObserver)
	{
	}

void CWin32ResolverWrapper::ConstructL(RWin32Factory& aWin32Factory)
	{
	User::LeaveIfError(iWin32Resolver.Open(aWin32Factory));
	CActiveScheduler::Add(this);
	}

TInt CWin32ResolverWrapper::GetByName(TNameRecord& aName)
	{
	__ASSERT_DEBUG(!IsActive(), Panic(EWinSockPrtResolverWrapperGetByNameWhilstActive));
	TInt err = iWin32Resolver.GetByName(aName, iStatus);
	if (err == KErrNone)
		{
		SetActive();
		}
	return err;
	}

TInt CWin32ResolverWrapper::GetByAddress(TNameRecord& aName)
	{
	__ASSERT_DEBUG(!IsActive(), Panic(EWinSockPrtResolverWrapperGetByAddressWhilstActive));
	TInt err = iWin32Resolver.GetByAddress(aName, iStatus);
	if (err == KErrNone)
		{
		SetActive();
		}
	return err;
	}

void CWin32ResolverWrapper::DoCancel()
	{
	iWin32Resolver.Cancel();
	}

void CWin32ResolverWrapper::RunL()
	{
	iObserver.HandleWin32ResolverCompletion(iStatus.Int());
	}

TInt CWin32ResolverWrapper::RunError(TInt /*aError*/)
	{
	__ASSERT_DEBUG(EFalse, Panic(EWinSockPrtResolverWrapperUnexpectedRunError));
	return 0;
	}


//
// CWinSockResolver.
//

CWinSockResolver* CWinSockResolver::NewL(RWin32Factory& aFactory)
	{
	CWinSockResolver* self = new(ELeave) CWinSockResolver();
	CleanupStack::PushL(self);
	self->ConstructL(aFactory);
	CleanupStack::Pop(self);
	return self;
	}

CWinSockResolver::~CWinSockResolver()
	{
	delete iResolverWrapper;
	}

void CWinSockResolver::CancelCurrentOperation()
	{
	iResolverWrapper->Cancel();
	}

void CWinSockResolver::GetByName(TNameRecord& aName)
	{
	iResolverWrapper->GetByName(aName);
	}

void CWinSockResolver::GetByAddress(TNameRecord& aName)
	{
	iResolverWrapper->GetByAddress(aName);
	}

void CWinSockResolver::SetHostName(TDes& /*aNameBuf*/)
	{
	// TODO:
	ASSERT(EFalse);
	}

void CWinSockResolver::GetHostName(TDes& /*aNameBuf*/)
	{
	// TODO:
	ASSERT(EFalse);
	}

void CWinSockResolver::HandleWin32ResolverCompletion(TInt aError)
	{
	iNotify->QueryComplete(aError);
	}

CWinSockResolver::CWinSockResolver()
	{
	}

void CWinSockResolver::ConstructL(RWin32Factory& aFactory)
	{
	iResolverWrapper = CWin32ResolverWrapper::NewL(*this, aFactory);
	}
