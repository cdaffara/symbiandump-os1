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
// This contains CommDb Unit Test Cases 021.XX

// EPOC includes
#include <e32base.h>
#include <commdb.h>
#include <d32comm.h>

// Test system includes
#include "Teststepcommdb.h"
#include "Step_021_xx.h"


/////////////////////
//	Test step 021.01
/////////////////////

// constructor
CCommDbTest021_01::CCommDbTest021_01()
	{
	// store the name of this test case
	SetTestStepName(_L("step_021_01"));
	}

// destructor
CCommDbTest021_01::~CCommDbTest021_01()
	{
	}


TVerdict CCommDbTest021_01::doTestStepPreambleL()
	{
	// Have a temp copy of database
	openDbL();
        SetTestStepResult(EPass);	return TestStepResult();
	}

TInt CCommDbTest021_01::executeStepL()
	{
	TInt ret=KErrGeneral;

	//Sets and verifies the client timeout

	//Set up the agent first
	_LIT(KAgent1,"Agent1");
	_LIT(KAgentExt,"Csd");
	TRAPD(r1,iTheDb->SetAgentL(TPtrC(DIAL_OUT_ISP),KAgent1));
	TRAPD(r2,iTheDb->SetAgentExtL(TPtrC(DIAL_OUT_ISP),KAgentExt));

	TUint timeout = 6;
	TInt value(0);

	TRAPD(r3,iTheDb->SetAgentClientTimeoutL(TPtrC(DIAL_OUT_ISP),timeout));
	TRAPD(r4,value = iTheDb->GetAgentClientTimeoutL(TPtrC(DIAL_OUT_ISP)));

	if(r1==KErrNotSupported && r2==KErrNotSupported && r3==KErrNotSupported
		&& (r4==KErrNotSupported || value==KErrNotSupported))
		{
		return KErrNotSupported;
		}
	
	return ret;
	}



TVerdict CCommDbTest021_01::doTestStepL( )
	{
	INFO_PRINTF1(_L("Step 021.01 called "));


	if ( executeStepL() == KErrNotSupported )
		SetTestStepResult(EPass);	else
		SetTestStepResult(EFail);
	return TestStepResult();
	}


/////////////////////
//	Test step 021.02
/////////////////////

// constructor
CCommDbTest021_02::CCommDbTest021_02()
	{
	// store the name of this test case
	SetTestStepName(_L("step_021_02"));
	}

// destructor
CCommDbTest021_02::~CCommDbTest021_02()
	{
	}


TVerdict CCommDbTest021_02::doTestStepPreambleL()
	{	
	openDbL();
        SetTestStepResult(EPass);	return TestStepResult();
	}

TInt CCommDbTest021_02::executeStepL()
	{
	TInt ret=KErrGeneral;

	//Try to set the timeout for an incorrect table

	//Set up the agent first
	_LIT(KAgent1,"Agent1");
	_LIT(KAgentExt,"Csd");
	TRAPD(r1,iTheDb->SetAgentL(TPtrC(DIAL_OUT_ISP),KAgent1));
	TRAPD(r2,iTheDb->SetAgentExtL(TPtrC(DIAL_OUT_ISP),KAgentExt));

	TUint timeout = 0x100;
	TRAPD(r3, iTheDb->SetAgentClientTimeoutL(TPtrC(LOCATION), timeout));

	if(r1==KErrNotSupported && r2==KErrNotSupported && r3==KErrNotSupported)
		{
		return KErrNotSupported;
		}
		
	return ret;
	}



TVerdict CCommDbTest021_02::doTestStepL( )
	{
	INFO_PRINTF1(_L("Step 021.02 called "));


	if ( executeStepL() == KErrNotSupported )
		SetTestStepResult(EPass);	else
		SetTestStepResult(EFail);
	return TestStepResult();
	}

/////////////////////
//	Test step 021.03
/////////////////////

// constructor
CCommDbTest021_03::CCommDbTest021_03()
	{
	// store the name of this test case
	SetTestStepName(_L("step_021_03"));
	}

// destructor
CCommDbTest021_03::~CCommDbTest021_03()
	{
	}


TVerdict CCommDbTest021_03::doTestStepPreambleL()
	{	
	openDbL();
        SetTestStepResult(EPass);	return TestStepResult();
	}

