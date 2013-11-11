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

#include "textendedbitmap.h"
#include "tdirectgditestbase.h"
#include <gdi.h>
#include <s32mem.h>
#include <e32base.h>
#include <e32uid.h>


const TInt KIterationsToTest = 1000;
const TDisplayMode KDisplayMode = EColor64K;
// Test the performance of extended bitmaps using the extended bitmap
// Uid supported by the test example rasterizer
const TUid KExtendedBitmapUid = {0x10285A78};


CTExtendedBitmap::CTExtendedBitmap()
	{
	SetTestStepName(KTExtendedBitmap);
	}

CTExtendedBitmap::~CTExtendedBitmap()
	{		
	delete iExtendedBmp;
	delete [] iExtendedBmpData;
	delete iNormalBmp;
	delete iTargetBmp;
	delete iBitmapDevice;
	delete iBitGc;		
	}

/**
Override of base class pure virtual
Our implementation only gets called if the base class doTestStepPreambleL() did
not leave. That being the case, the current test result value will be EPass.

@return - TVerdict code
*/
TVerdict CTExtendedBitmap::doTestStepL()
	{
	SetTestStepID(_L("GRAPHICS-UI-BENCH-0159"));
	CreateExtendedBitmapL();	
	RecordTestResultL();
	SetTestStepID(_L("GRAPHICS-UI-BENCH-0160"));
	DeleteExtendedBitmapL();
	RecordTestResultL();
	SetTestStepID(_L("GRAPHICS-UI-BENCH-0161"));
	GetScanlinePreRenderedExtendedBitmapL();
	RecordTestResultL();
	SetTestStepID(_L("GRAPHICS-UI-BENCH-0162"));
	GetScanlineNonPreRenderedExtendedBitmapL();
	RecordTestResultL();
	SetTestStepID(_L("GRAPHICS-UI-BENCH-0163"));
	BitBltExtendedBitmapL();
	RecordTestResultL();
	SetTestStepID(_L("GRAPHICS-UI-BENCH-0164"));
	BitBltNormalBitmapL();
	RecordTestResultL();
	SetTestStepID(_L("GRAPHICS-UI-BENCH-0165"));
	DrawBitmapExtendedBitmapL();
	RecordTestResultL();
	SetTestStepID(_L("GRAPHICS-UI-BENCH-0166"));
	DrawBitmapNormalBitmapL();
	RecordTestResultL();
	SetTestStepID(_L("GRAPHICS-UI-BENCH-0173"));
	GetPixelL(ETrue);	// extended bitmap
	RecordTestResultL();
	SetTestStepID(_L("GRAPHICS-UI-BENCH-0173"));
	GetPixelL(EFalse);	// normal bitmap
	RecordTestResultL();
		
	return TestStepResult();
	}

