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
// This contains CommDb Unit Test Cases 020.XX

// EPOC includes
#include <e32base.h>
#include <commdb.h>
#include <d32comm.h>

// Test system includes
#include "Teststepcommdb.h"
#include "Step_020_xx.h"

/////////////////////
//	Test step 020.01
/////////////////////

// constructor
CCommDbTest020_01::CCommDbTest020_01()
	{
	// store the name of this test case
	SetTestStepName(_L("step_020_01"));
	}

// destructor
CCommDbTest020_01::~CCommDbTest020_01()
	{
	}


TVerdict CCommDbTest020_01::doTestStepPreambleL()
	{
	openDbL();
        SetTestStepResult(EPass);	return TestStepResult();
	}

TInt CCommDbTest020_01::executeStepL()
	{
	TInt ret=KErrGeneral;
	TBuf<64> buffer;

	_LIT(KAgent,"Agent");

	TRAPD(r1,iTheDb->SetAgentL(TPtrC(DIAL_OUT_ISP),KAgent));
	TRAPD(r2,iTheDb->GetAgentL(TPtrC(DIAL_OUT_ISP),buffer));

	if(r1==KErrNotSupported && r2==KErrNotSupported)
		{
		return KErrNotSupported;
		}
	
	return ret;
	}



TVerdict CCommDbTest020_01::doTestStepL( )
	{
	INFO_PRINTF1(_L("Step 020.01 called "));


	if ( executeStepL() == KErrNotSupported )
		SetTestStepResult(EPass);	else
		SetTestStepResult(EFail);
	return TestStepResult();
	}

/////////////////////
//	Test step 020.02
/////////////////////

// constructor
CCommDbTest020_02::CCommDbTest020_02()
	{
	// store the name of this test case
	SetTestStepName(_L("step_020_02"));
	}

// destructor
CCommDbTest020_02::~CCommDbTest020_02()
	{
	}


TVerdict CCommDbTest020_02::doTestStepPreambleL()
	{	
	openDbL();
        SetTestStepResult(EPass);	return TestStepResult();
	}


TInt CCommDbTest020_02::executeStepL()
	{
	TInt ret=KErrGeneral;
	TBuf<64> buffer;

	_LIT(KAgent1,"Agent1");
	_LIT(KAgentExt,"Csd");

	TRAPD(r1,iTheDb->SetAgentL(TPtrC(DIAL_OUT_ISP),KAgent1));

	TRAPD(r2,iTheDb->SetAgentExtL(TPtrC(DIAL_OUT_ISP),KAgentExt));
	TRAPD(r3,iTheDb->GetAgentExtL(TPtrC(DIAL_OUT_ISP),buffer));
	if(r3 != KErrNone)
		{
		return r3;
		}

	if(r1==KErrNotSupported && r2==KErrNotSupported)
		{
		return KErrNotSupported;
		}
	
	return ret;
	}



TVerdict CCommDbTest020_02::doTestStepL( )
	{
	INFO_PRINTF1(_L("Step 020.02 called "));


	if ( executeStepL() == KErrNotSupported )
		SetTestStepResult(EPass);	else
		SetTestStepResult(EFail);
	return TestStepResult();
	}


/////////////////////
//	Test step 020.03
/////////////////////

// constructor
CCommDbTest020_03::CCommDbTest020_03()
	{
	// store the name of this test case
	SetTestStepName(_L("step_020_03"));
	}

// destructor
CCommDbTest020_03::~CCommDbTest020_03()
	{
	}


TVerdict CCommDbTest020_03::doTestStepPreambleL()
	{	
	openDbL();
        SetTestStepResult(EPass);	return TestStepResult();
	}


TInt CCommDbTest020_03::executeStepL()
	{
	TInt ret=KErrGeneral;

	//Try to set the agent for a non-service table
	_LIT(KAgent1,"Agent1");

	TRAPD( r, iTheDb->SetAgentL(TPtrC(LOCATION),KAgent1) );
	if (r==KErrNotSupported )
		ret = r;
	
	return ret;
	}



TVerdict CCommDbTest020_03::doTestStepL( )
	{
	INFO_PRINTF1(_L("Step 020.03 called "));


	if ( executeStepL() == KErrNotSupported )
		SetTestStepResult(EPass);	else
		SetTestStepResult(EFail);
	return TestStepResult();
	}



