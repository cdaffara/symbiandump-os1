// Copyright (c) 2006-2010 Nokia Corporation and/or its subsidiary(-ies).
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
// This module define the class and constants used for the different display
// mode supported.
// Include files                                                   
// 
//

/**
 @file
 @internalComponent
 @prototype
*/
/********************************************************************/
#ifndef __SCDRAW_H__
#define __SCDRAW_H__

#if !defined(__E32SVR_H__)
    #include <e32svr.h>
#endif /* __E32SVR_H__ */

#include <graphics/surfaceupdateclient.h>
#include "BMDRAW.H"
#include "BitDrawInterfaceId.h"
#include "bitdrawsurface.h"
#include <graphics/surface.h>
#include <pixelformats.h>


class TSurfaceId;

/**
The maximum number of rectangles to be held in the update region. This must be
at least 3, since it has to be able to hold the union of two rectangles using
disjoint rectangles.
*/
const TInt		KMaxUpdateRegionRectangles	= 10;

/**
 * Provide common features shared between screen devices. Included by composition
 * rather than inheritance, to avoid multiple inheritance of implementation.
 */
NONSHARABLE_CLASS(CScreenDeviceHelper) : public CBase
	{
public:
	// Default constructor.
	TInt Construct(TInt aScreenNo, TUidPixelFormat aPixelFormat, TUint aHalMode);
	~CScreenDeviceHelper();

	inline TInt ScreenNumber() const { return iSurface.iInternal[TSurfaceId::TScreenSurfaceUsage::EScreenField]; }

	void Update();
	void Update(TRequestStatus& aStatus);

	void UpdateRegion(const TRect& aRect);
	void ResetUpdateRegion();
	void NotifyWhenAvailable(TRequestStatus& aStatus);
	void CancelUpdateNotification();

	void GetSurface(TSurfaceId& aSid) const ;
	TUint DeviceOrientationsAvailable(const TSize& aScreenSize) const ;
	TUint BytesPerScanline() const;
	void* AddressFirstPixel() const;

	TBool SetDeviceOrientation(TDeviceOrientation aOrientation, TSize& aNewSize);
	
	TDeviceOrientation DeviceOrientation() const ;
	TBool	DeviceFlipped() const;

	TInt	HorzTwipsPerThousandPixels(const TSize& aPixels)const; 
	TInt	VertTwipsPerThousandPixels(const TSize& aPixels)const; 
	
protected:
	template <class Type> Type SecondIfFlipped(Type aValueUnFlipped,Type aValueFlipped)const
		{
		if (DeviceFlipped())
			{
			return aValueFlipped;
			}
		else
			{
			return aValueUnFlipped;
			}
		}
	TBool SetDeviceFlipMode(TBool aFlip, TSize& aNewSize);
	inline static TBool ConvertFlip(TDeviceOrientation aOrientation)
		{
		return (aOrientation&(EDeviceOrientation90CW|EDeviceOrientation270CW))!=0;
		}
		
private:
    TSurfaceId iSurface;
    TDeviceOrientation iAssignedOrientation;
	RSurfaceUpdateSession iSurfaceUpdateSession;
	TRegionFix<KMaxUpdateRegionRectangles> iUpdateRegion;
	RChunk iChunk;
	TBool  iHasChunk;
	};

/**
This template class is a helper to genericise the common implementation of the screen device.
This class implements all the non-specific implementation.
**/
template <class TCDrawXxxBppBitmap> 
class CGenericScreenDevice : public TCDrawXxxBppBitmap , public MSurfaceId
	{
public:
	typedef TCDrawXxxBppBitmap		CDrawXxxBppBitmap;
	virtual TInt  InitScreen() ;
	virtual TInt  ConstructScreen(TInt aScreenNo, TAny *aBitmapAddress, TSize aSize, TInt aHalMode) ;
    virtual void SetDisplayMode(CFbsDrawDevice*);
   	virtual TInt HorzTwipsPerThousandPixels() const;
	virtual TInt VertTwipsPerThousandPixels() const;

	virtual void Update();
	virtual void Update(const TRegion& aRegion);
	virtual void UpdateRegion(const TRect& aRect);

	virtual TInt GetInterface(TInt aInterfaceId, TAny*& aInterface);

	// From MSurfaceId
	virtual void GetSurface(TSurfaceId& aSid) const ;
	virtual TUint DeviceOrientationsAvailable() const ;
	virtual TDeviceOrientation DeviceOrientation() const ;
protected:
	//derived classes must implement virtual void SetSize(const TSize& aSize);
	//derived classes must implement virtual TBool SetDeviceOrientation(TDeviceOrientation aOrientation)=0;
	CScreenDeviceHelper iHelper;

	};

