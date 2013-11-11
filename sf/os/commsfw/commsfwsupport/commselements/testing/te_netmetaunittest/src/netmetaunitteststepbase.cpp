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
// netmetaunitteststepbase.cpp
//

/**
@file
*/


#include "netmetaunitteststepbase.h"


CNetMetaUnitTestStepBase::~CNetMetaUnitTestStepBase()
    {
    }



TVerdict CNetMetaUnitTestStepBase::doTestStepL()
    {
    SetTestStepResult(EFail);

    TVerdict testResult = EFail;
	TRAPD(err, testResult = RunTestStepL());
	if (err == KErrNone)
	    {
    	SetTestStepResult(testResult);
	    }
	else
	    {
	    ERR_PRINTF2(_L("Test step failed with Leave(%d)"), err);
	    }
	
	return TestStepResult();
    }


