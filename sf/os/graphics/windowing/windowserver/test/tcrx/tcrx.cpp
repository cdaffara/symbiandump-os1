// Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
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

/**
 @file
 @test
 @internalComponent - Internal Symbian test code
*/

#include <gdi.h>
#include "tcrx.h"
#include "wsredir.h"
#include "wslisten.h"

static TRedirectorInfo TheInfo;

class CTwoWindow: public CBase
	{
public:
	static CTwoWindow* NewL();
	~CTwoWindow();
private:
	void ConstructL();
private:
	RWsSession iWs;
	CWsScreenDevice* iScr;
	CWindowGc* iGc;
	RWindowGroup iGroup;
	RWindow iBg;
	RWindow iFg;
	};

CTwoWindow* CTwoWindow::NewL()
	{
	CTwoWindow* tw = new(ELeave) CTwoWindow;
	CleanupStack::PushL(tw);
	tw->ConstructL();
	CleanupStack::Pop(tw);
	return tw;
	}

CTwoWindow::~CTwoWindow()
	{
	iFg.Close();
	iBg.Close();
	iGroup.Close();
	delete iGc;
	delete iScr;
	iWs.Close();	
	}

void CTwoWindow::ConstructL()
	{
	User::LeaveIfError(iWs.Connect());
	iScr = new(ELeave) CWsScreenDevice(iWs);
	User::LeaveIfError(iScr->Construct());
	User::LeaveIfError(iScr->CreateContext(iGc));
	iGroup = RWindowGroup(iWs);
	User::LeaveIfError(iGroup.Construct(0xc0de,ETrue));
	
	iBg = RWindow(iWs);
	User::LeaveIfError(iBg.Construct(iGroup,0xc0debabe));
	iBg.SetRequiredDisplayMode(EColor64K);
	iBg.Activate();
	iWs.Flush();

	iBg.BeginRedraw();
	iGc->Activate(iBg);
	iGc->SetBrushColor(KRgbGreen);
	iGc->SetBrushStyle(CGraphicsContext::ESolidBrush);
	TRect rect(iScr->SizeInPixels());
	iGc->DrawRect(rect);
	iGc->SetBrushColor(KRgbBlue);
	iGc->DrawEllipse(rect);
	iGc->Deactivate();
	iBg.EndRedraw();
	iWs.Flush();
	
	iFg = RWindow(iWs);
	User::LeaveIfError(iFg.Construct(iGroup,0xc0decafe));
	iFg.SetRequiredDisplayMode(EColor64K);
	iFg.SetTransparencyAlphaChannel();
	iFg.SetBackgroundColor(TRgb(0xff,0x55,0x55,0x80));
	iFg.Activate();
	iWs.Flush();
	}

CClient* CClient::NewL()
	{
	CClient* self = new(ELeave) CClient;
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	
	return self;
	}

CClient::CClient(): CActive(CActive::EPriorityStandard)
	{
	}

void CClient::ConstructL()
	{
	CActiveScheduler::Add(this);
	User::LeaveIfError(iWs.Connect());
	iScreen = new(ELeave) CWsScreenDevice(iWs);
	User::LeaveIfError(iScreen->Construct(0));
	iGroup = RWindowGroup(iWs);
	User::LeaveIfError(iGroup.Construct(0xCAFE, ETrue));
	iGc = new(ELeave) CWindowGc(iScreen);
	User::LeaveIfError(iGc->Construct());
	iWin = new(ELeave) CWindow(this);
	iWin->ConstructL(NULL, EFalse);
	iRedraw = CRedrawHandler::NewL(this);

	iRedir = CWsRedir::NewL();
	iListen = CWsListen::NewL();
		
	MakeRequest();
	}

void CClient::HandleCommand(TInt aCmd)
	{
	switch (aCmd)
		{
		case 1:
		iRedir->Redirect(CWsRedir::EFrontBuffer, ETrue);
		break;

		case 2:
		iRedir->Redirect(CWsRedir::EBackBuffer, ETrue);
		break;

		case 3:
		iRedir->Redirect(CWsRedir::EFrontBuffer, EFalse);
		break;

		case 4:
		iRedir->Redirect(CWsRedir::EBackBuffer, EFalse);
		break;

		case 5:
		iListen->Enable(ETrue);
		break;

		case 6:
		iListen->Enable(EFalse);
		break;

		case 7:
		iRedir->QueryPlugin(TheInfo);
		break;
		
		case 8:
		iBlank.Close();
		iBlank = RBlankWindow(iWs);
		iBlank.Construct(iGroup, 0xbeef);
		iBlank.SetRequiredDisplayMode(EColor64K);
		iBlank.SetColor(KRgbRed);
		iBlank.SetExtent(TPoint(20,20),TSize(100,100));
		iBlank.Activate();
		break;
		
		case 9:
		ClearTwoWindow();
		DrawTwoWindow();
		break;
		
		case 10:
		CActiveScheduler::Stop();
		break;
		}
	}
	
CClient::~CClient()
	{	
	Deque();
	ClearTwoWindow();
	iBlank.Close();
	
	delete iListen;
	delete iRedir;
	delete iWin;
	delete iRedraw;
	delete iGc;
	delete iScreen;
	
	iGroup.Close();
	iWs.Close();
	}

void CClient::DrawTwoWindow()
	{
	TRAP_IGNORE(iTwo = CTwoWindow::NewL());
	}

void CClient::ClearTwoWindow()
	{
	delete iTwo;
	iTwo = NULL;	
	}

void CClient::RunL()
	{
	iWs.GetEvent(iEvent);
	if (iEvent.Type()==EEventKey)
		HandleCommand(iEvent.Key()->iCode-'0');
	MakeRequest();
	}

