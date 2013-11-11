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
// This contains CommDb Unit Test Cases 036.XX

// EPOC includes
#include <e32base.h>
#include <commdb.h>
#include <d32comm.h>
#include <cdbtemp.h>
#include <etelmm.h>

// Test system includes
#include "Teststepcommdb.h"
#include "Step_036_xx.h"


CCommDbTest036_01::CCommDbTest036_01()
	{
	// store the name of this test case
	SetTestStepName(_L("step_036_01"));
	}

CCommDbTest036_01::~CCommDbTest036_01()
	{}

TVerdict CCommDbTest036_01::doTestStepL( void )
	{
	if(executeStepL()!=KErrNone)
         SetTestStepResult(EFail);        SetTestStepResult(EPass);	return TestStepResult();
	}

TVerdict CCommDbTest036_01::doTestStepPreambleL()
	{
	iTheDb=CCommsDatabase::NewL();

        SetTestStepResult(EPass);	return TestStepResult();
	}

TInt CCommDbTest036_01::executeStepL()
	{
	CCommsDbTemplateRecord* templateRecord = CCommsDbTemplateRecord::NewL(iTheDb, TPtrC(MODEM_BEARER));
	CleanupStack::PushL(templateRecord);
	
	User::LeaveIfError(templateRecord->Modify());

	TBufC8<KCommsDbSvrMaxFieldLength> inputString = _L8("string");
	templateRecord->WriteTextL(TPtrC(COMMDB_NAME), _L("DummyName"));
	templateRecord->WriteTextL(TPtrC(MODEM_FAX_INIT_STRING), inputString);
	User::LeaveIfError(templateRecord->StoreModifications());

	CleanupStack::PopAndDestroy(templateRecord);

	//Create a new template record and check the value is the same as the one entered.
	CCommsDbTemplateRecord* checkRecord = CCommsDbTemplateRecord::NewL(iTheDb, TPtrC(MODEM_BEARER));
	CleanupStack::PushL(checkRecord);
	
	TBuf8<KCommsDbSvrMaxFieldLength> checkString;

	checkRecord->ReadTextL(TPtrC(MODEM_FAX_INIT_STRING), checkString);
	CleanupStack::PopAndDestroy(checkRecord);

	if(checkString!=inputString)
		return KErrGeneral;

	return KErrNone;
	}
	
TVerdict CCommDbTest036_01::doTestStepPostambleL()
	{
	delete iTheDb;
	iTheDb=NULL;
        SetTestStepResult(EPass);	return TestStepResult();
	}

/////////////////////////////////////////////////////////////////////////

CCommDbTest036_02::CCommDbTest036_02()
	{
	// store the name of this test case
	SetTestStepName(_L("step_036_02"));
	}

CCommDbTest036_02::~CCommDbTest036_02()
	{}

TVerdict CCommDbTest036_02::doTestStepL( void )
	{
	if(executeStepL()!=KErrNone)
         SetTestStepResult(EFail);        SetTestStepResult(EPass);	return TestStepResult();
	}

TVerdict CCommDbTest036_02::doTestStepPreambleL()
	{
	iTheDb=CCommsDatabase::NewL();

        SetTestStepResult(EPass);	return TestStepResult();
	}

TInt CCommDbTest036_02::executeStepL()
	{
	CCommsDbTemplateRecord* templateRecord = CCommsDbTemplateRecord::NewL(iTheDb, TPtrC(MODEM_BEARER));
	CleanupStack::PushL(templateRecord);
	
	User::LeaveIfError(templateRecord->Modify());

	TBufC8<KCommsDbSvrMaxFieldLength> firstInputString = _L8("string");
	templateRecord->WriteTextL(TPtrC(COMMDB_NAME), _L("DummyName"));
	templateRecord->WriteTextL(TPtrC(MODEM_FAX_INIT_STRING), firstInputString);
	User::LeaveIfError(templateRecord->StoreModifications());

	CleanupStack::PopAndDestroy(templateRecord);

	//Create a view on the modem table, over write the template value previosly set and check it sticks

	CCommsDbTableView* tableView = iTheDb->OpenTableLC(TPtrC(MODEM_BEARER));
	TUint32 dummyId;
	//Create a new record, so we can be sure it is the same as the templated one
	User::LeaveIfError(tableView->InsertRecord(dummyId));
	tableView->WriteTextL(TPtrC(COMMDB_NAME), _L("NewModem"));
	//Do overwrite of setting
	TBufC8<KCommsDbSvrMaxFieldLength> overwriteString = _L8("different string");
	tableView->WriteTextL(TPtrC(MODEM_FAX_INIT_STRING), overwriteString);
	User::LeaveIfError(tableView->PutRecordChanges());

	TBuf8<KCommsDbSvrMaxFieldLength> retrievedString;
	tableView->ReadTextL(TPtrC(MODEM_FAX_INIT_STRING), retrievedString);
	CleanupStack::PopAndDestroy(tableView);

	if(retrievedString!=overwriteString)
		return KErrGeneral;

	return KErrNone;
	}
	
TVerdict CCommDbTest036_02::doTestStepPostambleL()
	{
	delete iTheDb;
	iTheDb=NULL;
        SetTestStepResult(EPass);	return TestStepResult();
	}

/////////////////////////////////////////////////////////////////////////

CCommDbTest036_03::CCommDbTest036_03()
	{
	// store the name of this test case
	SetTestStepName(_L("step_036_03"));
	}

CCommDbTest036_03::~CCommDbTest036_03()
	{}

