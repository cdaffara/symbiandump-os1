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
// Screen device
// 
//

#include "ScrDev.H"
#include "Direct.H"

#include <e32hal.h>
#include <hal.h>
#include "server.h"
#include "gc.h"
#include "rootwin.h"
#include "windowgroup.h"
#include "wstop.h"
#include "EVENT.H"
#include "panics.h"
#include "../CLIENT/w32comm.h"
#include <w32std.h>

const TInt KEikSrvsSid=0x10003a4a;

static _LIT_SECURITY_POLICY_C1(KSecurityPolicy_WriteDeviceData,ECapabilityWriteDeviceData);
	
/*DWsScreenDevice*/

DWsScreenDevice::DWsScreenDevice(CWsClient* aOwner, TInt aDefaultScreenNumber, TUint aClientScreenDevicePointer) 
		: CWsScreenObject(aOwner, WS_HANDLE_SCREEN_DEVICE, CWsTop::Screen( aDefaultScreenNumber )), 
		  iClientScreenDevicePointer(aClientScreenDevicePointer)
	{}

DWsScreenDevice::~DWsScreenDevice()
	{}

void DWsScreenDevice::CopyScreenToBitmapL(const TRect &aRect, TInt aHandle)
	{
	CFbsBitmap *bitmap=new(ELeave) CFbsBitmap();
	CleanupStack::PushL(bitmap);
	TInt ret = bitmap->Duplicate(aHandle);
	if (ret == KErrNoMemory)
		{
		User::Leave(ret);
		}
	if (ret != KErrNone)
		OwnerPanic(EWservPanicBitmap);

	CFbsBitmapDevice *device=CFbsBitmapDevice::NewL(bitmap);
	CleanupStack::PushL(device);
	CFbsBitGc *gc;
	User::LeaveIfError(device->CreateContext(gc));
	CleanupStack::PushL(gc);			//Don't need to push this, but this is probably the eaiest way to delete it
	gc->SetDrawMode( CGraphicsContext::EDrawModeWriteAlpha );
	gc->BitBlt(TPoint(), *iScreen->ScreenGdi(), aRect);
	CleanupStack::PopAndDestroy(3);
	}

