// NotifierProxy.cpp
// 
// Copyright (c) 2010 Accenture. All rights reserved.
// This component and the accompanying materials are made available
// under the terms of the "Eclipse Public License v1.0"
// which accompanies this distribution, and is available
// at the URL "http://www.eclipse.org/legal/epl-v10.html".
// 
// Initial Contributors:
// Accenture - Initial contribution
//
#include <fshell/common.mmh>
#include <e32notif.h>
#include <e32ver.h>

#ifndef FSHELL_WSERV_SUPPORT

EXPORT_C TInt StartNotifierProxyThreadFunction(TAny*)
	{
	return KErrNotSupported;
	}

EXPORT_C TInt ShutdownProxyNotifier()
	{
	return KErrNotSupported;
	}

EXPORT_C TBool NotifierProxyIsRunning()
	{
	return EFalse;
	}

#else

#include "NotifierProxy.h"
#include <fshell/clogger.h>
#include "utils.h"

_LIT(KRealNotifierServerName,   "RealNtfyr");
_LIT8(KRealNotifierServerName8, "RealNtfyr");

#define LeaveIfErr(_err, _args...) \
	do \
		{ \
		if (_err < 0) \
			{ \
			RClogger::Slog(_args); \
			User::Leave(_err); \
			} \
		} \
		while (0)

const TInt KShutdownProxy = 1000;
const TInt KPingProxy = 1001;
const TInt KInfoPrintTime = 2000000; // 2 seconds

TInt RRealNotifier::Connect()
	{
	return CreateSession(KRealNotifierServerName, TVersion(KNotifierMajorVersionNumber,KNotifierMinorVersionNumber,KNotifierBuildVersionNumber),-1);
	}

CNotifierProxyServer::CNotifierProxyServer()
	: CServer2(0, ESharableSessions)
	{
	}

void CNotifierProxyServer::ConstructL()
	{
	//__DEBUGGER();
	SetupWindowL();
	TFontSpec spec(_L("Roman"), 12);
	LeaveIfErr(iScreenDevice->GetNearestFontInPixels(iFont, spec), _L("Couldn't find a suitable font"));
	iPeriodic = CPeriodic::NewL(CActive::EPriorityStandard);
	iText.ReserveL(8);
	iUndertaker = new(ELeave) CPanicDialogWatcher(*this);
	iUndertaker->ConstructL();

	StartL(KRealNotifierServerName); // We start with the name of what we're going to rename the real notifier to, this way we ensure that the following code can only be run by one thread at once

	RMemoryAccess::LoadDriver();
	LeaveIfErr(iMemAccess.Open(), _L("Couldn't open mem access"));

	TServerKernelInfoBuf buf;
	TInt err = iMemAccess.GetObjectInfo(EServer, _L("!Notifier"), buf);
	User::LeaveIfError(err);
	TUint8* realServer = buf().iAddressOfKernelObject;
	err = iMemAccess.GetObjectInfo(EServer, KRealNotifierServerName, buf);
	User::LeaveIfError(err);
	TUint8* myServer = buf().iAddressOfKernelObject;

	// Should really have some kind of swap operation here...
	iMemAccess.InPlaceObjectRename(EServer, realServer, KRealNotifierServerName8);
	iMemAccess.InPlaceObjectRename(EServer, myServer, _L8("!Notifier"));
	iProxying = ETrue;
	}

CNotifierProxyServer::~CNotifierProxyServer()
	{
	delete iRedrawer;
	//delete iAnimationTimer;
	//delete iBitmap;
	//delete iDsa;
	delete iScreenDevice;
	delete iGc;
	iWin.Close();
	iWg.Close();
	iWs.Close();
	delete iPeriodic;
	iText.ResetAndDestroy();
	delete iUndertaker;

	if (iProxying)
		{
		TServerKernelInfoBuf buf;
		TInt err = iMemAccess.GetObjectInfo(EServer, KRealNotifierServerName, buf);
		if (!err)
			{
			TUint8* realServer = buf().iAddressOfKernelObject;
			iMemAccess.InPlaceObjectRename(EServer, realServer, _L8("!Notifier"));
			// Temporarily both us and the real server will have name !Notifier. When we finish destructing, we'll clean up and the system should be back in the state it was
			}
		}
	iMemAccess.Close();
	RMemoryAccess::CloseDriver();
	}

