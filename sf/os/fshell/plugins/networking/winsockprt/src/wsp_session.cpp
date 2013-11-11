// wsp_session.cpp
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


#include "wsp_session.h"
#include "wsp_factory.h"
#include "wsp_request.h"
#include "wsp_panic.h"
#include "wsp_log.h"


//
// Constants.
//

_LIT(KWspThreadName, "WinSockPrtThread");
_LIT(KWspThreadNameWhilstWaitingForDeath, "WinSockPrtThread_WaitingForDeath_%Ld");
_LIT(KWspThreadStartSemaphoreName, "WinSockPrtThreadStartSemaphore");
const TInt KWspThreadStackSize = KDefaultStackSize;
const TInt KWspThreadMinHeapSize = 0x4000;		// 16 kb
const TInt KWspThreadMaxHeapSize = 0x40000;		// 256 kb


//
// RWin32Session.
//

TInt RWin32Session::Open()
	{
	return CreateThread();
	}

void RWin32Session::Close()
	{
	if (iThread.Handle())
		{
		iWin32Message.Set(CWin32Session::EClose);
		iRequest.MakeRequest(iWin32Message);
		iThread.Close();
		}
	}

TInt RWin32Session::MakeRequest(TWin32Message& aMessage)
	{
	return iRequest.MakeRequest(aMessage);
	}

TInt RWin32Session::CreateThread()
	{
	WSP_LOG(WspLog::Write(_L("RWin32Session::CreateThread")));
#ifdef _DEBUG
	TFindThread findThread(KWspThreadName);
	TFullName name;
	__ASSERT_DEBUG(findThread.Next(name)==KErrNotFound, Panic(EWinSockPrtThreadAlreadyExists));
#endif
	
	RSemaphore threadStartSemaphore;
	TInt err = threadStartSemaphore.CreateGlobal(KWspThreadStartSemaphoreName, 0, EOwnerThread);
	if (err)
		{
		return err;
		}

	err = iThread.Create(KWspThreadName, WinSockPrtThreadStart, KWspThreadStackSize, KWspThreadMinHeapSize, KWspThreadMaxHeapSize, &iRequest, EOwnerThread);
	if (err == KErrNone)
		{
		err = iRequest.DuplicateParentThreadHandle(iThread);
		if (err)
			{
			iThread.Close();
			threadStartSemaphore.Close();
			return err;
			}
		//iThread.SetPriority(EPriorityAbsoluteVeryLow); // Hack to prevent the thread from blocking all threads lower than the default priority. Should really be using a raw Win32 thread rather than a Symbian one.
		iThread.Resume();
		threadStartSemaphore.Wait();
		threadStartSemaphore.Close();
		}
	else
		{
		threadStartSemaphore.Close();
		}
	return err;
	}


//
// Thread start functions.
//

GLDEF_C TInt WinSockPrtThreadStart(TAny* aPtr)
	{
	__UHEAP_MARK;
	CTrapCleanup* trapCleanup = CTrapCleanup::New();
	if (trapCleanup == NULL)
		{
		return KErrNoMemory;
		}
	TRAPD(err, InitialiseAndRunThreadL(aPtr));
	WSP_LOG(WspLog::Close());
	delete trapCleanup;
	__UHEAP_MARKEND;
	return err;
	}

GLDEF_C void InitialiseAndRunThreadL(TAny* aPtr)
	{
	WSP_LOG(User::LeaveIfError(WspLog::Open()));
	CWin32Scheduler* win32Scheduler = CWin32Scheduler::NewL();
	CleanupStack::PushL(win32Scheduler);
	TWin32Request* request = static_cast<TWin32Request*>(aPtr);
	CWin32Factory* win32Factory = CWin32Factory::NewL(*request, *win32Scheduler);
	CleanupStack::PushL(win32Factory);

	RSemaphore threadStartSemaphore;
	User::LeaveIfError(threadStartSemaphore.OpenGlobal(KWspThreadStartSemaphoreName, EOwnerThread));
	threadStartSemaphore.Signal();
	threadStartSemaphore.Close();
	
	win32Scheduler->Start();

	CleanupStack::PopAndDestroy(2, win32Scheduler);
	}


//
// CWin32Session.
//

void CWin32Session::AddSubSessionL(CWin32SubSession* aSubSession)
	{
	TInt subSessionHandle = reinterpret_cast<TInt>(aSubSession);
	TPckg<TInt> subSessionHandlePckg(subSessionHandle);
	iRequest.Message().WriteBuffer().Copy(subSessionHandlePckg);
	CleanupStack::PushL(aSubSession);
	User::LeaveIfError(iSubSessions.Append(aSubSession));
	CleanupStack::Pop(aSubSession);
	}

CWin32Session::~CWin32Session()
	{
	__ASSERT_DEBUG(iSubSessions.Count() == 0, Panic(EWinSockPrtSubSessionListNotEmpty));
	iSubSessions.Close();
	}

CWin32Session::CWin32Session(TWin32Request& aRequest, CWin32Scheduler& aScheduler)
	: CWin32ActiveObject(aScheduler), iRequest(aRequest)
	{
	}

void CWin32Session::ConstructL()
	{
	CWin32ActiveObject::ConstructL();
	iRequest.SetEventHandle(iEvent);
	}

void CWin32Session::Run()
	{
	TInt err = KErrNone;

	CWin32SubSession* subSession = reinterpret_cast<CWin32SubSession*>(iRequest.SubSession());
	TWin32Message& message = iRequest.Message();
	if (subSession == NULL)
		{
		switch (message.OppCode())
			{
			case EClose:
				{
				Close();
				break;
				}
			case ECloseSubSession:
				{
				CloseSubSession();
				break;
				}
			default:
				{
				// Pass on to sub-class.
				TRAP(err, ServiceL(message));
				}
			}
		}
	else
		{
		TRAP(err, subSession->ServiceL(message));
		}

	iRequest.Requested(err);
	}

void CWin32Session::Close()
	{
	// This thread could take a while to die, so give it a different name so that subsequent client attempt to create another one don't fail with KErrInUse.
	TTime time;
	time.UniversalTime();
	const TInt64& int64 = time.Int64();
	TBuf<64> newThreadName;
	newThreadName.Format(KWspThreadNameWhilstWaitingForDeath(), int64);
	User::RenameThread(newThreadName); // Throw away error code.
	iScheduler.Stop();
	}

void CWin32Session::CloseSubSession()
	{
	TInt handle;
	TPckg<TInt> handlePckg(handle);
	handlePckg.Copy(iRequest.Message().ReadBuffer());
	CWin32SubSession* subSessionToClose = reinterpret_cast<CWin32SubSession*>(handle);
	TInt pos = iSubSessions.Find(subSessionToClose);
	__ASSERT_DEBUG(pos >= 0, Panic(EWinSockPrtUnableToFindSubSession));
	delete subSessionToClose;
	iSubSessions.Remove(pos);
	}
