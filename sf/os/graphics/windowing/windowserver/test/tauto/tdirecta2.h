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

#ifndef __TDIRECTA2_H__
#define __TDIRECTA2_H__

#include "AUTO.H"
#include "TGraphicsHarness.h"

// This DSA test code only works when run using a screen that supports transparency.
class CTDirect2 : public CTWsGraphicsBase, public MDirectScreenAccess
	{
public:
	CTDirect2(CTestStep* aStep);
	~CTDirect2();
	void ConstructL();

	static TInt Timeout(TAny* aAbortTest);

	// from MAbortDirectScreenAccess
	void AbortNow(RDirectScreenAccess::TTerminationReasons aReason);
	// from MDirectScreenAccess
	void Restart(RDirectScreenAccess::TTerminationReasons aReason);

protected:
	//from CTGraphicsStep
	void RunTestCaseL(TInt aCurTestCase);

private:
	void StartTranslucentWindowOverDsaL();
	void HandleTimeout();

private:
	CWsScreenDevice* iScreenDevice;
	CTWindowGroup* iWindowGroup;
	CTBlankWindow* iUnderWindow; // window to perform DSA on
	CTTitledWindow* iOverWindow; // translucent window to overlap DSA window
	CDirectScreenAccess* iDsa;
	CPeriodic* iTimer;
	TBool iTestCaseComplete;
	TBool iOnceOnly;
	};

class CTDirect2Step : public CTGraphicsStep
	{
public:
	CTDirect2Step();
protected:
	//from CTGraphicsStep
	virtual CTGraphicsBase* CreateTestL();
	};

_LIT(KTDirect2Step,"TDirect2");

#endif