CSession2* CNotifierProxyServer::NewSessionL(const TVersion& /*aVersion*/, const RMessage2& /*aMessage*/) const
	{
	CNotifierProxySession* res = new(ELeave) CNotifierProxySession();
	CleanupStack::PushL(res);
	res->ConstructL();
	CleanupStack::Pop(res);
	return res;
	}

void CNotifierProxySession::ConstructL()
	{
	User::LeaveIfError(iRealNotifier.Connect());
	}

CNotifierProxyServer& CNotifierProxySession::Server()
	{
	return *const_cast<CNotifierProxyServer*>(static_cast<const CNotifierProxyServer*>(CSession2::Server()));
	}

const CNotifierProxyServer& CNotifierProxySession::Server() const
	{
	return *static_cast<const CNotifierProxyServer*>(CSession2::Server());
	}

void CNotifierProxySession::ServiceL(const RMessage2 &aMessage)
	{
	switch (aMessage.Function())
		{
		case ENotifierInfoPrint:
			{
			//__DEBUGGER();
			RBuf buf;
			CleanupClosePushL(buf);
			buf.CreateL(aMessage.GetDesLengthL(0));
			aMessage.ReadL(0, buf);
			RThread client;
			TInt err = aMessage.Client(client);
			TUint clientId = 0;
			if (!err) clientId = client.Id();
			client.Close();
			Server().InfoPrint(buf, clientId);
			CleanupStack::PopAndDestroy(&buf);
			aMessage.Complete(KErrNone);
			break;
			}
		case EStartNotifier:
			{
			TUid notifierUid = TUid::Uid(aMessage.Int0());
			RBuf8 buf, responseBuf;
			CleanupClosePushL(buf);
			buf.CreateL(aMessage.GetDesLengthL(1));
			aMessage.ReadL(1, buf);
			TInt res = KErrNone;
			if (aMessage.Ptr2())
				{
				CleanupClosePushL(responseBuf);
				responseBuf.CreateL(aMessage.GetDesMaxLengthL(2));
				res = iRealNotifier.StartNotifier(notifierUid, buf, responseBuf);
				if (res == KErrNone || responseBuf.Length())
					{
					aMessage.WriteL(2, responseBuf);
					}
				CleanupStack::PopAndDestroy(&responseBuf);
				}
			else
				{
				res = iRealNotifier.StartNotifier(notifierUid, buf);
				}
			CleanupStack::PopAndDestroy(&buf);
			aMessage.Complete(res);
			break;
			}
		case EStartNotifierFromSpecifiedDll:
		case EStartNotifierFromSpecifiedDllAndGetResponse:
			aMessage.Complete(KErrNotSupported); // This is what the docs say - god knows why the client bothers to pass it through
			break;
		case ECancelNotifier:
			{
			TUid notifierUid = TUid::Uid(aMessage.Int0());
			TInt res = iRealNotifier.CancelNotifier(notifierUid);
			aMessage.Complete(res);
			break;
			}
		case EUpdateNotifier:
			{
			TUid notifierUid = TUid::Uid(aMessage.Int0());
			RBuf8 buf, responseBuf;
			CleanupClosePushL(buf);
			buf.CreateL(aMessage.GetDesLengthL(1));
			aMessage.ReadL(1, buf);
			CleanupClosePushL(responseBuf);
			responseBuf.CreateL(aMessage.GetDesMaxLengthL(2));
			TInt res = iRealNotifier.UpdateNotifier(notifierUid, buf, responseBuf);
			if (res == KErrNone || responseBuf.Length())
				{
				aMessage.WriteL(2, responseBuf);
				}
			CleanupStack::PopAndDestroy(&responseBuf);
			CleanupStack::PopAndDestroy(&buf);
			aMessage.Complete(res);
			break;
			}
		case EUpdateNotifierAndGetResponse:
		case EStartNotifierAndGetResponse:
		case ENotifierNotify:
			{
			CAsyncWaiter* waiter = new(ELeave) CAsyncWaiter(iRealNotifier, aMessage);
			waiter->ForwardMessageL();
			break;
			}
		case ENotifierNotifyCancel:
			iRealNotifier.NotifyCancel();
			aMessage.Complete(KErrNone);
			break;
		case KShutdownProxy:
			CActiveScheduler::Stop();
			break;
		case KPingProxy:
			aMessage.Complete(KErrNone);
			break;
		default:
			__DEBUGGER();
			aMessage.Complete(KErrNotSupported);
		}
	}

