// Copyright (c) 1996-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// Base classes used for building window server test code
// 
//

#include <e32std.h>
#include <w32std.h>
#include <e32svr.h>
#include "testbase.h"

EXPORT_C TSpriteCreateParams::TSpriteCreateParams(TSize aSize,TPoint aOffset,CTSpriteBase::TSpriteDrawFunction aDrawFunc,
								TAny *aDrawFuncParam,TBool aMask,CGraphicsContext::TDrawMode aDrawMode) :
	iSize(aSize),
	iOffset(aOffset),
	iDrawFunc(aDrawFunc),
	iDrawFuncParam(aDrawFuncParam),
	iMask(aMask),
	iDrawMode(aDrawMode),
	iInterval(0)
	{}

EXPORT_C TSpriteCreateParams::TSpriteCreateParams() :
	iDrawFunc(NULL),
	iDrawMode(CGraphicsContext::EDrawModePEN),
	iInterval(0)
	{}

void CTSpriteBase::CreateBitmapL(CFbsBitmap *&aBitmap, CFbsBitmapDevice *&aBitmapDevice, TSpriteCreateParams *aParams, TBool aDoMask)
	{
	if (aBitmap==NULL)
		{
		aBitmap=new(ELeave) CFbsBitmap();
		User::LeaveIfError(aBitmap->Create(aParams->iSize,EGray4));
		}
	aBitmapDevice=CFbsBitmapDevice::NewL(aBitmap);
	CFbsBitGc *gc=CFbsBitGc::NewL();
	gc->Activate(aBitmapDevice);
	aParams->iDrawFunc(gc,0,aParams->iSize,aDoMask,aParams->iDrawFuncParam);
	delete gc;
	}

EXPORT_C void CTSpriteBase::RedrawL(CFbsBitmap *&aBitmap, CFbsBitmap *&aMaskBitmap, TSpriteCreateParams *aParams)
	{
	CFbsBitmapDevice *bitmapDevice=NULL;
	TRAPD(err,CreateBitmapL(aBitmap,bitmapDevice,aParams,EFalse));
	if (err==KErrNone)
		{
		delete bitmapDevice;
		bitmapDevice=NULL;
		if (aMaskBitmap)
			{
			TRAP(err,CreateBitmapL(aMaskBitmap,bitmapDevice,aParams,ETrue));
			delete bitmapDevice;
			}
		SpriteBase().UpdateMember(0);
		}
	User::LeaveIfError(err);
	}

EXPORT_C void CTSpriteBase::UpdateL(TInt aIndex, TSpriteCreateParams *aParams)
	{
	CFbsBitmap *bitmap=NULL;
	CFbsBitmapDevice *bitmapDevice=NULL;
	TRAPD(err,CreateBitmapL(bitmap,bitmapDevice,aParams,EFalse));
	if (err==KErrNone)
		{
		delete bitmapDevice;
		bitmapDevice=NULL;
		CFbsBitmap *mask=NULL;
		if (aParams->iMask)
			TRAP(err,CreateBitmapL(mask,bitmapDevice,aParams,ETrue));
		if (err==KErrNone)
			{
			TSpriteMember sprite;
			sprite.iBitmap=bitmap;
			sprite.iMaskBitmap=mask;
			sprite.iInvertMask=EFalse;
			sprite.iDrawMode=aParams->iDrawMode;
			sprite.iInterval=aParams->iInterval;
			sprite.iOffset=aParams->iOffset;
			err=SpriteBase().UpdateMember(aIndex,sprite);
			}
		delete mask;
		}
	delete bitmap;
	delete bitmapDevice;
	User::LeaveIfError(err);
	}

EXPORT_C CTSprite::CTSprite(RWsSession &aWs) : iSprite(aWs)
	{
	}

EXPORT_C CTSprite::~CTSprite()
	{
	iSprite.Close();
	}

EXPORT_C RWsSpriteBase &CTSprite::SpriteBase()
	{
	return(iSprite);
	}

EXPORT_C RWsSprite &CTSprite::Sprite()
	{
	return(iSprite);
	}

