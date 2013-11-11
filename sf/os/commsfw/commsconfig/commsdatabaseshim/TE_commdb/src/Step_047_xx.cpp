//
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
//
//
// This contains CommDb Unit Test Cases 046.XX

// EPOC includes
#include <e32base.h>
#include <commdb.h>
#include <d32comm.h>
#include <cdbstore.h>

// Test system includes
#include "Teststepcommdb.h"
#include "Step_047_xx.h"
#include <e32math.h>


/////////////////////
//	Test step 0047.001
/////////////////////

// constructor
CCommDbTest047_01::CCommDbTest047_01()
	{
	// store the name of this test case
	SetTestStepName(_L("step_047_01"));
	}

// destructor
CCommDbTest047_01::~CCommDbTest047_01()
	{
	}


TVerdict CCommDbTest047_01::doTestStepPreambleL()
	{
        SetTestStepResult(EPass);	return TestStepResult();
	}


TBool CCommDbTest047_01::executeStepL()
	{
	iOverSet=CCommDbOverrideSettings::NewL(CCommDbOverrideSettings::EParamListPartial,EDatabaseTypeIAP);
	return KErrNone;
	}

TVerdict CCommDbTest047_01::doTestStepL( )
	{
	TRAPD(ret,executeStepL());
	if(ret==KErrNone)
		{
         SetTestStepResult(EPass);		return TestStepResult();	//If we've got this far we've passed
		}
	else
		{
         SetTestStepResult(EFail);		return TestStepResult();
		}
	}

TVerdict CCommDbTest047_01::doTestStepPostambleL()
	{
	delete iOverSet;
        SetTestStepResult(EPass);	return TestStepResult();
	}

/////////////////////
//	Test step 0047.02
/////////////////////

// constructor
CCommDbTest047_02::CCommDbTest047_02()
	{
	// store the name of this test case
	SetTestStepName(_L("step_047_02"));
	}

// destructor
CCommDbTest047_02::~CCommDbTest047_02()
	{
	}


TVerdict CCommDbTest047_02::doTestStepPreambleL()
	{
        SetTestStepResult(EPass);	return TestStepResult();
	}


TBool CCommDbTest047_02::executeStepL()
	{
	CCommsDatabase::NewL(EDatabaseTypeIAP);
	return KErrNone; 
	} //lint !e550

TVerdict CCommDbTest047_02::doTestStepL( )
	{
	TRAPD(ret,executeStepL());
	if(ret==KErrNone)
		{
         SetTestStepResult(EPass);		return TestStepResult();	//If we've got this far we've passed
		}
	else
		{
         SetTestStepResult(EFail);		return TestStepResult();
		}
	}

TVerdict CCommDbTest047_02::doTestStepPostambleL()
	{
        SetTestStepResult(EPass);	return TestStepResult();
	}

/////////////////////
//	Test step 0047.03
/////////////////////

// constructor
CCommDbTest047_03::CCommDbTest047_03()
	{
	// store the name of this test case
	SetTestStepName(_L("step_047_03"));
	}

// destructor
CCommDbTest047_03::~CCommDbTest047_03()
	{
	}


TVerdict CCommDbTest047_03::doTestStepPreambleL()
	{	
        SetTestStepResult(EPass);	return TestStepResult();
	}


TBool CCommDbTest047_03::executeStepL()
	{
	CStoreableOverrideSettings::NewL(CStoreableOverrideSettings::EParamListPartial,EDatabaseTypeIAP);
	return KErrNone;
	} //lint !e550

TVerdict CCommDbTest047_03::doTestStepL( )
	{
	TRAPD(ret,executeStepL());
	if(ret==KErrNone)
		{
         SetTestStepResult(EPass);		return TestStepResult();	//If we've got this far we've passed
		}
	else
		{
         SetTestStepResult(EFail);		return TestStepResult();
		}
	}

TVerdict CCommDbTest047_03::doTestStepPostambleL()
	{
        SetTestStepResult(EPass);	return TestStepResult();
	}

/////////////////////
//	Test step 0047.04
/////////////////////

// constructor
CCommDbTest047_04::CCommDbTest047_04()
	{
	// store the name of this test case
	SetTestStepName(_L("step_047_04"));
	}

