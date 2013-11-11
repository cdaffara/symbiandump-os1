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

#include "animatedwin.h"
#include "utils.h"

#include "stressanimcmd.h"

#include <hal.h>


//static configuration data, definitions and default assignments
TBool CAnimatedWin::iEnabled = ETrue;
TBool CAnimatedWin::iTransparent = ETrue;
TBool CAnimatedWin::iTransparentForegroundWindow = EFalse;

const TInt sInvisibleFrame = 24;	//how much bigger the invisible window is

CAnimatedWin* CAnimatedWin::NewLC(RWsSession& aWs, RWindowGroup* aGroup, CCompWin* aParent, CWindowGc& aGc)
	{
	CAnimatedWin* self = new (ELeave) CAnimatedWin( aWs, aGroup, aParent, aGc );
	CleanupStack::PushL( self );
	self->ConstructL();
	
	return self;
	}

void CAnimatedWin::LoadConfiguration(const MTestStepConfigurationContext* aContext)
	{
	aContext->GetBool(KT_WservStressParamEnabled, iEnabled);
	aContext->GetBool(KT_WservStressParamTransparent, iTransparent);
	aContext->GetBool(KT_WservStressParamTransparentForegroundWindow, iTransparentForegroundWindow);
	}

CAnimatedWin::~CAnimatedWin()
	{ 
	if (iForeWin)
		{
		iForeWin->Close();
		delete iForeWin;
		}
	
	if ( iAnim )
		{
		iAnim->Destroy();
		}
	iAnimDll.Close();
	
	TInt idx;
	for ( idx = 0; idx < ENofFrames; idx++)
		{
		delete iAnimCntDevice [idx];
		delete iAnimContent [idx]; 
		}
	
	delete iAnimMskDevice;
	delete iAnimMask;
	}

	
void CAnimatedWin::Redraw(const TRect& aRect)
	{
	iWsGc.Activate(*iWindow);
	
	iRedrawWindow->BeginRedraw( aRect );
	
	iWsGc.Reset();

	iWsGc.SetPenStyle( CGraphicsContext::ESolidPen );
	iWsGc.SetBrushStyle( CGraphicsContext::EDiamondCrossHatchBrush );
	iWsGc.SetBrushColor( iBgColor );
	iWsGc.SetPenColor( iBgColor );
	
	TRect drawRect( TPoint(0,0), iSize );
	
	iWsGc.DrawRect( drawRect );
	
	iRedrawWindow->EndRedraw();
	
	iWsGc.Deactivate();	

	}

void CAnimatedWin::AppendTime(TDes& aTimeText,const TTime& aTime) const
{
	_LIT(TimeFormat,"%:0%H%:1%T%:2%S");
	TRAPD(err,aTime.FormatL(aTimeText,TimeFormat));
	if (err!=KErrNone)
		{
		_LIT(DummyTime,"######");
		aTimeText.Append(DummyTime);
		}
}


void CAnimatedWin::DrawBitmap(CFbsBitGc* aGc, TRect& aClip, TPoint& aOrigin)
	{
	aGc->Reset();
	TPoint origin = iPos + aOrigin;
	aGc->SetOrigin(origin);
	TRect clip(origin, iSize);
	clip.Intersection(aClip);
	clip.Move(-origin);
	aGc->SetClippingRect(clip);

	// draw win background
	aGc->SetPenStyle(CGraphicsContext::ESolidPen);
	aGc->SetBrushStyle(CGraphicsContext::EDiamondCrossHatchBrush);
	aGc->SetBrushColor( iBgColor );
	aGc->SetPenColor( iBgColor );
	aGc->DrawRect(TRect(TPoint(0,0), iSize));
	
	// draw text
	TBuf<128> timebuf;
	TUint64 elapsedMs = iAnimLastKnownAction * iKernelTicksPeriod;
	TTime restoredTime(elapsedMs);
	AppendTime( timebuf , restoredTime );
		
	TInt textWidth = iAnimFont->MeasureText( timebuf );
	TInt textHalf = textWidth / 2;
	
	TInt centerX = iSize.iWidth / 2;
	TInt centerY = iSize.iHeight / 2;
	
	TRect rect(centerX - textHalf, centerY - iAnimFont->AscentInPixels(),
			   centerX + textHalf, centerY - iAnimFont->AscentInPixels() + iAnimFont->HeightInPixels());

	aGc->Reset();

	origin = iPos + aOrigin;
	aGc->SetOrigin( origin );
	clip = TRect( origin, iSize );
	clip.Intersection( aClip );
	clip.Move(-origin);
	aGc->SetClippingRect(clip);
	
	aGc->SetBrushStyle(CGraphicsContext::ESolidBrush);
	
	aGc->UseFont(iAnimFont);
	
	aGc->DrawText(timebuf, rect, iAnimFont->AscentInPixels());

	rect.Move(0, iAnimFont->HeightInPixels());
	aGc->DrawText(timebuf, rect, iAnimFont->AscentInPixels());

	// inherited
	CCompWin::DrawBitmap(aGc, aClip, aOrigin);	
	}

