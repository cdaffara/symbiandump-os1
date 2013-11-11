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
// TMULSCREENS.CPP
// Tests the newly API function added to RWindow which enables/disables
// the usage of Off-Screen Bitmap (OSB).
// The test will draw squares with random colours on screen filling the
// whole drawable area. The drawing will start first with flickering screen
// and will switch to flicker free in 4 seconds.
// 
//

/**
 @file
 @test
 @internalComponent - Internal Symbian test code
*/

#include "TFLICKERFREE.H"

//===================================================
// CBGWin Declaration
//===================================================

CBGWin::CBGWin(): CTWin()
	{
	}

CBGWin::~CBGWin()
	{
	}

void CBGWin::ConstructWin(TPoint aPos, TSize aSize, TBool aVisible)
	{
	iSize = aSize;
	SetUpL(aPos,iSize,TheClient->iGroup,*TheClient->iGc, aVisible);
	BaseWin()->SetRequiredDisplayMode(EColor256);
	}

void CBGWin::Draw()
	{
	iGc->Clear();
	iGc->SetBrushStyle(CGraphicsContext::ESolidBrush);

	TUint propW = iSize.iWidth/32;
	TUint propH = iSize.iHeight/12;

	for(TInt i = 0; i < iSize.iWidth ; i+=propW)
		{
		for(TInt j = 0; j < iSize.iHeight; j+=propH)
			{
			iGc->SetBrushColor(TRgb( (TInt)(Math::Random()%255), (TInt)(Math::Random()%255), (TInt)(Math::Random()%255) ));
			iGc->DrawRect(TRect(i, j, i+propW-1, j+propH-1));
			}
		}
	}

void CBGWin::EnableOSBd(TBool aState)
	{
	iWin.EnableOSB(aState);
	}

//===================================================
// CFlickerTest Declaration
//===================================================

/* This function will be called periodically to draw the rects
   on screen.*/
void CTFlickerFree::TestFlickering()
	{
	for( int i = 0; i < 40; ++i)
		{
		// for the first 3 seconds draw with flicker
		if(i < 20 )
			iBgWin->EnableOSBd(EFalse);
		// for the next 3 seconds draw with flicker free
		else if(i >=20 )
			iBgWin->EnableOSBd(ETrue);

		iBgWin->DrawNow();

		// this draws 20 frames per second for 4 seconds
		User::After(50000);
		}
	}

CTFlickerFree::CTFlickerFree(CTestStep* aStep):
	CTWsGraphicsBase(aStep)
	{
	}

CTFlickerFree::~CTFlickerFree()
	{
	delete iBgWin;
	}

void CTFlickerFree::ConstructL()
	{
	// get the size of the current client to assign it to the background window
	TSize scrSize = TSize(TheClient->iScreen->SizeInPixels());

	// construct the window in the background
	iBgWin = new (ELeave) CBGWin();
	iBgWin->ConstructWin(TPoint(0,0), scrSize, ETrue);
	}

/* Sets the windowing environment, constructs the CPeriod object and
	starts the CPeriod object.*/
void CTFlickerFree::RunTestCaseL(TInt aCurTestCase)
	{
	((CTFlickerFreeStep*)iStep)->SetTestStepID(KUnknownSYMTestCaseIDName);
	switch(aCurTestCase)
		{
		case 1:
/**
@SYMTestCaseID		GRAPHICS-WSERV-0567
*/
			((CTFlickerFreeStep*)iStep)->SetTestStepID(_L("GRAPHICS-WSERV-0567"));
			TestFlickering();
			break;
		default:
			((CTFlickerFreeStep*)iStep)->SetTestStepID(KNotATestSYMTestCaseIDName);
			((CTFlickerFreeStep*)iStep)->CloseTMSGraphicsStep();
			TestComplete();
		}
	((CTFlickerFreeStep*)iStep)->RecordTestResultL();
	}

__WS_CONSTRUCT_STEP__(FlickerFree)
