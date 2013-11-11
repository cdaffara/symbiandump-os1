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

TInt CDrawOneBppScreenBitmap::InitScreen()
	{
	TRect drawRect;
	GetDrawRect(drawRect);
    RWindows* window = ::WindowHandler(iScreenNo);
	window->iDisplayMode = DisplayMode();
	window->iEpocBitmapSize = drawRect.Size();
	window->iEpocBitmapLinePitch = (iScanLineWords*4);

	return KErrNone;
	}

void CDrawOneBppScreenBitmap::OrientationsAvailable(TBool aOrientation[4])
	{
	aOrientation[EOrientationNormal] = ETrue;
	aOrientation[EOrientationRotated90] = EFalse;
	aOrientation[EOrientationRotated180] = EFalse;
	aOrientation[EOrientationRotated270] = EFalse;
	}

void CDrawOneBppScreenBitmap::UpdateRect(const TRect& aRect) const
	{
	ASSERT(aRect.iTl.iX >= 0 && aRect.iTl.iY >= 0);
	ASSERT(aRect.iBr.iX <= iSize.iWidth);
	ASSERT(aRect.iBr.iY <= iSize.iHeight);

	TInt lx = aRect.iTl.iX & ~0x1f;
	TInt rx = (aRect.iBr.iX + 31) & ~0x1f;

	TUint32* srcePtr = ScanLine(aRect.iTl.iY) + (lx / 32);
	TUint32* srcePtrLimit = srcePtr + ((rx - lx) / 32);

	for(TInt row = aRect.iTl.iY; row < aRect.iBr.iY; row++)
		{
		TUint32* tempSrcePtr = srcePtr;
		TUint8* destPixel = WinPixelAddress(lx,row);

		while (tempSrcePtr < srcePtrLimit)
			{
			TUint32 data = *tempSrcePtr++;

			Mem::Fill(destPixel,96,0xff);

			for (TUint32 shift = 1; shift != 0; shift <<= 1)
				{
				if (!(data & shift))
					{
					destPixel[0] = 0;
					destPixel[1] = 0;
					destPixel[2] = 0;
					}

				destPixel += 3;
				}
			}
		srcePtr += iScanLineWords;
		srcePtrLimit += iScanLineWords;
		}
	}

