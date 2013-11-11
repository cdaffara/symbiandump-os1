// Copyright (c) 1995-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// Test Animated DLL
// 
//

#include <e32std.h>
#include <w32std.h>
#include <w32adll.h>
#include <bitstd.h>
#include "ANIMDLL.H"

#define ANIM_TEST_TEXT _L("Testing 123")

class CTestAnim : public CWindowAnim
	{
	enum {ENumPolyPoints=9};
public:
	~CTestAnim();
	virtual void ConstructL(TAny *aArgs, TBool aHasFocus);
	virtual void Animate(TDateTime *aDateTime);
	virtual void Redraw();
	virtual void Command(TInt aOpcode, TAny *aArgs);
	virtual TInt CommandReplyL(TInt aOpcode, TAny *aArgs);
	void SetPolyList(const TRect &aRect);
	void DrawPolyLine();
	void DrawBitmap();
	void DrawText();
	void TweakPolyList(TInt aState);
	void FocusChanged(TBool aState);
	void InvalidateText();
	void InvalidateBitmap();
	//Pure virtual function from MEventHandler
	virtual TBool OfferRawEvent(const TRawEvent &aRawEvent);
private:
	void AppendTime(TDes& aTimeText,const TTime& aTime) const;
private:
	TPoint iPos;
	TSize iSize;
	TInt iColor;
	TBool iMasked;
	TRect iPolyRect;
	TInt iPolyState;
	TInt iWiggleSize;
	TPoint iTextPos;
	TBool iHasFocus;
	CArrayFixFlat<TPoint> *iPolyList;
	CFbsBitmap iBitmap1;
	CFbsBitmap iBitmap2;
	CFbsBitmap iMask;
	CFbsFont *iFont;
	};

class CTestAnimDll : public CAnimDll 
	{
public:
	CAnim *CreateInstanceL(TInt aType);
private:
	};

/*#if defined(__WINS__)
#pragma data_seg(".E32_UID")
__WINS_UID(0, KWservAnimDllUidValue, 0)
#pragma data_seg()
#endif*/

EXPORT_C CAnimDll *CreateCAnimDllL()
	{
	return(new(ELeave) CTestAnimDll());
	}

// Instance code //

void CTestAnim::Animate(TDateTime *)
	{
	if (!iWindowFunctions->IsHidden())
		{
		iWindowFunctions->ActivateGc();
		if (iPolyList)
			DrawPolyLine();
		}
	if (iPolyList)	
		{
		iPolyState=(iPolyState+1)%4;
		TweakPolyList(iPolyState);
		}
	if (!iWindowFunctions->IsHidden())
		{
		if (iPolyList)
			DrawPolyLine();
		DrawText();
		DrawBitmap();
		}
	iColor=(iColor+16)&0xFF;
	}

void CTestAnim::DrawPolyLine()
	{
	iGc->SetDrawMode(CGraphicsContext::EDrawModeXOR);
	iGc->SetPenColor(TRgb(255,255,255));
	iGc->DrawPolyLine(iPolyList);
	iGc->SetDrawMode(CGraphicsContext::EDrawModePEN);
	iGc->SetPenColor(TRgb(0,0,0));
	}

void CTestAnim::DrawBitmap()
	{
	iGc->SetClippingRegion(TRegionFix<1>(TRect(iPos,iSize)));
	CFbsBitmap *bitmap=iFunctions->FlashStateOn() ? &iBitmap1 : &iBitmap2;
	if (iMasked)
		iGc->BitBltMasked(iPos,bitmap, TRect(iSize), &iMask,EFalse);
	else
		iGc->BitBlt(iPos,bitmap);
	iGc->CancelClippingRegion();
	}

void CTestAnim::AppendTime(TDes& aTimeText,const TTime& aTime) const
	{
	_LIT(TimeFormat,"%:0%H%:1%T%:2%S");
	TRAPD(err,aTime.FormatL(aTimeText,TimeFormat));
	if (err!=KErrNone)
		{
		_LIT(DummyTime,"######");
		aTimeText.Append(DummyTime);
		}
	}

void CTestAnim::DrawText()
	{
	if (iHasFocus)
		{
		iGc->UseFont(iFont);
		TBuf<0x20> timebuf;
		TTime time(iFunctions->SystemTime());
		AppendTime(timebuf,time);
		TRect rect(iTextPos.iX,iTextPos.iY-iFont->AscentInPixels(),iTextPos.iX+iFont->TextWidthInPixels(timebuf),iTextPos.iY-iFont->AscentInPixels()+iFont->HeightInPixels());
		iGc->SetBrushStyle(CGraphicsContext::ESolidBrush);
		iGc->DrawText(timebuf,rect,iFont->AscentInPixels());
		time.UniversalTime();
		AppendTime(timebuf,time);
		rect.Move(0,iFont->HeightInPixels());
		iGc->DrawText(timebuf,rect,iFont->AscentInPixels());
		}
	}

