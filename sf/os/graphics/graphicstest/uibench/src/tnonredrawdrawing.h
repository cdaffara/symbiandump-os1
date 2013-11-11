// Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef __GRAPHICSPERFORMANCENONREDRAWDRAWING_H__
#define __GRAPHICSPERFORMANCENONREDRAWDRAWING_H__

#include "te_graphicsperformanceSuiteStepBase.h"
#include <w32std.h>
#include <test/ttmsgraphicsstep.h>

class CTNonRedrawDrawing : public CTe_graphicsperformanceSuiteStepBase
	{
public:
	CTNonRedrawDrawing();
	~CTNonRedrawDrawing();

private:
	enum TTestCase
		{
		EUseOpaqueDraw,
		EUseTransparencyFactor
		};
	void NonRedrawDrawingL(TTestCase aTestCase);

	// From CTestStep
	virtual TVerdict doTestStepPreambleL();
	virtual TVerdict doTestStepL();

private:
	RWsSession 			iWsSession;
	RWindowGroup 		iWinGroup;
	RWindow				iBackgroundWindow;
	CWsScreenDevice* 	iScreenDevice;
	CWindowGc* 			iWindowGc;
	};

_LIT(KTNonRedrawDrawing,"tnonredrawdrawing");

#endif //__GRAPHICSPERFORMANCENONREDRAWDRAWING_H__
