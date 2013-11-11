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

#ifndef __BMDRAW_H__
#define __BMDRAW_H__

#include <gdi.h>
#include <bitdraw.h>
#include "BitDrawScaling.h"
#include "BitDrawOrigin.h"
#include "BmAlphaBlend.h"
#include "BitDrawOrientation.h"

#ifdef __ARMCC__
	#define FORCEINLINE __forceinline
#else
	#define FORCEINLINE inline
#endif

/**
Outline colour index used to get percentage of outline colour to be used to get 
the final colour from lookup table
@internalComponent
*/
const TInt KOutlineColorIndex = 0;

/**
Shadow colour index used to get percentage of shadow colour to be used to get 
the final colour from lookup table
@internalComponent
*/
const TInt KShadowColorIndex = 1;

/**
Fill colour index used to get percentage of fill colour to be used to get 
the final colour from lookup table
@internalComponent
*/
const TInt KFillColorIndex = 2;

/**
Background colour index used to get percentage of background colour to be used to get 
the final colour from lookup table
@internalComponent
*/
const TInt KBackgroundColorIndex = 3;

/**
Lookup table used for outline and shadow fonts.
@internalComponent
*/
GLREF_D const TInt FourColorBlendLookup[256][4];

TRgb AlphaBlend(TRgb aPrimary, TRgb aSecondary, TInt aAlphaValue);

 //Scaling settings. MScalingSettings interface operates with them.
struct TScalingSettings
	{
	inline TScalingSettings() :
		iFactorX(1),
		iFactorY(1),
		iDivisorX(1),
		iDivisorY(1)
		{
		}
	TInt iFactorX;
	TInt iFactorY;
	TInt iDivisorX;
	TInt iDivisorY;
	};

 // Drawing device origin. MDrawDeviceOrigin interface operates with it.
typedef TPoint TOrigin;

//This class is used as a template argument by CDrawEightBppBitmapCommon::SetPixels()
//and CDrawSixteenBppBitmapCommon::SetPixels()
//Each of its methods (EvalInc() and EvalDec()) has two arguments -
//aAddr and aValue, and it is used to set *aAddr with aValue and to increment/decrement
//respectively aAddr.
template <class TPixelType> class TPixelSet
	{
public:
	inline void EvalInc(TPixelType*& aAddr, TPixelType aValue) const
		{
		*aAddr++ = aValue;
		}
	inline void EvalDec(TPixelType*& aAddr, TPixelType aValue) const
		{
		*aAddr-- = aValue;
		}
	};

//This class is used as a template argument by CDrawEightBppBitmapCommon::XORPixels().
//and CDrawSixteenBppBitmapCommon::XORPixels()
//Each of its methods (EvalInc() and EvalDec()) has two arguments -
//aAddr and aValue, and it is used to XOR *aAddr with aValue and to increment/decrement
//respectively aAddr.
template <class TPixelType> class TPixelXor
	{
public:
	inline void EvalInc(TPixelType*& aAddr, TPixelType aValue) const
		{
		*aAddr++ ^= aValue;
		}
	inline void EvalDec(TPixelType*& aAddr, TPixelType aValue) const
		{
		*aAddr-- ^= aValue;
		}
	};

//This class is used as a template argument by CDrawEightBppBitmapCommon::ANDPixels().
//and CDrawSixteenBppBitmapCommon::ANDPixels()
//Each of its methods (EvalInc() and EvalDec()) has two arguments -
//aAddr and aValue, and it is used to AND *aAddr with aValue and to increment/decrement
//respectively aAddr.
template <class TPixelType> class TPixelAnd
	{
public:
	inline void EvalInc(TPixelType*& aAddr, TPixelType aValue) const
		{
		*aAddr++ &= aValue;
		}
	inline void EvalDec(TPixelType*& aAddr, TPixelType aValue) const
		{
		*aAddr-- &= aValue;
		}
	};

//This class is used as a template argument by CDrawEightBppBitmapCommon::ORPixels().
//and CDrawSixteenBppBitmapCommon::ORPixels()
//Each of its methods (EvalInc() and EvalDec()) has two arguments -
//aAddr and aValue, and it is used to OR *aAddr with aValue and to increment/decrement
//respectively aAddr.
template <class TPixelType> class TPixelOr
	{
public:
	inline void EvalInc(TPixelType*& aAddr, TPixelType aValue) const
		{
		*aAddr++ |= aValue;
		}
	inline void EvalDec(TPixelType*& aAddr, TPixelType aValue) const
		{
		*aAddr-- |= aValue;
		}
	};

//This template function should be used every time when a rectangle has to be drawn instead of
//a single pixel in EColor256/EColor64K scaled device.
//aPixelPtr - points to the pixel memory address, which contains top-left corner of the rectangle.
//aValue - the pixel value - EColor256.
//aPixelPtrRowLimit - the right end address of the scalline.
//aBitsStart - beginning address of the pixel memory.
//aBitsEnd - end address of the pixel memory.
//aLongWidth - aligned scanline width in pixels.
//aFx - X-axis scaling factor.
//aFy - Y-axis scaling factor.
//aOrientation - device's orientation.
//aOp - the operation, which must be applied to every pixel from the rectangle - template parameter.
template <class TPixelOp, class TPixelType>
inline void FillScaledRect(TPixelType* aPixelPtr,
						   const TPixelType aValue,
						   const TPixelType* aPixelPtrRowLimit,
						   const TPixelType* aBitsStart,
						   const TPixelType* aBitsEnd,
						   const TInt aLongWidth,
						   TInt aFx,
						   TInt aFy,
						   const CFbsDrawDevice::TOrientation aOrientation,
						   const TPixelOp& aOp)
	{
	if(aOrientation == CFbsDrawDevice::EOrientationNormal)
		{
		//for example: if aFx = 3 and aFy = 2 then
		//the following pixel values have to be evaluated:
		// (aPixelPtr), (aPixelPtr + 1),  (aPixelPtr + 2)
		// (aPixelPtr + aLongWidth), (aPixelPtr + 1 + aLongWidth), (aPixelPtr + 2 + aLongWidth)
		const TPixelType* nextStop = aPixelPtr + aFx;//the address of next scaled pixel value
		if(nextStop > aPixelPtrRowLimit)
			{//If the address is after the end of the current row, set it to the end of row.
			nextStop = aPixelPtrRowLimit;
			}
		do  //Fill pixel rectangle [aFx,aFy]
			{
			while(aPixelPtr < nextStop)
				{//Fill a row of up to aFx pixels.
				aOp.EvalInc(aPixelPtr, aValue);
				}
			aPixelPtr += (aLongWidth - aFx);//Move aPixelPtr to the next row
			if(aPixelPtr >= aBitsEnd)
				{//If aPixelPtr points after the end of video memory - stop the operation!
				break;
				}
			nextStop += aLongWidth;//Move nextPixelAddr to the next row
			}
		while(--aFy);
		return;
		}
	if(aOrientation == CFbsDrawDevice::EOrientationRotated90)
		{
		//for example: if aFy = 3 and aFx = 2 then
		//the following pixel values have to be evaluated:
		// (aPixelPtr), (aPixelPtr - 1),
		// (aPixelPtr + aLongWidth), (aPixelPtr - 1 + aLongWidth),
		// (aPixelPtr + 2 * aLongWidth), (aPixelPtr - 1 + 2 * aLongWidth),
		const TPixelType* pixelPtrPrevRowLimit = aPixelPtrRowLimit - aLongWidth;
		const TPixelType* nextStop = aPixelPtr - aFx;//the address of next scaled pixel value
		if(nextStop < pixelPtrPrevRowLimit)
			{//If the address is before the beginning of the current row, set it to the beginning of row.
			nextStop = pixelPtrPrevRowLimit;
			}
		do  //Fill pixel rectangle [aFx,aFy]
			{
			while(aPixelPtr > nextStop)
				{//Fill a row of up to aFy pixels.
				aOp.EvalDec(aPixelPtr, aValue);
				}
			aPixelPtr += (aLongWidth + aFx);//Move aPixelPtr to the next row
			if(aPixelPtr >= aBitsEnd)
				{//If aPixelPtr points after the end of video memory - stop the operation!
				break;
				}
			nextStop += aLongWidth;//Move nextPixelAddr to the next row
			}
		while(--aFy);
		return;
		}
	if(aOrientation == CFbsDrawDevice::EOrientationRotated180)
		{
		//for example: if aFx = 3 and aFy = 2 then
		//the following pixel values have to be evaluated:
		// (aPixelPtr), (aPixelPtr - 1),  (aPixelPtr - 2)
		// (aPixelPtr - aLongWidth), (aPixelPtr + 1 - aLongWidth), (aPixelPtr + 2 - aLongWidth)
		const TPixelType* pixelPtrPrevRowLimit = aPixelPtrRowLimit - aLongWidth;
		const TPixelType* nextStop = aPixelPtr - aFx;//the address of next scaled pixel value
		if(nextStop < pixelPtrPrevRowLimit)
			{//If the address is before the beginning of the current row, set it to the beginning of row.
			nextStop = pixelPtrPrevRowLimit;
			}
		do  //Fill pixel rectangle [aFx,aFy]
			{
			while(aPixelPtr > nextStop)
				{//Fill a row of up to aFx pixels.
				aOp.EvalDec(aPixelPtr, aValue);
				}
			aPixelPtr -= (aLongWidth - aFx);//Move aPixelPtr to the prev row
			if(aPixelPtr < aBitsStart)
				{//If aPixelPtr points before the beginning of video memory - stop the operation!
				break;
				}
			nextStop -= aLongWidth;//Move nextPixelAddr to the prev row
			}
		while(--aFy);
		return;
		}
	else //if(aOrientation == CFbsDrawDevice::EOrientationRotated270)
		{
		//for example: if aFy = 3 and aFx = 2 then
		//the following pixel values have to be evaluated:
		// (aPixelPtr), (aPixelPtr + 1)
		// (aPixelPtr - aLongWidth), (aPixelPtr + 1 - aLongWidth)
		// (aPixelPtr - 2 * aLongWidth), (aPixelPtr + 1 - 2 * aLongWidth)
		const TPixelType* nextStop = aPixelPtr + aFx;//the address of next scaled pixel value
		if(nextStop > aPixelPtrRowLimit)
			{//If the address is after the end of the current row, set it to the end of row.
			nextStop = aPixelPtrRowLimit;
			}
		do  //Fill pixel rectangle [aFx,aFy]
			{
			while(aPixelPtr < nextStop)
				{//Fill a row of up to aFy pixels.
				aOp.EvalInc(aPixelPtr, aValue);
				}
			aPixelPtr += (-aLongWidth - aFx);//Move aPixelPtr to the prev row
			if(aPixelPtr < aBitsStart)
				{//If aPixelPtr points befor the beginning of video memory - stop the operation!
				break;
				}
			nextStop -= aLongWidth;//Move nextPixelAddr to the prev row
			}
		while(--aFy);
		return;
		}
	}