void DWsScreenDevice::CommandL(TInt aOpcode, const TAny *aCmdData)
	{
	TWsSdCmdUnion pData;
	pData.any=aCmdData;
	switch(aOpcode)
		{
		case EWsSdOpGetNumScreenModes:
			SetReply(iScreen->NumScreenSizeModes());
			break;
		case EWsSdOpGetScreenMode:
			SetReply(iScreen->ScreenSizeMode());
			break;
		case EWsSdOpSetScreenMode:
			{
			if(!KSecurityPolicy_WriteDeviceData().CheckPolicy(iWsOwner->ClientMessage(),__PLATSEC_DIAGNOSTIC_STRING("Capability check failed for CWsScreenDevice::SetScreenMode API")))
				{
				iWsOwner->PPanic(EWservPanicPermissionDenied);
				}
			SetScreenMode(*pData.Int);
			}
			break;
		case EWsSdOpSetModeRotation:
			{
			if(KSecurityPolicy_WriteDeviceData().CheckPolicy(iWsOwner->ClientMessage(),__PLATSEC_DIAGNOSTIC_STRING("Capability check failed for CWsScreenDevice::SetModeRotation API")))
				{
				TClientPanic panic=iScreen->SetModeRotation(pData.SetScreenRotation->mode,pData.SetScreenRotation->rotation);
				if (panic)
					OwnerPanic(panic);
				}
			}
			break;
		case EWsSdOpGetRotationList:
			if (!iScreen->IsValidScreenSizeMode(*pData.Int))
				OwnerPanic(EWservPanicScreenModeNumber);
			SetReply(iScreen->ScreenSizeModeData(*pData.Int).iAlternativeRotations);
			break;
		case EWsSdOpGetScreenModeSizeAndRotation:
			GetScreenSizeAndRotationCmd(*pData.Int);
			break;
		case EWsSdOpGetScreenModeSizeAndRotation2:
			GetScreenSizeAndRotationCmd2(*pData.Int);
			break;
		case EWsSdOpSetScreenSizeAndRotation:
			SetScreenSizeAndRotation(*pData.PixelsTwipsAndRotation);
			break;
		case EWsSdOpSetScreenSizeAndRotation2:
			SetScreenSizeAndRotation(*pData.PixelsAndRotation);
			break;
		case EWsSdOpGetDefaultScreenSizeAndRotation:
			GetScreenSizeAndRotationCmd(iScreen->ScreenSizeMode());
			break;
		case EWsSdOpGetDefaultScreenSizeAndRotation2:
			GetScreenSizeAndRotationCmd2(iScreen->ScreenSizeMode());
			break;
		case EWsSdOpGetScreenModeDisplayMode:
			GetScreenModeDisplayMode(*pData.Int);
			break;
		case EWsSdOpGetScreenModeScale:
			GetScreenScale(*pData.Int);
			break;
		case EWsSdOpGetCurrentScreenModeScale:
			GetScreenScale(iScreen->ScreenSizeMode());
			break;
		case EWsSdOpSetAppScreenMode:
			SetAppScreenMode(*pData.Int);
			break;
		case EWsSdOpGetCurrentScreenModeScaledOrigin:
			CWsClient::ReplyPoint(iScreen->CurrentScreenModeScaledOrigin());
			break;
		case EWsSdOpGetScreenModeScaledOrigin:
			if (!iScreen->IsValidScreenSizeMode(*pData.Int))
				OwnerPanic(EWservPanicScreenModeNumber);
			CWsClient::ReplyPoint(iScreen->ScreenModeScaledOrigin(*pData.Int));
			break;
		case EWsSdOpGetCurrentScreenModeAttributes:
			GetCurrentScreenModeAttributes();
			break;
		case EWsSdOpSetCurrentScreenModeAttributes:
			if(!KSecurityPolicy_WriteDeviceData().CheckPolicy(iWsOwner->ClientMessage(),__PLATSEC_DIAGNOSTIC_STRING("Capability check failed for CWsScreenDevice::SetCurrentScreenModeAttributes API, API should be used for test purposes only")))
				{
				iWsOwner->PPanic(EWservPanicPermissionDenied);
				}
			SetCurrentScreenModeAttributes(*pData.ScreenSizeMode);
			break;
		case EWsSdOpSetScreenModeEnforcement:
			{
			if(!KSecurityPolicy_WriteDeviceData().CheckPolicy(iWsOwner->ClientMessage(),__PLATSEC_DIAGNOSTIC_STRING("Capability check failed for CWsScreenDevice::SetScreenModeEnforcement API")))
				{
				iWsOwner->PPanic(EWservPanicPermissionDenied);
				}
			if (!iScreen->SetScreenModeEnforcement(*pData.Int))
				OwnerPanic(EWservPanicScreenEnforcementMode);
			}
			break;
		case EWsSdOpScreenModeEnforcement:
			SetReply(iScreen->SizeEnforcementMode());
			break;
		case EWsSdOpGetDefaultScreenModeOrigin:
			GetScreenModeOriginCmd(iScreen->ScreenSizeMode());
			break;
		case EWsSdOpGetScreenModeOrigin:
			GetScreenModeOriginCmd(*pData.Int);
			break;
		case EWsSdOpPixel:
			{
			TRgb rgb;
			iScreen->ScreenDevice()->GetPixel(rgb,*pData.Point);
			SetReply(rgb.Internal());
			}
			break;
		case EWsSdOpGetScanLine:
			iScreen->GetScanLine(pData.GetScanLine);
			break;
		case EWsSdOpTwipsSize:
			CWsClient::ReplySize(iAppScreenSizeInTwips);
			break;
		case EWsSdOpPixelSize:
			CWsClient::ReplySize(iAppScreenSizeInPixels);
			break;
		case EWsSdOpHorizontalTwipsToPixels:
			SetReply(iScreen->ScreenDevice()->HorizontalTwipsToPixels(*pData.UInt));
			break;
		case EWsSdOpVerticalTwipsToPixels:
			SetReply(iScreen->ScreenDevice()->VerticalTwipsToPixels(*pData.UInt));
			break;
		case EWsSdOpHorizontalPixelsToTwips:
			SetReply(iScreen->ScreenDevice()->HorizontalPixelsToTwips(*pData.UInt));
			break;
		case EWsSdOpVerticalPixelsToTwips:
			SetReply(iScreen->ScreenDevice()->VerticalPixelsToTwips(*pData.UInt));
			break;
		case EWsSdOpPointerRect:
			{
			TMachineInfoV1Buf macInfo;
			UserHal::MachineInfo(macInfo);
			TRect rect(-macInfo().iOffsetToDisplayInPixels,macInfo().iXYInputSizeInPixels);
			CWsClient::ReplyRect(rect);
			}
			break;
		case EWsSdOpFree:
			{
			// Mark any group windows associated with the screen device being deleted
			CWsRootWindow* root=iScreen->RootWindow();
			for (CWsWindowGroup* grp = root->Child(); grp; grp = grp->NextSibling())
				{
				if (grp->Device() == this)
					{
#if defined(_DEBUG)
 					grp->SetScreenDeviceDeleted();
#endif
					grp->SetScreenDevice(NULL);
					}
				}
			}
			// Let our owner (CWsClient) know that a screen device is being deleted so it
			// can check whether to reset its iPrimaryScreenDevice member or not
   			if (WsOwner())	
 				WsOwner()->NotifyScreenDeviceDeleted(this);
			delete this;
			break;
		case EWsSdOpDisplayMode:
			iScreen->ScreenGdi();	// To force display mode update if required
			SetReply(iScreen->CurrentDisplayMode());
			break;
		case EWsSdOpRectCompare:
			{
//			if (pData.RectCompare->flags&CWsScreenDevice::EIncludeSprite)
//				{
//				SpriteManager()->IncrementProcessingSprites();
//				}

			RWsTextCursor * cursor = 0;
			if (!pData.RectCompare->flags&CWsScreenDevice::EIncludeTextCursor)	
				cursor = CWsTop::CurrentTextCursor();
			if (cursor)
				cursor->Disable();
			
			SetReply(iScreen->ScreenDevice()->RectCompare(pData.RectCompare->rect1,*iScreen->ScreenDevice(),pData.RectCompare->rect2));

			if (cursor)
				cursor->Enable();
			
//			if (pData.RectCompare->flags&CWsScreenDevice::EIncludeSprite)
//				{
//				SpriteManager()->DecrementProcessingSprites();
//				}
			}
			break;
		case EWsSdOpCopyScreenToBitmap:
			CopyScreenToBitmapL(TRect(iScreen->ScreenDevice()->SizeInPixels()),pData.CopyScreenToBitmap->handle);
			break;
		case EWsSdOpCopyScreenToBitmap2:
			CopyScreenToBitmapL(pData.CopyScreenToBitmap2->rect,pData.CopyScreenToBitmap2->handle);
			break;
		case EWsSdOpPaletteAttributes:
			{
			TBool modifiable;
			TInt numEntries;
			iScreen->ScreenDevice()->PaletteAttributes(modifiable,numEntries);
			if (modifiable)
				numEntries|=EWsSdSetableBitFlag;
			SetReply(numEntries);
			}
			break;
		case EWsSdOpSetPalette:
			{
			if(!KSecurityPolicy_WriteDeviceData().CheckPolicy(iWsOwner->ClientMessage(),__PLATSEC_DIAGNOSTIC_STRING("Capability check failed for CWsScreenDevice::SetPalette API")))
				{
				User::Leave(KErrPermissionDenied);
				}
			SetPaletteL();
			}
			break;
		case EWsSdOpGetPalette:
			GetPalette(*pData.Int);
			break;
			
		case EWsSdOpGetScreenNumber:
			SetReply(iScreen->ScreenNumber());
			break;
		case EWsSdOpGetScreenSizeModeList:
			SetReply(iScreen->GetScreenSizeModeListL());
			break;
		case EWsClOpSetBackLight:
			{
			TInt err=KErrNone;
			if(Screen()->BackLightFlag() && iWsOwner->ClientMessage().SecureId()==KEikSrvsSid)
				{
				TWindowServerEvent::ProcessErrorMessages(TWsErrorMessage::EBackLight, err=HAL::Set(iScreen->ScreenNumber(),HALData::EBacklightState,*pData.Int));
				SetReply(err);
				}
			else
				{
				User::Leave(KErrPermissionDenied);				
				}
			}
			break;
		default:
			OwnerPanic(EWservPanicOpcode);
			break;
		}
	}


