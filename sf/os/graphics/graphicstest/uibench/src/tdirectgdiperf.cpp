// Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#include "tdirectgdiperf.h"
#include <graphics/directgdidriver.h>
#include <graphics/directgdicontext.h>
#include <graphics/directgdiimagetarget.h>
#ifndef SYMBIAN_GRAPHICS_EGL_SGIMAGELITE
#include <graphics/sgimage.h>

// The number of iterations to perform for each test in this suite.
const TInt KIterationsToTest = 1000;

_LIT(KTileBitmap, "z:\\system\\data\\uibench_tile.mbm");
#endif

CTDirectGdiPerf::CTDirectGdiPerf()
	{
	SetTestStepName(KTDirectGdiPerfTest);
	}

CTDirectGdiPerf::~CTDirectGdiPerf()
	{
	}

/**
Override of base class pure virtual
Our implementation only gets called if the base class doTestStepPreambleL() did
not leave. That being the case, the current test result value will be EPass.

@return - TVerdict code
*/
TVerdict CTDirectGdiPerf::doTestStepL()
	{
#ifdef SYMBIAN_GRAPHICS_EGL_SGIMAGELITE
    INFO_PRINTF1(_L("CTDirectGdiPerf can only be run with RSgImage legacy"));
    return TestStepResult();
#else
	iContext = CDirectGdiContext::NewL(*iDGdiDriver);
	iDGdiImageTarget->Close();
		
	// for each display mode
	for(TInt dispModeIndex = 0; dispModeIndex < iTargetPixelFormatArray.Count(); ++dispModeIndex)
		{
		iDisplayMode = TDisplayModeMapping::MapPixelFormatToDisplayMode(iTargetPixelFormatArray[dispModeIndex]);
		
		SetTestStepID(_L("GRAPHICS-UI-BENCH-0095"));
		CreatingTargetsL();
		RecordTestResultL();
		SetTestStepID(_L("GRAPHICS-UI-BENCH-0096"));
		MakeCurrentOnDifferentFormatTargetL();
		RecordTestResultL();
		SetTestStepID(_L("GRAPHICS-UI-BENCH-0097"));
		MakeCurrentOnSameFormatTargetL();
		RecordTestResultL();
		}
	
	delete iContext;
	iContext = NULL;
	
    CloseTMSGraphicsStep();
	return TestStepResult();
#endif	
	}

#ifndef SYMBIAN_GRAPHICS_EGL_SGIMAGELITE
/**
@SYMTestCaseID
GRAPHICS-UI-BENCH-0095

@SYMPREQ PREQ39

@SYMREQ REQ9236 
@SYMREQ REQ9237

@SYMTestCaseDesc
Tests how long it takes to create, activate and destroy a DirectGDI target.

@SYMTestActions
Setup an RSgImage for the target to be constructed from.
For KIterationsToTest, create an image target, activate the context on it, and close the target.
Cleanup.

@SYMTestExpectedResults
The time taken to perform the creation/destruction of the test to be logged.
The driver should report no errors.
*/
void CTDirectGdiPerf::CreatingTargetsL()
	{
	TInt result = KErrNone;
	_LIT(KTestName, "CreatingTargets");
	
	RSgImage rsgImage;		
	// Set the bitmap up...
	TSgImageInfo imageInfo;
	imageInfo.iSizeInPixels = TSize (320, 240);
	imageInfo.iPixelFormat = TDisplayModeMapping::MapDisplayModeToPixelFormat(iDisplayMode);
	imageInfo.iUsage = ESgUsageDirectGdiTarget;
	result = rsgImage.Create(imageInfo, NULL,0);
	TESTL(result == KErrNone);
	CleanupClosePushL(rsgImage);
	
	iProfiler->InitResults();
	for(TInt lc=KIterationsToTest; lc>0; --lc)
		{		
		RDirectGdiImageTarget dgdiImageTarget(*iDGdiDriver);
		TESTNOERRORL(dgdiImageTarget.Create(rsgImage));
		TESTNOERRORL(iContext->Activate(dgdiImageTarget));
		
		dgdiImageTarget.Close();
		iProfiler->MarkResultSetL();
		}
	TESTNOERRORL(iDGdiDriver->GetError());
	
	CleanupStack::PopAndDestroy(1, &rsgImage);
	
	
	iProfiler->ResultsAnalysis(KTestName, 0, iDisplayMode, iDisplayMode, KIterationsToTest);	
	}

/**
@SYMTestCaseID
GRAPHICS-UI-BENCH-0096

@SYMPREQ PREQ39

@SYMREQ REQ9236 
@SYMREQ REQ9237

@SYMTestCaseDesc
Tests how long it takes to make a different target current on the context after performing
some drawing on a previous target. The two targets have different pixel formats.

@SYMTestActions
Initialise the DirectGDI driver.
Create a context.
Setup an RSgImage for the target to be constructed from.
For KIterationsToTest, create two targets, activate and draw to the first one,
then start the timer and activate the second target.
Cleanup.

@SYMTestExpectedResults
The time taken to perform the test should be logged.
The driver should report no errors.
*/
void CTDirectGdiPerf::MakeCurrentOnDifferentFormatTargetL()
	{
	_LIT(KTestName, "MakeCurrentOnDifferentTargets");
	DoMakeCurrentTestL(KTestName(), EFalse);
	}