/**
Called before doTestStepL() to allow
initialization steps common to each test case to take place.

@return - TVerdict code
*/
TVerdict CTExtendedBitmap::doTestStepPreambleL()
	{
	TVerdict res = CTe_graphicsperformanceSuiteStepBase::doTestStepPreambleL();
	
	const TRgb colors[] = {TRgb(0,255,255), TRgb(255,0,255), TRgb(255,255,0)};
	const TInt numColors = sizeof(colors)/sizeof(colors[0]);
	const TUint8 stripe = 1; // 1 mean draw horizontal stripes, 0 means draw vertical stripes
	iExtendedBmpDataSize = sizeof(colors)+sizeof(stripe); // estimate the data size
	iExtendedBmpData = new(ELeave) TUint8[iExtendedBmpDataSize];
	RMemWriteStream ws;
	ws.Open(iExtendedBmpData, iExtendedBmpDataSize);
	CleanupClosePushL(ws);
	for (TInt i = 0; i < numColors; i++)
		{
		ws << colors[i];
		}
	ws << stripe;
	iExtendedBmpDataSize = ws.Sink()->TellL(MStreamBuf::EWrite).Offset(); // get the actual size written
	CleanupStack::PopAndDestroy(&ws);
	
	iExtendedBmp = new(ELeave) CFbsBitmap;	
	TInt err = iExtendedBmp->CreateExtendedBitmap(KBitmapSize, KDisplayMode, KExtendedBitmapUid, iExtendedBmpData, iExtendedBmpDataSize);
	TESTL(err == KErrNone);
	
	iNormalBmp = new(ELeave) CFbsBitmap;
	err = iNormalBmp->Create(KBitmapSize, KDisplayMode);
	TESTL(err == KErrNone);
	
	iTargetBmp = new(ELeave) CFbsBitmap;
	err = iTargetBmp->Create(KBitmapSize, KDisplayMode);
	TESTL(err == KErrNone);
	
	iBitmapDevice = CFbsBitmapDevice::NewL(iTargetBmp);
	TESTL(iBitmapDevice != NULL);
	
	iBitGc = CFbsBitGc::NewL();
	iBitGc->Activate(iBitmapDevice);	
		
	iRasterizer = CFbsBitmap::Rasterizer();
	if (iRasterizer == NULL)
		{
		INFO_PRINTF1(_L("Error: Extended bitmap tests only work when the example rasterizer is available, make sure \"-DFBSRASTERIZER_DLL=fbsrasterizer_test.dll\" is included in your rombuild command."));
		User::Leave(KErrGeneral);
		}
	
	return res;
	}

/**
@SYMTestCaseID
GRAPHICS-UI-BENCH-0159

@SYMPREQ 
PREQ2096

@SYMREQ
REQ10847
REQ10849

@SYMTestCaseDesc
The test determines how long it takes to create an extended bitmap.

@SYMTestActions
Measure the average time taken to create an extended bitmap by creating an
extended bitmap 1000 times and then calculating the average time.

@SYMTestExpectedResults
Test should pass and display total test time and time per extended bitmap creation.
*/
void CTExtendedBitmap::CreateExtendedBitmapL()
	{		
	CFbsBitmap* bmp = new(ELeave) CFbsBitmap;
	TESTL(bmp != NULL);
	CleanupStack::PushL(bmp);
	
	iProfiler->InitResults();	
	for(TInt count=KIterationsToTest; count>0; --count)
		{		
		iProfiler->StartTimer();		
		TInt err = bmp->CreateExtendedBitmap(KBitmapSize, EColor64K, KExtendedBitmapUid, iExtendedBmpData, iExtendedBmpDataSize);						
		iProfiler->MarkResultSetL();
		TESTL(err == KErrNone);
		
		// Reset the bitmap so the time taken to reset it the next time CreateExtendedBitmap()
		// is called is not included in the test
		bmp->Reset();
		
		}	
	iProfiler->ResultsAnalysis(_L("Create-ExtendedBitmap-64K"), 0, EColor64K, EColor64K, KIterationsToTest);
	
	CleanupStack::PopAndDestroy(bmp);
	}

/**
@SYMTestCaseID
GRAPHICS-UI-BENCH-0160

@SYMPREQ 
PREQ2096

@SYMREQ
REQ10847
REQ10849

@SYMTestCaseDesc
The test determines how long it takes to delete an extended bitmap.

@SYMTestActions
Measure the average time taken to delete an extended bitmap by creating and
deleting an extended bitmap 1000 times and then calculating the average time for the deletion.

@SYMTestExpectedResults
Test should pass and display total test time and time per extended bitmap deletion.
*/
void CTExtendedBitmap::DeleteExtendedBitmapL()
	{		
	CFbsBitmap* bmp = new(ELeave) CFbsBitmap;
	TESTL(bmp != NULL);
	CleanupStack::PushL(bmp);
	
	iProfiler->InitResults();	
	for(TInt count=KIterationsToTest; count>0; --count)
		{						
		TInt err = bmp->CreateExtendedBitmap(KBitmapSize, EColor64K, KExtendedBitmapUid, iExtendedBmpData, iExtendedBmpDataSize);
		TESTL(err == KErrNone);		
		CleanupStack::Pop(bmp);
		
		iProfiler->StartTimer();
		delete bmp;
		iProfiler->MarkResultSetL();
		
		bmp = new(ELeave) CFbsBitmap;		
		TESTL(bmp != NULL);
		CleanupStack::PushL(bmp);
		}	
	iProfiler->ResultsAnalysis(_L("Delete-ExtendedBitmap-64K"), 0, EColor64K, EColor64K, KIterationsToTest);
	
	CleanupStack::PopAndDestroy(bmp);
	}

