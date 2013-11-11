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

#include "stressanim.h"
#include "stressanimcmd.h"
#include "panic.h"
#include <hal.h>

EXPORT_C CAnimDll *CreateCAnimDllL() //lint -e714 Suppress 'not referenced' 

	{
	return new (ELeave) CStressAnimDll();
	}

CAnim *CStressAnimDll::CreateInstanceL(TInt /*aType*/)
	{
	return new (ELeave) CStressWindowAnim();
	}

void CStressWindowAnim::Animate(TDateTime *)
	{
	if (!iDoAnimation)
		{
		return;
		}
	iLastAnimShot = User::NTickCount();
	if (!iWindowFunctions->IsHidden())
		{
		iWindowFunctions->ActivateGc();
		if (iPolyList)
			{
			DrawPolyLine();
			}
		}
	if (iPolyList)	
		{
		iPolyState=(iPolyState+1)%4;
		TweakPolyList(iPolyState);
		}
	if (!iWindowFunctions->IsHidden())
		{
		if (iPolyList)
			{
			DrawPolyLine();
			}
		DrawText();
		}
	iColor=(iColor+16)&0xFF;
	iDoAnimation = EFalse;
	}

void CStressWindowAnim::DrawPolyLine()
	{
	iGc->SetDrawMode(CGraphicsContext::EDrawModeXOR);
	iGc->SetPenColor(TRgb(255,255,255));
	iGc->DrawPolyLine(static_cast<CArrayFix<TPoint>*>(iPolyList));
	iGc->SetDrawMode(CGraphicsContext::EDrawModePEN);
	iGc->SetPenColor(TRgb(0,0,0));
	}

void CStressWindowAnim::DrawBitmap()
	{
	iGc->SetClippingRegion(TRegionFix<1>(TRect(iPos,iSize)));
	CFbsBitmap *bitmap=iFunctions->FlashStateOn() ? &iBitmap1 : &iBitmap2;
	if (iMasked)
		{
		iGc->BitBltMasked(iPos,bitmap, TRect(iSize), &iMask,EFalse);
		}
	else
		{
		iGc->BitBlt(iPos,bitmap);
		}
	iGc->CancelClippingRegion();
	}

void CStressWindowAnim::AppendTime(TDes& aTimeText,const TTime& aTime) const
	{
	_LIT(TimeFormat,"%:0%H%:1%T%:2%S");
	TRAPD(err,aTime.FormatL(aTimeText,TimeFormat));
	if (err!=KErrNone)
		{
		_LIT(DummyTime,"######");
		aTimeText.Append(DummyTime);
		}
	}

void CStressWindowAnim::DrawText()
	{
	if (iHasFocus)
		{
		iGc->UseFont(iFont);
		TBuf<0x20> timebuf;
		TUint64 elapsedMs = iLastAnimShot * iKernelTicksPeriod;
		TTime time(elapsedMs);

		AppendTime( timebuf , time );
		
		TSize winSize = iWindowFunctions->WindowSize();
		
		TInt textWidth = iFont->MeasureText( timebuf );
		
		TInt textHalf = textWidth / 2;
		
		TInt centerX = winSize.iWidth / 2;
		TInt centerY = winSize.iHeight / 2;
		
		TRect rect(centerX - textHalf, centerY - iFont->AscentInPixels(),
				   centerX + textHalf, centerY - iFont->AscentInPixels() + iFont->HeightInPixels());

		
		iGc->SetBrushStyle(CGraphicsContext::ESolidBrush);
		iGc->DrawText(timebuf,rect,iFont->AscentInPixels());
		rect.Move(0,iFont->HeightInPixels());
		iGc->DrawText(timebuf,rect,iFont->AscentInPixels());
		}
	}

void CStressWindowAnim::Redraw()
	{
	DrawText();
	if (iPolyList)
		{
		DrawPolyLine();
		}
	}