TVerdict CCommDbTest036_03::doTestStepL( void )
	{
	if(executeStepL()!=KErrNone)
         SetTestStepResult(EFail);        SetTestStepResult(EPass);	return TestStepResult();
	}

TVerdict CCommDbTest036_03::doTestStepPreambleL()
	{
	iTheDb=CCommsDatabase::NewL();

        SetTestStepResult(EPass);	return TestStepResult();
	}

TInt CCommDbTest036_03::executeStepL()
	{
	CCommsDbTemplateRecord* templateRecord = CCommsDbTemplateRecord::NewL(iTheDb, TPtrC(MODEM_BEARER));
	CleanupStack::PushL(templateRecord);
	
	User::LeaveIfError(templateRecord->Modify());

	TBufC<KCommsDbSvrMaxFieldLength> inputString = _L("string");
	templateRecord->WriteTextL(TPtrC(COMMDB_NAME), _L("DummyName"));
	templateRecord->WriteTextL(TPtrC(MODEM_PORT_NAME), inputString);
	User::LeaveIfError(templateRecord->StoreModifications());

	CleanupStack::PopAndDestroy(templateRecord);

	//Create a view on the modem table, make a new record and check the value is the one set for the template

	CCommsDbTableView* tableView = iTheDb->OpenTableLC(TPtrC(MODEM_BEARER));
	TUint32 dummyId;
	//Create a new record, so we can be sure it is the same as the templated one
	User::LeaveIfError(tableView->InsertRecord(dummyId));
	tableView->WriteTextL(TPtrC(COMMDB_NAME), _L("NewModem"));
	User::LeaveIfError(tableView->PutRecordChanges());

	//Retrieve the string we set for the template
	TBuf<KCommsDbSvrMaxFieldLength> retrievedString;
	tableView->ReadTextL(TPtrC(MODEM_PORT_NAME), retrievedString);
	CleanupStack::PopAndDestroy(tableView);

	if(retrievedString!=inputString)
		return KErrGeneral;

	return KErrNone;
	}
	
TVerdict CCommDbTest036_03::doTestStepPostambleL()
	{
	delete iTheDb;
	iTheDb=NULL;
        SetTestStepResult(EPass);	return TestStepResult();
	}

/////////////////////////////////////////////////////////////////////////

CCommDbTest036_04::CCommDbTest036_04()
	{
	// store the name of this test case
	SetTestStepName(_L("step_036_04"));
	}

CCommDbTest036_04::~CCommDbTest036_04()
	{}

TVerdict CCommDbTest036_04::doTestStepL( void )
	{
	if(executeStepL()!=KErrNone)
         SetTestStepResult(EFail);        SetTestStepResult(EPass);	return TestStepResult();
	}

TVerdict CCommDbTest036_04::doTestStepPreambleL()
	{
	iTheDb=CCommsDatabase::NewL();

        SetTestStepResult(EPass);	return TestStepResult();
	}

TInt CCommDbTest036_04::executeStepL()
	{
	CCommsDbTemplateRecord* templateRecord = CCommsDbTemplateRecord::NewL(iTheDb, TPtrC(MODEM_BEARER));
	CleanupStack::PushL(templateRecord);
	
	User::LeaveIfError(templateRecord->Modify());

	TBufC<KCommsDbSvrMaxFieldLength> inputString = _L("string");
	templateRecord->WriteTextL(TPtrC(COMMDB_NAME), _L("DummyName"));
	templateRecord->WriteTextL(TPtrC(MODEM_PORT_NAME), inputString);
	User::LeaveIfError(templateRecord->StoreModifications());

	CleanupStack::PopAndDestroy(templateRecord);

	//Create a view on the modem table, make a new record and overwrite the value set in the template

	CCommsDbTableView* tableView = iTheDb->OpenTableLC(TPtrC(MODEM_BEARER));
	TUint32 dummyId;
	//Create a new record, so we can be sure it is the same as the templated one
	User::LeaveIfError(tableView->InsertRecord(dummyId));
	tableView->WriteTextL(TPtrC(COMMDB_NAME), _L("NewModem"));
	//Do overwrite of setting
	TBufC<KCommsDbSvrMaxFieldLength> overwriteString = _L("different string");
	tableView->WriteTextL(TPtrC(MODEM_PORT_NAME), overwriteString);
	User::LeaveIfError(tableView->PutRecordChanges());

	//Retrieve the string we just set
	TBuf<KCommsDbSvrMaxFieldLength> retrievedString;
	tableView->ReadTextL(TPtrC(MODEM_PORT_NAME), retrievedString);
	CleanupStack::PopAndDestroy(tableView);

	if(retrievedString!=overwriteString)
		return KErrGeneral;

	return KErrNone;
	}
	
TVerdict CCommDbTest036_04::doTestStepPostambleL()
	{
	delete iTheDb;
	iTheDb=NULL;
        SetTestStepResult(EPass);	return TestStepResult();
	}

/////////////////////////////////////////////////////////////////////////

CCommDbTest036_05::CCommDbTest036_05()
	{
	// store the name of this test case
	SetTestStepName(_L("step_036_05"));
	}

CCommDbTest036_05::~CCommDbTest036_05()
	{}

TVerdict CCommDbTest036_05::doTestStepL( void )
	{
	if(executeStepL()!=KErrNone)
         SetTestStepResult(EFail);        SetTestStepResult(EPass);	return TestStepResult();
	}

TVerdict CCommDbTest036_05::doTestStepPreambleL()
	{
	iTheDb=CCommsDatabase::NewL();

        SetTestStepResult(EPass);	return TestStepResult();
	}

