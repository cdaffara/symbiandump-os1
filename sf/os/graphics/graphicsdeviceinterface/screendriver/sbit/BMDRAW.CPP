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

#include "BMDRAW.H"
#include "BitDrawInterfaceId.h"

GLDEF_C void Panic(TScreenDriverPanic aPanicCode)
	{
	_LIT(KSCDVPanicCategory,"SCDV");
	User::Panic(KSCDVPanicCategory,aPanicCode);
	}


/**
Alphablends a pixel.
The formula used for that, is:
(aPrimary * aAlphaValue + aSecondary * (255 - aAlphaValue)) / 255 - for each color (R,G,B).
@param aPrimary RGB color 1.
@param aSecondary RGB color 2.
@param aAlphaValue Mask.
@return Alpha blended value.
@internalComponent
*/
TRgb AlphaBlend(TRgb aPrimary, TRgb aSecondary, TInt aAlphaValue)
    {
    return ::AlphaBlend(aPrimary.Red(), aPrimary.Green(), aPrimary.Blue(), aSecondary, aAlphaValue);
    }

GLREF_D const TUint8 ditherlutab[16][4];

const TInt8 xIncArray[4] = { 1, 0, -1, 0 };
const TInt8 yIncArray[4] = { 0, 1, 0, -1 };

static CFbsDrawDevice* CreateBitmapDeviceL(const TSize& aSize, TDisplayMode aDispMode, TInt aDataStride)
	{
	CFbsDrawDevice* drawDevice = NULL;

	switch(aDispMode)
		{
	case EGray2:
		drawDevice = new(ELeave) CDrawOneBppBitmap;
		CleanupStack::PushL(drawDevice);
		User::LeaveIfError(((CDrawOneBppBitmap*)drawDevice)->Construct(aSize, aDataStride));
		break;
	case EGray4:
		drawDevice = new(ELeave) CDrawTwoBppBitmap;
		CleanupStack::PushL(drawDevice);
		User::LeaveIfError(((CDrawTwoBppBitmap*)drawDevice)->Construct(aSize, aDataStride));
		break;
	case EGray16:
		drawDevice = new(ELeave) CDrawFourBppBitmapGray;
		CleanupStack::PushL(drawDevice);
		User::LeaveIfError(((CDrawFourBppBitmapGray*)drawDevice)->Construct(aSize, aDataStride));
		break;
	case EGray256:
		drawDevice = new(ELeave) CDrawEightBppBitmapGray;
		CleanupStack::PushL(drawDevice);
		User::LeaveIfError(((CDrawEightBppBitmapGray*)drawDevice)->Construct(aSize, aDataStride));
		break;
	case EColor16:
		drawDevice = new(ELeave) CDrawFourBppBitmapColor;
		CleanupStack::PushL(drawDevice);
		User::LeaveIfError(((CDrawFourBppBitmapColor*)drawDevice)->Construct(aSize, aDataStride));
		break;
	case EColor256:
		drawDevice = new(ELeave) CDrawEightBppBitmapColor;
		CleanupStack::PushL(drawDevice);
		User::LeaveIfError(((CDrawEightBppBitmapColor*)drawDevice)->Construct(aSize, aDataStride));
		break;
	case EColor4K:
		drawDevice = new(ELeave) CDrawTwelveBppBitmap;
		CleanupStack::PushL(drawDevice);
		User::LeaveIfError(((CDrawTwelveBppBitmap*)drawDevice)->Construct(aSize, aDataStride));
		break;
	case EColor64K:
		drawDevice = new(ELeave) CDrawSixteenBppBitmap;
		CleanupStack::PushL(drawDevice);
		User::LeaveIfError(((CDrawSixteenBppBitmap*)drawDevice)->Construct(aSize, aDataStride));
		break;
	case EColor16M:
		drawDevice = new(ELeave) CDrawTwentyFourBppBitmap;
		CleanupStack::PushL(drawDevice);
		User::LeaveIfError(((CDrawTwentyFourBppBitmap*)drawDevice)->Construct(aSize, aDataStride));
		break;
	case EColor16MU:
		drawDevice = new(ELeave) CDrawUTwentyFourBppBitmap;
		CleanupStack::PushL(drawDevice);
		User::LeaveIfError(((CDrawUTwentyFourBppBitmap*)drawDevice)->Construct(aSize, aDataStride));
		break;
	case EColor16MA:
		drawDevice = new(ELeave) CDrawThirtyTwoBppBitmapAlpha;
		CleanupStack::PushL(drawDevice);
		User::LeaveIfError(((CDrawThirtyTwoBppBitmapAlpha*)drawDevice)->Construct(aSize, aDataStride));
		break;
	case EColor16MAP:
		drawDevice = new(ELeave) CDrawThirtyTwoBppBitmapAlphaPM;
		CleanupStack::PushL(drawDevice);
		User::LeaveIfError(((CDrawThirtyTwoBppBitmapAlphaPM*)drawDevice)->Construct(aSize, aDataStride));
		break;
	default:
		Panic(EScreenDriverPanicInvalidDisplayMode);
		}
	CleanupStack::Pop(drawDevice); // drawDevice
	return drawDevice;
	}

/**
@deprecated Use NewBitmapDeviceL(const TSize& aSize, TDisplayMode aDispMode, TInt aDataStride)
*/
EXPORT_C CFbsDrawDevice* CFbsDrawDevice::NewBitmapDeviceL(TScreenInfoV01 aInfo,
														  TDisplayMode aDispMode,
														  TInt aDataStride)
	{
	return ::CreateBitmapDeviceL(aInfo.iScreenSize, aDispMode, aDataStride);
	}

/**
Creates a new bitmap device instance, which implements CFbsDrawDevice interface.
@param aSize Bitmap device size
@param aDispMode Requested display mode
@return A pointer to just created bitmap device, which implements CFbsDrawDevice interface
@leave KErrNoMemory Not enough memory
       KErrArgument Invalid aSize value
*/
EXPORT_C CFbsDrawDevice* CFbsDrawDevice::NewBitmapDeviceL(const TSize& aSize,
														  TDisplayMode aDispMode,
														  TInt aDataStride)
	{
	return ::CreateBitmapDeviceL(aSize, aDispMode, aDataStride);
	}

//Logical coordinates will be initialized with the right values, when SetSize() is called.
CDrawBitmap::CDrawBitmap()
	{
	SetDefaults();
	TInt err = GetInterface(KAlphaBlendInterfaceID, reinterpret_cast <TAny*&> (iAlphaBlend));
    //There must be a support for an interface with KAlphaBlendInterfaceID id.
    __ASSERT_ALWAYS(iAlphaBlend && err == KErrNone, User::Invariant());
	}

//Scanline width in pixels.
//The return value can be greater or equal than iSize.iWidth, because
//the scan line memory is allocated in 32-bit words and can be rounded up, if
//the display mode allows more than 1 pixel to be stored in a single byte.
TInt CDrawBitmap::LongWidth() const
	{
	//0 or 180 dgrees
	if(!(iOrientation & 1))
		{
		return iLongWidth;
		}
	//90 or 270 degrees
	return iSize.iWidth == 0 ? 0 : iLongWidth * iSize.iHeight / iSize.iWidth;
	}

TUint32* CDrawBitmap::ScanLineBuffer() const
	{
	return iScanLineBuffer;
	}

//Scanline width in bytes
TInt CDrawBitmap::ScanLineBytes() const
	{
	register TInt scanLineBytes = iScanLineWords << 2;
	//90 or 270 degrees
	if(iOrientation & 1)
		{
		return iSize.iWidth == 0 ? 0 : scanLineBytes * iSize.iHeight / iSize.iWidth;
		}
	//0 or 180 degrees
	return scanLineBytes;
	}

/**
The method returns screen size in pixels. The orientation is taken into account.
Always prefer GetDrawRect() to SizeInPixels() call.
GetDrawRect() will take into account possible non-[0,0] top-left corner of the drawing
rectangle if the device is scaled.
@return TSize Screen size in pixels
*/
TSize CDrawBitmap::SizeInPixels() const
	{
	//90 or 270 degrees
	if(iOrientation & 1)
		{
		return TSize(iDrawRect.Height(), iDrawRect.Width());
		}
	//0 or 180 degrees
	return iDrawRect.Size();
	}