void CClient::DoCancel()
	{
	iWs.EventReadyCancel();
	}

void CClient::MakeRequest()
	{
	iWs.EventReady(&iStatus);
	SetActive();
	}

CRedrawHandler* CRedrawHandler::NewL(CClient* aClient)
	{
	CRedrawHandler* self=new(ELeave) CRedrawHandler(aClient);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	
	return self;
	}

CRedrawHandler::CRedrawHandler(CClient* aClient) : CActive(CActive::EPriorityStandard), iClient(aClient)
	{
	}

void CRedrawHandler::ConstructL()
	{
	CActiveScheduler::Add(this);
	MakeRequest();
	}

CRedrawHandler::~CRedrawHandler()
	{
	Cancel();
	}

void CRedrawHandler::RunL()
	{
	TWsRedrawEvent event;
	iClient->Ws().GetRedraw(event);
	CWindow* win = (CWindow*)event.Handle();
	if (win) 
		{
		TRect rect=event.Rect();
		iClient->Gc().Activate(win->Window());
		win->Window().BeginRedraw(rect);
		win->Draw(rect);
		win->Window().EndRedraw();
		iClient->Gc().Deactivate();
		}
		
	MakeRequest();
	}

void CRedrawHandler::MakeRequest()
	{
	iClient->Ws().RedrawReady(&iStatus);
	SetActive();
	}

void CRedrawHandler::DoCancel()
	{
	iClient->Ws().RedrawReadyCancel();
	}

CWindow::CWindow(CClient* aClient) : iClient(aClient)
	{
	}

CWindow::~CWindow()
	{
	iWin.Close();
	}

void CWindow::ConstructL(CWindow* aParent, TBool aTransparentFlag)
	{
	iParent = aParent;
	iWin = RWindow(iClient->Ws());
	RWindowTreeNode* node=iParent? &iParent->Window() : (RWindowTreeNode*)&iClient->Group();
	User::LeaveIfError(iWin.Construct(*node, (TUint32)this));
	iWin.SetRequiredDisplayMode(EColor64K);
	if (aTransparentFlag)
		{
		iWin.SetBackgroundColor(TRgb(0,0,0,0x80));
		iWin.SetTransparencyAlphaChannel();
		}
	iWin.SetExtent(TPoint(0,0),TSize(240,240));
	iWin.Activate();
	}

void CWindow::Draw(const TRect& aRect) const
	{
	CWindowGc& gc = iClient->Gc();
	gc.SetBrushStyle(CGraphicsContext::ESolidBrush);
	gc.SetBrushColor(TRgb(0,0,0xff,0xff));	
	gc.DrawRect(TRect(aRect.iTl.iX,aRect.iTl.iY,aRect.iBr.iX,aRect.iBr.iY/2));
	gc.SetBrushColor(TRgb(0,0,0xff,0x80));	
	gc.DrawRect(TRect(aRect.iTl.iX,aRect.iBr.iY/2,aRect.iBr.iX,aRect.iBr.iY));	
	DrawMenu();
	
	gc.DrawWsGraphic(iClient->WsRedir()->Id(),TRect(aRect.iTl.iX,aRect.iBr.iY/2,aRect.iBr.iX,aRect.iBr.iY));
	}

_LIT(KMenu1, "1. Redirect screen");
_LIT(KMenu2, "2. Redirect flickerfree");
_LIT(KMenu3, "3. Restore screen");
_LIT(KMenu4, "4. Restore flickerfree");
_LIT(KMenu5, "5. Register event listener");
_LIT(KMenu6, "6. Unregister listener");
_LIT(KMenu7, "7. Test send receive msg");
_LIT(KMenu8, "8. Test blank window");
_LIT(KMenu9, "9. Test two window");
_LIT(KMenuA, "A. Exit");

const TBufC<32> TheMenu[] = 
	{
	(const TDesC&)KMenu1,
	(const TDesC&)KMenu2,
	(const TDesC&)KMenu3,
	(const TDesC&)KMenu4,
	(const TDesC&)KMenu5,
	(const TDesC&)KMenu6,
	(const TDesC&)KMenu7,
	(const TDesC&)KMenu8,
	(const TDesC&)KMenu9,
	(const TDesC&)KMenuA
	};
	
_LIT(KTypeFace, "DejaVu Sans Condensed");

void CWindow::DrawMenu() const
	{
	CWsScreenDevice* scr = iClient->Screen();
	CFont* font = NULL;
	TFontSpec fs(KTypeFace, 15);
	scr->GetNearestFontInPixels(font, fs);
	CWindowGc& gc = iClient->Gc();
	gc.UseFont(font);
	TInt h = font->HeightInPixels();
	gc.SetPenColor(KRgbYellow);
	TInt nMenu = sizeof(TheMenu)/sizeof(TheMenu[0]);
	for (TInt ii=0; ii<nMenu; ++ii)
		gc.DrawText(TheMenu[ii], TPoint(10,(ii+1)*h));
	gc.DiscardFont();
	scr->ReleaseFont(font);
	}

void MainL()
	{
	CClient* client=CClient::NewL();
	CActiveScheduler::Start();
	
	delete client;
	}

GLDEF_C TInt E32Main()
	{
	CTrapCleanup* trap=CTrapCleanup::New();
	if (!trap)
		return KErrNoMemory;
	CActiveScheduler* scheduler = new(ELeave) CActiveScheduler;
	CActiveScheduler::Install(scheduler);
	
	__UHEAP_MARK;	
	TRAPD(err, MainL());
	__UHEAP_MARKEND;	
	
	delete scheduler;
	delete trap;
	return err;
	}
