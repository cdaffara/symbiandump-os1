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


#include "tsmallwindowstestopengl.h"
#include "tsmallwindowopengl.h"

#include <imageconversion.h>


const TInt KNumberOfBounces = 3;

_LIT(KDummy, "dummy");
_LIT(KTestStep0013,"GRAPHICS-UI-BENCH-S60-0013");

const TInt KWindowsAcross = 2;
const TInt KWindowsDown = 2;


CTSmallWindowsTestOpenGL::CTSmallWindowsTestOpenGL()
	{
	SetTestStepName(KTSmallWindowsOpenGL);	
	}

CTSmallWindowsTestOpenGL::~CTSmallWindowsTestOpenGL()
	{
    delete iFlowWindowsController;
    iFileNames.Close();
	}

TVerdict CTSmallWindowsTestOpenGL::doTestStepPreambleL()
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
			windowCreatorFunctions.AppendL(CTSmallWindowOpenGL::NewL);
			initialPositions.AppendL(initialPosition);
			iFileNames.AppendL(KDummy());
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
TVerdict CTSmallWindowsTestOpenGL::doTestStepL()
    {
    SetTestStepID(KTestStep0013);
    FlowWindowsL();
    //RecordTestResultL(); // todo: not possible because of heap alloc panic
    return TestStepResult();
    }

/**
@SYMTestCaseID
GRAPHICS-UI-BENCH-S60-0013

@SYMTestCaseDesc
Tests how long it takes to move small OpenGL ES windows over the screen.

@SYMTestActions
Creates windows which draw OpenGL ES content and moves them over the screen.

@SYMTestExpectedResults
Test should pass and write the average framerate of the test to a log file.
*/
void CTSmallWindowsTestOpenGL::FlowWindowsL()
    {
    iProfiler->InitResults();
    iIterationsToTest = 50;
    for (TInt i = 0; i < iIterationsToTest; ++i)
        {
        iFlowWindowsController->MoveL();
        }
    iProfiler->MarkResultSetL();
    TInt drawingRect = iWindowSize.iHeight * iWindowSize.iWidth * KWindowsAcross * KWindowsDown;  
    iProfiler->ResultsAnalysisFrameRate(KTestStep0013, 0, 0, 0, iIterationsToTest, drawingRect);    
    }
