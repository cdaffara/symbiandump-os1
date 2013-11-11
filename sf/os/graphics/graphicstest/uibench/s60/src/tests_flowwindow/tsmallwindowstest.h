// Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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
 
#ifndef TSMALLWINDOWSTEST_H
#define TSMALLWINDOWSTEST_H


#include "te_graphicsperformanceSuiteStepBase.h"

class CTFlowWindowsController;

/**
 *  CTSmallWindowsTest
 * 
 */
class CTSmallWindowsTest : public CTe_graphicsperformanceSuiteStepBase
	{
public:
	CTSmallWindowsTest();
	virtual ~CTSmallWindowsTest();

private:
	// From CTestStep
	virtual TVerdict doTestStepPreambleL();
	virtual TVerdict doTestStepL();
	
	void FlowWindowsL();
	void ComputeSmallWindows();
	
private:
    RArray<TPtrC> iFileNames;
    CTFlowWindowsController* iFlowWindowsController;
    TInt iIterationsToTest;
    TSize iScreenSize;
    TSize iWindowSize;
    TInt iWindowsAcross;
	};

_LIT(KTSmallWindows, "tsmallwindows");

#endif // TSMALLWINDOWSTEST_H
