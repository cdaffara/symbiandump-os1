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

#include "cteststepbtromconfigremcon001.h"
#include <remconinterfaceselector.h>
#include <remconcoreapicontroller.h>

CTestStepBtRomConfigRemCon001::~CTestStepBtRomConfigRemCon001()
	{
    }
	
CTestStepBtRomConfigRemCon001::CTestStepBtRomConfigRemCon001
	(CTestServer& aParent) 
	: CTestStepBtRomConfigBase(aParent)
	{
	SetTestStepName(KTestNameRemCon001);
	}

/**
Static Constructor
Note the lack of ELeave. This means that having insufficient memory will return NULL;
*/
CTestStepBtRomConfigRemCon001* CTestStepBtRomConfigRemCon001::New
	(CTestServer& aParent)
	{
	return new CTestStepBtRomConfigRemCon001(aParent); 
	}

/**
See BT_ROMCONFIG_REMCON_001.script
*/
TVerdict CTestStepBtRomConfigRemCon001::doTestStepL()
	{
	INFO_PRINTF1(\
		_L("&gt;&gt;CTestStepBtRomConfigRemCon001::doTestStepL()"));
	
	CActiveScheduler* sched = new (ELeave)CActiveScheduler;
	CActiveScheduler::Install(sched);
	CleanupStack::PushL(sched);
	
	CRemConInterfaceSelector* remconSelector = CRemConInterfaceSelector::NewL();
	CleanupStack::PushL( remconSelector );
	
	CRemConCoreApiController* cont = CRemConCoreApiController::NewL(*remconSelector, *this);
	// ownership of cont is with remconSelector now
	
	const TInt expectedError = ( iBtExcluded ? KErrNotFound : KErrNone );
	// this is only used for logging readability purposes
	const TDesC* expectedErrorDesPtr = ( iBtExcluded ? &KErrNotFoundLit : &KErrNoneLit ); 
	
	TRAPD(err, remconSelector->OpenControllerL())
	if ( err!=expectedError )
		{
		INFO_PRINTF4(\
			_L("Failed: Expected %S(%d) and got %d when calling CRemConInterfaceSelector::OpenControllerL"),\
			expectedErrorDesPtr, expectedError, err);
		SetTestStepResult(EFail);
		}
	
	CleanupStack::PopAndDestroy(remconSelector);
	CleanupStack::PopAndDestroy(sched);
	INFO_PRINTF1(\
		_L("&lt;&lt;CTestStepBtRomConfigRemCon001::doTestStepL()"));
	CheckAndSetTestResult();
	return TestStepResult(); 
	}

// stubs
void CTestStepBtRomConfigRemCon001::MrccacoResponse(TRemConCoreApiOperationId /*aOperationId*/, TInt /*aError*/)
	{
	}
	
// EOF