TInt CCommDbTest036_05::executeStepL()
	{
	CCommsDbTemplateRecord* templateRecord = CCommsDbTemplateRecord::NewL(iTheDb, TPtrC(DIAL_IN_ISP));
	CleanupStack::PushL(templateRecord);
	
	User::LeaveIfError(templateRecord->Modify());

	TBufC<KCommsDbSvrMaxFieldLength> inputString = _L("test string");
	templateRecord->WriteTextL(TPtrC(COMMDB_NAME), _L("Name"));
//	templateRecord->WriteTextL(TPtrC(ISP_IF_NAME), _L("SLIP"));
	templateRecord->WriteBoolL(TPtrC(ISP_IP_ADDR_FROM_SERVER), ETrue);
	templateRecord->WriteBoolL(TPtrC(ISP_IP_DNS_ADDR_FROM_SERVER), ETrue);
	templateRecord->WriteBoolL(TPtrC(ISP_IP6_DNS_ADDR_FROM_SERVER), ETrue);
	templateRecord->WriteLongTextL(TPtrC(ISP_LOGIN_SCRIPT), inputString);
	User::LeaveIfError(templateRecord->StoreModifications());

	CleanupStack::PopAndDestroy(templateRecord);

	//Create a view on the DialInISP table, make a new record and check the value is the one set for the template

	CCommsDbTableView* tableView = iTheDb->OpenTableLC(TPtrC(DIAL_IN_ISP));
	TUint32 dummyId;
	//Create a new record, so we can be sure it is the same as the templated one
	User::LeaveIfError(tableView->InsertRecord(dummyId));
	tableView->WriteTextL(TPtrC(COMMDB_NAME), _L("Test ISP"));
//	tableView->WriteTextL(TPtrC(ISP_IF_NAME), _L("Test If"));
	tableView->WriteBoolL(TPtrC(ISP_IP_ADDR_FROM_SERVER), EFalse);
	tableView->WriteBoolL(TPtrC(ISP_IP_DNS_ADDR_FROM_SERVER), EFalse);
	tableView->WriteBoolL(TPtrC(ISP_IP6_DNS_ADDR_FROM_SERVER), EFalse);
	User::LeaveIfError(tableView->PutRecordChanges());

	//Retrieve the string we set for the template
	HBufC* retrievedString = tableView->ReadLongTextLC(TPtrC(ISP_LOGIN_SCRIPT));

	TInt ret(KErrNone);
	if(*retrievedString!=inputString)
		ret= KErrGeneral;

	CleanupStack::PopAndDestroy();	//retrievedString
	CleanupStack::PopAndDestroy(tableView);

	return ret;
	}
	
TVerdict CCommDbTest036_05::doTestStepPostambleL()
	{
	delete iTheDb;
	iTheDb=NULL;
        SetTestStepResult(EPass);	return TestStepResult();
	}

/////////////////////////////////////////////////////////////////////////

CCommDbTest036_06::CCommDbTest036_06()
	{
	// store the name of this test case
	SetTestStepName(_L("step_036_06"));
	}

CCommDbTest036_06::~CCommDbTest036_06()
	{}

TVerdict CCommDbTest036_06::doTestStepL( void )
	{
	if(executeStepL()!=KErrNone)
         SetTestStepResult(EFail);        SetTestStepResult(EPass);	return TestStepResult();
	}

TVerdict CCommDbTest036_06::doTestStepPreambleL()
	{
	iTheDb=CCommsDatabase::NewL();

        SetTestStepResult(EPass);	return TestStepResult();
	}

TInt CCommDbTest036_06::executeStepL()
	{
	CCommsDbTemplateRecord* templateRecord = CCommsDbTemplateRecord::NewL(iTheDb, TPtrC(DIAL_IN_ISP));
	CleanupStack::PushL(templateRecord);
	
	User::LeaveIfError(templateRecord->Modify());

	TBufC<KCommsDbSvrMaxFieldLength> inputString = _L("test string");
	templateRecord->WriteTextL(TPtrC(COMMDB_NAME), _L("Name"));
//	templateRecord->WriteTextL(TPtrC(ISP_IF_NAME), _L("SLIP"));
	templateRecord->WriteBoolL(TPtrC(ISP_IP_ADDR_FROM_SERVER), ETrue);
	templateRecord->WriteBoolL(TPtrC(ISP_IP_DNS_ADDR_FROM_SERVER), ETrue);
	templateRecord->WriteBoolL(TPtrC(ISP_IP6_DNS_ADDR_FROM_SERVER), ETrue);
	templateRecord->WriteLongTextL(TPtrC(ISP_LOGIN_SCRIPT), inputString);
	User::LeaveIfError(templateRecord->StoreModifications());

	CleanupStack::PopAndDestroy(templateRecord);

	//Create a view on the DialInISP table, make a new record and change the value we set int he template

	CCommsDbTableView* tableView = iTheDb->OpenTableLC(TPtrC(DIAL_IN_ISP));
	TUint32 dummyId;
	//Create a new record, so we can be sure it is the same as the templated one
	User::LeaveIfError(tableView->InsertRecord(dummyId));
	tableView->WriteTextL(TPtrC(COMMDB_NAME), _L("Test ISP"));
//	tableView->WriteTextL(TPtrC(ISP_IF_NAME), _L("Test If"));
	tableView->WriteBoolL(TPtrC(ISP_IP_ADDR_FROM_SERVER), EFalse);
	tableView->WriteBoolL(TPtrC(ISP_IP_DNS_ADDR_FROM_SERVER), EFalse);
	tableView->WriteBoolL(TPtrC(ISP_IP6_DNS_ADDR_FROM_SERVER), EFalse);
	//Overwrite value set above in template
	TBufC<KCommsDbSvrMaxFieldLength> overwriteString = _L("new string");
	tableView->WriteLongTextL(TPtrC(ISP_LOGIN_SCRIPT), overwriteString);
	User::LeaveIfError(tableView->PutRecordChanges());

	//Retrieve the string we just set
	HBufC* retrievedString = tableView->ReadLongTextLC(TPtrC(ISP_LOGIN_SCRIPT));

	TInt ret(KErrNone);
	if(*retrievedString!=overwriteString)
		ret= KErrGeneral;

	CleanupStack::PopAndDestroy();	//retrievedString
	CleanupStack::PopAndDestroy(tableView);

	return ret;
	}
	