/////////////////////
//	Test step 020.04
/////////////////////

// constructor
CCommDbTest020_04::CCommDbTest020_04()
	{
	// store the name of this test case
	SetTestStepName(_L("step_020_04"));
	}

// destructor
CCommDbTest020_04::~CCommDbTest020_04()
	{
	}


TVerdict CCommDbTest020_04::doTestStepPreambleL()
	{	
	openDbL();
        SetTestStepResult(EPass);	return TestStepResult();
	}


TInt CCommDbTest020_04::executeStepL()
	{
	TInt ret=KErrGeneral;


	//Try to set the agent extension for a non-service table

	_LIT(KAgentExt,"Csd");

	TRAPD(r, iTheDb->SetAgentExtL(TPtrC(LOCATION), KAgentExt) );
	if ( r==KErrNotSupported )
		ret = r;
	
	return ret;
	}



TVerdict CCommDbTest020_04::doTestStepL( )
	{
	INFO_PRINTF1(_L("Step 020.04 called "));


	if ( executeStepL() == KErrNotSupported )
		SetTestStepResult(EPass);	else
		SetTestStepResult(EFail);
	return TestStepResult();
	}






/////////////////////
//	Test step 020.05
/////////////////////

// constructor
CCommDbTest020_05::CCommDbTest020_05()
	{
	// store the name of this test case
	SetTestStepName(_L("step_020_05"));
	}

// destructor
CCommDbTest020_05::~CCommDbTest020_05()
	{
	}


TVerdict CCommDbTest020_05::doTestStepPreambleL()
	{	
	openDbL();
        SetTestStepResult(EPass);	return TestStepResult();
	}


TInt CCommDbTest020_05::executeStepL()
	{
	TInt ret=KErrGeneral;

	//Try to open a view on the agent table
	
	TRAPD(r, iTheView = iTheDb->OpenTableLC(_L("NonExisting")); CleanupStack::Pop(); );

	if ( r==KErrNotFound )
		ret = r;

	return ret;
	}



TVerdict CCommDbTest020_05::doTestStepL( )
	{

	if ( executeStepL() == KErrNotFound )
		SetTestStepResult(EPass);	else
		SetTestStepResult(EFail);
	return TestStepResult();
	}



/////////////////////
//	Test step 020.06
/////////////////////

// constructor
CCommDbTest020_06::CCommDbTest020_06()
	{
	// store the name of this test case
	SetTestStepName(_L("step_020_06"));
	}

// destructor
CCommDbTest020_06::~CCommDbTest020_06()
	{
	}


TVerdict CCommDbTest020_06::doTestStepPreambleL()
	{	
	openDbL();
        SetTestStepResult(EPass);	return TestStepResult();
	}

TInt CCommDbTest020_06::executeStepL()
	{
	TInt ret=KErrGeneral;
	TBuf<64> buffer;

	//Check that the agent setting can be overridden

	_LIT(KAgent1,"Agent1");
	_LIT(KAgent2,"Agent2");

	//Set the agent 
	TRAPD(r1,iTheDb->SetAgentL(TPtrC(DIAL_OUT_ISP),KAgent1));
	//Add a new agent and verify
	TRAPD(r2,iTheDb->SetAgentL(TPtrC(DIAL_OUT_ISP),KAgent2));
	TRAPD(r3,iTheDb->GetAgentL(TPtrC(DIAL_OUT_ISP),buffer));
	if (r1==KErrNotSupported && r2==KErrNotSupported && r3==KErrNotSupported)
		ret = KErrNotSupported;

	return ret;
	}



TVerdict CCommDbTest020_06::doTestStepL( )
	{
	INFO_PRINTF1(_L("Step 020.06 called "));


	if ( executeStepL() == KErrNotSupported )
		SetTestStepResult(EPass);	else
		SetTestStepResult(EFail);
	return TestStepResult();
	}


/////////////////////
//	Test step 020.07
/////////////////////

// constructor
CCommDbTest020_07::CCommDbTest020_07()
	{
	// store the name of this test case
	SetTestStepName(_L("step_020_07"));
	}

// destructor
CCommDbTest020_07::~CCommDbTest020_07()
	{
	}