void CNotifierProxySession::Disconnect(const RMessage2 &aMessage)
	{
	iRealNotifier.Close();
	//TODO Need to wait for any CAsyncWaiters to be completed? If so, do it here and defer the super call until that time
	CSession2::Disconnect(aMessage);
	}

CAsyncWaiter::CAsyncWaiter(RRealNotifier& aRealNotifier, const RMessage2& aMessage)
	: CActive(CActive::EPriorityStandard), iRealNotifier(aRealNotifier), iMsg(aMessage)
	{
	CActiveScheduler::Add(this);
	}

CAsyncWaiter::~CAsyncWaiter()
	{
	iBuf.Close();
	iWideBuf.Close();
	iResponseBuf.Close();
	}

void CAsyncWaiter::ForwardMessageL()
	{
	switch (iMsg.Function())
		{
		case EUpdateNotifierAndGetResponse:
			{
			TUid notifierUid = TUid::Uid(iMsg.Int0());
			iBuf.CreateL(iMsg.GetDesLengthL(1));
			iMsg.ReadL(1, iBuf);
			iResponseBuf.CreateL(iMsg.GetDesMaxLengthL(2));
			iRealNotifier.UpdateNotifierAndGetResponse(iStatus, notifierUid, iBuf, iResponseBuf);
			SetActive();
			break;
			}
		case EStartNotifierAndGetResponse:
			{
			TUid notifierUid = TUid::Uid(iMsg.Int0());
			iBuf.CreateL(iMsg.GetDesLengthL(1));
			iMsg.ReadL(1, iBuf);
			iResponseBuf.CreateL(iMsg.GetDesMaxLengthL(2));
			iRealNotifier.StartNotifierAndGetResponse(iStatus, notifierUid, iBuf, iResponseBuf);
			SetActive();
			break;
			}
		case ENotifierNotify:
			{
			// iWideBuf is the combined buffer
			iWideBuf.CreateL(iMsg.GetDesLengthL(1));
			iMsg.ReadL(1, iWideBuf);
			// iResponseBuf is the button val
			iResponseBuf.CreateMaxL(sizeof(TInt));
			TInt comboLineLen = iMsg.Int2();
			TInt comboButLen = iMsg.Int3();
			const TUint16* ptr = iWideBuf.Ptr();
			iTempPtrs[0].Set(ptr, comboLineLen >> 16);
			ptr += iTempPtrs[0].Length();
			iTempPtrs[1].Set(ptr, comboLineLen & 0xFFFF);
			ptr += iTempPtrs[1].Length();
			iTempPtrs[2].Set(ptr, comboButLen >> 16);
			ptr += iTempPtrs[2].Length();
			iTempPtrs[3].Set(ptr, comboButLen & 0xFFFF);
			
			iRealNotifier.Notify(iTempPtrs[0], iTempPtrs[1], iTempPtrs[2], iTempPtrs[3], *(TInt*)iResponseBuf.Ptr(), iStatus);
			SetActive();
			break;
			}
		default:
			__DEBUGGER();
		}
	}

void CAsyncWaiter::RunL()
	{
	TInt res = iStatus.Int();
	switch (iMsg.Function())
		{
		case EUpdateNotifierAndGetResponse:
		case EStartNotifierAndGetResponse:
			if (res == KErrNone || iResponseBuf.Length())
				{
				TInt err = iMsg.Write(2, iResponseBuf);
				if (err)
					{
					res = err;
					}
				}
			iMsg.Complete(res);
			break;
		case ENotifierNotify:
			{
			TInt err = iMsg.Write(0, iResponseBuf);
			if (err) res = err;
			iMsg.Complete(res);
			break;
			}
		default:
			__DEBUGGER();
			User::Leave(KErrNotSupported);
		}
	delete this; // Our work here is done
	}

