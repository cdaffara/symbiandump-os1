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
 @internalComponent - Internal Symbian test code 
*/

#include "spriteanimdll.h"

EXPORT_C CAnimDll *CreateCAnimDllL()
	{
	return(new(ELeave) CTAnimDll());
	}

CAnim* CTAnimDll::CreateInstanceL(TInt aType)
	{
	CAnim *anim=NULL;
	switch(aType)
		{
	case ESpriteAnimType:
		anim=new(ELeave) CTSpriteAnim();
		break;
		}
	return anim;
	}

// CTSpriteAnim test sprite access //

void CTSpriteAnim::ConstructL(TAny* /*aParam*/)
	{
	iSpriteFunctions->SizeChangedL();
	iSpriteFunctions->Activate(ETrue);
	}


void CTSpriteAnim::Animate(TDateTime*)
	{
	}

void CTSpriteAnim::Command(TInt aOpcode,TAny* /*aParam*/)
	{
	switch (aOpcode)
		{
	case EADllDraw1:
	case EADllDraw2:
	case EADllDraw3:
	case EADllDrawBlank:
		TRAPD(err,DrawL(aOpcode));
		if (err)
			{
			RDebug::Print(_L("CTSpriteAnim::Command - DrawL leave error: %d"), err);
			}		
		break;
	default:;
		}
	}

TInt CTSpriteAnim::CommandReplyL(TInt /*aOpcode*/,TAny* /*aParam*/)
	{
	return(KErrNone);
	}


TBool CTSpriteAnim::OfferRawEvent(const TRawEvent& /*aRawEvent*/)
	{
	return EFalse;
	}

void CTSpriteAnim::DrawL(TInt aOpcode)
	{
	CFbsBitmap *bitmap=iSpriteFunctions->GetSpriteMember(0)->iBitmap;
	CFbsBitmapDevice *device=CFbsBitmapDevice::NewL(bitmap);
	CleanupStack::PushL(device);
	CFbsBitGc *gc;
	User::LeaveIfError(device->CreateContext(gc));
	CleanupStack::PushL(gc);

	gc->SetBrushStyle(CGraphicsContext::ESolidBrush);
	gc->SetPenSize(TSize());
	TSize bitmapSize=bitmap->SizeInPixels();
	TSize size= bitmapSize;
	size.SetSize(size.iWidth/2,size.iHeight/2);
	TPoint point=size.AsPoint();

	// according to the opcode used, a quarter of the sprite bitmap is drawn to a different colour 
	switch(aOpcode)
		{
		case EADllDraw1:
			{
			gc->SetBrushColor(TRgb(255,0,0));
			gc->DrawRect(TRect(TPoint(),size));			
			break;
			}
		case EADllDraw2:
			{
			gc->SetBrushColor(TRgb(0,255,0));
			gc->DrawRect(TRect(TPoint(point.iX,0),size));
			break;
			}
		case EADllDraw3:
			{
			gc->SetBrushColor(TRgb(0,0,255));
			gc->DrawRect(TRect(TPoint(0,point.iY),size));
			break;
			}
		case EADllDrawBlank:
			{//the whole sprite bitmap is set to blank
			gc->SetBrushColor(TRgb(255,255,255));
			gc->DrawRect(TRect(TPoint(),bitmapSize));
			break;
			}
		default:;
		}
	iSpriteFunctions->UpdateMember(0,TRect(bitmap->SizeInPixels()),ETrue);
	CleanupStack::PopAndDestroy(2); //gc and device
	}
