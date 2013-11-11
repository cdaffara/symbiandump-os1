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

#include "cteststepbtromconfigrcommserv001.h"
#include <c32comm.h>

_LIT(KBtComm,"BTCOMM");

CTestStepBtRomConfigRCommServ001::~CTestStepBtRomConfigRCommServ001()
	{
    }	

CTestStepBtRomConfigRCommServ001::CTestStepBtRomConfigRCommServ001
	(CTestServer& aParent) 
	: CTestStepBtRomConfigBase(aParent)
	{
	SetTestStepName(KTestNameRCommServ001);
	}

/**
Static Constructor
Note the lack of ELeave. This means that having insufficient memory will return NULL;
*/
CTestStepBtRomConfigRCommServ001* CTestStepBtRomConfigRCommServ001::New
	(CTestServer& aParent)
	{
	return new CTestStepBtRomConfigRCommServ001(aParent); 
	}

/**
See BT_ROMCONFIG_RCOMMSERV_001.script
*/
TVerdict CTestStepBtRomConfigRCommServ001::doTestStepL()
	{
	INFO_PRINTF1(\
		_L("&gt;&gt;CTestStepBtRomConfigRCommServ001::doTestStepL()"));
	const TInt expectedError = ( iBtExcluded ? KErrNotFound : KErrNone );
	// this is only used for logging readability purposes
	const TDesC* expectedErrorDesPtr = ( iBtExcluded ? &KErrNotFoundLit : &KErrNoneLit ); 
	
	RCommServ commServ;
	commServ.Connect();
	
	TInt err = commServ.LoadCommModule( KBtComm );
	if ( err!=expectedError )
		{
		INFO_PRINTF4(\
			_L("Failed: Expected %S(%d) and got %d when calling LoadCommModule(\"BTCOMM\")"),\
			expectedErrorDesPtr, expectedError, err);
		SetTestStepResult(EFail);
		}
	
	commServ.Close();

	INFO_PRINTF1(\
		_L("&lt;&lt;CTestStepBtRomConfigRCommServ001::doTestStepL()"));
	CheckAndSetTestResult();
	return TestStepResult(); 
	}

// EOF
