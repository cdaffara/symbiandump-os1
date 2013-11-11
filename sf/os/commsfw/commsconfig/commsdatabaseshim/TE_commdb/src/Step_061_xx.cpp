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
// This contains CommDb Unit Test Cases 061.XX

// EPOC includes
#include <e32base.h>
#include <commdb.h>
#include <d32comm.h>
#include <cdbstore.h>

// Test system includes
#include "Teststepcommdb.h"
#include "etelmm.h"
#include "Step_061_xx.h"
#include <e32math.h>

CCommsDbTableView *CCommDbTest061_xx::OpenIspTable(CCommsDatabase* aDb, TInt &aNoOfRows, TInt aExpectedCount)
{
	char OK = 1;
	TInt ErrCode = KErrGeneral;

	CCommsDbTableView *TableView = NULL;
	
	TRAP(ErrCode, TableView = aDb->OpenTableLC( TPtrC(DIAL_OUT_ISP) ); CleanupStack::Pop(TableView););

	if(ErrCode != KErrNone)
		{
		INFO_PRINTF2(_L("CCommDbTest061_xx::OpenIspTable : aDb->OpenTableLC LEFT with %d"),ErrCode);
		return(NULL);
		}
	   
	aNoOfRows = 0;
	char DropOut = 0;
	while(!DropOut)
	{
		if(aNoOfRows == 0)
			ErrCode = TableView->GotoFirstRecord(); 
		else
			ErrCode = TableView->GotoNextRecord(); 
		DropOut = (ErrCode != KErrNone);
		if(ErrCode == KErrNone)
			++aNoOfRows;
	}

	OK = (ErrCode == KErrNotFound); // should be the last value returned.
	if(OK)
		OK = (aNoOfRows == aExpectedCount);
	if(OK)
	{
		INFO_PRINTF1(_L("Found expected number of table entries "));
		return(TableView);
	}
	INFO_PRINTF2(_L("CCommDbTest061_xx::OpenIspTable : function failed with OK = %d"),OK);
	return(NULL);
}

/////////////////////
//	Test step 061.01
/////////////////////

// constructor
CCommDbTest061_01::CCommDbTest061_01()
	{
	// store the name of this test case
	SetTestStepName(_L("step_061_01"));
	}

// destructor
CCommDbTest061_01::~CCommDbTest061_01()
	{
	}


TVerdict CCommDbTest061_01::doTestStepPreambleL()
	{	
	openDbL();
        SetTestStepResult(EPass);	return TestStepResult();
	}

TInt CCommDbTest061_01::executeStepL()
	{
	TInt ret=KErrGeneral;
	TInt RowCount;

	// Values tested
	TInt EnumArray[] = {
		RMobileCall::EProtocolUnspecified, // first two are different ways of saying unspecified
		RMobileCall::EProtocolUnspecified,
		RMobileCall::EProtocolV22bis,
		RMobileCall::EProtocolV32,
		RMobileCall::EProtocolV34,
		RMobileCall::EProtocolV110,
		RMobileCall::EProtocolV120,
		RMobileCall::EProtocolX31FlagStuffing,
		RMobileCall::EProtocolPIAFS,
		RMobileCall::EProtocolBitTransparent,
		RMobileCall::EProtocolPstnMultimediaVoiceFallback,
		RMobileCall::EProtocolPstnMultimedia,
		RMobileCall::EProtocolIsdnMultimedia
					};

	iTheView = OpenIspTable(iTheDb, RowCount, (sizeof(EnumArray)/sizeof(TInt)));

	char OK = (iTheView != NULL);

	if(OK)
	{
		for(TInt i = 0; (i < RowCount) && OK; ++i)
		{
			TUint32 BearerProtocol;
			if(i == 0)
				iTheView->GotoFirstRecord(); 
			else
				iTheView->GotoNextRecord(); 
			iTheView->ReadUintL(TPtrC(ISP_BEARER_PROTOCOL), BearerProtocol);
			OK = (BearerProtocol == (TUint)EnumArray[i]);
		}
	}

	ret = (OK ? KErrNone : KErrGeneral);

	return ret;
	}


TVerdict CCommDbTest061_01::doTestStepL( )
	{
	INFO_PRINTF1(_L("Step 061.01 called "));

	if ( executeStepL() == KErrNone )
		SetTestStepResult(EPass);	else
		SetTestStepResult(EFail);
	return TestStepResult();
	}

/////////////////////
//	Test step 061.02
/////////////////////

// constructor
CCommDbTest061_02::CCommDbTest061_02()
	{
	// store the name of this test case
	SetTestStepName(_L("step_061_02"));
	}

// destructor
CCommDbTest061_02::~CCommDbTest061_02()
	{
	}


TVerdict CCommDbTest061_02::doTestStepPreambleL()
	{	
	openDbL();
        SetTestStepResult(EPass);	return TestStepResult();
	}