EXPORT_C void CTSprite::ConstructL(RWindowTreeNode &aWindow, const TPoint &aPos, TInt aCount, TSpriteCreateParams *aParams, TInt aFlags)
	{
	User::LeaveIfError(iSprite.Construct(aWindow,aPos,aFlags));
	for(TInt index=0;index<aCount;index++,aParams++)
		{
		CFbsBitmap *bitmap=NULL;
		CFbsBitmap *mask=NULL;
		CFbsBitmapDevice *bitmapDevice=NULL;
		TRAPD(err,CreateBitmapL(bitmap,bitmapDevice,aParams,EFalse));
		if (err==KErrNone)
			{
			delete bitmapDevice;
			bitmapDevice=NULL;
			if (aParams->iMask)
				TRAP(err,CreateBitmapL(mask,bitmapDevice,aParams,ETrue));
			if (err==KErrNone)
				{
				TSpriteMember sprite;
				sprite.iBitmap=bitmap;
				sprite.iMaskBitmap=mask;
				sprite.iInvertMask=EFalse;
				sprite.iDrawMode=aParams->iDrawMode;
				sprite.iOffset=aParams->iOffset;
				sprite.iInterval=aParams->iInterval;
				err=iSprite.AppendMember(sprite);
				}
			}
		delete mask;
		delete bitmap;
		delete bitmapDevice;
		User::LeaveIfError(err);
		}
	User::LeaveIfError(iSprite.Activate());
	}

EXPORT_C CTPointerCursor::CTPointerCursor(RWsSession &aWs) : iCursor(aWs)
	{
	}

EXPORT_C CTPointerCursor::~CTPointerCursor()
	{
	iCursor.Close();
	}

EXPORT_C RWsSpriteBase &CTPointerCursor::SpriteBase()
	{
	return(iCursor);
	}

EXPORT_C RWsPointerCursor &CTPointerCursor::PointerCursor()
	{
	return(iCursor);
	}

EXPORT_C void CTPointerCursor::ConstructL(TInt aFlags)
	{
	User::LeaveIfError(iCursor.Construct(aFlags));
	TSpriteMember sprite;
	sprite.iBitmap=NULL;
	User::LeaveIfError(iCursor.AppendMember(sprite));
	}

EXPORT_C void CTPointerCursor::ConstructL(TInt aCount, TSpriteCreateParams *aParams, TInt aFlags)
	{
	CFbsBitmap *bitmap=NULL;
	CFbsBitmap *mask=NULL;
	TRAPD(err,ConstructL(aCount, aParams, aFlags, bitmap, mask));
	delete bitmap;
	delete mask;
	User::LeaveIfError(err);
	}

EXPORT_C void CTPointerCursor::ConstructL(TInt aCount, TSpriteCreateParams *aParams, TInt aFlags, CFbsBitmap *&aBitmap, CFbsBitmap *&aMaskBitmap)
	{
	User::LeaveIfError(iCursor.Construct(aFlags));
	for(TInt index=0;index<aCount;index++,aParams++)
		{
		CFbsBitmapDevice *bitmapDevice=NULL;
		TRAPD(err,CreateBitmapL(aBitmap,bitmapDevice,aParams,EFalse));
		if (err==KErrNone)
			{
			delete bitmapDevice;
			bitmapDevice=NULL;
			if (aParams->iMask)
				TRAP(err,CreateBitmapL(aMaskBitmap,bitmapDevice,aParams,ETrue));
			if (err==KErrNone)
				{
				TSpriteMember sprite;
				sprite.iBitmap=aBitmap;
				sprite.iMaskBitmap=aMaskBitmap;
				sprite.iInvertMask=EFalse;
				sprite.iDrawMode=aParams->iDrawMode;
				sprite.iInterval=aParams->iInterval;
				sprite.iOffset=aParams->iOffset;
				err=iCursor.AppendMember(sprite);
				}
			}
		delete bitmapDevice;
		bitmapDevice=NULL;
		User::LeaveIfError(err);
		}
	User::LeaveIfError(iCursor.Activate());
	}
