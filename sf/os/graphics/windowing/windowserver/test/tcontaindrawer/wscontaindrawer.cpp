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
// is suppose to return whether some drawers are contained within a specific region. 
// But right now it doesn't check for drawers in drawers.
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
#include "wscontaindrawer.h"

CWsContainGraphicBitmap::CWsContainGraphicBitmap()
	{
	}

EXPORT_C CWsContainGraphicBitmap::~CWsContainGraphicBitmap()
	{
	iIsReady = EFalse;
	}

EXPORT_C CWsContainGraphicBitmap* CWsContainGraphicBitmap::NewL()
	{
	CWsContainGraphicBitmap* self = new(ELeave) CWsContainGraphicBitmap;
	CleanupStack::PushL(self);
	self->BaseConstructL(KContainDrawerImplId,KNullDesC8());
	self->iIsReady = ETrue;
	CleanupStack::Pop(self);
	return self;
	}

EXPORT_C CWsContainGraphicBitmap* CWsContainGraphicBitmap::NewL(TUid aUid)
	{
	CWsContainGraphicBitmap* self = new(ELeave) CWsContainGraphicBitmap;
	CleanupStack::PushL(self);
	self->BaseConstructL(aUid,KContainDrawerImplId,KNullDesC8());
 	self->iIsReady = ETrue;
  	CleanupStack::Pop(self);
	return self;
	}
	
EXPORT_C CWsContainGraphicBitmap* CWsContainGraphicBitmap::NewL(const TWsGraphicId& aReplace)
	{
	CWsContainGraphicBitmap* self = new(ELeave) CWsContainGraphicBitmap;
	CleanupStack::PushL(self);
    self->BaseConstructL(aReplace,KContainDrawerImplId,KNullDesC8());
  	self->iIsReady = ETrue;
  	CleanupStack::Pop(self);
	return self;
	}

EXPORT_C TInt CWsContainGraphicBitmap::UpdateColor(TRgb aColor)
	{
	if (!iIsReady)
		return KErrNotReady;
	// Send the color the server side
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


EXPORT_C void CWsContainGraphicBitmap::HandleMessage(const TDesC8& /*aData*/)
	{
	}

EXPORT_C void CWsContainGraphicBitmap::OnReplace()
	{
	}

EXPORT_C TInt CWsContainGraphicBitmap::ShareGlobally()
	{
	return CWsGraphic::ShareGlobally();
	}

EXPORT_C TInt CWsContainGraphicBitmap::UnShareGlobally()
	{
	return CWsGraphic::UnShareGlobally();
	}

EXPORT_C TInt CWsContainGraphicBitmap::Share(TSecureId aClientId)
	{
	return CWsGraphic::Share(aClientId);
	}

EXPORT_C TInt CWsContainGraphicBitmap::UnShare(TSecureId aClientId)
	{
	return CWsGraphic::UnShare(aClientId);
	}   
	 
