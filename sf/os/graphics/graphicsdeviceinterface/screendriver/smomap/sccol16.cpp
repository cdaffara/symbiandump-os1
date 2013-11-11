// Copyright (c) 2003-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// This module implements the class for a 16 bpp color screen.
// Include files                                                   
// 
//

/**
 @file
*/
/********************************************************************/
#include <hal.h>
#include <e32std.h>
#include <bitdraw.h>
#include "scdraw.h"

/********************************************************************/
/*  Implementation of CDrawSixteenBppScreenBitmap class             */
/********************************************************************/
void CDrawSixteenBppScreenBitmap::SetSize(const TSize& aSize) 
	{
    CDrawBitmap::SetSize(aSize);
    __ASSERT_DEBUG(iSize == aSize, User::Invariant());
	iLongWidth = iScanLineWords *2;
	}
void CDrawSixteenBppScreenBitmap::SetDisplayMode(CFbsDrawDevice* aDrawDevice)
	{
	CopyOldSettings(aDrawDevice) ;
	InitScreen() ;
	}

TInt CDrawSixteenBppScreenBitmap::HorzTwipsPerThousandPixels() const
	{
	if (iSize.iWidth == 0)
		return 0 ;
	TMachineInfoV1Buf miBuf ;
	UserHal::MachineInfo(miBuf) ;
	return miBuf().iPhysicalScreenSize.iWidth * 1000 / iSize.iWidth ;
	}

TInt CDrawSixteenBppScreenBitmap::VertTwipsPerThousandPixels() const
	{
	if (iSize.iHeight == 0)
		return 0 ;

	TMachineInfoV1Buf miBuf ;
	UserHal::MachineInfo(miBuf) ;
	return miBuf().iPhysicalScreenSize.iHeight * 1000 / iSize.iHeight ;
	}

/**
This function initialize the palette. In this mode, there is no palette
defined
*/
TInt CDrawSixteenBppScreenBitmap::InitScreen()
	{
	return KErrNone ;
	}

/**
Constructs the CDrawSixteenBppScreenBitmap object.
@param aScreenNo Screen number. It will be used in HAL::Get() calls.
@param aBitmapAddress The screen memory address.
@param aSize Screen size
@return System-wide error codes, KErrNone if the construction was successfull.
*/
TInt CDrawSixteenBppScreenBitmap::ConstructScreenL(TInt aScreenNo, TAny* aBitmapAddress, TSize aSize)
	{
	iScreenNo = aScreenNo;
	TInt displayMode;
    
	TInt ret = HAL::Get(aScreenNo, HALData::EDisplayMode, displayMode);
	if (ret != KErrNone)
		return ret;

	TInt linepitchInBytes = displayMode;
	ret = HAL::Get(aScreenNo,HALData::EDisplayOffsetBetweenLines,linepitchInBytes);
	if (ret != KErrNone)
		return ret;

	CDrawBitmap::iScanLineWords = linepitchInBytes / 4;
	ret = CDrawSixteenBppBitmap::Construct(aSize);
	if (ret != KErrNone)
		return ret ;

	/* Set the pointer on the first pixel */
	TInt offsetToFirstPixel = displayMode;
	ret = HAL::Get(aScreenNo,HALData::EDisplayOffsetToFirstPixel, offsetToFirstPixel);
	if (ret != KErrNone)
		return ret;
	iBits = (TUint32*)((TUint32)aBitmapAddress + offsetToFirstPixel);
	
	return KErrNone ;
	}

/**
Define the screen orientation available
*/
void CDrawSixteenBppScreenBitmap::OrientationsAvailable(TBool aOrientation[4])
	{
	aOrientation[EOrientationNormal]     = ETrue ;
	aOrientation[EOrientationRotated90]  = ETrue ;
	aOrientation[EOrientationRotated180] = ETrue ;
	aOrientation[EOrientationRotated270] = ETrue ;
	}

/**
Set the current display orientation
*/
TBool CDrawSixteenBppScreenBitmap::SetOrientation(TOrientation aOrientation)
	{
	iOrientation = aOrientation ;
	return ETrue ;
	}
