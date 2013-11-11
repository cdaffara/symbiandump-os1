// Copyright (c) 1997-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef __SCDRAW_H__
#define __SCDRAW_H__

#include <e32svr.h>
#include <e32wins.h>

#if defined(SYMBIAN_GRAPHICS_GCE)
#include <graphics/surfaceupdateclient.h>
#include <bitdrawsurface.h>
#endif //SYMBIAN_GRAPHICS_GCE

#include "BMDRAW.H"
#include "_WININC.H"

/**
@internalComponent
*/
template <class T>
class CScreenDevice : public T
	{
public:
	TInt ConstructScreen(TInt aScreenNo, TAny* aHwnd,const TSize& aSize);
protected:
	virtual ~CScreenDevice();
	void SetScreenOrientation(TInt aOrientation);
	TUint8* WinPixelAddress(TInt aX,TInt aY) const;
private:
	// From CFbsDrawDevice
	virtual TInt HorzTwipsPerThousandPixels() const;
	virtual TInt VertTwipsPerThousandPixels() const;
	virtual void OrientationsAvailable(TBool aOrientation[4]);
	virtual void SetAutoUpdate(TBool aAutoUpdate);
	virtual void SetDisplayMode(CFbsDrawDevice* aDrawDevice);
	virtual void Update();
	virtual void Update(const TRegion& aUpdateRegion);
	virtual void UpdateRegion(const TRect& aRect);
	virtual void UpdateScreen(const TRegion& aRegion);
	// New functions
	virtual void UpdateRect(const TRect& aRect) const = 0;
protected:
	CFbsDrawDevice::TOrientation iEmulatorOrientation;
    TInt iScreenNo;
private:
	RWindows* iWindow;
	RSemaphore iWindowInUse;
	TBool iAutoUpdate;
	RRegion iUpdateRegion;
#if defined(SYMBIAN_GRAPHICS_GCE)
	RSurfaceUpdateSession iSurfaceUpdateSession;
	TInt iSurfaceUpdateError;
#endif //SYMBIAN_GRAPHICS_GCE
	};

#include "SCDRAW.INL"	// implementation of CScreenDevice<>

/**
@internalComponent
*/
class CDrawOneBppScreenBitmap : public CScreenDevice<CDrawOneBppBitmap>
	{
private:
	// From CFbsDrawDevice
	void OrientationsAvailable(TBool aOrientation[4]);
	//virtual function from CScreenDevice
	void UpdateRect(const TRect& aRect) const;
	//new functions
	virtual TInt InitScreen();
	};

/**
@internalComponent
*/
class CDrawTwoBppScreenBitmap : public CScreenDevice<CDrawTwoBppBitmap>
	{
private:
	// From CFbsDrawDevice
	void OrientationsAvailable(TBool aOrientation[4]);
	//virtual function from CScreenDevice
	void UpdateRect(const TRect& aRect) const;
	//new functions
	virtual TInt InitScreen();
	};

/**
@internalComponent
*/
class CDrawFourBppScreenBitmapGray : public CScreenDevice<CDrawFourBppBitmapGray>
	{
private:
	// From CFbsDrawDevice
	void OrientationsAvailable(TBool aOrientation[4]);
	//virtual function from CScreenDevice
	void UpdateRect(const TRect& aRect) const;
	//new functions
	virtual TInt InitScreen();
	};

/**
@internalComponent
*/
class CDrawFourBppScreenBitmapColor : public CScreenDevice<CDrawFourBppBitmapColor>
	{
private:
	// From CFbsDrawDevice
	void OrientationsAvailable(TBool aOrientation[4]);
	//virtual function from CScreenDevice
	void UpdateRect(const TRect& aRect) const;
	//new functions
	virtual TInt InitScreen();
	};

/**
@internalComponent
*/
class CDrawEightBppScreenBitmapGray : public CScreenDevice<CDrawEightBppBitmapGray>
	{
private:
	//virtual function from CDrawBitmap
	TBool SetOrientation(TOrientation aOrientation);
	//virtual function from CScreenDevice
	void UpdateRect(const TRect& aRect) const;
	//new functions
	virtual TInt InitScreen();
	};

/**
@internalComponent
*/
class CDrawEightBppScreenBitmapColor : public CScreenDevice<CDrawEightBppBitmapColor>
	{
private:
	//virtual function from CDrawEightBppBitmapColor
	TInt SetCustomPalette(const CPalette* aPalette);
	//virtual function from CDrawBitmap
	TBool SetOrientation(TOrientation aOrientation);
	//virtual function from CScreenDevice
	void UpdateRect(const TRect& aRect) const;
	//new functions
	virtual TInt InitScreen();
	};

/**
@internalComponent
*/
class CDrawTwelveBppScreenBitmap : public CScreenDevice<CDrawTwelveBppBitmap>
	{
private:
	//virtual function from CDrawBitmap
	TBool SetOrientation(TOrientation aOrientation);
	//virtual function from CScreenDevice
	void UpdateRect(const TRect& aRect) const;
	//new functions
	virtual TInt InitScreen();
	};

/**
@internalComponent
*/
class CDrawSixteenBppScreenBitmap : public CScreenDevice<CDrawSixteenBppBitmap>
	{
private:
	//virtual function from CDrawBitmap
	TBool SetOrientation(TOrientation aOrientation);
	//virtual function from CScreenDevice
	void UpdateRect(const TRect& aRect) const;
	//new functions
	virtual TInt InitScreen();
	};

/**
@internalComponent
*/
class CDrawTwentyFourBppScreenBitmap : public CScreenDevice<CDrawTwentyFourBppBitmap>
	{
private:
	//virtual function from CDrawBitmap
	TBool SetOrientation(TOrientation aOrientation);
	//virtual function from CScreenDevice
	void UpdateRect(const TRect& aRect) const;
	//new functions
	virtual TInt InitScreen();
	};

/**
@internalComponent
*/
class CDrawUTwentyFourBppScreenBitmap : public CScreenDevice<CDrawUTwentyFourBppBitmap>
	{
private:
	//virtual function from CDrawBitmap
	TBool SetOrientation(TOrientation aOrientation);
	//virtual function from CScreenDevice
	void UpdateRect(const TRect& aRect) const;
	//new functions
	virtual TInt InitScreen();
	};

/**
@internalComponent
*/
class CDrawThirtyTwoBppScreenBitmapAlpha : public CScreenDevice<CDrawThirtyTwoBppBitmapAlpha>
	{
private:
	//virtual function from CDrawBitmap
	TBool SetOrientation(TOrientation aOrientation);
	//virtual function from CScreenDevice
	void UpdateRect(const TRect& aRect) const;
	//new functions
	virtual TInt InitScreen();
	};

/**
@internalComponent
*/
class CDrawThirtyTwoBppScreenBitmapAlphaPM : public CScreenDevice<CDrawThirtyTwoBppBitmapAlphaPM>
	{
private:
	//virtual function from CDrawBitmap
	TBool SetOrientation(TOrientation aOrientation);
	//virtual function from CScreenDevice
	void UpdateRect(const TRect& aRect) const;
	//new functions
	virtual TInt InitScreen();
	};

#endif	//__SCDRAW_H__

