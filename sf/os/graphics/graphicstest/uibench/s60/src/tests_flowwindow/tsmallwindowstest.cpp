// Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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

#include "tsmallwindowstest.h"
#include "tsmallwindowraster.h"
#include "tflowwindowscontroller.h"

#include <imageconversion.h>


const TInt KNumberOfBounces = 3;

_LIT(KTestStep0012,"GRAPHICS-UI-BENCH-S60-0012");


CTSmallWindowsTest::CTSmallWindowsTest()
	{
	SetTestStepName(KTSmallWindows);
	}

CTSmallWindowsTest::~CTSmallWindowsTest()
	{
	delete iFlowWindowsController;
    iFileNames.Close();
    }

TVerdict CTSmallWindowsTest::doTestStepPreambleL()
	{
	CTe_graphicsperformanceSuiteStepBase::doTestStepPreambleL();
	iScreenSize = CTWindow::GetDisplaySizeInPixels();	
	TBool preload = EFalse;
	GetBoolFromConfig(_L("FlowTests"), _L("Preload"), preload);

	TPtrC fileNameList;
	TEST(GetStringFromConfig(_L("FlowTests"), _L("Files"), fileNameList));
	ExtractListL(fileNameList, iFileNames);

	ComputeSmallWindows();

	TPoint initialPosition(0, 0);
	RArray<TPoint> initialPositions;
    RArray<pTWindowCreatorFunction> windowCreatorFunctions;
    CleanupClosePushL(initialPositions);
    CleanupClosePushL(windowCreatorFunctions);
	for (TInt i = 0; i < iWindowsAcross; i++)
		{
		initialPosition.iY = 0;
		for (TInt j = 0; j < iWindowsAcross; j++)
			{
			windowCreatorFunctions.AppendL(CTSmallWindowRaster::NewL);
			initialPositions.AppendL(initialPosition);
			initialPosition.iY += iWindowSize.iHeight;
			}
		initialPosition.iX += iWindowSize.iWidth;
		}

	iFlowWindowsController = CTFlowWindowsController::NewL(preload, iFileNames, iWindowSize, windowCreatorFunctions, initialPositions, ETrue);
    CleanupStack::PopAndDestroy(2, &initialPositions);
	return TestStepResult();
	}

void CTSmallWindowsTest::ComputeSmallWindows()
    {
    TReal sqrtWindowsAcross;
    Math::Sqrt(sqrtWindowsAcross, iFileNames.Count());
    iWindowsAcross = sqrtWindowsAcross;

    RFs fs;
    TESTNOERRORL(fs.Connect());
    CleanupClosePushL(fs);
    CImageDecoder* decoder = CImageDecoder::FileNewL(fs, iFileNames[0], CImageDecoder::EOptionAlwaysThread);
    TFrameInfo info = decoder->FrameInfo();
    TReal imageAspectRatio = (TReal)info.iOverallSizeInPixels.iWidth/(TReal)info.iOverallSizeInPixels.iHeight;
    delete decoder;
    CleanupStack::PopAndDestroy();
    
    // set window size to create required grid
    iWindowSize.iHeight = iScreenSize.iHeight / iWindowsAcross;
    iWindowSize.iWidth = iScreenSize.iWidth / iWindowsAcross;
    TReal windowAspectRatio = (TReal)iWindowSize.iWidth / (TReal)iWindowSize.iHeight;
    
    // adjust window size to maintain image aspect ratio
    if (iWindowSize.iWidth > iWindowSize.iHeight)
        {
        iWindowSize.iWidth /= windowAspectRatio / imageAspectRatio;
        }
    else
        {
        iWindowSize.iHeight *= windowAspectRatio / imageAspectRatio;
        }
        
    // run the test enough frames to see bounce the grid twice
    if (iScreenSize.iHeight > iScreenSize.iWidth)
        {
        iIterationsToTest = KNumberOfBounces * (iScreenSize.iHeight - iWindowSize.iHeight * iWindowsAcross);
        }
    else 
        {
        iIterationsToTest = KNumberOfBounces * (iScreenSize.iWidth - iWindowSize.iWidth * iWindowsAcross);
        }
    }


/**
    Override of base class pure virtual
    Our implementation only gets called if the base class doTestStepPreambleL() did
    not leave.
    
    @return - TVerdict code
*/
TVerdict CTSmallWindowsTest::doTestStepL()
    {
    SetTestStepID(KTestStep0012);
    FlowWindowsL();
    //RecordTestResultL(); // todo: not possible because of heap alloc panic
    return TestStepResult();
    }

/**
@SYMTestCaseID
GRAPHICS-UI-BENCH-S60-0012

@SYMTestCaseDesc
Tests how long it takes to move small windows over the screen.

@SYMTestActions
Creates small windows which draw bitmaps and moves them over the screen.

@SYMTestExpectedResults
Test should pass and write the average framerate of the test to a log file.
*/
void CTSmallWindowsTest::FlowWindowsL()
    {
    iProfiler->InitResults();
    iIterationsToTest = 10;
    for (TInt i = 0; i <= iIterationsToTest; ++i)
        {
        iFlowWindowsController->MoveL();        
        }
    iProfiler->MarkResultSetL();
    TInt drawingRect = iWindowSize.iHeight * iWindowSize.iWidth * iWindowsAcross * iWindowsAcross;
    iProfiler->ResultsAnalysisFrameRate(KTestStep0012, 0, 0, 0, iIterationsToTest, drawingRect);    
    }


