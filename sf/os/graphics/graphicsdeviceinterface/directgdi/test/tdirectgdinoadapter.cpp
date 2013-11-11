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

#include "tdirectgdinoadapter.h"
#include <graphics/sgimage.h>
#include <graphics/directgdidriver.h>


CTNoAdapter::CTNoAdapter()
	{
	SetTestStepName(KTDirectGdiNoAdapter);
	}

CTNoAdapter::~CTNoAdapter()
	{	
	}

/**
@SYMTestCaseID
	GRAPHICS-DIRECTGDI-NOADAPTER-0001

@SYMPREQ 
	PREQ39

@SYMREQ
	REQ9229

@SYMTestCaseDesc
	Test drivers return errors when attemptiong to load adaptation when it isn't present.

@SYMTestStatus
	Implemented

@SYMTestPriority
	High

@SYMTestActions
	1.	Attempt to open SgDriver
	2.	Attempt to open DirectGDI Driver 
		
@SYMTestExpectedResults
	1.	SgDriver returns KErrNotSupported as the adaptation layer is not present.
	2.	DirectGDI Driver returns KErrNotSupoorted as the adaptation layer is not present.
 */
void CTNoAdapter::TestNoAdapter()
	{	
	SetTestStepID(_L("GRAPHICS-DIRECTGDI-NOADAPTER-0001"));
	_LIT(KTestName, "Test Calls to Open Drivers Return Error"); 
	INFO_PRINTF1(KTestName);

	TEST(KErrNotSupported == SgDriver::Open());
	TEST(KErrNotSupported == CDirectGdiDriver::Open());
	TRAPD(err,RecordTestResultL());
    	if (err!=KErrNone)
    		INFO_PRINTF1(_L("Failed to record test result"));
	}

/** 
Override of base class pure virtual
Our implementation only gets called if the base class doTestStepPreambleL() did
not leave. That being the case, the current test result value will be EPass.
@leave Gets system wide error code
@return TVerdict code
*/	
TVerdict CTNoAdapter::doTestStepL()
	{
#ifndef __WINS__
	TestNoAdapter();
#else
	INFO_PRINTF1(_L("These tests only run on hardware!"));
#endif
	return TestStepResult();
	}


