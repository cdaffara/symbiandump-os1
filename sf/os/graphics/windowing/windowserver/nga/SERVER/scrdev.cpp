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

#include <hal.h>
#include <graphics/wsscreendevice.h>
#include "W32STD.H"
#include "server.h"
#include "gc.h"
#include "rootwin.h"
#include "windowgroup.h"
#include "wstop.h"
#include "EVENT.H"
#include "panics.h"
#include "../CLIENT/w32comm.h"
#include "devicemap.h"
#include <graphics/wsdisplaycontrol.h>
#include "wsdisplaychangeao.h"
#include <graphics/wstestscreencapture.h>
#include <graphics/WSGRAPHICDRAWERINTERFACE.H>

const TInt KEikSrvsSid=0x10003a4a;

static _LIT_SECURITY_POLICY_C1(KSecurityPolicy_WriteDeviceData,ECapabilityWriteDeviceData);
	
/*DWsScreenDevice*/

DWsScreenDevice::DWsScreenDevice(CWsClient* aOwner, TInt aDefaultScreenNumber, TUint aClientScreenDevicePointer)
		: CWsScreenObject(aOwner, WS_HANDLE_SCREEN_DEVICE, CWsTop::Screen( aDefaultScreenNumber ))
		,iClientScreenDevicePointer(aClientScreenDevicePointer)
	{
	MWsScreen* pOI=Screen();
	iDispCont=pOI->ObjectInterface<MWsDisplayControl>();
	iDispMap =pOI->ObjectInterface<MWsDisplayMapping>();
	iTestScreenCapture = pOI->ObjectInterface<MWsTestScreenCapture>();
	}

DWsScreenDevice::~DWsScreenDevice()
	{
	//remove it for display change notification, if it asked for notification
	Screen()->RemoveNotificationClient(iWsOwner);
	}

