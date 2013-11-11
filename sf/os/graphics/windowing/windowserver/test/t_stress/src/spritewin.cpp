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

#include "spritewin.h"
#include "utils.h"

#include <hal.h>


//common configuration parameter names
_LIT(KT_WservStressParamRandomizeDrawMode, "randomize_draw_mode");
_LIT(KT_WservStressParamMaxRandomStepX, "max_random_step_x");
_LIT(KT_WservStressParamMaxRandomStepY, "max_random_step_y");
_LIT(KT_WservStressParamMaxRandomOffsetX, "max_random_offset_x");
_LIT(KT_WservStressParamMaxRandomOffsetY, "max_random_offset_y");

//static configuration data, definitions and default assignments
TBool CSpritedWin::iEnabled = ETrue;
TBool CSpritedWin::iTransparent = ETrue;
TBool CSpritedWin::iRandomizePenStyle = EFalse;
TBool CSpritedWin::iRandomizeBrushStyle = EFalse;
TBool CSpritedWin::iRandomizeDrawMode = EFalse;
TBool CSpritedWin::iTransparentForegroundWindow = EFalse;
TInt CSpritedWin::iMaxRandomStepX = 0;
TInt CSpritedWin::iMaxRandomStepY = 0;
TInt CSpritedWin::iMaxRandomOffsetX = 0;
TInt CSpritedWin::iMaxRandomOffsetY = 0;

const TInt sInvisibleFrame = 24;	//how much bigger the invisible window is


CSpritedWin* CSpritedWin::NewLC(RWsSession& aWs, RWindowGroup* aGroup, CCompWin* aParent, CWindowGc& aGc)
	{
	CSpritedWin* self = new (ELeave) CSpritedWin( aWs, aGroup, aParent, aGc );
	CleanupStack::PushL( self );
	self->ConstructL();
	
	return self;
	}

void CSpritedWin::LoadConfiguration(const MTestStepConfigurationContext* aContext)
	{
	aContext->GetBool(KT_WservStressParamEnabled, iEnabled);
	aContext->GetBool(KT_WservStressParamTransparent, iTransparent);
	aContext->GetBool(KT_WservStressParamRandomizePenStyle, iRandomizePenStyle);
	aContext->GetBool(KT_WservStressParamRandomizeBrushStyle, iRandomizeBrushStyle);
	aContext->GetBool(KT_WservStressParamRandomizeDrawMode, iRandomizeDrawMode);
	aContext->GetBool(KT_WservStressParamTransparentForegroundWindow, iTransparentForegroundWindow);

	aContext->GetInt(KT_WservStressParamMaxRandomStepX, iMaxRandomStepX);
	aContext->GetInt(KT_WservStressParamMaxRandomStepY, iMaxRandomStepY);
	aContext->GetInt(KT_WservStressParamMaxRandomOffsetX, iMaxRandomOffsetX);
	aContext->GetInt(KT_WservStressParamMaxRandomOffsetY, iMaxRandomOffsetY);
	}

CSpritedWin::~CSpritedWin()
	{ 
	if (iForeWin)
		{
		iForeWin->Close();
		delete iForeWin;
		}

	iSprite.Close();
	
	TInt idx;
	for ( idx = 0; idx < ENofSlides; idx++)
		{
		delete iSpriteCntDevice [idx];
		delete iSpriteMskDevice [idx]; 
		
		delete iSpriteContent[idx];
		delete iSpriteMask[idx];
		}
	}
	
void CSpritedWin::Redraw(const TRect& aRect)
	{
	if (iSpriteStartup == 0)
		{
		//first time getting here should be very close to the sprite activation time
		iSpriteStartup = User::NTickCount();
		}
	iWsGc.Activate(*iWindow);

	iRedrawWindow->BeginRedraw( aRect );

	iWsGc.Reset();

	//draw rectangle filling window area
	iWsGc.SetPenStyle( iPenStyle );
	iWsGc.SetBrushStyle( iBrushStyle );
	iWsGc.SetBrushColor( iBgColor );
	iWsGc.SetPenColor( iBgColor );
	iWsGc.SetDrawMode(iDrawMode);
	
	TRect drawRect( TPoint(0,0), iSize );
	
	iWsGc.DrawRect( drawRect );
	
	iRedrawWindow->EndRedraw();
	
	iWsGc.Deactivate();	
	}

