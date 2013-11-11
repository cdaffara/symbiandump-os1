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

#include "compwin.h"
#include "utils.h"
#include "crpwin.h"
#include "edgedwin.h"
#include "coordinatewin.h"
#include "backedupwin.h"
#include "enormouswin.h"
#include "spritewin.h"
#include "animatedwin.h"

#include "test_step_logger.h"
#include "test_step_comparison.h"
#include "stresslet.h"
#include "comparison.h"

//static configuration data, definitions and default assignments
TInt CCompWin::sBackgroundAlpha = 0;

//configuration parameter names
_LIT(KT_WservStressParamBackgroundAlpha, "background_alpha");


/**
BASE COMPARISON WINDOW
*/
CCompWin* CCompWin::NewLC(RWsSession& aWs, RWindowGroup* aGroup, CCompWin* aParent, CWindowGc& aGc)
	{
	typedef CCompWin* (*TPtrNewLC)(RWsSession&, RWindowGroup*, CCompWin*, CWindowGc&);
	TPtrNewLC winFactory[8];
	TInt numEnabledWindows = 0;

	//setup an array of factory functions, one for each enabled window type
	if (CAnimatedWin::IsEnabled())   
		{
		winFactory[numEnabledWindows++] = TPtrNewLC(CAnimatedWin::NewLC);
		}
	if (CCoordinateWin::IsEnabled()) 
		{
		winFactory[numEnabledWindows++] = TPtrNewLC(CCoordinateWin::NewLC);
		}
	if (CBackedupWin::IsEnabled())   
		{
		winFactory[numEnabledWindows++] = TPtrNewLC(CBackedupWin::NewLC);
		}
	if (CEnormousWin::IsEnabled())   
		{
		winFactory[numEnabledWindows++] = TPtrNewLC(CEnormousWin::NewLC);
		}
	if (CCrpWin::IsEnabled())        
		{
		winFactory[numEnabledWindows++] = TPtrNewLC(CCrpWin::NewLC);
		}
	if (CEdgedWin::IsEnabled())      
		{
		winFactory[numEnabledWindows++] = TPtrNewLC(CEdgedWin::NewLC);
		}
	if (CSpritedWin::IsEnabled())    
		{
		winFactory[numEnabledWindows++] = TPtrNewLC(CSpritedWin::NewLC);
		}

	if (numEnabledWindows == 0)
		{
		aWs.LogMessage(_L("Error: No window-type is enabled(see ini file)"));
		User::Leave(KErrNotFound);
		}
	
	TInt winType = TRnd::rnd(numEnabledWindows);
	CCompWin* self = winFactory[ winType ](aWs, aGroup, aParent, aGc);
	return self;
	}

void CCompWin::LoadConfiguration(const MTestStepConfigurationContext* aContext)
	{
	aContext->GetInt(KT_WservStressParamBackgroundAlpha, sBackgroundAlpha);
	}

/**
 Get a random TDrawMode value.
 */
CGraphicsContext::TDrawMode CCompWin::GetRandomDrawMode()
	{
	static CGraphicsContext::TDrawMode modes[] =
		{
		CGraphicsContext::EDrawModeAND, CGraphicsContext::EDrawModeNOTAND, 
		CGraphicsContext::EDrawModePEN, CGraphicsContext::EDrawModeANDNOT,
		CGraphicsContext::EDrawModeXOR, CGraphicsContext::EDrawModeOR, 
		CGraphicsContext::EDrawModeNOTANDNOT, CGraphicsContext::EDrawModeNOTXOR,
		CGraphicsContext::EDrawModeNOTSCREEN, CGraphicsContext::EDrawModeNOTOR, 
		CGraphicsContext::EDrawModeNOTPEN, CGraphicsContext::EDrawModeORNOT,
		CGraphicsContext::EDrawModeNOTORNOT, CGraphicsContext::EDrawModeWriteAlpha
		};
	return modes[TRnd::rnd(sizeof(modes)/sizeof(CGraphicsContext::TDrawMode))];
	}

CGraphicsContext::TPenStyle CCompWin::GetRandomPenStyle()
	{
	static CGraphicsContext::TPenStyle styles[] = 
		{
		CGraphicsContext::ENullPen,
		CGraphicsContext::ESolidPen,
		CGraphicsContext::EDottedPen,
		CGraphicsContext::EDashedPen,
		CGraphicsContext::EDotDashPen,
		CGraphicsContext::EDotDotDashPen
		};
	return styles[TRnd::rnd(sizeof(styles)/sizeof(CGraphicsContext::TPenStyle))];
	}

