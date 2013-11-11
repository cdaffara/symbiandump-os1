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
 
#ifndef TROTATETESTSTEP_H
#define TROTATETESTSTEP_H

#include "te_graphicsperformanceSuiteStepBase.h"

class CTFlowWindowsController;


/**
 * Rotates the screen to each of the four orientations and measures the amount
 * of time it took. Note: The wsini_rotate_test.ini must be manually copied to 
 * z:\system\data\wsini.ini before the test is run.
 * 
 */
class CTRotateTestStep : public CTe_graphicsperformanceSuiteStepBase
	{
public:
	CTRotateTestStep();
	virtual ~CTRotateTestStep();
	
private:
	// From CTestStep
	virtual TVerdict doTestStepPreambleL();
	virtual TVerdict doTestStepL();
	
	void RotateWindowsL();
    
private:	
    CTFlowWindowsController* iFlowWindowsController;
    RArray<TPtrC> iFileNames;
    TSize iScreenSize;
    };

_LIT( KTRotateTestStep, "trotate");

#endif // TROTATETESTSTEP_H