void DWsScreenDevice::SetScreenMode(TInt aMode)
	{
	if (!iScreen->IsValidScreenSizeMode(aMode))
		OwnerPanic(EWservPanicScreenModeNumber);
	iScreen->doSetScreenMode(aMode);
	}

TBool DWsScreenDevice::ScreenDeviceValidState() const
	{
	WS_ASSERT_DEBUG(iScreen->IsValidScreenSizeMode(iScreen->ScreenSizeMode()), EWsPanicInvalidScreenSizeMode);
	const TSizeMode& currentMode=iScreen->ScreenSizeModeData();
	TBool state=ETrue;
	// In all enforcement modes scale should match, so check scale first
	if (currentMode.iScreenScale!=iAppScale)
		{
		state=EFalse;
		}
	else
		{
		if (iScreen->SizeEnforcementMode()!=ESizeEnforcementNone)
			{
			if (currentMode.iRotation!=iAppRotation)
				state=EFalse;
			else if (currentMode.iScreenSize!=iAppScreenSizeInPixels)
				state=EFalse;
			}
		if (iScreen->SizeEnforcementMode()==ESizeEnforcementPixelsTwipsAndRotation)
			{
			if (currentMode.iScreenTwipsSize!=iAppScreenSizeInTwips)
				state=EFalse;
			}
		}
	return(state);
	}