TInt CCommDbTest061_02::executeStepL()
	{
	TInt ret=KErrGeneral;
	TInt RowCount;

	// Values tested
	TInt EnumArray[] = {
		RMobileCall::KCapsDataCircuitAsynchronous,
		RMobileCall::KCapsDataCircuitAsynchronousRDI,
		RMobileCall::KCapsDataCircuitSynchronous,
		RMobileCall::KCapsDataCircuitSynchronousRDI,
		RMobileCall::KCapsPADAsyncUDI,
		RMobileCall::KCapsPADAsyncRDI,
		RMobileCall::KCapsPacketAccessSyncUDI,
		RMobileCall::KCapsPacketAccessSyncRDI,
		RMobileCall::KCapsServiceExtended
					};

	iTheView = OpenIspTable(iTheDb, RowCount, (sizeof(EnumArray)/sizeof(TInt)));

	char OK = (iTheView != NULL);

	if(OK)
	{
		for(TInt i = 0; (i < RowCount) && OK; ++i)
		{
			TUint32 BearerName;
			if(i == 0)
				iTheView->GotoFirstRecord(); 
			else
				iTheView->GotoNextRecord(); 
			iTheView->ReadUintL(TPtrC(ISP_BEARER_NAME), BearerName);
			OK = (BearerName == (TUint)EnumArray[i]);
		}
	}

	ret = (OK ? KErrNone : KErrGeneral);

	return ret;
	}


TVerdict CCommDbTest061_02::doTestStepL( )
	{
	INFO_PRINTF1(_L("Step 061.02 called "));

	if ( executeStepL() == KErrNone )
		SetTestStepResult(EPass);	else
		SetTestStepResult(EFail);
	return TestStepResult();
	}

/////////////////////
//	Test step 061.03
/////////////////////

// constructor
CCommDbTest061_03::CCommDbTest061_03()
	{
	// store the name of this test case
	SetTestStepName(_L("step_061_03"));
	}

// destructor
CCommDbTest061_03::~CCommDbTest061_03()
	{
	}


TVerdict CCommDbTest061_03::doTestStepPreambleL()
	{	
	openDbL();
        SetTestStepResult(EPass);	return TestStepResult();
	}

TInt CCommDbTest061_03::executeStepL()
	{
	TInt ret=KErrGeneral;
	TInt RowCount;

	// Values tested
	TInt EnumArray[] = {
		RMobileCall::ESpeedUnspecified,
		RMobileCall::ESpeedAutobauding,
		RMobileCall::ESpeed2400,
		RMobileCall::ESpeed4800,
		RMobileCall::ESpeed9600,
		RMobileCall::ESpeed14400,
		RMobileCall::ESpeed19200,
		RMobileCall::ESpeed28800,
		RMobileCall::ESpeed32000,
		RMobileCall::ESpeed33600,
		RMobileCall::ESpeed38400,
		RMobileCall::ESpeed43200,
		RMobileCall::ESpeed48000,
		RMobileCall::ESpeed56000,
		RMobileCall::ESpeed57600,
		RMobileCall::ESpeed64000
					};

	iTheView = OpenIspTable(iTheDb, RowCount, (sizeof(EnumArray)/sizeof(TInt)));

	char OK = (iTheView != NULL);

	if(OK)
	{
		for(TInt i = 0; (i < RowCount) && OK; ++i)
		{
			TUint32 BearerSpeed;
			if(i == 0)
				iTheView->GotoFirstRecord(); 
			else
				iTheView->GotoNextRecord(); 
			iTheView->ReadUintL(TPtrC(ISP_BEARER_SPEED), BearerSpeed);
			OK = (BearerSpeed == (TUint)EnumArray[i]);
		}
	}

	ret = (OK ? KErrNone : KErrGeneral);

	return ret;
	}


TVerdict CCommDbTest061_03::doTestStepL( )
	{
	INFO_PRINTF1(_L("Step 061.03 called "));

	if ( executeStepL() == KErrNone )
		SetTestStepResult(EPass);	else
		SetTestStepResult(EFail);
	return TestStepResult();
	}

/////////////////////
//	Test step 061.04
/////////////////////

// constructor
CCommDbTest061_04::CCommDbTest061_04()
	{
	// store the name of this test case
	SetTestStepName(_L("step_061_04"));
	}

// destructor
CCommDbTest061_04::~CCommDbTest061_04()
	{
	}


TVerdict CCommDbTest061_04::doTestStepPreambleL()
	{	
	openDbL();
        SetTestStepResult(EPass);	return TestStepResult();
	}

TInt CCommDbTest061_04::executeStepL()
	{
	TInt ret=KErrGeneral;
	TInt RowCount;

	// Values tested (see RMobileCall::TMobileCallDataQoS, etelmm.h)
	TInt EnumArray[] = {
		RMobileCall::EQoSUnspecified,
		RMobileCall::EQoSTransparent,
		RMobileCall::EQoSNonTransparent,
		RMobileCall::EQosTransparentPreferred,
		RMobileCall::EQosNonTransparentPreferred
					};

	iTheView = OpenIspTable(iTheDb, RowCount, (sizeof(EnumArray)/sizeof(TInt)));

	char OK = (iTheView != NULL);

	if(OK)
	{
		for(TInt i = 0; (i < RowCount) && OK; ++i)
		{
			TUint32 BearerCE;
			if(i == 0)
				iTheView->GotoFirstRecord(); 
			else
				iTheView->GotoNextRecord(); 
			iTheView->ReadUintL(TPtrC(ISP_BEARER_CE), BearerCE);
			OK = (BearerCE == (TUint)EnumArray[i]);
		}
	}

	ret = (OK ? KErrNone : KErrGeneral);

	return ret;
	}


TVerdict CCommDbTest061_04::doTestStepL( )
	{
	INFO_PRINTF1(_L("Step 061.04 called "));

	if ( executeStepL() == KErrNone )
		SetTestStepResult(EPass);	else
		SetTestStepResult(EFail);
	return TestStepResult();
	}

