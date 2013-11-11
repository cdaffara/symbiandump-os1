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

#ifndef __BITDEV_H__
#define __BITDEV_H__

#include <fbs.h>
#include <bitbase.h>
#include <bitstd.h>
#include <graphicsaccelerator.h>

class CFbsDrawDevice;
class TSurfaceId;

/** Screen device orientations.

WARNING: Enumeration for internal use ONLY.  Compatibility is not guaranteed in future releases.

@publishedAll
@released
 */
enum TDeviceOrientation
	{
	EDeviceOrientationNormal = 1,
	EDeviceOrientation90CW = 2,
	EDeviceOrientation180 = 4,
	EDeviceOrientation270CW = 8
	};

/** Abstract base class for graphics devices to which bitmaps and fonts can be 
drawn.

The class implements the pure virtual CBitmapDevice functions and additionally 
provides:

support for creating and enquiring about fonts

comparing two device rectangles

creating a graphics context for the device.

CFbsBitmapDevice and CFbsScreenDevice are derived fom this class.

@see CFbsBitmapDevice
@see CFbsScreenDevice 
@publishedAll
@released
*/
class CFbsDevice : public CBitmapDevice
	{
	friend class CFbsBitGc;
public:
	IMPORT_C ~CFbsDevice();
	IMPORT_C TInt CreateContext(CFbsBitGc*& aGc);
	inline TInt CreateContext(CGraphicsContext*& aGc);
	IMPORT_C TDisplayMode DisplayMode() const;
	IMPORT_C TSize SizeInPixels() const;
	IMPORT_C TBool RectCompare(const TRect& aSourceRect,const CFbsDevice& aDevice,const TRect& aDeviceRect) const;
	IMPORT_C TInt AddFile(const TDesC& aName,TInt& aId);
	IMPORT_C void RemoveFile(TInt aId=0);
	IMPORT_C virtual TInt GetNearestFontInTwips(CFont*& aFont, const TFontSpec& aFontSpec);
	IMPORT_C virtual TInt GetNearestFontInPixels(CFont*& aFont, const TFontSpec& aFontSpec);
	IMPORT_C virtual TInt GetNearestFontToDesignHeightInTwips(CFont*& aFont, const TFontSpec& aFontSpec);
	IMPORT_C virtual TInt GetNearestFontToDesignHeightInPixels(CFont*& aFont, const TFontSpec& aFontSpec);
	IMPORT_C virtual TInt GetNearestFontToMaxHeightInTwips(CFont*& aFont, const TFontSpec& aFontSpec, TInt aMaxHeight); 
	IMPORT_C virtual TInt GetNearestFontToMaxHeightInPixels(CFont*& aFont, const TFontSpec& aFontSpec, TInt aMaxHeight);
	IMPORT_C TInt GetFontById(CFont*& aFont,TUid aFileId,const TAlgStyle& aStyle);
	inline TInt GetNearestFontInTwips(CFbsFont*& aFont, const TFontSpec& aFontSpec);
	inline TInt GetNearestFontInPixels(CFbsFont*& aFont, const TFontSpec& aFontSpec);
	inline TInt GetNearestFontToDesignHeightInTwips(CFbsFont*& aFont, const TFontSpec& aFontSpec);
	inline TInt GetNearestFontToDesignHeightInPixels(CFbsFont*& aFont, const TFontSpec& aFontSpec);
	inline TInt GetNearestFontToMaxHeightInTwips(CFbsFont*& aFont, const TFontSpec& aFontSpec, TInt aMaxHeight);
	inline TInt GetNearestFontToMaxHeightInPixels(CFbsFont*& aFont, const TFontSpec& aFontSpec, TInt aMaxHeight);
	inline TInt GetFontById(CFbsFont*& aFont,TUid aFileId,const TAlgStyle& aStyle);
	IMPORT_C void ReleaseFont(CFont* aFont);
	IMPORT_C TInt FontHeightInTwips(TInt aTypefaceIndex,TInt aHeightIndex) const;
	IMPORT_C TInt FontHeightInPixels(TInt aTypefaceIndex,TInt aHeightIndex) const;
	IMPORT_C TInt NumTypefaces() const;
	IMPORT_C void TypefaceSupport(TTypefaceSupport& aTypefaceSupport,TInt aTypefaceIndex) const;
	IMPORT_C TInt SetCustomPalette(const CPalette* aPalette);
	inline CGraphicsAccelerator* GraphicsAccelerator() const;
	inline CFbsBitGc::TGraphicsOrientation Orientation() const;
	IMPORT_C static TDisplayMode DisplayMode16M();
	IMPORT_C TInt SetScalingFactor(const TPoint& aOrigin,
								   TInt aFactorX, TInt aFactorY,
								   TInt aDivisorX, TInt aDivisorY);
	IMPORT_C void GetDrawRect(TRect& aRect) const;
	IMPORT_C TInt RegisterLinkedTypeface(const CLinkedTypefaceSpecification& aLinkedTypefaceSpec, TInt& aId);
	IMPORT_C TInt SetDrawDeviceOffset(const TPoint& aOrigin);

protected:
	CFbsDevice();
	virtual void DrawingBegin(TBool /*aAlways*/ = EFalse) {}
	virtual void DrawingEnd(TBool /*aAlways*/ = EFalse) {}
	
	/**
	Sets the bit level.
	*/
	virtual void SetBits() {}
	
	/**
	This method has been deprecated.  Sprites are no longer supported in BitGDI.
	Calling this method has no effect.
	@deprecated
	*/
	virtual void CancelSprite() const {}
	
	/**
	This method has been deprecated.  Sprites are no longer supported in BitGDI.
	Calling this method has no effect.
	@return TSpriteBase* NULL.
	@deprecated
	*/
	virtual TSpriteBase* HideSprite() const {return(NULL);}
	
	/**
	This method has been deprecated.  Sprites are no longer supported in BitGDI.
	Calling this method has no effect.
	@param aRect Ignored.
	@param aClippingRegion Ignored. 
  	@return TSpriteBase* NULL.
  	@deprecated
	*/
	virtual TSpriteBase* HideSprite(const TRect&,const TRegion*) const {return(NULL);}
	
	/**
	This method has been deprecated.  Sprites are no longer supported in BitGDI.
	Calling this method has no effect.
	@param aSprite Ignored.
	@deprecated
	*/
	virtual void ShowSprite(TSpriteBase*) const {}
	
	/**
	This method has been deprecated.  Sprites are no longer supported in BitGDI.
	Calling this method has no effect.
	@param aSprite Ignored. 
 	@param aRect Ignored. 
 	@param aClippingRegion Ignored.
 	@deprecated
 	*/
	virtual void ShowSprite(TSpriteBase*,const TRect&,const TRegion*) const {}
	TInt GetNearestFbsFont(CFbsFont*& aFont,const TFontSpec& aFontSpec);
	void DoGetScanLine(TDes8& aBuf,const TPoint& aPixel,TInt aLength,TDisplayMode aDispMode);
	void ReadLineDiffMode(TInt,TInt,TInt,TAny*,TDisplayMode) const;
	void TruncateRect(TRect& aRect);
	TBool SetOrientation(CFbsBitGc::TGraphicsOrientation aOrientation);
	void DrawingBegin(const CFbsBitmap* aBitmap, TBool aAlways = EFalse);
	void DrawingEnd(const CFbsBitmap* aBitmap, TBool aAlways = EFalse);
protected:
	CFbsDrawDevice* iDrawDevice;
	RFbsSession* iFbs;
	CFbsTypefaceStore* iTypefaceStore;
	TInt iSpare;
	TBool iScreenDevice;
	TUint8* iBitBltMaskedBuffer;
	CGraphicsAccelerator* iGraphicsAccelerator;
	CFbsBitGc::TGraphicsOrientation iOrientation;
	};

