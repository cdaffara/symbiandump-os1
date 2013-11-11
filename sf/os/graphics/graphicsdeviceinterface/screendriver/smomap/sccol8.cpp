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
// This module implemente the class use for a 8 bpp color screen.
// Include files                                                   
// 
//

/**
 @file
*/
/********************************************************************/
#include <bitdraw.h>
#include "scdraw.h"
#include <hal.h>

/********************************************************************/
/*  Implementation of CDrawEightBppScreenBitmapColor class          */
/********************************************************************/
void CDrawEightBppScreenBitmapColor::SetSize(const TSize& aSize) 
	{
	CDrawBitmap::SetSize(aSize);
	__ASSERT_DEBUG(iSize == aSize, User::Invariant());
	iLongWidth = CDrawBitmap::iScanLineWords * 4;
	}
void CDrawEightBppScreenBitmapColor::SetDisplayMode(CFbsDrawDevice* aDrawDevice)
	{
	CopyOldSettings(aDrawDevice) ;
	InitScreen() ;
	}

TInt CDrawEightBppScreenBitmapColor::HorzTwipsPerThousandPixels() const
	{
	if (iSize.iWidth == 0)
		return 0 ;
	TMachineInfoV1Buf miBuf ;
	UserHal::MachineInfo(miBuf) ;
	return miBuf().iPhysicalScreenSize.iWidth * 1000 / iSize.iWidth ;
	}

TInt CDrawEightBppScreenBitmapColor::VertTwipsPerThousandPixels() const
	{
	if (iSize.iHeight == 0)
		return 0 ;

	TMachineInfoV1Buf miBuf ;
	UserHal::MachineInfo(miBuf) ;
	return miBuf().iPhysicalScreenSize.iHeight * 1000 / iSize.iHeight ;
	}
	
/**
Initialises the palette
*/
TInt CDrawEightBppScreenBitmapColor::InitScreen()
	{

	__ASSERT_ALWAYS(iPaletteAddress!=NULL, Panic(EScreenDriverPanicNullPointer)) ;

	/* Fill the palette to support 8 Bpp color */
	TUint16* palettePtr = iPaletteAddress ;
	for (TInt count = 0; count < KEightBppPaletteEntries; count++)
		*palettePtr++ = TRgb::Color256(count)._Color4K() | KEightBppPixelBitSize ;

	return KErrNone ;
	}

/**
Constructs the CDrawEightBppScreenBitmapColor object.
@param aScreenNo Screen number. It will be used in HAL::Get() calls.
@param aBitmapAddress The screen memory address.
@param aSize Screen size
@return System-wide error codes, KErrNone if the construction was successfull.
*/
TInt CDrawEightBppScreenBitmapColor::ConstructScreenL(TInt aScreenNo, TAny* aBitmapAddress, TSize aSize)
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
	ret = CDrawEightBppBitmapColor::Construct(aSize);
	if (ret != KErrNone)
		return ret ;

	/* Set the pointer on the first palette entry */
	iPaletteAddress = (TUint16*)aBitmapAddress ;
	
	TInt offsetToFirstPixel = displayMode;
	ret = HAL::Get(aScreenNo,HALData::EDisplayOffsetToFirstPixel, offsetToFirstPixel);
	if (ret != KErrNone)
		return ret;
	iBits = (TUint32*)((TUint32)aBitmapAddress + offsetToFirstPixel);

	return KErrNone ;
	}

void CDrawEightBppScreenBitmapColor::OrientationsAvailable(TBool aOrientation[4])
	{
	aOrientation[EOrientationNormal]     = ETrue ;
	aOrientation[EOrientationRotated90]  = ETrue ;
	aOrientation[EOrientationRotated180] = ETrue ;
	aOrientation[EOrientationRotated270] = ETrue ;
	}

TBool CDrawEightBppScreenBitmapColor::SetOrientation(TOrientation aOrientation)
	{
	iOrientation = aOrientation ;
	return ETrue ;
	}

TInt CDrawEightBppScreenBitmapColor::SetCustomPalette(const CPalette* aPalette)
	{
	TInt ret = CDrawEightBppBitmapColor::SetCustomPalette(aPalette) ;

	if (ret == KErrNone)
		{
		TInt index = 0 ;
		TUint16* palettePtr = iPaletteAddress ;
		const TUint16* const palettePtrLimit = iPaletteAddress + KEightBppPaletteEntries ;

		*palettePtr++ = IndexToColor(index++)._Color4K() | KEightBppPixelBitSize ;

		while (palettePtr < palettePtrLimit)
			*palettePtr++ = IndexToColor(index++)._Color4K() ;
		}
	return ret ;
	}