//aPoint - logical coordinates
void CDrawBitmap::SetDitherOrigin(const TPoint& aPoint)
	{
	if (iOrientation&1)
		{
		iDitherOrigin.iX = ::Log2Phys(aPoint.iX,iOrigin.iX,iScalingSettings.iFactorY,iSize.iHeight);
		iDitherOrigin.iY = ::Log2Phys(aPoint.iY,iOrigin.iY,iScalingSettings.iFactorX,iSize.iWidth);
		}
	else
		{
		iDitherOrigin.iX = ::Log2Phys(aPoint.iX,iOrigin.iX,iScalingSettings.iFactorX,iSize.iWidth);
		iDitherOrigin.iY = ::Log2Phys(aPoint.iY,iOrigin.iY,iScalingSettings.iFactorY,iSize.iHeight);
		}
	}

TInt CDrawBitmap::BitsPerPixel(TDisplayMode aDispMode)
	{
	switch(aDispMode)
		{
	case EGray2:
		return 1;
	case EGray4:
		return 2;
	case EGray16:
	case EColor16:
		return 4;
	case EGray256:
	case EColor256:
		return 8;
	case EColor4K:
	case EColor64K:
		return 16;
	case EColor16M:
		return 24;
	case EColor16MU:
	case EColor16MA:
	case EColor16MAP:
		return 32;
	default:
		return 0;
		}
	}

void CDrawBitmap::OrientationsAvailable(TBool aOrientation[4])
	{
	aOrientation[EOrientationNormal] = ETrue;
	aOrientation[EOrientationRotated90] = EFalse;
	aOrientation[EOrientationRotated180] = EFalse;
	aOrientation[EOrientationRotated270] = EFalse;
	}

//Works with logical coordinates
void CDrawBitmap::Clear()
	{
	if(iBits)
		{
		if(iScalingOff && iOriginIsZero)
			{
			Mem::Fill(iBits, iScanLineWords * 4 * iSize.iHeight, 0xFF);
			}
		else
			{
			TShadowMode storedShadowMode = iShadowMode;
			iShadowMode = ENoShadow;
			TRect drawRect;
			GetDrawRect(drawRect);
			WriteRgbMulti(drawRect.iTl.iX, drawRect.iTl.iY, drawRect.Width(), drawRect.Height(), KRgbWhite, CGraphicsContext::EDrawModeWriteAlpha);
			iShadowMode = storedShadowMode;
			}
		}
	}

CDrawBitmap::~CDrawBitmap()
	{
	if(iScanLineBuffer)
		{
		User::Free(iScanLineBuffer);
		}
	}

//Works with logical sizes
//The new device will accept old device scalling&scaling settings
//All graphics contexts, already created by the scaled device, should be
//re-activated calling CFbsBitGc::Activate().
void CDrawBitmap::CopyOldSettings(CFbsDrawDevice* aDrawDevice)
	{
	__ASSERT_DEBUG(aDrawDevice, User::Invariant());
	//Scaling&Origin settings - their values when scaling&origin are off.
	const TPoint KDefaultOrigin(0, 0);
	const TInt KDefaultFactorX = 1, KDefaultFactorY = 1;
	const TInt KDefaultDivisorX = 1, KDefaultDivisorY = 1;
	TPoint origin = KDefaultOrigin;//old device origin
	TInt factorX = KDefaultFactorX, factorY = KDefaultFactorY;//old device - X and Y scaling factors
	TInt divisorX = KDefaultDivisorX, divisorY = KDefaultDivisorY;//old device - X and Y scaling divisors
	//Old device - set default scaling setings.
	MScalingSettings* scalingSettings = NULL;
	TInt err = aDrawDevice->GetInterface(KScalingSettingsInterfaceID, reinterpret_cast <TAny*&> (scalingSettings));
	if(err == KErrNone)
		{//There is a support for the interface with KScalingSettingsInterfaceID id.
		__ASSERT_DEBUG(scalingSettings, User::Invariant());
		scalingSettings->Get(factorX, factorY, divisorX, divisorY);
		(void)scalingSettings->Set(KDefaultFactorX, KDefaultFactorY, KDefaultDivisorX, KDefaultDivisorY);
		}
	//Current device - set default scaling setings.
	if(CanBeScaled())
		{
		(void)Set(KDefaultFactorX, KDefaultFactorY, KDefaultDivisorX, KDefaultDivisorY);
		}
	//Old device - set default origin.
	MDrawDeviceOrigin* originInterface = NULL;
	err = aDrawDevice->GetInterface(KDrawDeviceOriginInterfaceID, reinterpret_cast <TAny*&> (originInterface));
	if(err == KErrNone)
		{//There is a support for the interface with KDrawDeviceOriginInterfaceID id.
		__ASSERT_DEBUG(originInterface, User::Invariant());
		originInterface->Get(origin);
		(void)originInterface->Set(KDefaultOrigin);
		}
	//Current device - set default origin.
	if(CanOriginBeMoved())
		{
		(void)Set(KDefaultOrigin);
		}
	//Copy setigns
	DoCopyOldSettings(aDrawDevice);
	//Old device - restore scaling setings.
	if(scalingSettings)
		{
		(void)scalingSettings->Set(factorX, factorY, divisorX, divisorY);
		}
	//Old device - restore origin.
	if(originInterface)
		{
		(void)originInterface->Set(origin);
		}
	//Set current device scaling&origin settings to be the same as
	//scaling&origin settings of the old device.
	if(CanBeScaled())
		{
		(void)Set(factorX, factorY, divisorX, divisorY);
		}
	if(CanOriginBeMoved())
		{
		(void)Set(origin);
		}
	}

void CDrawBitmap::DoCopyOldSettings(CFbsDrawDevice* aDrawDevice)
	{
	CDrawBitmap* oldDevice = (CDrawBitmap*)aDrawDevice;
	iDitherOrigin = oldDevice->iDitherOrigin;
	iShadowMode = oldDevice->iShadowMode;
	SetOrientation(oldDevice->iOrientation);
	iFadeMapFactor = oldDevice->iFadeMapFactor;
	iFadeMapOffset = oldDevice->iFadeMapOffset;

	TUint32* destRowPtr = iBits;
	TUint32* destRowPtrLimit = iBits;
	TInt destRowPtrInc = iScanLineWords;

	TInt row = 0;
	TInt rowInc = 1;

	if (BitsPerPixel(oldDevice->iDispMode) < BitsPerPixel(iDispMode))
		{
		destRowPtr += (iSize.iHeight - 1) * iScanLineWords;
		destRowPtrInc = -destRowPtrInc;
		destRowPtrLimit -= iScanLineWords;
		row = iSize.iHeight - 1;
		rowInc = -1;
		}
	else
		destRowPtrLimit += iSize.iHeight * iScanLineWords;

	TOrientation oldOrientation=oldDevice->iOrientation;
	oldDevice->SetOrientation(CFbsDrawDevice::EOrientationNormal);
	while (destRowPtr != destRowPtrLimit)
		{
		aDrawDevice->ReadLine(0,row,iSize.iWidth,iScanLineBuffer,iDispMode);
		Mem::Copy(destRowPtr,iScanLineBuffer,iScanLineWords << 2);
		destRowPtr += destRowPtrInc;
		row += rowInc;
		}

	oldDevice->SetOrientation(oldOrientation);
	UpdateRegion(TRect(SizeInPixels()));
	Update();
	}

TUint32 CDrawBitmap::Hash(TUint32 aGray16,TInt aX,TInt aY) const
	{
	if (iDitherOrigin.iX & 1)
		aX++;
	if (iDitherOrigin.iY & 1)
		aY++;
	aX &= 1;
	aY &= 1;
	return ditherlutab[aGray16][aX + (aY << 1)];
	}