void CAsyncWaiter::DoCancel()
	{
	// We never call Cancel on our waiters
	}

void MainL()
	{
	CActiveScheduler* scheduler = new(ELeave) CActiveScheduler;
	CleanupStack::PushL(scheduler);
	CActiveScheduler::Install(scheduler);
	CNotifierProxyServer* server = new(ELeave) CNotifierProxyServer;
	CleanupStack::PushL(server);
	server->ConstructL();
	CActiveScheduler::Start();
	CleanupStack::PopAndDestroy(2, scheduler); // server, scheduler
	}

EXPORT_C TInt StartNotifierProxyThreadFunction(TAny*)
	{
	TInt err = KErrNoMemory;
	CTrapCleanup* cleanup = CTrapCleanup::New();
	if (cleanup)
		{
		TRAP(err, MainL());
		delete cleanup;
		}
	return err;
	}

class RDebugNotifier : public RNotifier
	{
public:
	TInt ShutdownProxy()
		{
		return SendReceive(KShutdownProxy, TIpcArgs());
		}
	TInt PingProxy()
		{
		return SendReceive(KPingProxy, TIpcArgs());
		}
	};

EXPORT_C TInt ShutdownProxyNotifier()
	{
	RDebugNotifier notifier;
	TInt err = notifier.Connect();
	if (err == KErrNotFound)
		{
		// Oh dear, !Notifier isn't running. Meaning we renamed it then crashed, probably. Try renaming the real one
		RMemoryAccess::LoadDriver();
		RMemoryAccess mem;
		err = mem.Open();
		if (err) return err;
		TServerKernelInfoBuf buf;
		TInt err = mem.GetObjectInfo(EServer, KRealNotifierServerName, buf);
		if (!err)
			{
			TUint8* realServer = buf().iAddressOfKernelObject;
			mem.InPlaceObjectRename(EServer, realServer, _L8("!Notifier"));
			}
		mem.Close();
		return err;
		}

	err = notifier.ShutdownProxy();
	if (err == KErrServerTerminated) err = KErrNone; // It's expected to get KErrServerTerminated, because we deliberately don't complete the message. This way the client is more likely to get the completion once the server has actually gone, and not slightly before
	notifier.Close();

	return err;
	}

EXPORT_C TBool NotifierProxyIsRunning()
	{
	RDebugNotifier notifier;
	TInt err = notifier.Connect();
	if (err) return EFalse; // Not even original notifier is running?!
	err = notifier.PingProxy();
	notifier.Close();
	return err == KErrNone; // The real proxy will return KErrNotSupported in this scenario
	}


void CNotifierProxyServer::SetupWindowL()
	{
	//__DEBUGGER();
	LeaveIfErr(iWs.Connect(), _L("Couldn't connect to windowserver"));
	//iWs.SetAutoFlush(ETrue);
	iRedrawer = new(ELeave) CRedrawer(*this);
	iRedrawer->Go();

	iScreenDevice = new(ELeave) CWsScreenDevice(iWs);
	LeaveIfErr(iScreenDevice->Construct(), _L("Couldn't construct screen device")); // User default screen number
	LeaveIfErr(iScreenDevice->CreateContext(iGc), _L("Couldn't create gc"));
	iWg = RWindowGroup(iWs);
	LeaveIfErr(iWg.Construct(2, EFalse), _L("Couldn't construct windowgroup"));
	iWin = RWindow(iWs);
	LeaveIfErr(iWin.Construct(iWg, (TUint32)this), _L("Couldn't construct window"));

	iWin.SetBackgroundColor(KRgbBlack);
	iWg.SetOrdinalPosition(0, 1000); // 1000 is ECoeWinPriorityAlwaysAtFront, but I don't want the dependancy on a cone header
	TRect rect(TPoint(), TSize(iScreenDevice->SizeInPixels().iWidth, 50));
	iWin.SetExtent(rect.iTl, rect.Size());
	iWin.SetVisible(EFalse);
	iWin.Activate();
	iWs.Flush();
	
	//iBitmap = new(ELeave) CFbsBitmap;
	//LeaveIfErr(iBitmap->Create(iScreenDevice->SizeInPixels(), EColor16MU), _L("Couldn't create bitmap"));
	//iDsa = CDirectScreenAccess::NewL(iWs, *iScreenDevice, iWin, *this);
	}

