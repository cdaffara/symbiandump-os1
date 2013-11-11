// Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
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

//The method prepares RGB value writting, calculating physical aX, aY, aWidth and aHeight.
//The calculated coordinates are used by methods wich do not use methods in BmDraw8Scaling.cpp
//file - WriteRgbMultiXOR(), WriteRgbMultiAND(), WriteRgbMultiOR(). 
//If the scaling is "off" then it is very simple - draw physical pixels. But if the scaling is 
//"on" then a rectanngle is being drawn instead of a pixel. And rectangle coordinates are 
//calculated depending on the device' orientation.
//aX, aY - logical coordinates.
//aWidth, aHeight - output parameters, they will be initialized in the method's body,
//if aDrawMode is not CGraphicsContext::EPenmode. If aDrawMode is CGraphicsContext::EPenmode,
//then aWidth will be set with iScalingSettings.iFactorX and aheight - with 
//iScalingSettings.iFactorY.
void CDrawBitmap::PreWriteRgb(TInt& aWidth, 
							  TInt& aHeight, 
							  TInt& aX, 
							  TInt& aY, 
							  CGraphicsContext::TDrawMode aDrawMode)
	{
	DeOrientate(aX, aY);//aX and aY - physical coordinates

	__ASSERT_DEBUG(aX >= 0 && aX < iSize.iWidth,Panic(EScreenDriverPanicOutOfBounds));
	__ASSERT_DEBUG(aY >= 0 && aY < iSize.iHeight,Panic(EScreenDriverPanicOutOfBounds));

	//If scaling is ON, instead of drawing single point, rectangle with size [FactorX, FactorY]
	//is drawn. Rectangle's width and height might be swapped, depending on the orientation.
	aWidth = iScalingSettings.iFactorX;
	aHeight = iScalingSettings.iFactorY;

	if(iScalingOff)
		{
		return;
		}

	//Do this additional deorientation only for not EPenmode modes and when scaling is ON!
	//EPenmode uses WriteRgb(aX,aY,aColor), which does not need any changes in 
	//coordinates!
	if(!(aDrawMode & CGraphicsContext::EPenmode))
		{
		//aX, aY - physical coordinates
		//aWidth, aWidth - output parameters
		//When scaling is ON, instead of drawing single point, rectangle is drawn 
		//and top-left coordinates, width and height has to be calculated.
		register TInt scalingFactorX = aWidth;
		register TInt scalingFactorY = aHeight;
		__ASSERT_DEBUG(scalingFactorX > 0, User::Invariant());
		__ASSERT_DEBUG(scalingFactorY > 0, User::Invariant());
		register CFbsDrawDevice::TOrientation orientation = iOrientation;

	switch(orientation)
		{
		case  EOrientationRotated90:
		case  EOrientationRotated270:
			{
			if(orientation == EOrientationRotated90)
					{
					aX -= (scalingFactorX - 1);
					}
			if(orientation == EOrientationRotated270)
					{
					aY -= (scalingFactorY - 1);
					}
			break;
			}	
		case EOrientationRotated180 :
			{
			aX -= (scalingFactorX - 1);
			aY -= (scalingFactorY - 1);
			break;
			}
		default:
			break;
		}
		//If the calculated coordinates are negative, set them to 0.
		if(aX < 0)
			{
			aX = 0;
			}
		if(aY < 0)
			{
			aY = 0;
			}
		}
	}

//Writes RGB value at the specified physical screen coordinates.
//When scaling is ON - rectangle [aWidth, aHeight] is drawn instead of single point.
//Depending on the orientation aWidth and aHeight might be swapped.
//aX, aY - physical coordinates
//aWidth, aHeight - physical
void CDrawBitmap::WriteRgb(TInt& aWidth, TInt& aHeight, 
						   TInt& aX, TInt& aY, TRgb aColor, 
						   CGraphicsContext::TDrawMode aDrawMode)
	{
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
		WriteRgb(aX,aY,aColor);
		return;
		}
	__ASSERT_DEBUG(aWidth > 0, User::Invariant());
	__ASSERT_DEBUG(aHeight > 0, User::Invariant());
	if(aDrawMode&CGraphicsContext::EWriteAlpha)
		{
		// WriteRgbMulti is the only available api that writes alpha
		WriteRgbMulti(aX,aY,aWidth,aHeight,aColor);
		return;
		}
	if(aDrawMode&CGraphicsContext::EInvertScreen)
		{
		WriteRgbMultiXOR(aX, aY, aWidth, aHeight, KRgbWhite);
		}
	if(aDrawMode&CGraphicsContext::EXor)
		{
		WriteRgbMultiXOR(aX, aY, aWidth, aHeight, aColor);
		}
	else if(aDrawMode&CGraphicsContext::EAnd)
		{
		WriteRgbMultiAND(aX, aY, aWidth, aHeight, aColor);
		}
	else if(aDrawMode&CGraphicsContext::EOr)
		{
		WriteRgbMultiOR(aX, aY, aWidth, aHeight, aColor);
		}
	}

