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

#include "cteststepbtromconfigmanager002.h"
#include <btmanclient.h>

CTestStepBtRomConfigManager002::~CTestStepBtRomConfigManager002()
	{
    }
	
CTestStepBtRomConfigManager002::CTestStepBtRomConfigManager002
	(CTestServer& aParent) 
	: CTestStepBtRomConfigBase(aParent)
	{
	SetTestStepName(KTestNameManager002);
	}

/**
Static Constructor
Note the lack of ELeave. This means that having insufficient memory will return NULL;
*/
CTestStepBtRomConfigManager002* CTestStepBtRomConfigManager002::New
	(CTestServer& aParent)
	{
	return new CTestStepBtRomConfigManager002(aParent); 
	}

/**
See BT_ROMCONFIG_MANAGER_002.script
*/
TVerdict CTestStepBtRomConfigManager002::doTestStepL()
	{
	INFO_PRINTF1(\
		_L("&gt;&gt;CTestStepBtRomConfigManager002::doTestStepL()"));

	const TInt expectedError = ( iBtExcluded ? KErrNotFound : KErrNone );
	// this is only used for logging readability purposes
	const TDesC* expectedErrorDesPtr = ( iBtExcluded ? &KErrNotFoundLit : &KErrNoneLit ); 
		
	RBTRegServ btregserv;
	TInt err = btregserv.Connect();
	if ( err!=expectedError )
		{
		INFO_PRINTF4(\
			_L("Failed: Expected %S(%d) and got %d when calling RBTRegServ::Connect"),\
			expectedErrorDesPtr, expectedError, err);
		SetTestStepResult(EFail);
		}
	btregserv.Close(); // ideally we should check we have a handle, but it's private/protected?
		
	INFO_PRINTF1(\
		_L("&lt;&lt;CTestStepBtRomConfigManager002::doTestStepL()"));
	CheckAndSetTestResult();
	return TestStepResult(); 
	}

// EOF
