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
 
#ifndef TFLOWWINDOWSTESTOPENGLVG_H
#define TFLOWWINDOWSTESTOPENGLVG_H

#include "te_graphicsperformanceSuiteStepBase.h"

class CTFlowWindowsController;

/**
 *  CTFlowWindowsOpenGLVG
 * 
 */
class CTFlowWindowsTestOpenGLVG : public CTe_graphicsperformanceSuiteStepBase
	{
public:
	CTFlowWindowsTestOpenGLVG();
	virtual ~CTFlowWindowsTestOpenGLVG();

private:
	// From CTestStep
    virtual TVerdict doTestStepPreambleL();
	virtual TVerdict doTestStepL();
		
	void FlowOpenGLVGWindowsL();
	
private:
    TInt iIterationsToTest;
	CTFlowWindowsController* iFlowWindowsController;
	RArray<TPtrC> iFileNames;
	TBool iPreload;
	};

_LIT(KTFlowWindowsOpenGLVG, "tflowwindowstestopenglvg");

#endif // TFLOWWINDOWSOPENGLVG_H