void CTestAnim::Redraw()
	{
	DrawText();
	DrawBitmap();
	if (iPolyList)
		DrawPolyLine();
	}

void CTestAnim::ConstructL(TAny *aArgs, TBool aHasFocus)
	{
	iHasFocus=aHasFocus;
	iPos=((CTAParams *)aArgs)->pos;
	iFunctions->SetInterval(((CTAParams *)aArgs)->interval);
	if (iBitmap1.Duplicate(((CTAParams *)aArgs)->bit1)!=KErrNone || 
		iBitmap2.Duplicate(((CTAParams *)aArgs)->bit2)!=KErrNone ||
		iMask.Duplicate(((CTAParams *)aArgs)->mask)!=KErrNone)
		iFunctions->Panic();
	iSize.iWidth=Min(iBitmap1.SizeInPixels().iWidth,iBitmap2.SizeInPixels().iWidth);
	iSize.iHeight=Min(iBitmap1.SizeInPixels().iHeight,iBitmap2.SizeInPixels().iHeight);
	iWiggleSize=10;
	iFont=iFunctions->DuplicateFontL(((CTAParams *)aArgs)->font);
	}

void CTestAnim::SetPolyList(const TRect &aRect)
	{
	iPolyRect=aRect;
	TSize halfsize=aRect.Size();
	halfsize.iWidth>>=1;
	halfsize.iHeight>>=1;
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

void CTestAnim::TweakPolyList(TInt aState)
	{
	TSize halfsize=iPolyRect.Size();
	halfsize.iWidth>>=1;
	halfsize.iHeight>>=1;
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
		}
	}

void CTestAnim::InvalidateText()
	{
	TRect invalidate;
	invalidate.iTl.iX=iTextPos.iX;
	invalidate.iTl.iY=iTextPos.iY-iFont->AscentInPixels();
	invalidate.iBr.iX=iTextPos.iX+iFont->TextWidthInPixels(ANIM_TEST_TEXT);
	invalidate.iBr.iY=iTextPos.iY+iFont->DescentInPixels();
	iWindowFunctions->Invalidate(invalidate);
	}

void CTestAnim::InvalidateBitmap()
	{
	iWindowFunctions->Invalidate(TRect(iPos,iSize));
	}

void CTestAnim::Command(TInt aOpcode, TAny *aArgs)
	{
	switch(aOpcode)
		{
		case EADllOpcodeMove:
			{
			InvalidateBitmap();
			iPos=((CTAParams *)aArgs)->pos;
			iWindowFunctions->ActivateGc();
			DrawBitmap();
			iFunctions->SetInterval(((CTAParams *)aArgs)->interval);
			}
			break;
		case EADllTextPos:
			{
			InvalidateText();
			iTextPos=((CTAParams *)aArgs)->pos;
			iWindowFunctions->ActivateGc();
			DrawText();
			}
			break;
		case EADllToggleBitmapMask:
			iMasked=!iMasked;
			InvalidateBitmap();
			break;
		}
	}

TInt CTestAnim::CommandReplyL(TInt aOpcode, TAny *aArgs)
	{
	switch(aOpcode)
		{
		case EADllOpcodePolyLineRect:
			iWindowFunctions->ActivateGc();
			if (!iPolyList)
				{
				iPolyList=new(ELeave) CArrayFixFlat<TPoint>(ENumPolyPoints);
				TPoint zeropoint;
				for(TInt i=0;i<ENumPolyPoints;i++)
					iPolyList->AppendL(zeropoint);
				}
			else
				DrawPolyLine();
			SetPolyList(*((TRect *)aArgs));
			DrawPolyLine();
			break;
		default:
			iFunctions->Panic();
		}
	return(KErrNone);
	}

CTestAnim::~CTestAnim()
	{
	delete iPolyList;
	iFunctions->CloseFont(iFont);
	}

void CTestAnim::FocusChanged(TBool aNewState)
	{
	iHasFocus=aNewState;
	InvalidateText();
	InvalidateBitmap();
	}

TBool CTestAnim::OfferRawEvent(const TRawEvent &/*aRawEvent*/)
	{
	return EFalse;
	}

// DLL code //

CAnim *CTestAnimDll::CreateInstanceL(TInt )
	{
	return(new(ELeave) CTestAnim());
	}

// Dummy E32Dll needed by E32 to build //

