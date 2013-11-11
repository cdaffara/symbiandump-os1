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
// This contains CommDb Unit Test Cases 012.XX

// EPOC includes
#include <e32base.h>
#include <commdb.h>


// Test system includes
#include "Teststepcommdb.h"
#include "Step_012_xx.h"



/////////////////////
//	Test step 012.001
/////////////////////

// constructor
CCommDbTest012_01::CCommDbTest012_01()
	{
	// store the name of this test case
	SetTestStepName(_L("step_012_01"));
	}

// destructor
CCommDbTest012_01::~CCommDbTest012_01()
	{
	}


TVerdict CCommDbTest012_01::doTestStepPreambleL()
	{	
	openDbL();
        SetTestStepResult(EPass);	return TestStepResult();
	}


TInt CCommDbTest012_01::executeStepL()
	{
	TInt ret=KErrNone;

	//This query should select 3 records
	iTheView = iTheDb->OpenViewLC( TPtrC(DIAL_IN_ISP),_L("select * from DialInISP where Ifname='PPP'") );
	CleanupStack::Pop();

	//Now check that the records are what we expect
	CDBLEAVE( iTheView->GotoFirstRecord(), KErrNone );

	//Read the value from the field and compare it to the expected value
	TBuf<KCommsDbSvrMaxFieldLength> value;
	iTheView->ReadTextL( TPtrC(COMMDB_NAME), value );
	if( value.Compare(_L("Record #1")) )
		{
		ret = KErrGeneral;
		}

	CDBLEAVE( iTheView->GotoNextRecord(), KErrNone );
	iTheView->ReadTextL( TPtrC(COMMDB_NAME), value );
	if( value.Compare(_L("Record #2")) )
		{
		ret = KErrGeneral;
		}

	CDBLEAVE( iTheView->GotoNextRecord(), KErrNone );
	iTheView->ReadTextL( TPtrC(COMMDB_NAME), value );
	if( value.Compare(_L("Record #5 (Read Only)")) )
		{
		ret = KErrGeneral;
		}

	CDBLEAVE( ret = iTheView->GotoNextRecord(), KErrNotFound);

	return ret;
	}



TVerdict CCommDbTest012_01::doTestStepL( )
	{
	INFO_PRINTF1(_L("Step 012.01 called "));

	
	if ( executeStepL() == KErrNotFound )
		SetTestStepResult(EPass);	else
		SetTestStepResult(EFail);

	return TestStepResult();
	}


/////////////////////
//	Test step 012.02
/////////////////////

// constructor
CCommDbTest012_02::CCommDbTest012_02()
	{
	// store the name of this test case
	SetTestStepName(_L("step_012_02"));

	}

// destructor
CCommDbTest012_02::~CCommDbTest012_02()
	{
	}

TVerdict CCommDbTest012_02::doTestStepPreambleL()
	{	
	openDbL();
        SetTestStepResult(EPass);	return TestStepResult();
	}


TInt CCommDbTest012_02::executeStepL()
	{
	TInt ret=KErrNone;

	//This query should select 0 records
	iTheView = iTheDb->OpenViewLC( TPtrC(DIAL_IN_ISP),_L("select * from DialInISP where Ifname='XXX'") );
	CleanupStack::Pop();

	CDBLEAVE(ret = iTheView->GotoFirstRecord(), KErrNotFound );

	return ret;
	}



TVerdict CCommDbTest012_02::doTestStepL( )
	{
	INFO_PRINTF1(_L("Step 012.02 called "));

	if ( executeStepL() == KErrNotFound )
		SetTestStepResult(EPass);	else
		SetTestStepResult(EFail);
	return TestStepResult();
	}


/////////////////////
//	Test step 012.03
/////////////////////

// constructor
CCommDbTest012_03::CCommDbTest012_03()
	{
	// store the name of this test case
	SetTestStepName(_L("step_012_03"));

	}

// destructor
CCommDbTest012_03::~CCommDbTest012_03()
	{
	}


TVerdict CCommDbTest012_03::doTestStepPreambleL()
	{	
	openDbL();
        SetTestStepResult(EPass);	return TestStepResult();
	}