TVerdict CCommDbTest036_06::doTestStepPostambleL()
	{
	delete iTheDb;
	iTheDb=NULL;
        SetTestStepResult(EPass);	return TestStepResult();
	}

/////////////////////////////////////////////////////////////////////////

CCommDbTest036_07::CCommDbTest036_07()
	{
	// store the name of this test case
	SetTestStepName(_L("step_036_07"));
	}

CCommDbTest036_07::~CCommDbTest036_07()
	{}

TVerdict CCommDbTest036_07::doTestStepL( void )
	{
	if(executeStepL()!=KErrNone)
         SetTestStepResult(EFail);        SetTestStepResult(EPass);	return TestStepResult();
	}

TVerdict CCommDbTest036_07::doTestStepPreambleL()
	{
	iTheDb=CCommsDatabase::NewL();

        SetTestStepResult(EPass);	return TestStepResult();
	}

TInt CCommDbTest036_07::executeStepL()
	{
	CCommsDbTemplateRecord* templateRecord = CCommsDbTemplateRecord::NewL(iTheDb, TPtrC(DIAL_IN_ISP));
	CleanupStack::PushL(templateRecord);
	
	User::LeaveIfError(templateRecord->Modify());

	TBool inputBool = ETrue;
	templateRecord->WriteTextL(TPtrC(COMMDB_NAME), _L("Name"));
//	templateRecord->WriteTextL(TPtrC(ISP_IF_NAME), _L("SLIP"));
	templateRecord->WriteBoolL(TPtrC(ISP_IP_ADDR_FROM_SERVER), ETrue);
	templateRecord->WriteBoolL(TPtrC(ISP_IP_DNS_ADDR_FROM_SERVER), ETrue);
	templateRecord->WriteBoolL(TPtrC(ISP_IP6_DNS_ADDR_FROM_SERVER), ETrue);
	templateRecord->WriteBoolL(TPtrC(ISP_USE_LOGIN_SCRIPT), inputBool);	//Set to ETrue, since EFalse is default
	User::LeaveIfError(templateRecord->StoreModifications());

	CleanupStack::PopAndDestroy(templateRecord);

	//Create a view on the DialInISP table, make a new record and check the value is the one set for the template

	CCommsDbTableView* tableView = iTheDb->OpenTableLC(TPtrC(DIAL_IN_ISP));
	TUint32 dummyId;
	//Create a new record, so we can be sure it is the same as the templated one
	User::LeaveIfError(tableView->InsertRecord(dummyId));
	tableView->WriteTextL(TPtrC(COMMDB_NAME), _L("Test ISP"));
//	tableView->WriteTextL(TPtrC(ISP_IF_NAME), _L("Test If"));
	tableView->WriteBoolL(TPtrC(ISP_IP_ADDR_FROM_SERVER), EFalse);
	tableView->WriteBoolL(TPtrC(ISP_IP_DNS_ADDR_FROM_SERVER), EFalse);
	tableView->WriteBoolL(TPtrC(ISP_IP6_DNS_ADDR_FROM_SERVER), EFalse);
	User::LeaveIfError(tableView->PutRecordChanges());

	//Retrieve the bool we set for the template
	TBool retrievedBool;
	tableView->ReadBoolL(TPtrC(ISP_USE_LOGIN_SCRIPT), retrievedBool);

	CleanupStack::PopAndDestroy(tableView);

	if(retrievedBool!=inputBool)
		return KErrGeneral;

	return KErrNone;
	}
	
TVerdict CCommDbTest036_07::doTestStepPostambleL()
	{
	delete iTheDb;
	iTheDb=NULL;
        SetTestStepResult(EPass);	return TestStepResult();
	}

/////////////////////////////////////////////////////////////////////////

CCommDbTest036_08::CCommDbTest036_08()
	{
	// store the name of this test case
	SetTestStepName(_L("step_036_08"));
	}

CCommDbTest036_08::~CCommDbTest036_08()
	{}

TVerdict CCommDbTest036_08::doTestStepL( void )
	{
	if(executeStepL()!=KErrNone)
         SetTestStepResult(EFail);        SetTestStepResult(EPass);	return TestStepResult();
	}

TVerdict CCommDbTest036_08::doTestStepPreambleL()
	{
	iTheDb=CCommsDatabase::NewL();

        SetTestStepResult(EPass);	return TestStepResult();
	}

