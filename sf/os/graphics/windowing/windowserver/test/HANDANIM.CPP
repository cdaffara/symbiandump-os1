// Copyright (c) 1999-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// Anim DLL to deal with handwriting
// 
//

#include "HANDANIM.H"

#define DEFAUlT_LINE_WIDTH 4
#define DEFAUlT_MASK_WIDTH_FACTOR 3
#define DEFAUlT_END_POINT_FACTOR 2
#define BLACK TRgb::Gray2(0)
#define WHITE TRgb::Gray2(1)

#define leavescan_needs_to_see_some_brackets_here	{	}
EXPORT_C CAnimDll *CreateCAnimDllL()
	{
	return(new(ELeave) CHandWritingAnimDll());
	}


/*CHandWritingAnimDll*/

CAnim *CHandWritingAnimDll::CreateInstanceL(TInt /*aType*/)
	{
	return new(ELeave) CHandWritingAnim();
	}


/*CHandWritingAnim*/

CHandWritingAnim::~CHandWritingAnim()
	{
	iFunctions->GetRawEvents(EFalse);
	delete iBitmapDevice;
	delete iMaskBitmapDevice;
	}

void CHandWritingAnim::Activate()
	{
	if (iState==EHwStateDeactive)
		{
		iState=EHwStateInactive;
		iFunctions->GetRawEvents(ETrue);
		}
	}

void CHandWritingAnim::Deactivate()
	{
	if (iState!=EHwStateDeactive)
		{
		iState=EHwStateDeactive;
		iFunctions->GetRawEvents(EFalse);
		iSpriteFunctions->Activate(EFalse);
		ClearSprite();
		}
	}

void CHandWritingAnim::SpriteChangeL(TBool aUsingSeparateMask)
	{
	if (aUsingSeparateMask)
		{
		TSpriteMember *spriteMember=iSpriteFunctions->GetSpriteMember(0);
		iMaskBitmapDevice=CFbsBitmapDevice::NewL(spriteMember->iMaskBitmap);		//If this leaves the error value will be returned to the client side
		}
	else
		{
		delete iMaskBitmapDevice;
		iMaskBitmapDevice=NULL;
		iDrawData.iLineColor=BLACK;		//Must use black ink when there is no mask
		iDrawData.iInitialBitmapColor=WHITE;		//Must have white background when there is no mask
		}
	iIsMask=aUsingSeparateMask;
	}

void CHandWritingAnim::SetDrawData(THandwritingDrawData *aDrawData)
	{
	iDrawData=*aDrawData;
	if (!iIsMask)
		{
		iDrawData.iLineColor=BLACK;		//Must use black ink when there is no mask
		iDrawData.iInitialBitmapColor=WHITE;		//Must have white background when there is no mask
		}
	}

TBool CHandWritingAnim::HandlePointerDown(TPoint aPoint)
	{
	if (iState==EHwStateWaitingMove)
		return EFalse;
	iCurrentDrawPoint=aPoint;
	if (iState==EHwStateInactive)
		{
		iState=EHwStateWaitingMove;
		StartTimer();
		return ETrue;
		}
	iState=EHwStateDrawing;
	DrawPoint();
	UpdateSprite();
	return ETrue;
	}

TBool CHandWritingAnim::HandlePointerMove(TPoint aPoint)
	{
	switch (iState)
		{
	case EHwStateWaitingMove:
		{
		TPoint moved=aPoint-iCurrentDrawPoint;
		if (Abs(moved.iX)<5 && Abs(moved.iY)<5)			//Need to do something with these constants
			return ETrue;
		iSpriteFunctions->Activate(ETrue);
		DrawPoint();
		iState=EHwStateDrawing;
		}
	case EHwStateDrawing:
		break;
	default:
		return EFalse;
		}
	DrawLine(aPoint);
	UpdateSprite();
	return ETrue;
	}

TBool CHandWritingAnim::HandlePointerUp(TPoint aPoint)
	{
	if (iState==EHwStateInactive)
		return EFalse;
	else if (iState==EHwStateWaitingMove)
		{
		TPoint moved=aPoint-iCurrentDrawPoint;
		if (Abs(moved.iX)<5 && Abs(moved.iY)<5)			//Need to do something with these constants
			{
			SendEatenDownEvent();
			return EFalse;
			}
		iSpriteFunctions->Activate(ETrue);
		DrawPoint();
		}
	DrawLine(aPoint);
	DrawPoint();
	UpdateSprite();
	iState=EHwStateWaitingStroke;
	StartTimer();
	return ETrue;
	}