/**
@SYMTestCaseID
GRAPHICS-UI-BENCH-0161

@SYMPREQ 
PREQ2096

@SYMREQ 
REQ10847 
REQ10860

@SYMTestCaseDesc
Measure the average time taken to get a scanline from an extended bitmap directly using
the example rasterizer where there is one call to BeginBitmap() and EndBitmap() for 
many GetScanLine() calls. This test only measures the performance of the example rasterizer. 
It is included here to help isolate performance changes in CTExtendedBitmap::BitBltExtendedBitmapL() 
and CTExtendedBitmap::DrawBitmapExtendedBitmapL().

@SYMTestActions
Measure the average time taken to get a scanline from an extended bitmap directly using
the example rasterizer by calling CFbsRasterizer::ScanLine() 1000 times for an extended
bitmap.

@SYMTestExpectedResults
Test should pass and display total test time and time per scanline.

@see GetScanlineNonPreRenderedExtendedBitmapL()
*/
void CTExtendedBitmap::GetScanlinePreRenderedExtendedBitmapL()
	{			
	CFbsRasterizer::TBitmapDesc bitmapDesc;
	TInt64 bitmapId = iExtendedBmp->SerialNumber();
	bitmapDesc.iSizeInPixels = KBitmapSize;
	bitmapDesc.iDispMode = KDisplayMode;
	bitmapDesc.iDataType = KExtendedBitmapUid;
	bitmapDesc.iData = iExtendedBmpData;
	bitmapDesc.iDataSize = iExtendedBmpDataSize;	
	iRasterizer->BeginBitmap(bitmapId, bitmapDesc, NULL);
		
	TInt h;
	const TUint32* scanline;
	TPoint point(0,0);
	
	iProfiler->InitResults();
	for(TInt count=KIterationsToTest; count>0; --count)
		{								
		for (h = 0; h < KBitmapSize.iHeight; h++)
			{
			point.iY = h;
			scanline = iRasterizer->ScanLine(bitmapId, point, KBitmapSize.iWidth);
			TESTL(scanline != NULL);
			}
		iProfiler->MarkResultSetL();
		}	
	iProfiler->ResultsAnalysis(_L("GetScanline-PreRendered-64K"), 0, EColor64K, EColor64K, KIterationsToTest);
	
	iRasterizer->EndBitmap(bitmapId);
	}