void DWsScreenDevice::ConstructL()
//
// App specific construct
//
	{
	NewObjL();
	const TSizeMode& mode=iScreen->ScreenSizeModeData(iScreen->ScreenSizeMode());
	iAppScreenSizeInPixels=mode.iScreenSize;
	iAppScreenSizeInTwips=mode.iScreenTwipsSize;
	iAppRotation=mode.iRotation;
	iAppMode=iScreen->ScreenSizeMode();
	iAppScale=mode.iScreenScale;
	}

void DWsScreenDevice::SetScreenSizeAndRotation(const TPixelsTwipsAndRotation &aSar)
	{
	iAppScreenSizeInPixels=aSar.iPixelSize;
	iAppScreenSizeInTwips=aSar.iTwipsSize;
	iAppRotation=aSar.iRotation;
	SetScreenDeviceValidStates(this);
	}

void DWsScreenDevice::SetScreenSizeAndRotation(const TPixelsAndRotation &aSar)
	{
	iAppScreenSizeInPixels=aSar.iPixelSize;
	iAppScreenSizeInTwips.iWidth=iScreen->ScreenDevice()->HorizontalPixelsToTwips(iAppScreenSizeInPixels.iWidth);
	iAppScreenSizeInTwips.iHeight=iScreen->ScreenDevice()->VerticalPixelsToTwips(iAppScreenSizeInPixels.iHeight);
	iAppRotation=aSar.iRotation;
	SetScreenDeviceValidStates(this);
	}

void DWsScreenDevice::GetScreenModeOriginCmd(TInt aMode)
	{
	if (!iScreen->IsValidScreenSizeMode(aMode))
		OwnerPanic(EWservPanicScreenModeNumber);
	CWsClient::ReplyPoint(iScreen->ScreenModeOrigin(aMode));
	}

void DWsScreenDevice::GetCurrentScreenModeAttributes()
	{
	TSizeMode aModeData=iScreen->ScreenSizeModeData();
	CWsClient::ReplyBuf(&aModeData,sizeof(aModeData));
	}

void DWsScreenDevice::SetCurrentScreenModeAttributes(const TSizeMode &aModeData)
	{
#if defined(_DEBUG)
	if (aModeData.iScreenScale.iWidth==0 || aModeData.iScreenScale.iHeight==0)
		OwnerPanic(EWservPanicScreenModeNumber);
#endif
	iScreen->SetCurrentScreenModeAttributes(aModeData);
	}

void DWsScreenDevice::GetScreenSizeAndRotationCmd(TInt aMode)
	{
	if (!iScreen->IsValidScreenSizeMode(aMode))
		OwnerPanic(EWservPanicScreenModeNumber);
	TPixelsTwipsAndRotation sar;
	iScreen->GetScreenSizeAndRotation(sar,aMode);
	CWsClient::ReplyBuf(&sar, sizeof(sar));
	}