CGraphicsContext::TBrushStyle CCompWin::GetRandomBrushStyle()
	{
	//CGraphicsContext::EPatternedBrush is not joined in the test in current version.
	static CGraphicsContext::TBrushStyle styles[] = 
		{
		CGraphicsContext::ENullBrush,
		CGraphicsContext::ESolidBrush,
		CGraphicsContext::EVerticalHatchBrush,
		CGraphicsContext::EForwardDiagonalHatchBrush,
		CGraphicsContext::EHorizontalHatchBrush,
		CGraphicsContext::ERearwardDiagonalHatchBrush,
		CGraphicsContext::ESquareCrossHatchBrush,
		CGraphicsContext::EDiamondCrossHatchBrush
		};
	return styles[TRnd::rnd(sizeof(styles)/sizeof(CGraphicsContext::TBrushStyle))];
	}

CCompWin::CCompWin(RWsSession& aWs, RWindowGroup* aGroup, CCompWin* aParent, CWindowGc& aGc) :
	iWs(aWs),
	iGroup(aGroup),
	iParent(aParent),
	iWsGc(aGc)
	{
	}

CCompWin::~CCompWin()
	{
	iChildren.ResetAndDestroy();
	
	if (iParent)
		{
		iParent->RemoveChild(this);
		}

	if (iWindow)
		{
		iWindow->Close();
		delete iWindow;
		}
	}

void CCompWin::PreConstructL(TBool aTransparency, TBool aBackedup)
	{
	if (aBackedup)
		{
		iWindow = iBackedupWindow = new(ELeave) RBackedUpWindow(iWs);
		if (iParent)
			{
			User::LeaveIfError(iBackedupWindow->Construct(*iParent->Window(), EColor64K, (TUint32)(this)));
			iParent->AddChildL(this);
			}
		else
			{
			User::LeaveIfError(iBackedupWindow->Construct(*iGroup, EColor64K, (TUint32)(this)));
			}
		}
	else
		{
		iWindow = iRedrawWindow = new(ELeave) RWindow(iWs);
		if (iParent)
			{
			User::LeaveIfError(iRedrawWindow->Construct(*iParent->Window(), (TUint32)(this)));
			iParent->AddChildL(this);
			}
		else
			{
			User::LeaveIfError(iRedrawWindow->Construct(*iGroup, (TUint32)(this)));
			}
		}
	if (iRedrawWindow)
		{
		if (aTransparency)
			{
			iRedrawWindow->SetTransparencyAlphaChannel();
			}
		/* Note: if background color is set to include a transparent background
		 * and AUTOCLEAR is set to 1 in wsini.ini, window's background will not be
		 * totaly cleared.
		 */
		iRedrawWindow->SetBackgroundColor(BackgroundColor());
		}

	iPos.iX = TRnd::rnd(KPosLimit);
	iPos.iY = TRnd::rnd(KPosLimit);
	iSize.iWidth = TRnd::rnd(KPosLimit);
	iSize.iHeight = TRnd::rnd(KPosLimit);
	iPos.iX -= iSize.iWidth / 2;
	iPos.iY -= iSize.iHeight / 2;
	SetPos(iPos);
	SetSize(iSize);
	iWindow->SetOrdinalPosition(0);
	}
	
void CCompWin::PostConstructL()
	{
	iVisible = ETrue;
	iWindow->Activate();
	}
	
void CCompWin::SetPos(const TPoint & aPos)
	{
	iPos = aPos;
	iWindow->SetPosition(iPos);
	}

void CCompWin::SetSize(const TSize & aSize)
	{
	iSize = aSize;
	TInt err = iWindow->SetSizeErr(iSize);
	if (err != KErrNone)
		{
		__DEBUGGER();
		}
	if (iRedrawWindow)
		{
		iRedrawWindow->Invalidate();
		}
	else if (iBackedupWindow)
		{
		Redraw(aSize);
		}
	}


void CCompWin::BringToFrontL()
	{
	if (iParent)
		{
		iParent->RemoveChild(this);
		iParent->AddChildL(this);
		}
	iWindow->SetOrdinalPosition(0);
	}

void CCompWin::SendToBackL()
	{
	if (iParent)
		{
		iParent->RemoveChild(this);
		iParent->iChildren.Insert(this, 0);
		iWindow->SetOrdinalPosition(-1);
		}
	}

void CCompWin::RemoveChild(CCompWin* aChild)
	{
	TInt num = iChildren.Find(aChild);
	if (num != KErrNotFound)
		{
		iChildren.Remove(num);
		}
	}

void CCompWin::AddChildL(CCompWin* aChild)
	{
	CleanupStack::PushL(aChild);
	User::LeaveIfError(iChildren.Append(aChild));
	CleanupStack::Pop(aChild);
	}

CCompWin * CCompWin::RandomWindow()
	{
	TInt num = TRnd::rnd(iChildren.Count() + 1);
	
	if (num == iChildren.Count())
		{
		return this;
		}
	else
		{
		return iChildren[num]->RandomWindow();
		}
	}

