// Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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

#include "twindows.h"
#include <w32std.h>

//could you useful for visual checking 
//define DRAW_WINDOW

CTWindow * CTWindow::NewL(RWsSession & aWs, CTWindowTreeNode& aGroup, CWindowGc& aGc)
	{
	CTWindow * self = new (ELeave) CTWindow(aGc);
	CleanupStack::PushL(self);
	self->ConstructL(aWs, aGroup);
	CleanupStack::Pop(self);
	return self;
	}

CTWindow::CTWindow(CWindowGc& aGc) :
iGc(aGc)
	{
	}
	
void CTWindow::ConstructL(RWsSession & aWs, CTWindowTreeNode& aGroup)
	{
	iWin = new (ELeave) RWindow(aWs);
	Window()->Construct(*aGroup.WindowTreeNode(), TUint32(this));
	Window()->SetRequiredDisplayMode(EColor16MAP);
	Window()->SetShadowHeight(0);
	Window()->SetShadowDisabled(ETrue);
	}
	
CTWindow::~CTWindow()
	{
	iWin->Close();
	delete iWin;
	}

void CTWindow::Event(TWsEvent & /*aEvent*/)
	{
	}

void CTWindow::Redraw(TWsRedrawEvent & /*aEvent*/)
	{
	Window()->BeginRedraw();
	iGc.Activate(*Window());
	Draw();
	iGc.Deactivate();
	Window()->EndRedraw();
	}

void CTWindow::Draw()
	{
#ifdef DRAW_WINDOW	
	TSize size = Window()->Size();
	
	TInt l = size.iWidth / 3;
	TInt r = size.iWidth * 2 / 3;
	TInt t = size.iHeight / 3;
	TInt b = size.iHeight * 2 / 3;

	iGc.SetBrushStyle(CGraphicsContext::ESolidBrush);
	iGc.SetBrushColor(KRgbRed);
	iGc.SetPenStyle(CGraphicsContext::ESolidPen);
	iGc.SetPenColor(KRgbBlue);
	iGc.DrawLine(TPoint(-400, -400),               TPoint(size.iWidth + 400, size.iHeight + 400));	
	iGc.DrawLine(TPoint(-400, size.iHeight + 400), TPoint(size.iWidth + 400, -400));	
	iGc.SetPenColor(KRgbGreen);
	iGc.DrawRect(TRect(TPoint(l,0), TPoint(r, size.iHeight)));
	iGc.DrawRect(TRect(TPoint(0,t), TPoint(size.iWidth, b)));
#endif	
	}

CTWindowGroup * CTWindowGroup::NewL(RWsSession & aWs, CWsScreenDevice* aScreenDevice)
	{
	CTWindowGroup * self = new (ELeave) CTWindowGroup;
	CleanupStack::PushL(self);
	self->ConstructL(aWs, aScreenDevice);
	CleanupStack::Pop(self);
	return self;
	}

CTWindowGroup::CTWindowGroup()
	{
	}

void CTWindowGroup::ConstructL(RWsSession & aWs, CWsScreenDevice* aScreenDevice)
	{
	iWin = new (ELeave) RWindowGroup(aWs);
	WindowGroup()->Construct(TUint32(this), aScreenDevice);
	}

CTWindowGroup::~CTWindowGroup()
	{
	iWin->Close();
	delete iWin;
	}

void CTWindowGroup::Event(TWsEvent & /*aEvent*/)
	{
	}

void CTWindowGroup::Redraw(TWsRedrawEvent & /*aEvent*/)
	{
	}