//aRect - logical coordinates
void CDrawBitmap::MapColors(const TRect& aRect, const TRgb* aColors,
							TInt aNumPairs, TBool aMapForwards)
	{
	const TRect rect = DeOrientate(aRect);//deorientation and transformation of coordinates.
	//rect - physical coordinates
	__ASSERT_DEBUG(rect.iTl.iX >= 0 && rect.iBr.iX <= iSize.iWidth,Panic(EScreenDriverPanicOutOfBounds));
	__ASSERT_DEBUG(rect.iTl.iY >= 0 && rect.iBr.iY <= iSize.iHeight,Panic(EScreenDriverPanicOutOfBounds));
	__ASSERT_DEBUG(aColors,Panic(EScreenDriverPanicNullPointer));
	__ASSERT_DEBUG(aNumPairs > 0,Panic(EScreenDriverPanicZeroLength));

	TRgb color;

	TInt offset = aMapForwards ? 0 : 1;
	TInt scaleX;
	TInt scaleY;
	if (iOrientation&1)
		{
		scaleX=iScalingSettings.iFactorY;
		scaleY=iScalingSettings.iFactorX;
		}
	else
		{
		scaleX=iScalingSettings.iFactorX;
		scaleY=iScalingSettings.iFactorY;
		}
	for(TInt ycoord = rect.iTl.iY; ycoord < rect.iBr.iY; ycoord+=scaleY)
		{
		for(TInt xcoord = rect.iTl.iX; xcoord < rect.iBr.iX; xcoord+=scaleX)
			{
			color = ReadRgbNormal(xcoord,ycoord);
			for (TInt rgbcount = 0; rgbcount < aNumPairs; rgbcount++)
				{
				if (color == aColors[(rgbcount << 1) + offset])
					{
					WriteRgb(xcoord,ycoord,aColors[(rgbcount << 1) + 1 - offset]);
					break;
					}
				}
			}
		}
	}

//form an int from the end portion of one and the start portion of another
TUint32 CDrawBitmap::PasteInt(TUint32 aFirst,TUint32 aSecond,TInt aOffset) const
	{
	TUint32 mask=0;
	if(aOffset<32) mask=0xffffffff>>aOffset;
	aFirst&=mask;
	aSecond&=~mask;
	return(aFirst|aSecond);
	}

//returns the address of the start of scanline aY
//aY- physical coordinate
TUint32* CDrawBitmap::ScanLine(TInt aY) const
	{
	return iBits + (aY * iScanLineWords);
	}

void CDrawBitmap::SetBits(TAny* aBits)
	{
	iBits = STATIC_CAST(TUint32*,aBits);
	}

TBool CDrawBitmap::SetOrientation(TOrientation aOrientation)
	{
	if (iOrientation == aOrientation)
		return ETrue;

	return EFalse;
	}

void CDrawBitmap::SetFadingParameters(TUint8 aBlackMap,TUint8 aWhiteMap)
	{
	iFadeMapFactor = aWhiteMap - aBlackMap + 1;
	iFadeMapOffset = aBlackMap;
	}

TRgb CDrawBitmap::FadeRgb(TRgb aColor)
	{
	TInt value = aColor.Internal();
	TInt b = (((value & 0x000000ff) * iFadeMapFactor) >> 8)  + iFadeMapOffset;
	TInt g = (((value & 0x0000ff00) * iFadeMapFactor) >> 16) + iFadeMapOffset;
	//the multiplication by iFadeMapFactor can overflow into the sign bit, so we shift down in two steps
	TInt r = ((((value & 0x00ff0000) >> 16) * iFadeMapFactor) >> 8) + iFadeMapOffset;
	TInt a = aColor.Alpha();
  	return TRgb(r,g,b,a);
	}

 TUint32 CDrawBitmap::FadeRgb(TUint32 aColor)
  	{
  	TInt value = aColor;

  	TInt b = (((value & 0x000000ff) * iFadeMapFactor) >> 8)  + iFadeMapOffset;
   	TInt g = (((value & 0x0000ff00) * iFadeMapFactor) >> 16) + iFadeMapOffset;
  	//the multiplication by iFadeMapFactor can overflow into the sign bit, so we shift down in two steps
   	TInt r = ((((value & 0x00ff0000) >> 16) * iFadeMapFactor) >> 8) + iFadeMapOffset;
   	TInt a = aColor >> 24;
   	return (a<<24) | ((r&0xff)<<16) | ((g&0xff)<<8) | (b&0xff);
    }

/**
The overloaded function for FadeRgb(TRgb) which works directly with
the Red, Green and Blue colour components to increase the performance.
@param aRed Red component of colour.
@param aGreen Green component of colour.
@param aBlue Blue component of colour.
*/
void CDrawBitmap::FadeRgb(TInt& aRed, TInt& aGreen, TInt& aBlue)
	{
	aRed = ((aRed * iFadeMapFactor) >> 8)  + iFadeMapOffset;
	aGreen = ((aGreen * iFadeMapFactor) >> 8)  + iFadeMapOffset;
	aBlue = ((aBlue * iFadeMapFactor) >> 8)  + iFadeMapOffset;
	}

TUint8 CDrawBitmap::FadeGray(TInt aGray256)
	{
	return STATIC_CAST(TUint8,((aGray256 * iFadeMapFactor) >> 8) + iFadeMapOffset);
	}

//aX and aY - logical coordinates
//aX and aY - deorientated and transformed to physical coordinates after the call
void CDrawBitmap::DeOrientate(TInt& aX,TInt& aY) const
	{
	register TInt physWidth = iSize.iWidth;
	register TInt physHeight = iSize.iHeight;
	register TInt originX = iOrigin.iX;
	register TInt originY = iOrigin.iY;
	register TInt scalingFactorX = iScalingSettings.iFactorX;
	register TInt scalingFactorY = iScalingSettings.iFactorY;
	if(iOrientation & 0x1)
		{
		aX = ::Log2Phys(aX, originX, scalingFactorY, physHeight);
		aY = ::Log2Phys(aY, originY, scalingFactorX, physWidth);
		}
	else
		{
		aX = ::Log2Phys(aX, originX, scalingFactorX, physWidth);
		aY = ::Log2Phys(aY, originY, scalingFactorY, physHeight);
		}

	//aX and aY - descaled.
	switch(iOrientation)
		{
		case EOrientationNormal:
			{
			return;
			}
		case EOrientationRotated180:
			{
			aX = physWidth - aX - 1;
			aY = physHeight - aY - 1;
			break;
			}
			case EOrientationRotated90:
			{
			TInt temp = physWidth - aY - 1;
			aY = aX;
			aX = temp;
			break;
			}
		default: // EOrientationRotated270
			{
			TInt temp = aY;
			aY = physHeight - aX - 1;
			aX = temp;
			}
		}
	}

//aPoint - logical coordinates
//The method returns TPoint object with deorientated and transformed to physical coordinates.
TPoint CDrawBitmap::DeOrientate(const TPoint& aPoint) const
	{
	register TInt physWidth = iSize.iWidth;
	register TInt physHeight = iSize.iHeight;
	register TInt originX = iOrigin.iX;
	register TInt originY = iOrigin.iY;
	register TInt scalingFactorX = iScalingSettings.iFactorX;
	register TInt scalingFactorY = iScalingSettings.iFactorY;
	TPoint physPt;
	if(iOrientation & 0x1)
		{
		physPt.iX = ::Log2Phys(aPoint.iX, originX, scalingFactorY, physHeight);
		physPt.iY = ::Log2Phys(aPoint.iY, originY, scalingFactorX, physWidth);
		}
	else
		{
		physPt.iX = ::Log2Phys(aPoint.iX, originX, scalingFactorX, physWidth);
		physPt.iY = ::Log2Phys(aPoint.iY, originY, scalingFactorY, physHeight);
		}

	//physPt - descaled
	switch(iOrientation)
		{
		case EOrientationNormal:
			{
			return physPt;
			}
		case EOrientationRotated180:
			{
			return TPoint(physWidth - physPt.iX - 1, physHeight - physPt.iY - 1);
			}
		case EOrientationRotated90:
			{
			return TPoint(physWidth - physPt.iY - 1, physPt.iX);
			}
		// EOrientationRotated270
		default:
			return TPoint(physPt.iY, physHeight - physPt.iX - 1);
		}
	}