/**
Implementation for CFbsDrawDevice::GetDrawRect().
Gets logical coordinates of the drawing rectangle.
If the device is not scaled and with zero origin, logocal coordinates of 
the drawing rectangle are the same as its physical coordinates.
If the device is rotated, drawing rectangle width and height are swapped.
Always prefer GetDrawRect() to SizeInPixels() call. SizeInPixels() will return
drawing rectangle width and height. But if the device is scaled or with nonzero origin,
GetDrawRect() will take into account and the top-left corner of the drawing rectangle too,
which may not be [0, 0].
@param aRect Upon return aRect contains drawing rectangle logical coordinates.
*/
void CDrawBitmap::GetDrawRect(TRect& aDrawRect) const
	{
	aDrawRect = iDrawRect;
	if (iOrientation & 1)
		{//90 or 270 degrees
		if (iOrigin.iX!=iOrigin.iY || iScalingSettings.iFactorX!=iScalingSettings.iFactorY)
			{		//When the scales are different between the dimensions the origin needs to recalculating
			aDrawRect.iTl.iX=Origin(iOrigin.iX,iScalingSettings.iFactorY);
			aDrawRect.iTl.iY=Origin(iOrigin.iY,iScalingSettings.iFactorX);
			}
		aDrawRect.SetWidth(iDrawRect.Height());
		aDrawRect.SetHeight(iDrawRect.Width());
		}
	}

/**
Implementation for MScalingSettings::Set().
Sets scaling factor by which the drawing device should scale the drawing images.
If you want to un-scale the device, call Set() with 
factorX = 1, factorY = 1, divisorX = 1, divisorY = 1.
@param aFactorX Scaling factor for the X-axis of the screen device.
@param aFactorY Scaling factor for the y-axis of the screen device.
@param aDivisorX Not used. Should be set to 1.
@param aDivisorY Not used. Should be set to 1.
@return KErrNone success.
*/
TInt CDrawBitmap::Set(TInt aFactorX, TInt aFactorY, TInt aDivisorX, TInt aDivisorY)
	{
	__ASSERT_DEBUG(aDivisorX == 1 && aDivisorY == 1, User::Invariant());
	__ASSERT_DEBUG(aFactorX > 0 && aFactorY > 0, User::Invariant());

	iScalingSettings.iFactorX = aFactorX;
	iScalingSettings.iFactorY = aFactorY;
	iScalingSettings.iDivisorX = aDivisorX;
	iScalingSettings.iDivisorY = aDivisorY;

	iScalingOff = aFactorX == 1 && aFactorY == 1 && aDivisorX == 1 && aDivisorY == 1;

	InitLogicalCoordinates();

	return KErrNone;
	}

/**
Implementation for MScalingSettings::Get().
Retrieves X-axis and Y-axis scaling factors.
@param aFactorX Upon return contains X-axis scaling factor.
@param aFactorY Upon return contains Y-axis scaling factor.
@param aDivisorX Upon return contains the decimal fraction of X-axis scaling factor.
@param aDivisorY Upon return contains the decimal fraction of Y-axis scaling factor.
*/
void CDrawBitmap::Get(TInt& aFactorX, TInt& aFactorY, TInt& aDivisorX, TInt& aDivisorY)
	{
	aFactorX = iScalingSettings.iFactorX;
	aFactorY = iScalingSettings.iFactorY;
	aDivisorX = iScalingSettings.iDivisorX;
	aDivisorY = iScalingSettings.iDivisorY;
	}

/**
Implementation for MScalingSettings::IsScalingOff().
Notifies the caller whether the drawing device is scaled or not.
@return ETrue Drawing device is not scaled, EFalse - it is scaled.
*/
TBool CDrawBitmap::IsScalingOff()
	{
	return iScalingOff;
	}