/**
@publishedPartner
*/
NONSHARABLE_CLASS(CDrawBitmap) : public CFbsDrawDevice,
                                 public MScalingSettings,
                                 public MDrawDeviceOrigin,
                                 public MAlphaBlend,
                                 public MOutlineAndShadowBlend,
                                 public MDrawDeviceOrientation,
								 public MFastBlend
	{
public:
	virtual ~CDrawBitmap();
	// From CFbsDrawDevice
	virtual TDisplayMode DisplayMode() const { return iDispMode; }
	virtual TInt LongWidth() const;
	virtual void MapColors(const TRect& aRect,const TRgb* aColors,TInt aNumPairs,TBool aMapForwards);
	virtual TRgb ReadPixel(TInt aX,TInt aY) const;
	virtual void ReadLine(TInt aX,TInt aY,TInt aLength,TAny* aBuffer,TDisplayMode aDispMode) const;
	virtual TUint32* ScanLineBuffer() const;
	virtual TInt ScanLineBytes() const;
	virtual TDisplayMode ScanLineDisplayMode() const { return iDispMode; }
	virtual TSize SizeInPixels() const;
	virtual TInt HorzTwipsPerThousandPixels() const { return 0; }
	virtual TInt VertTwipsPerThousandPixels() const { return 0; }
	virtual void OrientationsAvailable(TBool aOrientation[4]);
	virtual void WriteRgb(TInt aX,TInt aY,TRgb aColor,CGraphicsContext::TDrawMode aDrawMode);
	virtual void WriteBinary(TInt aX,TInt aY,TUint32* aBuffer,TInt aLength,TInt aHeight,TRgb aColor,CGraphicsContext::TDrawMode aDrawMode);
	virtual void WriteBinaryLine(TInt aX,TInt aY,TUint32* aBuffer,TInt aLength,TRgb aColor,CGraphicsContext::TDrawMode aDrawMode);
	virtual void WriteBinaryLineVertical(TInt aX,TInt aY,TUint32* aBuffer,TInt aLength,TRgb aColor,CGraphicsContext::TDrawMode aDrawMode,TBool aUp);
	virtual void WriteLine(TInt aX,TInt aY,TInt aLength,TUint32* aBuffer ,CGraphicsContext::TDrawMode);
	virtual void WriteRgbMulti(TInt aX,TInt aY,TInt aWidth,TInt aLength,TRgb aColor,CGraphicsContext::TDrawMode);
	virtual void SetBits(TAny* aBits);
	virtual void SetDitherOrigin(const TPoint& aPoint);
	virtual void SetShadowMode(TShadowMode aShadowMode) { iShadowMode = aShadowMode; }
	virtual void SetFadingParameters(TUint8 aBlackMap,TUint8 aWhiteMap);
	virtual void SetUserDisplayMode(TDisplayMode aDisplayMode) { iUserDispMode = aDisplayMode; }
	virtual TBool SetOrientation(TOrientation aOrientation);
	virtual void WriteRgbAlphaLine(TInt aX,TInt aY,TInt aLength,
                                   TUint8* aRgbBuffer, TUint8* aMaskBuffer,
                                   CGraphicsContext::TDrawMode aDrawMode);
	virtual void WriteRgbAlphaLine(TInt aX,TInt aY,TInt aLength,
								const TUint8* aRgbBuffer1,
								const TUint8* aBuffer2,
								const TUint8* aMaskBuffer,
								CGraphicsContext::TDrawMode aDrawMode);
	virtual TInt GetInterface(TInt aInterfaceId, TAny*& aInterface);
	virtual void GetDrawRect(TRect& aDrawRect) const;
	virtual void SwapWidthAndHeight();
	// From MScalingSettings
	virtual TInt Set(TInt aFactorX, TInt aFactorY, TInt aDivisorX, TInt aDivisorY);
	virtual void Get(TInt& aFactorX, TInt& aFactorY, TInt& aDivisorX, TInt& aDivisorY);
	virtual TBool IsScalingOff();
	// From MDrawDeviceOrigin
	virtual TInt Set(const TPoint& aOrigin);
	virtual void Get(TPoint& aOrigin);
	// From MDrawDeviceOrientation
	virtual CFbsDrawDevice::TOrientation Orientation();	
	// From MFastBlend
	virtual TInt FastBlendBitmap(const TPoint& aDest,CFbsDrawDevice* aSrcDrawDevice, const TRect& aSrcRect, CGraphicsContext::TDrawMode aDrawMode, TInt aShadowMode);
	virtual TInt FastBlendBitmap(const TPoint& aDest,const TUint32* aSrcBase, TInt aSrcStride, const TSize& aSrcSize, const TRect& aSrcRect, TDisplayMode aDisplayMode, CGraphicsContext::TDrawMode aDrawMode, TInt aShadowMode);
	virtual TInt FastBlendBitmapMasked(const TPoint& aDest, const TUint32* aSrcBase, TInt aSrcStride,
							const TSize& aSrcSize, const TRect& aSrcRect, TDisplayMode aSrcDisplayMode,
							const TUint32* aMaskBase, TInt aMaskStride, TDisplayMode aMaskDisplayMode, const TSize &aMaskSize,const TPoint &aMaskSrcPos,TBool aInvertMask,
							CGraphicsContext::TDrawMode aDrawMode, TInt aShadowMode);
	virtual TInt FastBlendBitmapScaled(const TRect &aClipRect, const TRect& aDest, const TRect& aSrcRect, const TUint32 *aSrcBase, TInt aSrcLinePitch, TDisplayMode aSrcDisplayMode, const TSize &aSrcSize, CGraphicsContext::TDrawMode aDrawMode, TInt aShadowMode);
	virtual TInt FastBlendBitmapMaskedScaled(const TRect &aClipRect, const TRect& aDest,
							const TRect& aSrcRect, const TUint32 *aSrcBase, TInt aSrcLinePitch,
							TDisplayMode aSrcDisplayMode, const TSize &aSrcSize,
							const TUint32* aMaskBase, TInt aMaskStride, TDisplayMode aMaskDisplayMode, const TSize &aMaskSize,TBool aInvertMask,
							CGraphicsContext::TDrawMode aDrawMode, TInt aShadowMode);
protected:
	CDrawBitmap();
	static TInt BitsPerPixel(TDisplayMode aDispMode);
	void Clear();
	void CopyOldSettings(CFbsDrawDevice* aDrawDevice);
	void DeOrientate(TInt& aX,TInt& aY) const;
	TPoint DeOrientate(const TPoint& aPoint) const;
	TRect DeOrientate(const TRect& aRect) const;
	virtual void Shadow(TRgb& aColor) = 0;
	TRgb FadeRgb(TRgb aColor);
	TUint32 FadeRgb(TUint32 aColor);
	void FadeRgb(TInt& aRed, TInt& aGreen, TInt& aBlue);
	TUint8 FadeGray(TInt aGray256);
	TUint32 Hash(TUint32 aValue,TInt aX,TInt aY) const;
	TUint32 PasteInt(TUint32 aFirst,TUint32 aSecond,TInt aOffset) const;
	static TAny* CopyOffset(TAny* aDestination,const TAny* aSource,TInt aWordsToCopy,TInt aSourceBitOffset);
	static void ReadLineCommon(TUint32* aPixelPtr,TUint32* aBufferPtr,TInt aWordsCnt,TInt aRestPixels,TInt aBytesCnt,TInt aStartDiffBits);
	virtual void SetSize(const TSize& aSize);
	// Scaling related
	TBool CanBeScaled() const;
	TInt PixelAddressIncrement() const;
	TInt LogicalPixelAddressIncrement() const;
	void SetPixelInc(TInt& aPixelInc, TInt& aRowInc) const;
	void IncScaledY(TInt& aY) const;
	void IncScaledY(TInt& aY, TInt aYOrg) const;
	//Generic set pixels operations, has to be defined here to make it compiled in VC++6.0
	template <class TPixelType>
	void SetPixels(TPixelType* aPixelPtr, TPixelType aValue, const TPixelType* aPixelPtrRowLimit,
				   const TPixelType* aBitsStart, const TPixelType* aBitsEnd) const
		{
		TPixelSet<TPixelType> op;
		::FillScaledRect(aPixelPtr, aValue, aPixelPtrRowLimit, aBitsStart, aBitsEnd, iLongWidth,
						 iScalingSettings.iFactorX, iScalingSettings.iFactorY, iOrientation, op);
		}
	template <class TPixelType>
	void XORPixels(TPixelType* aPixelPtr, TPixelType aValue, const TPixelType* aPixelPtrRowLimit,
				   const TPixelType* aBitsStart, const TPixelType* aBitsEnd) const
		{
		TPixelXor<TPixelType> op;
		::FillScaledRect(aPixelPtr, aValue, aPixelPtrRowLimit, aBitsStart, aBitsEnd, iLongWidth,
						 iScalingSettings.iFactorX, iScalingSettings.iFactorY, iOrientation, op);
		}
	template <class TPixelType>
	void ANDPixels(TPixelType* aPixelPtr, TPixelType aValue, const TPixelType* aPixelPtrRowLimit,
				   const TPixelType* aBitsStart, const TPixelType* aBitsEnd) const
		{
		TPixelAnd<TPixelType> op;
		::FillScaledRect(aPixelPtr, aValue, aPixelPtrRowLimit, aBitsStart, aBitsEnd, iLongWidth,
						 iScalingSettings.iFactorX, iScalingSettings.iFactorY, iOrientation, op);
		}
	template <class TPixelType>
	void ORPixels(TPixelType* aPixelPtr, TPixelType aValue, const TPixelType* aPixelPtrRowLimit,
				  const TPixelType* aBitsStart, const TPixelType* aBitsEnd) const
		{
		TPixelOr<TPixelType> op;
		::FillScaledRect(aPixelPtr, aValue, aPixelPtrRowLimit, aBitsStart, aBitsEnd, iLongWidth,
						 iScalingSettings.iFactorX, iScalingSettings.iFactorY, iOrientation, op);
		}
	//Origin related
	TBool CanOriginBeMoved() const;
	void MapColorToUserDisplayMode(TInt& red,TInt& green,TInt& blue);
protected:
	virtual void InvertBuffer(TInt aLength,TUint32* aBuffer) = 0;
	virtual TRgb ReadRgbNormal(TInt aX,TInt aY) const = 0;
	virtual void ReadLine(TInt aX,TInt aY,TInt aLength,TAny* aBuffer) const = 0;
	virtual TUint32* ScanLine(TInt aY) const;
	virtual void WriteRgbMultiXOR(TInt aX,TInt aY,TInt aWidth,TInt aLength,TRgb aColor) = 0;
	virtual void WriteRgbMultiAND(TInt aX,TInt aY,TInt aWidth,TInt aLength,TRgb aColor) = 0;
	virtual void WriteRgbMultiOR(TInt aX,TInt aY,TInt aWidth,TInt aLength,TRgb aColor) = 0;
	virtual void WriteLineXOR(TInt aX,TInt aY,TInt aLength,TUint32* aBuffer) = 0;
	virtual void WriteLineAND(TInt aX,TInt aY,TInt aLength,TUint32* aBuffer) = 0;
	virtual void WriteLineOR(TInt aX,TInt aY,TInt aLength,TUint32* aBuffer) = 0;
	virtual void WriteRgb(TInt aX,TInt aY,TRgb aColor) = 0;
	virtual void WriteBinary(TInt aX,TInt aY,TUint32* aBuffer,TInt aLength,TInt aHeight,TRgb aColor) = 0;
	virtual void WriteBinaryOp(TInt aX,TInt aY,TUint32* aBuffer,TInt aLength,TInt aHeight,TRgb aColor,CGraphicsContext::TDrawMode aDrawMode) = 0;
	virtual void WriteBinaryLineVertical(TInt aX,TInt aY,TUint32* aBuffer,TInt aLength,TRgb aColor,TBool aUp) = 0;
	virtual void WriteRgbMulti(TInt aX,TInt aY,TInt aWidth,TInt aLength,TRgb aColor) = 0;
	virtual void BlendRgbMulti(TInt aX,TInt aY,TInt aWidth,TInt aLength,TRgb aColor);
	virtual void WriteLine(TInt aX,TInt aY,TInt aLength,TUint32* aBuffer) = 0;
	virtual void BlendLine(TInt aX,TInt aY,TInt aLength,TUint32* aBuffer);
	virtual void MapColorToUserDisplayMode(TRgb& /*aColor*/) {}
	virtual void MapBufferToUserDisplayMode(TInt /*aLength*/,TUint32* /*aBuffer*/) {}
private:
	void GetBlendPosAndRect(TRect &aSrcRect, const TRect &aSrcRectIn, const TSize &aSrcSize, const TPoint &aDestOffset);
	void GetBlendPosAndRect(TRect &aDstRect, TRect &aSrcRect, const TRect &aDestRectIn, const TRect &aSrcRectIn, const TSize &aSrcSize);
	TBool FastBlendSupported(TDisplayMode aSrcDisplayMode, CGraphicsContext::TDrawMode aDrawMode, TInt aShadowMode, TInt aSrcLinePitch);
	TBool FastBlendMaskSupported(TDisplayMode aMaskDisplayMode, TInt aMaskStride);
	void DoCopyOldSettings(CFbsDrawDevice* aDrawDevice);
	TInt DoFastBlendBitmap(const TPoint &aDest, const TRect& aSrcRect, const TUint8 *aSrcBase, TInt aSrcLinePitch, TDisplayMode aSrcDisplayMode, const TSize &aSrcSize);
	// Scaling related
	void InitLogicalCoordinates();
	void PreWriteRgb(TInt& aWidth, TInt& aHeight, TInt& aX, TInt& aY, CGraphicsContext::TDrawMode aDrawMode);
	void WriteRgb(TInt& aWidth, TInt& aHeight, TInt& aX, TInt& aY, TRgb aColor, CGraphicsContext::TDrawMode aDrawMode);
	void SetDefaults();
	inline TInt Origin(TInt aPhysOrg, TInt aScale) const;
	inline TInt OtherSide(TInt aPhysOrg, TInt aPhysSize, TInt aScale) const;
protected:
	TDisplayMode iDispMode;
	TInt iLongWidth; // aligned scanline width in pixels
	TPoint iDitherOrigin;
	TUint32* iScanLineBuffer;
	TInt iScanLineWords;
	TShadowMode iShadowMode;
	TSize iSize; // (0, 0, iSize.iWidth, iSize.iHeight) - drawing rectangle - physical coordinates
	TUint32* iBits;
	TDisplayMode iUserDispMode;
	TOrientation iOrientation; // 0, 90, 180, 270 degrees
	TInt iFadeMapFactor;
	TInt iFadeMapOffset;
	TScalingSettings iScalingSettings; // X-axis and Y-axis scaling factors
	TOrigin iOrigin; // drawing device origin
	TBool iScalingOff; // ETrue, if the scaling is off.
	TBool iOriginIsZero; // ETrue, if the origin is (0, 0).
	TRect iDrawRect; // drawing rectangle - logical coordinates
    MAlphaBlend* iAlphaBlend; // Alphablending interface
	};