//aRect - logical coordinates
//The method returns TRect object with deorientated and transformed to physical coordinates.
TRect CDrawBitmap::DeOrientate(const TRect& aRect) const
	{
	register TInt originX = iOrigin.iX;
	register TInt originY = iOrigin.iY;
	register TInt scalingFactorX = iScalingSettings.iFactorX;
	register TInt scalingFactorY = iScalingSettings.iFactorY;
	register TInt physWidth = iSize.iWidth;
	register TInt physHeight = iSize.iHeight;
	TRect physRect;
	if(iOrientation & 0x1)
		{
		physRect.iTl.iX = ::Log2Phys(aRect.iTl.iX, originX, scalingFactorY, physHeight);
		physRect.iTl.iY = ::Log2Phys(aRect.iTl.iY, originY, scalingFactorX, physWidth);
		physRect.iBr.iX = ::RBtmLog2Phys(aRect.iBr.iX, originX, scalingFactorY, physHeight);
		physRect.iBr.iY = ::RBtmLog2Phys(aRect.iBr.iY, originY, scalingFactorX, physWidth);
		}
	else
		{
		physRect.iTl.iX = ::Log2Phys(aRect.iTl.iX, originX, scalingFactorX, physWidth);
		physRect.iTl.iY = ::Log2Phys(aRect.iTl.iY, originY, scalingFactorY, physHeight);
		physRect.iBr.iX = ::RBtmLog2Phys(aRect.iBr.iX, originX, scalingFactorX, physWidth);
		physRect.iBr.iY = ::RBtmLog2Phys(aRect.iBr.iY, originY, scalingFactorY, physHeight);
		}

	//physRect - descaled
	if(iOrientation == EOrientationNormal)
		{
		return physRect;
		}
	if (iOrientation == EOrientationRotated180)
		{
		return TRect(TPoint(physWidth - physRect.iBr.iX, physHeight - physRect.iBr.iY), physRect.Size());
		}
	TSize altSize(physRect.Height(), physRect.Width());
	TPoint altPoint;
	if (iOrientation == EOrientationRotated90)
		{
		altPoint.SetXY(physWidth - physRect.iBr.iY, physRect.iTl.iX);
		}
	else // EOrientationRotated270
		{
		altPoint.SetXY(physRect.iTl.iY, physHeight - physRect.iBr.iX);
		}
	return TRect(altPoint, altSize);
	}

//aX and aY - logical coordinates
TRgb CDrawBitmap::ReadPixel(TInt aX,TInt aY) const
	{
	DeOrientate(aX, aY);//aX and aY - physical coordinates

	__ASSERT_DEBUG(aX >= 0 && aX < iSize.iWidth,Panic(EScreenDriverPanicOutOfBounds));
	__ASSERT_DEBUG(aY >= 0 && aY < iSize.iHeight,Panic(EScreenDriverPanicOutOfBounds));

	return ReadRgbNormal(aX, aY);
	}

//aX and aY - logical coordinates
void CDrawBitmap::ReadLine(TInt aX,TInt aY,TInt aLength,
						   TAny* aBuffer,TDisplayMode aDispMode) const
	{
	DeOrientate(aX,aY);//aX and aY - physical coordinates

	__ASSERT_DEBUG(aX >= 0 && aX < iSize.iWidth,Panic(EScreenDriverPanicOutOfBounds));
	__ASSERT_DEBUG(aY >= 0 && aY < iSize.iHeight,Panic(EScreenDriverPanicOutOfBounds));
#if defined(_DEBUG)
	switch (iOrientation)
		{
	case EOrientationNormal:
		__ASSERT_DEBUG(aX + aLength <= iLongWidth,Panic(EScreenDriverPanicOutOfBounds));
		break;
	case EOrientationRotated90:
		__ASSERT_DEBUG(aY + aLength <= iSize.iHeight,Panic(EScreenDriverPanicOutOfBounds));
		break;
	case EOrientationRotated180:
		__ASSERT_DEBUG(aX - aLength >= -1,Panic(EScreenDriverPanicOutOfBounds));
		break;
	default: // EOrientationRotated270
		__ASSERT_DEBUG(aY - aLength >= -1,Panic(EScreenDriverPanicOutOfBounds));
		break;
		}
#endif
	__ASSERT_DEBUG(aLength > 0,Panic(EScreenDriverPanicZeroLength));
	__ASSERT_DEBUG(aBuffer,Panic(EScreenDriverPanicNullPointer));

	if (aDispMode == iDispMode)
		{
		ReadLine(aX,aY,aLength,aBuffer);
		return;
		}

	TInt xInc = xIncArray[iOrientation];
	TInt yInc = yIncArray[iOrientation];
	if(iOrientation & 0x1)
		{
		xInc*=iScalingSettings.iFactorY;
		yInc*=iScalingSettings.iFactorX;
		}
	else
		{
		xInc*=iScalingSettings.iFactorX;
		yInc*=iScalingSettings.iFactorY;
		}

	switch (aDispMode)
		{
		case EGray2:
			{
			TUint8* bufferPtr = (TUint8*)aBuffer;

			while (aLength >= 8)
				{
				bufferPtr[0] = TUint8(ReadRgbNormal(aX,aY)._Gray2());
				aX += xInc; aY += yInc;
				bufferPtr[0] |= TUint8(ReadRgbNormal(aX,aY)._Gray2() << 1);
				aX += xInc; aY += yInc;
				bufferPtr[0] |= TUint8(ReadRgbNormal(aX,aY)._Gray2() << 2);
				aX += xInc; aY += yInc;
				bufferPtr[0] |= TUint8(ReadRgbNormal(aX,aY)._Gray2() << 3);
				aX += xInc; aY += yInc;
				bufferPtr[0] |= TUint8(ReadRgbNormal(aX,aY)._Gray2() << 4);
				aX += xInc; aY += yInc;
				bufferPtr[0] |= TUint8(ReadRgbNormal(aX,aY)._Gray2() << 5);
				aX += xInc; aY += yInc;
				bufferPtr[0] |= TUint8(ReadRgbNormal(aX,aY)._Gray2() << 6);
				aX += xInc; aY += yInc;
				bufferPtr[0] |= TUint8(ReadRgbNormal(aX,aY)._Gray2() << 7);
				aX += xInc; aY += yInc;
				bufferPtr++;
				aLength -= 8;
				}
			TInt bitShift = 0;
			TInt bitLimit = aLength;
			if (bitShift < bitLimit)
				bufferPtr[0] = 0;
			while (bitShift < bitLimit)
				{
				bufferPtr[0] |= TUint8(ReadRgbNormal(aX,aY)._Gray2() << bitShift);
				aX += xInc; aY += yInc;
				bitShift++;
				}
			}
			break;
		case EGray4:
			{
			TUint8* bufferPtr = REINTERPRET_CAST(TUint8*,aBuffer);

			while (aLength > 3)
				{
				*bufferPtr = TUint8(ReadRgbNormal(aX,aY)._Gray4());
				aX += xInc; aY += yInc;
				*bufferPtr |= TUint8(ReadRgbNormal(aX,aY)._Gray4() << 2);
				aX += xInc; aY += yInc;
				*bufferPtr |= TUint8(ReadRgbNormal(aX,aY)._Gray4() << 4);
				aX += xInc; aY += yInc;
				*bufferPtr++ |= TUint8(ReadRgbNormal(aX,aY)._Gray4() << 6);
				aX += xInc; aY += yInc;
				aLength -= 4;
				}
			if (aLength > 0)
				{
				*bufferPtr = TUint8(ReadRgbNormal(aX,aY)._Gray4());
				aX += xInc; aY += yInc;
				aLength--;
				}
			if (aLength > 0)
				{
				*bufferPtr |= TUint8(ReadRgbNormal(aX,aY)._Gray4() << 2);
				aX += xInc; aY += yInc;
				aLength--;
				}
			if (aLength > 0)
				*bufferPtr |= TUint8(ReadRgbNormal(aX,aY)._Gray4() << 4);
			}
			break;
		case EGray16:
			{
			TUint8* bufferPtr = (TUint8*)aBuffer;

			while (aLength > 1)
				{
				*bufferPtr = TUint8(ReadRgbNormal(aX,aY)._Gray16());
				aX += xInc; aY += yInc;
				*bufferPtr++ |= TUint8(ReadRgbNormal(aX,aY)._Gray16() << 4);
				aX += xInc; aY += yInc;
				aLength -= 2;
				}
			if (aLength > 0)
				*bufferPtr = TUint8(ReadRgbNormal(aX,aY)._Gray16());
			}
			break;
		case EGray256:
			{
			TUint8* bufferPtr = (TUint8*)aBuffer;
			const TUint8* bufferPtrLimit = bufferPtr + aLength;

			while (bufferPtr < bufferPtrLimit)
				{
				*bufferPtr++ = TUint8(ReadRgbNormal(aX,aY)._Gray256());
				aX += xInc; aY += yInc;
				}
			}
			break;
		case EColor16:
			{
			TUint8* bufferPtr = (TUint8*)aBuffer;

			while (aLength > 1)
				{
				*bufferPtr = TUint8(ReadRgbNormal(aX,aY).Color16());
				aX += xInc; aY += yInc;
				*bufferPtr++ |= TUint8(ReadRgbNormal(aX,aY).Color16() << 4);
				aX += xInc; aY += yInc;
				aLength -= 2;
				}
			if (aLength > 0)
				*bufferPtr = TUint8(ReadRgbNormal(aX,aY).Color16());
			}
			break;
		case EColor256:
			{
			TUint8* bufferPtr = (TUint8*)aBuffer;
			const TUint8* bufferPtrLimit = bufferPtr + aLength;

			while (bufferPtr < bufferPtrLimit)
				{
				*bufferPtr++ = TUint8(ReadRgbNormal(aX,aY).Color256());
				aX += xInc; aY += yInc;
				}
			}
			break;
		case EColor4K:
			{
			TUint16* bufferPtr = (TUint16*)aBuffer;
			const TUint16* bufferPtrLimit = bufferPtr + aLength;

			while (bufferPtr < bufferPtrLimit)
				{
				*bufferPtr++ = TUint16(ReadRgbNormal(aX,aY)._Color4K());
				aX += xInc; aY += yInc;
				}
			}
			break;
		case EColor64K:
			{
			TUint16* bufferPtr = (TUint16*)aBuffer;
			const TUint16* bufferPtrLimit = bufferPtr + aLength;

			while (bufferPtr < bufferPtrLimit)
				{
				*bufferPtr++ = TUint16(ReadRgbNormal(aX,aY)._Color64K());
				aX += xInc; aY += yInc;
				}
			}
			break;
		case EColor16M:
			{
			TUint8* bufferPtr = (TUint8*)aBuffer;
			const TUint8* bufferPtrLimit = bufferPtr + (aLength * 3);

			while (bufferPtr < bufferPtrLimit)
				{
				TUint32 pixelColorValue = ReadRgbNormal(aX,aY).Internal();
				aX += xInc; aY += yInc;
				bufferPtr[0] = TUint8(pixelColorValue);
				bufferPtr[1] = TUint8(pixelColorValue >> 8);
				bufferPtr[2] = TUint8(pixelColorValue >> 16);
				bufferPtr += 3;
				}
			}
			break;
		case ERgb:
			{
			TRgb* bufferPtr = (TRgb*)aBuffer;
			const TRgb* bufferPtrLimit = bufferPtr + aLength;

			while (bufferPtr < bufferPtrLimit)
				{
				*bufferPtr++ = ReadRgbNormal(aX,aY);
				aX += xInc; aY += yInc;
				}
			}
			break;
		case EColor16MU:
			{
			TUint32* bufferPtr = (TUint32*)aBuffer;
			const TUint32* bufferPtrLimit = bufferPtr + aLength;

			while (bufferPtr < bufferPtrLimit)
				{
				*bufferPtr++ = ReadRgbNormal(aX,aY)._Color16MU();//BGRA (Blue/Green/Red/Alpha) as little endian bite order
				aX += xInc; aY += yInc;
				}
			}
			break;
		case EColor16MA:
			{
			TUint32* bufferPtr = (TUint32*)aBuffer;
			const TUint32* bufferPtrLimit = bufferPtr + aLength;

			while (bufferPtr < bufferPtrLimit)
				{
				*bufferPtr++ = ReadRgbNormal(aX,aY)._Color16MA();//BGRA (Blue/Green/Red/Alpha) as little endian bite order
				aX += xInc; aY += yInc;
				}
			}
			break;
		case EColor16MAP:
			{
			TUint32* bufferPtr = (TUint32*)aBuffer;
			const TUint32* bufferPtrLimit = bufferPtr + aLength;

			while (bufferPtr < bufferPtrLimit)
				{
				*bufferPtr++ = ReadRgbNormal(aX,aY)._Color16MAP();;
				aX += xInc; aY += yInc;
				}
			}
			break;
		default:
			Panic(EScreenDriverPanicInvalidDisplayMode);
		}
	}