/**
Implementation for MDrawDeviceOrigin::Set().
Sets drawing device origin.
If you want to the default origin, call Set() with Origin (0,0).
@param aOrigin Specifies physical coordinates of the new scaling origin
of the drawing device. The drawing device maps the logical point [0,0] to
the "aOrigin" physical point .
@return KErrNone success.
*/
TInt CDrawBitmap::Set(const TPoint& aOrigin)
	{
	__ASSERT_DEBUG(aOrigin.iX >= 0 && aOrigin.iY >= 0, User::Invariant());
	__ASSERT_DEBUG(aOrigin.iX < iSize.iWidth && aOrigin.iY < iSize.iHeight, User::Invariant());

	iOrigin = aOrigin;

	iOriginIsZero = iOrigin.iX == 0 && iOrigin.iY == 0;

	InitLogicalCoordinates();

	return KErrNone;
	}

/**
Implementation for MDrawDeviceOrigin::Get().
Retrieves origin point.
@param aOrigin Upon return contains scaling origin point.
*/
void CDrawBitmap::Get(TPoint& aOrigin)
	{
	aOrigin = iOrigin;
	}

//Initializes iDrawRect data member, which contains logical coordinates of
//the drawing(screen) rectangle.
//The method does not use iOrientation data member.
void CDrawBitmap::InitLogicalCoordinates()
	{
	register TInt orgX = iOrigin.iX;
	register TInt orgY = iOrigin.iY;
	register TInt fX = iScalingSettings.iFactorX;
	register TInt fY = iScalingSettings.iFactorY;

	iDrawRect.iTl.iX = Origin(orgX, fX);
	iDrawRect.iTl.iY = Origin(orgY, fY);
	iDrawRect.iBr.iX = OtherSide(orgX, iSize.iWidth, fX);
	iDrawRect.iBr.iY = OtherSide(orgY, iSize.iHeight, fY);
	if (orgX!=orgY)
		{		//The number of addressable pixels in the physical dimensions
				//Sometimes needs to be one less when rotationed by 90 or 270
				//If so set it one less all the time as the value is not recalculated
		if (fX>1)
			{	//Calculated the physical left and right of screen when rotation by 90 or 270
				//Use this width if it is smaller
			TInt left = Origin(orgY, fX);
			TInt right = OtherSide(orgY, iSize.iWidth, fX);
			iDrawRect.iBr.iX = Min(iDrawRect.iBr.iX, iDrawRect.iTl.iX+(right-left));
			}
		if (fY>1)
			{	//Calculated the physical top and bottom of screen when rotation by 90 or 270
				//Use this height if it is smaller
			TInt top = Origin(orgX, fY);
			TInt bottom = OtherSide(orgX, iSize.iHeight, fY);
			iDrawRect.iBr.iY = Min(iDrawRect.iBr.iY, iDrawRect.iTl.iY+(bottom-top));
			}
		}
	}

/**
Notifies the caller whether the drawing device can be scaled or not.
@return ETrue Drawing device can be scaled, EFalse - it can't be scaled.
*/
TBool CDrawBitmap::CanBeScaled() const
	{
	//The function will return true for all display modes 
	return ETrue;
	}

/**
Notifies the caller whether the drawing device origin can be moved from (0, 0) point or not.
@return ETrue Drawing device origin can be moved, EFalse - it can't be moved.
*/
TBool CDrawBitmap::CanOriginBeMoved() const
	{
	//The function will return true for all display modes 
	return ETrue;
	}

//This method calculates pixel increment value and row increment value depending
//on the orientation. The device might be scaled. The result is returned in
//aPixelInc and aRowInc parameters.
void CDrawBitmap::SetPixelInc(TInt& aPixelInc, TInt& aRowInc) const
	{
	register TInt scalingFactorX = iScalingSettings.iFactorX;
	register TInt scalingFactorY = iScalingSettings.iFactorY;
	switch(iOrientation)
		{
		case EOrientationNormal:
			{
			aPixelInc = scalingFactorX;
			aRowInc = iLongWidth * scalingFactorY;
			break;
			}
		case EOrientationRotated90:
			{
			aPixelInc = iLongWidth * scalingFactorY;
			aRowInc = -scalingFactorX;
			break;
			}
		case EOrientationRotated180:
			{
			aPixelInc = -scalingFactorX;
			aRowInc = -iLongWidth * scalingFactorY;
			break;
			}
		default: // EOrientationRotated270
			{
			aPixelInc = -iLongWidth * scalingFactorY;
			aRowInc = scalingFactorX;
			break;
			}	
		}
	}