TInt CCommDbTest012_03::executeStepL()
	{
	TInt ret=KErrNone;

	//This query should select 1 record
	iTheView = iTheDb->OpenViewLC( TPtrC(DIAL_IN_ISP),_L("select * from DialInISP where Name='Record #3'") );
	CleanupStack::Pop();

	//Read the value from the field and compare it to the expected value
	CDBLEAVE( iTheView->GotoFirstRecord(), KErrNone );
	TBuf<KCommsDbSvrMaxFieldLength> value;
	iTheView->ReadTextL( TPtrC(COMMDB_NAME), value );
	if( value.Compare(_L("Record #3")) )
		{
		ret = KErrGeneral;
		}

	//Check that there was only one record
	CDBLEAVE( ret = iTheView->GotoNextRecord(), KErrNotFound );

	return ret;
	}



TVerdict CCommDbTest012_03::doTestStepL( )
	{
	INFO_PRINTF1(_L("Step 012.03 called "));

	if ( executeStepL() == KErrNotFound )
		SetTestStepResult(EPass);	else
		SetTestStepResult(EFail);	
	return TestStepResult();
	}


/////////////////////
//	Test step 012.04
/////////////////////

// constructor
CCommDbTest012_04::CCommDbTest012_04()
	{
	// store the name of this test case
	SetTestStepName(_L("step_012_04"));
	// default to no local objects allocated
	iLocalObjects = EFalse;
	}

// destructor
CCommDbTest012_04::~CCommDbTest012_04()
	{
	}

TVerdict CCommDbTest012_04::doTestStepPreambleL()
	{	
	openDbL();
        SetTestStepResult(EPass);	return TestStepResult();
	}


TInt CCommDbTest012_04::executeStepL()
	{
	TInt ret=KErrNone;

	//This query should select al1 records
	iTheView = iTheDb->OpenViewLC( TPtrC(DIAL_IN_ISP),_L("select * from DialInISP") );
	CleanupStack::Pop();

	//For each expected record read the value from the field and compare it to the expected value
	CDBLEAVE( iTheView->GotoFirstRecord(), KErrNone );
	TBuf<KCommsDbSvrMaxFieldLength> value;
	iTheView->ReadTextL( TPtrC(COMMDB_NAME), value );
	if( value.Compare(_L("Record #1")) )
		{
		ret = KErrGeneral;
		}

	CDBLEAVE( iTheView->GotoNextRecord(), KErrNone );
	iTheView->ReadTextL( TPtrC(COMMDB_NAME), value );
	if( value.Compare(_L("Record #2")) )
		{
		ret = KErrGeneral;
		}

	CDBLEAVE( iTheView->GotoNextRecord(), KErrNone );
	iTheView->ReadTextL( TPtrC(COMMDB_NAME), value );
	if( value.Compare(_L("Record #3")) )
		{
		ret = KErrGeneral;
		}

	CDBLEAVE( iTheView->GotoNextRecord(), KErrNone );
	iTheView->ReadTextL( TPtrC(COMMDB_NAME), value );
	if( value.Compare(_L("Record #4")) )
		{
		ret = KErrGeneral;
		}

	CDBLEAVE( iTheView->GotoNextRecord(), KErrNone );
	iTheView->ReadTextL( TPtrC(COMMDB_NAME), value );
	if( value.Compare(_L("Record #5")) )
		{
		ret = KErrGeneral;
		}
	//Check there no more records

	CDBLEAVE( ret = iTheView->GotoNextRecord(), KErrNotFound );
		
	return ret;
	}



TVerdict CCommDbTest012_04::doTestStepL( )
	{
	INFO_PRINTF1(_L("Step 012.04 called "));

	if ( executeStepL() == KErrNotFound )
		SetTestStepResult(EPass);	else
		SetTestStepResult(EFail);

	return TestStepResult();
	}


/////////////////////
//	Test step 012.05
/////////////////////

// constructor
CCommDbTest012_05::CCommDbTest012_05()
	{
	// store the name of this test case
	SetTestStepName(_L("step_012_05"));
	// default to no local objects allocated
	iLocalObjects = EFalse;
	}

// destructor
CCommDbTest012_05::~CCommDbTest012_05()
	{
	}

TVerdict CCommDbTest012_05::doTestStepPreambleL()
	{	
	openDbL();
        SetTestStepResult(EPass);	return TestStepResult();
	}

TInt CCommDbTest012_05::myOpenViewL()
{
	//This query contains an erroneous query
//	TRAPD( r, iTheView = iTheDb->OpenViewLC( TPtrC(DIAL_IN_ISP),_L("select * from DialInISP where") ) );
	 iTheView = iTheDb->OpenViewLC( TPtrC(DIAL_IN_ISP),_L("select * from DialInISP where") ) ;

	
	return 0;
}