void CSpritedWin::DrawBitmap(CFbsBitGc* aGc, TRect& aClip, TPoint& aOrigin)
	{
	if (iSpriteStartup == 0)
		{
		//although bitmap drawing requested, sprite was not yet rendered - skip
		return;
		}
	aGc->Reset();
	TPoint origin = iPos + aOrigin;
	aGc->SetOrigin(origin);
	TRect clip(origin, iSize);
	clip.Intersection(aClip);
	clip.Move(-origin);
	aGc->SetClippingRect(clip);

	// draw win
	aGc->SetPenStyle(iPenStyle);
	aGc->SetBrushStyle(iBrushStyle);
	aGc->SetBrushColor( iBgColor );
	aGc->SetPenColor( iBgColor );
	aGc->SetDrawMode(iDrawMode);
	aGc->DrawRect(TRect(TPoint(0,0), iSize));
	
	// emulate sprite, cur frame
	// time (in ticks) passed since sprite activation
	TUint64 delta = TTickUtils::CalcTickDelta( iVerifyTick, iSpriteStartup );
	// time for a single slide (in microseconds)
	TUint slideDur  = iSlideDuration.Int();
	
	TInt slideNo = TUint32( ( (delta * iKernelTicksPeriod ) / slideDur ) % ENofSlides );
	
	TPoint spritePos( iSize.iWidth / 2 - ESpriteSzXHalf, 
			iSize.iHeight / 2 - ESpriteSzYHalf );
	
	TSize spriteSize(ESpriteSzX, ESpriteSzY);
	
	TRect	spriteRect ( TPoint(0,0), spriteSize );
	
	TRect   destRect( spritePos, spriteSize );
	destRect.Move(iSpriteDef[slideNo].iOffset);
	
	aGc->DrawBitmapMasked( destRect, 
						   iSpriteContent[slideNo], 
						   spriteRect, 
						   iSpriteMask[slideNo],
						   EFalse);
	
	// inherited
	CCompWin::DrawBitmap(aGc, aClip, aOrigin);	
	}

void CSpritedWin::ClearBitmapBackground(CFbsBitGc* aGc, TRect& aClip, TPoint& aOrigin)
	{
	if (iSpriteStartup == 0)
		{
		//although bitmap drawing requested, sprite was not yet rendered - skip
		return;
		}
	CCompWin::ClearBitmapBackground(aGc, aClip, aOrigin);
	}

void CSpritedWin::SetSize(const TSize & aSize)
	{
	CCompWin::SetSize( aSize );

	if ( iConstructed )
		{		
		TPoint spritePos( iSize.iWidth / 2 - ESpriteSzXHalf, 
				iSize.iHeight / 2 - ESpriteSzYHalf );
		
		iSprite.SetPosition( spritePos );
		}
	if (iForeWin)
		{
		iForeWin->SetExtent(TPoint(iPos.iX-sInvisibleFrame, iPos.iY-sInvisibleFrame), 
	            TSize(iSize.iWidth+2*sInvisibleFrame, iSize.iHeight+2*sInvisibleFrame));
		}
	}

void CSpritedWin::SetPos(const TPoint & aPos)
	{
	CCompWin::SetPos( aPos );
	if (iForeWin)
		{
		iForeWin->SetPosition(TPoint(iPos.iX-sInvisibleFrame, iPos.iY-sInvisibleFrame));
		}
	}

CSpritedWin::CSpritedWin(RWsSession& aWs, RWindowGroup* aGroup, CCompWin* aParent, CWindowGc& aGc):
	CCompWin( aWs, aGroup, aParent, aGc ), iSprite ( aWs ), iForeWin(NULL)
	{
	iBgColor 	= TRnd::rnd();
	if (!iTransparent)
		{
		iBgColor.SetAlpha(255);
		}
	iSlideDuration = (TRnd::rnd( ESlideDurMaxTenthSec ) + 1) * ESlideDurMult;

	HAL::Get( HAL::ENanoTickPeriod, iKernelTicksPeriod );	
	
	}

