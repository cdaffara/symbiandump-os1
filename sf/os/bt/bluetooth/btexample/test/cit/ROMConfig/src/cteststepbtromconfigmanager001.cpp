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

#include "cteststepbtromconfigmanager001.h"
#include <btmanclient.h>

CTestStepBtRomConfigManager001::~CTestStepBtRomConfigManager001()
	{
    }
	
CTestStepBtRomConfigManager001::CTestStepBtRomConfigManager001
	(CTestServer& aParent) 
	: CTestStepBtRomConfigBase(aParent)
	{
	SetTestStepName(KTestNameManager001);
	}

/**
Static Constructor
Note the lack of ELeave. This means that having insufficient memory will return NULL;
*/
CTestStepBtRomConfigManager001* CTestStepBtRomConfigManager001::New
	(CTestServer& aParent)
	{
	return new CTestStepBtRomConfigManager001(aParent); 
	}

/**
See BT_ROMCONFIG_MANAGER_001.script
*/
TVerdict CTestStepBtRomConfigManager001::doTestStepL()
	{
	INFO_PRINTF1(\
		_L("&gt;&gt;CTestStepBtRomConfigManager001::doTestStepL()"));
	
	const TInt expectedError = ( iBtExcluded ? KErrNotFound : KErrNone );
	// this is only used for logging readability purposes
	const TDesC* expectedErrorDesPtr = ( iBtExcluded ? &KErrNotFoundLit : &KErrNoneLit ); 
	
	RBTMan btman;
	TInt err = btman.Connect();
	if ( err!=expectedError )
		{
		INFO_PRINTF4(\
			_L("Failed: Expected %S(%d) and got %d when calling RBTMan::Connect"),\
			expectedErrorDesPtr, expectedError, err);
		SetTestStepResult(EFail);
		}
	
	if ( btman.Handle() )	
		{
		btman.Close();
		}
		
	INFO_PRINTF1(\
		_L("&lt;&lt;CTestStepBtRomConfigManager001::doTestStepL()"));
	CheckAndSetTestResult();
	return TestStepResult(); 
	}

// EOF