/**
@publishedPartner
*/
NONSHARABLE_CLASS(CDrawOneBppBitmap) : public CDrawBitmap
	{
public:
	TInt Construct(TSize aSize);
	TInt Construct(TSize aSize, TInt aStride);
	virtual void InvertBuffer(TInt aLength,TUint32* aBuffer);
	virtual TRgb ReadRgbNormal(TInt aX,TInt aY) const;
	virtual void ShadowArea(const TRect& aRect);
	virtual void WriteRgbMultiXOR(TInt aX,TInt aY,TInt aWidth,TInt aLength,TRgb aColor);
	virtual void WriteRgbMultiAND(TInt aX,TInt aY,TInt aWidth,TInt aLength,TRgb aColor);
	virtual void WriteRgbMultiOR(TInt aX,TInt aY,TInt aWidth,TInt aLength,TRgb aColor);
	virtual void WriteLineXOR(TInt aX,TInt aY,TInt aLength,TUint32* aBuffer);
	virtual void WriteLineAND(TInt aX,TInt aY,TInt aLength,TUint32* aBuffer);
	virtual void WriteLineOR(TInt aX,TInt aY,TInt aLength,TUint32* aBuffer);
	virtual void WriteRgb(TInt aX,TInt aY,TRgb aColor);
	virtual void WriteBinary(TInt aX,TInt aY,TUint32* aBuffer,TInt aLength,TInt aHeight,TRgb aColor);
	virtual void WriteBinaryOp(TInt aX,TInt aY,TUint32* aBuffer,TInt aLength,TInt aHeight,TRgb aColor,CGraphicsContext::TDrawMode aDrawMode);
	virtual void WriteBinaryLineVertical(TInt aX,TInt aY,TUint32* aBuffer,TInt aLength,TRgb aColor,TBool aUp);
	virtual void WriteRgbMulti(TInt aX,TInt aY,TInt aWidth,TInt aLength,TRgb aColor);
	virtual void WriteLine(TInt aX,TInt aY,TInt aLength,TUint32* aBuffer);
	virtual void WriteRgbAlphaMulti(TInt aX,TInt aY,TInt aLength,TRgb aColor,const TUint8* aMaskBuffer);
protected:
	virtual void SetSize(const TSize& aSize);
	virtual void ReadLine(TInt aX,TInt aY,TInt aLength,TAny* aBuffer) const;
	virtual void Shadow(TRgb& aColor);
	void ShadowBuffer(TInt aLength,TUint32* aBuffer);
private:
	TUint32 ColorWord(TRgb aColor) const;
	virtual void WriteRgbAlphaLine(TInt aX,
                                   TInt aY,
                                   TInt aLength,
                                   const TUint8* aRgbBuffer,
                                   const TUint8* aMaskBuffer,
                                   MAlphaBlend::TShadowing aShadowing,
                                   CGraphicsContext::TDrawMode aDrawMode);
	// From MOutlineAndShadowBlend
	virtual TInt WriteRgbOutlineAndShadow(TInt aX, TInt aY, const TInt aLength, 
	                                   TUint32 aOutlinePenColor, TUint32 aShadowColor, 
	                                   TUint32 aFillColor, const TUint8* aDataBuffer);
	};

/**
@publishedPartner
*/
NONSHARABLE_CLASS(CDrawTwoBppBitmap) : public CDrawBitmap
	{
public:
	TInt Construct(TSize aSize);
	TInt Construct(TSize aSize, TInt aStride);
	TUint32 ColorInt(TRgb aColor) const;
	void HashInt(TUint32& aInt1,TUint32& aInt2,TRgb aColor,TInt aX,TInt aY) const;
	virtual void InvertBuffer(TInt aLength,TUint32* aBuffer);
	void MapColors(const TRect& aRect,const TRgb* aColors,TInt aNumPairs,TBool aMapForwards);
	virtual TRgb ReadRgbNormal(TInt aX,TInt aY) const;
	virtual void ShadowArea(const TRect& aRect);
	virtual void WriteRgbMultiXOR(TInt aX,TInt aY,TInt aWidth,TInt aLength,TRgb aColor);
	virtual void WriteRgbMultiAND(TInt aX,TInt aY,TInt aWidth,TInt aLength,TRgb aColor);
	virtual void WriteRgbMultiOR(TInt aX,TInt aY,TInt aWidth,TInt aLength,TRgb aColor);
	virtual void WriteLineXOR(TInt aX,TInt aY,TInt aLength,TUint32* aBuffer);
	virtual void WriteLineAND(TInt aX,TInt aY,TInt aLength,TUint32* aBuffer);
	virtual void WriteLineOR(TInt aX,TInt aY,TInt aLength,TUint32* aBuffer);
	virtual void WriteRgb(TInt aX,TInt aY,TRgb aColor);
	virtual void WriteBinary(TInt aX,TInt aY,TUint32* aBuffer,TInt aLength,TInt aHeight,TRgb aColor);
	virtual void WriteBinaryOp(TInt aX,TInt aY,TUint32* aBuffer,TInt aLength,TInt aHeight,TRgb aColor,CGraphicsContext::TDrawMode aDrawMode);
	virtual void WriteBinaryLineVertical(TInt aX,TInt aY,TUint32* aBuffer,TInt aLength,TRgb aColor,TBool aUp);
	virtual void WriteRgbMulti(TInt aX,TInt aY,TInt aWidth,TInt aLength,TRgb aColor);
	virtual void WriteLine(TInt aX,TInt aY,TInt aLength,TUint32* aBuffer);
	virtual void WriteRgbAlphaMulti(TInt aX,TInt aY,TInt aLength,TRgb aColor,const TUint8* aMaskBuffer);
	virtual void MapColorToUserDisplayMode(TRgb& aColor);
	virtual void MapBufferToUserDisplayMode(TInt aLength,TUint32* aBuffer);
protected:
	virtual void SetSize(const TSize& aSize);
	TUint32 MapInt(TUint32 aInt,TUint32* aColorMap) const;
	virtual void ReadLine(TInt aX,TInt aY,TInt aLength,TAny* aBuffer) const;
	virtual void Shadow(TRgb& aColor);
	TUint32 ShadowWord(TUint32 aWord);
	TUint32 FadeWord(TUint32 aWord);
	void ShadeBuffer(TInt aLength,TUint32* aBuffer);
	void ShadowBuffer(TInt aLength,TUint32* aBuffer);
private:
	virtual void WriteRgbAlphaLine(TInt aX,
                                   TInt aY,
                                   TInt aLength,
                                   const TUint8* aRgbBuffer,
                                   const TUint8* aMaskBuffer,
                                   MAlphaBlend::TShadowing aShadowing,
                                   CGraphicsContext::TDrawMode aDrawMode);
	// From MOutlineAndShadowBlend
	virtual TInt WriteRgbOutlineAndShadow(TInt aX, TInt aY, const TInt aLength, 
	                                   TUint32 aOutlinePenColor, TUint32 aShadowColor, 
	                                   TUint32 aFillColor, const TUint8* aDataBuffer);
	};

