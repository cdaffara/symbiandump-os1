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
// trasterizerinvalid.h
// 
//

#include "trasterizerinvalid.h"

const TUid KUidInvalidInterface = {0xFFFFFFFF};

CTRasterizerInvalid::CTRasterizerInvalid()
	{
	SetTestStepName(KTRasterizerInvalidStep);
	}

/**
@SYMTestCaseID		
	GRAPHICS-FBSERV-RASTERIZER-010

@SYMTestPriority
	Normal

@SYMPREQ
	PREQ2096

@SYMREQ
	REQ10860
	REQ10861 
	
@SYMTestCaseDesc
	Test that CFbsRasterizer::GetInterface() returns the correct error code when
	passed a non-existent interface UID.

@SYMTestActions	
	Call CFbsRasterizer::GetInterface() with a UID of an interface that does not exist.

@SYMTestExpectedResults
	CFbsRasterizer::GetInterface() should return KErrExtensionNotSupported.
	The pointer to the in/out interface parameter should be set to NULL.
*/
void CTRasterizerInvalid::TestGetInterfaceL()
	{
	INFO_PRINTF1(_L("TestGetInterface"));		
	
	// Get the example rasterizer
	GetExampleRasterizerL();
	
	TAny* interfaceRet;
	TInt err = iRasterizer->GetInterface(KUidInvalidInterface, interfaceRet);
	TESTL(err == KErrExtensionNotSupported);
	TESTL(interfaceRet == NULL);
	}

/**
Override of base class pure virtual
Lists the tests to be run
*/
TVerdict CTRasterizerInvalid::doTestStepL()
	{		
	TestGetInterfaceL();
	
	return TestStepResult();
	}
