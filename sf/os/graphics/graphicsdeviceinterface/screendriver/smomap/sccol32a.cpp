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
// This module implements the class for a 32a bpp unpack color screen.
// 
//

/**
 @file
*/

#include <hal.h>
#include <e32std.h>
#include <bitdraw.h>
#include "scdraw.h"


void CDrawThirtyTwoBppScreenBitmapAlpha::SetSize(const TSize& aSize) 
	{
    CDrawBitmap::SetSize(aSize);
    __ASSERT_DEBUG(iSize == aSize, User::Invariant());
	iLongWidth = iScanLineWords;
	}

void CDrawThirtyTwoBppScreenBitmapAlpha::SetDisplayMode(CFbsDrawDevice* aDrawDevice)
	{
	CopyOldSettings(aDrawDevice) ;
	InitScreen() ;
	}

TInt CDrawThirtyTwoBppScreenBitmapAlpha::HorzTwipsPerThousandPixels() const
	{
	if (iSize.iWidth == 0)
		return 0;
	
    TInt displayMode;
    TInt r = HAL::Get(iScreenNo, HAL::EDisplayMode, displayMode);
    if (r != KErrNone)
    	return 0;
    
    TInt width = displayMode;
    r = HAL::Get(iScreenNo, HAL::EDisplayXTwips, width);
	if (r != KErrNone)
    	return 0;
   
    return (width * 1000) / iSize.iWidth;
	}

TInt CDrawThirtyTwoBppScreenBitmapAlpha::VertTwipsPerThousandPixels() const
	{
	if (iSize.iHeight == 0)
		return 0;

    TInt displayMode;
    TInt r = HAL::Get(iScreenNo, HAL::EDisplayMode, displayMode);
    if (r != KErrNone)
    	return 0;
    TInt height = displayMode;
    r = HAL::Get(iScreenNo, HAL::EDisplayYTwips, height);
    if (r != KErrNone)
    	return 0;

    return (height * 1000) / iSize.iHeight;
	}

TInt CDrawThirtyTwoBppScreenBitmapAlpha::InitScreen()
	{
	return KErrNone ;
	}

TInt CDrawThirtyTwoBppScreenBitmapAlpha::ConstructScreenL(TInt aScreenNo, TAny* aBitmapAddress, TSize aSize)
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
        
	iScanLineWords = linepitchInBytes / 4;
	ret = CDrawThirtyTwoBppBitmapAlpha::Construct(aSize);
	if (ret != KErrNone)
		return ret;
	
	TInt offsetToFirstPixel = displayMode;
	ret = HAL::Get(aScreenNo,HALData::EDisplayOffsetToFirstPixel, offsetToFirstPixel);
	if (ret != KErrNone)
		return ret;
    
	iBits = (TUint32*)((TUint32)aBitmapAddress + offsetToFirstPixel);
	return KErrNone;
	}

void CDrawThirtyTwoBppScreenBitmapAlpha::OrientationsAvailable(TBool aOrientation[4])
	{
	aOrientation[EOrientationNormal]     = ETrue ;
	aOrientation[EOrientationRotated90]  = ETrue ;
	aOrientation[EOrientationRotated180] = ETrue ;
	aOrientation[EOrientationRotated270] = ETrue ;
	}

TBool CDrawThirtyTwoBppScreenBitmapAlpha::SetOrientation(TOrientation aOrientation)
	{
	iOrientation = aOrientation ;
	return ETrue ;
	}


