// Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
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
 
#include "tsolidcolour.h"

const TInt KIterationsToTest = 250;

const TRgb KColourSet[] = {KRgbBlack,KRgbGreen,KRgbMagenta,KRgbDarkBlue,KRgbGray,KRgbRed};
const TInt KNumColourSet = TInt(sizeof(KColourSet)/sizeof(KColourSet[0]));

CTSolidColour::CTSolidColour()
	{
	SetTestStepName(KTSolidColourName);
	}

/**
Override of base class pure virtual
Our implementation only gets called if the base class doTestStepPreambleL() did
not leave. That being the case, the current test result value will be EPass.

@return - TVerdict code
*/
TVerdict CTSolidColour::doTestStepL()
	{
	SetTestStepID(_L("GRAPHICS-UI-BENCH-0026"));
	DrawSolidColourL();
	RecordTestResultL();
	
	return TestStepResult();
	}

/**
   @SYMTestCaseID
   GRAPHICS-UI-BENCH-0026

   @SYMTestCaseDesc
   Tests how long it takes to draw a solid colour

   @SYMTestActions
   Compare the results over time, and before and after changes to bitmap duplication code.

   @SYMTestExpectedResults
   Test should pass and display total test time and time per bitmap
*/
void CTSolidColour::DrawSolidColourL()
	{
	TBool orientation[4];
	TRect blockRect(10, 10, 200, 180);

	for(TInt dispModeIndex = 0; dispModeIndex < KNumValidDisplayModes; dispModeIndex++)
		{
		SetScreenModeL(KValidDisplayModes[dispModeIndex]);			
		iGc->OrientationsAvailable(orientation);
		for (TInt orient = CFbsBitGc::EGraphicsOrientationNormal; orient <= CFbsBitGc::EGraphicsOrientationRotated270; orient++)
			{
			if (orientation[orient])
				{
				iGc->SetOrientation((CFbsBitGc::TGraphicsOrientation)orient);

				for(TInt count=KNumColourSet-1; count>=0; --count)
					{
					TRgb colour = KColourSet[count];

					iGc->SetBrushColor(KRgbWhite);
					iGc->Clear();
					iGc->SetBrushColor(colour);
					iGc->SetBrushStyle(CGraphicsContext::ESolidBrush);
					iProfiler->InitResults();
					for(TInt lc=KIterationsToTest; lc>=0; --lc)
						{
						iGc->DrawRect(blockRect);
						iProfiler->MarkResultSetL();
						}
					iProfiler->ResultsAnalysis(_L("Rect-Fill"), orient, count, KValidDisplayModes[dispModeIndex], KIterationsToTest);
					}
				}
			}
		}
	}
