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

#ifndef CTREDRAWSTOREPERF_H
#define CTREDRAWSTOREPERF_H

#include "te_graphicsperformanceSuiteStepBase.h"
#include <w32std.h>

enum TRedrawstoreTestCase
	{
	EFragmentStore,
	EFragmentStoreWithManyRects
	};

class CTRedrawstorePerf : public CTe_graphicsperformanceSuiteStepBase
	{
public:
	CTRedrawstorePerf();
	~CTRedrawstorePerf();

	// From CTestStep
	TVerdict doTestStepPreambleL();
	TVerdict doTestStepL();

private:
	void Init();
	void CreateRWindowL();
	void TestStoringL(const TDesC& aTestName, TRedrawstoreTestCase aTestCase);	
	void TestPlayingbackL(const TDesC& aTestName, TRedrawstoreTestCase aTestCase);
	void TestStoringAndPlayingbackL(const TDesC& aTestName, TRedrawstoreTestCase aTestCase);	
	void PopulateRedrawStore();
	void PopulateRedrawStoreWithManyRects();
	
	void TestStoreWithFewRectsL();
	void TestStoreWithManyRectsL();
	void TestPlaybackWithFewRectsL();
	void TestPlaybackWithManyRectsL();
	void TestStoreAndPlaybackWithFewRectsL();
	void TestStoreAndPlaybackWithManyRectsL();
	
private:
	RWsSession 			iWs;
	RWindowGroup 		iWinGroup;
	RWindow				iWin;
	CWsScreenDevice* 	iScreenDevice;
	CWindowGc* 			iGc;
	};

_LIT(KTRedrawstorePerf,"tredrawstoreperf");

#endif // CTREDRAWSTOREPERF_H
