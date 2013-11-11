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

#include "cteststepbtromconfigsdp001.h"
#include <btsdp.h>

CTestStepBtRomConfigSdp001::~CTestStepBtRomConfigSdp001()
	{
    }
	
CTestStepBtRomConfigSdp001::CTestStepBtRomConfigSdp001
	(CTestServer& aParent) 
	: CTestStepBtRomConfigBase(aParent)
	{
	SetTestStepName(KTestNameSdp001);
	}

/**
Static Constructor
Note the lack of ELeave. This means that having insufficient memory will return NULL;
*/
CTestStepBtRomConfigSdp001* CTestStepBtRomConfigSdp001::New
	(CTestServer& aParent)
	{
	return new CTestStepBtRomConfigSdp001(aParent); 
	}

/**
See BT_ROMCONFIG_SDP_001.script
*/
TVerdict CTestStepBtRomConfigSdp001::doTestStepL()
	{
	INFO_PRINTF1(\
		_L("&gt;&gt;CTestStepBtRomConfigSdp001::doTestStepL()"));
	const TInt expectedError = ( iBtExcluded ? KErrNotFound : KErrNone );
	// this is only used for logging readability purposes
	const TDesC* expectedErrorDesPtr = ( iBtExcluded ? &KErrNotFoundLit : &KErrNoneLit ); 
	
	RSdp sdp;
	TInt err = sdp.Connect();
	if ( err!=expectedError )
		{
		INFO_PRINTF4(\
			_L("Failed: Expected %S(%d) and got %d when calling RSdp::Connect"),\
			expectedErrorDesPtr, expectedError, err);
		SetTestStepResult(EFail);
		}
	
	if ( sdp.Handle() )	
		{
		sdp.Close();
		}
	INFO_PRINTF1(\
		_L("&lt;&lt;CTestStepBtRomConfigSdp001::doTestStepL()"));
	CheckAndSetTestResult();
	return TestStepResult(); 
	}

// EOF
