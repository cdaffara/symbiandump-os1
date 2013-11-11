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
 
#ifndef __TEVENTHANDLINGPERF_H__
#define __TEVENTHANDLINGPERF_H__

#include "te_graphicsperformanceSuiteStepBase.h"

class CTEventHandlingPerf : public CTe_graphicsperformanceSuiteStepBase
	{
public:
	CTEventHandlingPerf();
	~CTEventHandlingPerf();

	// From CTestStep
	virtual TVerdict doTestStepPreambleL();
	virtual TVerdict doTestStepPostambleL();
	
	virtual TVerdict doTestStepL();
	
private:
	};

_LIT(KEventHandlingPerfName,"teventhandlingperf");

#endif // __TEVENTHANDLINGPERF_H__
