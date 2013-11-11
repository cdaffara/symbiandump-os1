// Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// The plug-in provides the client side CWsGraphic instance CWsListen, which is used in 
// GRAPHICS-WSERV-0438. 
// This also provides Test Case INC103472: CRedrawRegion::ContainsDrawers does not look for all drawers. 
// The customer incident "INC103472" reports CWsRedrawMsgWindow::CRedrawRegion::ContainsDrawers in wnredraw.cpp 
// is suppose to return wheather some drawers are contained within a specific region. 
// But it currently fails to check for drawers in drawers.
// So if one drawer from the iDrawerArray contains one of the drawers passed along 
// in aDrawers it will still return EFalse when it should return ETrue.
// The fix is added in CWsRedrawMsgWindow::CRedrawRegion::ContainsDrawers(const TArray& aDrawers,const RRegion& aRegion).
// The drawer->Contains() call will end up in HasAsChild(const TArray& aIds), 
// where the CWsGraphicDrawer has to look for its own nested drawers and return ETrue or EFalse.
// 
//

/**
 @file
 @test
 @internalComponent - Internal Symbian test code
*/
//CWsGraphic
#include "wssimpledrawer.h"
#include <s32mem.h>

CWsSimpleGraphicBitmap::CWsSimpleGraphicBitmap()
	{
	}

EXPORT_C CWsSimpleGraphicBitmap::~CWsSimpleGraphicBitmap()
	{
	iIsReady = EFalse;
	}

EXPORT_C CWsSimpleGraphicBitmap* CWsSimpleGraphicBitmap::NewL()
	{
	CWsSimpleGraphicBitmap* self = new(ELeave) CWsSimpleGraphicBitmap;
	CleanupStack::PushL(self);
 	self->BaseConstructL(KSimpleDrawerImplId,KNullDesC8());
	self->iIsReady = ETrue;
	CleanupStack::Pop(self);
	return self;
	}

EXPORT_C CWsSimpleGraphicBitmap* CWsSimpleGraphicBitmap::NewL(TUid aUid)
	{
	CWsSimpleGraphicBitmap* self = new(ELeave) CWsSimpleGraphicBitmap;
	CleanupStack::PushL(self);
	self->BaseConstructL(aUid,KSimpleDrawerImplId,KNullDesC8());
	self->iIsReady = ETrue;
	CleanupStack::Pop(self);
	return self;
	}
	
EXPORT_C CWsSimpleGraphicBitmap* CWsSimpleGraphicBitmap::NewL(const TWsGraphicId& aReplace)
	{
	CWsSimpleGraphicBitmap* self = new(ELeave) CWsSimpleGraphicBitmap;
	CleanupStack::PushL(self);
	self->BaseConstructL(aReplace,KSimpleDrawerImplId,KNullDesC8());
	self->iIsReady = ETrue;
 	CleanupStack::Pop(self);
	return self;
	}

EXPORT_C TInt CWsSimpleGraphicBitmap::UpdateColor(TRgb aColor)
	{
	if (!iIsReady)
		return KErrNotReady;
	// Send the message to server side
	TBuf8<3> cmd;
	TInt red = aColor.Red();
	TInt green = aColor.Green();
	TInt blue = aColor.Blue();
    //Append the color
	cmd.Append(red);
	cmd.Append(green);
	cmd.Append(blue);
	
	SendMessage(cmd);
	return Flush();
	}

EXPORT_C void CWsSimpleGraphicBitmap::HandleMessage(const TDesC8& /*aData*/)
	{
	}

EXPORT_C void CWsSimpleGraphicBitmap::OnReplace()
	{
	}

EXPORT_C TInt CWsSimpleGraphicBitmap::ShareGlobally()
	{
	return CWsGraphic::ShareGlobally();
	}

EXPORT_C TInt CWsSimpleGraphicBitmap::UnShareGlobally()
	{
	return CWsGraphic::UnShareGlobally();
	}

EXPORT_C TInt CWsSimpleGraphicBitmap::Share(TSecureId aClientId)
	{
	return CWsGraphic::Share(aClientId);
	}

EXPORT_C TInt CWsSimpleGraphicBitmap::UnShare(TSecureId aClientId)
	{
	return CWsGraphic::UnShare(aClientId);
	}  		 

EXPORT_C CWsInvisibleGraphicBitmap1* CWsInvisibleGraphicBitmap1::NewL()
	{
	CWsInvisibleGraphicBitmap1* self = new(ELeave) CWsInvisibleGraphicBitmap1;
	CleanupStack::PushL(self);
	self->BaseConstructL(KInvisibleDrawerImplId1,KNullDesC8());
	self->iIsReady = ETrue;
	CleanupStack::Pop(self);
	return self;
	}

EXPORT_C CWsInvisibleGraphicBitmap1* CWsInvisibleGraphicBitmap1::NewL(TUid aUid)
	{
	CWsInvisibleGraphicBitmap1* self = new(ELeave) CWsInvisibleGraphicBitmap1;
	CleanupStack::PushL(self);
	self->BaseConstructL(aUid,KInvisibleDrawerImplId1,KNullDesC8());
	self->iIsReady = ETrue;
	CleanupStack::Pop(self);
	return self;
	}
	
EXPORT_C CWsInvisibleGraphicBitmap1* CWsInvisibleGraphicBitmap1::NewL(const TWsGraphicId& aReplace)
	{
	CWsInvisibleGraphicBitmap1* self = new(ELeave) CWsInvisibleGraphicBitmap1;
	CleanupStack::PushL(self);
	self->BaseConstructL(aReplace,KInvisibleDrawerImplId1,KNullDesC8());
	self->iIsReady = ETrue;
	CleanupStack::Pop(self);
	return self;
	}

EXPORT_C CWsInvisibleGraphicBitmap2* CWsInvisibleGraphicBitmap2::NewL()
	{
	CWsInvisibleGraphicBitmap2* self = new(ELeave) CWsInvisibleGraphicBitmap2;
	CleanupStack::PushL(self);
 	self->BaseConstructL(KInvisibleDrawerImplId2,KNullDesC8());
	self->iIsReady = ETrue;
	CleanupStack::Pop(self);
	return self;
	}

EXPORT_C CWsInvisibleGraphicBitmap2* CWsInvisibleGraphicBitmap2::NewL(TUid aUid)
	{
	CWsInvisibleGraphicBitmap2* self = new(ELeave) CWsInvisibleGraphicBitmap2;
	CleanupStack::PushL(self);
	self->BaseConstructL(aUid,KInvisibleDrawerImplId2,KNullDesC8());
	self->iIsReady = ETrue;
 	CleanupStack::Pop(self);
	return self;
	}
	
EXPORT_C CWsInvisibleGraphicBitmap2* CWsInvisibleGraphicBitmap2::NewL(const TWsGraphicId& aReplace)
	{
	CWsInvisibleGraphicBitmap2* self = new(ELeave) CWsInvisibleGraphicBitmap2;
	CleanupStack::PushL(self);
	self->BaseConstructL(aReplace,KInvisibleDrawerImplId2,KNullDesC8());
	self->iIsReady = ETrue;
 	CleanupStack::Pop(self);
	return self;
	}