void CStressWindowAnim::ConstructL(TAny *aArgs, TBool aHasFocus)
	{
	iHasFocus=aHasFocus;
	iPos=((TStressTestAnimParams *)aArgs)->pos;
	iFunctions->SetInterval(((TStressTestAnimParams *)aArgs)->interval);
	if (iBitmap1.Duplicate(((TStressTestAnimParams *)aArgs)->bit1)!=KErrNone || 
		iBitmap2.Duplicate(((TStressTestAnimParams *)aArgs)->bit2)!=KErrNone ||
		iMask.Duplicate(((TStressTestAnimParams *)aArgs)->mask)!=KErrNone)
		{
		iFunctions->Panic();
		}
	iSize.iWidth=Min(iBitmap1.SizeInPixels().iWidth,iBitmap2.SizeInPixels().iWidth);
	iSize.iHeight=Min(iBitmap1.SizeInPixels().iHeight,iBitmap2.SizeInPixels().iHeight);
	iWiggleSize=10;
	iFont=iFunctions->DuplicateFontL(((TStressTestAnimParams *)aArgs)->font);
	iDoAnimation = ETrue;
	HAL::Get( HAL::ENanoTickPeriod, iKernelTicksPeriod );	
	iFunctions->SetSync( MAnimGeneralFunctions::ESyncSecond );
	}

void CStressWindowAnim::SetPolyList(const TRect &aRect)
	{
	iPolyRect=aRect;
	TSize halfsize=aRect.Size();
	__ASSERT_ALWAYS(halfsize.iWidth >= 0, Panic(EPanic10));
	__ASSERT_ALWAYS(halfsize.iHeight >= 0, Panic(EPanic11));
	halfsize.iWidth>>=1;	//lint !e702 Shift right of signed quantity (int)
	halfsize.iHeight>>=1;	//lint !e702 Shift right of signed quantity (int)
	(*iPolyList)[0]=aRect.iTl;
	(*iPolyList)[1]=TPoint(aRect.iTl.iX+iWiggleSize,aRect.iTl.iY+halfsize.iHeight);
	(*iPolyList)[2]=TPoint(aRect.iTl.iX,aRect.iBr.iY);
	(*iPolyList)[3]=TPoint(aRect.iTl.iX+halfsize.iWidth,aRect.iBr.iY-iWiggleSize);
	(*iPolyList)[4]=aRect.iBr;
	(*iPolyList)[5]=TPoint(aRect.iBr.iX-iWiggleSize,aRect.iTl.iY+halfsize.iHeight);
	(*iPolyList)[6]=TPoint(aRect.iBr.iX,aRect.iTl.iY);
	(*iPolyList)[7]=TPoint(aRect.iTl.iX+halfsize.iWidth,aRect.iTl.iY+iWiggleSize);
	(*iPolyList)[8]=aRect.iTl;
	TweakPolyList(iPolyState);
	}

void CStressWindowAnim::TweakPolyList(TInt aState)
	{
	TSize halfsize=iPolyRect.Size();
	__ASSERT_ALWAYS(halfsize.iWidth >= 0, Panic(EPanic12));
	__ASSERT_ALWAYS(halfsize.iHeight >= 0, Panic(EPanic13));
	halfsize.iWidth>>=1;	//lint !e702 Shift right of signed quantity (int)
	halfsize.iHeight>>=1;	//lint !e702 Shift right of signed quantity (int)
	switch(aState)
		{
		case 0:
			(*iPolyList)[7]=TPoint(iPolyRect.iTl.iX+halfsize.iWidth,iPolyRect.iTl.iY+iWiggleSize);
			(*iPolyList)[1]=TPoint(iPolyRect.iTl.iX,iPolyRect.iTl.iY+halfsize.iHeight);
			break;
		case 1:
			(*iPolyList)[1]=TPoint(iPolyRect.iTl.iX+iWiggleSize,iPolyRect.iTl.iY+halfsize.iHeight);
			(*iPolyList)[3]=TPoint(iPolyRect.iTl.iX+halfsize.iWidth,iPolyRect.iBr.iY);
			break;
		case 2:
			(*iPolyList)[3]=TPoint(iPolyRect.iTl.iX+halfsize.iWidth,iPolyRect.iBr.iY-iWiggleSize);
			(*iPolyList)[5]=TPoint(iPolyRect.iBr.iX,iPolyRect.iTl.iY+halfsize.iHeight);
			break;
		case 3:
			(*iPolyList)[5]=TPoint(iPolyRect.iBr.iX-iWiggleSize,iPolyRect.iTl.iY+halfsize.iHeight);
			(*iPolyList)[7]=TPoint(iPolyRect.iTl.iX+halfsize.iWidth,iPolyRect.iTl.iY);
			break;
		default:
			__ASSERT_ALWAYS(EFalse, Panic(EPanic14));
			break;
		}
	}

