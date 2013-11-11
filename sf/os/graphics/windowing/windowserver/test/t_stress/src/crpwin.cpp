// Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
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
 @internalComponent
*/
 

#include "crpwin.h"

/*******************************************************************************
Client side of the crp used by CCrpWin
*******************************************************************************/

	
CCrpClient * CCrpClient::NewL()
	{
	CCrpClient * self = new (ELeave) CCrpClient();
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;
	}
	
void CCrpClient::DrawCrp(CWindowGc& aGc,const TRect& aRect)
	{
	aGc.DrawWsGraphic(Id(),aRect);
	CWsGraphic::Flush();
	}

CCrpClient::~CCrpClient()
	{
	}
	
CCrpClient::CCrpClient()
	{
	}
	
void CCrpClient::ConstructL()
	{
	BaseConstructL(TUid::Uid(0xa0005923),KNullDesC8()); //lint !e569 Loss of information (arg. no. 1) (32 bits to 31 bits)
	}
	
void CCrpClient::HandleMessage(const TDesC8& /*aData*/)
	{
	}
		
void CCrpClient::OnReplace()
	{
	__DEBUGGER();
	}
	



/**
CRP WIN
*/


//static configuration data, definitions and default assignments
TBool CCrpWin::iEnabled = ETrue;
TBool CCrpWin::iTransparent = ETrue;


CCrpWin* CCrpWin::NewLC(RWsSession& aWs, RWindowGroup* aGroup, CCompWin* aParent, CWindowGc& aGc)
	{
	CCrpWin* self = new (ELeave) CCrpWin(aWs, aGroup, aParent, aGc);
	CleanupStack::PushL(self);
	self->ConstructL();
	return self;
	}

void CCrpWin::LoadConfiguration(const MTestStepConfigurationContext* aContext)
	{
	aContext->GetBool(KT_WservStressParamEnabled, iEnabled);
	aContext->GetBool(KT_WservStressParamTransparent, iTransparent);
	}

CCrpWin::~CCrpWin()
    {
	delete iCrp;
    }

void CCrpWin::SetSize(const TSize & aSize)
	{
	CCompWin::SetSize(aSize);
	}
		
void CCrpWin::Redraw(const TRect& aRect)
    {
    iWsGc.Activate(*iWindow);
    iWsGc.Reset();

    iRedrawWindow->BeginRedraw(aRect);
    if(iCrp)
    	{
	    iCrp->DrawCrp(iWsGc,iSize);
	    }
    iRedrawWindow->EndRedraw();

    iWsGc.Deactivate();
    } 

void CCrpWin::DrawBitmap(CFbsBitGc* aGc, TRect& aClip, TPoint& aOrigin)
    {
    aGc->Reset();
	TPoint origin = iPos + aOrigin;
	aGc->SetOrigin(origin);
	
	TRect clip(origin, iSize);
	clip.Intersection(aClip);
	clip.Move(-origin);
	aGc->SetClippingRect(clip);
    
    TRect windowRect(origin,iSize);
    windowRect.Move(-origin);
    
    aGc->SetDrawMode(CGraphicsContext::EDrawModePEN);
	aGc->SetPenColor(TRgb(255,0,0));
	aGc->SetPenStyle(CGraphicsContext::ESolidPen);
	aGc->SetBrushStyle(CGraphicsContext::ESolidBrush);
	aGc->SetBrushColor(TRgb(255,0,0));
	aGc->DrawRect(windowRect);
	aGc->SetBrushColor(TRgb(0,0,255));
	aGc->DrawEllipse(windowRect);
	
	CCompWin::DrawBitmap(aGc, aClip, aOrigin);
    }
	
CCrpWin::CCrpWin(RWsSession& aWs, RWindowGroup* aGroup, CCompWin* aParent, CWindowGc& aGc) :
	CCompWin(aWs, aGroup, aParent, aGc)
    {
    }

void CCrpWin::ConstructL()
    {
    CCompWin::PreConstructL(iTransparent);
    iCrp = CCrpClient::NewL();
    CCompWin::PostConstructL();
    }

void CCrpWin::DumpDetails(RFile& aFile, TInt aDepth)
	{
	TBuf8<256> buf;
	buf.SetLength(0);
	for (TInt d = 0; d < aDepth; ++d)
		{
		buf.Append(_L8("  "));
		}
	buf.Append(_L8("Transparent = ["));
	buf.AppendNum((TInt64)iTransparent);
	buf.Append(_L8("]\r\n"));
	aFile.Write(buf);
	}

    
