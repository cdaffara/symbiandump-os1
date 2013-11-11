// Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef __TBITBLT_H__
#define __TBITBLT_H__

#include <bitdev.h>
#include "TGraphicsHarness.h"


/**
Creates a virtual bitmap device if a real screen device of the specified display mode is not supported by the hardware
*/
class CVirtualBitmapDevice : public CBase
	{
public:
	virtual ~CVirtualBitmapDevice();
	static CVirtualBitmapDevice* NewL(TDisplayMode aDisplayMode, TSize aSize);
	void Update();
	CBitmapDevice& BitmapDevice();

private:
	void ConstructL(TDisplayMode aDisplayMode, TSize aSize);

private:
	CBitmapDevice* iBitmapDevice;	// The vitural or real screen device
	CFbsBitmap* iBitmap;			// Virtual bitmap
	TBool iIsScreenDevice; 			// ETrue if the hardware supports the screen device at the specified display mode
	};


class CTBitBlt : public CTGraphicsBase
	{
public:
	CTBitBlt(CTestStep* aStep);
	~CTBitBlt();
protected:
	//from 	CTGraphicsStep
	virtual void RunTestCaseL(TInt aCurTestCase);

	void ConstructL();
	void TestBitBltPositionsL(TBool aDoCompressed);
	TUint8* CaptureDeviceDataLC(CBitmapDevice& aDevice, TInt& aScreenByteSize);
 	TInt CreateBitmapImageAndCompareL(CVirtualBitmapDevice& aDevice, CFbsBitGc& aGc, TSize aScreenSize, TSize aBitmapSize, TDisplayMode aScreenMode, TPoint aOffset, TRect& aRect);
 	void SimpleBitBltAndTestL(CVirtualBitmapDevice& aDevice, CFbsBitGc& aGc, CFbsBitmap& aBitmap, TDisplayMode aScreenMode, TPoint aStartPoint, TRect& aRect);

private:
	CFbsBitmap *iBitmap;
	CVirtualBitmapDevice* iVirtualBmpDevice;
	};

class CTBitBltStep : public CTGraphicsStep
	{
public:
	CTBitBltStep();
protected:
	//from CTGraphicsStep
	virtual CTGraphicsBase* CreateTestL();
	virtual void TestSetupL();
	virtual void TestClose();
	};

_LIT(KTBitBltStep,"TBitGDIBitBlt");

#endif