void CCompWin::ToggleVisible()
	{
	iVisible = !iVisible;
	iWindow->SetVisible(iVisible);
	}

void CCompWin::Dump(RFile& aFile, TPoint& aOrigin, TInt aDepth, CCompWin * aMark)
	{
	TPoint abs = aOrigin + iPos;
	static TBuf8<256> buf;
	buf.SetLength(0);
	for (TInt d = 0; d < aDepth; ++d)
		{
		buf.Append(_L8("  "));
		}
	buf.Append(_L8("rel ["));
	buf.AppendNum((TInt64)iPos.iX);
	buf.Append(_L8(","));
	buf.AppendNum((TInt64)iPos.iY);
	buf.Append(_L8("] abs ["));
	buf.AppendNum((TInt64)abs.iX);
	buf.Append(_L8(","));
	buf.AppendNum((TInt64)abs.iY);
	buf.Append(_L8("] size ["));
	buf.AppendNum((TInt64)iSize.iWidth);
	buf.Append(_L8(","));
	buf.AppendNum((TInt64)iSize.iHeight);
	buf.Append(_L8("] "));
	if (iVisible)
		{
		buf.Append(_L8("visible "));
		}
	else
		{
		buf.Append(_L8("invisible "));
		}
	buf.Append(TypeName());
	if (this == aMark)
		{
		buf.Append(_L8(" <active>"));
		}
	buf.Append(_L8("\r\n"));
	aFile.Write(buf);
	
	DumpDetails(aFile, aDepth);
	
	for (TInt num = 0; num < iChildren.Count(); ++num)
		{
		iChildren[num]->Dump(aFile, abs, aDepth + 1, aMark);
		}
	}

void CCompWin::DumpDetails(RFile&, TInt)
	{
	}

void CCompWin::HandleRedraw(TWsRedrawEvent& aEvent)
	{
	Redraw(aEvent.Rect());
	}

void CCompWin::DrawBitmap(CFbsBitGc* aGc, TRect& aClip, TPoint& aOrigin)
	{
	if (aClip.Width() > 0 && aClip.Height() > 0)
		{
		TPoint origin = iPos + aOrigin;
		TRect clip(origin, iSize);
		clip.Intersection(aClip);

		for (TInt num = 0; num < iChildren.Count(); ++num)
			{
			if (iChildren[num]->iVisible)
				{
				iChildren[num]->DrawBitmap(aGc, clip, origin);
				}
			}
		}
	}

/**
 Fills window's background with it's background color, 
 then iterates visible children
 */
void CCompWin::ClearBitmapBackground(CFbsBitGc* aGc, TRect& aClip, TPoint& aOrigin)
	{
	if (aClip.Width() > 0 && aClip.Height() > 0)
		{
		TPoint origin = iPos + aOrigin;
		TRect clip(origin, iSize);
		clip.Intersection(aClip);

		aGc->Reset();
		aGc->SetOrigin(TPoint(0,0));
		aGc->SetPenColor(BackgroundColor());
		aGc->SetBrushColor(BackgroundColor());
		aGc->SetPenStyle(CGraphicsContext::ESolidPen);
		aGc->SetBrushStyle(CGraphicsContext::ESolidBrush);
		aGc->DrawRect(clip);

		for (TInt num = 0; num < iChildren.Count(); ++num)
			{
			if (iChildren[num]->iVisible)
				{
				iChildren[num]->ClearBitmapBackground(aGc, clip, origin);
				}
			}
		}
	}

TBool CCompWin::QueryReadyForVerification()
	{
	TBool res = ETrue;
	
	TInt idx = 0;

	while ( idx < iChildren.Count() && res )
		{
		res = iChildren[ idx ]->QueryReadyForVerification();
		idx++;
		}
	
	return res;	
	}

/**	
 Subtracts window region from the supplied region 
 then calls all children to subtract themsleves
 @param aRegion region from which current window are should be subtructed
 @param aClip clipping area provided by the parent window
 @param aOrigin absolute origin of parent
 */
void CCompWin::SubSelfFromRegion(TRegion& aRegion, const TRect& aClip, const TPoint& aOrigin)
	{
	if (!iVisible)
		{
		return;
		}
	TPoint origin = iPos + aOrigin;
	TRect clip(origin, iSize);
	clip.Intersection(aClip);
	aRegion.SubRect(clip);
	for (TInt num = 0; num < iChildren.Count(); ++num)
		{
		if (iChildren[num]->iVisible)
			{
			iChildren[num]->SubSelfFromRegion(aRegion, clip, origin);
			}
		}
	}

void CCompWin::SetVerifyTick(TUint32 aTick)
	{
	iVerifyTick = aTick;
	for (TInt num = 0; num < iChildren.Count(); ++num)
		{
		if (iChildren[num]->iVisible)
			{
			iChildren[num]->SetVerifyTick(aTick);
			}
		}
	}