void DWsScreenDevice::GetScreenSizeAndRotationCmd2(TInt aMode)
	{
	if (!iScreen->IsValidScreenSizeMode(aMode))
		OwnerPanic(EWservPanicScreenModeNumber);
	TPixelsAndRotation sar;
	iScreen->GetScreenSizeAndRotation(sar,aMode);
	CWsClient::ReplyBuf(&sar, sizeof(sar));
	}

void DWsScreenDevice::GetScreenModeDisplayMode(const TInt aMode)
	{
	if (!iScreen->IsValidScreenSizeMode(aMode))
		{
		OwnerPanic(EWservPanicScreenModeNumber);
		}
	SetReply(STATIC_CAST(TInt,iScreen->DefaultDisplayMode(aMode)));
	}

void DWsScreenDevice::GetScreenScale(TInt aMode)
	{
	if (!iScreen->IsValidScreenSizeMode(aMode))
		{
		OwnerPanic(EWservPanicScreenModeNumber);
		}
	CWsClient::ReplySize(iScreen->GetScreenScale(aMode));
	}

void DWsScreenDevice::SetAppScreenMode(TInt aMode)
	{
	if (!iScreen->IsValidScreenSizeMode(aMode))
		{
		OwnerPanic(EWservPanicScreenModeNumber);
		}
	const TSizeMode& sizeMode=iScreen->ScreenSizeModeData(aMode);
	iAppScreenSizeInPixels=sizeMode.iScreenSize;
	iAppScreenSizeInTwips=sizeMode.iScreenTwipsSize;
	iAppMode=aMode;
	iAppScale=sizeMode.iScreenScale;
	iAppRotation=sizeMode.iRotation;
	CWsWindowGroup::SetScreenDeviceValidStates(this);
	}

#define ROTATION_TO_FLAG(x) 1<<x

void DWsScreenDevice::SetScreenDeviceValidStates(const DWsScreenDevice *aDevice)
	{
	TInt modes=iScreen->NumScreenSizeModes();
	TInt mode;
	for (mode=0;mode<modes;++mode)
		{
		if (!iScreen->IsValidScreenSizeMode(mode))
			continue;
		const TSizeMode& sizeMode=iScreen->ScreenSizeModeData(mode);
		if (iAppScreenSizeInPixels==sizeMode.iScreenSize && ROTATION_TO_FLAG(iAppRotation)&sizeMode.iAlternativeRotations)
			{
			iAppMode=mode;
			iAppScale=sizeMode.iScreenScale;
			break;
			}
		}
	CWsWindowGroup::SetScreenDeviceValidStates(aDevice);
	}

void DWsScreenDevice::NewOrientation(TInt aMode,CFbsBitGc::TGraphicsOrientation aRotation)
	{
	if (iAppMode==aMode)
		iAppRotation=aRotation;
	}

void DWsScreenDevice::SetPaletteL()
	{
	const TInt size=iWsOwner->ClientMessage().GetDesLength(KRemoteBufferMessageSlot);
	TInt numEntries=size/sizeof(TRgb);
	CPalette* palette=CPalette::NewL(numEntries);
	CleanupStack::PushL(palette);
	TPtr8 paletteData(NULL,0);
	palette->GetDataPtr(0,numEntries,paletteData);
	iWsOwner->RemoteReadL(paletteData,0);
	SetReply(iScreen->ScreenDevice()->SetCustomPalette(palette));
	iScreen->Update((RWsRegion&)RootWindow()->WindowArea());
	CleanupStack::PopAndDestroy(palette);
	}

void DWsScreenDevice::GetPalette(TInt aNumColors)
	{
	CPalette* palette;
	TInt ret=iScreen->ScreenDevice()->GetPalette(palette);
	if (ret<KErrNone)
		goto Reply;
	ret=palette->Entries();
	if (ret!=aNumColors)
		{
		delete palette;
Reply:
		SetReply(ret);
		return;
		}
	TPtr8 paletteData(NULL,0);
	palette->GetDataPtr(0,ret,paletteData);
	CWsClient::ReplyBuf(paletteData);
	SetReply(KErrNone);
	delete palette;
	}

TUint DWsScreenDevice::ClientDevicePointer()
	{
	return iClientScreenDevicePointer;	
	}
