// Copyright (c) 2000-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// Defines the screen device class
// 
//

#ifndef __SCRDEV_H__
#define __SCRDEV_H__

#ifndef __E32STD_H__
#include <e32std.h>
#endif
#ifndef __E32BASE_H__
#include <e32base.h>
#endif
#ifndef __OBJECT_H__
#include "OBJECT.H"
#endif
#ifndef __SCREEN_H__
#include "screen.h"
#endif

class DWsBitmap : public CWsObject
	{
public:
	DWsBitmap(CWsClient *aOwner);
	void ConstructL(const TWsClCmdCreateBitmap &aParams);
	inline CFbsBitmap *FbsBitmap() const;
	void CommandL(TInt aOpcode, const TAny *aCmdData);
	void CloseObject();
	void IncRefCount();
	void DecRefCount();
private:
	~DWsBitmap();
private:
	enum // Flags
		{
		EWsBitmapClientObjectFreed = 0x00000001 // Set if the associated CWsBitmap object has been deleted
		};
private:
	CFbsBitmap *iFbsBitmap;
	TInt iRefCount;
	TUint32 iFlags;
	};

class DWsScreenDevice : public CWsScreenObject
	{
public:
	DWsScreenDevice(CWsClient* aOwner, TInt aDefaultScreenNumber, TUint aClientScreenDevicePointer);
	~DWsScreenDevice();
	void CommandL(TInt aOpcode, const TAny *aCmdData);
	void ConstructL();
	void GetDefaultScreenSizeAndRotationCmd();
	void GetDefaultScreenSizeAndRotationCmd2();
	void GetScreenSizeAndRotationCmd(TInt aMode);
	void GetScreenSizeAndRotationCmd2(TInt aMode);
	void GetScreenModeOriginCmd(TInt aMode);
	void GetScreenScale(TInt aMode);
	void SetAppScreenMode(TInt aMode);
	void SetScreenSizeAndRotation(const TPixelsTwipsAndRotation &aSar);
	void SetScreenSizeAndRotation(const TPixelsAndRotation &aSar);
	TBool ScreenDeviceValidState() const;
	void SetPaletteL();
	void GetPalette(TInt aNumColors);
	inline TPoint ScreenModeOrigin();
	void GetCurrentScreenModeAttributes();
	void SetCurrentScreenModeAttributes(const TSizeMode &aModeData);
//
	inline CFbsBitGc::TGraphicsOrientation Orientation();
	void NewOrientation(TInt aMode,CFbsBitGc::TGraphicsOrientation aRotation);
	void SetScreenMode(TInt aMode);
	inline TInt AppMode();
	void GetScreenModeDisplayMode(TInt aMode);
	inline TDisplayMode DefaultDisplayMode();
	TUint ClientDevicePointer();
private:
	void CopyScreenToBitmapL(const TRect &aRect, TInt aHandle);
	void SetScreenDeviceValidStates(const DWsScreenDevice *aDevice);
private:
// Non-static app specific data
	TSize iAppScreenSizeInPixels;
	TSize iAppScreenSizeInTwips;
	CFbsBitGc::TGraphicsOrientation iAppRotation;
	TInt iAppMode;
	TSize iAppScale;
	TUint iClientScreenDevicePointer; //This is the pointer to the screen device on the client side CWsScreenDevice
	};


//
// inlines			//
//

//
// DWsScreenDevice
//
inline CFbsBitGc::TGraphicsOrientation DWsScreenDevice::Orientation()
	{return iAppRotation;}
inline TInt DWsScreenDevice::AppMode()
	{return iAppMode;}
inline TPoint DWsScreenDevice::ScreenModeOrigin()
	{return iScreen->ScreenModeOrigin(iAppMode);}
inline TDisplayMode DWsScreenDevice::DefaultDisplayMode()
	{return iScreen->DefaultDisplayMode(iAppMode);}

//
// DWsBitmap
//
inline CFbsBitmap* DWsBitmap::FbsBitmap() const
	{return(iFbsBitmap);}

#endif