TVerdict CCommDbTest020_07::doTestStepPreambleL()
	{	
	openDbL();
        SetTestStepResult(EPass);	return TestStepResult();
	}

TInt CCommDbTest020_07::executeStepL()
	{
	TInt ret=KErrGeneral;
	TBuf<64> buffer;

	//Check that the agent settings can be deleted

	_LIT(KAgent,"Agent");
	_LIT(KExtension,"psd");

	//Set the agent 
	TRAPD(r1,iTheDb->SetAgentL(TPtrC(DIAL_OUT_ISP),KAgent));
	TRAPD(r2,iTheDb->SetAgentExtL(TPtrC(DIAL_OUT_ISP),KExtension));

	//Clear the settings
	TRAPD(r3,iTheDb->ClearAgentAndExtL( TPtrC(DIAL_OUT_ISP)));

	//Now try to read the settings
	TRAPD( r4, iTheDb->GetAgentL(TPtrC(DIAL_OUT_ISP),buffer) );
	TRAPD( r5, iTheDb->GetAgentExtL(TPtrC(DIAL_OUT_ISP),buffer) );

	if(r1==KErrNotSupported && r2==KErrNotSupported && r3==KErrNotSupported
		&& r4==KErrNotSupported && r5==KErrNotSupported)
		{
		ret = KErrNotSupported;
		}

	return ret;
	}



TVerdict CCommDbTest020_07::doTestStepL( )
	{
	INFO_PRINTF1(_L("Step 020.07 called "));


	if ( executeStepL() == KErrNotSupported )
		SetTestStepResult(EPass);	else
		SetTestStepResult(EFail);
	return TestStepResult();
	}



/////////////////////
//	Test step 020.08
/////////////////////

// constructor
CCommDbTest020_08::CCommDbTest020_08()
	{
	// store the name of this test case
	SetTestStepName(_L("step_020_08"));
	}

// destructor
CCommDbTest020_08::~CCommDbTest020_08()
	{
	}

TVerdict CCommDbTest020_08::doTestStepL( )
	{
	INFO_PRINTF1(_L("Step 020.08 called "));

	SetTestStepResult(EPass);
		// Heap test for 020.01

	CCommDbTest020_01* step020_01 = new(ELeave) CCommDbTest020_01;
	CleanupStack::PushL(step020_01);
	if ( doTestStepWithHeapFailureL( *step020_01, KErrNotSupported) == EFail )
		SetTestStepResult(EFail);	CleanupStack::PopAndDestroy(step020_01);


		// Heap test for 020.02

	CCommDbTest020_02* step020_02 = new CCommDbTest020_02;
	CleanupStack::PushL(step020_02);
	if ( doTestStepWithHeapFailureL( *step020_02, KErrNotSupported) == EFail )
		SetTestStepResult(EFail);	CleanupStack::PopAndDestroy(step020_02);

		// Heap test for 020.03

	CCommDbTest020_03* step020_03 = new CCommDbTest020_03;
	CleanupStack::PushL(step020_03);
	if ( doTestStepWithHeapFailureL( *step020_03, KErrNotSupported) == EFail )
		SetTestStepResult(EFail);	CleanupStack::PopAndDestroy(step020_03);


		// Heap test for 020.04

	CCommDbTest020_04* step020_04 = new CCommDbTest020_04;
	CleanupStack::PushL(step020_04);
	if ( doTestStepWithHeapFailureL( *step020_04, KErrNotSupported) == EFail )
		SetTestStepResult(EFail);	CleanupStack::PopAndDestroy(step020_04);


		// Heap test for 020.06

	CCommDbTest020_06* step020_06 = new CCommDbTest020_06;
	CleanupStack::PushL(step020_06);
	if ( doTestStepWithHeapFailureL( *step020_06, KErrNotSupported) == EFail )
		SetTestStepResult(EFail);	CleanupStack::PopAndDestroy(step020_06);



		// Heap test for 020.07

	CCommDbTest020_07* step020_07 = new CCommDbTest020_07;
	CleanupStack::PushL(step020_07);
	if ( doTestStepWithHeapFailureL( *step020_07, KErrNotSupported) == EFail )
		SetTestStepResult(EFail);	CleanupStack::PopAndDestroy(step020_07);




	return TestStepResult();	

}