//aX, aY - logical coordinates
void CDrawBitmap::WriteRgb(TInt aX,TInt aY,TRgb aColor,CGraphicsContext::TDrawMode aDrawMode)
	{
	register TInt width = -1;
	register TInt height = -1;
	PreWriteRgb(width, height, aX, aY, aDrawMode);
	WriteRgb(width, height, aX, aY, aColor, aDrawMode);
	}

//aX, aY - logical coordinates
void CDrawBitmap::WriteRgbMulti(TInt aX,TInt aY,TInt aLength,TInt aHeight,
								TRgb aColor,CGraphicsContext::TDrawMode aDrawMode)
	{
	const TRect rect = DeOrientate(TRect(aX,aY,aX + aLength,aY + aHeight));//rect - physical coordinates
	aX = rect.iTl.iX;
	aY = rect.iTl.iY;
	aLength = rect.Width();
	aHeight = rect.Height();

	__ASSERT_DEBUG(aX>=0 && aX+aLength<=iSize.iWidth,Panic(EScreenDriverPanicOutOfBounds));
	__ASSERT_DEBUG(aY>=0 && aY+aHeight<=iSize.iHeight,Panic(EScreenDriverPanicOutOfBounds));

	MapColorToUserDisplayMode(aColor);
	if(iShadowMode)
		{
		Shadow(aColor);
		}
	if(aDrawMode&CGraphicsContext::EInvertPen)
		{
		aColor=~aColor;
		}
	if(aDrawMode&CGraphicsContext::EPenmode)
		{
		BlendRgbMulti(aX,aY,aLength,aHeight,aColor);
		return;
		}
	if(aDrawMode&CGraphicsContext::EWriteAlpha)
		{
		WriteRgbMulti(aX,aY,aLength,aHeight,aColor);
		return;
		}
	if(aDrawMode&CGraphicsContext::EInvertScreen)
		{
		WriteRgbMultiXOR(aX,aY,aLength,aHeight,KRgbWhite);
		}
	if(aDrawMode&CGraphicsContext::EXor)
		{
		WriteRgbMultiXOR(aX,aY,aLength,aHeight,aColor);
		}
	else if(aDrawMode&CGraphicsContext::EAnd)
		{
		WriteRgbMultiAND(aX,aY,aLength,aHeight,aColor);
		}
	else if(aDrawMode&CGraphicsContext::EOr)
		{
		WriteRgbMultiOR(aX,aY,aLength,aHeight,aColor);
		}
	}

//aX, aY - logical coordinates
void CDrawBitmap::WriteBinary(TInt aX,TInt aY,TUint32* aBuffer,TInt aLength,
							  TInt aHeight,TRgb aColor,CGraphicsContext::TDrawMode aDrawMode)
	{
	TRect drawRect;
	GetDrawRect(drawRect);
	__ASSERT_DEBUG(aX >= drawRect.iTl.iX && (aX + aLength) <= drawRect.iBr.iX, Panic(EScreenDriverPanicOutOfBounds));
	__ASSERT_DEBUG(aY >= drawRect.iTl.iY && (aY + aHeight) <= drawRect.iBr.iY, Panic(EScreenDriverPanicOutOfBounds));
	__ASSERT_DEBUG(aBuffer, Panic(EScreenDriverPanicNullPointer));
	__ASSERT_DEBUG(aLength > 0, Panic(EScreenDriverPanicZeroLength));
	__ASSERT_DEBUG(aLength <= 32, Panic(EScreenDriverPanicOutOfBounds));

	MapColorToUserDisplayMode(aColor);
	if(iShadowMode)
		Shadow(aColor);
	if(aDrawMode&CGraphicsContext::EInvertPen)
		aColor=~aColor;
	if(aDrawMode&CGraphicsContext::EPenmode)
		{
		WriteBinary(aX,aY,aBuffer,aLength,aHeight,aColor);
		return;
		}
	if(aDrawMode&CGraphicsContext::EInvertScreen)
		WriteBinaryOp(aX,aY,aBuffer,aLength,aHeight,KRgbWhite,CGraphicsContext::EDrawModeXOR);
	if(aDrawMode&CGraphicsContext::ELogicalOp)
		WriteBinaryOp(aX,aY,aBuffer,aLength,aHeight,aColor,(CGraphicsContext::TDrawMode)(aDrawMode&CGraphicsContext::ELogicalOp));
	}

