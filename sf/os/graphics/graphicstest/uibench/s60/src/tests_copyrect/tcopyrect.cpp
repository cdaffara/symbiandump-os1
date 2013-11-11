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


#include "tcopyrect.h"


const TInt KIterationsToTest = 100;
_LIT(KTestStep0017,"GRAPHICS-UI-BENCH-S60-0017");


CTCopyRect::CTCopyRect()
	{
	SetTestStepName(KTCopyRect);
	}
	
CTCopyRect::~CTCopyRect()	
	{
	// empty
	}

/**
Override of base class pure virtual
Our implementation only gets called if the base class doTestStepPreambleL() did
not leave. That being the case, the current test result value will be EPass.

@return - TVerdict code
*/
TVerdict CTCopyRect::doTestStepL()
    {
    SetTestStepID(KTestStep0017);
    TRAPD(err, CopyRectL());
    if (err != KErrNone)
        {
        SetTestStepResult(EAbort);
        }
    return TestStepResult();
	}

/**
@SYMTestCaseID
GRAPHICS-UI-BENCH-S60-0017

@SYMTestCaseDesc
Tests how long it takes to copy a rectangle to a different location with CFbsBitGc.
The test runs with the display modes EColor16MAP, EColor16MA, EColor16MU and EColor64K.

@SYMTestActions
Copy a rectangular area several times and measure the time it takes.

@SYMTestExpectedResults
Test should pass and display the average framerate.
*/
void CTCopyRect::CopyRectL()
    {
    for (TInt dispModeIndex = KNumValidDisplayModes - 1; dispModeIndex >= 0; --dispModeIndex)
        {
        SetScreenModeL(KValidDisplayModes[dispModeIndex]);
        iSourceRect = TRect(TPoint(0, 1), iScreenDevice->SizeInPixels());   
        iSourceRect.Resize(0, -2);
        iProfiler->InitResults();
        for(TInt i = 0; i < KIterationsToTest; ++i)
            {
            iGc->CopyRect(TPoint(0,1),iSourceRect);
            iScreenDevice->Update();
            }        
        iProfiler->MarkResultSetL();
        iProfiler->ResultsAnalysisFrameRate(KTestStep0017, 0, 0, iScreenDevice->BitmapDevice().DisplayMode(),
                KIterationsToTest, iSourceRect.Size().iWidth * iSourceRect.Size().iHeight);   
        }    
    }