TInt CCommDbTest036_08::executeStepL()
	{
	CCommsDbTemplateRecord* templateRecord = CCommsDbTemplateRecord::NewL(iTheDb, TPtrC(DIAL_IN_ISP));
	CleanupStack::PushL(templateRecord);
	
	User::LeaveIfError(templateRecord->Modify());

	TBool inputBool = ETrue;
	templateRecord->WriteTextL(TPtrC(COMMDB_NAME), _L("Name"));
//	templateRecord->WriteTextL(TPtrC(ISP_IF_NAME), _L("SLIP"));
	templateRecord->WriteBoolL(TPtrC(ISP_IP_ADDR_FROM_SERVER), ETrue);
	templateRecord->WriteBoolL(TPtrC(ISP_IP_DNS_ADDR_FROM_SERVER), ETrue);
	templateRecord->WriteBoolL(TPtrC(ISP_IP6_DNS_ADDR_FROM_SERVER), ETrue);
	templateRecord->WriteBoolL(TPtrC(ISP_USE_LOGIN_SCRIPT), inputBool);	//Set to ETrue, since EFalse is default
	User::LeaveIfError(templateRecord->StoreModifications());

	CleanupStack::PopAndDestroy(templateRecord);

	//Create a view on the DialInISP table, make a new record and change the value we set in the template

	CCommsDbTableView* tableView = iTheDb->OpenTableLC(TPtrC(DIAL_IN_ISP));
	TUint32 dummyId;
	//Create a new record, so we can be sure it is the same as the templated one
	User::LeaveIfError(tableView->InsertRecord(dummyId));
	tableView->WriteTextL(TPtrC(COMMDB_NAME), _L("Test ISP"));
//	tableView->WriteTextL(TPtrC(ISP_IF_NAME), _L("Test If"));
	tableView->WriteBoolL(TPtrC(ISP_IP_ADDR_FROM_SERVER), EFalse);
	tableView->WriteBoolL(TPtrC(ISP_IP_DNS_ADDR_FROM_SERVER), EFalse);
	tableView->WriteBoolL(TPtrC(ISP_IP6_DNS_ADDR_FROM_SERVER), EFalse);
	//Overwrite the value set in the template
	TBool overwriteBool = EFalse;
	tableView->WriteBoolL(TPtrC(ISP_USE_LOGIN_SCRIPT), overwriteBool);
	User::LeaveIfError(tableView->PutRecordChanges());

	//Retrieve the bool we just set for the record
	TBool retrievedBool;
	tableView->ReadBoolL(TPtrC(ISP_USE_LOGIN_SCRIPT), retrievedBool);

	CleanupStack::PopAndDestroy(tableView);

	if(retrievedBool!=overwriteBool)
		return KErrGeneral;

	return KErrNone;
	}
	
TVerdict CCommDbTest036_08::doTestStepPostambleL()
	{
	delete iTheDb;
	iTheDb=NULL;
        SetTestStepResult(EPass);	return TestStepResult();
	}

/////////////////////////////////////////////////////////////////////////

CCommDbTest036_09::CCommDbTest036_09()
	{
	// store the name of this test case
	SetTestStepName(_L("step_036_09"));
	}

CCommDbTest036_09::~CCommDbTest036_09()
	{}

TVerdict CCommDbTest036_09::doTestStepL( void )
	{
	if(executeStepL()!=KErrNone)
         SetTestStepResult(EFail);        SetTestStepResult(EPass);	return TestStepResult();
	}

TVerdict CCommDbTest036_09::doTestStepPreambleL()
	{
	iTheDb=CCommsDatabase::NewL();

        SetTestStepResult(EPass);	return TestStepResult();
	}

TInt CCommDbTest036_09::executeStepL()
	{
	CCommsDbTemplateRecord* templateRecord = CCommsDbTemplateRecord::NewL(iTheDb, TPtrC(DIAL_IN_ISP));
	CleanupStack::PushL(templateRecord);
	User::LeaveIfError(templateRecord->Modify());

	TUint32 inputInt = RMobileCall::KCapsSpeed32000;
	templateRecord->WriteTextL(TPtrC(COMMDB_NAME), _L("Name"));
	templateRecord->WriteTextL(TPtrC(ISP_IP_NAME_SERVER1),_L("MyDnsServer"));
	templateRecord->WriteBoolL(TPtrC(ISP_IP_ADDR_FROM_SERVER), ETrue);
	templateRecord->WriteBoolL(TPtrC(ISP_IP_DNS_ADDR_FROM_SERVER), ETrue);
	templateRecord->WriteBoolL(TPtrC(ISP_IP6_DNS_ADDR_FROM_SERVER), ETrue);
	templateRecord->WriteUintL(TPtrC(ISP_BEARER_SPEED), inputInt);
	User::LeaveIfError(templateRecord->StoreModifications());

	CleanupStack::PopAndDestroy(templateRecord);

	//Create a view on the DialInISP table, make a new record and check the value is the one set for the template

	CCommsDbTableView* tableView = iTheDb->OpenTableLC(TPtrC(DIAL_IN_ISP));
	TUint32 dummyId;
	//Create a new record, so we can be sure it is the same as the templated one
	User::LeaveIfError(tableView->InsertRecord(dummyId));
	tableView->WriteTextL(TPtrC(COMMDB_NAME), _L("Test ISP"));
	tableView->WriteTextL(TPtrC(ISP_IP_NAME_SERVER1),_L("NotMyDnsServer"));
	tableView->WriteBoolL(TPtrC(ISP_IP_ADDR_FROM_SERVER), EFalse);
	tableView->WriteBoolL(TPtrC(ISP_IP_DNS_ADDR_FROM_SERVER), EFalse);
	tableView->WriteBoolL(TPtrC(ISP_IP6_DNS_ADDR_FROM_SERVER), EFalse);
	User::LeaveIfError(tableView->PutRecordChanges());

	//Retrieve the TUint32 we set for the template
	TUint32 retrievedInt;
	tableView->ReadUintL(TPtrC(ISP_BEARER_SPEED), retrievedInt);
	// And now check that we retrieve the text from the template

	TBuf16<KCommsDbSvrMaxFieldLength> ifAuthName(_L16("Invalid"));
	tableView->ReadTextL(TPtrC(ISP_IP_NAME_SERVER1), ifAuthName);
	if(ifAuthName.Compare(_L16("NotMyDnsServer")))
		{
		User::Leave(KErrGeneral);
		}

	CleanupStack::PopAndDestroy(tableView);

	if(retrievedInt!=inputInt)
		return KErrGeneral;

	return KErrNone;
	}
	