//aX, aY - logical coordinates
void CDrawBitmap::WriteBinaryLine(TInt aX,TInt aY,TUint32* aBuffer,
								  TInt aLength,TRgb aColor,
								  CGraphicsContext::TDrawMode aDrawMode)
	{
	MapColorToUserDisplayMode(aColor);
	while(aLength>32)
		{
		WriteBinary(aX,aY,aBuffer,32,1,aColor,aDrawMode);
		aX+=32;
		aBuffer++;
		aLength-=32;
		}
	WriteBinary(aX,aY,aBuffer,aLength,1,aColor,aDrawMode);
	}

//aX, aY - logical coordinates
void CDrawBitmap::WriteBinaryLineVertical(TInt aX,TInt aY,TUint32* aBuffer,
										  TInt aHeight,TRgb aColor,
										  CGraphicsContext::TDrawMode aDrawMode,TBool aUp)
	{
	TRect drawRect;
	GetDrawRect(drawRect);
	__ASSERT_DEBUG(aX >= drawRect.iTl.iX && aX < drawRect.iBr.iX, Panic(EScreenDriverPanicOutOfBounds));
	__ASSERT_DEBUG(aY >= drawRect.iTl.iY, Panic(EScreenDriverPanicOutOfBounds));
	__ASSERT_DEBUG(aUp || (aY + aHeight) <= drawRect.iBr.iY, Panic(EScreenDriverPanicOutOfBounds));
	__ASSERT_DEBUG(!aUp || (aY - aHeight + 1) >= drawRect.iTl.iY, Panic(EScreenDriverPanicOutOfBounds));
	__ASSERT_DEBUG(aBuffer, Panic(EScreenDriverPanicNullPointer));
	__ASSERT_DEBUG(aHeight > 0, Panic(EScreenDriverPanicZeroLength));

	MapColorToUserDisplayMode(aColor);
	if((aDrawMode & CGraphicsContext::EPenmode) && iScalingOff)
		{
		if(iShadowMode)
			Shadow(aColor);
		if(aDrawMode&CGraphicsContext::EInvertPen)
			aColor=~aColor;
		WriteBinaryLineVertical(aX,aY,aBuffer,aHeight,aColor,aUp);
		return;
		}

	TUint32 mask = 1;
	TUint32 data = *aBuffer++;
	TInt endrow = aY + (aUp ? -aHeight : aHeight);
	TInt rowInc = aUp ? -1 : 1;

	while (aY != endrow)
		{
		if (!mask)
			{
			data = *aBuffer++;
			mask = 1;
			}

		if (data & mask)
			{
			WriteRgb(aX, aY, aColor, aDrawMode);
			}

		aY += rowInc;
		mask <<= 1;
		}
	}

//aX, aY - logical coordinates
void CDrawBitmap::WriteLine(TInt aX,TInt aY,TInt aLength,TUint32* aBuffer,
							CGraphicsContext::TDrawMode aDrawMode)
	{
	const TPoint originalPoint(aX,aY);
	DeOrientate(aX,aY);//aX and aY - physical coordinates

	__ASSERT_DEBUG(aX >= 0,Panic(EScreenDriverPanicOutOfBounds));
	__ASSERT_DEBUG(aY >= 0,Panic(EScreenDriverPanicOutOfBounds));
#if defined(_DEBUG)
	switch (iOrientation)
		{
	case EOrientationNormal:
		__ASSERT_DEBUG(aX + aLength <= iSize.iWidth,Panic(EScreenDriverPanicOutOfBounds));
		break;
	case EOrientationRotated90:
		__ASSERT_DEBUG(aY + aLength <= iSize.iHeight,Panic(EScreenDriverPanicOutOfBounds));
		break;
	case EOrientationRotated180:
		__ASSERT_DEBUG(aX - aLength >= -1,Panic(EScreenDriverPanicOutOfBounds));
		break;
	default: // EOrientationRotated270
		__ASSERT_DEBUG(aY - aLength >= -1,Panic(EScreenDriverPanicOutOfBounds));
		break;
		}
#endif
	__ASSERT_DEBUG(aLength > 0,Panic(EScreenDriverPanicZeroLength));
	__ASSERT_DEBUG(aBuffer,Panic(EScreenDriverPanicNullPointer));

	MapBufferToUserDisplayMode(aLength,aBuffer);
	if(iShadowMode)
		{
		ShadowBuffer(aLength,aBuffer);
		}
	if(aDrawMode&CGraphicsContext::EInvertPen)
		{
		InvertBuffer(aLength,aBuffer);
		}
	if(aDrawMode&CGraphicsContext::EPenmode)
		{
		BlendLine(aX,aY,aLength,aBuffer);
		return;
		}
	if(aDrawMode&CGraphicsContext::EWriteAlpha)
		{
		WriteLine(aX,aY,aLength,aBuffer);
		return;
		}
	if(aDrawMode&CGraphicsContext::EInvertScreen)
		{
		const TRect rect = DeOrientate(TRect(originalPoint,TSize(aLength,1)));//"rect" - deorientated and scaled
		WriteRgbMultiXOR(rect.iTl.iX,rect.iTl.iY,rect.Width(),rect.Height(),KRgbWhite);
		}
	if(aDrawMode&CGraphicsContext::EXor)
		{
		WriteLineXOR(aX,aY,aLength,aBuffer);
		}
	else if(aDrawMode&CGraphicsContext::EAnd)
		{
		WriteLineAND(aX,aY,aLength,aBuffer);
		}
	else if(aDrawMode&CGraphicsContext::EOr)
		{
		WriteLineOR(aX,aY,aLength,aBuffer);
		}
	}

TAny* CDrawBitmap::CopyOffset(TAny* aDestination,const TAny* aSource,TInt aWordsToCopy,TInt aSourceBitOffset)
	{
	ASSERT(aSourceBitOffset > 0 && aSourceBitOffset < 32);

	const TUint32* srcPtr = REINTERPRET_CAST(const TUint32*,aSource);
	TUint32* destPtr = REINTERPRET_CAST(TUint32*,aDestination);
	const TUint32* destPtrLimit = destPtr + aWordsToCopy;
	const TInt sourceBitOffsetComplement = 32 - aSourceBitOffset;

	TUint32 sourceValue = *srcPtr++;

	while (destPtr < destPtrLimit)
		{
		TUint32 destValue = sourceValue >> aSourceBitOffset;
		sourceValue = *srcPtr++;
		destValue |= sourceValue << sourceBitOffsetComplement;
		*destPtr++ = destValue;
		}
	return destPtr;
	}
/**	Common code to read a line of pixels where there are >1 pixels per byte.
  	The source words must be shifted to fit the target buffer.
@param 	aPixelPtr 	Source location to start copying from (word aligned, last word is safe)
@param	aBufferPtr	Target location to write to (word aligned - may or may not own all last word)
@param	aWordsCnt	Number of source words that can be safely copied
@param	aRestPixels	Number of pixels that must be read from the next word for the final byte copy
@param	aBytesCount	Number of bytes to write from final input word
@param	aShiftBits	Number of bits shifted between input and output words.
 **/