/**
@SYMTestCaseID
GRAPHICS-UI-BENCH-0097

@SYMPREQ PREQ39

@SYMREQ REQ9236 
@SYMREQ REQ9237

@SYMTestCaseDesc
Tests how long it takes to make a different target current on the context after performing
some drawing on a previous target. The targets have the same pixel formats.

@SYMTestActions
Setup an RSgImage for the target to be constructed from.
For KIterationsToTest, create two targets, activate and draw to the first one,
then start the timer and activate the second target.
Cleanup.

@SYMTestExpectedResults
The time taken to perform the test should be logged.
The driver should report no errors.
*/
void CTDirectGdiPerf::MakeCurrentOnSameFormatTargetL()
	{
	_LIT(KTestName, "MakeCurrentOnSameFormatTarget");		
	DoMakeCurrentTestL(KTestName(), ETrue);
	}


/**
Helper function to share the common code for the MakeCurrent() tests.

@param aTestName The name of the test being run, for logging.
@param aMatchingFormats If ETrue, will only run the test when the source and target display modes are 
       the same. If EFalse, will only run the test if the source and target display modes are different.
*/
void CTDirectGdiPerf::DoMakeCurrentTestL(TPtrC aTestName, TBool aMatchingFormats)
	{
	TInt result = KErrNone;
	RSgImage rsgImage1;
	RSgImage rsgImage2;
	
	// Use the other target display modes for the second pixel format. 
	for(TInt dispModeIndex = 0; dispModeIndex < iTargetPixelFormatArray.Count(); ++dispModeIndex)
		{
		const TDisplayMode displayModeOther = TDisplayModeMapping::MapPixelFormatToDisplayMode(iTargetPixelFormatArray[dispModeIndex]);
		if (aMatchingFormats && displayModeOther != iDisplayMode) continue;
		if (!aMatchingFormats && displayModeOther == iDisplayMode) continue;
	
		// Set the bitmap up...
		TSgImageInfo imageInfo;
		imageInfo.iSizeInPixels = TSize (150, 150);
		imageInfo.iPixelFormat = TDisplayModeMapping::MapDisplayModeToPixelFormat(displayModeOther);
		imageInfo.iUsage = ESgUsageDirectGdiTarget;
		result = rsgImage1.Create(imageInfo, NULL,0);
		TESTL(result == KErrNone);
		CleanupClosePushL(rsgImage1);
	
		imageInfo.iPixelFormat = TDisplayModeMapping::MapDisplayModeToPixelFormat(iDisplayMode);
		result = rsgImage2.Create(imageInfo, NULL,0);
		TESTL(result == KErrNone);
		CleanupClosePushL(rsgImage2);
		
		CFbsBitmap* bitmap1 = LoadBitmapL(KTileBitmap, 0);	
		CleanupStack::PushL(bitmap1);
		CFbsBitmap* bitmap2 = LoadBitmapL(KTileBitmap, 0);		
		CleanupStack::PushL(bitmap2);
		
		RDirectGdiImageTarget dgdiImageTarget1(*iDGdiDriver);
		RDirectGdiImageTarget dgdiImageTarget2(*iDGdiDriver);
		
		iProfiler->InitResults();
		for(TInt lc=KIterationsToTest; lc>0; --lc)
			{			
			result = dgdiImageTarget1.Create(rsgImage1);
			TESTL(result == KErrNone);
			CleanupClosePushL(dgdiImageTarget1);
			result = dgdiImageTarget2.Create(rsgImage2);
			TESTL(result == KErrNone);
			CleanupClosePushL(dgdiImageTarget2);					
	
			result = iContext->Activate (dgdiImageTarget1);		
			TESTL(result == KErrNone);
			
			iContext->Reset();
			iContext->Clear();
			iContext->BitBlt(TPoint(20, 20), *bitmap1);		
			
			iProfiler->StartTimer();
			result = iContext->Activate (dgdiImageTarget2);
			TESTL(result == KErrNone);
			iProfiler->MarkResultSetL();
			
			iContext->Reset();
			iContext->Clear();
			iContext->BitBlt(TPoint(30, 30), *bitmap2);
			
			// Close the targets.
			CleanupStack::PopAndDestroy(2, &dgdiImageTarget1);
			}
		TESTNOERRORL(iDGdiDriver->GetError());
		
		CleanupStack::PopAndDestroy(4, &rsgImage1);		
		iProfiler->ResultsAnalysis(aTestName, 0, displayModeOther, iDisplayMode, KIterationsToTest);
		}
	}
#endif
