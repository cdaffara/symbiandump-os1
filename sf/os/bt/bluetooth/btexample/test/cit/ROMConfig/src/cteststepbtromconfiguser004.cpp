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

#include "cteststepbtromconfiguser004.h"
#include <es_sock.h>
#include <bt_sock.h>

#ifdef SYMBIAN_ENABLE_SPLIT_HEADERS
#include <bthci.h>
#endif

CTestStepBtRomConfigUser004::~CTestStepBtRomConfigUser004()
	{
    }	

CTestStepBtRomConfigUser004::CTestStepBtRomConfigUser004
	(CTestServer& aParent) 
	: CTestStepBtRomConfigBase(aParent)
	{
	SetTestStepName(KTestNameUser004);
	}

/**
Static Constructor
Note the lack of ELeave. This means that having insufficient memory will return NULL;
*/
CTestStepBtRomConfigUser004* CTestStepBtRomConfigUser004::New
	(CTestServer& aParent)
	{
	return new CTestStepBtRomConfigUser004(aParent); 
	}

/**
See BT_ROMCONFIG_USER_004.script
*/
TVerdict CTestStepBtRomConfigUser004::doTestStepL()
	{
	INFO_PRINTF1(\
		_L("&gt;&gt;CTestStepBtRomConfigUser004::doTestStepL()"));
	RSocketServ socketServer;
	User::LeaveIfError( socketServer.Connect() );
	
	const TInt expectedError = ( iBtExcluded ? KErrBadName : KErrNone );
	// this is only used for logging readability purposes
	const TDesC* expectedErrorDesPtr = ( iBtExcluded ? &KErrBadNameLit : &KErrNoneLit ); 
	
	RHCIDirectAccess access;
	TInt err = access.Open(socketServer);
	if ( err!=expectedError )
		{
		INFO_PRINTF4(\
			_L("Failed: Expected %S(%d) and got %d calling RHCIDirectAccess::Open"),
			expectedErrorDesPtr, expectedError, err);
		SetTestStepResult(EFail);
		}
	
	if ( access.SubSessionHandle() )
		{
		access.Close();
		}
	socketServer.Close();
	
	INFO_PRINTF1(\
		_L("&lt;&lt;CTestStepBtRomConfigUser004::doTestStepL()"));
	CheckAndSetTestResult();
	return TestStepResult(); 
	}

// EOF
