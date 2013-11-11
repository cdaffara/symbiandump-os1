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

TInt CDrawFourBppScreenBitmapGray::InitScreen()
	{
	TRect drawRect;
	GetDrawRect(drawRect);
    RWindows* window = ::WindowHandler(iScreenNo);
	window->iDisplayMode = DisplayMode();
	window->iEpocBitmapSize = drawRect.Size();
	window->iEpocBitmapLinePitch = (iScanLineWords*4);

	return KErrNone;
	}

void CDrawFourBppScreenBitmapGray::OrientationsAvailable(TBool aOrientation[4])
	{
	aOrientation[EOrientationNormal] = ETrue;
	aOrientation[EOrientationRotated90] = EFalse;
	aOrientation[EOrientationRotated180] = EFalse;
	aOrientation[EOrientationRotated270] = EFalse;
	}

void CDrawFourBppScreenBitmapGray::UpdateRect(const TRect& aRect) const
	{
	ASSERT(aRect.iTl.iX >= 0 && aRect.iTl.iY >= 0);
	ASSERT(aRect.iBr.iX <= iSize.iWidth);
	ASSERT(aRect.iBr.iY <= iSize.iHeight);

	TInt lx = aRect.iTl.iX & ~7;
	TInt rx = (aRect.iBr.iX + 7) & ~7;
	TInt wordwidth = (rx - lx) >> 3;

	for (TInt row = aRect.iTl.iY; row < aRect.iBr.iY; row++)
		{
		TUint8* destPixel = WinPixelAddress(lx,row);
		TInt wordx = lx;

		for(TInt word = 0; word < wordwidth; word++)
			{
			TUint32 data;
			ReadLine(wordx,row,8,&data);

			TUint8 grayIndex = TUint8((data & 0xf) * 17);
			destPixel[0] = grayIndex;
			destPixel[1] = grayIndex;
			destPixel[2] = grayIndex;
			grayIndex = TUint8(((data >> 4) & 0xf) * 17);
			destPixel[3] = grayIndex;
			destPixel[4] = grayIndex;
			destPixel[5] = grayIndex;
			grayIndex = TUint8(((data >> 8) & 0xf) * 17);
			destPixel[6] = grayIndex;
			destPixel[7] = grayIndex;
			destPixel[8] = grayIndex;
			grayIndex = TUint8(((data >> 12) & 0xf) * 17);
			destPixel[9] = grayIndex;
			destPixel[10] = grayIndex;
			destPixel[11] = grayIndex;
			grayIndex = TUint8(((data >> 16) & 0xf) * 17);
			destPixel[12] = grayIndex;
			destPixel[13] = grayIndex;
			destPixel[14] = grayIndex;
			grayIndex = TUint8(((data >> 20) & 0xf) * 17);
			destPixel[15] = grayIndex;
			destPixel[16] = grayIndex;
			destPixel[17] = grayIndex;
			grayIndex = TUint8(((data >> 24) & 0xf) * 17);
			destPixel[18] = grayIndex;
			destPixel[19] = grayIndex;
			destPixel[20] = grayIndex;
			grayIndex = TUint8(((data >> 28) & 0xf) * 17);
			destPixel[21] = grayIndex;
			destPixel[22] = grayIndex;
			destPixel[23] = grayIndex;

			destPixel += 24;
			wordx += 8;
			}
		}
	}