/**
@publishedPartner
*/
NONSHARABLE_CLASS(CDrawFourBppBitmapGray) : public CDrawBitmap
	{
public:
	TInt Construct(TSize aSize);
	TInt Construct(TSize aSize, TInt aStride);
	TUint32 ColorInt(TRgb aColor) const;
	TUint32 HashInt(TRgb aColor,TInt aX,TInt aY) const;
	virtual void InvertBuffer(TInt aLength,TUint32* aBuffer);
	virtual TRgb ReadRgbNormal(TInt aX,TInt aY) const;
	virtual void ShadowArea(const TRect& aRect);
	virtual void WriteRgbMultiXOR(TInt aX,TInt aY,TInt aWidth,TInt aLength,TRgb aColor);
	virtual void WriteRgbMultiAND(TInt aX,TInt aY,TInt aWidth,TInt aLength,TRgb aColor);
	virtual void WriteRgbMultiOR(TInt aX,TInt aY,TInt aWidth,TInt aLength,TRgb aColor);
	virtual void WriteLineXOR(TInt aX,TInt aY,TInt aLength,TUint32* aBuffer);
	virtual void WriteLineAND(TInt aX,TInt aY,TInt aLength,TUint32* aBuffer);
	virtual void WriteLineOR(TInt aX,TInt aY,TInt aLength,TUint32* aBuffer);
	virtual void WriteRgb(TInt aX,TInt aY,TRgb aColor);
	virtual void WriteBinary(TInt aX,TInt aY,TUint32* aBuffer,TInt aLength,TInt aHeight,TRgb aColor);
	virtual void WriteBinaryOp(TInt aX,TInt aY,TUint32* aBuffer,TInt aLength,TInt aHeight,TRgb aColor,CGraphicsContext::TDrawMode aDrawMode);
	virtual void WriteBinaryLineVertical(TInt aX,TInt aY,TUint32* aBuffer,TInt aLength,TRgb aColor,TBool aUp);
	virtual void WriteRgbMulti(TInt aX,TInt aY,TInt aWidth,TInt aLength,TRgb aColor);
	virtual void WriteLine(TInt aX,TInt aY,TInt aLength,TUint32* aBuffer);
	virtual void WriteRgbAlphaMulti(TInt aX,TInt aY,TInt aLength,TRgb aColor,const TUint8* aMaskBuffer);
	virtual void MapColorToUserDisplayMode(TRgb& aColor);
	virtual void MapBufferToUserDisplayMode(TInt aLength,TUint32* aBuffer);
protected:
	virtual void SetSize(const TSize& aSize);
	void DitherBuffer(TInt x,TInt y,TInt aLength,TUint32* aBuffer);
	virtual void ReadLine(TInt aX,TInt aY,TInt aLength,TAny* aBuffer) const;
	virtual void Shadow(TRgb& aColor);
	TUint8 ShadowAndFadeGray16(TInt aGray16);
	TUint32 FadeWord(TUint32 aWord);
	void ShadeBuffer(TInt aLength,TUint32* aBuffer);
	void ShadowBuffer(TInt aLength,TUint32* aBuffer);
private:
	virtual void WriteRgbAlphaLine(TInt aX,
                                   TInt aY,
                                   TInt aLength,
                                   const TUint8* aRgbBuffer,
                                   const TUint8* aMaskBuffer,
                                   MAlphaBlend::TShadowing aShadowing,
                                   CGraphicsContext::TDrawMode aDrawMode);
	// From MOutlineAndShadowBlend
	virtual TInt WriteRgbOutlineAndShadow(TInt aX, TInt aY, const TInt aLength, 
	                                   TUint32 aOutlinePenColor, TUint32 aShadowColor, 
	                                   TUint32 aFillColor, const TUint8* aDataBuffer);
	TRgb BlendFourColors(TUint32 aOutlinePenColor, TUint32 aShadowColor, TUint32 aFillColor,
						TInt aRedOutlinePenColor,TInt aRedShadowColor,TInt aRedFillColor,
						TInt aGreenOutlinePenColor, TInt aGreenShadowColor, TInt aGreenFillColor,
						TInt aBlueOutlinePenColor, TInt aBlueShadowColor, TInt aBlueFillColor,
						TRgb aBackgroundColor, TUint8 aIndex) const;
	};

/**
@publishedPartner
*/
NONSHARABLE_CLASS(CDrawFourBppBitmapColor) : public CDrawBitmap
	{
public:
	TInt Construct(TSize aSize);
	TInt Construct(TSize aSize, TInt aStride);
	TUint32 ColorInt(TRgb aColor) const;
	virtual void InvertBuffer(TInt aLength,TUint32* aBuffer);
	virtual TRgb ReadRgbNormal(TInt aX,TInt aY) const;
	virtual void ShadowArea(const TRect& aRect);
	virtual void WriteRgbMultiXOR(TInt aX,TInt aY,TInt aWidth,TInt aLength,TRgb aColor);
	virtual void WriteRgbMultiAND(TInt aX,TInt aY,TInt aWidth,TInt aLength,TRgb aColor);
	virtual void WriteRgbMultiOR(TInt aX,TInt aY,TInt aWidth,TInt aLength,TRgb aColor);
	virtual void WriteLineXOR(TInt aX,TInt aY,TInt aLength,TUint32* aBuffer);
	virtual void WriteLineAND(TInt aX,TInt aY,TInt aLength,TUint32* aBuffer);
	virtual void WriteLineOR(TInt aX,TInt aY,TInt aLength,TUint32* aBuffer);
	virtual void WriteRgb(TInt aX,TInt aY,TRgb aColor);
	virtual void WriteBinary(TInt aX,TInt aY,TUint32* aBuffer,TInt aLength,TInt aHeight,TRgb aColor);
	virtual void WriteBinaryOp(TInt aX,TInt aY,TUint32* aBuffer,TInt aLength,TInt aHeight,TRgb aColor,CGraphicsContext::TDrawMode aDrawMode);
	virtual void WriteBinaryLineVertical(TInt aX,TInt aY,TUint32* aBuffer,TInt aLength,TRgb aColor,TBool aUp);
	virtual void WriteRgbMulti(TInt aX,TInt aY,TInt aWidth,TInt aLength,TRgb aColor);
	virtual void WriteLine(TInt aX,TInt aY,TInt aLength,TUint32* aBuffer);
	virtual void WriteRgbAlphaMulti(TInt aX,TInt aY,TInt aLength,TRgb aColor,const TUint8* aMaskBuffer);
	virtual void MapColorToUserDisplayMode(TRgb& aColor);
	virtual void MapBufferToUserDisplayMode(TInt aLength,TUint32* aBuffer);
protected:
	virtual void SetSize(const TSize& aSize);
	virtual void ReadLine(TInt aX,TInt aY,TInt aLength,TAny* aBuffer) const;
	virtual void Shadow(TRgb& aColor);
	TUint32 ShadowWord(TUint32 aWord);
	TUint32 FadeWord(TUint32 aWord);
	void ShadowBuffer(TInt aLength,TUint32* aBuffer);
private:
	virtual void WriteRgbAlphaLine(TInt aX,
                                   TInt aY,
                                   TInt aLength,
                                   const TUint8* aRgbBuffer,
                                   const TUint8* aMaskBuffer,
                                   MAlphaBlend::TShadowing aShadowing,
                                   CGraphicsContext::TDrawMode aDrawMode);
	// From MOutlineAndShadowBlend
	virtual TInt WriteRgbOutlineAndShadow(TInt aX, TInt aY, const TInt aLength, 
	                                   TUint32 aOutlinePenColor, TUint32 aShadowColor, 
	                                   TUint32 aFillColor, const TUint8* aDataBuffer);
	TRgb BlendFourColors(TUint32 aOutlinePenColor, TUint32 aShadowColor, TUint32 aFillColor,
						TInt aRedOutlinePenColor,TInt aRedShadowColor,TInt aRedFillColor,
						TInt aGreenOutlinePenColor, TInt aGreenShadowColor, TInt aGreenFillColor,
						TInt aBlueOutlinePenColor, TInt aBlueShadowColor, TInt aBlueFillColor,
						TRgb aBackgroundColor, TUint8 aIndex) const;
	};

const TInt KInvalidValue = 0xABCDABCD;

/**
@publishedPartner
*/
NONSHARABLE_CLASS(CDrawEightBppBitmapCommon) : public CDrawBitmap, public MFastBlit2
	{
public:
	virtual void InvertBuffer(TInt aLength,TUint32* aBuffer);
	virtual void WriteLine(TInt aX,TInt aY,TInt aLength,TUint32* aBuffer);
	virtual void WriteLineXOR(TInt aX,TInt aY,TInt aLength,TUint32* aBuffer);
	virtual void WriteLineAND(TInt aX,TInt aY,TInt aLength,TUint32* aBuffer);
	virtual void WriteLineOR(TInt aX,TInt aY,TInt aLength,TUint32* aBuffer);
	virtual TInt GetInterface(TInt aInterfaceId, TAny*& aInterface);	
	// From MFastBlit2
	virtual TInt WriteBitmapBlock(const TPoint& aDest,
									CFbsDrawDevice* aSrcDrawDevice,
									const TRect& aSrcRect);
	virtual TInt WriteBitmapBlock(const TPoint& aDest,
									const TUint32* aSrcBase,
									TInt aSrcStride,
									const TSize& aSrcSize,
									const TRect& aSrcRect);
	virtual const TUint32* Bits() const;
protected:
	virtual void SetSize(const TSize& aSize);
	TInt Construct(TSize aSize, TInt aStride);
	TUint8* PixelAddress(TInt aX,TInt aY) const;
	void ReadLine(TInt aX,TInt aY,TInt aLength,TAny* aBuffer) const;
	void WriteBinary(TInt aX,TInt aY,TUint32* aBuffer,TInt aLength,TInt aHeight,TUint8 aPixel);
	void WriteBinaryOp(TInt aX,TInt aY,TUint32* aBuffer,TInt aLength,TInt aHeight,TUint8 aPixel,CGraphicsContext::TDrawMode aDrawMode);
	void WriteBinaryLineVertical(TInt aX,TInt aY,TUint32* aBuffer,TInt aLength,TUint8 aPixel,TBool aUp);
	void WriteRgb(TInt aX,TInt aY,TUint8 aPixel);
	void WriteRgbMulti(TInt aX,TInt aY,TInt aLength,TInt aHeight,TUint8 aPixel);
	void WriteRgbMultiXOR(TInt aX,TInt aY,TInt aLength,TInt aHeight,TUint8 aPixel);
	void WriteRgbMultiAND(TInt aX,TInt aY,TInt aLength,TInt aHeight,TUint8 aPixel);
	void WriteRgbMultiOR(TInt aX,TInt aY,TInt aLength,TInt aHeight,TUint8 aPixel);
	};