void DWsScreenDevice::CopyScreenToBitmapL(const TRect &aRect, TInt aHandle)
	{
	iScreen->DoRedrawNow();

	CFbsBitmap *bitmap=new(ELeave) CFbsBitmap();
	CleanupStack::PushL(bitmap);
	TInt ret = bitmap->Duplicate(aHandle);
	if (ret == KErrNoMemory)
		{
		User::Leave(ret);
		}
	if (ret != KErrNone)
		OwnerPanic(EWservPanicBitmap);

	iScreen->ScreenDevice().CopyScreenToBitmapL(bitmap, aRect);
	CleanupStack::PopAndDestroy(bitmap);
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
			if (!iScreen->IsValidScreenSizeMode(*pData.Int))
				OwnerPanic(EWservPanicScreenModeNumber);
			CWsClient::ReplySize(TSize(1,1));
			break;
		case EWsSdOpGetCurrentScreenModeScale:
			CWsClient::ReplySize(TSize(1,1));
			break;
		case EWsSdOpSetAppScreenMode:
			SetAppScreenMode(*pData.Int);
			break;
		case EWsSdOpGetCurrentScreenModeScaledOrigin:
			//scaling is not supported, but origin is.
			CWsClient::ReplyPoint(iScreen->Origin());
			break;
		case EWsSdOpGetScreenModeScaledOrigin:
			{
			if (!iScreen->IsValidScreenSizeMode(*pData.Int))
				OwnerPanic(EWservPanicScreenModeNumber);
			//scaling is not supported, but origin is.
			const TSizeMode& sizeMode=iScreen->ScreenSizeModeData(*pData.Int);
			CWsClient::ReplyPoint(sizeMode.iOrigin);
			}
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
			CWsClient::ReplyPoint(iScreen->Origin());
			break;
		case EWsSdOpGetScreenModeOrigin:
			{
			if (!iScreen->IsValidScreenSizeMode(*pData.Int))
				OwnerPanic(EWservPanicScreenModeNumber);
			const TSizeMode& mode=iScreen->ScreenSizeModeData(*pData.Int);
			CWsClient::ReplyPoint(mode.iOrigin);
			}
			break;
		case EWsSdOpPixel:
			{
			iScreen->DoRedrawNow();
			TRgb rgb;
			iScreen->ScreenDevice().GetPixel(rgb,*pData.Point);
			SetReply(rgb.Internal());
			}
			break;
		case EWsSdOpGetScanLine:
			iScreen->DoRedrawNow();
			iScreen->GetScanLine(pData.GetScanLine);
			break;
		case EWsSdOpTwipsSize:
			CWsClient::ReplySize(AppScreenSizeInTwips()); 
			break;
		case EWsSdOpPixelSize:
			CWsClient::ReplySize(AppScreenSizeInPixels()); 
			break;
		case EWsSdOpHorizontalTwipsToPixels:
			SetReply(iScreen->DeviceMap().HorizontalTwipsToPixels(*pData.UInt));
			break;
		case EWsSdOpVerticalTwipsToPixels:
			SetReply(iScreen->DeviceMap().VerticalTwipsToPixels(*pData.UInt));
			break;
		case EWsSdOpHorizontalPixelsToTwips:
			SetReply(iScreen->DeviceMap().HorizontalPixelsToTwips(*pData.UInt));
			break;
		case EWsSdOpVerticalPixelsToTwips:
			SetReply(iScreen->DeviceMap().VerticalPixelsToTwips(*pData.UInt));
			break;
		case EWsSdOpPointerRect:
			{
			MWsScreen* pOI=iScreen;
			const MWsScreenDevice* screenDevice = pOI->ObjectInterface<MWsScreenDevice>();
			WS_ASSERT_ALWAYS(screenDevice, EWsPanicScreenDeviceMissing);
			CWsClient::ReplyRect(screenDevice->PointerRect());
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
			SetReply(iScreen->DisplayMode());
			break;
		case EWsSdOpRectCompare:
			{
//			if (pData.RectCompare->flags&CWsScreenDevice::EIncludeSprite)
//				{
//				SpriteManager()->IncrementProcessingSprites();
//				}

			RWsTextCursor * cursor = 0;
			if (!(pData.RectCompare->flags&CWsScreenDevice::EIncludeTextCursor))	
				cursor = CWsTop::CurrentTextCursor();
			if (cursor)
				cursor->Disable();
			iScreen->DoRedrawNow();
			SetReply(iScreen->ScreenDevice().RectCompare(pData.RectCompare->rect1, pData.RectCompare->rect2));

			if (cursor)
				cursor->Enable();

//			if (pData.RectCompare->flags&CWsScreenDevice::EIncludeSprite)
//				{
//				SpriteManager()->DecrementProcessingSprites();
//				}
			}
			break;
		case EWsSdOpCopyScreenToBitmap:
			CopyScreenToBitmapL(TRect(iScreen->SizeInPixels()), pData.CopyScreenToBitmap->handle);
			break;
		case EWsSdOpCopyScreenToBitmap2:
			CopyScreenToBitmapL(pData.CopyScreenToBitmap2->rect,pData.CopyScreenToBitmap2->handle);
			break;
		case EWsSdOpPaletteAttributes:
			{
			TInt numEntries = 0;
			MWsScreen* pOI=iScreen;
			MWsPalette* palette = pOI->ObjectInterface<MWsPalette>();

			if (palette)
				{
				TBool modifiable;
				palette->PaletteAttributes(modifiable, numEntries);
				if (modifiable)
					numEntries |= EWsSdSetableBitFlag;
				}
			else
				{
				TInt aGrays;
				iScreen->MaxNumColors(numEntries, aGrays);
				}

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
		case EWsSdOpIsScreenModeDynamic:
			{
			TInt mode = (*pData.Int == -1) ? iAppMode : *pData.Int;
			TInt flags=0;
			TRAPD(err,flags = Screen()->ModeFlagsL(mode));
			if (err == KErrNone && flags & MWsScreenConfigList::EDynamic)
				{
				SetReply(ETrue);
				}
			else
				{
				SetReply(EFalse);
				}
			}
			break;
			
		default:
			ExtensionCommandL(aOpcode, aCmdData);
			break;
		}
	}

void DWsScreenDevice::ExtensionCommandL(TInt aOpcode, const TAny *aCmdData)
	{
	TWsSdCmdUnion pData;
	pData.any=aCmdData;
	switch(aOpcode)
		{
		case EWsSdOpExtensionsSupported:
			{
			TInt reply=0;
			if (iDispCont)
				{
				reply|=TWsSdXDisplayControl;
				}
			if(iDispMap)
				{
				reply|=TWsSdXDisplayMapping;
				}
			MWsScreen* pOI=Screen();
			iTestScreenCapture = pOI->ObjectInterface<MWsTestScreenCapture>();
			if (iTestScreenCapture != NULL)
				{
				reply|=TWsSdXDebugComposition;
				}
			SetReply(reply);
			}
			break; 
		case EWsSdOpXDcGetNumberOfResolutions:
			{
			if (iDispCont)
				{
				SetReply(iDispCont->NumberOfResolutions());
				}
			else
				{
				SetReply(KErrNotSupported);
				}
			}
			break; 
		case EWsSdOpXDcGetResolutionsList:
			{
			if (iDispCont)
				{
				RArray<MWsDisplayControl::TResolution> resolutions;
				TInt err = iDispCont->GetResolutions(resolutions);
				if(err != KErrNone)
					{
					SetReply(err);
					resolutions.Close();
					break;
					}
				TInt bufLength = resolutions.Count()*sizeof(MWsDisplayControl::TResolution);
				SetReply(bufLength);
				 
				if(CWsClient::ReplyBufSpace() >= bufLength)
					{	//must not overflow the client's allocated storage.
					CWsClient::ReplyBuf(&resolutions[0], bufLength);
					}
				resolutions.Close();
				}
			else
				{
				SetReply(KErrNotSupported);
				}
			}
			break;
		case EWsSdOpXDcGetConfiguration:
			{
			if(iDispCont)
				{
				TDisplayConfiguration dispConfig = *pData.DisplayConfiguration;
				if (dispConfig.Version() < sizeof(TDisplayConfiguration))
					{
					//Test not required until there are multiple configuration sizes
					__ASSERT_COMPILE(sizeof(TDisplayConfiguration1)==sizeof(TDisplayConfiguration));
					//if (dispConfig.Version() != sizeof(TDisplayConfiguration1))
						{
						CWsClient::PanicCurrentClient(EWservPanicInvalidDisplayConfiguration);
						}
					}
				iDispCont->GetConfiguration(dispConfig);
				TInt expectedVersion = pData.DisplayConfiguration->Version();
				if (sizeof(TDisplayConfiguration)< expectedVersion)
					expectedVersion = sizeof(TDisplayConfiguration);
				CWsClient::ReplyBuf(&dispConfig, expectedVersion);
				}
			else
				{
				SetReply(KErrNotSupported);
				}
			SetReply(KErrNone);
			}
			break;
		case EWsSdOpXDcSetConfiguration:
			{
			TDisplayConfiguration dispConfig = *pData.DisplayConfiguration;
			if (dispConfig.Version() < TDisplayConfiguration().Version())
				{
				//Test not required until there are multiple configuration sizes
				__ASSERT_COMPILE(sizeof(TDisplayConfiguration1)==sizeof(TDisplayConfiguration));
				//if (dispConfig.Version() != sizeof(TDisplayConfiguration1))
					{
					CWsClient::PanicCurrentClient(EWservPanicInvalidDisplayConfiguration);
					}
				}
			TInt reply=Screen()->SetConfiguration(dispConfig);
			SetReply(reply);
			}
			break;
	
		case EWsSdOpXDcGetPreferredDisplayVersion:
			{
			if (iDispCont)
				{
				SetReply(iDispCont->PreferredDisplayVersion());
				}
			else
				{
				SetReply(KErrNotSupported);
				}
			}
			break;
		case EWsSdOpXDcNotifyOnDisplayChange:
			{
			if(iDispCont)
				{
				SetReply(Screen()->AddNotificationClient(iWsOwner));
				}
			else
				{
				SetReply(KErrNotSupported);
				}
			}
			break;
		case EWsSdOpXDcNotifyOnDisplayChangeCancel:
			{
			if (iDispCont)
				{
				Screen()->RemoveNotificationClient(iWsOwner);
				SetReply(KErrNone);
				}
			else
				{
				SetReply(KErrNotSupported);
				}
			}
			break;
		case EWsSdOpXTestScreenCapture:
			TestScreenCaptureL(pData.any);
			break;
		case EWsSdOpXTestScreenCaptureSize:
		    CWsClient::ReplySize(iScreen->SizeInPixels());
            break;
		case EWsSdOpXDcDisplayChangeEventEnabled:
			{
			if(iDispCont)
				{
				if(Screen()->FindNotificationClient(iWsOwner) != KErrNotFound)
					{
					SetReply(1);
					break;
					}
				}
			
			SetReply(0);
		
			}
			break;
		case EWsSdOpXDmMapExtent:
			{
			if(iDispMap)
				{
				TWsSdCmdMapCoordinates mapCoord = *pData.MapCoordinates;
				TRect targetRect;
				TInt err = iDispMap->MapCoordinates(mapCoord.sourceSpace, mapCoord.sourceRect, mapCoord.targetSpace, targetRect);
				if(err != KErrNone)
					{
					SetReply(err);
					}
				else
					{
					CWsClient::ReplyBuf(&targetRect, sizeof(TRect));
					SetReply(KErrNone);
					}
				}
			else
				{
				SetReply(KErrNotSupported);
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

TSize DWsScreenDevice::AppScreenSizeInPixels() const
	{
	if (iAppScreenSizeInPixels == TSize(-1,-1))
		{
		TSize returnSize;
		TRAP_IGNORE(returnSize = iScreen->ScreenModeSizeInPixelsL(iAppMode));
		return returnSize;
		}
	return iAppScreenSizeInPixels;
	}

TSize DWsScreenDevice::AppScreenSizeInTwips() const
	{
	if (iAppScreenSizeInTwips == TSize(-1,-1))
		{
		TSize returnSize;
		TRAP_IGNORE(returnSize = iScreen->ScreenModeSizeInTwipsL(iAppMode));
		return returnSize;
		}
	return iAppScreenSizeInTwips;
	}

TBool DWsScreenDevice::ScreenDeviceValidState() const
	{
	WS_ASSERT_DEBUG(iScreen->IsValidScreenSizeMode(iScreen->ScreenSizeMode()), EWsPanicInvalidScreenSizeMode);
	const TSizeMode& currentMode=iScreen->ScreenSizeModeData();
	TBool state=ETrue;
	
	// In all enforcement modes scale should match, so check scale first
	if (iScreen->SizeEnforcementMode()!=ESizeEnforcementNone)
		{
		TSize appScreenSizeInPixels = AppScreenSizeInPixels();
		if (currentMode.iRotation!=iAppRotation)
			{
			state=EFalse;
			}
		else if (!(appScreenSizeInPixels == currentMode.iScreenSize))
			{
			state=EFalse;
			}
		}
	if (iScreen->SizeEnforcementMode()==ESizeEnforcementPixelsTwipsAndRotation)
		{
		TSize appScreenSizeInTwips = AppScreenSizeInTwips();
		if (!(appScreenSizeInTwips == currentMode.iScreenTwipsSize))
			{
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
	TInt modeNumber = iScreen->ScreenSizeMode();
	TInt flags = iScreen->ModeFlagsL(modeNumber);
	if (flags & MWsScreenConfigList::EDynamic)
		{
		iAppScreenSizeInPixels = TSize(-1,-1);
		iAppScreenSizeInTwips = TSize(-1,-1);
		}
	else
		{
		iAppScreenSizeInPixels=mode.iScreenSize;
		iAppScreenSizeInTwips=mode.iScreenTwipsSize;
		}
	iAppRotation=mode.iRotation;
	iAppMode=modeNumber;
	}

void DWsScreenDevice::SetScreenSizeAndRotation(const TPixelsTwipsAndRotation &aSar)
	{
	iAppScreenSizeInPixels=aSar.iPixelSize;
	iAppScreenSizeInTwips=aSar.iTwipsSize;
	iAppRotation=aSar.iRotation;
	SetScreenDeviceValidStates(this);
	TInt flags=0;
	TRAP_IGNORE(flags = iScreen->ModeFlagsL(iScreen->ScreenSizeMode()));
	if (flags & MWsScreenConfigList::EDynamic)
		{
		iAppScreenSizeInPixels = TSize(-1,-1);
		iAppScreenSizeInTwips = TSize(-1,-1);
		}
	}

void DWsScreenDevice::SetScreenSizeAndRotation(const TPixelsAndRotation &aSar)
	{
	iAppScreenSizeInPixels=aSar.iPixelSize;
	iAppScreenSizeInTwips.iWidth=iScreen->DeviceMap().HorizontalPixelsToTwips(iAppScreenSizeInPixels.iWidth);
	iAppScreenSizeInTwips.iHeight=iScreen->DeviceMap().VerticalPixelsToTwips(iAppScreenSizeInPixels.iHeight);
	iAppRotation=aSar.iRotation;
	SetScreenDeviceValidStates(this);
	TInt flags=0;
	TRAP_IGNORE(flags = iScreen->ModeFlagsL(iScreen->ScreenSizeMode()));
	if (flags & MWsScreenConfigList::EDynamic)
		{
		iAppScreenSizeInPixels = TSize(-1,-1);
		iAppScreenSizeInTwips = TSize(-1,-1);
		}
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

void DWsScreenDevice::SetAppScreenMode(TInt aMode)
	{
	if (!iScreen->IsValidScreenSizeMode(aMode))
		{
		OwnerPanic(EWservPanicScreenModeNumber);
		}
	const TSizeMode& sizeMode=iScreen->ScreenSizeModeData(aMode);
	TInt flags=0;
	TRAP_IGNORE(flags = iScreen->ModeFlagsL(aMode));
	if (flags & MWsScreenConfigList::EDynamic)
		{
		iAppScreenSizeInPixels = TSize(-1,-1);
		iAppScreenSizeInTwips = TSize(-1,-1);
		}
	else
		{
		iAppScreenSizeInPixels=sizeMode.iScreenSize;
		iAppScreenSizeInTwips=sizeMode.iScreenTwipsSize;
		}
	iAppMode=aMode;
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
		TInt flags=0;
		TRAPD(err,flags = iScreen->ModeFlagsL(mode));
		if (err != KErrNone || flags&MWsScreenConfigList::EDynamic)
			{
			continue;
			}
		const TSizeMode& sizeMode=iScreen->ScreenSizeModeData(mode);
		if (iAppScreenSizeInPixels==sizeMode.iScreenSize && ROTATION_TO_FLAG(iAppRotation)&sizeMode.iAlternativeRotations)
			{
			iAppMode=mode;
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
	MWsScreen* pOI=iScreen;
	MWsPalette* paletteInterface = pOI->ObjectInterface<MWsPalette>();

	if (paletteInterface)
		{
		const TInt size=iWsOwner->ClientMessage().GetDesLength(KRemoteBufferMessageSlot);
		TInt numEntries=size/sizeof(TRgb);
		CPalette* palette=CPalette::NewL(numEntries);
		CleanupStack::PushL(palette);
		TPtr8 paletteData(NULL,0);
		palette->GetDataPtr(0,numEntries,paletteData);
		iWsOwner->RemoteReadL(paletteData,0);
		SetReply(paletteInterface->SetCustomPalette(palette));
		CleanupStack::PopAndDestroy(palette);
		}
	else
		SetReply(KErrNotSupported);
	}

void DWsScreenDevice::GetPalette(TInt aNumColors)
	{
	MWsScreen* pOI=iScreen;
	MWsPalette* paletteInterface = pOI->ObjectInterface<MWsPalette>();

	if (paletteInterface)
		{
		CPalette* palette = NULL;
		TInt ret = paletteInterface->GetPalette(palette);

		if (ret != KErrNone)
			{
			SetReply(ret);
			return;
			}

		ret = palette->Entries();

		if (ret != aNumColors)
			{
			delete palette;
			SetReply(ret);
			return;
			}

		TPtr8 paletteData(NULL,0);
		palette->GetDataPtr(0,ret,paletteData);
		CWsClient::ReplyBuf(paletteData);
		SetReply(KErrNone);
		delete palette;
		}
	else
		SetReply(KErrNotSupported);
	}

TUint DWsScreenDevice::ClientDevicePointer()
	{
	return iClientScreenDevicePointer;	
	}
//Integer version means callers don't need exported panic enumeration
void DWsScreenDevice::PanicCurrentClient(TInt aPanic)
	{
	PanicCurrentClient((TClientPanic)aPanic);
	}

//Allows render stage to initiate a client panic
void DWsScreenDevice::PanicCurrentClient(TClientPanic aPanic)
	{
	CWsClient::PanicCurrentClient(aPanic);
	}

void DWsScreenDevice::SetReply(TInt aReply)
	{
	CWsObject::SetReply(aReply);
	}

void DWsScreenDevice::RedrawNowIfPending()
	{
	iScreen->RedrawNowIfPending();
	}

void DWsScreenDevice::ReplyBuf(const TAny* aSource, TInt aLength)
	{
	TPtrC8 src(reinterpret_cast<const TUint8*>(aSource),aLength);
	CWsClient::ReplyBuf(src);
	}

void DWsScreenDevice::TestScreenCaptureL(const TAny* aMessage)
	{
	if (iTestScreenCapture == NULL)
		{
		// at this point we expect that Screen Capture Interface exists always
		OwnerPanic(EWservPanicScreenCaptureInvalidRequest);
		SetReply(KErrNotSupported);
		}
	iTestScreenCapture->TestScreenCaptureL(aMessage, *static_cast<MWsScreenDeviceUtils*>(this));
	}