// destructor
CCommDbTest047_04::~CCommDbTest047_04()
	{
	}


TVerdict CCommDbTest047_04::doTestStepPreambleL()
	{	
	iTheDb=CCommsDatabase::NewL();
	iPrefView = iTheDb->OpenConnectionPrefTableLC();
	CleanupStack::Pop();

        SetTestStepResult(EPass);	return TestStepResult();
	}


TBool CCommDbTest047_04::executeStepL()
	{
	CCommsDbConnectionPrefTableView::TCommDbIspConnectionPref pref1;

	iPrefView->InsertConnectionPreferenceL(pref1, EFalse);
	return KErrNone;
	}

TVerdict CCommDbTest047_04::doTestStepL( )
	{
	TRAPD(ret,executeStepL());
	if(ret==KErrNotSupported)
		{
         SetTestStepResult(EPass);		return TestStepResult();	//If we've got this far we've passed
		}
	else
		{
         SetTestStepResult(EFail);		return TestStepResult();
		}
	}

TVerdict CCommDbTest047_04::doTestStepPostambleL()
	{
	delete iPrefView;
	iPrefView = NULL;
	delete iTheDb;
	iTheDb=NULL;

        SetTestStepResult(EPass);	return TestStepResult();
	}

/////////////////////
//	Test step 0047.05
/////////////////////

// constructor
CCommDbTest047_05::CCommDbTest047_05()
	{
	// store the name of this test case
	SetTestStepName(_L("step_047_05"));
	}

// destructor
CCommDbTest047_05::~CCommDbTest047_05()
	{
	}


TVerdict CCommDbTest047_05::doTestStepPreambleL()
	{	
	iTheDb=CCommsDatabase::NewL();
        SetTestStepResult(EPass);	return TestStepResult();
	}


TBool CCommDbTest047_05::executeStepL()
	{
	CCommDbOverrideSettings* overSet=CCommDbOverrideSettings::NewL(CCommDbOverrideSettings::EParamListPartial);
	CleanupStack::PushL(overSet);

	CCommsDbConnectionPrefTableView::TCommDbIspConnectionPref tempIspPref;

	User::LeaveIfError(overSet->GetConnectionPreferenceOverride(tempIspPref));
	CleanupStack::PopAndDestroy(overSet);
	return KErrNone;
	}

TVerdict CCommDbTest047_05::doTestStepL( )
	{
	TRAPD(ret,executeStepL());
	if(ret==KErrNotSupported)
		{
         SetTestStepResult(EPass);		return TestStepResult();	//If we've got this far we've passed
		}
	else
		{
         SetTestStepResult(EFail);		return TestStepResult();
		}
	}

TVerdict CCommDbTest047_05::doTestStepPostambleL()
	{
        SetTestStepResult(EPass);	return TestStepResult();
	}

/////////////////////
//	Test step 0047.06
/////////////////////
CCommDbTest047_06::CCommDbTest047_06()
	{
	// store the name of this test case
	SetTestStepName(_L("step_047_06"));
	}

CCommDbTest047_06::~CCommDbTest047_06()
	{}

TVerdict CCommDbTest047_06::doTestStepL( void )
	{
	TRAPD(ret,executeStepL());
	if(ret==KErrNotSupported)
		{
         SetTestStepResult(EPass);		return TestStepResult();	//If we've got this far we've passed
		}
	else
		{
         SetTestStepResult(EFail);		return TestStepResult();
		}
	}

TVerdict CCommDbTest047_06::doTestStepPreambleL()
	{
	iTheDb=CCommsDatabase::NewL();

	iPrefView = iTheDb->OpenConnectionPrefTableLC();
	CleanupStack::Pop();

        SetTestStepResult(EPass);	return TestStepResult();
	}

TInt CCommDbTest047_06::executeStepL()
	{
	CCommsDbConnectionPrefTableView::TCommDbIspConnectionPref pref;
	iPrefView->ReadConnectionPreferenceL(pref);
	return KErrNone;
	}
	
TVerdict CCommDbTest047_06::doTestStepPostambleL()
	{
	delete iPrefView;
	iPrefView = NULL;
	delete iTheDb;
	iTheDb=NULL;
        SetTestStepResult(EPass);	return TestStepResult();
	}