void CHandWritingAnim::DrawPoint()
	{
	iSpriteGc->Activate(iBitmapDevice);
	iSpriteGc->SetPenSize(TSize(iDrawData.iEndPontWidth,iDrawData.iEndPontWidth));
	iSpriteGc->SetPenColor(iDrawData.iLineColor);
	iSpriteGc->Plot(iCurrentDrawPoint);
	if (iMaskBitmapDevice)
		{
		iSpriteGc->Activate(iMaskBitmapDevice);
		iSpriteGc->SetPenSize(TSize(iDrawData.iMaskLineWidth,iDrawData.iMaskLineWidth));
		iSpriteGc->SetPenColor(BLACK);		//Mask must be drawn in black
		iSpriteGc->Plot(iCurrentDrawPoint);
		}
	iPointStore->AddPoint(iCurrentDrawPoint);
	}

void CHandWritingAnim::DrawLine(TPoint aEndPoint)
	{
	iSpriteGc->Activate(iBitmapDevice);
	iSpriteGc->SetPenSize(TSize(iDrawData.iLineWidth,iDrawData.iLineWidth));
	iSpriteGc->SetPenColor(iDrawData.iLineColor);
	iSpriteGc->MoveTo(iCurrentDrawPoint);
	iSpriteGc->DrawLineTo(aEndPoint);
	if (iMaskBitmapDevice)
		{
		iSpriteGc->Activate(iMaskBitmapDevice);
		iSpriteGc->SetPenSize(TSize(iDrawData.iMaskLineWidth,iDrawData.iMaskLineWidth));
		iSpriteGc->SetPenColor(BLACK);		//Mask must be drawn in black
		iSpriteGc->MoveTo(iCurrentDrawPoint);
		iSpriteGc->DrawLineTo(aEndPoint);
		}
	iCurrentDrawPoint=aEndPoint;
	iPointStore->AddPoint(aEndPoint);
	}

void CHandWritingAnim::UpdateSprite()
	{
	TRect drawTo;
	iSpriteGc->RectDrawnTo(drawTo);
	iSpriteFunctions->UpdateMember(0,drawTo,EFalse);
	}

void CHandWritingAnim::StartTimer()
	{
	iFunctions->SetNextInterval(2);
	}

void CHandWritingAnim::SendEatenDownEvent()
	{
	TRawEvent rawEvent;
	rawEvent.Set(TRawEvent::EButton1Down,iCurrentDrawPoint.iX,iCurrentDrawPoint.iY);
	iFunctions->PostRawEvent(rawEvent);
	iState=EHwStateInactive;
	}

void CHandWritingAnim::CharacterFinished()
	{		
	iState=EHwStateInactive;
	iLastGeneratedCharacter=iPointStore->GetChar();
	/*TRawEvent rawEvent;
	rawEvent.Set(TRawEvent::EKeyDown,iLastGeneratedCharacter);
	iFunctions->PostKeyEvent(rawEvent);*/
    TKeyEvent keyEvent;
    keyEvent.iCode=keyEvent.iScanCode=iLastGeneratedCharacter;
    keyEvent.iModifiers=keyEvent.iRepeats=0;
	iFunctions->PostKeyEvent(keyEvent);
	iPointStore->ClearPoints();
	iSpriteFunctions->Activate(EFalse);
	ClearSprite();
	}

void CHandWritingAnim::ClearSprite()
	{
	iSpriteGc->Activate(iBitmapDevice);
	iSpriteGc->SetBrushStyle(CGraphicsContext::ESolidBrush);
	iSpriteGc->SetBrushColor(iDrawData.iInitialBitmapColor);
	iSpriteGc->Clear();
	if (iMaskBitmapDevice)
		{
		iSpriteGc->Activate(iMaskBitmapDevice);
		iSpriteGc->SetBrushColor(WHITE);		//Mask must be cleared in white
		iSpriteGc->Clear();
		}
    TRect drawnTo;
	iSpriteGc->RectDrawnTo(drawnTo);		//Clear the drawnTo rect.
	}

TBool CHandWritingAnim::OfferRawEvent(const TRawEvent &aRawEvent)
	{
	if (iState==EHwStateDeactive)
		return EFalse;
	switch (aRawEvent.Type())
		{
	case TRawEvent::EButton1Down:
		{
		iDownTime.HomeTime();		
		return HandlePointerDown(aRawEvent.Pos());
		}
	case TRawEvent::EPointerMove:
		return HandlePointerMove(aRawEvent.Pos());
	case TRawEvent::EButton1Up:
		return HandlePointerUp(aRawEvent.Pos());
	default:
		return EFalse;
		}
	}