//Calculates "Y" increment value and assigns it to aY parameter.
//The device might be scaled and rotated.
//It is used by WriteBinary(), WriteBinaryOp(), ... methods and only there.
//The method is very specific for the methods mentioned above - 
//if the device is scaled and rotated 0 or 180 degrees, Y-axis coordinate has to 
//be incremented not by 1, but by iScalingSettings.iFactorY. Because of the integer divison 
//when transforming logical to physical coordinates, incremented Y-axis coordinates may 
//go negative or greater than drawing rectangle height. Then it has to be adjusted to 0 
//or rectangle height.
void CDrawBitmap::IncScaledY(TInt& aY, TInt aYOrg) const
	{
	const TOrientation orientation = iOrientation;
	const TInt fY = iScalingSettings.iFactorY;
	switch(orientation)
		{
		case EOrientationNormal:
			{
			aY += fY;
			const TInt height = iSize.iHeight - 1;
			if(aY > height)
				{
				aY = height;
				}
			break;
			}
		case EOrientationRotated180:
			{
			aY -= fY;
			if(aY < 0)
				{
				aY = 0;
				}
			break;
			}
		default:
			{
			aY = aYOrg;
			}	
		}
	}

//Calculates "Y" increment value and assigns it to aY parameter.
//The device might be scaled and rotated.
//It is used by WriteBinary(), WriteBinaryOp(), WriteLine...() methods and only there.
//The method is very specific for the methods mentioned above - 
//if the device is scaled and rotated 90 or 270 degrees, Y-axis coordinate has to 
//be incremented not by 1, but by iScalingSettings.iFactorX. Because of the integer divison 
//when transforming logical to physical coordinates, incremented Y-axis coordinates may 
//go negative or greater than drawing rectangle height. Then it has to be adjusted to 0 
//or rectangle height.
void CDrawBitmap::IncScaledY(TInt& aY) const
	{
	const TOrientation orientation = iOrientation;
	const TInt fX = iScalingSettings.iFactorY;
	if(orientation == EOrientationRotated90)
		{
		aY += fX;
		const TInt height = iSize.iHeight - 1;
		if(aY > height)
			{
			aY = height;
			}
		}
	else if(orientation == EOrientationRotated270)
		{
		aY -= fX;
		if(aY < 0)
			{
			aY = 0;
			}
		}
	}

//Increment without scaling
TInt CDrawBitmap::PixelAddressIncrement() const
	{
	switch (iOrientation)
		{
	case EOrientationNormal:
		return 1;
	case EOrientationRotated90:
		return iLongWidth;
	case EOrientationRotated180:
		return -1;
	case EOrientationRotated270:
		return -iLongWidth;
	default:
		break;
		}
	return KInvalidValue;
	}

//Pixel increment with scaling.
//It is used by CDrawEightBppBitmapCommon::SetPixels,
//CDrawEightBppBitmapCommon::XORPixels, CDrawEightBppBitmapCommon::ANDPixels,
//CDrawEightBppBitmapCommon::ORPixels methods
TInt CDrawBitmap::LogicalPixelAddressIncrement() const
	{
	register TInt scalingFactorX = iScalingSettings.iFactorX;
	register TInt scalingFactorY = iScalingSettings.iFactorY;
	switch (iOrientation)
		{
	case EOrientationNormal:
		return scalingFactorX;
	case EOrientationRotated90:
		return iLongWidth * scalingFactorY;
	case EOrientationRotated180:
		return -scalingFactorX;
	case EOrientationRotated270:
		return -iLongWidth * scalingFactorY;
	default:
		break;
		}
	return KInvalidValue;
	}

inline TInt CDrawBitmap::Origin(TInt aPhysOrg, TInt aScale) const
	{
	return -(aPhysOrg/aScale + (aPhysOrg%aScale ? 1:0));
	}

inline TInt CDrawBitmap::OtherSide(TInt aPhysOrg, TInt aPhysSize, TInt aScale) const
	{
	return (aPhysSize-1-aPhysOrg) / aScale + 1;
	}