TInt CCommDbTest021_03::executeStepL()
	{
	TInt ret=KErrGeneral;

	//Sets and verifies the routet timeout

	//Set up the agent first
	_LIT(KAgent1,"Agent1");
	_LIT(KAgentExt,"Csd");
	TRAPD(r1,iTheDb->SetAgentL(TPtrC(DIAL_OUT_ISP),KAgent1));
	TRAPD(r2,iTheDb->SetAgentExtL(TPtrC(DIAL_OUT_ISP),KAgentExt));

	TUint timeout = 0x100;
	TInt value(0);

	TRAPD(r3,iTheDb->SetAgentRouteTimeoutL(TPtrC(DIAL_OUT_ISP),timeout));
	TRAPD(r4,value = iTheDb->GetAgentRouteTimeoutL(TPtrC(DIAL_OUT_ISP)));

	if(r1==KErrNotSupported && r2==KErrNotSupported && r3==KErrNotSupported
		&& (r4==KErrNotSupported || value==KErrNotSupported))
		{
		return KErrNotSupported;
		}
	
	return ret;
	}



TVerdict CCommDbTest021_03::doTestStepL( )
	{
	INFO_PRINTF1(_L("Step 021.03 called "));


	if ( executeStepL() == KErrNotSupported )
		SetTestStepResult(EPass);	else
		SetTestStepResult(EFail);
	return TestStepResult();
	}


/////////////////////
//	Test step 021.04
/////////////////////

// constructor
CCommDbTest021_04::CCommDbTest021_04()
	{
	// store the name of this test case
	SetTestStepName(_L("step_021_04"));
	}

// destructor
CCommDbTest021_04::~CCommDbTest021_04()
	{
	}


TVerdict CCommDbTest021_04::doTestStepPreambleL()
	{	
	openDbL();
        SetTestStepResult(EPass);	return TestStepResult();
	}

TInt CCommDbTest021_04::executeStepL()
	{
	TInt ret=KErrGeneral;

	//Try to set the timeout for an incorrect table

	//Set up the agent first
	_LIT(KAgent1,"Agent1");
	_LIT(KAgentExt,"Csd");
	TRAPD(r1,iTheDb->SetAgentL(TPtrC(DIAL_OUT_ISP),KAgent1));
	TRAPD(r2,iTheDb->SetAgentExtL(TPtrC(DIAL_OUT_ISP),KAgentExt));

	TUint timeout = 0x100;
	
	TRAPD(r3, iTheDb->SetAgentRouteTimeoutL(TPtrC(LOCATION), timeout) );

	if(r1==KErrNotSupported && r2==KErrNotSupported && r3==KErrNotSupported)
		{
		return KErrNotSupported;
		}
	
	return ret;
	}



TVerdict CCommDbTest021_04::doTestStepL( )
	{
	INFO_PRINTF1(_L("Step 021.04 called "));


	if ( executeStepL() == KErrNotSupported )
		SetTestStepResult(EPass);	else
		SetTestStepResult(EFail);
	return TestStepResult();
	}




/////////////////////
//	Test step 021.05
/////////////////////

// constructor
CCommDbTest021_05::CCommDbTest021_05()
	{
	// store the name of this test case
	SetTestStepName(_L("step_021_05"));
	}

// destructor
CCommDbTest021_05::~CCommDbTest021_05()
	{
	}

TVerdict CCommDbTest021_05::doTestStepL( )
	{
	INFO_PRINTF1(_L("Step 021.05 called "));

	SetTestStepResult(EPass);
		// Heap test for 021.01

	CCommDbTest021_01* step021_01 = new(ELeave) CCommDbTest021_01;
	CleanupStack::PushL(step021_01);
	if ( doTestStepWithHeapFailureL( *step021_01, KErrNotSupported) == EFail )
		SetTestStepResult(EFail);	CleanupStack::PopAndDestroy(step021_01);


		// Heap test for 021.02

	CCommDbTest021_02* step021_02 = new CCommDbTest021_02;
	CleanupStack::PushL(step021_02);
	if ( doTestStepWithHeapFailureL( *step021_02, KErrNotSupported) == EFail )
		SetTestStepResult(EFail);	CleanupStack::PopAndDestroy(step021_02);

		// Heap test for 021.03

	CCommDbTest021_03* step021_03 = new CCommDbTest021_03;
	CleanupStack::PushL(step021_03);
	if ( doTestStepWithHeapFailureL( *step021_03, KErrNotSupported) == EFail )
		SetTestStepResult(EFail);	CleanupStack::PopAndDestroy(step021_03);


		// Heap test for 021.04

	CCommDbTest021_04* step021_04 = new CCommDbTest021_04;
	CleanupStack::PushL(step021_04);
	if ( doTestStepWithHeapFailureL( *step021_04, KErrNotSupported) == EFail )
		SetTestStepResult(EFail);	CleanupStack::PopAndDestroy(step021_04);



	return TestStepResult();	

}