void CNotifierProxyServer::DoWservInitiatedRedraw(const TRect& /*aRect*/)
	{
	//__DEBUGGER();
	// This code based on CCoeControl::HandleRedrawEvent
	//iWin.BeginRedraw(aRect);
	iWin.BeginRedraw(TRect(TPoint(), iWin.Size()));
	iGc->Activate(iWin);

	iGc->UseFont(iFont);
	iGc->SetPenColor(KRgbGreen);
	iGc->SetBrushStyle(CGraphicsContext::ESolidBrush);
	iGc->SetBrushColor(KRgbBlack);
	for (TInt i = 0; i < iText.Count(); i++)
		{
		TPoint drawPoint(0, iFont->FontMaxAscent() + i * iFont->FontLineGap());
		if (drawPoint.iY > iWin.Size().iHeight)
			{
			// We've more to draw than we have room. Bail.
			break;
			}
		//iGc->DrawText(iText[i]->iText, drawPoint);
		iGc->DrawText(iText[i]->iText, BoxForLine(i), iFont->FontMaxAscent());
		}
	iGc->DiscardFont();
	iGc->Deactivate();
	iWin.EndRedraw();
	}

TRect CNotifierProxyServer::BoxForLine(TInt aLineIndex)
	{
	TRect textBox = TRect(0, aLineIndex * iFont->FontLineGap(), iWin.Size().iWidth, (aLineIndex+1) * iFont->FontLineGap());
	return textBox;
	}

RWsSession& CNotifierProxyServer::WsSession()
	{
	return iWs;
	}

void CNotifierProxyServer::InfoPrint(const TDesC& aText, TUint aClientId)
	{
	TPtrC text(aText);
	TInt newline = aText.Locate('\n');
	if (newline != KErrNotFound)
		{
		// UIQ supported 2-line infoprints so some code puts a newline in - if so split it up and treat as two calls
		TPtrC first = aText.Left(newline);
		TPtrC second = aText.Mid(newline+1);
		InfoPrint(first, aClientId);
		//InfoPrint(second, aClientId);
		text.Set(second); // Drop through at this point - prevents recursing if there are many newlines (also stops more than one newline from being recognised)
		}

	CLine* line = new CLine(text, aClientId);
	if (!line) return; // OOM

	// Check if client should be getting frequent flyer miles, if so limit it
	for (TInt i = iText.Count() - 1; i >= 0; i--)
		{
		CLine* line = iText[i];
		if (aClientId && aClientId == line->iClientId && text.Left(6) == line->iText.Left(6))
			{
			iText.Remove(i);
			// And restart timer if the one we've just removed was at index zero, ie about to be removed anyway (otherwise timer will remove the new one sooner at the time when the old one was due)
			if (i == 0) iPeriodic->Cancel(); // (cancelling it here will cause it to be restarted in DisplayAndStartTimer)
			}
		}

	TInt err = iText.Append(line);
	if (err)
		{
		delete line;
		}
	else
		{
		RecalculateWindow();
		iWin.Invalidate();
		DisplayAndStartTimer();
		iWs.Flush();
		}
	}

CRedrawer::CRedrawer(CNotifierProxyServer& aServer) 
	: CActive(CActive::EPriorityStandard), iServer(aServer)
	{
	CActiveScheduler::Add(this);
	}

void CRedrawer::Go()
	{
	if (!IsActive())
		{
		iServer.WsSession().RedrawReady(&iStatus);
		SetActive();
		}
	}

