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


#include "trotateteststep.h"
#include "tsmallwindowraster.h"
#include "tflowwindowscontroller.h"


_LIT(KTestStep0003,"GRAPHICS-UI-BENCH-S60-0003");
_LIT(KSectNameOpenFlowTests, "FlowTests");
_LIT(KKeyNameFiles, "Files");

const TInt KNumFullRotations = 20; // number of full rotations
const TInt KNumPartlyRotations = 4; // number of partly rotations within one full rotation


CTRotateTestStep::CTRotateTestStep()
	{
	SetTestStepName(KTRotateTestStep);
	iScreenSize = CTWindow::GetDisplaySizeInPixels();
	}

CTRotateTestStep::~CTRotateTestStep()
	{	
	delete iFlowWindowsController;	
	iFileNames.Close();
	}

TVerdict CTRotateTestStep::doTestStepPreambleL()
	{
	CTe_graphicsperformanceSuiteStepBase::doTestStepPreambleL();
	TPtrC fileNameList;
	TESTL(GetStringFromConfig(KSectNameOpenFlowTests, KKeyNameFiles, fileNameList));
	ExtractListL(fileNameList, iFileNames);
 
	RArray<TPoint> initialPositions;
	RArray<pTWindowCreatorFunction> windowCreatorFunctions;
	CleanupClosePushL(initialPositions);
	CleanupClosePushL(windowCreatorFunctions);
    TInt numberOfWindows = iFileNames.Count();
    TPoint initialPosition(0, 0);
	for (TInt i = 0; i < numberOfWindows; i++)
		{
		windowCreatorFunctions.AppendL(CTSmallWindowRaster::NewL);
		initialPositions.AppendL(initialPosition);
		initialPosition.iX += iScreenSize.iWidth;
		}

	iFlowWindowsController = CTFlowWindowsController::NewL(ETrue, iFileNames, iScreenSize, windowCreatorFunctions, initialPositions, EFalse);
	// Ensure all windows are visible so that the image appears when the screen is rotated.
	iFlowWindowsController->SetWindowsVisibility(ETrue);
	
	CleanupStack::PopAndDestroy(2, &initialPositions);
	return TestStepResult();
	}

/**
    Override of base class pure virtual
    Our implementation only gets called if the base class doTestStepPreambleL() did
    not leave.
    
    @return - TVerdict code
*/
TVerdict CTRotateTestStep::doTestStepL()
    {
    SetTestStepID(KTestStep0003);
    TRAPD(err, RotateWindowsL());
    if (err != KErrNone)
        {
        SetTestStepResult(EAbort);
        }
    return TestStepResult();
    }

/**
@SYMTestCaseID
GRAPHICS-UI-BENCH-S60-0003

@SYMTestCaseDesc
Tests how long it takes to rotate and redraw the screen.

@SYMTestActions
Creates windows which draw bitmaps and rotates the screen.

@SYMTestExpectedResults
Test should pass and write the average framerate of the test to a log file.
*/
void CTRotateTestStep::RotateWindowsL()
    {
    iProfiler->InitResults();
    for (TInt i = 0; i < KNumFullRotations; i++)
        {
        iFlowWindowsController->Rotate(CFbsBitGc::EGraphicsOrientationRotated90);
        iFlowWindowsController->Rotate(CFbsBitGc::EGraphicsOrientationRotated180);
        iFlowWindowsController->Rotate(CFbsBitGc::EGraphicsOrientationRotated270);
        iFlowWindowsController->Rotate(CFbsBitGc::EGraphicsOrientationNormal);
        }
    iProfiler->MarkResultSetL();
    iProfiler->ResultsAnalysisScreenRotationRate(KTestStep0003, 0, 0, 0, KNumFullRotations * KNumPartlyRotations,
            iScreenSize.iWidth * iScreenSize.iHeight);
    }