/** A graphics device interface that provides direct access to the screen, without 
the mediation of the window server.

@publishedAll
@released
*/
class CFbsScreenDevice : public CFbsDevice
	{
public:
	IMPORT_C static CFbsScreenDevice* NewL(const TDesC& aLibname,TDisplayMode aDispMode);
	IMPORT_C static CFbsScreenDevice* NewL(const TDesC& aLibname,TDisplayMode aDispMode, TRgb aWhite);
	IMPORT_C static CFbsScreenDevice* NewL(TInt aScreenNo, TDisplayMode aDispMode);
	IMPORT_C virtual ~CFbsScreenDevice();
	IMPORT_C void GetScanLine(TDes8& aBuf,const TPoint& aStartPixel,TInt aLength,TDisplayMode aDispMode) const;
	IMPORT_C void GetPixel(TRgb& aColor,const TPoint& aPixel) const;
	IMPORT_C TInt HorizontalPixelsToTwips(TInt aPixels) const;
	IMPORT_C TInt VerticalPixelsToTwips(TInt aPixels) const;
	IMPORT_C TInt HorizontalTwipsToPixels(TInt aTwips) const;
	IMPORT_C TInt VerticalTwipsToPixels(TInt aTwips) const;
	IMPORT_C void SetAutoUpdate(TBool aValue);
	IMPORT_C void DrawSpriteBegin();		//< @deprecated
	IMPORT_C void DrawSpriteEnd();		//< @deprecated
	IMPORT_C void CancelSprite() const;		//< @deprecated
	IMPORT_C TSpriteBase* HideSprite() const;		//< @deprecated
	IMPORT_C TSpriteBase* HideSprite(const TRect& aRect,const TRegion* aClippingRegion) const;		//< @deprecated
	IMPORT_C void ShowSprite(TSpriteBase* aSprite) const;		//< @deprecated
	IMPORT_C void ShowSprite(TSpriteBase* aSprite,const TRect& aRect,const TRegion* aClippingRegion) const;		//< @deprecated
	IMPORT_C void ChangeScreenDevice(CFbsScreenDevice* aOldDevice);
	IMPORT_C void PaletteAttributes(TBool& aModifiable,TInt& aNumEntries) const;
	IMPORT_C void SetPalette(CPalette* aPalette);
	IMPORT_C TInt GetPalette(CPalette*& aPalette) const;
	IMPORT_C void Update();
	IMPORT_C void Update(const TRegion& aRegion);
	IMPORT_C TSize SizeInTwips() const;
	IMPORT_C RHardwareBitmap HardwareBitmap();
	IMPORT_C const TUint32* Bits() const;
	IMPORT_C TInt Stride() const;
	IMPORT_C TInt ScreenNo() const;
	IMPORT_C TUint DeviceOrientationsAvailable() const;	/**< WARNING: Function for internal use ONLY. Compatibility is not guaranteed in future releases. */
	IMPORT_C TBool SetDeviceOrientation(TDeviceOrientation aOrientation) const;	/**< WARNING: Function for internal use ONLY. Compatibility is not guaranteed in future releases. */
	IMPORT_C void GetSurface(TSurfaceId& aSurface) const; /**< WARNING: Function for internal use ONLY. Compatibility is not guaranteed in future releases. */
	IMPORT_C TDeviceOrientation DeviceOrientation() const; /**< WARNING: Function for internal use ONLY. Compatibility is not guaranteed in future releases. */
private:
	CFbsScreenDevice(TInt aScreenNo);
	void ConstructL(TInt aScreenNo, TDisplayMode aDispMode);
private:
	TInt iSpare1_CFbsScreenDevice;
	TInt iSpare2_CFbsScreenDevice;
    TInt iScreenNo;
	};

