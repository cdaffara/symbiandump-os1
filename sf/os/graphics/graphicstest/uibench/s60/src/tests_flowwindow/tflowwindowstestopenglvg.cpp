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


#include "tflowwindowstestopenglvg.h"
#include "tflowwindowopengl.h"
#include "tflowwindowopenvg.h"
#include "tsmallwindowraster.h"
#include "tflowwindowscontroller.h"


_LIT(KTestStep0002,"GRAPHICS-UI-BENCH-S60-0002");
_LIT(KSectNameOpenGLVGTest, "OpenGLVGTest");
_LIT(KKeyNamePreload, "Preload");
_LIT(KKeyNameFiles, "Files");
_LIT(KKeyNameWindowWidth, "WindowWidth");
_LIT(KKeyNameWindowHeight, "WindowHeight");

CTFlowWindowsTestOpenGLVG::CTFlowWindowsTestOpenGLVG()
	{
	SetTestStepName(KTFlowWindowsOpenGLVG);
	}

CTFlowWindowsTestOpenGLVG::~CTFlowWindowsTestOpenGLVG()
	{
	delete iFlowWindowsController;
	iFileNames.Close();
	}

TVerdict CTFlowWindowsTestOpenGLVG::doTestStepPreambleL()
	{
	CTe_graphicsperformanceSuiteStepBase::doTestStepPreambleL();

	iPreload = EFalse; // default behaviour
	GetBoolFromConfig(KSectNameOpenGLVGTest, KKeyNamePreload, iPreload);
	
	TSize windowSize = CTWindow::GetDisplaySizeInPixels();
	GetIntFromConfig(KSectNameOpenGLVGTest, KKeyNameWindowWidth, windowSize.iWidth);
	GetIntFromConfig(KSectNameOpenGLVGTest, KKeyNameWindowHeight, windowSize.iHeight);
	TPtrC fileNameList;
	TEST(GetStringFromConfig(KSectNameOpenGLVGTest, KKeyNameFiles, fileNameList));
	ExtractListL(fileNameList, iFileNames);
	iIterationsToTest = windowSize.iWidth * iFileNames.Count();
	
	RArray<TPoint> initialPositions;
	RArray<pTWindowCreatorFunction> windowCreatorFunctions;
	TPoint initialPosition(0, 0);
	CleanupClosePushL(initialPositions);
	CleanupClosePushL(windowCreatorFunctions);
	windowCreatorFunctions.AppendL(CTFlowWindowOpenGL::NewL);
	initialPositions.AppendL(initialPosition);
	
    windowCreatorFunctions.AppendL(CTFlowWindowOpenVG::NewL);
    initialPosition.iX += windowSize.iWidth;
    initialPositions.AppendL(initialPosition);

	windowCreatorFunctions.AppendL(CTSmallWindowRaster::NewL);
	initialPosition.iX += windowSize.iWidth;
	initialPositions.AppendL(initialPosition);

	iFlowWindowsController = CTFlowWindowsController::NewL(ETrue, iFileNames, windowSize, windowCreatorFunctions, initialPositions, EFalse);
	CleanupStack::PopAndDestroy(2, &initialPositions);	    
	return TestStepResult();
	}

/**
    Override of base class pure virtual
    Our implementation only gets called if the base class doTestStepPreambleL() did
    not leave.
    
    @return - TVerdict code
*/
TVerdict CTFlowWindowsTestOpenGLVG::doTestStepL()
    {
    SetTestStepID(KTestStep0002);
    TRAPD(err, FlowOpenGLVGWindowsL());
    if (err != KErrNone)
        {
        SetTestStepResult(EAbort);
        }
    return TestStepResult();
    }

/**
@SYMTestCaseID
GRAPHICS-UI-BENCH-S60-0002

@SYMTestCaseDesc
Tests how long it takes to moves windows over the screen.

@SYMTestActions
Creates windows which draw bitmaps, OpenVG and OpenGL ES content and moves them over the screen.

@SYMTestExpectedResults
Test should pass and write the average framerate of the test to a log file.
*/
void CTFlowWindowsTestOpenGLVG::FlowOpenGLVGWindowsL()
    {
    iProfiler->InitResults();
    iIterationsToTest = 25;
    for (TInt i = 0; i < iIterationsToTest; ++i)
        {
        iFlowWindowsController->MoveL();
        }
    iProfiler->MarkResultSetL();
    TSize windowSize = CTWindow::GetDisplaySizeInPixels();   
    iProfiler->ResultsAnalysisFrameRate(KTestStep0002, 0, 0, 0, iIterationsToTest, windowSize.iWidth * windowSize.iHeight);
    }
