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

#include "cteststepbtromconfiguser003.h"

/** 
Panic string if an invalid test subcase is specified
Panic number is the invalid subcase
*/
_LIT(KInvalidTestPanicString, "Invalid Test subcase specified");

CTestStepBtRomConfigUser003::~CTestStepBtRomConfigUser003()
	{
	if ( iSocketServ.Handle() )
		{
		iSocketServ.Close();
		}
    }	

CTestStepBtRomConfigUser003::CTestStepBtRomConfigUser003
	(CTestServer& aParent) 
	: CTestStepBtRomConfigBase(aParent)
	{
	SetTestStepName(KTestNameUser003);
	}

/**
Static Constructor
Note the lack of ELeave. This means that having insufficient memory will return NULL;
*/
CTestStepBtRomConfigUser003* CTestStepBtRomConfigUser003::New
	(CTestServer& aParent)
	{
	return new CTestStepBtRomConfigUser003(aParent); 
	}

/**
See BT_ROMCONFIG_USER_003.script
*/
TVerdict CTestStepBtRomConfigUser003::doTestStepL()
	{
	INFO_PRINTF1(\
		_L("&gt;&gt;CTestStepBtRomConfigUser003::doTestStepL()"));
	User::LeaveIfError( iSocketServ.Connect() );

	CActiveScheduler* sched = new (ELeave)CActiveScheduler;
	CActiveScheduler::Install(sched);
	CleanupStack::PushL(sched);
	
	const TInt expectedError = ( iBtExcluded ? KErrBadName : KErrNone );
	// this is only used for logging readability purposes
	const TDesC* expectedErrorDesPtr = ( iBtExcluded ? &KErrBadNameLit : &KErrNoneLit );
	
	TRAPD(err, TestSynchronousLinkConstructorL(1));
	if ( err!=expectedError )
		{
		INFO_PRINTF4(\
			_L("Failed: Expected %S(%d) and got %d calling CBluetoothSynchronousLink::NewL"),\
			expectedErrorDesPtr, expectedError, err);
		SetTestStepResult(EFail);
		}
	
	TRAPD(err2, TestSynchronousLinkConstructorL(2));
	if ( err2!=expectedError )
		{
		INFO_PRINTF4(\
			_L("Failed: Expected %S(%d) and got %d calling CBluetoothSynchronousLink::NewLC"),\
			expectedErrorDesPtr, expectedError, err2);
		SetTestStepResult(EFail);
		}	
		
	iSocketServ.Close();
	
	CleanupStack::PopAndDestroy(sched);
	INFO_PRINTF1(\
		_L("&lt;&lt;CTestStepBtRomConfigUser003::doTestStepL()"));
	CheckAndSetTestResult();
	return TestStepResult(); 
	}

void CTestStepBtRomConfigUser003::TestSynchronousLinkConstructorL(TInt aTestSubCase)
	{
	switch(aTestSubCase)
		{
		case 1:
			{
			CBluetoothSynchronousLink* linkCase1 = 
			CBluetoothSynchronousLink::NewL(*this, iSocketServ);
			CleanupStack::PushL(linkCase1);
			User::LeaveIfError(linkCase1->AcceptConnection());	
			CleanupStack::PopAndDestroy(linkCase1);
			}
			break;
			
		case 2:
			{
			CBluetoothSynchronousLink* linkCase2 = 
			CBluetoothSynchronousLink::NewLC(*this, iSocketServ);
			User::LeaveIfError(linkCase2->AcceptConnection());					
			CleanupStack::PopAndDestroy(linkCase2);	
			}
			break;
		
		default:
			SetTestStepResult(EFail);
			User::Panic(KInvalidTestPanicString, aTestSubCase);
			break;
		}
	}

// stubs from MBluetoothSynchronousLinkNotifier
void CTestStepBtRomConfigUser003::HandleSetupConnectionCompleteL(TInt /*aErr*/)
	{
	}
	
void CTestStepBtRomConfigUser003::HandleDisconnectionCompleteL(TInt /*aErr*/)
	{
	}
	
void CTestStepBtRomConfigUser003::HandleAcceptConnectionCompleteL(TInt /*aErr*/)
	{
	}
	
void CTestStepBtRomConfigUser003::HandleSendCompleteL(TInt /*aErr*/)
	{
	}
	
void CTestStepBtRomConfigUser003::HandleReceiveCompleteL(TInt /*aErr*/)
	{
	}
	
// EOF
