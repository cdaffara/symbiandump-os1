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
 @internalComponent
*/

#ifndef T_WSERVRATELIMITERTESTSTEP_H_
#define T_WSERVRATELIMITERTESTSTEP_H_

#include <fbs.h>
#include <w32std.h>
#include <test/testexecutestepbase.h>
#include "../tratelimitdrawer/wsratelimitdrawer.h"

const TUint32 KNullWsHandle = 0xFFFFFFFF;

/**
  A main interface for implementation test cases
 */
class MTestCases
	{
public:
	virtual void RunTestCaseL(TInt aCurTestCase) = 0;
	};

class CTestManager;

class CT_WServRateLimiterTestStep : public CTestStep, public MTestCases
	{
public:
	CT_WServRateLimiterTestStep();
	~CT_WServRateLimiterTestStep();
	// from CTestStep
	virtual enum TVerdict doTestStepL();
	void RunTestCaseL(TInt aCurTestCase);
	
private:
	void CreateRWindowL(const RWindowGroup& aWinGroup, RWindow& aWin, const TPoint& aPos, const TRgb& aBkgdColor, const TSize& aWinSize,  const TUint32 aHandle = KNullWsHandle );
	void DrawGraphic(RWindow& aWin, CWsGraphic* aGraphic, CWindowGc* aGc);
	// Test cases
	void TestRateLimitedAnimationL();
	TBool IsRunningAtExpectedFrameRate(TReal aActualFrameRate, TReal aExpectedFrameRate, TReal aErrorThreshHold);

private:	// Data members
	TInt					iTestId;
	TDisplayMode			iDisplayMode;
	CWindowGc* 				iGc;
	CWindowGc* 				iGc2;
	CWsScreenDevice* 		iScreen;
	CWsScreenDevice* 		iScreen1;
	RWsSession 				iWsSession;
	RWindowGroup 			iWinGroup;
	RWindowGroup 			iWinGroup1;
	CTestManager* 			iTestManager;
	TBool					iTwoScreens;
	};

#endif /*T_WSERVRATELIMITERTESTSTEP_H_*/
