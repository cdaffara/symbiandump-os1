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
// This contains CommDb Unit Test Cases 024.XX

// EPOC includes
#include <e32base.h>
#include <commdb.h>
#include <d32comm.h>

// Test system includes
#include "Teststepcommdb.h"
#include "Step_024_xx.h"



/////////////////////
//	Test step 024.01
/////////////////////

// constructor
CCommDbTest024_01::CCommDbTest024_01()
	{
	// store the name of this test case
	SetTestStepName(_L("step_024_01"));
	}

// destructor
CCommDbTest024_01::~CCommDbTest024_01()
	{
	}


TVerdict CCommDbTest024_01::doTestStepPreambleL()
	{	
	// Have a temp copy of database
	openDbL();
        SetTestStepResult(EPass);	return TestStepResult();
	}

TInt CCommDbTest024_01::executeStepL()
	{
	TInt ret=KErrGeneral;

	//Now test the settings
	TUint32 value;
	TRAPD(r,iTheDb->GetCurrentDialOutSettingL( TPtrC(MODEM_BEARER),value));

	if(r==KErrNotSupported)
		{
		return KErrNotSupported;
		}

	return ret;
	}


TVerdict CCommDbTest024_01::doTestStepL( )
	{
	INFO_PRINTF1(_L("Step 024.03 called "));


	if ( executeStepL() == KErrNotSupported )
		SetTestStepResult(EPass);	else
		SetTestStepResult(EFail);
	return TestStepResult();
	}


/////////////////////
//	Test step 024.02
/////////////////////

// constructor
CCommDbTest024_02::CCommDbTest024_02()
	{
	// store the name of this test case
	SetTestStepName(_L("step_024_02"));
	}

// destructor
CCommDbTest024_02::~CCommDbTest024_02()
	{
	}


TVerdict CCommDbTest024_02::doTestStepPreambleL()
	{	
	openDbL();
        SetTestStepResult(EPass);	return TestStepResult();
	}

TInt CCommDbTest024_02::executeStepL()
	{
	TInt ret=KErrGeneral;

	//Now test the settings
	TUint32 value;
	TRAPD(r,iTheDb->GetCurrentDialOutSettingL( TPtrC(LOCATION), value ));

	if(r==KErrNotSupported)
		{
		return KErrNotSupported;
		}

	return ret;
	}


TVerdict CCommDbTest024_02::doTestStepL( )
	{
	INFO_PRINTF1(_L("Step 024.04 called "));


	if ( executeStepL() == KErrNotSupported )
		SetTestStepResult(EPass);	else
		SetTestStepResult(EFail);
	return TestStepResult();
	}




/////////////////////
//	Test step 024.03
/////////////////////

// constructor
CCommDbTest024_03::CCommDbTest024_03()
	{
	// store the name of this test case
	SetTestStepName(_L("step_024_03"));
	}

// destructor
CCommDbTest024_03::~CCommDbTest024_03()
	{
	}


TVerdict CCommDbTest024_03::doTestStepPreambleL()
	{	
	openDbL();
        SetTestStepResult(EPass);	return TestStepResult();
	}

TInt CCommDbTest024_03::executeStepL()
	{
	TInt ret=KErrGeneral;

	//Point to an IAP record
	iTheDb->SetGlobalSettingL( TPtrC(DEFAULT_NETWORK), 0 );

	//Now test the settings
	TUint32 value;
	TRAPD(r1,iTheDb->GetCurrentDialOutSettingL( TPtrC(IAP_SERVICE), value ));

	if(r1==KErrNotSupported)
		{
		return KErrNotSupported;
		}

	return ret;
	}


TVerdict CCommDbTest024_03::doTestStepL( )
	{
	INFO_PRINTF1(_L("Step 024.03 called "));


	if ( executeStepL() == KErrNotSupported )
		SetTestStepResult(EPass);	else
		SetTestStepResult(EFail);
	return TestStepResult();
	}



/////////////////////
//	Test step 024.04
/////////////////////

// constructor
CCommDbTest024_04::CCommDbTest024_04()
	{
	// store the name of this test case
	SetTestStepName(_L("step_024_04"));
	}

// destructor
CCommDbTest024_04::~CCommDbTest024_04()
	{
	}


TVerdict CCommDbTest024_04::doTestStepPreambleL()
	{	
	openDbL();
        SetTestStepResult(EPass);	return TestStepResult();
	}

