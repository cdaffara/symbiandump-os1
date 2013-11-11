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

#include "cteststepbtromconfiguser001.h"

/** 
Panic string if an invalid test subcase is specified
Panic number is the invalid subcase
*/
_LIT(KInvalidTestPanicString, "Invalid Test subcase specified");

CTestStepBtRomConfigUser001::~CTestStepBtRomConfigUser001()
	{
	if ( iSocketServ.Handle() )
		{
		iSocketServ.Close();
		}
    }
	
CTestStepBtRomConfigUser001::CTestStepBtRomConfigUser001
	(CTestServer& aParent) 
	: CTestStepBtRomConfigBase(aParent)
	{
	SetTestStepName(KTestNameUser001);
	}

/**
Static Constructor
Note the lack of ELeave. This means that having insufficient memory will return NULL;
*/
CTestStepBtRomConfigUser001* CTestStepBtRomConfigUser001::New
	(CTestServer& aParent)
	{
	return new CTestStepBtRomConfigUser001(aParent); 
	}

/**
see BT_ROMCONFIG_USER_001.script
*/
TVerdict CTestStepBtRomConfigUser001::doTestStepL()
	{
	INFO_PRINTF1(\
		_L("&gt;&gt;CTestStepBtRomConfigUser001::doTestStepL()"));
	User::LeaveIfError( iSocketServ.Connect() );
	
	const TInt expectedError = ( iBtExcluded ? KErrBadName : KErrNone );
	// this is only used for logging readability purposes
	const TDesC* expectedErrorDesPtr = ( iBtExcluded ? &KErrBadNameLit : &KErrNoneLit );
	
	TRAPD(err, TestPhysicalLinksConstructorL(1));
	if ( err!=expectedError )
		{
		INFO_PRINTF4(\
			_L("Failed: Expected %S(%d) and got %d calling CBluetoothPhysicalLinks::NewL"),\
			expectedErrorDesPtr, expectedError, err);
		SetTestStepResult(EFail);
		}
	
	TRAPD(err2, TestPhysicalLinksConstructorL(2));
	if ( err2!=expectedError )
		{
		INFO_PRINTF4(\
			_L("Failed: Expected %S(%d) and got %d calling CBluetoothPhysicalLinks::NewLC"),\
			expectedErrorDesPtr, expectedError, err2);
		SetTestStepResult(EFail);
		}	
		
	iSocketServ.Close();
	INFO_PRINTF1(\
		_L("&lt;&lt;CTestStepBtRomConfigUser001::doTestStepL()"));
	CheckAndSetTestResult();
	return TestStepResult(); 
	}

void CTestStepBtRomConfigUser001::TestPhysicalLinksConstructorL(TInt aTestSubCase)
	{
	switch(aTestSubCase)
		{
		case 1:
			{
			CBluetoothPhysicalLinks* linksCase1 = 
				CBluetoothPhysicalLinks::NewL(*this, iSocketServ);
			delete linksCase1;
			}
			break;
			
		case 2:
			{
			CBluetoothPhysicalLinks* linksCase2 = 
				CBluetoothPhysicalLinks::NewLC(*this, iSocketServ);
			CleanupStack::PopAndDestroy(linksCase2);
			}
			break;
		
		default:
			SetTestStepResult(EFail);
			User::Panic(KInvalidTestPanicString, aTestSubCase);
			break;
		}
	}

// stubs
void CTestStepBtRomConfigUser001::HandleCreateConnectionCompleteL(TInt /*aErr*/)
	{	
	}
	
void CTestStepBtRomConfigUser001::HandleDisconnectCompleteL(TInt /*aErr*/)
	{
	}
	
void CTestStepBtRomConfigUser001::HandleDisconnectAllCompleteL(TInt /*aErr*/)
	{
	}
	
// EOF