void CSpritedWin::ConstructL()
	{
	// prepare surface
	CCompWin::PreConstructL(iTransparent);
	
	TInt idx, err = KErrNone;
	TRect rect;
	CGraphicsContext* gCtxSpr = NULL;
	CGraphicsContext* gCtxMsk = NULL;

	// init sprite handles
	TPoint spritePos( iSize.iWidth / 2 - ESpriteSzXHalf, 
			iSize.iHeight / 2 - ESpriteSzYHalf );
	err = iSprite.Construct( *iWindow, spritePos, ESpriteNoChildClip  );
	User::LeaveIfError( err );
	
	iPenStyle = iRandomizePenStyle ? GetRandomPenStyle() : CGraphicsContext::ESolidPen;
	iBrushStyle = iRandomizeBrushStyle ? GetRandomBrushStyle() : CGraphicsContext::ESolidBrush;
	iDrawMode = iRandomizeDrawMode ? GetRandomDrawMode() : CGraphicsContext::EDrawModePEN;
	
	TInt stepx = iMaxRandomStepX > 0 ? TRnd::rnd(2*iMaxRandomStepX) - iMaxRandomStepX : 0;
	TInt stepy = iMaxRandomStepY > 0 ? TRnd::rnd(2*iMaxRandomStepY) - iMaxRandomStepY : 0;
	TInt offsetx = iMaxRandomOffsetX > 0 ? TRnd::rnd(2*iMaxRandomOffsetX) - iMaxRandomOffsetX : 0;
	TInt offsety = iMaxRandomOffsetY > 0 ? TRnd::rnd(2*iMaxRandomOffsetY) - iMaxRandomOffsetY : 0;
	
	// create sprites & masks
	for ( idx = 0; idx < ENofSlides; idx++)
		{
		iSpriteContent[idx] = new (ELeave) CFbsBitmap();
		iSpriteMask[idx]	= new (ELeave) CFbsBitmap();;

		err = iSpriteContent[idx]->Create(TSize(ESpriteSzX,ESpriteSzY), EColor64K);
		User::LeaveIfError( err );
		
		err = iSpriteMask[idx]->Create(TSize(ESpriteSzX,ESpriteSzY), EColor64K);
		User::LeaveIfError( err );
		
		iSpriteCntDevice [idx] = CFbsBitmapDevice::NewL (iSpriteContent[idx]);
		iSpriteMskDevice [idx] = CFbsBitmapDevice::NewL (iSpriteMask[idx]);
		
	
		// draw sprite content
		err = iSpriteCntDevice[idx]->CreateContext( gCtxSpr );
		User::LeaveIfError( err );
		CleanupStack::PushL( gCtxSpr );

		err = iSpriteMskDevice[idx]->CreateContext( gCtxMsk );
		User::LeaveIfError( err );
		CleanupStack::PushL( gCtxMsk );

		gCtxSpr->SetPenStyle(CGraphicsContext::ESolidPen);
		gCtxSpr->SetPenColor(KRgbWhite);
		gCtxSpr->SetBrushColor(KRgbWhite);
		gCtxSpr->SetBrushStyle( iBrushStyle );

		gCtxMsk->SetPenStyle(CGraphicsContext::ESolidPen);
		gCtxMsk->SetPenColor(KRgbBlack);
		gCtxMsk->SetBrushColor(KRgbBlack);
		gCtxMsk->SetBrushStyle( CGraphicsContext::ESolidBrush );
		
		rect.SetRect( 0, 0, ESpriteSzX, ESpriteSzY );
		gCtxSpr->DrawRect( rect );
		gCtxMsk->DrawRect( rect );

		// cross mask
		gCtxMsk->SetBrushColor(KRgbWhite);
		gCtxMsk->SetPenColor(KRgbWhite);
		rect.SetRect( ESpriteSzXHalf - EStepWidth * (idx + 1), 0, 
					  ESpriteSzXHalf + EStepWidth * (idx + 1), ESpriteSzY );
		gCtxMsk->DrawRect( rect );
		rect.SetRect( 0, ESpriteSzYHalf - EStepHeight * (idx + 1),
				      ESpriteSzX, ESpriteSzYHalf + EStepHeight * (idx + 1) );
		gCtxMsk->DrawRect( rect );

		CleanupStack::PopAndDestroy( gCtxMsk );		
		CleanupStack::PopAndDestroy( gCtxSpr );		
		
		// make sprite
		iSpriteDef[idx].iBitmap 		= iSpriteContent[idx];
		iSpriteDef[idx].iMaskBitmap	= iSpriteMask[idx];
		iSpriteDef[idx].iOffset		= TPoint( offsetx+idx*stepx, offsety+idx*stepy );
		iSpriteDef[idx].iDrawMode		= CGraphicsContext::EDrawModeAND;
		iSpriteDef[idx].iInvertMask	= EFalse;
		iSpriteDef[idx].iInterval		= iSlideDuration;
		
		err = iSprite.AppendMember( iSpriteDef[idx] );
		User::LeaveIfError( err );
		}

	//Create a transparent foreground window, moving and resizing with the sprite window
	if (iTransparentForegroundWindow)
		{
		iForeWin = new(ELeave) RBlankWindow(iWs);
		iForeWin->Construct(*iGroup,reinterpret_cast<TUint32>(iForeWin));
		iForeWin->SetColor(TRgb(0, 0, 0, 0));	//a transparent window
		iForeWin->SetExtent(TPoint(iPos.iX-sInvisibleFrame, iPos.iY-sInvisibleFrame), 
				            TSize(iSize.iWidth+2*sInvisibleFrame, iSize.iHeight+2*sInvisibleFrame));
		iForeWin->SetOrdinalPosition(0);
		iForeWin->Activate();
		}
	
	// finally
	CCompWin::PostConstructL();	

	iSpriteStartup = 0;
	// show up the sprite
	err = iSprite.Activate();
	User::LeaveIfError( err );
	
	iConstructed = ETrue;
	}

