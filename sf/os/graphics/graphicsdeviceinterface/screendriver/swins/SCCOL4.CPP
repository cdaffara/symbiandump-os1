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

#include "SCDRAW.H"
#include "_WININC.H"

TInt CDrawFourBppScreenBitmapColor::InitScreen()
	{
	TRect drawRect;
	GetDrawRect(drawRect);
    RWindows* window = ::WindowHandler(iScreenNo);
	window->iDisplayMode = DisplayMode();
	window->iEpocBitmapSize = drawRect.Size();
	window->iEpocBitmapLinePitch = (iScanLineWords*4);

	return KErrNone;
	}

void CDrawFourBppScreenBitmapColor::OrientationsAvailable(TBool aOrientation[4])
	{
	aOrientation[EOrientationNormal] = ETrue;
	aOrientation[EOrientationRotated90] = EFalse;
	aOrientation[EOrientationRotated180] = EFalse;
	aOrientation[EOrientationRotated270] = EFalse;
	}

void CDrawFourBppScreenBitmapColor::UpdateRect(const TRect& aRect) const
	{
	ASSERT(aRect.iTl.iX >= 0 && aRect.iTl.iY >= 0);
	ASSERT(aRect.iBr.iX <= iSize.iWidth);
	ASSERT(aRect.iBr.iY <= iSize.iHeight);

	TInt lx = aRect.iTl.iX & ~7;
	TInt rx = (aRect.iBr.iX + 7) & ~7;

	TUint8* srcePtr = ((TUint8*)(ScanLine(aRect.iTl.iY))) + (lx / 2);
	TUint8* srcePtrLimit = srcePtr + ((rx - lx) / 2);

	TInt byteWidth = iScanLineWords * 4;
	TRgb pixelColor;

	for(TInt row = aRect.iTl.iY; row < aRect.iBr.iY; row++)
		{
		TUint8* tempSrcePtr = srcePtr;
		TUint8* destPixel = WinPixelAddress(lx,row);

		while (tempSrcePtr < srcePtrLimit)
			{
			TUint8 pixelValue1 = *tempSrcePtr++;
			TUint8 pixelValue2 = TUint8(pixelValue1 >> 4);
			pixelValue1 &= 0xf;

			pixelColor = TRgb::Color16(pixelValue1);
			destPixel[0] = TUint8(pixelColor.Blue());
			destPixel[1] = TUint8(pixelColor.Green());
			destPixel[2] = TUint8(pixelColor.Red());

			if (pixelValue2 != pixelValue1)
				pixelColor = TRgb::Color16(pixelValue2);

			destPixel[3] = TUint8(pixelColor.Blue());
			destPixel[4] = TUint8(pixelColor.Green());
			destPixel[5] = TUint8(pixelColor.Red());

			destPixel += 6;
			}

		srcePtr += byteWidth;
		srcePtrLimit += byteWidth;
		}
	}

