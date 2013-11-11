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
//

#include "W32STDGRAPHIC.H"

LOCAL_C const TUid KStdBitmapType = {0x10281921};

NONSHARABLE_STRUCT(TBitmapInit)
	{
	TBitmapInit(const CFbsBitmap* aBitmap,const CFbsBitmap* aMask);
	TInt iBitmapHandle;
	TInt iMaskHandle;
	};
	
TBitmapInit::TBitmapInit(const CFbsBitmap* aBitmap,const CFbsBitmap* aMask)
	{
	iBitmapHandle = (aBitmap?aBitmap->Handle():0);
	iMaskHandle = (aMask?aMask->Handle():0);
	}

CWsGraphicBitmap::CWsGraphicBitmap()
	{
	}

EXPORT_C CWsGraphicBitmap::~CWsGraphicBitmap()
	{
	}

EXPORT_C CWsGraphicBitmap* CWsGraphicBitmap::NewL(const CFbsBitmap* aBitmap,const CFbsBitmap* aMask)
/** 
Constructs the client representation of a window-server-side CFbsBitmap owned by this Client.
The base class CWsGraphic is therefore constructed having KStdBitmapType as artwork type.

@param aBitmap CFbsBitmap owned by the Client 
@param aMask Bitmap's Mask.
*/
	{
	CWsGraphicBitmap* self = new(ELeave) CWsGraphicBitmap;
	CleanupStack::PushL(self);
	const TPckgBuf<TBitmapInit> data(TBitmapInit(aBitmap,aMask));
	self->BaseConstructL(KStdBitmapType,data);
	CleanupStack::Pop(self);
	return self;
	}

EXPORT_C CWsGraphicBitmap* CWsGraphicBitmap::NewL(TUid aUid,const CFbsBitmap* aBitmap,const CFbsBitmap* aMask)
/** 
Constructs the client representation of a window-server-side CFbsBitmap owned by this Client.
The base class CWsGraphic is therefore constructed having KStdBitmapType as artwork type.

@param aUid	Graphic Bitmap UID.
@param aBitmap CFbsBitmap owned by the Client 
@param aMask Bitmap's Mask.
*/
	{
	CWsGraphicBitmap* self = new(ELeave) CWsGraphicBitmap;
	CleanupStack::PushL(self);
	const TPckgBuf<TBitmapInit> data(TBitmapInit(aBitmap,aMask));
	self->BaseConstructL(aUid,KStdBitmapType,data);
	CleanupStack::Pop(self);
	return self;
	}
	
EXPORT_C CWsGraphicBitmap* CWsGraphicBitmap::NewL(const TWsGraphicId& aReplace,const CFbsBitmap* aBitmap,const CFbsBitmap* aMask)
/** 
Constructs the client representation of a window-server-side CFbsBitmap owned by this Client.
The base class CWsGraphic is therefore constructed having KStdBitmapType as artwork type.

Atomically replace the artwork that already exists with this artwork (i.e. aBitmap) .
If failure to properly construct the replacement artwork occurs, the replacee artwork will remain

@param aReplace Bitmap artwork which will be replaced.
@param aBitmap CFbsBitmap owned by the Client 
@param aMask Bitmap's Mask.
*/	
	{
	CWsGraphicBitmap* self = new(ELeave) CWsGraphicBitmap;
	CleanupStack::PushL(self);
	const TPckgBuf<TBitmapInit> data(TBitmapInit(aBitmap,aMask));
	self->BaseConstructL(aReplace,KStdBitmapType,data);
	CleanupStack::Pop(self);
	return self;
	}

EXPORT_C void CWsGraphicBitmap::HandleMessage(const TDesC8& /*aData*/)
	{
	}

EXPORT_C void CWsGraphicBitmap::OnReplace()
	{
	}

EXPORT_C TInt CWsGraphicBitmap::ShareGlobally()
	{
	return CWsGraphic::ShareGlobally();
	}

EXPORT_C TInt CWsGraphicBitmap::UnShareGlobally()
	{
	return CWsGraphic::UnShareGlobally();
	}

EXPORT_C TInt CWsGraphicBitmap::Share(TSecureId aClientId)
	{
	return CWsGraphic::Share(aClientId);
	}

EXPORT_C TInt CWsGraphicBitmap::UnShare(TSecureId aClientId)
	{
	return CWsGraphic::UnShare(aClientId);
	}
