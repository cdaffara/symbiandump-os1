// Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
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
 
#ifndef __TSPRITEPERF_H__
#define __TSPRITEPERF_H__

#include "te_graphicsperformanceSuiteStepBase.h"
#include <w32std.h>
#include <w32adll.h>
#include "tspriteanimdll.h"

#define TEST_BITMAP_NAME _L("z:\\system\\data\\test.mbm")
class CTSpritePerf : public CTe_graphicsperformanceSuiteStepBase
	{
public:
	CTSpritePerf();
	~CTSpritePerf();
	enum TConstants
		{ENumWins = 5,};
private:
	void SpriteAnimTestL(const TDesC& aTestName, TBool aOverWholeScreen, TBool aTransparentMask);
	void SpriteAnimOverWholeScreenL();
	void SpriteAnimUnderTranslucentWindowsL();
	void SpriteAnimWithSemitransparentMaskOverWholeScreenL();
	void SpriteAnimWithSemitransparentMaskUnderTranslucentWindowsL();
	
	void FloatingSpriteTestL(const TDesC& aTestName, TBool aIsOpaqueSprite, TBool aOverlapUpdateArea);
	void OpaqueFloatingSpriteNonOverlapUpdateAreaL();
	void SemitransparentFloatingSpriteNonOverlapUpdateAreaL();
	void OpaqueFloatingSpriteOverlapUpdateAreaL();
	void SemitransparentFloatingSpriteOverlapUpdateAreaL();
	
	// From CTestStep
	TVerdict doTestStepPreambleL();
	TVerdict doTestStepPostambleL();
	TVerdict doTestStepL();

	void SetUpWindowEnvironmentL(RAnimDll* aAnimDll);
	void ConstructArrayOfWindowsL(TBool aUseTransparency);
	void ReleaseWindowsAndEnvironment();
		
	RWsSession iWs;
	CWsScreenDevice* iWsScreenDev; 
	RWindowGroup* iWinGroup;	
	RWindow* iWins[ENumWins];
	TRgb iBackColour[ENumWins];
	};
	
_LIT(KTSpritePerfName,"tspriteperf");

#endif
