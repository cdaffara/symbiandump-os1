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
// mecunitteststepbase.cpp
//

/**
@file
*/


#include "mecunitteststepbase.h"
#include <comms-infras/ss_rmetaextensioncontainer.h>

CMecUnitTestStepBase::~CMecUnitTestStepBase()
    {
    }



TVerdict CMecUnitTestStepBase::doTestStepL()
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


void CMecUnitTestStepBase::CreateAndAppendExtensionL(ESock::RMetaExtensionContainer& aMec, TUint32 aExtensionType, TUint32 aVersion)
	{
	Meta::SMetaData* ext = NULL;
	
	switch (aExtensionType)
		{
		case TTestExtension1::ETypeId:
			ext = new(ELeave)TTestExtension1(aVersion);
			break;

		case TTestExtension2::ETypeId:
			ext = new(ELeave)TTestExtension2(aVersion);
			break;
			
		case TTestExtension3::ETypeId:
			ext = new(ELeave)TTestExtension3(aVersion);
			break;
			
		default:
			User::Leave(KErrArgument);
		}
	
	CleanupStack::PushL(ext);
	aMec.AppendExtensionL(ext);
	CleanupStack::Pop(ext);
	}



