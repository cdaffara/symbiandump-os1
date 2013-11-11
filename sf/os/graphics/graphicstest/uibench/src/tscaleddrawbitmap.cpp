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

#include "tscaleddrawbitmap.h"

const TInt KIterationsToTest = 55;
const TInt KDelay = 500*1000; // 0.5 seconds

#define __CyclesPerPixel(__a,__b) ((TInt)(((__a/(TInt64)__b)*((TInt64)iCPUSpeed)/((TInt64)(iWindowSize.iWidth*iWindowSize.iHeight)))/(TInt64)1000))

_LIT(KAlphaTestBitmap,"z:\\system\\data\\uibench_24bit.mbm");

const TDisplayMode ModeArray[]=
	{
	EColor64K,
	EColor16MU,
	EColor16MA,
	EColor16MAP
	};
const TInt ModeCount=sizeof(ModeArray)/sizeof(ModeArray[0]);

#define FPSHFT 8
#define VARSIZE TSize((actsizexFP>>FPSHFT),(actsizeyFP>>FPSHFT))

/**
   @SYMTestCaseID
   GRAPHICS-UI-BENCH-0020

   @SYMTestCaseDesc
   The test determines how long DrawBitmap function takes when scaling is involved.

   @SYMTestActions
   Compare the results over time, and before and after changes to wserv code.

   @SYMTestExpectedResults
   Test should pass and display total test time and cycles per pixel
*/
void CTScaledDrawBitmap::DoTestScaledDrawBitmapL(TInt aRotation, TDisplayMode aMode, TDisplayMode aScreenMode, TInt aIters) 
	{
	TSize siz=iScreenDevice->SizeInPixels();
	
	CFbsBitmap * bmpModeDep = CreateSoftwareBitmapLC(siz, aMode);
	
	CFbsBitmap * sourceOverImage = LoadBitmapL(KAlphaTestBitmap,0);
	CleanupStack::PushL(sourceOverImage);
	
	CopyBitmapL(bmpModeDep, sourceOverImage);
	
	TInt startsizeFP=1<<FPSHFT;
	TInt endscale=1;
	
	TInt endsizexFP= (endscale*iScreenDevice->SizeInPixels().iWidth)<<FPSHFT;
	TInt endsizeyFP= (endscale*iScreenDevice->SizeInPixels().iHeight)<<FPSHFT;
	
	TInt dxFP=(endsizexFP - startsizeFP)/aIters;
	TInt dyFP=(endsizeyFP - startsizeFP)/aIters;
	
	TInt actsizexFP=startsizeFP;
	TInt actsizeyFP=startsizeFP;

	iProfiler->InitResults();	
	for (TInt i=1;i<aIters;i++)
	{
		iGc->DrawBitmap(siz, bmpModeDep,VARSIZE);
		actsizexFP+=dxFP;
		actsizeyFP+=dyFP;
		iProfiler->MarkResultSetL();
	}

	iProfiler->ResultsAnalysis(_L("ScaledDrawBmpVarDest"), aRotation, aMode, aScreenMode, aIters);
		
	startsizeFP=1<<FPSHFT;
	endscale=50;
	
	endsizexFP= (endscale*iScreenDevice->SizeInPixels().iWidth)<<FPSHFT;
	endsizeyFP= (endscale*iScreenDevice->SizeInPixels().iHeight)<<FPSHFT;
	
	dxFP=(endsizexFP - startsizeFP)/aIters;
	dyFP=(endsizeyFP - startsizeFP)/aIters;
	
	actsizexFP=startsizeFP;
	actsizeyFP=startsizeFP;

	iProfiler->InitResults();	
	for (TInt i=1;i<aIters;i++)
	{
		iGc->DrawBitmap(VARSIZE, bmpModeDep, siz);
		actsizexFP+=dxFP;
		actsizeyFP+=dyFP;
		iProfiler->MarkResultSetL();
	}
	iProfiler->ResultsAnalysis(_L("ScaledDrawBmpVarSrc"), aRotation, aMode, aScreenMode, aIters);

	CFbsBitmap * greyMask = CreateSoftwareBitmapLC(iScreenDevice->SizeInPixels(), EGray256);
	VerticalGradientAlphaL(greyMask, TRgb(0xffffffff), TRgb(0x00000000));
	
	CFbsBitmap * bmpMask = CreateSoftwareBitmapLC(iScreenDevice->SizeInPixels(), EGray2);
	CopyBitmapL(bmpMask, greyMask);
			
	startsizeFP=1<<FPSHFT;
	endscale=1;
	
	endsizexFP= (endscale*iScreenDevice->SizeInPixels().iWidth)<<FPSHFT;
	endsizeyFP= (endscale*iScreenDevice->SizeInPixels().iHeight)<<FPSHFT;
	
	dxFP=(endsizexFP - startsizeFP)/aIters;
	dyFP=(endsizeyFP - startsizeFP)/aIters;
	
	actsizexFP=startsizeFP;
	actsizeyFP=startsizeFP;

	iProfiler->InitResults();
	for (TInt i=1;i<aIters;i++)
	{
		// Parameters: DrawBitmapMasked(const TRect &aDestRect, const CWsBitmap *aBitmap, const TRect &aSourceRect, const CWsBitmap *aMaskBitmap, TBool aInvertMask)
		iGc->DrawBitmapMasked(siz, bmpModeDep, VARSIZE, bmpMask, ETrue);
		actsizexFP+=dxFP;
		actsizeyFP+=dyFP;
		iProfiler->MarkResultSetL();
	}
	iProfiler->ResultsAnalysis(_L("ScaledDrawBmpMskdVarDest"), aRotation, aMode, aScreenMode, aIters);
	
	startsizeFP=1<<FPSHFT;
	endscale=50;
	
	endsizexFP= (endscale*iScreenDevice->SizeInPixels().iWidth)<<FPSHFT;
	endsizeyFP= (endscale*iScreenDevice->SizeInPixels().iHeight)<<FPSHFT;
	
	dxFP=(endsizexFP - startsizeFP)/aIters;
	dyFP=(endsizeyFP - startsizeFP)/aIters;
	
	actsizexFP=startsizeFP;
	actsizeyFP=startsizeFP;

	iProfiler->InitResults();
	for (TInt i=1;i<aIters;i++)
	{	
		iGc->DrawBitmapMasked(VARSIZE, bmpModeDep, siz, bmpMask,ETrue);
		actsizexFP+=dxFP;
		actsizeyFP+=dyFP;
		iProfiler->MarkResultSetL();
	}
	
	iProfiler->ResultsAnalysis(_L("ScaledDrawBmpMskdVarSrc"), aRotation, aMode, aScreenMode, aIters);

	CleanupStack::PopAndDestroy(4, bmpModeDep); // bmpMask, greyMask, sourceOverImage, bmpModeDep
	User::After(KDelay);
	}

CTScaledDrawBitmap::CTScaledDrawBitmap()
	{
	SetTestStepName(KTScaledDrawBitmap);
	}
	
/**
@return - TVerdict code
Override of base class pure virtual
Our implementation only gets called if the base class doTestStepPreambleL() did
not leave. That being the case, the current test result value will be EPass.
*/	
TVerdict CTScaledDrawBitmap::doTestStepL()
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
				SetTestStepID(_L(" GRAPHICS-UI-BENCH-0020"));

				// test drawing speed of scaled bitmaps
				for (TInt m=0; m<ModeCount; m++)
					DoTestScaledDrawBitmapL(orient, ModeArray[m], KValidDisplayModes[dispModeIndex], KIterationsToTest);
				RecordTestResultL();
				}
			}
		}
	return TestStepResult();
	}
	