TInt CCommDbTest024_04::executeStepL()
	{
	TInt ret=KErrGeneral;

	//Point to an IAP record
	iTheDb->SetGlobalSettingL( TPtrC(DEFAULT_NETWORK), 0 );

	//Now test the settings
	TUint32 value;
	TRAPD(r1,iTheDb->GetCurrentDialOutSettingL( TPtrC(IAP_NETWORK),value));

	if(r1==KErrNotSupported)
		{
		return KErrNotSupported;
		}

	return ret;
	}


TVerdict CCommDbTest024_04::doTestStepL( )
	{
	INFO_PRINTF1(_L("Step 024.02 called "));


	if ( executeStepL() == KErrNotSupported )
		SetTestStepResult(EPass);	else
		SetTestStepResult(EFail);
	return TestStepResult();
	}


/////////////////////
//	Test step 024.05
/////////////////////

// constructor
CCommDbTest024_05::CCommDbTest024_05()
	{
	// store the name of this test case
	SetTestStepName(_L("step_024_05"));
	}

// destructor
CCommDbTest024_05::~CCommDbTest024_05()
	{
	}


TVerdict CCommDbTest024_05::doTestStepPreambleL()
	{	
	openDbL();
        SetTestStepResult(EPass);	return TestStepResult();
	}

TInt CCommDbTest024_05::executeStepL()
	{
	TInt ret=KErrGeneral;

	//Now test the settings
	TUint32 value;
	TRAPD(r1,iTheDb->GetCurrentDialInSettingL( TPtrC(MODEM_BEARER), value));

	if(r1==KErrNotSupported)
		{
		return KErrNotSupported;
		}

	return ret;
	}


TVerdict CCommDbTest024_05::doTestStepL( )
	{
	INFO_PRINTF1(_L("Step 024.03 called "));


	if ( executeStepL() == KErrNotSupported )
		SetTestStepResult(EPass);	else
		SetTestStepResult(EFail);
	return TestStepResult();
	}



/////////////////////
//	Test step 024.06
/////////////////////

// constructor
CCommDbTest024_06::CCommDbTest024_06()
	{
	// store the name of this test case
	SetTestStepName(_L("step_024_06"));
	}

// destructor
CCommDbTest024_06::~CCommDbTest024_06()
	{
	}


TVerdict CCommDbTest024_06::doTestStepPreambleL()
	{	
	openDbL();
        SetTestStepResult(EPass);	return TestStepResult();
	}

TInt CCommDbTest024_06::executeStepL()
	{
	TInt ret=KErrGeneral;

	//Now test the settings
	TUint32 value;
	TRAPD(r1,iTheDb->GetCurrentDialInSettingL( TPtrC(LOCATION), value));

	if(r1==KErrNotSupported)
		{
		return KErrNotSupported;
		}

	return ret;
	}


TVerdict CCommDbTest024_06::doTestStepL( )
	{
	INFO_PRINTF1(_L("Step 024.04 called "));


	if ( executeStepL() == KErrNotSupported )
		SetTestStepResult(EPass);	else
		SetTestStepResult(EFail);
	return TestStepResult();
	}




/////////////////////
//	Test step 024.07
/////////////////////

// constructor
CCommDbTest024_07::CCommDbTest024_07()
	{
	// store the name of this test case
	SetTestStepName(_L("step_024_07"));
	}

// destructor
CCommDbTest024_07::~CCommDbTest024_07()
	{
	}


TVerdict CCommDbTest024_07::doTestStepPreambleL()
	{	
	openDbL();
        SetTestStepResult(EPass);	return TestStepResult();
	}

TInt CCommDbTest024_07::executeStepL()
	{
	TInt ret=KErrGeneral;

	//Point to an IAP record
	iTheDb->SetGlobalSettingL( TPtrC(DEFAULT_NETWORK), 0 );

	//Now test the settings
	TUint32 value;
	TRAPD(r1,iTheDb->GetCurrentDialInSettingL( TPtrC(IAP_SERVICE),value));

	if(r1==KErrNotSupported)
		{
		return KErrNotSupported;
		}

	return ret;
	}


TVerdict CCommDbTest024_07::doTestStepL( )
	{
	INFO_PRINTF1(_L("Step 024.01 called "));


	if ( executeStepL() == KErrNotSupported )
		SetTestStepResult(EPass);	else
		SetTestStepResult(EFail);
	return TestStepResult();
	}



/////////////////////
//	Test step 024.08
/////////////////////

