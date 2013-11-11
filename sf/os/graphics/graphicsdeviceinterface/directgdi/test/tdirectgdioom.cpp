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

#include "tdirectgdioom.h"
#include <graphics/directgdicontext.h>

CTDirectGdiOom::CTDirectGdiOom()
	{
	SetTestStepName(KTDirectGdiOom);
	}

CTDirectGdiOom::~CTDirectGdiOom()
	{
	}

/**
@SYMTestCaseID		
	GRAPHICS-DIRECTGDI-OOM-0001

@SYMTestPriority
	Medium

@SYMPREQ
	PREQ39

@SYMREQ
	REQ9195
	REQ9201 
	REQ9202 
	REQ9222 
	REQ9223 
	REQ9236 
	REQ9237

@SYMTestCaseDesc
	Draw a line with many points to increase coverage of path drawing in OOM conditions.

@SYMTestActions		
	Create an array of 1000 points.
	Draw it as a line.

@SYMTestExpectedResults
	No image is produced by this test, it is purely for checking that no OOM errors occur.
*/
void CTDirectGdiOom::TestDrawLargePolyLineL()
	{	
	_LIT(KTestName, "OOM_TestDrawLargePolyLine"); 
	if(!iRunningOomTests)
		{
		INFO_PRINTF1(KTestName);
		}
	
	TInt err = CDirectGdiDriver::Open();
	TESTNOERRORL(err);
			
	CDirectGdiDriver* dgdiDriver = CDirectGdiDriver::Static();
	TESTL(dgdiDriver != NULL);	
	CleanupClosePushL(*dgdiDriver);	
	
	CDirectGdiContext* gc = CDirectGdiContext::NewL(*dgdiDriver);
	TESTL(gc != NULL);
	CleanupStack::PushL(gc);
	
	// Set the bitmap up...
	RSgImage rsgImage;
	TSgImageInfo imageInfo;
	imageInfo.iSizeInPixels = TSize (320, 240);
	imageInfo.iPixelFormat = iTestParams.iTargetPixelFormat;
	imageInfo.iUsage = ESgUsageDirectGdiTarget;
	err = rsgImage.Create(imageInfo, NULL,0);
	TESTNOERRORL(err);
	CleanupClosePushL(rsgImage);	
	
	RDirectGdiImageTarget dgdiImageTarget(*dgdiDriver);	
	err = dgdiImageTarget.Create(rsgImage);
	TESTNOERRORL(err);	
	CleanupClosePushL(dgdiImageTarget);

	err = gc->Activate(dgdiImageTarget);
	TESTNOERRORL(err);
	
	// Draw a polygon that had lots of points to attempt to make the AppendPathCommand 
	// methods in vgengine fail
	const TInt pCount = 1000;
	CArrayFixFlat<TPoint>* points = new (ELeave)CArrayFixFlat<TPoint>(pCount);
	TESTL(points != NULL);
	CleanupStack::PushL(points);
	
	for(TInt i = pCount; i > 0; --i)
		{
		points->AppendL(TPoint(i, i));
		}
	
	gc->DrawPolyLine(points->Array());
	TESTNOERRORL(dgdiDriver->GetError());
	
	CleanupStack::PopAndDestroy(5, dgdiDriver);
	
	FreeSgImagePixelFormatCache();
	}

