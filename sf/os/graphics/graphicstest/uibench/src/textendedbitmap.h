// Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
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
 
#ifndef TEXTENDEDBITMAP_H
#define TEXTENDEDBITMAP_H

#include "te_graphicsperformanceSuiteStepBase.h"
#include <graphics/fbsrasterizer.h>

const TInt KBitmapWidth = 128;
const TInt KBitmapHeight = 128;
const TSize KBitmapSize(KBitmapWidth,KBitmapHeight);

/** Class used to measure performance of extended bitmaps. 
Please note that all tests in this class (apart from CreateExtendedBitmapL()) 
are currently set to measure the performance of the test example rasterizer 
(found in graphics\fbserv\trasterizer\). Users that wish to measure the
performance of a different rasterizer must at least change the Uid of the 
extended bitmaps on test to one supported by the extended bitmap rasterizer
to be tested (see KExtendedBitmapUid).
 */
class CTExtendedBitmap : public CTe_graphicsperformanceSuiteStepBase
	{
public:
	CTExtendedBitmap();
	~CTExtendedBitmap();

private:			
	// From CTestStep
	virtual TVerdict doTestStepL();
	virtual TVerdict doTestStepPreambleL(); 	
	
	// Test steps
	void CreateExtendedBitmapL();	
	void DeleteExtendedBitmapL();
	void GetScanlinePreRenderedExtendedBitmapL();
	void GetScanlineNonPreRenderedExtendedBitmapL();
	void BitBltExtendedBitmapL();
	void BitBltNormalBitmapL();
	void DrawBitmapExtendedBitmapL();
	void DrawBitmapNormalBitmapL();
	void GetPixelL(TBool aIsExtendedBmp);

private:	
	// Helper functions for setting up tests and cleaning up afterwards
	void InitializeL();
	void CleanUpL();		
		
	CFbsRasterizer* iRasterizer;
	
	CFbsBitmap* iExtendedBmp;
	TUint8* iExtendedBmpData;
	TInt iExtendedBmpDataSize;
	
	CFbsBitmap* iNormalBmp;	
	CFbsBitmap* iTargetBmp;	
	CFbsBitmapDevice* iBitmapDevice;
	CFbsBitGc* iBitGc;	
	};

_LIT(KTExtendedBitmap,"textendedbitmap");

#endif // TEXTENDEDBITMAP_H