void CAnimatedWin::SetSize(const TSize & aSize)
	{
	CCompWin::SetSize( aSize );
	if (iForeWin)
		{
		iForeWin->SetExtent(TPoint(iPos.iX-sInvisibleFrame, iPos.iY-sInvisibleFrame), 
				            TSize(iSize.iWidth+2*sInvisibleFrame, iSize.iHeight+2*sInvisibleFrame));
		}
	}

void CAnimatedWin::SetPos(const TPoint & aPos)
	{
	CCompWin::SetPos( aPos );
	if (iForeWin)
		{
		iForeWin->SetPosition(TPoint(iPos.iX-sInvisibleFrame, iPos.iY-sInvisibleFrame));
		}
	}

CAnimatedWin::CAnimatedWin(RWsSession& aWs, RWindowGroup* aGroup, CCompWin* aParent, CWindowGc& aGc):
	CCompWin( aWs, aGroup, aParent, aGc ), iAnimDll( aWs ), iForeWin(NULL)
	{
	iBgColor 	= TRnd::rnd(); 
	
	iFrameDuration = (TRnd::rnd( EFrameDurMaxTenthSec ) + 1) * EFrameDurMult;

	HAL::Get( HAL::ENanoTickPeriod, iKernelTicksPeriod );	
	
	}

void CAnimatedWin::ConstructL()
	{
	// prepare surface
	CCompWin::PreConstructL(iTransparent);
	
	TInt idx, err = KErrNone;
	TRect rect;

	TPoint animPos( iSize.iWidth / 2 - EFrameSzXHalf, 
			iSize.iHeight / 2 - EFrameSzYHalf );
	
	// create anim frames & masks
	for ( idx = 0; idx < ENofFrames; idx++)
		{
		iAnimContent[idx]	= new (ELeave) CFbsBitmap();

		err = iAnimContent[idx]->Create( TSize( EFrameSzX , EFrameSzY), EColor64K );
		User::LeaveIfError( err );
		
		iAnimCntDevice [idx] = CFbsBitmapDevice::NewL (iAnimContent[idx]);
		}

	iAnimMask	= new (ELeave) CFbsBitmap();

	err = iAnimMask->Create( TSize( EFrameSzX , EFrameSzY), EColor64K );
	User::LeaveIfError( err );
	
	iAnimMskDevice = CFbsBitmapDevice::NewL ( iAnimMask );
	

	CGraphicsContext* gCtxAni = NULL;
	CGraphicsContext* gCtxMsk = NULL;
	// draw frame 1 and mask
	err = iAnimCntDevice [ EFrame1 ]->CreateContext( gCtxAni );
	User::LeaveIfError( err );
	CleanupStack::PushL( gCtxAni );

	err = iAnimMskDevice->CreateContext( gCtxMsk );
	User::LeaveIfError( err );
	CleanupStack::PushL( gCtxMsk );
	
	
	gCtxAni->SetBrushColor(KRgbWhite);
	gCtxAni->SetPenColor(KRgbWhite);
	gCtxAni->SetBrushStyle( CGraphicsContext::ESolidBrush );

	gCtxMsk->SetBrushColor(KRgbWhite);
	gCtxMsk->SetPenColor(KRgbWhite);
	gCtxMsk->SetBrushStyle( CGraphicsContext::ESolidBrush );
	
	rect.SetRect( EFrameSzXHalf - EFrameSzXEgt, 0, EFrameSzXHalf + EFrameSzXEgt, EFrameSzYHalf ); //lint !e656 Arithmetic operation uses (compatible) enum's 
	gCtxAni->DrawRect( rect );
	gCtxMsk->DrawRect( rect );

	rect.SetRect( EFrameSzXHalf - EFrameSzXQtr, EFrameSzYHalf, EFrameSzXHalf + EFrameSzXQtr, EFrameSzY ); //lint !e656 Arithmetic operation uses (compatible) enum's
	gCtxAni->DrawRect( rect );
	gCtxMsk->DrawRect( rect );

	gCtxAni->SetPenColor(KRgbBlack);
	gCtxAni->DrawLine( TPoint(EFrameSzXHalf,0), TPoint (EFrameSzXHalf,EFrameSzY) );
	
	
	CleanupStack::PopAndDestroy( gCtxMsk ); 
	CleanupStack::PopAndDestroy( gCtxAni ); 

	// draw frame 2
	err = iAnimCntDevice [ EFrame2 ]->CreateContext( gCtxAni );
	User::LeaveIfError( err );
	CleanupStack::PushL( gCtxAni );

	gCtxAni->SetPenColor(KRgbWhite);
	gCtxAni->DrawLine( TPoint(EFrameSzXHalf,0), TPoint (EFrameSzXHalf,EFrameSzY) );

	CleanupStack::PopAndDestroy( gCtxAni ); 
	
	// init font
	err	= iAnimMskDevice->GetNearestFontInPixels( iAnimFont, TFontSpec( _L("Roman"), 16 ) );
	User::LeaveIfError( err );

	// load anim DLL
	err = iAnimDll.Load( KAnimDllName );
	User::LeaveIfError( err );
	
	// construct server side anim
	TPckgBuf<TStressTestAnimParams> animParams;
	animParams().pos 		= animPos;
	animParams().interval = iFrameDuration.Int();
	animParams().bit1		= iAnimContent[ EFrame1 ]->Handle();
	animParams().bit2		= iAnimContent[ EFrame2 ]->Handle();
	animParams().mask		= iAnimMask->Handle();
	animParams().font		= iAnimFont->Handle();

	iAnim = new (ELeave) CAnimatedWin::RStressAnim ( iAnimDll );
	
	err = iAnim->Construct( *iWindow, animParams );
	User::LeaveIfError( err );

	// finally
	CCompWin::PostConstructL();	

	// got time shot when we started
	iAnimStartup = User::NTickCount();

	//Create a transparent foreground window, moving and resizing with the animation window
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
	
	iConstructed = ETrue;
	}

