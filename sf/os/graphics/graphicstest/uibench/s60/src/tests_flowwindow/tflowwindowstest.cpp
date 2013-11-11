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


#include "tflowwindowstest.h"
#include "tsmallwindowraster.h"


_LIT(KTestStep0006, "GRAPHICS-UI-BENCH-S60-0006");
_LIT(KSectNameFlowsTest, "FlowTests");
_LIT(KKeyNamePreload, "Preload");
_LIT(KKeyNameFiles, "Files");
_LIT(KKeyNameWindowWidth, "WindowWidth");
_LIT(KKeyNameWindowHeight, "WindowHeight");

CTFlowWindowsTest::CTFlowWindowsTest()
	{
	SetTestStepName(KTFlowWindows);
	}

CTFlowWindowsTest::~CTFlowWindowsTest()
	{	
	iFileNames.Close();
	delete iFlowWindowsController;
	}

TVerdict CTFlowWindowsTest::doTestStepPreambleL()
	{
  	CTe_graphicsperformanceSuiteStepBase::doTestStepPreambleL();
	
	iPreload = EFalse; // default behaviour
	GetBoolFromConfig(KSectNameFlowsTest, KKeyNamePreload, iPreload);

	TPtrC fileNameList;
	TEST(GetStringFromConfig(KSectNameFlowsTest,KKeyNameFiles, fileNameList));
	ExtractListL(fileNameList, iFileNames);	
	
	TSize windowSize = CTWindow::GetDisplaySizeInPixels();
	INFO_PRINTF2(_L("Window-width: %d"), windowSize.iWidth);
	INFO_PRINTF2(_L("Window-height: %d"), windowSize.iHeight);
	GetIntFromConfig(KSectNameFlowsTest, KKeyNameWindowWidth, windowSize.iWidth);
	GetIntFromConfig(KSectNameFlowsTest, KKeyNameWindowHeight, windowSize.iHeight);
	
	RArray<TPoint> initialPositions;
	RArray<pTWindowCreatorFunction> windowCreatorFunctions;
	CleanupClosePushL(initialPositions);
	CleanupClosePushL(windowCreatorFunctions);
	TInt numberOfWindows = iFileNames.Count();
	TPoint initialPosition(0, 0);
	for (TInt i = 0; i < numberOfWindows; ++i)
		{
		windowCreatorFunctions.AppendL(CTSmallWindowRaster::NewL);
		initialPositions.AppendL(initialPosition);
		initialPosition.iX += windowSize.iWidth;
		}
	// run the test enough frames to see all the windows go by
	iIterationsToTest = windowSize.iWidth * numberOfWindows;
    iFlowWindowsController = CTFlowWindowsController::NewL(iPreload, iFileNames, windowSize, windowCreatorFunctions, initialPositions, EFalse);
	
	CleanupStack::PopAndDestroy(2, &initialPositions);
	return TestStepResult();
	}

/**
    Override of base class pure virtual
    Our implementation only gets called if the base class doTestStepPreambleL() did
    not leave.
    
    @return - TVerdict code
*/
TVerdict CTFlowWindowsTest::doTestStepL()
    {
    SetTestStepID(KTestStep0006);
    TRAPD(err, FlowWindowsL());
    if (err != KErrNone)
        {
        SetTestStepResult(EAbort);
        }
    return TestStepResult();
    }

/**
@SYMTestCaseID
GRAPHICS-UI-BENCH-S60-0006

@SYMTestCaseDesc
Tests how long it takes to moves windows over the screen.

@SYMTestActions
Creates windows which draw bitmaps and moves them over the screen.

@SYMTestExpectedResults
Test should pass and write the average framerate of the test to a log file.
*/
void CTFlowWindowsTest::FlowWindowsL()
    {
    iIterationsToTest = 50;
    iProfiler->InitResults();
    for (TInt i = 0; i < iIterationsToTest; ++i)
        {
        iFlowWindowsController->MoveL();
        }
    iProfiler->MarkResultSetL();
    TSize windowSize = CTWindow::GetDisplaySizeInPixels();   
    iProfiler->ResultsAnalysisFrameRate(KTestStep0006, 0, 0, 0, iIterationsToTest, windowSize.iWidth * windowSize.iHeight);    
    }
