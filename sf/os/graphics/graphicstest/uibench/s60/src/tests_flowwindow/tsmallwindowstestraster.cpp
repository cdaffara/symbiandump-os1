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


#include "tsmallwindowstestraster.h"
#include "tsmallwindowraster.h"


const TInt KWindowsAcross = 2;
const TInt KWindowsDown = 2;

const TInt KNumberOfBounces = 3;

_LIT(KSmallBitmap,"z:\\resource\\apps\\uibench_s60_small.mbm");
_LIT(KTestStep0015,"GRAPHICS-UI-BENCH-S60-0015");


CTSmallWindowsTestRaster::CTSmallWindowsTestRaster()
    {
    SetTestStepName(KTSmallWindowsRaster);  
    }

CTSmallWindowsTestRaster::~CTSmallWindowsTestRaster()
    {
    delete iFlowWindowsController;
    iFileNames.Close();
    }

TVerdict CTSmallWindowsTestRaster::doTestStepPreambleL()
    {
    CTe_graphicsperformanceSuiteStepBase::doTestStepPreambleL();
    
    TSize screenSize = CTWindow::GetDisplaySizeInPixels();
    
    // set window size to create required grid
    iWindowSize.iHeight = screenSize.iHeight / KWindowsDown;
    iWindowSize.iWidth = screenSize.iWidth / KWindowsAcross;    
    // adjust window size to maintain image aspect ratio
    if (iWindowSize.iWidth > iWindowSize.iHeight)
        {
        iWindowSize.iWidth = iWindowSize.iHeight;
        }
    else
        {
        iWindowSize.iHeight = iWindowSize.iWidth;
        }
    
    TPoint initialPosition(0, 0);
    RArray<TPoint> initialPositions;
    RArray<pTWindowCreatorFunction> windowCreatorFunctions;
    CleanupClosePushL(initialPositions);
    CleanupClosePushL(windowCreatorFunctions);
    for (TInt i = 0; i < KWindowsAcross; i++)
        {
        initialPosition.iY = 0;
        for (TInt j = 0; j < KWindowsDown; j++)
            {
            windowCreatorFunctions.AppendL(CTSmallWindowRaster::NewL);
            initialPositions.AppendL(initialPosition);
            iFileNames.AppendL(KSmallBitmap());
            initialPosition.iY += iWindowSize.iHeight;          
            }
        initialPosition.iX += iWindowSize.iWidth;
        }
    
    iFlowWindowsController = CTFlowWindowsController::NewL(ETrue, iFileNames, iWindowSize, windowCreatorFunctions, initialPositions, ETrue);
    CleanupStack::PopAndDestroy(2, &initialPositions);
    
    // run the test enough frames to see move the grid across the screen
    if (screenSize.iHeight > screenSize.iWidth)
        {
        iIterationsToTest = KNumberOfBounces * (screenSize.iHeight - iWindowSize.iHeight * KWindowsDown);
        }
    else 
        {
        iIterationsToTest = KNumberOfBounces * (screenSize.iWidth - iWindowSize.iWidth * KWindowsAcross);
        }

    return TestStepResult();
    }

/**
    Override of base class pure virtual
    Our implementation only gets called if the base class doTestStepPreambleL() did
    not leave.
    
    @return - TVerdict code
*/
TVerdict CTSmallWindowsTestRaster::doTestStepL()
    {
    SetTestStepID(KTestStep0015);
    TRAPD(err, FlowWindowsL());
    if (err != KErrNone)
        {
        SetTestStepResult(EAbort);
        }
    return TestStepResult();
    }

/**
@SYMTestCaseID
GRAPHICS-UI-BENCH-S60-0015

@SYMTestCaseDesc
Tests how long it takes to move small windows over the screen.

@SYMTestActions
Creates windows which draw Bitmaps and moves them over the screen.

@SYMTestExpectedResults
Test should pass and write the average framerate of the test to a log file.
*/
void CTSmallWindowsTestRaster::FlowWindowsL()
    {
    iProfiler->InitResults();
    iIterationsToTest = 50;
    for (TInt i = 0; i < iIterationsToTest; ++i)
        {
        iFlowWindowsController->MoveL();
        // todo: why is a pause needed to let wserv redraw the screen?
        User::After(10000); // needed, otherwise test is not waiting for wserv to redraw (10 ms)
        }
    iProfiler->MarkResultSetL();
    TInt drawingRect = iWindowSize.iHeight * iWindowSize.iWidth * KWindowsAcross * KWindowsDown;
    iProfiler->ResultsAnalysisFrameRate(KTestStep0015, 0, 0, 0, iIterationsToTest, drawingRect);
    }