/**
@publishedPartner
*/
NONSHARABLE_CLASS(CDrawEightBppBitmapGray) : public CDrawEightBppBitmapCommon
	{
public:
	TInt Construct(TSize aSize);
	TInt Construct(TSize aSize, TInt aStride);
	virtual TRgb ReadRgbNormal(TInt aX,TInt aY) const;
	virtual void ShadowArea(const TRect& aRect);
	virtual void WriteRgbMulti(TInt aX,TInt aY,TInt aWidth,TInt aLength,TRgb aColor);
	virtual void WriteRgbMultiXOR(TInt aX,TInt aY,TInt aWidth,TInt aLength,TRgb aColor);
	virtual void WriteRgbMultiAND(TInt aX,TInt aY,TInt aWidth,TInt aLength,TRgb aColor);
	virtual void WriteRgbMultiOR(TInt aX,TInt aY,TInt aWidth,TInt aLength,TRgb aColor);
	virtual void WriteRgb(TInt aX,TInt aY,TRgb aColor);
	virtual void WriteBinary(TInt aX,TInt aY,TUint32* aBuffer,TInt aLength,TInt aHeight,TRgb aColor);
	virtual void WriteBinaryOp(TInt aX,TInt aY,TUint32* aBuffer,TInt aLength,TInt aHeight,TRgb aColor,CGraphicsContext::TDrawMode aDrawMode);
	virtual void WriteBinaryLineVertical(TInt aX,TInt aY,TUint32* aBuffer,TInt aLength,TRgb aColor,TBool aUp);
	virtual void WriteRgbAlphaMulti(TInt aX,TInt aY,TInt aLength,TRgb aColor,const TUint8* aMaskBuffer);
	virtual void MapColorToUserDisplayMode(TRgb& aColor);
	virtual void MapBufferToUserDisplayMode(TInt aLength,TUint32* aBuffer);
protected:
	virtual void Shadow(TRgb& aColor);
	void ShadowBuffer(TInt aLength,TUint32* aBuffer);
	TUint8 ShadowAndFade(TInt aGray256);
private:
	virtual void WriteRgbAlphaLine(TInt aX,
                                   TInt aY,
                                   TInt aLength,
                                   const TUint8* aRgbBuffer,
                                   const TUint8* aMaskBuffer,
                                   MAlphaBlend::TShadowing aShadowing,
                                   CGraphicsContext::TDrawMode aDrawMode);
	// From MOutlineAndShadowBlend
	virtual TInt WriteRgbOutlineAndShadow(TInt aX, TInt aY, const TInt aLength, 
	                                   TUint32 aOutlinePenColor, TUint32 aShadowColor, 
	                                   TUint32 aFillColor, const TUint8* aDataBuffer);
	};

/**
@publishedPartner
*/
NONSHARABLE_CLASS(CDrawEightBppBitmapColor) : public CDrawEightBppBitmapCommon
	{
public:
	~CDrawEightBppBitmapColor();
	TInt Construct(TSize aSize);
	TInt Construct(TSize aSize, TInt aStride);
	virtual TRgb ReadRgbNormal(TInt aX,TInt aY) const;
	virtual TInt SetCustomPalette(const CPalette* aPalette);
	virtual TInt GetCustomPalette(CPalette*& aPalette);
	virtual void ShadowArea(const TRect& aRect);
	virtual void WriteRgbMulti(TInt aX,TInt aY,TInt aWidth,TInt aLength,TRgb aColor);
	virtual void WriteRgbMultiXOR(TInt aX,TInt aY,TInt aWidth,TInt aLength,TRgb aColor);
	virtual void WriteRgbMultiAND(TInt aX,TInt aY,TInt aWidth,TInt aLength,TRgb aColor);
	virtual void WriteRgbMultiOR(TInt aX,TInt aY,TInt aWidth,TInt aLength,TRgb aColor);
	virtual void WriteRgb(TInt aX,TInt aY,TRgb aColor);
	virtual void WriteBinary(TInt aX,TInt aY,TUint32* aBuffer,TInt aLength,TInt aHeight,TRgb aColor);
	virtual void WriteBinaryOp(TInt aX,TInt aY,TUint32* aBuffer,TInt aLength,TInt aHeight,TRgb aColor,CGraphicsContext::TDrawMode aDrawMode);
	virtual void WriteBinaryLineVertical(TInt aX,TInt aY,TUint32* aBuffer,TInt aLength,TRgb aColor,TBool aUp);
	virtual void WriteRgbAlphaMulti(TInt aX,TInt aY,TInt aLength,TRgb aColor,const TUint8* aMaskBuffer);
	virtual void MapColorToUserDisplayMode(TRgb& aColor);
	virtual void MapBufferToUserDisplayMode(TInt aLength,TUint32* aBuffer);
	// From MOutlineAndShadowBlend
	virtual TInt WriteRgbOutlineAndShadow(TInt aX, TInt aY, const TInt aLength, 
	                                   TUint32 aOutlinePenColor, TUint32 aShadowColor, 
	                                   TUint32 aFillColor, const TUint8* aDataBuffer);
protected:
	TUint8 ColorToIndex(TRgb aColor) const;
	TRgb IndexToColor(TInt aIndex) const;
	virtual void Shadow(TRgb& aColor);
	void ShadowBuffer(TInt aLength,TUint32* aBuffer);
private:
	virtual void WriteRgbAlphaLine(TInt aX,
                                   TInt aY,
                                   TInt aLength,
                                   const TUint8* aRgbBuffer,
                                   const TUint8* aMaskBuffer,
                                   MAlphaBlend::TShadowing aShadowing,
                                   CGraphicsContext::TDrawMode aDrawMode);
protected:
	TRgb* iPalette;
	TUint8* iColor4KIndex;
	TUint8* iShadowIndex;
	};

/**
@publishedPartner
*/
NONSHARABLE_CLASS(CDrawSixteenBppBitmapCommon) : public CDrawBitmap, public MFastBlit2
	{
public:
	TInt Construct(TSize aSize, TInt aStride);
	virtual void InvertBuffer(TInt aLength,TUint32* aBuffer);
	virtual void ShadowArea(const TRect& aRect);
	virtual void WriteRgbMultiXOR(TInt aX,TInt aY,TInt aLength,TInt aHeight,TUint16 aColor);
	virtual void WriteRgbMultiAND(TInt aX,TInt aY,TInt aLength,TInt aHeight,TUint16 aColor);
	virtual void WriteRgbMultiOR(TInt aX,TInt aY,TInt aLength,TInt aHeight,TUint16 aColor);
	virtual void WriteLineXOR(TInt aX,TInt aY,TInt aLength,TUint32* aBuffer);
	virtual void WriteLineAND(TInt aX,TInt aY,TInt aLength,TUint32* aBuffer);
	virtual void WriteLineOR(TInt aX,TInt aY,TInt aLength,TUint32* aBuffer);
	virtual void WriteBinary(TInt aX,TInt aY,TUint32* aBuffer,TInt aLength,TInt aHeight,TUint16 aColor);
	virtual void WriteBinaryOp(TInt aX,TInt aY,TUint32* aBuffer,TInt aLength,TInt aHeight,TUint16 aColor,CGraphicsContext::TDrawMode aDrawMode);
	virtual void WriteBinaryLineVertical(TInt aX,TInt aY,TUint32* aBuffer,TInt aLength,TUint16 aColor,TBool aUp);
	virtual void WriteRgbMulti(TInt aX,TInt aY,TInt aLength,TInt aHeight,TUint16 aColor);
	virtual void WriteLine(TInt aX,TInt aY,TInt aLength,TUint32* aBuffer);
	virtual TInt GetInterface(TInt aInterfaceId, TAny*& aInterface);
	// From MFastBlit2
	virtual TInt WriteBitmapBlock(const TPoint& aDest,
									CFbsDrawDevice* aSrcDrawDevice,
									const TRect& aSrcRect);
	virtual TInt WriteBitmapBlock(const TPoint& aDest,
									const TUint32* aSrcBase,
									TInt aSrcStride,
									const TSize& aSrcSize,
									const TRect& aSrcRect);
	virtual const TUint32* Bits() const;
protected:
	virtual void SetSize(const TSize& aSize);
	TUint16* PixelAddress(TInt aX,TInt aY) const;
	virtual void ReadLine(TInt aX,TInt aY,TInt aLength,TAny* aBuffer) const;
	virtual TUint16 ShadowIndex(TUint16 aColor64KIndex) = 0;
	virtual TUint16 FadeIndex(TUint16 aColor4KIndex) = 0;
	void ShadowBuffer(TInt aLength,TUint32* aBuffer);
	};

/**
@publishedPartner
*/
NONSHARABLE_CLASS(CDrawTwelveBppBitmap) : public CDrawSixteenBppBitmapCommon
	{
public:
	TInt Construct(TSize aSize);
	TInt Construct(TSize aSize, TInt aStride);
	virtual TRgb ReadRgbNormal(TInt aX,TInt aY) const;
	virtual void WriteRgbMultiXOR(TInt aX,TInt aY,TInt aWidth,TInt aLength,TRgb aColor);
	virtual void WriteRgbMultiAND(TInt aX,TInt aY,TInt aWidth,TInt aLength,TRgb aColor);
	virtual void WriteRgbMultiOR(TInt aX,TInt aY,TInt aWidth,TInt aLength,TRgb aColor);
	virtual void WriteRgb(TInt aX,TInt aY,TRgb aColor);
	virtual void WriteBinary(TInt aX,TInt aY,TUint32* aBuffer,TInt aLength,TInt aHeight,TRgb aColor);
	virtual void WriteBinaryOp(TInt aX,TInt aY,TUint32* aBuffer,TInt aLength,TInt aHeight,TRgb aColor,CGraphicsContext::TDrawMode aDrawMode);
	virtual void WriteBinaryLineVertical(TInt aX,TInt aY,TUint32* aBuffer,TInt aLength,TRgb aColor,TBool aUp);
	virtual void WriteRgbMulti(TInt aX,TInt aY,TInt aWidth,TInt aLength,TRgb aColor);
	virtual void WriteRgbAlphaMulti(TInt aX,TInt aY,TInt aLength,TRgb aColor,const TUint8* aMaskBuffer);
	virtual void MapColorToUserDisplayMode(TRgb& aColor);
	virtual void MapBufferToUserDisplayMode(TInt aLength,TUint32* aBuffer);
	// From MOutlineAndShadowBlend
	virtual TInt WriteRgbOutlineAndShadow(TInt aX, TInt aY, const TInt aLength, 
	                                   TUint32 aOutlinePenColor, TUint32 aShadowColor, 
	                                   TUint32 aFillColor, const TUint8* aDataBuffer);
protected:
	virtual void Shadow(TRgb& aColor);
	virtual TUint16 ShadowIndex(TUint16 aColor4KIndex);
	virtual TUint16 FadeIndex(TUint16 aColor4KIndex);
private:
	virtual void WriteRgbAlphaLine(TInt aX,
                                   TInt aY,
                                   TInt aLength,
                                   const TUint8* aRgbBuffer,
                                   const TUint8* aMaskBuffer,
                                   MAlphaBlend::TShadowing aShadowing,
                                   CGraphicsContext::TDrawMode aDrawMode);
	};