void CAnimatedWin::SetAnimPos(const TPoint& aPos)
	{
	TPckgBuf<TStressTestAnimParams> animParams;
	animParams().pos = aPos;
	iAnim->RequestAnimThis( EADllTextPos, animParams );
	}

TBool CAnimatedWin::QueryReadyForVerification()
	{

	TBool res;
	
	TInt32 curTicks = User::NTickCount();

	iAnimLastKnownAction = (TUint32) iAnim->RequestAnimThis( EADllQuerySync ); 
	
	TUint64 elapsed = TTickUtils::CalcTickDelta( curTicks, iAnimLastKnownAction );

	TUint64 elapsedMs = elapsed * iKernelTicksPeriod;
	
	res = ( elapsedMs < EWatchMatchGap );
	
	res = res && CCompWin::QueryReadyForVerification();
		
	return res;
	}

CAnimatedWin::RStressAnim::RStressAnim (RAnimDll& aAnimDll):
	RAnim( aAnimDll )
	{ } // empty

TInt CAnimatedWin::RStressAnim::Construct(const RWindowBase &aDevice, const TDesC8 &aParams)
	{
	return RAnim::Construct( aDevice, 0, aParams );
	}

TInt CAnimatedWin::RStressAnim::RequestAnimThis(TInt aOpcode)
	{
	TInt res;
	
	TPckgC<TBool> params(ETrue);
	
	res = RequestAnimThis( aOpcode, params );
	
	return res;
	}

TInt CAnimatedWin::RStressAnim::RequestAnimThis(TInt aOpcode, const TDesC8 &aParams)
	{
	TInt res;
	
	res = CommandReply( aOpcode, aParams );
	
	return res;	
	}

void CAnimatedWin::DumpDetails(RFile& aFile, TInt aDepth)
	{
	TBuf8<256> buf;
	buf.SetLength(0);
	for (TInt d = 0; d < aDepth; ++d)
		{
		buf.Append(_L8("  "));
		}
	buf.Append(_L8("Transparent = ["));
	buf.AppendNum((TInt64)iTransparent);
	buf.Append(_L8("] transparent_foreground_window = ["));
	buf.AppendNum((TInt64)iTransparentForegroundWindow);
	buf.Append(_L8("] BgColor = ["));
	buf.AppendNum(iBgColor.Value());
	buf.Append(_L8("]\r\n"));
	aFile.Write(buf);
	}