/** A graphics device to which a bitmap managed by the font and bitmap server can 
be drawn.

The class specialises the bitmap graphics device interface CBitmapDevice for 
drawing to in-memory bitmaps. 
@publishedAll
@released
*/
class CFbsBitmapDevice : public CFbsDevice
	{
public:
	IMPORT_C static CFbsBitmapDevice* NewL(CFbsBitmap* aFbsBitmap);
	IMPORT_C static CFbsBitmapDevice* NewL(CFbsBitmap* aFbsBitmap,const TDesC& aLibname);
	IMPORT_C virtual ~CFbsBitmapDevice();
	IMPORT_C TInt Resize(const TSize& aSize);
	IMPORT_C virtual void GetScanLine(TDes8& aBuf,const TPoint& aStartPixel,TInt aLength,TDisplayMode iDispMode) const;
	IMPORT_C virtual void GetPixel(TRgb& aColor,const TPoint& aPixel) const;
	IMPORT_C virtual TInt HorizontalPixelsToTwips(TInt aPixels) const;
	IMPORT_C virtual TInt VerticalPixelsToTwips(TInt aPixels) const;
	IMPORT_C virtual TInt HorizontalTwipsToPixels(TInt aTwips) const;
	IMPORT_C virtual TInt VerticalTwipsToPixels(TInt aTwips) const;
	IMPORT_C virtual TSize SizeInTwips() const;
	IMPORT_C virtual void PaletteAttributes(TBool& aModifiable,TInt& aNumEntries) const;
	IMPORT_C virtual void SetPalette(CPalette* aPalette);
	IMPORT_C virtual TInt GetPalette(CPalette*& aPalette) const;
	IMPORT_C virtual void DrawingBegin(TBool aAlways = EFalse);
	IMPORT_C virtual void DrawingEnd(TBool aAlways = EFalse);
	IMPORT_C TInt SwapWidthAndHeight();

private:
	CFbsBitmapDevice();
	virtual void SetBits();
	void ConstructL(CFbsBitmap* aFbsBitmap);
	void ConstructL(CFbsBitmap* aFbsBitmap,const TDesC& aLibname);
private:
	CFbsBitGcBitmap* iFbsBmp;
	};

#include <bitdev.inl>
#endif