void CDrawBitmap::ReadLineCommon(TUint32* aPixelPtr,TUint32* aBufferPtr,TInt aWordsCount,TInt aRestPixels,TInt aBytesCount,TInt aShiftBits)
	{
// 	As many pixels as possible are copied by shifting whole words.
// 	This involves reading two source words, shifting them, 
// 	and merging the result to one target word.
//  	However, two cases mean the last few pixels need to be treated carefully:
//  	1) The target buffer may be a number of bytes, not whole words.
//			The number of pixels to copy defines the number of bytes to copy.
//  	2) The number of pixels to read may mean the last read does not need 
//  	   	to read a second word in order to satisfy the number of pixels requested.
//  	   	This next word may not be mapped, so must not be read!

	//Are we willing to pay for these asserts for every scanline copy, even in debug?
	__ASSERT_DEBUG(aPixelPtr, User::Invariant());
	__ASSERT_DEBUG(aBufferPtr, User::Invariant());
	__ASSERT_DEBUG(aWordsCount>=0, User::Invariant());
	__ASSERT_DEBUG(aBytesCount<5, User::Invariant());
	__ASSERT_DEBUG(aShiftBits>=0 && aShiftBits<33, User::Invariant());
	
	TUint32 nextpixel;
	if(aWordsCount>0)	
		{
		if (aShiftBits==0)
			{
			while (aWordsCount--)
				{
				*aBufferPtr++ = *aPixelPtr++;
				}
			if (aBytesCount>0)	//I hope the optimiser can concatenate these two tests?
				{
				nextpixel=aPixelPtr[0];
				}
			}
		else
			{
			const TInt shiftBitsExtra = 32 - aShiftBits;
			nextpixel=aPixelPtr[0]; 
			while (aWordsCount--)
				{
				TUint32 prevpixel=nextpixel;
				nextpixel=aPixelPtr[1];	//this must not read forward when it doesn't need to
				aBufferPtr[0] = (prevpixel >> aShiftBits) | (nextpixel << shiftBitsExtra);

				aPixelPtr++;
				aBufferPtr++;
				}
			}
		}
	else
		{
		nextpixel=aPixelPtr[0];
		}
	
	//deal with the trailing bytes
	if (aBytesCount>0)
		{
		if (aBytesCount==4)
			{
			//The client only requests 4 bytes rather than 1 more word when the requested pixels
			//mean the second word should not be read from.
			//Can also write the result in a single operation!
			aBufferPtr[0]=nextpixel>> aShiftBits;
			}
		else
			{
			if (aRestPixels>0)
				{	
				//do need to read from next word to fill all the requested pixels.
				aWordsCount=(nextpixel >> aShiftBits) | (aPixelPtr[1] << (32 - aShiftBits));
				}
			else
				{	
				//Don't read second word, otherwise might read past end of picture data!
				aWordsCount=(nextpixel >> aShiftBits);	
				}
			TUint8*	bufferPtrChar=reinterpret_cast <TUint8*> (aBufferPtr);

			//max 3 bytes to store
			if (aBytesCount&2)
				{
				bufferPtrChar[0]=aWordsCount;
				bufferPtrChar[1]=aWordsCount>>8;
				bufferPtrChar+=2;
				aWordsCount>>=16;
				}
			if (aBytesCount&1)
				{
				bufferPtrChar[0]=aWordsCount;
				}
			}
		}
	}

/**
The method performs an alpha blending of the source data - aRgbBuffer and screen pixels, using
the data from aMaskBuffer buffer as an alpha blending factor.
If the shadowing/fading flag is set, a shadow/fade copy of the source bitmap will be used.
The formula used for that, is:
(C1 * A + C2 * (255 - A)) / 255, where:
- C1 - a pixel from aRgbBuffer;
- C2 - a pixel from the sceen;
- A  - a pixel from aMaskBuffer;
The content of source and mask buffers is preserved.
The calculated alpha blended pixel is written to the destination - the screen or a bitmap.
@param aX Logical X coordinate of the position in the target the result should be drawn to.
@param aY Logical Y coordinate of the position in the target the result should be drawn to.
@param aLength Source data - length in pixels.
@param aRgbBuffer A pointer to a line of the source bitmap data.
@param aMaskBuffer Buffer containing the data which should be used as an
                   alpha blending factor.
*/
void CDrawBitmap::WriteRgbAlphaLine(TInt aX, TInt aY, TInt aLength,
                                    TUint8* aRgbBuffer, TUint8* aMaskBuffer,
                                    CGraphicsContext::TDrawMode aDrawMode)
	{
	iAlphaBlend->WriteRgbAlphaLine(aX, aY, aLength, aRgbBuffer, aMaskBuffer,
                                   MAlphaBlend::EShdwBefore,
                                   aDrawMode);
	}

/**
The method performs an alpha blending of the source data - aRgbBuffer1 and aBuffer2, using
the data from aMaskBuffer buffer as an alpha blending factor.
If the shadowing/fading flag is set, the resulting pixels will be shadowed/faded.
The formula used for that, is:
(C1 * A + C2 * (255 - A)) / 255, where:
- C1 - a pixel from aRgbBuffer1;
- C2 - a pixel from aBuffer2;
- A  - a pixel from aMaskBuffer;
The content of source and mask buffers is preserved.
The calculated alpha blended pixel is written to the destination - the screen or a bitmap.
@param aX Logical X coordinate of the position in the target the result should be drawn to.
@param aY Logical Y coordinate of the position in the target the result should be drawn to.
@param aLength Source data - length in pixels.
@param aRgbBuffer1 A pointer to a line of the source bitmap data 1.
@param aBuffer2 A pointer to a line of the source bitmap data 2.
                Source bitmap data 2 should be mapped to current display mode
				before the method call.
@param aMaskBuffer Buffer containing the data which should be used as an
                   alpha blending factor.
@param aDrawMode Drawing mode
*/
void CDrawBitmap::WriteRgbAlphaLine(TInt aX,TInt aY,TInt aLength,
									const TUint8* aRgbBuffer1,
									const TUint8* aBuffer2,
									const TUint8* aMaskBuffer,
									CGraphicsContext::TDrawMode aDrawMode)
	{
	// Save current shadow mode
	TShadowMode temp_mode = iShadowMode;
	iShadowMode = ENoShadow;
	// copy the source data 2 to the screen/bitmap target buffer
	WriteLine(aX, aY, aLength, (TUint32*)aBuffer2, aDrawMode);
	// restore current shadow mode
	iShadowMode = temp_mode;
	// DrawModePen is the only supported operation for blending masks. 
	iAlphaBlend->WriteRgbAlphaLine(aX, aY, aLength, aRgbBuffer1, aMaskBuffer,
                                   MAlphaBlend::EShdwAfter, CGraphicsContext::EDrawModePEN);
	}

//Initializes iSize and iDrawRect data members.
//It should be called every time when iSize is going to be changed - Construct() implementations
//in derived classes.
//The method does not use iOrientation data member.
//@param aSize Physical screen size in pixels.
//@panic EScreenDriverPanicInvalidSize - Invalid aSize parameter. This might happen if the
//device is scaled and the scaling origin goes outside physical drawing rectangle.
void CDrawBitmap::SetSize(const TSize& aSize)
	{
	iSize = aSize;
	InitLogicalCoordinates();
	}

/**
The method swaps bitmap device's width and height.
For example: if the size is (40, 20), the swapped size will be (20, 40).
The device's content is not preserved.
The method leaves CDrawBitmap object in a consistent state -
scaling settings will be set with their default values (the scaling is switched off),
iDitherOrigin will be set to (0,0), iOrigin to (0,0).

Note: This method is used internally by BITGDI component. Do not call it!
*/
void CDrawBitmap::SwapWidthAndHeight()
	{
	SetDefaults();
	//Swap width and height
	TSize swappedSize(iSize.iHeight, iSize.iWidth);
	//Initialize iSize, iScanLineWords, iLongWidth data members.
	SetSize(swappedSize);
	}

//This method initializes some of the data members.
//Espetially it switches scaling off, sets iDitherOrigin to (0,0),
//iOrigin to (0,0), iDrawRect to (0,0,0,0). iSize value is preserved.
//Do not forget to update it if adding new data members!
void CDrawBitmap::SetDefaults()
	{
	iLongWidth = 0;
	iDitherOrigin.SetXY(0, 0);
	iFadeMapFactor = 128;
	iFadeMapOffset = 128;
	iScalingSettings = TScalingSettings();
	iOrigin.SetXY(0, 0);
	iScalingOff = ETrue;
	iOriginIsZero = ETrue;
	iDrawRect.SetRect(0, 0, 0, 0);
	}