TVerdict CCommDbTest036_09::doTestStepPostambleL()
	{
	delete iTheDb;
	iTheDb=NULL;
        SetTestStepResult(EPass);	return TestStepResult();
	}

/////////////////////////////////////////////////////////////////////////

CCommDbTest036_10::CCommDbTest036_10()
	{
	// store the name of this test case
	SetTestStepName(_L("step_036_10"));
	}

CCommDbTest036_10::~CCommDbTest036_10()
	{}

TVerdict CCommDbTest036_10::doTestStepL( void )
	{
	if(executeStepL()!=KErrNone)
         SetTestStepResult(EFail);        SetTestStepResult(EPass);	return TestStepResult();
	}

TVerdict CCommDbTest036_10::doTestStepPreambleL()
	{
	iTheDb=CCommsDatabase::NewL();

        SetTestStepResult(EPass);	return TestStepResult();
	}

TInt CCommDbTest036_10::executeStepL()
	{
	CCommsDbTemplateRecord* templateRecord = CCommsDbTemplateRecord::NewL(iTheDb, TPtrC(DIAL_IN_ISP));
	CleanupStack::PushL(templateRecord);
	
	User::LeaveIfError(templateRecord->Modify());

	TUint32 inputInt = RMobileCall::KCapsSpeed32000;
	templateRecord->WriteTextL(TPtrC(COMMDB_NAME), _L("Name"));
	templateRecord->WriteTextL(TPtrC(ISP_IP_NAME_SERVER1),_L("MyDnsServer"));
	templateRecord->WriteBoolL(TPtrC(ISP_IP_ADDR_FROM_SERVER), ETrue);
	templateRecord->WriteBoolL(TPtrC(ISP_IP_DNS_ADDR_FROM_SERVER), ETrue);
	templateRecord->WriteBoolL(TPtrC(ISP_IP6_DNS_ADDR_FROM_SERVER), ETrue);
	templateRecord->WriteUintL(TPtrC(ISP_BEARER_SPEED), inputInt);
	User::LeaveIfError(templateRecord->StoreModifications());

	CleanupStack::PopAndDestroy(templateRecord);

	//Create a view on the DialInISP table, make a new record and change the value we set in the template
	CCommsDbTableView* tableView = iTheDb->OpenTableLC(TPtrC(DIAL_IN_ISP));
	TUint32 dummyId;
	//Create a new record, so we can be sure it is the same as the templated one
	User::LeaveIfError(tableView->InsertRecord(dummyId));
	tableView->WriteTextL(TPtrC(COMMDB_NAME), _L("Test ISP"));
	tableView->WriteBoolL(TPtrC(ISP_IP_ADDR_FROM_SERVER), EFalse);
	tableView->WriteBoolL(TPtrC(ISP_IP_DNS_ADDR_FROM_SERVER), EFalse);
	tableView->WriteBoolL(TPtrC(ISP_IP6_DNS_ADDR_FROM_SERVER), EFalse);
	//Overwrite value we set in the template
	TUint32 overwriteInt = RMobileCall::KCapsSpeed64000;
	tableView->WriteUintL(TPtrC(ISP_BEARER_SPEED), overwriteInt);
	User::LeaveIfError(tableView->PutRecordChanges());

	//Retrieve the TUint32 we just set
	TUint32 retrievedInt;
	tableView->ReadUintL(TPtrC(ISP_BEARER_SPEED), retrievedInt);

	TBuf16<KCommsDbSvrMaxFieldLength> ifAuthName(_L16("Invalid"));
	tableView->ReadTextL(TPtrC(ISP_IP_NAME_SERVER1), ifAuthName);
	if(ifAuthName.Compare(_L16("MyDnsServer")))
		{
		User::Leave(KErrGeneral);
		}

	CleanupStack::PopAndDestroy(tableView);

	if(retrievedInt!=overwriteInt)
		return KErrGeneral;

	return KErrNone;
	}
	
TVerdict CCommDbTest036_10::doTestStepPostambleL()
	{
	delete iTheDb;
	iTheDb=NULL;
        SetTestStepResult(EPass);	return TestStepResult();
	}

/////////////////////////////////////////////////////////////////////////

CCommDbTest036_11::CCommDbTest036_11()
	{
	// store the name of this test case
	SetTestStepName(_L("step_036_11"));
	}

CCommDbTest036_11::~CCommDbTest036_11()
	{}

TVerdict CCommDbTest036_11::doTestStepL( void )
	{
	executeStepL();
        SetTestStepResult(EFail);	return TestStepResult();	//Test is supposed to panic, so if we get here we've failed
	}

TVerdict CCommDbTest036_11::doTestStepPreambleL()
	{
	iTheDb=CCommsDatabase::NewL();

        SetTestStepResult(EPass);	return TestStepResult();
	}

TInt CCommDbTest036_11::executeStepL()
	{
	CCommsDbTemplateRecord* templateRecord = CCommsDbTemplateRecord::NewL(iTheDb, TPtrC(DIAL_IN_ISP));
	CleanupStack::PushL(templateRecord);
	
	templateRecord->StoreModifications();

	CleanupStack::PopAndDestroy(templateRecord);

	return KErrGeneral;
	}
	
