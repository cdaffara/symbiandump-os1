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

/**
 @file
 @test
 @internalComponent - Internal Symbian test code 
*/
 
#ifndef __TBITBLTPERF_DIRECTGDI_H__
#define __TBITBLTPERF_DIRECTGDI_H__

#include "tdirectgditestbase.h"
#include <bitdev.h>
#include <graphics/directgdicontext.h>


class CTBitBltPerfDirectGdi : public CTDirectGdiTestBase
	{
public:	
	CTBitBltPerfDirectGdi();
	~CTBitBltPerfDirectGdi();	
	
private:
	enum TBitBltTest
		{
		EBitBlt,
		EBitBltMasked,
		EDrawBitmap,
		EDrawBitmapMasked,
		};		
		
	void BitBltBitmapTestL(TBitBltTest aBitBltTest, CFbsBitmap* aBitmapToBlt,  CFbsBitmap* aBitmapMask, TRect aCropTo, const TDesC& aTestName, CDirectGdiContext* aGc, TBool aScaled=EFalse);
	void MultiBitBltBitmapTestL(TBitBltTest aBitBltTest, RArray<CFbsBitmap*> aBitmapArray, CFbsBitmap* aBitmapMask, TBool aCrop, const TDesC& aTestName, CDirectGdiContext* aGc);
	
	void SimpleBitBltL();
	void SimpleDrawBitmapL();
	void MaskedBitBltL();
	void MaskedBitBltSameL();
	void MaskedBitBltFlickerL();
	void MaskedBitBltAlphaL();
	void WriteAlphaLineExTestL();
	void BitBltCpuAccessL();
	void SimpleDrawResourceL();
	void MaskedDrawBitmapL();
	void MaskedDrawBitmapScaledL();

	// From CTestStep
#ifndef SYMBIAN_GRAPHICS_EGL_SGIMAGELITE
	virtual TVerdict doTestStepPreambleL();
	virtual TVerdict doTestStepPostambleL();
#endif	
	virtual TVerdict doTestStepL();
	
private:	
	CFbsBitmapDevice* iBitmapDevice;
	CFbsBitmap* iAlpha8bit;
	RArray<CFbsBitmap*> iBitmapImage;	// Store of bitmap images for BitBlt tests
	RArray<CFbsBitmap*> iBitmapTile;		
	};

_LIT(KTBitBltPerfDirectGdi,"tbitbltperf_directgdi");

#endif
