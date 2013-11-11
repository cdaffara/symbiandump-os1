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

TInt CDrawTwoBppScreenBitmap::InitScreen()
	{
	TRect drawRect;
	GetDrawRect(drawRect);
    RWindows* window = ::WindowHandler(iScreenNo);
	window->iDisplayMode = DisplayMode();
	window->iEpocBitmapSize = drawRect.Size();
	window->iEpocBitmapLinePitch = (iScanLineWords*4);

	return KErrNone;
	}

void CDrawTwoBppScreenBitmap::OrientationsAvailable(TBool aOrientation[4])
	{
	aOrientation[EOrientationNormal] = ETrue;
	aOrientation[EOrientationRotated90] = EFalse;
	aOrientation[EOrientationRotated180] = EFalse;
	aOrientation[EOrientationRotated270] = EFalse;
	}

void CDrawTwoBppScreenBitmap::UpdateRect(const TRect& aRect) const
	{
	ASSERT(aRect.iTl.iX >= 0 && aRect.iTl.iY >= 0);
	ASSERT(aRect.iBr.iX <= iSize.iWidth);
	ASSERT(aRect.iBr.iY <= iSize.iHeight);

	const TUint8 gray4lookup[4] = {0, 85, 170, 255};

	TInt lx = aRect.iTl.iX & ~0xf;
	TInt rx = (aRect.iBr.iX + 15) & ~0xf;

	TUint8* srcePtr = ((TUint8*)(ScanLine(aRect.iTl.iY))) + (lx / 4);
	TUint8* srcePtrLimit = srcePtr + ((rx - lx) / 4);

	TInt byteWidth = iScanLineWords * 4;

	for(TInt row = aRect.iTl.iY; row < aRect.iBr.iY; row++)
		{
		TUint8* tempSrcePtr = srcePtr;
		TUint8* destPixel = WinPixelAddress(lx,row);

		while (tempSrcePtr < srcePtrLimit)
			{
			TUint8 pixelValue1 = *tempSrcePtr++;
			TUint8 pixelValue2 = TUint8((pixelValue1 >> 2) & 0x03);
			TUint8 pixelValue3 = TUint8((pixelValue1 >> 4) & 0x03);
			TUint8 pixelValue4 = TUint8((pixelValue1 >> 6) & 0x03);
			pixelValue1 &= 0x03;

			TUint8 pixelGray = gray4lookup[pixelValue1];
			destPixel[0] = pixelGray;
			destPixel[1] = pixelGray;
			destPixel[2] = pixelGray;

			pixelGray = gray4lookup[pixelValue2];
			destPixel[3] = pixelGray;
			destPixel[4] = pixelGray;
			destPixel[5] = pixelGray;

			pixelGray = gray4lookup[pixelValue3];
			destPixel[6] = pixelGray;
			destPixel[7] = pixelGray;
			destPixel[8] = pixelGray;

			pixelGray = gray4lookup[pixelValue4];
			destPixel[9] = pixelGray;
			destPixel[10] = pixelGray;
			destPixel[11] = pixelGray;

			destPixel += 12;
			}

		srcePtr += byteWidth;
		srcePtrLimit += byteWidth;
		}
	}