/**
@SYMTestCaseID
GRAPHICS-UI-BENCH-0162

@SYMPREQ PREQ2096

@SYMREQ 
REQ10847 
REQ10860

@SYMTestCaseDesc
Measure the average time taken to get a scanline from an extended bitmap directly using
the example rasterizer where the calls to BeginBitmap() and EndBitmap() are done for each 
GetScanLine().

@SYMTestActions
Call CFbsRasterizer::ScanLine() 1000 times for an extended bitmap. This test case is similar 
to GetScanlinePreRenderedExtendedBitmapL() but in this case each CFbsRasterizer::ScanLine() 
call is bracketed by CFbsRasterizer::BeginBitmap() and CFbsRasterizer::EndBitmap() to highlight 
the performance hit that would be seen by an application calling CFbsBitmap::GetScanLine() 
when using an extended bitmap rasterizer that does not have a cache. 

@SYMTestExpectedResults
Test should pass and display total test time and time per scanline when bracketed by 
CFbsRasterizer::BeginBitmap() and CFbsRasterizer::EndBitmap().

@see GetScanlinePreRenderedExtendedBitmapL()
*/
void CTExtendedBitmap::GetScanlineNonPreRenderedExtendedBitmapL()
	{				
	CFbsRasterizer::TBitmapDesc bitmapDesc;
	TInt64 bitmapId = iExtendedBmp->SerialNumber();
	bitmapDesc.iSizeInPixels = KBitmapSize;
	bitmapDesc.iDispMode = KDisplayMode;
	bitmapDesc.iDataType = KExtendedBitmapUid;
	bitmapDesc.iData = iExtendedBmpData;
	bitmapDesc.iDataSize = iExtendedBmpDataSize;		
		
	TInt h;
	const TUint32* scanline;
	TPoint point(0,0);
	
	iProfiler->InitResults();
	for(TInt count=KIterationsToTest; count>0; --count)
		{
		for (h = 0; h < KBitmapSize.iHeight; h++)
			{
			// Only the first call to BeginBitmap() should cause rasterization of the bitmap,
			// since the test rasterizer has a cache of recently used bitmaps.
			iRasterizer->BeginBitmap(bitmapId, bitmapDesc, NULL);
			point.iY = h;
			scanline = iRasterizer->ScanLine(bitmapId, point, KBitmapSize.iWidth);
			TESTL(scanline != NULL);
			iRasterizer->EndBitmap(bitmapId);
			}
		iProfiler->MarkResultSetL();
		}
	iProfiler->ResultsAnalysis(_L("GetScanline-NonPreRendered-64K"), 0, EColor64K, EColor64K, KIterationsToTest);
	}

/**
@SYMTestCaseID
GRAPHICS-UI-BENCH-0163

@SYMPREQ 
PREQ2096

@SYMREQ
REQ10857
REQ10859

@SYMTestCaseDesc
The test determines how long it takes to BitBlt() an extended bitmap to an offscreen bitmap.
This test is paired with BitBltNormalBitmapL().

@SYMTestActions
Measure the time taken to BitBlt() an extended bitmap to an offscreen buffer 1000 times
and calculate the average time taken. 

@SYMTestExpectedResults
Test should pass and display total test time and time per image BitBlt().

@see BitBltNormalBitmapL()
*/
void CTExtendedBitmap::BitBltExtendedBitmapL()
	{			
	const TPoint pt(0,0);
	
	iProfiler->InitResults();	
	for(TInt count=KIterationsToTest; count>0; --count)
		{
		iBitGc->BitBlt(pt, iExtendedBmp);
		iProfiler->MarkResultSetL();
		}	
	iProfiler->ResultsAnalysis(_L("BitBlt-ExtendedBitmap-64K"), 0, EColor64K, EColor64K, KIterationsToTest);
	}

/**
@SYMTestCaseID
GRAPHICS-UI-BENCH-0164

@SYMPREQ PREQ2096

@SYMTestCaseDesc
The test determines how long it takes to BitBlt() a standard bitmap that is the same size
as the extended bitmap used in the previous test case BitBltExtendedBitmapL().
This test is paired with BitBltExtendedBitmapL().

@SYMTestActions
Measure the time taken to BitBlt() a normal bitmap to an offscreen buffer 1000 times
and calculate the average time taken.

@SYMTestExpectedResults
Test should pass and display total test time and time per image BitBlt().

@see BitBltExtendedBitmapL()
*/
void CTExtendedBitmap::BitBltNormalBitmapL()
	{		
	const TPoint pt(0,0);
	
	iProfiler->InitResults();	
	for(TInt count=KIterationsToTest; count>0; --count)
		{
		iBitGc->BitBlt(pt, iNormalBmp);
		iProfiler->MarkResultSetL();
		}	
	iProfiler->ResultsAnalysis(_L("BitBlt-NormalBitmap-64K"), 0, EColor64K, EColor64K, KIterationsToTest);
	}