/**
@publishedPartner
*/
NONSHARABLE_CLASS(CDrawSixteenBppBitmap) : public CDrawSixteenBppBitmapCommon
	{
public:
	TInt Construct(TSize aSize);
	TInt Construct(TSize aSize, TInt aStride);
	virtual TRgb ReadRgbNormal(TInt aX,TInt aY) const;
	virtual void WriteRgbMultiXOR(TInt aX,TInt aY,TInt aWidth,TInt aLength,TRgb aColor);
	virtual void WriteRgbMultiAND(TInt aX,TInt aY,TInt aWidth,TInt aLength,TRgb aColor);
	virtual void WriteRgbMultiOR(TInt aX,TInt aY,TInt aWidth,TInt aLength,TRgb aColor);
	virtual void WriteRgb(TInt aX,TInt aY,TRgb aColor);
	virtual void WriteBinary(TInt aX,TInt aY,TUint32* aBuffer,TInt aLength,TInt aHeight,TRgb aColor);
	virtual void WriteBinaryOp(TInt aX,TInt aY,TUint32* aBuffer,TInt aLength,TInt aHeight,TRgb aColor,CGraphicsContext::TDrawMode aDrawMode);
	virtual void WriteBinaryLineVertical(TInt aX,TInt aY,TUint32* aBuffer,TInt aLength,TRgb aColor,TBool aUp);
	virtual void WriteRgbMulti(TInt aX,TInt aY,TInt aWidth,TInt aLength,TRgb aColor);
	virtual void WriteRgbAlphaMulti(TInt aX,TInt aY,TInt aLength,TRgb aColor,const TUint8* aMaskBuffer);
	virtual void MapColorToUserDisplayMode(TRgb& aColor);
	FORCEINLINE void MapColorToUserDisplayMode(TUint16& aColor64K);
	virtual void MapBufferToUserDisplayMode(TInt aLength,TUint32* aBuffer);
	virtual void BlendRgbMulti(TInt aX,TInt aY,TInt aWidth,TInt aLength,TRgb aColor);
	// From MOutlineAndShadowBlend
	virtual TInt WriteRgbOutlineAndShadow(TInt aX, TInt aY, const TInt aLength, 
	                                   TUint32 aOutlinePenColor, TUint32 aShadowColor, 
	                                   TUint32 aFillColor, const TUint8* aDataBuffer);
protected:
	virtual void Shadow(TRgb& aColor);
	FORCEINLINE void Shadow(TInt& aRed, TInt& aGreen, TInt& aBlue);
	FORCEINLINE void Shadow(TUint16& a64KColor);
	virtual TUint16 ShadowIndex(TUint16 aColor64KIndex);
	FORCEINLINE void ShadowIndex(TInt& aRed, TInt& aGreen, TInt& aBlue);
	virtual TUint16 FadeIndex(TUint16 aColor64KIndex);
private:
	virtual void WriteRgbAlphaLine(TInt aX,
                                   TInt aY,
                                   TInt aLength,
                                   const TUint8* aRgbBuffer,
                                   const TUint8* aMaskBuffer,
                                   MAlphaBlend::TShadowing aShadowing,
                                   CGraphicsContext::TDrawMode aDrawMode);
	};

/**
@publishedPartner
*/
NONSHARABLE_CLASS(CDrawTwentyFourBppBitmap) : public CDrawBitmap
	{
public:
	TInt Construct(TSize aSize);
	TInt Construct(TSize aSize, TInt aStride);
	virtual void Shadow(TRgb& aColor);
	virtual void InvertBuffer(TInt aLength,TUint32* aBuffer);
	virtual TRgb ReadRgbNormal(TInt aX,TInt aY) const;
	virtual void ShadowArea(const TRect& aRect);
	virtual void WriteRgb(TInt aX,TInt aY,TRgb aColor);
	virtual void WriteBinary(TInt aX,TInt aY,TUint32* aBuffer,TInt aLength,TInt aHeight,TRgb aColor);
	virtual void WriteBinaryOp(TInt aX,TInt aY,TUint32* aBuffer,TInt aLength,TInt aHeight,TRgb aColor,CGraphicsContext::TDrawMode aDrawMode);
	virtual void WriteBinaryLineVertical(TInt aX,TInt aY,TUint32* aBuffer,TInt aLength,TRgb aColor,TBool aUp);
	virtual void WriteLine(TInt aX,TInt aY,TInt aLength,TUint32* aBuffer);
	virtual void WriteLineXOR(TInt aX,TInt aY,TInt aLength,TUint32* aBuffer);
	virtual void WriteLineAND(TInt aX,TInt aY,TInt aLength,TUint32* aBuffer);
	virtual void WriteLineOR(TInt aX,TInt aY,TInt aLength,TUint32* aBuffer);
	virtual void WriteRgbMulti(TInt aX,TInt aY,TInt aLength,TInt aHeight,TRgb aColor);
	virtual void WriteRgbMultiXOR(TInt aX,TInt aY,TInt aLength,TInt aHeight,TRgb aColor);
	virtual void WriteRgbMultiAND(TInt aX,TInt aY,TInt aLength,TInt aHeight,TRgb aColor);
	virtual void WriteRgbMultiOR(TInt aX,TInt aY,TInt aLength,TInt aHeight,TRgb aColor);
	virtual void WriteRgbAlphaMulti(TInt aX,TInt aY,TInt aLength,TRgb aColor,const TUint8* aMaskBuffer);
	virtual void MapColorToUserDisplayMode(TRgb& aColor);
	virtual void MapBufferToUserDisplayMode(TInt aLength,TUint32* aBuffer);
	// From MOutlineAndShadowBlend
	virtual TInt WriteRgbOutlineAndShadow(TInt aX, TInt aY, const TInt aLength, 
	                                   TUint32 aOutlinePenColor, TUint32 aShadowColor, 
	                                   TUint32 aFillColor, const TUint8* aDataBuffer);
	void BlendRgbMulti(TInt aX,TInt aY,TInt aLength,TInt aHeight,TRgb aColor);
protected:
	virtual void SetSize(const TSize& aSize);
	TUint8* PixelAddress(TInt aX,TInt aY) const;
	TInt PixelAddressIncrement() const;
	void PixelAddressIncrement(TInt& aPixelInc,TInt& aRowInc) const;
	void ReadLine(TInt aX,TInt aY,TInt aLength,TAny* aBuffer) const;
	void ShadowBuffer(TInt aLength,TUint32* aBuffer);
	TUint8 ShadowComponent(TInt aRgbComponent);
	TUint8 FadeComponent(TInt aRgbComponent);
	TUint8 ShadowAndFade(TInt aComponent);
private:
	FORCEINLINE void FadeRgb(TInt& red,TInt& green,TInt& blue);
	FORCEINLINE void Shadow(TInt& red,TInt& green,TInt& blue);
	FORCEINLINE TUint8 ShadowComponentInl(TInt aRgbComponent);
	virtual void WriteRgbAlphaLine(TInt aX,
                                   TInt aY,
                                   TInt aLength,
                                   const TUint8* aRgbBuffer,
                                   const TUint8* aMaskBuffer,
                                   MAlphaBlend::TShadowing aShadowing,
                                   CGraphicsContext::TDrawMode aDrawMode);
protected:
	TInt iScanLineBytes;
	};

inline TUint8* CDrawTwentyFourBppBitmap::PixelAddress(TInt aX,TInt aY) const
	{
	return ((TUint8*)iBits) + (aY * iScanLineBytes) + (aX * 3);
	}

/**
@publishedPartner
*/
NONSHARABLE_CLASS(CDrawThirtyTwoBppBitmapCommon) : public CDrawBitmap, public MFastBlit2
	{
public:
	TInt Construct(TSize aSize, TInt aStride);
	virtual void Shadow(TRgb& aColor);
	virtual void Shadow(TUint32& aColor);
	virtual void InvertBuffer(TInt aLength,TUint32* aBuffer);
	virtual TRgb ReadRgbNormal(TInt aX,TInt aY) const;
	virtual void ShadowArea(const TRect& aRect);
	virtual void WriteRgb(TInt aX,TInt aY,TRgb aColor);
	virtual void WriteBinary(TInt aX,TInt aY,TUint32* aBuffer,TInt aLength,TInt aHeight,TRgb aColor);
	virtual void WriteBinaryOp(TInt aX,TInt aY,TUint32* aBuffer,TInt aLength,TInt aHeight,TRgb aColor,CGraphicsContext::TDrawMode aDrawMode);
	virtual void WriteBinaryLineVertical(TInt aX,TInt aY,TUint32* aBuffer,TInt aLength,TRgb aColor,TBool aUp);
	virtual void WriteLine(TInt aX,TInt aY,TInt aLength,TUint32* aBuffer);
	virtual void WriteLineXOR(TInt aX,TInt aY,TInt aLength,TUint32* aBuffer);
	virtual void WriteLineAND(TInt aX,TInt aY,TInt aLength,TUint32* aBuffer);
	virtual void WriteLineOR(TInt aX,TInt aY,TInt aLength,TUint32* aBuffer);
	virtual void WriteRgbMulti(TInt aX,TInt aY,TInt aLength,TInt aHeight,TRgb aColor);
	virtual void WriteRgbMultiXOR(TInt aX,TInt aY,TInt aLength,TInt aHeight,TRgb aColor);
	virtual void WriteRgbMultiAND(TInt aX,TInt aY,TInt aLength,TInt aHeight,TRgb aColor);
	virtual void WriteRgbMultiOR(TInt aX,TInt aY,TInt aLength,TInt aHeight,TRgb aColor);
	virtual void WriteRgbAlphaMulti(TInt aX,TInt aY,TInt aLength,TRgb aColor,const TUint8* aMaskBuffer);
	virtual void MapColorToUserDisplayMode(TRgb& aColor);
	virtual void MapBufferToUserDisplayMode(TInt aLength,TUint32* aBuffer);
	virtual TInt GetInterface(TInt aInterfaceId, TAny*& aInterface);
	// From MFastBlit2
	virtual TInt WriteBitmapBlock(const TPoint& aDest,
									CFbsDrawDevice* aSrcDrawDevice,
									const TRect& aSrcRect);
	virtual TInt WriteBitmapBlock(const TPoint& aDest,
									const TUint32* aSrcBase,
									TInt aSrcStride,
									const TSize& aSrcSize,
									const TRect& aSrcRect);
	virtual const TUint32* Bits() const;
protected:
	virtual TUint32 Color(const TRgb& aColor) = 0;
	virtual TRgb RgbColor(TUint32 aColor) const = 0;
	virtual void SetSize(const TSize& aSize);
	FORCEINLINE TUint32* PixelAddress(TInt aX,TInt aY) const;
	FORCEINLINE TInt PixelAddressIncrement() const;
	void ReadLine(TInt aX,TInt aY,TInt aLength,TAny* aBuffer) const;
	void ShadowBuffer(TInt aLength,TUint32* aBuffer);
	FORCEINLINE TUint8 ShadowComponent(TInt aRgbComponent);
	TUint8 FadeComponent(TInt aRgbComponent);
	TUint8 ShadowAndFade(TInt aComponent);
private:
	virtual void WriteRgbAlphaLine(TInt aX,
                                   TInt aY,
                                   TInt aLength,
                                   const TUint8* aRgbBuffer,
                                   const TUint8* aMaskBuffer,
                                   MAlphaBlend::TShadowing aShadowing,
                                   CGraphicsContext::TDrawMode aDrawMode);
	};