void CRedrawer::RunL()
	{
	TWsRedrawEvent redrawEvent;
	iServer.WsSession().GetRedraw(redrawEvent);
	TRect rect=redrawEvent.Rect();
	
	iServer.DoWservInitiatedRedraw(rect);
	Go();
	iServer.WsSession().Flush();
	}

void CRedrawer::DoCancel()
	{
	iServer.WsSession().RedrawReadyCancel();
	}

CRedrawer::~CRedrawer()
	{
	Cancel();
	}

void CNotifierProxyServer::RecalculateWindow()
	{
	// Grow or shrink the window if needed to display all the text we have
	TInt numLines = iText.Count();
	
	const TInt KMaxWindowHeight = 200;
	TInt newHeight = Min(BoxForLine(0).Height() * numLines, KMaxWindowHeight); // Don't exceed our defined max height
	TInt currentHeight = iWin.Size().iHeight;
	if (newHeight != currentHeight)
		{
		iWin.SetSize(TSize(iWin.Size().iWidth, newHeight));
		}
	}

TInt CNotifierProxyServer::StaticTick(TAny* aSelf)
	{
	static_cast<CNotifierProxyServer*>(aSelf)->Tick();
	return 0;
	}

void CNotifierProxyServer::Tick()
	{
	// Remove 1 line from the top of the infoprint each time the timer fires, if nothing left hide the window
	if (iText.Count())
		{
		iText.Remove(0);
		}

	if (iText.Count() == 0)
		{
		iWin.SetVisible(EFalse);
		iPeriodic->Cancel(); // No need for further ticks
		}
	else
		{
		RecalculateWindow();
		iWin.Invalidate();
		}
	iWs.Flush();
	}
	
void CNotifierProxyServer::DisplayAndStartTimer()
	{
	if (iText.Count())
		{
		iWin.SetVisible(ETrue);
		if (!iPeriodic->IsActive())
			{
			iPeriodic->Start(KInfoPrintTime, KInfoPrintTime, TCallBack(&StaticTick, this));
			}
		}
	}

CLine::CLine(const TDesC& aText, TUint aClientId)
	: iText(aText), iClientId(aClientId)
	{
	//iTimeOfPrint.UniversalTime();
	}

CPanicDialogWatcher::CPanicDialogWatcher(CNotifierProxyServer& aServer)
	: CActive(CActive::EPriorityStandard), iServer(aServer)
	{
	CActiveScheduler::Add(this);
	}

void CPanicDialogWatcher::ConstructL()
	{
	User::LeaveIfError(iUndertaker.Create());
	iUndertaker.Logon(iStatus, iThreadHandle);
	SetActive();
	}

CPanicDialogWatcher::~CPanicDialogWatcher()
	{
	Cancel();
	iUndertaker.Close();
	}

void CPanicDialogWatcher::RunL()
	{
	TInt threadHandle = iThreadHandle;
	iUndertaker.Logon(iStatus, iThreadHandle);
	SetActive();

	RThread thread;
	thread.SetHandle(threadHandle);
	iTempName = thread.FullName();
	PrettyName(EListThread, iTempName);
	if (thread.ExitType() == EExitPanic)
		{
		TExitCategoryName cat = thread.ExitCategory();
		TBuf<256> buf;
		buf.Format(_L("Panic: %S %d from %S"), &cat, thread.ExitReason(), &iTempName);
		iServer.InfoPrint(buf, 0);
		}
	else if (thread.ExitType() == EExitKill)
		{
		TBuf<256> buf;
		buf.Format(_L("Kill %d: Tid %d %S"), thread.ExitReason(), TUint(thread.Id()), &iTempName);
		iServer.InfoPrint(buf, 0);
		}
	else if (thread.ExitType() == EExitTerminate)
		{
		TBuf<256> buf;
		buf.Format(_L("Terminate %d: Tid %d %S"), thread.ExitReason(), TUint(thread.Id()), &iTempName);
		iServer.InfoPrint(buf, 0);
		}
	thread.Close();
	}

void CPanicDialogWatcher::DoCancel()
	{
	iUndertaker.LogonCancel();
	}

#endif