void CHandWritingAnim::ConstructL(TAny *)
	{
	TSpriteMember *spriteMember=iSpriteFunctions->GetSpriteMember(0);
	iIsMask=(spriteMember->iBitmap->Handle() != spriteMember->iMaskBitmap->Handle());
	iBitmapDevice=CFbsBitmapDevice::NewL(spriteMember->iBitmap);
	if (iIsMask)
		iMaskBitmapDevice=CFbsBitmapDevice::NewL(spriteMember->iMaskBitmap);
	iState=EHwStateDeactive;
	iSpriteGc=CFbsBitGc::NewL();
	iSpriteGc->Reset();
	iDrawData.iLineColor=BLACK;
	iDrawData.iInitialBitmapColor=WHITE;
	iDrawData.iLineWidth=DEFAUlT_LINE_WIDTH;
	iDrawData.iMaskLineWidth=DEFAUlT_MASK_WIDTH_FACTOR*DEFAUlT_LINE_WIDTH;
	iDrawData.iEndPontWidth=DEFAUlT_END_POINT_FACTOR*DEFAUlT_LINE_WIDTH;
	iSpriteFunctions->SizeChangedL();
	iPointStore=new(ELeave) CPointStore();
	iPointStore->ConstructL();
	}

void CHandWritingAnim::Animate(TDateTime* /*aDateTime*/)
	{
	iFunctions->SetInterval(0);
	if (iState==EHwStateWaitingMove)
		SendEatenDownEvent();
	else if (iState==EHwStateWaitingStroke)
		CharacterFinished();
	}

void CHandWritingAnim::Redraw()
	{
	}

void CHandWritingAnim::Command(TInt aOpcode,TAny *aParams)
	{
	switch (aOpcode)
		{
		case EHwOpActivate:
			Activate();
			break;
		case EHwOpDeactivate:
			Deactivate();
			break;
		case EHwOpSetDrawData:;
			SetDrawData(STATIC_CAST(THandwritingDrawData*,aParams));
			break;
		default:
			iFunctions->Panic();
		}
	}

void CHandWritingAnim::FocusChanged(TBool )
	{
	}

TInt CHandWritingAnim::CommandReplyL(TInt aOpcode,TAny *aParams)
	{
	switch (aOpcode)
		{
		case EHwOpSpriteMask:
			SpriteChangeL(*STATIC_CAST(TBool*,aParams));
			break;
		case EHwOpGetLastChar:
			return iLastGeneratedCharacter;
		default:
			iFunctions->Panic();
		}
	return KErrNone;
	}


/*CPointStore*/

CPointStore::CPointStore()
	{}

void CPointStore::ConstructL()
	{
	iPoints=new(ELeave) CArrayFixFlat<TPoint>(16);
	iPoints->ResizeL(256);
	}

void CPointStore::AddPoint(TPoint aPoint)
	{
	if (iNumPoints<256)
		(*iPoints)[iNumPoints++]=aPoint;
	}

TInt CPointStore::GetChar()
	{
	TPoint oldPoint=(*iPoints)[0];
	TPoint newPoint;
	TPoint totalPoint=oldPoint;
	TInt xInc=0,xDec=0,yInc=0,yDec=0;
	TInt yState=0,xState=0;
	TInt ii;
	for (ii=1;ii<iNumPoints;++ii)
		{
		newPoint=(*iPoints)[ii];
		totalPoint+=newPoint;
		if (newPoint.iX>oldPoint.iX)
			++xInc;
		if (newPoint.iX<oldPoint.iX)
			++xDec;
		if (newPoint.iY>oldPoint.iY)
			++yInc;
		if (newPoint.iY<oldPoint.iY)
			++yDec;
		oldPoint=newPoint;
		}
	newPoint-=(*iPoints)[0];
	if (10*yInc<yDec)
		yState=-1;
	else if (yInc>10*yDec)
		yState=1;
	if (10*xInc<xDec)
		xState=-1;
	else if (xInc>10*xDec)
		xState=1;
	if (xState!=0 && yState!=0)
		{
		if (Abs(newPoint.iY)<Abs(newPoint.iX))
			yState=0;
		else
			xState=0;
		}
	if (xState!=0)
		return xState>0 ? EKeyRightArrow:EKeyLeftArrow;
	if (yState!=0)
		return yState>0 ? EKeyDownArrow:EKeyUpArrow;
	TInt firstChar='a';
	TInt numChars=26;
	TInt type=(totalPoint.iY/10)%10;
	if (type>5)
		firstChar='A';
	else if (type==0)
		{
		firstChar='0';
		numChars=10;
		}
	return firstChar+((totalPoint.iX/10)%numChars);
	}

void CPointStore::ClearPoints()
	{
	iNumPoints=0;
	}