/////////////////////
//	Test step 0047.07
/////////////////////

// constructor
CCommDbTest047_07::CCommDbTest047_07()
	{
	// store the name of this test case
	SetTestStepName(_L("step_047_07"));
	}

// destructor
CCommDbTest047_07::~CCommDbTest047_07()
	{
	}


TVerdict CCommDbTest047_07::doTestStepPreambleL()
	{	
	iTheDb=CCommsDatabase::NewL();
        SetTestStepResult(EPass);	return TestStepResult();
	}


TBool CCommDbTest047_07::executeStepL()
	{
	CCommDbOverrideSettings* overSet=CCommDbOverrideSettings::NewL(CCommDbOverrideSettings::EParamListPartial);
	CleanupStack::PushL(overSet);

	CCommsDbConnectionPrefTableView::TCommDbIspConnectionPref tempIspPref;

	User::LeaveIfError(overSet->SetConnectionPreferenceOverride(tempIspPref));
	CleanupStack::PopAndDestroy(overSet);
	return KErrNone;
	}

TVerdict CCommDbTest047_07::doTestStepL( )
	{
	TRAPD(ret,executeStepL());
	if(ret==KErrNotSupported)
		{
         SetTestStepResult(EPass);		return TestStepResult();	//If we've got this far we've passed
		}
	else
		{
         SetTestStepResult(EFail);		return TestStepResult();
		}
	}

TVerdict CCommDbTest047_07::doTestStepPostambleL()
	{
        SetTestStepResult(EPass);	return TestStepResult();
	}

/////////////////////
//	Test step 0047.08
/////////////////////
CCommDbTest047_08::CCommDbTest047_08()
	{
	// store the name of this test case
	SetTestStepName(_L("step_047_08"));
	}

CCommDbTest047_08::~CCommDbTest047_08()
	{}

TVerdict CCommDbTest047_08::doTestStepL( void )
	{
	TRAPD(ret,executeStepL());
	if(ret==KErrNotSupported)
		{
         SetTestStepResult(EPass);		return TestStepResult();	//If we've got this far we've passed
		}
	else
		{
         SetTestStepResult(EFail);		return TestStepResult();
		}
	}

TVerdict CCommDbTest047_08::doTestStepPreambleL()
	{
	iTheDb=CCommsDatabase::NewL();

	iPrefView = iTheDb->OpenConnectionPrefTableLC();
	CleanupStack::Pop();

        SetTestStepResult(EPass);	return TestStepResult();
	}

TInt CCommDbTest047_08::executeStepL()
	{
	CCommsDbConnectionPrefTableView::TCommDbIspConnectionPref pref;
	iPrefView->UpdateBearerL(pref.iBearer);
	return KErrNone;
	}
	
TVerdict CCommDbTest047_08::doTestStepPostambleL()
	{
	delete iPrefView;
	iPrefView = NULL;
	delete iTheDb;
	iTheDb=NULL;
        SetTestStepResult(EPass);	return TestStepResult();
	}

/////////////////////
//	Test step 0047.13
/////////////////////

// constructor
CCommDbTest047_13::CCommDbTest047_13()
	{
	// store the name of this test case
	SetTestStepName(_L("step_047_13"));
	}

// destructor
CCommDbTest047_13::~CCommDbTest047_13()
	{
	}


TVerdict CCommDbTest047_13::doTestStepPreambleL()
	{	
        SetTestStepResult(EPass);	return TestStepResult();
	}


TBool CCommDbTest047_13::executeStepL()
	{
	TCommDbOpeningMethod openingMethod(ECommDbCreated);
	CCommsDatabase::NewL(EDatabaseTypeISP,openingMethod);
	return KErrNone;
	} //lint !e550

TVerdict CCommDbTest047_13::doTestStepL( )
	{
	TRAPD(ret,executeStepL());
	if(ret==KErrNone)
		{
         SetTestStepResult(EPass);		return TestStepResult();	//If we've got this far we've passed
		}
	else
		{
         SetTestStepResult(EFail);		return TestStepResult();
		}
	}

TVerdict CCommDbTest047_13::doTestStepPostambleL()
	{
        SetTestStepResult(EPass);	return TestStepResult();
	}