// constructor
CCommDbTest024_08::CCommDbTest024_08()
	{
	// store the name of this test case
	SetTestStepName(_L("step_024_08"));
	}

// destructor
CCommDbTest024_08::~CCommDbTest024_08()
	{
	}


TVerdict CCommDbTest024_08::doTestStepPreambleL()
	{	
	openDbL();
        SetTestStepResult(EPass);	return TestStepResult();
	}

TInt CCommDbTest024_08::executeStepL()
	{
	TInt ret=KErrGeneral;

	//Point to an IAP record
	iTheDb->SetGlobalSettingL( TPtrC(DEFAULT_NETWORK), 0 );

	//Now test the settings
	TUint32 value;
	TRAPD(r1,iTheDb->GetCurrentDialInSettingL( TPtrC(IAP_SERVICE),value));

	if(r1==KErrNotSupported)
		{
		return KErrNotSupported;
		}

	return ret;
	}


TVerdict CCommDbTest024_08::doTestStepL( )
	{
	INFO_PRINTF1(_L("Step 024.02 called "));


	if ( executeStepL() == KErrNotSupported )
		SetTestStepResult(EPass);	else
		SetTestStepResult(EFail);
	return TestStepResult();
	}


/////////////////////
//	Test step 024.09
/////////////////////

// constructor
CCommDbTest024_09::CCommDbTest024_09()
	{
	// store the name of this test case
	SetTestStepName(_L("step_024_09"));
	}

// destructor
CCommDbTest024_09::~CCommDbTest024_09()
	{
	}

TVerdict CCommDbTest024_09::doTestStepL( )
	{
	INFO_PRINTF1(_L("Step 024.09 called "));

	SetTestStepResult(EPass);
		// Heap test for 024.01

	CCommDbTest024_01* step024_01 = new(ELeave) CCommDbTest024_01;
	CleanupStack::PushL(step024_01);
	if ( doTestStepWithHeapFailureL( *step024_01, KErrNotSupported) == EFail )
		SetTestStepResult(EFail);	CleanupStack::PopAndDestroy(step024_01);


		// Heap test for 024.02

	CCommDbTest024_02* step024_02 = new CCommDbTest024_02;
	CleanupStack::PushL(step024_02);
	if ( doTestStepWithHeapFailureL( *step024_02, KErrNotSupported) == EFail ) 
		SetTestStepResult(EFail);	CleanupStack::PopAndDestroy(step024_02);

		// Heap test for 024.03

	CCommDbTest024_03* step024_03 = new CCommDbTest024_03;
	CleanupStack::PushL(step024_03);
	if ( doTestStepWithHeapFailureL( *step024_03, KErrNotSupported) == EFail )
		SetTestStepResult(EFail);	CleanupStack::PopAndDestroy(step024_03);


		// Heap test for 024.04

	CCommDbTest024_04* step024_04 = new CCommDbTest024_04;
	CleanupStack::PushL(step024_04);
	if ( doTestStepWithHeapFailureL( *step024_04, KErrNotSupported) == EFail )
		SetTestStepResult(EFail);	CleanupStack::PopAndDestroy(step024_04);

		// Heap test for 024.05

	CCommDbTest024_05* step024_05 = new CCommDbTest024_05;
	CleanupStack::PushL(step024_05);
	if ( doTestStepWithHeapFailureL( *step024_05, KErrNotSupported) == EFail )
		SetTestStepResult(EFail);	CleanupStack::PopAndDestroy(step024_05);

		// Heap test for 024.06

	CCommDbTest024_06* step024_06 = new CCommDbTest024_06;
	CleanupStack::PushL(step024_06);
	if ( doTestStepWithHeapFailureL( *step024_06, KErrNotSupported) == EFail )
		SetTestStepResult(EFail);	CleanupStack::PopAndDestroy(step024_06);

		// Heap test for 024.07

	CCommDbTest024_07* step024_07 = new CCommDbTest024_07;
	CleanupStack::PushL(step024_07);
	if ( doTestStepWithHeapFailureL( *step024_07, KErrNotSupported) == EFail )
		SetTestStepResult(EFail);	CleanupStack::PopAndDestroy(step024_07);

	
		// Heap test for 024.08

	CCommDbTest024_08* step024_08 = new CCommDbTest024_08;
	CleanupStack::PushL(step024_08);
	if ( doTestStepWithHeapFailureL( *step024_08, KErrNotSupported) == EFail )
		SetTestStepResult(EFail);	CleanupStack::PopAndDestroy(step024_08);

	return TestStepResult();	

}
