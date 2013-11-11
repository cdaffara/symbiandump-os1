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
 
#ifndef __TSCROLLINGPERF_H__
#define __TSCROLLINGPERF_H__

#include "te_graphicsperformanceSuiteStepBase.h"
#include <w32std.h>

class CScrollingPerf : public CTe_graphicsperformanceSuiteStepBase
	{
public:
	CScrollingPerf();
	~CScrollingPerf();

private:
	TBool SetScreenModeL(TDisplayMode aScreenMode);
	void ScrollRWindowL();
	void MoveClippedChildWindowL();
	
	// From CTestStep
	virtual TVerdict doTestStepPreambleL();
	virtual TVerdict doTestStepL();
	
private:
	RWsSession iWs;
	CWsScreenDevice* iWsScreenDevice;
	CWindowGc* iWGc;
	TSize iScreenSize;
	CFbsBitmap* iBitmap32bit;
	};

_LIT(KScrollingPerfName,"tscrollingperf");

#endif // __TSCROLLINGPERF_H__