/**
@SYMTestCaseID
GRAPHICS-UI-BENCH-0165

@SYMPREQ 
PREQ2096

@SYMREQ
REQ10857
REQ10859

@SYMTestCaseDesc
The test determines how long it takes to draw an extended bitmap to an offscreen bitmap
using DrawBitmap(). This test is paired with DrawBitmapNormalBitmapL().

@SYMTestActions
Measure the time taken to DrawBitmap() an extended bitmap to an offscreen buffer 1000 times
and calculate the average time taken. This test is paired with DrawBitmapNormalBitmapL().

@SYMTestExpectedResults
Test should pass and display total test time and time per image DrawBitmap().

@see DrawBitmapNormalBitmapL()
*/
void CTExtendedBitmap::DrawBitmapExtendedBitmapL()
	{			
	const TPoint pt(0,0);
	const TRect rect(pt, KBitmapSize);
	
	iProfiler->InitResults();	
	for(TInt count=KIterationsToTest; count>0; --count)
		{
		iBitGc->DrawBitmap(rect, iExtendedBmp, rect);
		iProfiler->MarkResultSetL();
		}	
	iProfiler->ResultsAnalysis(_L("DrawBitmap-ExtendedBitmap-64K"), 0, EColor64K, EColor64K, KIterationsToTest);
	}

/**
@SYMTestCaseID
GRAPHICS-UI-BENCH-0166

@SYMPREQ PREQ2096

@SYMTestCaseDesc
The test determines how long it takes to draw a standard bitmap that is the same size
as the extended bitmap used in the previous test case (DrawBitmapExtendedBitmapL()) 
using DrawBitmap(). This test is paired with DrawBitmapExtendedBitmapL().
This test is paired with DrawBitmapExtendedBitmapL().

@SYMTestActions
Measure the time taken to DrawBitmap() a normal bitmap to an offscreen buffer 1000 times
and calculate the average time taken.

@SYMTestExpectedResults
Test should pass and display total test time and time per image DrawBitmap().

@see DrawBitmapExtendedBitmapL()
*/
void CTExtendedBitmap::DrawBitmapNormalBitmapL()
	{		
	const TPoint pt(0,0);
	const TRect rect(pt, KBitmapSize);
	
	iProfiler->InitResults();	
	for(TInt count=KIterationsToTest; count>0; --count)
		{
		iBitGc->DrawBitmap(rect, iNormalBmp, rect);
		iProfiler->MarkResultSetL();
		}	
	iProfiler->ResultsAnalysis(_L("DrawBitmap-NormalBitmap-64K"), 0, EColor64K, EColor64K, KIterationsToTest);
	}

/**
@SYMTestCaseID
GRAPHICS-UI-BENCH-0173

@SYMCR
CR1804

@SYMREQ 
REQ10858

@SYMTestCaseDesc
Measure the average time taken to get 128 pixels from a normal or extended bitmap.

@SYMTestActions
Measure the average time taken to get 128 pixels from a normal or extended bitmap 
by calling CFbsBitmap::GetPixel() 1000 times on the bitmap.

@SYMTestExpectedResults
Test should pass and display total test time and time per 128 pixels.
*/
void CTExtendedBitmap::GetPixelL(TBool aIsExtendedBmp)
	{
	CFbsBitmap* bmp = NULL;
	_LIT(KTestName, "GetPixel-%S-64K");
	TBuf<30> buf;

	if (aIsExtendedBmp)
		{
		_LIT(KExtBmpName, "ExtendedBitmap");
		buf.Format(KTestName, &KExtBmpName);
		bmp= iExtendedBmp;
		}
	else
		{
		_LIT(KNormalBmpName, "NormalBitmap");
		buf.Format(KTestName, &KNormalBmpName);
		bmp = iNormalBmp;
		}

	TInt y;
	TPoint point(0,0);
	TRgb color;
	iProfiler->InitResults();
	for(TInt count=KIterationsToTest; count>0; --count)
		{								
		for (y = 0; y < KBitmapSize.iHeight; ++y)
			{
			point.iY = y;
			bmp->GetPixel(color, point);
			}
		iProfiler->MarkResultSetL();
		}
	iProfiler->ResultsAnalysis(buf, 0, KDisplayMode, KDisplayMode, KIterationsToTest);
	}