TVerdict CCommDbTest036_11::doTestStepPostambleL()
	{
	delete iTheDb;
	iTheDb=NULL;
        SetTestStepResult(EPass);	return TestStepResult();
	}

/////////////////////////////////////////////////////////////////////////

CCommDbTest036_12::CCommDbTest036_12()
	{
	// store the name of this test case
	SetTestStepName(_L("step_036_12"));
	}

CCommDbTest036_12::~CCommDbTest036_12()
	{}

TVerdict CCommDbTest036_12::doTestStepL( void )
	{
	executeStepL();
        SetTestStepResult(EFail);	return TestStepResult();	//Test should panic, so if we get here something's wrong
	}

TVerdict CCommDbTest036_12::doTestStepPreambleL()
	{
	iTheDb=CCommsDatabase::NewL();

        SetTestStepResult(EPass);	return TestStepResult();
	}

TInt CCommDbTest036_12::executeStepL()
	{
	CCommsDbTemplateRecord* templateRecord = CCommsDbTemplateRecord::NewL(iTheDb, TPtrC(DIAL_IN_ISP));
	CleanupStack::PushL(templateRecord);
	
	templateRecord->WriteBoolL(TPtrC(ISP_USE_LOGIN_SCRIPT), ETrue);

	CleanupStack::PopAndDestroy(templateRecord);
	return KErrGeneral;
	}
	
TVerdict CCommDbTest036_12::doTestStepPostambleL()
	{
	delete iTheDb;
	iTheDb=NULL;
        SetTestStepResult(EPass);	return TestStepResult();
	}

/////////////////////////////////////////////////////////////////////////

CCommDbTest036_13::CCommDbTest036_13()
	{
	// store the name of this test case
	SetTestStepName(_L("step_036_13"));
	}

CCommDbTest036_13::~CCommDbTest036_13()
	{}

TVerdict CCommDbTest036_13::doTestStepL( void )
	{
	if (executeStepL()!=KErrNotFound)
         SetTestStepResult(EFail);        SetTestStepResult(EPass);	return TestStepResult();
	}

TVerdict CCommDbTest036_13::doTestStepPreambleL()
	{
	iTheDb=CCommsDatabase::NewL();

        SetTestStepResult(EPass);	return TestStepResult();
	}

TInt CCommDbTest036_13::executeStepL()
	{
	CCommsDbTemplateRecord* templateRecord = CCommsDbTemplateRecord::NewL(iTheDb, TPtrC(DIAL_IN_ISP));
	CleanupStack::PushL(templateRecord);

	User::LeaveIfError(templateRecord->Modify());
	TRAPD(err, templateRecord->WriteBoolL(_L("Invalid"), ETrue));

	CleanupStack::PopAndDestroy(templateRecord);
	return err;
	}
	
TVerdict CCommDbTest036_13::doTestStepPostambleL()
	{
	delete iTheDb;
	iTheDb=NULL;
        SetTestStepResult(EPass);	return TestStepResult();
	}

/////////////////////////////////////////////////////////////////////////

CCommDbTest036_14::CCommDbTest036_14()
	{
	// store the name of this test case
	SetTestStepName(_L("step_036_14"));
	}

CCommDbTest036_14::~CCommDbTest036_14()
	{}

TVerdict CCommDbTest036_14::doTestStepL( void )
	{
	if (executeStepL()!=KErrNotFound)
         SetTestStepResult(EFail);        SetTestStepResult(EPass);	return TestStepResult();
	}

TVerdict CCommDbTest036_14::doTestStepPreambleL()
	{
	iTheDb=CCommsDatabase::NewL();

        SetTestStepResult(EPass);	return TestStepResult();
	}

TInt CCommDbTest036_14::executeStepL()
	{
	CCommsDbTemplateRecord* templateRecord = CCommsDbTemplateRecord::NewL(iTheDb, TPtrC(DIAL_IN_ISP));
	CleanupStack::PushL(templateRecord);

	User::LeaveIfError(templateRecord->Modify());
	TRAPD(err, templateRecord->WriteBoolL(_L(""), ETrue));

	CleanupStack::PopAndDestroy(templateRecord);
	return err;
	}
	
TVerdict CCommDbTest036_14::doTestStepPostambleL()
	{
	delete iTheDb;
	iTheDb=NULL;
        SetTestStepResult(EPass);	return TestStepResult();
	}

/////////////////////////////////////////////////////////////////////////

CCommDbTest036_15::CCommDbTest036_15()
	{
	// store the name of this test case
	SetTestStepName(_L("step_036_15"));
	}

CCommDbTest036_15::~CCommDbTest036_15()
	{}

TVerdict CCommDbTest036_15::doTestStepL( void )
	{
	if (executeStepL()!=KErrNotFound)
         SetTestStepResult(EFail);        SetTestStepResult(EPass);	return TestStepResult();
	}

TVerdict CCommDbTest036_15::doTestStepPreambleL()
	{
	iTheDb=CCommsDatabase::NewL();

        SetTestStepResult(EPass);	return TestStepResult();
	}

TInt CCommDbTest036_15::executeStepL()
	{
	CCommsDbTemplateRecord* templateRecord = CCommsDbTemplateRecord::NewL(iTheDb, TPtrC(DIAL_IN_ISP));
	CleanupStack::PushL(templateRecord);

	TBool dummyBool;
	TRAPD(err, templateRecord->ReadBoolL(_L("Invalid"), dummyBool));

	CleanupStack::PopAndDestroy(templateRecord);
	return err;
	}
	