FORCEINLINE TUint32* CDrawThirtyTwoBppBitmapCommon::PixelAddress(TInt aX,TInt aY) const
	{
	return iBits + aY * iScanLineWords + aX;
	}

FORCEINLINE TInt CDrawThirtyTwoBppBitmapCommon::PixelAddressIncrement() const
	{
	switch (iOrientation)
		{
	case EOrientationNormal:
		return 1;
	case EOrientationRotated90:
		return iScanLineWords;
	case EOrientationRotated180:
		return -1;
	case EOrientationRotated270:
		return -iScanLineWords;
	default:
		return 1;
		}
	}


/**
@publishedPartner
*/
NONSHARABLE_CLASS(CDrawUTwentyFourBppBitmap) : public CDrawThirtyTwoBppBitmapCommon, MFastBlit
	{
public:
	TInt Construct(TSize aSize);
	TInt Construct(TSize aSize, TInt aStride);
	virtual TDisplayMode ScanLineDisplayMode() const { return EColor16MAP; }
	virtual void ReadLine(TInt aX, TInt aY, TInt aLength, TAny* aBuffer, TDisplayMode aDispMode) const;
	virtual TInt GetInterface(TInt aInterfaceId, TAny*& aInterface);
	// From MFastBlit
	virtual void WriteAlphaLineEx(TInt aX,
						TInt aY,
						TInt aLength,
						TInt aSrcX,
						const TUint32* aSrcPtr,
						TDisplayMode aSrcFormat,
						TInt aMaskX,
						const TUint32* aMaskPtr,
						MAlphaBlend::TShadowing aShadowing);
	virtual void WriteMaskLineEx(TInt aX,
						TInt aY,
						TInt aLength,
						TInt aSrcX,
						const TUint32* aSrcPtr,
						TDisplayMode aSrcFormat,
						TInt aMaskX,
						const TUint32* aMaskPtr,
						TBool aInvertMask);
	void BlendRgbMulti(TInt aX,TInt aY,TInt aLength,TInt aHeight,TRgb aColor);
	void BlendLine(TInt aX,TInt aY,TInt aLength,TUint32* aBuffer);
	// From MOutlineAndShadowBlend
	virtual TInt WriteRgbOutlineAndShadow(TInt aX, TInt aY, const TInt aLength, 
	                                   TUint32 aOutlinePenColor, TUint32 aShadowColor, 
	                                   TUint32 aFillColor, const TUint8* aDataBuffer);
protected:
	virtual TUint32 Color(const TRgb& aColor);
	virtual TRgb RgbColor(TUint32 aColor) const;
private:
	virtual void WriteRgbAlphaLine(TInt aX,
                                   TInt aY,
                                   TInt aLength,
                                   const TUint8* aRgbBuffer,
                                   const TUint8* aMaskBuffer,
                                   MAlphaBlend::TShadowing aShadowing,
                                   CGraphicsContext::TDrawMode aDrawMode);
	};


/**
@publishedPartner
*/
class CDrawThirtyTwoBppBitmapAlpha : public CDrawThirtyTwoBppBitmapCommon
	{
public:
	TInt Construct(TSize aSize);
	TInt Construct(TSize aSize, TInt aStride);
	// from CDrawThirtyTwoBppBitmap
	void WriteRgb(TInt aX,TInt aY,TRgb aColor);
	void WriteBinary(TInt aX,TInt aY,TUint32* aBuffer,TInt aLength,TInt aHeight,TRgb aColor);
	void WriteBinaryLineVertical(TInt aX,TInt aY,TUint32* aBuffer,TInt aLength,TRgb aColor,TBool aUp);
	void WriteRgbAlphaMulti(TInt aX,TInt aY,TInt aLength,TRgb aColor,const TUint8* aMaskBuffer);
	void MapBufferToUserDisplayMode(TInt aLength,TUint32* aBuffer);
	void BlendRgbMulti(TInt aX,TInt aY,TInt aWidth,TInt aLength,TRgb aColor);
	void BlendLine(TInt aX,TInt aY,TInt aLength,TUint32* aBuffer);
	void ShadowArea(const TRect& aRect);
	// From MOutlineAndShadowBlend
	virtual TInt WriteRgbOutlineAndShadow(TInt aX, TInt aY, const TInt aLength, 
	                                   TUint32 aOutlinePenColor, TUint32 aShadowColor, 
	                                   TUint32 aFillColor, const TUint8* aDataBuffer);
protected:
	virtual TUint32 Color(const TRgb& aColor);
	virtual TRgb RgbColor(TUint32 aColor) const;
	using CDrawThirtyTwoBppBitmapCommon::Shadow;
	void Shadow(TUint32& aColor);
private:
	virtual void WriteRgbAlphaLine(TInt aX,
                                   TInt aY,
                                   TInt aLength,
                                   const TUint8* aRgbBuffer,
                                   const TUint8* aMaskBuffer,
                                   MAlphaBlend::TShadowing aShadowing,
                                   CGraphicsContext::TDrawMode aDrawMode);
	};


/**
@publishedPartner
*/
class CDrawThirtyTwoBppBitmapAlphaPM : public CDrawThirtyTwoBppBitmapCommon
	{
public:
	TInt Construct(TSize aSize);
	TInt Construct(TSize aSize, TInt aStride);
	// from CDrawThirtyTwoBppBitmap
	void WriteRgb(TInt aX,TInt aY,TRgb aColor);
	void WriteBinary(TInt aX,TInt aY,TUint32* aBuffer,TInt aLength,TInt aHeight,TRgb aColor);
	void WriteBinaryLineVertical(TInt aX,TInt aY,TUint32* aBuffer,TInt aLength,TRgb aColor,TBool aUp);
	void WriteRgbAlphaMulti(TInt aX,TInt aY,TInt aLength,TRgb aColor,const TUint8* aMaskBuffer);
	void BlendRgbMulti(TInt aX,TInt aY,TInt aWidth,TInt aLength,TRgb aColor);
	void BlendLine(TInt aX,TInt aY,TInt aLength,TUint32* aBuffer);
	void ShadowArea(const TRect& aRect);
	virtual void WriteLine(TInt aX,TInt aY,TInt aLength,TUint32* aBuffer,
		CGraphicsContext::TDrawMode aDrawMode);
	// From MOutlineAndShadowBlend
	virtual TInt WriteRgbOutlineAndShadow(TInt aX, TInt aY, const TInt aLength, 
	                                   TUint32 aOutlinePenColor, TUint32 aShadowColor, 
	                                   TUint32 aFillColor, const TUint8* aDataBuffer);
protected:
	virtual TUint32 Color(const TRgb& aColor);
	virtual TRgb RgbColor(TUint32 aColor) const;
private:
	virtual void WriteRgbAlphaLine(TInt aX,
                                   TInt aY,
                                   TInt aLength,
                                   const TUint8* aRgbBuffer,
                                   const TUint8* aMaskBuffer,
                                   MAlphaBlend::TShadowing aShadowing,
                                   CGraphicsContext::TDrawMode aDrawMode);
	};



#include "CoordTransformation.inl"

/** DO NOT have a declaration as the compiler will think the source is in the .cpp and make this non-inline

 Made a global inline to force the compiler to inline so as to gain performance.
 A member inlines are not guarenteed to be inline.

 ASSERT Primary Color >= 0 && Primary Color <= 255
 If <0 or >255, after a shift right of 8-bits should prove true.
 00000001 00000000 = 256 >> 8 = 00000000 00000001 = TRUE
 11111111 11111111 = -1  >> 8 = 00000000 11111111 = TRUE
 00000000 11111111 = 255 >> 8 = 00000000 00000000 = FALSE


 Initial AlphaValue = 0...1.0

 R = APs + (1 - A)Pd ...so if A=0 the destination pixel is more dominant
                        and if A=1 the sourec pixel is more dominant

 re-adjusting...

 R = APs + Pd - APd

 R = A(Ps-Pd) + Pd

 Passed-in AlphaValue = 0...255
 AlphaValue = 257 * Passed-in AlphaValue = 0...65536

 NOTE: AlphaValue is now a 16-bit number. the colours are 8-bit, so we shouldn't
 have overflow problems.
 NOTE: Negative values (Ps-Pd) shouldn't be a problem as the shifting right of
       signed values duplicates the signed bit in the msb.

 R = A(Ps-Pd) + Pd
     --------
      65536

 R = (A(Ps-Pd)) >> 16) + Pd
*/
inline TRgb AlphaBlend(TInt aPrimaryRed, TInt aPrimaryGreen, TInt aPrimaryBlue, TInt aSecondaryRed,
						TInt aSecondaryGreen, TInt aSecondaryBlue, TInt aAlphaValue)
	{
	__ASSERT_DEBUG(!(aPrimaryRed>>8) && !(aPrimaryGreen>>8) && !(aPrimaryBlue>>8) && !(aAlphaValue>>8) && 
					!(aSecondaryRed>>8) && !(aSecondaryGreen>>8) && !(aSecondaryBlue>>8),
					Panic(EScreenDriverPanicAlphaBlendInvariant));

	const TInt alphaValue = aAlphaValue * 257;
	return TRgb(((alphaValue * (aPrimaryRed   - aSecondaryRed))   >> 16) + aSecondaryRed,
	     		((alphaValue * (aPrimaryGreen - aSecondaryGreen)) >> 16) + aSecondaryGreen,
	     		((alphaValue * (aPrimaryBlue  - aSecondaryBlue))  >> 16) + aSecondaryBlue);
	}