/**
This template class is a helper to genericise the common implementation of the screen device.
This class implements the "bits per pixel" specific implementation for modes that do not have GUIDs,
but probably have palettes instead.
params:	
	-	CDrawXxxBppBitmap	The base pixel class that provides most of the drawing functionality.
	-	displayMode			The symbian enumeraion for the mode - used when no GUID is defined.
	-	pixelsPerWord		Number of pixels packed in a 32-bit word
**/
template <class TCDrawXxxBppBitmap,TDisplayMode displayMode,TInt pixelsPerWord>
class CPalettizedScreenDevice : public CGenericScreenDevice<TCDrawXxxBppBitmap>
	{
public:
	typedef  CGenericScreenDevice<TCDrawXxxBppBitmap>	CGenericScreenDevice;
	virtual TInt  ConstructScreen(TInt aScreenNo, TAny *aBitmapAddress, TSize aSize, TInt aHalMode) ;
	virtual TBool SetDeviceOrientation(TDeviceOrientation aOrientation);
protected:
	virtual void SetSize(const TSize& aSize);
	};

/**
This template class is a helper to genericise the common implementation of the screen device.
This class implements the "bits per pixel" specific implementation for modes that do have GUIDs.
params:	
	-	CDrawXxxBppBitmap	The base pixel class that provides most of the drawing functionality.
	-	guidMode			The GUID for the mode - used by GCE
	-	pixelsPerWord		Number of pixels packed in a 32-bit word
**/
template <class TCDrawXxxBppBitmap,TUidPixelFormat guidMode,TInt pixelsPerWord> 
class CGuidScreenDevice : public CGenericScreenDevice<TCDrawXxxBppBitmap>
	{
public:
	typedef  ::CGenericScreenDevice<TCDrawXxxBppBitmap>	CGenericScreenDevice;
	virtual TInt  ConstructScreen(TInt aScreenNo, TAny *aBitmapAddress, TSize aSize, TInt aHalMode) ;
	virtual TBool SetDeviceOrientation(TDeviceOrientation aOrientation);
protected:
	virtual void SetSize(const TSize& aSize);
	};

/**
@internalComponent
*/
class CDrawOneBppScreenBitmap : public CPalettizedScreenDevice<CDrawOneBppBitmap,EGray2,32>
	{};

/**
@internalComponent
*/
class CDrawTwoBppScreenBitmap : public CPalettizedScreenDevice<CDrawTwoBppBitmap,EGray4,16>
	{};

/**
@internalComponent
*/
class CDrawFourBppScreenBitmapGray : public CPalettizedScreenDevice<CDrawFourBppBitmapGray,EGray16,8>
	{};

/**
@internalComponent
*/
class CDrawFourBppScreenBitmapColor : public CPalettizedScreenDevice<CDrawFourBppBitmapColor,EColor16,8>
	{};

/**
@internalComponent
*/
class CDrawEightBppScreenBitmapGray : public CPalettizedScreenDevice<CDrawEightBppBitmapGray,EGray256,4>
	{};

/**
@internalComponent
*/
class CDrawEightBppScreenBitmapColor : public CPalettizedScreenDevice<CDrawEightBppBitmapColor,EColor256,4>
	{};

/**
@internalComponent
*/
class CDrawTwelveBppScreenBitmapColor : public CGuidScreenDevice<CDrawTwelveBppBitmap,EUidPixelFormatXRGB_4444,2>
	{};

/**
@internalComponent
*/
class CDrawSixteenBppScreenBitmap : public CGuidScreenDevice<CDrawSixteenBppBitmap,EUidPixelFormatRGB_565,2>
	{};

/**
@internalComponent
*/
class CDrawUTwentyFourBppScreenBitmap : public CGuidScreenDevice<CDrawUTwentyFourBppBitmap,EUidPixelFormatXRGB_8888,1>
	{};

/**
@internalComponent
*/
class CDrawThirtyTwoBppScreenBitmapAlpha : public CGuidScreenDevice<CDrawThirtyTwoBppBitmapAlpha,EUidPixelFormatARGB_8888,1>
	{};


/**
@internalComponent
*/
class CDrawThirtyTwoBppScreenBitmapAlphaPM : public CGuidScreenDevice<CDrawThirtyTwoBppBitmapAlphaPM,EUidPixelFormatARGB_8888_PRE,1>
	{};


#endif /* __SCDRAW_H__ */

