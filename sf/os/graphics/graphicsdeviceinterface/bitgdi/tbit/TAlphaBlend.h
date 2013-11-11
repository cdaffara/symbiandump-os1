// Copyright (c) 2004-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef __TALPHABLEND_H__
#define __TALPHABLEND_H__

#include <bitdev.h>
#include "TGraphicsHarness.h"


class CTAlphaBlending : public CTGraphicsBase
	{
public:
	CTAlphaBlending(CTestStep* aStep);
	~CTAlphaBlending();

protected:
//from 	CTGraphicsStep
	virtual void RunTestCaseL(TInt aCurTestCase);

private:
	TInt CreateScreenDevice(TDisplayMode aDisplayMode, CFbsBitGc::TGraphicsOrientation aOrientation=CFbsBitGc::EGraphicsOrientationNormal);
	void DeleteScreenDevice();
	void DeleteGraphicsContext();

	void TestAlphaBlendingL();
	void DoAlphaBlendingTestsL(const TPoint& aOrigin,
							   const TPoint& aDestPt, 
							   const TRect& aSrcRc1, 
							   const TPoint& aScrPt2,
							   const TPoint& aAlphaPt);
	void DoAlphaBlendingTestL(CFbsBitmap* aScreenBmp, 
							  const CFbsBitmap* aSrcBmp, 
							  const CFbsBitmap* aAlphaBmp,
							  const TPoint& aDestPt, 
							  const TRect& aSrcRc1, 
							  const TPoint& aSrcPt2,
							  const TPoint& aAlphaPt);

	void CreateAlphaBlendingBitmapsLC(CFbsBitmap*& aScreenBmp, 
									  CFbsBitmap*& aSrcBmp, 
									  CFbsBitmap*& aAlphaBmp,
									  TDisplayMode aMode);
	void DestroyAlphaBlendingBitmaps(CFbsBitmap*& aScreenBmp, 
									 CFbsBitmap*& aSrcBmp, 
									 CFbsBitmap*& aAlphaBmp);

	void DoDrawBitmapTestL(TDisplayMode aTestDisplayMode);
	void DoDrawBitmapTestsL();
	void TestAlphaBlending2L();

	void TestAlphaBlendCorrect(TDisplayMode aScreenMode, TDisplayMode aBitmapMode);
	void TestAlphaBlendingPlotL();
	TUint32 GetRawPixel(CFbsBitmap* aBitmap, TPoint aPos);

private:
	CFbsScreenDevice* iDevice;
	CFbsBitGc* iGc;
	};



class CTAlphaBlendingStep : public CTGraphicsStep
	{
public:
	CTAlphaBlendingStep();
protected:	
	//from CTGraphicsStep
	virtual CTGraphicsBase* CreateTestL();
	virtual void TestSetupL();
	};

_LIT(KTAlphaBlendingStep,"TAlphaBlending");


#endif