TBool CSpritedWin::QueryReadyForVerification()
	{

	TBool res;

	// time (in ticks) passed since sprite activation
	TUint64 delta = TTickUtils::CalcTickDelta( iVerifyTick, iSpriteStartup );
	// time for a single slide (in microseconds)
	TUint slideDur  = iSlideDuration.Int();
	// time for all sprite slides (in microseconds)
	TUint spriteDur = slideDur * ENofSlides;
	// time passed since begining of sprite sequence (in microseconds)
	TUint64 sense = (delta * iKernelTicksPeriod ) % spriteDur; // microsec
	
	res = (sense >= EFrameBorderSenseMs);
	
	res = res && CCompWin::QueryReadyForVerification();
		
	return res;
	}

void CSpritedWin::DumpDetails(RFile& aFile, TInt aDepth)
	{
	TBuf8<256> buf;
	buf.SetLength(0);
	for (TInt d = 0; d < aDepth; ++d)
		{
		buf.Append(_L8("  "));
		}
	buf.Append(_L8("Transparent = ["));
	buf.AppendNum((TInt64)iTransparent);
	buf.Append(_L8("] pen_style = ["));
	buf.AppendNum((TInt64)iPenStyle);
	buf.Append(_L8("] brush_style = ["));
	buf.AppendNum((TInt64)iBrushStyle);
	buf.Append(_L8("] draw_mode = ["));
	buf.AppendNum((TInt64)iDrawMode);
	buf.Append(_L8("] transparent_foreground_window = ["));
	buf.AppendNum((TInt64)iTransparentForegroundWindow);
	buf.Append(_L8("] max_random_step_x = ["));
	buf.AppendNum((TInt64)iMaxRandomStepX);
	buf.Append(_L8("] max_random_step_y = ["));
	buf.AppendNum((TInt64)iMaxRandomStepY);
	buf.Append(_L8("] max_random_offset_x = ["));
	buf.AppendNum((TInt64)iMaxRandomOffsetX);
	buf.Append(_L8("] max_random_offset_y = ["));
	buf.AppendNum((TInt64)iMaxRandomOffsetY);
	buf.Append(_L8("]\r\n"));
	aFile.Write(buf);
	}