inline TRgb AlphaBlend(TInt aPrimaryRed,TInt aPrimaryGreen,TInt aPrimaryBlue,TRgb aSecondary,TInt aAlphaValue)
	{
	return AlphaBlend(aPrimaryRed,aPrimaryGreen,aPrimaryBlue,aSecondary.Red(), aSecondary.Green(), aSecondary.Blue(), aAlphaValue);
	}

/**
It unpacks the 16 bit colour in to the red, green and blue colour components.
@param aColor64K The 16 bit colour which needs to be unpacked.
@param aRed   Red component of aColor64K is returned into this.
@param aGreen Green component of aColor64K is returned into this.
@param aBlue Blue component of aColor64K is returned into this.
*/
FORCEINLINE void UnpackColor64K(TUint16 aColor64K, TInt& aRed, TInt& aGreen, TInt& aBlue)
	{
	aRed  = (aColor64K&0xF800)>>8;
	aRed += aRed>>5;
	aGreen  = (aColor64K&0x07E0)>>3;
	aGreen += aGreen>>6;
	aBlue  = (aColor64K&0x001F)<<3;
	aBlue += aBlue>>5;
	}

/**
It creates the 16 bit colour from the red, green and blue colour components.
@param aRed   The Red component for creating 16 bit colour.
@param aGreen The Green component for creating 16 bit colour.
@param aBlue The Blue component for creating 16 bit colour.
@return TUint16 The 16 bit colour created from the colour components.
*/
FORCEINLINE TUint16 PackColor64K(TInt aRed, TInt aGreen, TInt aBlue)
	{
	TUint16 color64K = (aBlue & 0xF8) >> 3;
	color64K |= (aGreen & 0xFc) << 3;
	color64K |= (aRed & 0xF8) << 8;
	return color64K;
	}

/**
It is an overloaded function for alpha bending which does blending for 16 bit colour.
@param aPrimaryColor64K   The foreground pixel colour in 16 bit format.
@param aSecondaryColor64K The background pixel colour in 16 bit format.
@param aAlphaValue The alpha value used for blending.
@return The 16 bit blended colour.
*/
FORCEINLINE TUint16 AlphaBlend(TUint16 aPrimaryColor64K, TUint16 aSecondaryColor64K,TInt aAlphaValue)
	{
	TInt primaryRed;
	TInt primaryGreen;
	TInt primaryBlue;
	TInt secondaryRed;
	TInt secondaryGreen;
	TInt secondaryBlue;

	UnpackColor64K(aPrimaryColor64K, primaryRed, primaryGreen, primaryBlue);
	UnpackColor64K(aSecondaryColor64K, secondaryRed, secondaryGreen, secondaryBlue);

	__ASSERT_DEBUG(!(primaryRed>>8) && !(primaryGreen>>8) && !(primaryBlue>>8) && !(aAlphaValue>>8),
					Panic(EScreenDriverPanicAlphaBlendInvariant));

	const TInt alphaValue = aAlphaValue * 257;
	return PackColor64K(((alphaValue * (primaryRed   - secondaryRed))   >> 16) + secondaryRed,
	     		((alphaValue * (primaryGreen - secondaryGreen)) >> 16) + secondaryGreen,
	     		((alphaValue * (primaryBlue  - secondaryBlue))  >> 16) + secondaryBlue);
	}

/**
It is an overloaded function for alpha bending which does blending for 16 bit colour.
@param aPrimaryRed  Red component of foreground pixel colour.
@param aPrimaryGreen  Green component of foreground pixel colour.
@param aPrimaryBlue  Blue component of foreground pixel colour.
@param aSecondaryColor64K The background pixel colour in 16 bit format.
@param aAlphaValue The alpha value used for blending.
@return The 16 bit blended colour.
*/
FORCEINLINE TUint16 AlphaBlend(TInt aPrimaryRed,TInt aPrimaryGreen,TInt aPrimaryBlue, TUint16 aSecondaryColor64K,TInt aAlphaValue)
	{
	__ASSERT_DEBUG(!(aPrimaryRed>>8) && !(aPrimaryGreen>>8) && !(aPrimaryBlue>>8) && !(aAlphaValue>>8),
					Panic(EScreenDriverPanicAlphaBlendInvariant));

	if (aAlphaValue==0xFF)
		return(PackColor64K(aPrimaryRed, aPrimaryGreen, aPrimaryBlue));
	TInt secondaryRed;
	TInt secondaryGreen;
	TInt secondaryBlue;

	UnpackColor64K(aSecondaryColor64K, secondaryRed, secondaryGreen, secondaryBlue);

	const TInt alphaValue = aAlphaValue * 257;
	return PackColor64K(((alphaValue * (aPrimaryRed   - secondaryRed))   >> 16) + secondaryRed,
	     		((alphaValue * (aPrimaryGreen - secondaryGreen)) >> 16) + secondaryGreen,
	     		((alphaValue * (aPrimaryBlue  - secondaryBlue))  >> 16) + secondaryBlue);
	}

FORCEINLINE LOCAL_C TUint16 Conv32To16(TUint32 col)
	{
	TUint b = (col&0x000000ff)>>3;
	TUint g = (col&0x0000fc00)>>5;
	TUint r = (col&0x00f80000)>>8;
	return TUint16(r|g|b);
	}

// Alpha-blends an EColor16MA pixel to an EColor64K screen using a fixed mask value.
FORCEINLINE LOCAL_C TUint16 Blend32To16(TUint32 aSrc32, TUint aMask, TUint16 aDest16)
	{
	__ASSERT_DEBUG(aMask < 256, Panic(EScreenDriverPanicInvalidParameter));

	if(aMask)
		{
		if(aMask >= 0xFF)
			return Conv32To16(aSrc32);

		// blend red and blue channel in one go
		TUint32 d_rb = (aDest16 & 0xF800)<<8 | (aDest16 & 0x001F)<<3;
		d_rb |= (d_rb>>5) & 0x00ff00ff;
		const TUint32 s_rb = aSrc32 & 0x00FF00FF;
		const TUint32 mask2 = aMask | (aMask << 16);
		const TUint32 rb = ((((aMask * ((0x01000100 + s_rb) - d_rb)) >> 8) + d_rb) - mask2) & 0x00FF00FF;

		// do the green channel normally
		TInt32 d_g  = (aDest16 & 0x07E0)>>3;
		d_g |= d_g>>6;
		const TInt32 s_g = (aSrc32 & 0xFF00) >> 8;
		TInt g = (((aMask * (s_g - d_g)) >> 8) + d_g) & 0xFF;

		return (Conv32To16(rb | g<<8));
		}

	return aDest16;
	}

// As for Blend32To16, but assumes 0xFF and 0 alpha checks already done elsewhere
FORCEINLINE LOCAL_C TUint16 Blend32To16NoChecks(TUint32 aSrc32, TUint aMask, TUint16 aDest16)
	{
	// blend red and blue channel in one go
	TUint32 d_rb = (aDest16 & 0xF800)<<8 | (aDest16 & 0x001F)<<3;
	d_rb |= (d_rb>>5) & 0x00ff00ff;
	const TUint32 s_rb = aSrc32 & 0x00FF00FF;
	const TUint32 mask2 = aMask | (aMask << 16);
	const TUint32 rb = ((((aMask * ((0x01000100 + s_rb) - d_rb)) >> 8) + d_rb) - mask2) & 0x00FF00FF;

	// do the green channel normally
	TInt32 d_g  = (aDest16 & 0x07E0)>>3;
	d_g |= d_g>>6;
	const TInt32 s_g = (aSrc32 & 0xFF00) >> 8;
	TInt g = (((aMask * (s_g - d_g)) >> 8) + d_g) & 0xFF;

	return (Conv32To16(rb | g<<8));
	}

// Alpha-blends to an EColor64K screen using a fixed mask value.
// The source values are pre-split from a 32 bit source into the two following components:
//
// aSrcRB=aSrc32 & 0x00FF00FF;
// aSrcG=(aSrc32 & 0xFF00) >> 8;
//
// Mask is assumed not to be 0 or 255, these should be pre-checked for and never arrive here
//
FORCEINLINE LOCAL_C TUint16 BlendTo16(const TUint32 aSrcRB, const TUint32 aSrcG, TUint aMask, TUint16 aDest16)
	{
	// blend red and blue channel in one go
	TUint32 d_rb = (aDest16 & 0xF800)<<8 | (aDest16 & 0x001F)<<3;
	d_rb |= (d_rb>>5) & 0x00ff00ff;
	const TUint32 mask2 = aMask | (aMask << 16);
	const TUint32 rb = ((((aMask * ((0x01000100 + aSrcRB) - d_rb)) >> 8) + d_rb) - mask2) & 0x00FF00FF;

	// do the green channel normally
	TInt32 d_g  = (aDest16 & 0x07E0)>>3;
	d_g |= d_g>>6;
	const TInt32 g = (((aMask * (aSrcG - d_g)) >> 8) + d_g) & 0xFF;

	// inline conversion to 16 bit
	return TUint16(((rb&0x00f80000)>>8)|((g&0x0000fc)<<3)|((rb&0x000000ff)>>3));
	}

// Alpha-blends an EColor16MA pixel to an EColor16MU screen using a fixed mask value.
FORCEINLINE LOCAL_C TUint32 CalcAlphaPixel(const TUint32 aSrcPixel, const TUint8 aMask, TUint32 aDestPixel)
	{
	// (a)  (mask * src + (255 - mask) * dest) / 255           This ideal formula
	// (b)  ((mask * (src - dest)) >> 8) + dest                A faster approximation to (a)
	// (c)  ((mask * (256 + src - dest) >> 8) + dest - mask    Equivalent to (b) but can be used on multiple colors at a time

	if(aMask)
		{
		if(aMask == 0xFF)
			return 0xff000000|aSrcPixel;

		const TUint32 s_rb = aSrcPixel & 0x00FF00FF;
		const TUint32 d_rb = aDestPixel & 0x00FF00FF;
		const TUint32 mask2 = aMask | (aMask << 16);
		const TUint32 rb = ((((aMask * ((0x01000100 + s_rb) - d_rb)) >> 8) + d_rb) - mask2) & 0x00FF00FF;

		const TInt s_g = (aSrcPixel & 0xFF00) >> 8;
		const TInt d_g = (aDestPixel & 0xFF00) >> 8;
		const TInt g = ((aMask * (s_g - d_g)) >> 8) + d_g;

		return(rb | (g<<8) | 0xff000000);
		}

	return aDestPixel;

	}

// Alpha-blends an EColor16MA rgb to an EColor16MU screen using a fixed mask value.
FORCEINLINE LOCAL_C void AlphaBlendPixelToDest(TUint32 aSrcValue, const TUint8 aSourceAlpha, TUint32* aDestPtr)
 {
 *aDestPtr = CalcAlphaPixel(aSrcValue, aSourceAlpha, *aDestPtr);
 }

void MemFillTUint32(TUint32* tempWordPtr, TInt aCount,  const TUint32 aValue);

#endif

