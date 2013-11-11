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
// This contains CommDb Unit Test Cases 005.01 - 005.04

// EPOC includes
#include <e32base.h>
#include <commdb.h>


// Test system includes
#include "Teststepcommdb.h"
#include "step_005_xx.h"




/////////////////////
//	Test step 005.001
/////////////////////

// constructor
CCommDbTest005_01::CCommDbTest005_01()
	{
	// store the name of this test case
	SetTestStepName(_L("step_005_01"));
	}

// destructor
CCommDbTest005_01::~CCommDbTest005_01()
	{
	}


TVerdict CCommDbTest005_01::doTestStepPreambleL()
	{	
	openDbAndViewL(TPtrC(DIAL_IN_ISP));
        SetTestStepResult(EPass);	return TestStepResult();
	}


TBool CCommDbTest005_01::executeStepL()
	{
	TDbColType colType;
	TUint32 attrib;
	TBool ret=EFail;

	//NB this step is design NOT to leave. 

	//Read column type attibutes and check against the known true values
	iTheView->ReadTypeAttribL( TPtrC(ISP_DESCRIPTION), colType, attrib );
	if ( colType == EDbColText && attrib == 0 )
		ret = EPass;

	if ( ret == EPass )
		{
			iTheView->ReadTypeAttribL( TPtrC(ISP_LOGIN_SCRIPT), colType, attrib );
			if ( !(colType == EDbColLongText && attrib == 0) )
				ret = EFail;
		}

	if ( ret == EPass )
		{
			iTheView->ReadTypeAttribL( TPtrC(ISP_TYPE), colType, attrib );
			if ( !(colType == EDbColUint32 && attrib == 0) )
				ret = EFail;
		}

	if ( ret == EPass )
		{
			iTheView->ReadTypeAttribL( TPtrC(ISP_USE_LOGIN_SCRIPT), colType, attrib );
			if ( !(colType == EDbColBit && attrib == 0) )
				ret = EFail;
		}

	return ret;
	}

TVerdict CCommDbTest005_01::doTestStepL( )
	{
	INFO_PRINTF1(_L("Step 005.01 called "));
	
	if ( executeStep() )
		{
		SetTestStepResult(EFail);		}
	else
		{
		SetTestStepResult(EPass);		}
		
	return TestStepResult();
	}


/////////////////////
//	Test step 005.002
/////////////////////

// constructor
CCommDbTest005_02::CCommDbTest005_02()
	{
	// store the name of this test case
	SetTestStepName(_L("step_005_02"));
	}

// destructor
CCommDbTest005_02::~CCommDbTest005_02()
	{
	}

TVerdict CCommDbTest005_02::doTestStepPreambleL()
	{	
	openDbAndViewL(TPtrC(DIAL_OUT_ISP));
        SetTestStepResult(EPass);	return TestStepResult();
	}


TBool CCommDbTest005_02::executeStepL()
	{
	TDbColType colType;
	TUint32 attrib;
	TBool ret=EFail;


	//NB. This step is designed not to leave

	//Read attributes for columns, and check against known correct values
	iTheView->ReadTypeAttribL( TPtrC(COMMDB_ID), colType, attrib );

	if ( colType == EDbColUint32 && attrib == (TDbCol::EAutoIncrement|TDbCol::ENotNull) )
		ret = EPass;

	if ( ret == EPass )
		{
			iTheView->ReadTypeAttribL( TPtrC(COMMDB_NAME), colType, attrib );
			if ( !(colType == EDbColText && attrib == TDbCol::ENotNull) )
				ret = EFail;
		}

	return ret;
		
	}

TVerdict CCommDbTest005_02::doTestStepL( )
	{
	INFO_PRINTF1(_L("Step 005.02 called "));

	if ( executeStep() )
		{
		SetTestStepResult(EFail);		}
	else
		{
		SetTestStepResult(EPass);		}

	
	return TestStepResult();
	}


/////////////////////
//	Test step 005.003
/////////////////////

// constructor
CCommDbTest005_03::CCommDbTest005_03()
	{
	// store the name of this test case
	SetTestStepName(_L("step_005_03"));
}

// destructor
CCommDbTest005_03::~CCommDbTest005_03()
	{
	}

TVerdict CCommDbTest005_03::doTestStepPreambleL()
	{	
	openDbAndViewL(TPtrC(DIAL_OUT_ISP));
        SetTestStepResult(EPass);	return TestStepResult();
	}


TInt CCommDbTest005_03::executeStepL()
	{
	TDbColType colType;
	TUint32 attrib;

	//NB. This test should leave
	TRAPD(ret, iTheView->ReadTypeAttribL( KInvalidColumnName, colType, attrib ) );

	return ret;
	}

TVerdict CCommDbTest005_03::doTestStepL( )
	{
	INFO_PRINTF1(_L("Step 005.03 called "));

	SetTestStepResult(EFail);	
	if ( executeStep() == KErrNotFound )
		SetTestStepResult(EPass);
	return TestStepResult();
	}



/////////////////////
//	Test step 005.004
/////////////////////

// constructor
CCommDbTest005_04::CCommDbTest005_04()
	{
	// store the name of this test case
	SetTestStepName(_L("step_005_04"));
	}

// destructor
CCommDbTest005_04::~CCommDbTest005_04()
	{
	}



TVerdict CCommDbTest005_04::doTestStepL( )
	{
	INFO_PRINTF1(_L("Step 005.04 called "));
	
	SetTestStepResult(EPass);
	//Heap test for 005.01

	CCommDbTest005_01* step005_01 = new CCommDbTest005_01;
	CleanupStack::PushL(step005_01);
	doTestStepWithHeapFailureL( *step005_01, KErrNone);
	if( step005_01->TestStepResult() == EFail )
		SetTestStepResult(EFail);	CleanupStack::PopAndDestroy(step005_01);
		
	//Heap test for 005.02
	
	CCommDbTest005_02* step005_02 = new CCommDbTest005_02;
	CleanupStack::PushL(step005_02);
	doTestStepWithHeapFailureL( *step005_02, KErrNone);
	if( step005_02->TestStepResult() == EFail )
		SetTestStepResult(EFail);	CleanupStack::PopAndDestroy(step005_02);


	
	//Heap test for 005.03

	
	CCommDbTest005_03* step005_03 = new CCommDbTest005_03;
	CleanupStack::PushL(step005_03);
	doTestStepWithHeapFailureL( *step005_03, KErrNotFound);
	if( step005_03->TestStepResult() == EFail )
		SetTestStepResult(EFail);	CleanupStack::PopAndDestroy(step005_03);

	return TestStepResult();
	}