void CStressWindowAnim::InvalidateText()
	{
	TRect invalidate;
	invalidate.iTl.iX=iTextPos.iX;
	invalidate.iTl.iY=iTextPos.iY-iFont->AscentInPixels();
	invalidate.iBr.iX=iTextPos.iX+iFont->TextWidthInPixels(_L("Overstressed ANIM"));
	invalidate.iBr.iY=iTextPos.iY+iFont->DescentInPixels();
	iWindowFunctions->Invalidate(invalidate);
	}

void CStressWindowAnim::InvalidateBitmap()
	{
	iWindowFunctions->Invalidate(TRect(iPos,iSize));
	}

void CStressWindowAnim::Command(TInt aOpcode, TAny *aArgs)
	{
	switch(aOpcode)
		{
		case EADllOpcodeMove:
			{
			InvalidateBitmap();
			iPos=((TStressTestAnimParams *)aArgs)->pos;
			iWindowFunctions->ActivateGc();
			DrawBitmap();
			iFunctions->SetInterval(((TStressTestAnimParams *)aArgs)->interval);
			}
			break;
		case EADllTextPos:
			{
			InvalidateText();
			iTextPos=((TStressTestAnimParams *)aArgs)->pos;
			iWindowFunctions->ActivateGc();
			DrawText();
			}
			break;
		case EADllToggleBitmapMask:
			iMasked=!iMasked;
			InvalidateBitmap();
			break;
		default:
			__ASSERT_ALWAYS(EFalse, Panic(EPanic15));
			break;
		}
	}

TInt CStressWindowAnim::CommandReplyL(TInt aOpcode, TAny *aArgs)
	{
	
	TInt res = KErrNone;
	
	switch(aOpcode)
		{
		case EADllOpcodePolyLineRect:
			iWindowFunctions->ActivateGc();
			if (!iPolyList)
				{
				iPolyList=new(ELeave) CArrayFixFlat<TPoint>(ENumPolyPoints);
				TPoint zeropoint;
				for(TInt i=0;i<ENumPolyPoints;i++)
					{
					iPolyList->AppendL(zeropoint);
					}
				}
			else
				{
				DrawPolyLine();
				}
			SetPolyList(*((TRect *)aArgs));
			DrawPolyLine();
			break;
			
		case EADllQuerySync:
			res = iLastAnimShot;
			iDoAnimation = ETrue;
			break;
			
		default:
			iFunctions->Panic();
		}
	
	return res;
	}

CStressWindowAnim::~CStressWindowAnim()
	{
	delete iPolyList;
	iFunctions->CloseFont(iFont);
	}

void CStressWindowAnim::FocusChanged(TBool aNewState)
	{
	iHasFocus=aNewState;
	InvalidateText();
	InvalidateBitmap();
	}

TBool CStressWindowAnim::OfferRawEvent(const TRawEvent &/*aRawEvent*/)
	{
	return EFalse;
	}