/**
Implementation for CFbsDrawDevice::GetInterface().
Retrieves a pointer to a specified interface of CFbsDrawDevice implementation.
@param aInterfaceId Interface identifier of the interface to be retrieved.
@param aInterface Address of variable that retrieves the specified interface.
@return KErrNone If the interface is supported, KErrNotSupported otherwise.
*/
TInt CDrawBitmap::GetInterface(TInt aInterfaceId, TAny*& aInterface)
	{
	aInterface = NULL;
	TInt err = KErrNotSupported;

	switch (aInterfaceId)
		{
		case KScalingSettingsInterfaceID:
			{
			if(CanBeScaled())
				{
				aInterface = static_cast <MScalingSettings*> (this);
				err = KErrNone;
				}
			break;
			}
		case KDrawDeviceOriginInterfaceID:
			{
			if(CanOriginBeMoved())
				{
				aInterface = static_cast <MDrawDeviceOrigin*> (this);
				err = KErrNone;
				}
			break;
			}
		case KAlphaBlendInterfaceID:
			{
			aInterface = static_cast <MAlphaBlend*> (this);
			err = KErrNone;
			break;
			}
		case KOrientationInterfaceID:
			{
			aInterface = static_cast <MDrawDeviceOrientation*> (this);
			err = KErrNone;    		
			break;
			}
		case KOutlineAndShadowInterfaceID:
			{
			aInterface = static_cast <MOutlineAndShadowBlend*> (this);
			err = KErrNone;    		
			break;
			}
		case KFastBlendInterfaceID:
			{
			aInterface = static_cast <MFastBlend*> (this);
			err = KErrNone;
			break;
			}
		}
	
	return err;
	}

void CDrawBitmap::BlendRgbMulti(TInt aX,TInt aY,TInt aLength,TInt aHeight,TRgb aColor)
	{
	WriteRgbMulti(aX,aY,aLength,aHeight,aColor);
	}

void CDrawBitmap::BlendLine(TInt aX,TInt aY,TInt aLength,TUint32* aBuffer)
	{
	WriteLine(aX, aY,aLength, aBuffer);
	}

/**
Convert a RGB pixel into the color associated to the user display mode.
@param aRed red color
@param aGreen green color
@param aBlue blue color
@internalComponent
*/
void CDrawBitmap::MapColorToUserDisplayMode(TInt& aRed,TInt& aGreen,TInt& aBlue)
	{
	TUint32 tmpValue;
		
	switch (iUserDispMode)
		{
		case EGray2:
			tmpValue = ((aRed<<1) + aGreen + (aGreen<<2) + aBlue) >> 10;
			if (tmpValue) 	{ aRed = 0xff; aBlue = 0xff; aGreen = 0xff; }
			else			{ aRed = 0x00; aBlue = 0x00; aGreen = 0x00; }
			break;
		case EGray4:
			tmpValue = ((aRed<<1) + aGreen + (aGreen<<2) + aBlue) >> 9;
			tmpValue = tmpValue | (tmpValue << 2) | (tmpValue << 4) | (tmpValue << 6);
			aRed = tmpValue; aGreen = tmpValue; aBlue = tmpValue;
			break;
		case EGray16:
			tmpValue = ((aRed<<1) + aGreen + (aGreen<<2) + aBlue) >> 7;
			tmpValue = tmpValue | (tmpValue << 4);
			aRed = tmpValue; aGreen = tmpValue; aBlue = tmpValue;
			break;
	  	case EGray256:
			tmpValue = ((aRed<<1) + aGreen + (aGreen<<2) + aBlue) >> 3;
	  		aRed = tmpValue; aGreen = tmpValue; aBlue = tmpValue;
	  		break;
	  	case EColor16:
	  		{
	  		TRgb aColor(aRed,aGreen,aBlue);
	  		aColor = TRgb::Color16(aColor.Color16());
	  		aRed = aColor.Red(); aGreen = aColor.Green(); aBlue = aColor.Blue();
	  		break;  		
	  		}
	  	case EColor256:
	  		{
	  		TRgb aColor2(aRed,aGreen,aBlue);
	  		aColor2 = TRgb::Color256(aColor2.Color256());
	  		aRed = aColor2.Red(); aGreen = aColor2.Green(); aBlue = aColor2.Blue();
	  		break;
	  		}
	  	case EColor4K:
	  		aBlue = aBlue  & 0xf0; 	aBlue |= (aBlue >> 4);
	  		aGreen = aGreen  & 0xf0;	aGreen |= (aGreen >> 4);
	  		aRed = aRed  & 0xf0; 		aRed |= (aRed >> 4);
	  		break;
	  	case EColor64K:
	  		aBlue = aBlue  & 0xf8;	aBlue += (aBlue >> 5);
	  		aGreen = aGreen  & 0xfc;	aGreen += (aGreen >> 6);
	  		aRed = aRed  & 0xf8;		aRed += (aRed >> 5);
	  		break;
	  	default:
	  		break;
  		}
	}

/**
CDrawBitmap::Orientation() implementation.
@internalTechnology
@see MDrawDeviceOrientation::Orientation()
*/
CFbsDrawDevice::TOrientation CDrawBitmap::Orientation()
	{
	return iOrientation;
	}

#ifdef __ARMCC__
#ifndef __MARM_THUMB__
#define USE_SCREENDRIVER_ARM_ASM
#endif //__MARM_THUMB__
#endif //__ARMCC__

#ifdef USE_SCREENDRIVER_ARM_ASM
/**
MemFill - using an unrolled loop to perform the following:
	for (TUint32* tempWordPtr = wordPtr; tempWordPtr < wordPtrLimit; tempWordPtr++)
		{
		*tempWordPtr = colorWord;
		}
*/
__asm void MemFillTUint32(TUint32* /*aTrg*/, TInt /*aLength*/, const TUint32 /*aValue*/)
	{
	//r0 - aTrg, r1 - aLength, r2 - aValue
	push     {r1,r3,lr}
	and      r3,r1,#7
	cmp      r3,#7
	addls    pc,pc,r3,LSL #2
	b        mf_switch0
	b        mf_switch0
	b        mf_switch1
	b        mf_switch2
	b        mf_switch3
	b        mf_switch4
	b        mf_switch5
	b        mf_switch6
	b        mf_switch7
mf_switch7
	str      r2,[r0],#4
mf_switch6
	str      r2,[r0],#4
mf_switch5
	str      r2,[r0],#4
mf_switch4
	str      r2,[r0],#4
mf_switch3
	str      r2,[r0],#4
mf_switch2
	str      r2,[r0],#4
mf_switch1
	str      r2,[r0],#4
mf_switch0

	asr      r1,r1,#3
	cmp      r1,#0
	beq      mf_complete

	push     {r0,r2,r4-r8}
	mov      r3,r2
	mov      r4,r2
    mov      r5,r2
    mov      r6,r2
	mov      r7,r2
	mov      r8,r2
	mov      lr,r2

mf_more
	stmia    r0!,{r2-r8,lr}
	subs     r1,r1,#1
	bgt      mf_more
	pop     {r0,r2,r4-r8}

mf_complete
	pop     {r1,r3,pc}
	}

#else //USE_SCREENDRIVER_ARM_ASM

const TInt KDevideByEightShift = 3;
const TInt KModulusByEightFlag = 7;

void MemFillTUint32(TUint32* tempWordPtr, TInt aCount,  const TUint32 aValue)
	{
	TInt remainder = aCount & KModulusByEightFlag;
	switch (remainder)
		{
	case 7:
		*tempWordPtr++ = aValue;
	case 6:
		*tempWordPtr++ = aValue;
	case 5:
		*tempWordPtr++ = aValue;
	case 4:
		*tempWordPtr++ = aValue;
	case 3:
		*tempWordPtr++ = aValue;
	case 2:
		*tempWordPtr++ = aValue;
	case 1:
		*tempWordPtr++ = aValue;
		}
	register TUint32 value0 = aValue;
	register TUint32 value1 = aValue;
	register TUint32 value2 = aValue;
	for(TInt times = (aCount >> KDevideByEightShift); times > 0; --times)
		{
		*tempWordPtr++ = value0;
		*tempWordPtr++ = value1;
		*tempWordPtr++ = value2;
		*tempWordPtr++ = aValue;
		*tempWordPtr++ = value0;
		*tempWordPtr++ = value1;
		*tempWordPtr++ = value2;
		*tempWordPtr++ = aValue;
		}
	}

#endif //USE_SCREENDRIVER_ARM_ASM