TInt CCommDbTest012_05::executeStepL()
	{
	TInt ret=KErrGeneral;

	TRAPD(r, myOpenViewL() );

	if ( r!=KErrNone )
		ret = r;

	

	return ret;
	}



TVerdict CCommDbTest012_05::doTestStepL( )
	{
	INFO_PRINTF1(_L("Step 012.05 called "));

	
	if ( executeStepL() == KErrArgument )
		SetTestStepResult(EPass);	else
		SetTestStepResult(EFail);


	return TestStepResult();
	}


/////////////////////
//	Test step 012.06
/////////////////////

// constructor
CCommDbTest012_06::CCommDbTest012_06()
	{
	// store the name of this test case
	SetTestStepName(_L("step_012_06"));
	}

// destructor
CCommDbTest012_06::~CCommDbTest012_06()
	{
	}

TVerdict CCommDbTest012_06::doTestStepPreambleL()
	{	
	openDbL();
        SetTestStepResult(EPass);	return TestStepResult();
	}

TInt CCommDbTest012_06::myOpenViewL()
{
	//This query contains an erroneous query
	iTheView = iTheDb->OpenViewLC( TPtrC(DIAL_IN_ISP),_L("") );

	
	return 0;
}


TInt CCommDbTest012_06::executeStepL()
	{
	TInt ret=KErrGeneral;

	TRAPD(r, myOpenViewL() );

	if (r!=KErrNone )
		ret = r;

	return ret;
	}



TVerdict CCommDbTest012_06::doTestStepL( )
	{
	INFO_PRINTF1(_L("Step 012.06 called "));
	
	if ( executeStepL() == KErrArgument )
		SetTestStepResult(EPass);	else
		SetTestStepResult(EFail);
	return TestStepResult();
	}



/////////////////////
//	Test step 012.07
/////////////////////

// constructor
CCommDbTest012_07::CCommDbTest012_07()
	{
	// store the name of this test case
	SetTestStepName(_L("step_012_07"));
	}

// destructor
CCommDbTest012_07::~CCommDbTest012_07()
	{
	}

TVerdict CCommDbTest012_07::doTestStepL( )
	{
	INFO_PRINTF1(_L("Step 012.07 called "));

	SetTestStepResult(EPass);
/*	// Heap test for 012.01

#if 1
	CCommDbTest012_01* step012_01 = new CCommDbTest012_01;
	CleanupStack::PushL(step012_01);
	doTestStepWithHeapFailureL( *step012_01, KErrNotFound);
	if ( step012_01->TestStepResult() == EFail )
		SetTestStepResult(EFail);	CleanupStack::PopAndDestroy(step012_01);
#endif

	// Heap test for 012.02

	CCommDbTest012_02* step012_02 = new CCommDbTest012_02;
	CleanupStack::PushL(step012_02);
	doTestStepWithHeapFailureL( *step012_02, KErrNotFound);
	if ( step012_02->TestStepResult() == EFail )
		SetTestStepResult(EFail);	CleanupStack::PopAndDestroy(step012_02);

*/	// Heap test for 012.03

	CCommDbTest012_03* step012_03 = new CCommDbTest012_03;
	CleanupStack::PushL(step012_03);
	doTestStepWithHeapFailureL( *step012_03, KErrNotFound);
	if ( step012_03->TestStepResult() == EFail )
		SetTestStepResult(EFail);	CleanupStack::PopAndDestroy(step012_03);

	// Heap test for 012.04

	CCommDbTest012_04* step012_04 = new CCommDbTest012_04;
	CleanupStack::PushL(step012_04);
	doTestStepWithHeapFailureL( *step012_04, KErrNotFound);
	if( step012_04->TestStepResult() == EFail )
		SetTestStepResult(EFail);	CleanupStack::PopAndDestroy(step012_04);

	// Heap test for 012.05

	CCommDbTest012_05* step012_05 = new CCommDbTest012_05;
	CleanupStack::PushL(step012_05);
	doTestStepWithHeapFailureL( *step012_05, KErrArgument);
	if( step012_05->TestStepResult() == EFail )
		SetTestStepResult(EFail);	CleanupStack::PopAndDestroy(step012_05);


	// Heap test for 012.06

	CCommDbTest012_06* step012_06 = new CCommDbTest012_06;
	CleanupStack::PushL(step012_06);
	doTestStepWithHeapFailureL( *step012_06, KErrArgument);
	if( step012_06->TestStepResult() == EFail )
		SetTestStepResult(EFail);	CleanupStack::PopAndDestroy(step012_06);

	return TestStepResult();	

}