/**
@SYMTestCaseID		
	GRAPHICS-DIRECTGDI-OOM-0002

@SYMTestPriority
	Medium

@SYMPREQ
	PREQ39

@SYMREQ
	REQ9195
	REQ9201 
	REQ9202 
	REQ9222 
	REQ9223 
	REQ9236 
	REQ9237

@SYMTestCaseDesc
	Set the pen size in OOM conditions.

@SYMTestActions	
	Create a graphics context.
	Call SetPenSize().

@SYMTestExpectedResults
	No memory leak should occur.
*/
void CTDirectGdiOom::TestSetPenSizeL()
	{	
	_LIT(KTestName, "OOM_TestSetPenSize"); 
	if(!iRunningOomTests)
		{
		INFO_PRINTF1(KTestName);
		}

	TInt err = CDirectGdiDriver::Open();
	TESTNOERRORL(err);
			
	CDirectGdiDriver* dgdiDriver = CDirectGdiDriver::Static();
	TESTL(dgdiDriver != NULL);	
	CleanupClosePushL(*dgdiDriver);	
	
	CDirectGdiContext* gc = CDirectGdiContext::NewL(*dgdiDriver);
	TESTL(gc != NULL);
	CleanupStack::PushL(gc);
	
	// Set the bitmap up...
	RSgImage rsgImage;
	TSgImageInfo imageInfo;
	imageInfo.iSizeInPixels = TSize (320, 240);
	imageInfo.iPixelFormat = iTestParams.iTargetPixelFormat;
	imageInfo.iUsage = ESgUsageDirectGdiTarget;
	err = rsgImage.Create(imageInfo, NULL,0);
	TESTNOERRORL(err);
	CleanupClosePushL(rsgImage);	
	
	RDirectGdiImageTarget dgdiImageTarget(*dgdiDriver);	
	err = dgdiImageTarget.Create(rsgImage);
	TESTNOERRORL(err);	
	CleanupClosePushL(dgdiImageTarget);
	
	err = gc->Activate(dgdiImageTarget);
	TESTNOERRORL(err);
	
	TRect rect(0, 0, 200, 1000);
	TSize csize(300, 300);
	
	for(TInt i = 0; i < 10; i++)
		{
		gc->SetPenSize(TSize(i,i));		
		gc->SetPenStyle(DirectGdi::ENullPen);
		gc->SetBrushStyle(DirectGdi::ESolidBrush);
		gc->DrawRoundRect(rect, csize);
		gc->SetPenStyle(DirectGdi::ESolidPen);
		gc->DrawRoundRect(rect, csize);
		TESTNOERRORL(dgdiDriver->GetError());
		}
	
	CleanupStack::PopAndDestroy(4, dgdiDriver); 
	
	FreeSgImagePixelFormatCache();
	}

/**
Helper function to force SgImage's internal cache of pixel formats to be emptied,
by asking it to lookup a configuration that will always return 0.
*/
void CTDirectGdiOom::FreeSgImagePixelFormatCache()
	{
	// This will force the SgImage's cached array of pixel formats to be freed.
	TInt count;
	TSgImageInfo info;
	info.iSizeInPixels = TSize(10, 10);
	info.iUsage = ESgUsageDirectGdiSource;
	info.iShareable = EFalse;	
	info.iCpuAccess = ESgCpuAccessReadWrite;
	info.iScreenId = 100;
	TEST(KErrNone == RSgImage::GetPixelFormats(info, NULL, count));
	TEST(count == 0);
	}

/** 
Override of base class pure virtual
Our implementation only gets called if the base class doTestStepPreambleL() did
not leave. That being the case, the current test result value will be EPass.
@leave Gets system wide error code
@return TVerdict code
*/	
TVerdict CTDirectGdiOom::doTestStepL()
	{		
	if (iUseDirectGdi)
		{
		// Just test for one pixel format to speed up testing
		iTestParams.iTargetPixelFormat = iTargetPixelFormatArray[0];
		RunOomTestsL();
		}
	SetOverallTestStepID(_L("GRAPHICS-DIRECTGDI-OOM-0001"));
	RecordTestResultL();
	SetOverallTestStepID(_L("GRAPHICS-DIRECTGDI-OOM-0002"));
	RecordTestResultL();
	CloseTMSGraphicsStep();
	return TestStepResult();
	}

/**
Override of base class virtual
@leave Gets system wide error code
@return - TVerdict code
*/
TVerdict CTDirectGdiOom::doTestStepPreambleL()
	{			
	CTDirectGdiStepBase::doTestStepPreambleL();	
	return TestStepResult();
	}

/**
Override of base class pure virtual
Lists the tests to be run
*/
void CTDirectGdiOom::RunTestsL()
	{
	TestDrawLargePolyLineL();		
	TestSetPenSizeL();
	}