TVerdict CCommDbTest036_15::doTestStepPostambleL()
	{
	delete iTheDb;
	iTheDb=NULL;
        SetTestStepResult(EPass);	return TestStepResult();
	}

//////////////////////////////////////////////////////////////////////////

CCommDbTest036_16::CCommDbTest036_16()
	{
	// store the name of this test case
	SetTestStepName(_L("step_036_16"));
	}

CCommDbTest036_16::~CCommDbTest036_16()
	{}

TVerdict CCommDbTest036_16::doTestStepL( void )
	{
	INFO_PRINTF1(_L("Step 036.16 called "));

	SetTestStepResult(EPass);
	CCommDbTest036_01* step036_01 = new(ELeave) CCommDbTest036_01;
	CleanupStack::PushL(step036_01);
	doTestStepWithHeapFailureL( *step036_01, KErrNone);
	CleanupStack::PopAndDestroy(step036_01);

	CCommDbTest036_02* step036_02 = new(ELeave) CCommDbTest036_02;
	CleanupStack::PushL(step036_02);
	doTestStepWithHeapFailureL( *step036_02, KErrNone);
	CleanupStack::PopAndDestroy(step036_02);

	CCommDbTest036_03* step036_03 = new(ELeave) CCommDbTest036_03;
	CleanupStack::PushL(step036_03);
	doTestStepWithHeapFailureL( *step036_03, KErrNone);
	CleanupStack::PopAndDestroy(step036_03);
	
	CCommDbTest036_04* step036_04 = new(ELeave) CCommDbTest036_04;
	CleanupStack::PushL(step036_04);
	doTestStepWithHeapFailureL( *step036_04, KErrNone);
	CleanupStack::PopAndDestroy(step036_04);

	CCommDbTest036_05* step036_05 = new(ELeave) CCommDbTest036_05;
	CleanupStack::PushL(step036_05);
	doTestStepWithHeapFailureL( *step036_05, KErrNone);
	CleanupStack::PopAndDestroy(step036_05);

	CCommDbTest036_06* step036_06 = new(ELeave) CCommDbTest036_06;
	CleanupStack::PushL(step036_06);
	doTestStepWithHeapFailureL( *step036_06, KErrNone);
	CleanupStack::PopAndDestroy(step036_06);

	CCommDbTest036_07* step036_07 = new(ELeave) CCommDbTest036_07;
	CleanupStack::PushL(step036_07);
	doTestStepWithHeapFailureL( *step036_07, KErrNone);
	CleanupStack::PopAndDestroy(step036_07);

	CCommDbTest036_08* step036_08 = new(ELeave) CCommDbTest036_08;
	CleanupStack::PushL(step036_08);
	doTestStepWithHeapFailureL( *step036_08, KErrNone);
	CleanupStack::PopAndDestroy(step036_08);

	CCommDbTest036_09* step036_09 = new(ELeave) CCommDbTest036_09;
	CleanupStack::PushL(step036_09);
	doTestStepWithHeapFailureL( *step036_09, KErrNone);
	CleanupStack::PopAndDestroy(step036_09);

	CCommDbTest036_10* step036_10 = new(ELeave) CCommDbTest036_10;
	CleanupStack::PushL(step036_10);
	doTestStepWithHeapFailureL( *step036_10, KErrNone);
	CleanupStack::PopAndDestroy(step036_10);

	CCommDbTest036_13* step036_13 = new(ELeave) CCommDbTest036_13;
	CleanupStack::PushL(step036_13);
	doTestStepWithHeapFailureL( *step036_13, KErrNotFound);
	CleanupStack::PopAndDestroy(step036_13);

	CCommDbTest036_14* step036_14 = new(ELeave) CCommDbTest036_14;
	CleanupStack::PushL(step036_14);
	doTestStepWithHeapFailureL( *step036_14, KErrNotFound);
	CleanupStack::PopAndDestroy(step036_14);

	CCommDbTest036_15* step036_15 = new(ELeave) CCommDbTest036_15;
	CleanupStack::PushL(step036_15);
	doTestStepWithHeapFailureL( *step036_15, KErrNotFound);
	CleanupStack::PopAndDestroy(step036_15);

	return TestStepResult();
	}

//////////////////////////////////////////////////////////////////////////

CCommDbTest036_17::CCommDbTest036_17()
	{
	// store the name of this test case
	SetTestStepName(_L("step_036_17"));
	}

CCommDbTest036_17::~CCommDbTest036_17()
	{}

TVerdict CCommDbTest036_17::doTestStepL( void )
	{

	SetTestStepResult(EPass);
	CCommDbTest036_11* step036_11 = new(ELeave) CCommDbTest036_11;
	CleanupStack::PushL(step036_11);
	doTestStepWithHeapFailureL( *step036_11, KErrNone);
	CleanupStack::PopAndDestroy(step036_11);

	return TestStepResult();
	}

//////////////////////////////////////////////////////////////////////////

CCommDbTest036_18::CCommDbTest036_18()
	{
	// store the name of this test case
	SetTestStepName(_L("step_036_18"));
	}

CCommDbTest036_18::~CCommDbTest036_18()
	{}

TVerdict CCommDbTest036_18::doTestStepL( void )
	{

	SetTestStepResult(EPass);
	CCommDbTest036_12* step036_12 = new(ELeave) CCommDbTest036_12;
	CleanupStack::PushL(step036_12);
	doTestStepWithHeapFailureL( *step036_12, KErrNone);
	CleanupStack::PopAndDestroy(step036_12);

	return TestStepResult();
	}
