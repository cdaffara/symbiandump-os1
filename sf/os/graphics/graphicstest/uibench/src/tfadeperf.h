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
 
#ifndef __TFADEPERF_H__
#define __TFADEPERF_H__

#include "te_graphicsperformanceSuiteStepBase.h"
#include <w32std.h>

class CTFadePerf : public CTe_graphicsperformanceSuiteStepBase
	{
public:
	CTFadePerf();
	~CTFadePerf();
	
private:
	void FadeOpaqueSingleWindowRepeatedlyL();
	void FadeTransparentSingleWindowRecordFadeL();
	void FadeTransparentSingleWindowRecordUnfadeL();
	void FadeOpaqueSingleWindowRecordFadeL();
	void FadeOpaqueSingleWindowRecordUnfadeL();
	
	void FadeOpaqueWindowsBehindRepeatedlyL();
	void FadeTransparentWindowsBehindRecordFadeL();
	void FadeTransparentWindowsBehindRecordUnfadeL();
	void FadeOpaqueWindowsBehindRecordFadeL();
	void FadeOpaqueWindowsBehindRecordUnfadeL();
	
	void SetSystemFadedRepeatedlyL();
	void SetSystemFadedTransparentWindowsRecordFadeL();
	void SetSystemFadedTransparentWindowsRecordUnfadeL();
	void SetSystemFadedOpaqueWindowsRecordFadeL();
	void SetSystemFadedOpaqueWindowsRecordUnfadeL();
	
	void SetNonFadingRepeatedlyL();
	void SetNonFadingTransparentWindowsRecordFadeL();
	void SetNonFadingTransparentWindowsRecordUnfadeL();
	void SetNonFadingOpaqueWindowsRecordFadeL();
	void SetNonFadingOpaqueWindowsRecordUnfadeL();
	
	// From CTestStep
	TVerdict doTestStepPreambleL();
	TVerdict doTestStepPostambleL();
	TVerdict doTestStepL();
	
	void SetUpWindowEnvironmentL();
	void ConstructSingleWindowL(TBool aUseTransparency);
	void ConstructArrayOfWindowsL(TBool aUseTransparency, TBool aNonFadeHalfWindows);
	void ReleaseWindowsAndEnvironment(TBool aIsSingleWindow);
	
	enum TConstants
		{ENumWins = 40,};
	RWsSession iWs;
	CWsScreenDevice* iWsScreenDev; 
	RWindowGroup* iWinGroup;
	RWindow* iWins[ENumWins];
	RWindow* iSingleWin;
	};

_LIT(KTFadePerfName,"tfadeperf");

#endif
