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
// CCommsdat80Step
// CCommsdat81Step
// CCommsdat82Step
// CCommsdat83Step
// CCommsdat84Step
// CCommsdat85Step
// CCommsdat86Step
// CCommsdat87Step
// CCommsdat88Step
// 
//

/**
 @file commsdatGenericRecordTests.cpp
*/
#include "commsdatGenericRecordTests.h"
#include "Te_commsdatSuiteDefs.h"
#include <e32test.h>
#ifdef SYMBIAN_ENABLE_SPLIT_HEADERS
#include <commsdat_partner.h>
#endif

// table name
_LIT(KCDTypeNameTestTable, "Test");



// 'CCDRecordBase' means of accessing the same table.

	#define	KCDTypeNameTestIntegerField		      	_S("TestIntegerField")
	#define	KCDTypeNameTestStringField		          _S("TestStringField")
	#define	KCDTypeNameTestMediumStringField		  _S("A ftest name which is 64 characters; that is the limit defined")


class CCDTestRecord : public CCDRecordBase
	{
public :

	// Constructor
    CCDTestRecord(TMDBElementId aElementId)
        : CCDRecordBase(aElementId), 
          iIntegerField((aElementId & KCDMaskShowRecordType) | KCDTIdTestIntegerField),
          iStringField((aElementId & KCDMaskShowRecordType) | KCDTIdTestStringField),
          iMediumStringField((aElementId & KCDMaskShowRecordType) | KCDTIdTestMediumStringField),
          iRecordTag((aElementId & KCDMaskShowRecordType) | KCDTIdRecordTag),
          iRecordName((aElementId & KCDMaskShowRecordType) | KCDTIdRecordName)
        {}
        
    const SRecordTypeInfo* GetRecordInfo(){return iRecordInfo;}    

	DATA_VTABLE

public :

	CMDBField<TUint32>	iIntegerField;
	CMDBField<TDesC>	iStringField;
	CMDBField<TDesC>	iMediumStringField;
	
	
	CMDBField<TInt>	 iRecordTag;  
	CMDBField<TDesC> iRecordName;   
	
private:
    
		static const SRecordTypeInfo iRecordInfo[];	
	};
	
	
const SRecordTypeInfo CCDTestRecord::iRecordInfo[]=
  	{
		{ KCDTIdRecordTag,	        EInt,		ENoAttrs,   KCDTypeNameRecordTag },
  		{ KCDTIdRecordName,     	EText,		ENotNull,   KCDTypeNameRecordName },
		{ KCDTIdTestIntegerField,	EInt,		ENotNull,   KCDTypeNameWAPCurrentBearer },
		{ KCDTIdTestStringField,    EText,  	ENoAttrs,   KCDTypeNameWAPStartPage },
		{ KCDTIdTestMediumStringField,    EText,  	ENoAttrs,   KCDTypeNameTestMediumStringField },
  		{ 0, 0, ENoAttrs, KCDNull }
  	};	


START_ATTRIBUTE_TABLE(CCDTestRecord, KCDTIdTestRecord,  KCDTIdTestRecord )
	X_REGISTER_ATTRIBUTE(CCDTestRecord,   iIntegerField, TMDBNum)
	X_REGISTER_ATTRIBUTE(CCDTestRecord,   iStringField,  TMDBText)
	X_REGISTER_ATTRIBUTE(CCDTestRecord,   iMediumStringField,  TMDBText)
	X_REGISTER_ATTRIBUTE(CCDTestRecord,   iRecordTag,    TMDBNum )
	X_REGISTER_ATTRIBUTE(CCDTestRecord,   iRecordName,   TMDBText )
END_ATTRIBUTE_TABLE()

const TUint TestRecordId = 1;   
const TUint TestIntegerVal = 6;   
const TUint TestTagValue = 5;  
const TUint TestTagValue1 = 100;  

void DoTableProtectionL(CMDBRecordSetBase* aRecordSet, CMDBSession* aDbSession, TBool aProtect)
    {
   // backup hidden flag
   TBool showHidden = aDbSession->IsSetAttributeMask(ECDHidden); 
    // set all attribute masks as must be able to see every record (already set for ECDPrivate)
   aDbSession->SetAttributeMask(ECDHidden | ECDNoWriteButDelete | ECDProtectedWrite ); 
    // (re)set Table protection
   aProtect ? aRecordSet->SetAttributes(ECDProtectedWrite) : aRecordSet->ClearAttributes(ECDProtectedWrite);
   aRecordSet->ModifyL(*aDbSession);
    // restore hidden flag
   showHidden ? aDbSession->ClearAttributeMask(ECDNoWriteButDelete | ECDProtectedWrite)
              : aDbSession->ClearAttributeMask(ECDHidden | ECDNoWriteButDelete | ECDProtectedWrite);
    }

TVerdict CGenericRecordTestBase::doTestStepPreambleL()
	{
	INFO_PRINTF1(_L("Creating  a user defined table"));
	SetTestStepResult(EFail);
	
#ifdef SYMBIAN_NON_SEAMLESS_NETWORK_BEARER_MOBILITY
 	CMDBSession *cmdbSession = CMDBSession::NewL(KCDVersion1_2);
#else
 	CMDBSession *cmdbSession = CMDBSession::NewL(KCDVersion1_1);
#endif
	CleanupStack::PushL(cmdbSession); 

	CMDBGenericRecord* testRecord = static_cast<CMDBGenericRecord*>(CCDRecordBase::RecordFactoryL(KCDNewTableRequest));
	CleanupStack::PushL(testRecord); 
    testRecord->InitializeL(KCDTypeNameTestTable(), TestGenRecordInfo);	
         
	testRecord->StoreL(*cmdbSession);
	TMDBElementId id = testRecord->TableId();
	
	// Check the value of the TableId that has been assigned
	TESTL(id == KCDTIdTestRecord);
	CleanupStack::PopAndDestroy(testRecord);
	
	//Firs load the schema
	testRecord = static_cast<CMDBGenericRecord *>(CCDRecordBase::RecordFactoryL(KCDTIdTestRecord/*todo..hardcoded testRecord->TypeId() & KCDMaskShowRecordType*/));
	CleanupStack::PushL(testRecord);
	
	testRecord->LoadL(*cmdbSession);
		
	INFO_PRINTF1(_L("Storing a new Generic record "));	
 
	//Store a record
    testRecord->SetRecordId(KCDNewRecordRequest);

	CMDBField<TUint32>* recordTag = static_cast<CMDBField<TUint32>*>(testRecord->GetFieldByIdL(KCDTIdRecordTag));
	*recordTag = TestTagValue1;
	_LIT(KRecordName, "Test Record 1");
	CMDBField<TDesC>* recordName = static_cast<CMDBField<TDesC>*>(testRecord->GetFieldByIdL(KCDTIdRecordName));
	recordName->SetMaxLengthL(KRecordName().Length());
	*recordName = KRecordName;
	CMDBField<TUint32>* testInteger = static_cast<CMDBField<TUint32>*>(testRecord->GetFieldByIdL(KCDTIdTestIntegerField));
	*testInteger = TestIntegerVal;	       	
	_LIT(KTestString, "TestString!!");
	CMDBField<TDesC>* testString = static_cast<CMDBField<TDesC>*>(testRecord->GetFieldByIdL(KCDTIdTestStringField));
	testString->SetMaxLengthL(KTestString().Length());
	*testString = KTestString;
	
	_LIT(KTestMediumString, "MediumString");
	CMDBField<TDesC>* testMediumString = static_cast<CMDBField<TDesC>*>(testRecord->GetFieldByIdL(KCDTIdTestMediumStringField));
	testMediumString->SetMaxLengthL(KTestMediumString().Length());
	*testMediumString = KTestMediumString;

	testRecord->StoreL(*cmdbSession);
	CleanupStack::PopAndDestroy(testRecord);	
	CleanupStack::PopAndDestroy(cmdbSession);
	
	SetTestStepResult(EPass);
	return TestStepResult();	
	}
	
//----------------------------------------------------------------------------------------------------------------	
CCommsdat80Step::CCommsdat80Step()
/**
 * Constructor
 */
	{
	SetTestStepName(KCommsdat80Step);                                                                                           
	}

TVerdict CCommsdat80Step::doTestStepL()
/**
 * @return - TVerdict code
 * Create a user defined table 
 */
	{
    SetTestStepResult(EFail);	
	INFO_PRINTF1(_L("CCommsdat80Step::doTestStepL()"));
	
	// Create a new table
#ifdef SYMBIAN_NON_SEAMLESS_NETWORK_BEARER_MOBILITY
 	CMDBSession *cmdbSession = CMDBSession::NewL(KCDVersion1_2);
#else
 	CMDBSession *cmdbSession = CMDBSession::NewL(KCDVersion1_1);
#endif
	CleanupStack::PushL(cmdbSession); 

	CMDBGenericRecord* testRecord = static_cast<CMDBGenericRecord*>(CCDRecordBase::RecordFactoryL(KCDNewTableRequest));
	CleanupStack::PushL(testRecord); 
    testRecord->InitializeL(KCDTypeNameTestTable(), TestGenRecordInfo);	
         
	testRecord->StoreL(*cmdbSession);
	TMDBElementId id = testRecord->TableId();
	
	// Check the value of the TableId that has been assigned
	TESTL(id == KCDTIdTestRecord);

	CleanupStack::PopAndDestroy(testRecord);
	
	//to test you wouldnt delete the schema stored above
	CMDBRecordSet<CMDBGenericRecord>* GenericRecordSet1 = new(ELeave) CMDBRecordSet<CMDBGenericRecord>(KCDTIdTestRecord);
	CleanupStack::PushL(GenericRecordSet1);
	GenericRecordSet1->DeleteL(*cmdbSession);
	CleanupStack::PopAndDestroy(GenericRecordSet1);
	
	// Create a new table with the same table as above: Fail case
	CMDBGenericRecord* testRecord2 = static_cast<CMDBGenericRecord*>(CCDRecordBase::RecordFactoryL(KCDNewTableRequest));
	CleanupStack::PushL(testRecord2);
    testRecord2->InitializeL(KCDTypeNameTestTable(), TestGenRecordInfo);	
	TRAPD(storeErr, testRecord2->StoreL(*cmdbSession));
	TESTL(storeErr == KErrAlreadyExists);
	CleanupStack::PopAndDestroy(testRecord2);
	CleanupStack::PopAndDestroy(cmdbSession);
	
	SetTestStepResult(EPass);
	return TestStepResult();		
	}

//----------------------------------------------------------------------------------------------------------------
CCommsdat81Step::CCommsdat81Step()
/**
 * Constructor
 */
	{
	SetTestStepName(KCommsdat81Step);                                                                                           
	}

_LIT(KRecordName, "Test Record 4");
_LIT(KTestString, "TestString 4!!");

TVerdict CCommsdat81Step::doTestStepL()
/**
 * @return - TVerdict code
 *	Test for Load, Store, Delete, GetFieldByNameL and GetFieldByIdL of generic records
 */
	{
 	SetTestStepResult(EFail);	
	INFO_PRINTF1(_L("CCommsdat81Step::doTestStepL()"));

#ifdef SYMBIAN_NON_SEAMLESS_NETWORK_BEARER_MOBILITY
 	CMDBSession *cmdbSession = CMDBSession::NewL(KCDVersion1_2);
#else
 	CMDBSession *cmdbSession = CMDBSession::NewL(KCDVersion1_1);
#endif
	CleanupStack::PushL(cmdbSession);

	CMDBGenericRecord* record1 = static_cast<CMDBGenericRecord *>(CCDRecordBase::RecordFactoryL(KCDTIdTestRecord/*todo..hardcoded testRecord->TypeId() & KCDMaskShowRecordType*/));
	CleanupStack::PushL(record1);
	
	//This loads the table schema
	record1->LoadL(*cmdbSession);
		
	//STORE 3 new records//
	INFO_PRINTF1(_L("Storing a new Generic record "));	
 
    record1->SetRecordId(KCDNewRecordRequest);

	//Record 1
	CMDBField<TUint32>* recordTag = static_cast<CMDBField<TUint32>*>(record1->GetFieldByIdL(KCDTIdRecordTag));
	*recordTag = TestTagValue1;
	_LIT(KRecordName, "Test Record 1");
	CMDBField<TDesC>* recordName = static_cast<CMDBField<TDesC>*>(record1->GetFieldByIdL(KCDTIdRecordName));
	recordName->SetMaxLengthL(KRecordName().Length());
	*recordName = KRecordName;
	CMDBField<TUint32>* testInteger = static_cast<CMDBField<TUint32>*>(record1->GetFieldByIdL(KCDTIdTestIntegerField));
	*testInteger = TestIntegerVal;	       	
	_LIT(KTestString, "TestString!!");
	CMDBField<TDesC>* testString = static_cast<CMDBField<TDesC>*>(record1->GetFieldByIdL(KCDTIdTestStringField));
	testString->SetMaxLengthL(KTestString().Length());
	*testString = KTestString;
	record1->StoreL(*cmdbSession);
	CleanupStack::PopAndDestroy(record1);	
	
	//Record 2
	CMDBGenericRecord* record2 = static_cast<CMDBGenericRecord *>(CCDRecordBase::RecordFactoryL(KCDTIdTestRecord));
	CleanupStack::PushL(record2);
	record2->LoadL(*cmdbSession);
	record2->SetRecordId(KCDNewRecordRequest);
	CMDBField<TUint32>* recordTag1 = static_cast<CMDBField<TUint32>*>(record2->GetFieldByIdL(KCDTIdRecordTag));
	*recordTag1 = TestTagValue1;
	record2->StoreL(*cmdbSession);
	
	//Record 3
	CMDBGenericRecord* record3 = static_cast<CMDBGenericRecord *>(CCDRecordBase::RecordFactoryL(KCDTIdTestRecord));
	CleanupStack::PushL(record3);
	record3->LoadL(*cmdbSession);
	record3->SetRecordId(KCDNewRecordRequest);
	CMDBField<TUint32>* recordTag2 = static_cast<CMDBField<TUint32>*>(record3->GetFieldByIdL(KCDTIdRecordTag));
	*recordTag2 = TestTagValue;
	record3->StoreL(*cmdbSession);

	//delete the last 2 records		
	record2->DeleteL(*cmdbSession);
	record3->DeleteL(*cmdbSession);
	CleanupStack::PopAndDestroy(record3); 
	CleanupStack::PopAndDestroy(record2);

	//LOAD the above stored records// 
	 
	 //Only ID is set
	INFO_PRINTF1(_L("Loading a  Generic record with table identified by its ID "));	
	TGenericTypeName roundTripStr;
	TInt ignoreThis;

	CMDBGenericRecord* record4 = static_cast<CMDBGenericRecord *>(CCDRecordBase::RecordFactoryL(KCDTIdTestRecord/*testRecord->TypeId() & KCDMaskShowRecordType*/));
	CleanupStack::PushL(record4);
	record4->SetRecordId(TestRecordId);
	record4->LoadL(*cmdbSession); 
	CMDBField<TUint32>* LoadInteger = static_cast<CMDBField<TUint32>*>(record4->GetFieldByIdL(KCDTIdTestIntegerField));
	CMDBField<TDesC>* LoadStr = NULL;
	LoadStr = static_cast<CMDBField<TDesC>*>(record4->GetFieldByNameL(KCDTypeNameStringField, ignoreThis));
    roundTripStr.Copy(*LoadStr);
	TESTL(*LoadInteger == TestIntegerVal && roundTripStr == KTestString);
	
	CleanupStack::PopAndDestroy(record4);
	 
	 //Only Table Name is set
	INFO_PRINTF1(_L("Loading a  Generic record with table identified by its Name "));	
	CMDBGenericRecord* record5 = static_cast<CMDBGenericRecord *>(CCDRecordBase::RecordFactoryL(0));
	CleanupStack::PushL(record5);
	record5->InitializeL(KCDTypeNameTestTable(), NULL);
	record5->SetRecordId(TestRecordId);
	record5->LoadL(*cmdbSession); 
	CMDBField<TUint32>* LoadInteger1 = static_cast<CMDBField<TUint32>*>(record5->GetFieldByIdL(KCDTIdTestIntegerField));
	CMDBField<TDesC>* LoadStr1 = NULL;
	TInt ignoreThis1;
	LoadStr1 = static_cast<CMDBField<TDesC>*>(record5->GetFieldByNameL(KCDTypeNameStringField, ignoreThis1));
	TGenericTypeName roundTripStr1;
    roundTripStr1.Copy(*LoadStr1);

	TESTL(*LoadInteger1 == TestIntegerVal && roundTripStr == KTestString);
		
	CleanupStack::PopAndDestroy(record5);
	CleanupStack::PopAndDestroy(cmdbSession);
	SetTestStepResult(EPass);
	return TestStepResult(); 
	}

//----------------------------------------------------------------------------------------------------------------
CCommsdat82Step::CCommsdat82Step()
/**
 * Constructor
 */
	{
	SetTestStepName(KCommsdat82Step);                                                                                           
	}

TVerdict CCommsdat82Step::doTestStepL()
/**
 * @return - TVerdict code
 *	Test for generic record recordset [Expects  table and a record to be in the db ]
 */
	{
	SetTestStepResult(EFail);	
	INFO_PRINTF1(_L("CCommsdat82Step::doTestStepL()"));

#ifdef SYMBIAN_NON_SEAMLESS_NETWORK_BEARER_MOBILITY
 	CMDBSession *cmdbSession = CMDBSession::NewL(KCDVersion1_2);
#else
 	CMDBSession *cmdbSession = CMDBSession::NewL(KCDVersion1_1);
#endif
	CleanupStack::PushL(cmdbSession);

	//LOAD the record using record set//  
	//Initialization [Add 2 more records]
	CMDBGenericRecord* record2 = static_cast<CMDBGenericRecord *>(CCDRecordBase::RecordFactoryL(KCDTIdTestRecord));
	CleanupStack::PushL(record2);
	record2->LoadL(*cmdbSession);
		
	record2->SetRecordId(KCDNewRecordRequest);
	CMDBField<TUint32>* recordTag1 = static_cast<CMDBField<TUint32>*>(record2->GetFieldByIdL(KCDTIdRecordTag));
	*recordTag1 = TestTagValue1;
	record2->StoreL(*cmdbSession);
	CMDBGenericRecord* record3 = static_cast<CMDBGenericRecord *>(CCDRecordBase::RecordFactoryL(KCDTIdTestRecord));
	CleanupStack::PushL(record3);
	record3->LoadL(*cmdbSession);
	record3->SetRecordId(KCDNewRecordRequest);
	CMDBField<TUint32>* recordTag2 = static_cast<CMDBField<TUint32>*>(record3->GetFieldByIdL(KCDTIdRecordTag));
	*recordTag2 = TestTagValue;
	record3->StoreL(*cmdbSession);	
	CleanupStack::PopAndDestroy(record3);
	CleanupStack::PopAndDestroy(record2);
	
	//using generic interface
	CMDBRecordSet<CMDBGenericRecord>* GenericRecordSet1 = new(ELeave) CMDBRecordSet<CMDBGenericRecord>(KCDTIdTestRecord);
	CleanupStack::PushL(GenericRecordSet1);
	GenericRecordSet1->LoadL(*cmdbSession);
	INFO_PRINTF2(_L("Number of records: %d"), GenericRecordSet1->iRecords.Count());
	TESTL(GenericRecordSet1->iRecords.Count() == 3);
	CleanupStack::PopAndDestroy(GenericRecordSet1);

	//Using ccd based interface
	CMDBRecordSet<CCDTestRecord>* ccdRecordSet = new(ELeave) CMDBRecordSet<CCDTestRecord>(KCDTIdTestRecord);
	CleanupStack::PushL(ccdRecordSet);
	ccdRecordSet->LoadL(*cmdbSession);
	TESTL(ccdRecordSet->iRecords.Count() == 3);
	CleanupStack::PopAndDestroy(ccdRecordSet);
		

	//recordset find using generic interface
	CMDBRecordSet<CMDBGenericRecord>* GenericRecordSet2 = new(ELeave) CMDBRecordSet<CMDBGenericRecord>(KCDTIdTestRecord);
	CleanupStack::PushL(GenericRecordSet2);	
	CMDBGenericRecord* PrimingRecord = static_cast<CMDBGenericRecord*>(CCDRecordBase::RecordFactoryL(KCDTIdTestRecord));	
	CleanupStack::PushL(PrimingRecord);
	PrimingRecord->LoadL(*cmdbSession); //loads the table schema
	CMDBField<TUint32>*  PickAField = static_cast<CMDBField<TUint32>*>(PrimingRecord->GetFieldByIdL(KCDTIdRecordTag));
	*PickAField = TestTagValue1;
	GenericRecordSet2->iRecords.Append(PrimingRecord);
	CleanupStack::Pop(PrimingRecord);	
	GenericRecordSet2->FindL(*cmdbSession);
	INFO_PRINTF2(_L("Number of records: %d"), GenericRecordSet2->iRecords.Count());
	TESTL(GenericRecordSet2->iRecords.Count() == 2);
	PickAField = static_cast<CMDBField<TUint32>*>((*GenericRecordSet2)[1]->GetFieldByIdL(KCDTIdRecordTag));
	TESTL(*PickAField == TestTagValue1);
	CleanupStack::PopAndDestroy(GenericRecordSet2);
	
	//recordset find using CCD based interface
	CMDBRecordSet<CCDTestRecord>* CCDRecordSet2 = new(ELeave) CMDBRecordSet<CCDTestRecord>(KCDTIdTestRecord);
	CleanupStack::PushL(CCDRecordSet2);	
	CCDTestRecord* ccdPrimingRecord = static_cast<CCDTestRecord*>(CCDRecordBase::RecordFactoryL(KCDTIdTestRecord));	
	CleanupStack::PushL(ccdPrimingRecord);
	ccdPrimingRecord->LoadL(*cmdbSession); //loads the table schema
	
	CMDBField<TUint32>*  PickAField1 = static_cast<CMDBField<TUint32>*>(ccdPrimingRecord->GetFieldByIdL(KCDTIdRecordTag));
	*PickAField1 = TestTagValue1;
	CCDRecordSet2->iRecords.Append(ccdPrimingRecord);
	CleanupStack::Pop(ccdPrimingRecord);	
	CCDRecordSet2->FindL(*cmdbSession);
	INFO_PRINTF2(_L("Number of records: %d"), CCDRecordSet2->iRecords.Count());
	TESTL(CCDRecordSet2->iRecords.Count() == 2);
	 PickAField1 = static_cast<CMDBField<TUint32>*>((*CCDRecordSet2)[1]->GetFieldByIdL(KCDTIdRecordTag));
	TESTL(*PickAField1 == TestTagValue1);
	CleanupStack::PopAndDestroy(CCDRecordSet2);	
		
	CleanupStack::PopAndDestroy(cmdbSession);	
	SetTestStepResult(EPass);
	return TestStepResult(); 		
	}
//----------------------------------------------------------------------------------------------------------------
CCommsdat83Step::CCommsdat83Step()
/**
 * Constructor
 */
	{
	SetTestStepName(KCommsdat83Step);                                                                                           
	}

TVerdict CCommsdat83Step::doTestStepL()
/**
 * @return - TVerdict code
 *	Test for generic record recordset [Expects  table and a record to be in the db ]
 */
	{
	SetTestStepResult(EFail);	
	INFO_PRINTF1(_L("CCommsdat83Step::doTestStepL()"));

#ifdef SYMBIAN_NON_SEAMLESS_NETWORK_BEARER_MOBILITY
 	CMDBSession *cmdbSession = CMDBSession::NewL(KCDVersion1_2);
#else
 	CMDBSession *cmdbSession = CMDBSession::NewL(KCDVersion1_1);
#endif
	CleanupStack::PushL(cmdbSession);
	
	//delete the table
	CMDBRecordSet<CMDBGenericRecord>* GenericRecordSet = new(ELeave) CMDBRecordSet<CMDBGenericRecord>(KCDTIdTestRecord);
	CleanupStack::PushL(GenericRecordSet);
	GenericRecordSet->DeleteL(*cmdbSession);
	CleanupStack::PopAndDestroy(GenericRecordSet);

	//store a new table
	CMDBGenericRecord* testRecord = static_cast<CMDBGenericRecord*>(CCDRecordBase::RecordFactoryL(KCDNewTableRequest));
	CleanupStack::PushL(testRecord); 
	_LIT(KCDTypeNameTestTable1, "Test1");
    testRecord->InitializeL(KCDTypeNameTestTable1(), TestGenRecordInfo);	
	testRecord->StoreL(*cmdbSession);
 	
	//create a new record
	CCDTestRecord* ccdRecordStore = new (ELeave) CCDTestRecord(testRecord->TableId());
	CleanupStack::PushL(ccdRecordStore);
	ccdRecordStore->SetRecordId(KCDNewRecordRequest);
	ccdRecordStore->iStringField.SetL(KTestString());
	ccdRecordStore->iIntegerField = TestIntegerVal;	
	ccdRecordStore->iRecordTag = TestTagValue1;
    ccdRecordStore->iRecordName.SetL(KRecordName());
	ccdRecordStore->StoreL(*cmdbSession);
		
	//Load the above stored record
	CCDTestRecord* ccdRecord = new (ELeave) CCDTestRecord(testRecord->TableId());
	CleanupStack::PushL(ccdRecord);
	ccdRecord->SetRecordId(ccdRecordStore->RecordId());
	ccdRecord->LoadL(*cmdbSession);
	CleanupStack::PopAndDestroy(ccdRecord); 
    CleanupStack::PopAndDestroy(ccdRecordStore);	
	
	//Find, modify,refresh
	CCDTestRecord* ccdRecordFind_Modify = new (ELeave) CCDTestRecord(testRecord->TableId());
	CCDTestRecord* ccdRecordRefresh= new (ELeave) CCDTestRecord(testRecord->TableId());
	CleanupStack::PushL(ccdRecordFind_Modify);
	CleanupStack::PushL(ccdRecordRefresh);
	ccdRecordFind_Modify->iIntegerField = TestIntegerVal;	
	ccdRecordRefresh->iStringField.SetL(KTestString());
	TESTL(ccdRecordFind_Modify->FindL(*cmdbSession) &&
	ccdRecordRefresh->FindL(*cmdbSession));
		
	ccdRecordFind_Modify->iIntegerField = TestTagValue1;
	ccdRecordFind_Modify->ModifyL(*cmdbSession);	
	
	ccdRecordRefresh->RefreshL(*cmdbSession);	
	
	TESTL(ccdRecordRefresh->iIntegerField == TestTagValue1);
	CleanupStack::PopAndDestroy(ccdRecordRefresh); 
	
	//change it back to whatever it was before
	ccdRecordFind_Modify->iIntegerField = TestIntegerVal;
	ccdRecordFind_Modify->ModifyL(*cmdbSession);
	CleanupStack::PopAndDestroy(ccdRecordFind_Modify); 
	
	CleanupStack::PopAndDestroy(testRecord);
	CleanupStack::PopAndDestroy(cmdbSession);	
	
	SetTestStepResult(EPass);
	return TestStepResult(); 
	}
//----------------------------------------------------------------------------------------------------------------
CCommsdat84Step::CCommsdat84Step()
/**
 * Constructor
 */
	{
	SetTestStepName(KCommsdat84Step);                                                                                           
	}


TVerdict CCommsdat84Step::doTestStepL()
/**
 * @return - TVerdict code
 * Check values of a generic record
 */
	{
    SetTestStepResult(EFail);	

	INFO_PRINTF1(_L("CCommsdat84Step::doTestStepL()"));
	
#ifdef SYMBIAN_NON_SEAMLESS_NETWORK_BEARER_MOBILITY
 	CMDBSession *cmdbSession = CMDBSession::NewL(KCDVersion1_2);
#else
 	CMDBSession *cmdbSession = CMDBSession::NewL(KCDVersion1_1);
#endif
	CleanupStack::PushL(cmdbSession);

    // Create another example record in the repository

	CMDBGenericRecord* record = static_cast<CMDBGenericRecord*>(CCDRecordBase::RecordFactoryL(KCDTIdTestRecord));
	CleanupStack::PushL(record);	
	
	// Load the schmema data
    TRAPD(trapErr, record->LoadL(*cmdbSession));
    TESTL(trapErr == KErrNone);
		
    const TUint KTestRecordId = 4;   
    const TUint KTestTagValue = 6;   
    const TUint KTestInteger  = 5;   

    record->SetRecordId(KTestRecordId);

	CMDBField<TUint32>* recordTag = NULL;
	TRAP(trapErr, recordTag = static_cast<CMDBField<TUint32>*>(record->GetFieldByIdL(KCDTIdRecordTag)));
	TESTL(trapErr == KErrNone);
	*recordTag = KTestTagValue;

	CMDBField<TDesC>* recordName = NULL;
	TRAP(trapErr, recordName = static_cast<CMDBField<TDesC>*>(record->GetFieldByIdL(KCDTIdRecordName)));
	TESTL(trapErr == KErrNone);
	recordName->SetMaxLengthL(KRecordName().Length());
	*recordName = KRecordName;

	CMDBField<TUint32>* testInteger = NULL;
	TRAP(trapErr, testInteger = static_cast<CMDBField<TUint32>*>(record->GetFieldByIdL(KCDTIdTestIntegerField)));
	TESTL(trapErr == KErrNone);
	*testInteger = KTestInteger;
	        	
	CMDBField<TDesC>* testString = NULL;
	TRAP(trapErr, testString = static_cast<CMDBField<TDesC>*>(record->GetFieldByIdL(KCDTIdTestStringField)));
	TESTL(trapErr == KErrNone);
	testString->SetMaxLengthL(KTestString().Length());
	*testString = KTestString;
		
	TRAP(trapErr, record->StoreL(*cmdbSession));
	TESTL(trapErr == KErrNone);

    // Now create a priming container record to find on.
    
	CMDBGenericRecord* findRecord = static_cast<CMDBGenericRecord*>(CCDRecordBase::RecordFactoryL(KCDTIdTestRecord));	
	CleanupStack::PushL(findRecord);	

	findRecord->LoadL(*cmdbSession); //loads the table schema

    // Set the field to select on.	
	testString = NULL;
	TRAP(trapErr, testString = static_cast<CMDBField<TDesC>*>(findRecord->GetFieldByIdL(KCDTIdTestStringField)));
	TESTL(trapErr == KErrNone);
	testString->SetMaxLengthL(KTestString().Length());
	*testString = KTestString;

    TInt tableFound = 0;
	TRAP(trapErr, tableFound = findRecord->FindL(*cmdbSession));
	TESTL(trapErr == KErrNone && tableFound);

    // Read a couple of sample values back out.

    CMDBField<TUint>* ptrField = NULL;
    TRAP(trapErr, ptrField = static_cast<CMDBField<TUint>*>(findRecord->GetFieldByIdL(KCDTIdRecordTag)));
    TESTL(trapErr == KErrNone && ptrField);
    TUint roundTripInt = *ptrField;
    TESTL(roundTripInt == KTestTagValue);
    INFO_PRINTF2(_L("Sample integer field value is \"%d\""), roundTripInt);

    CMDBField<TDesC>* ptrStr = NULL;
    TRAP(trapErr, ptrStr = static_cast<CMDBField<TDesC>*>(findRecord->GetFieldByIdL(KCDTIdTestStringField)));
    TESTL(trapErr == KErrNone && ptrStr);

    TGenericTypeName roundTripStr;
    roundTripStr.Copy(*ptrStr);
    TESTL(roundTripStr == KTestString);
    INFO_PRINTF2(_L("Sample string field value is \"%S\""), &roundTripStr);
 
    SetTestStepResult(EPass);
			
	CleanupStack::PopAndDestroy(findRecord);
	CleanupStack::PopAndDestroy(record);
	CleanupStack::PopAndDestroy(cmdbSession);

	return TestStepResult(); 
	}
//----------------------------------------------------------------------------------------------------------------
CCommsdat85Step::CCommsdat85Step()
/**
 * Constructor
 */
	{
	SetTestStepName(KCommsdat85Step);                                                                                           
	}

TVerdict CCommsdat85Step::doTestStepL()
/**
 * @return - TVerdict code
 * Test modify and refresh on generic records
 */
	{
    SetTestStepResult(EFail);	
	INFO_PRINTF1(_L("CCommsdat85Step::doTestStepL()"));
	
#ifdef SYMBIAN_NON_SEAMLESS_NETWORK_BEARER_MOBILITY
 	CMDBSession *cmdbSession = CMDBSession::NewL(KCDVersion1_2);
#else
 	CMDBSession *cmdbSession = CMDBSession::NewL(KCDVersion1_1);
#endif
	CleanupStack::PushL(cmdbSession);

	CMDBGenericRecord* findRecord = static_cast<CMDBGenericRecord*>(CCDRecordBase::RecordFactoryL(KCDTIdTestRecord));
	CleanupStack::PushL(findRecord);	

	CMDBGenericRecord* refreshRecord = static_cast<CMDBGenericRecord*>(CCDRecordBase::RecordFactoryL(KCDTIdTestRecord));
	CleanupStack::PushL(refreshRecord);	

	// Load the schmema data
    TRAPD(trapErr, findRecord->LoadL(*cmdbSession));
    TESTL(trapErr == KErrNone);
    TRAP(trapErr, refreshRecord->LoadL(*cmdbSession));
    TESTL(trapErr == KErrNone);
	
	// Set the field to select on.	
	_LIT(KRecordName, "Test Record 1");
	CMDBField<TDesC>* recordName = NULL;
	TRAP(trapErr, recordName = static_cast<CMDBField<TDesC>*>(findRecord->GetFieldByIdL(KCDTIdRecordName)));
	TESTL(trapErr == KErrNone);
	recordName->SetMaxLengthL(KRecordName().Length());
	*recordName = KRecordName;
	
    // Find and load Record 1.
	TBool tableFound = EFalse;
	TRAP(trapErr, tableFound = findRecord->FindL(*cmdbSession));
	TESTL(trapErr == KErrNone && tableFound && findRecord->RecordId() == 1);
	
    // Construct a second container to also contain the same record (1).
    const TUint TestRecordId = 1;   
	refreshRecord->SetRecordId(TestRecordId);
	TRAP(trapErr, refreshRecord->LoadL(*cmdbSession));
	TESTL(trapErr == KErrNone);
	
    // Modify the first container.
	_LIT(KNewRecordName, "Test Record 1 - Now slightly modified");
	CMDBField<TDesC>* newRecordName = NULL;
	TRAP(trapErr, newRecordName = static_cast<CMDBField<TDesC>*>(findRecord->GetFieldByIdL(KCDTIdRecordName)));
	TESTL(trapErr == KErrNone);
	newRecordName->SetMaxLengthL(KNewRecordName().Length());
	*newRecordName = KNewRecordName;
	
    // And save the change.
    TRAP(trapErr, findRecord->ModifyL(*cmdbSession));
	TESTL(trapErr == KErrNone);
	
    // Update the 2nd container
	TRAP(trapErr, refreshRecord->RefreshL(*cmdbSession));
	TESTL(trapErr == KErrNone);

    // Read the changed field and check it's the value expected.
    CMDBField<TDesC>* ptrStr = NULL;
    TRAP(trapErr, ptrStr = static_cast<CMDBField<TDesC>*>(refreshRecord->GetFieldByIdL(KCDTIdRecordName)));
    TGenericTypeName roundTripStr;
    roundTripStr.Copy(*ptrStr);
	
    // The concluding test condition.
    TESTL(roundTripStr == KNewRecordName);
    
    INFO_PRINTF2(_L("Sample string field value is \"%S\""), &roundTripStr);
    
    SetTestStepResult(EPass);		
				
	CleanupStack::PopAndDestroy(refreshRecord);
	CleanupStack::PopAndDestroy(findRecord);
	CleanupStack::PopAndDestroy(cmdbSession);
	
	return TestStepResult(); 
	}
//----------------------------------------------------------------------------------------------------------------
CCommsdat86Step::CCommsdat86Step()
/**
 * Constructor
 */
	{
	SetTestStepName(KCommsdat86Step);                                                                                           
	}


TVerdict CCommsdat86Step::doTestStepL()
/**
 * @return - TVerdict code
 * Test generic record sets
 */
	{
    SetTestStepResult(EFail);	
	INFO_PRINTF1(_L("CCommsdat86Step::doTestStepL()"));

#ifdef SYMBIAN_NON_SEAMLESS_NETWORK_BEARER_MOBILITY
 	CMDBSession *cmdbSession = CMDBSession::NewL(KCDVersion1_2);
#else
 	CMDBSession *cmdbSession = CMDBSession::NewL(KCDVersion1_1);
#endif
	CleanupStack::PushL(cmdbSession);

    // Construct a CMDBGeneric record to hold the schema data.
    
	CMDBGenericRecord* genRecord = static_cast<CMDBGenericRecord*>(CCDRecordBase::RecordFactoryL(KCDTIdTestRecord));
	CleanupStack::PushL(genRecord);
	
    TRAPD(trapErr, genRecord->LoadL(*cmdbSession));
    TESTL(trapErr == KErrNone);

    // Create another record
    
    const TUint KTestRecordId5 = 5;   
    const TUint KTestRecordId6 = 6;
    const TUint KTestTagValue5 = 6;   
    const TUint KTestInteger5  = 7;   
	_LIT(KRecordName5, "Test Record 5");
	_LIT(KTestString5, "TestString 5!!");

    genRecord->SetRecordId(KTestRecordId5);

	CMDBField<TUint32>* recordTag = NULL;
	TRAP(trapErr, recordTag = static_cast<CMDBField<TUint32>*>(genRecord->GetFieldByIdL(KCDTIdRecordTag)));
	TESTL(trapErr == KErrNone);
	*recordTag = KTestTagValue5;

	CMDBField<TDesC>* recordName = NULL;
	TRAP(trapErr, recordName = static_cast<CMDBField<TDesC>*>(genRecord->GetFieldByIdL(KCDTIdRecordName)));
	TESTL(trapErr == KErrNone);
	recordName->SetMaxLengthL(KRecordName5().Length());
	*recordName = KRecordName5;

	CMDBField<TUint32>* testInteger = NULL;
	TRAP(trapErr, testInteger = static_cast<CMDBField<TUint32>*>(genRecord->GetFieldByIdL(KCDTIdTestIntegerField)));
	TESTL(trapErr == KErrNone);
	*testInteger = KTestInteger5;
	        	
	CMDBField<TDesC>* testString = NULL;
	TRAP(trapErr, testString = static_cast<CMDBField<TDesC>*>(genRecord->GetFieldByIdL(KCDTIdTestStringField)));
	TESTL(trapErr == KErrNone);
	testString->SetMaxLengthL(KTestString5().Length());
	*testString = KTestString5;

    TRAP(trapErr, genRecord->StoreL(*cmdbSession));
	TESTL(trapErr == KErrNone);

    // And create yet another record
  
    const TUint KTestTagValue6 = 7;   
    const TUint KTestInteger6  = 8;   
	_LIT(KRecordName6, "Test Record 6");
	_LIT(KTestString6, "TestString 6!!");

    genRecord->SetRecordId(KCDNewRecordRequest);

	*recordTag = KTestTagValue6;
	recordName->SetMaxLengthL(KRecordName6().Length());
	*recordName = KRecordName6;
	*testInteger = KTestInteger6;
	testString->SetMaxLengthL(KTestString6().Length());
	*testString = KTestString6;

    TRAP(trapErr, genRecord->StoreL(*cmdbSession));
	TESTL(trapErr == KErrNone);
	
	// Now we've got four records of our test generic type in total, let's delete one

	CleanupStack::PopAndDestroy(genRecord);
	
    genRecord = static_cast<CMDBGenericRecord*>(CCDRecordBase::RecordFactoryL(KCDTIdTestRecord));
	CleanupStack::PushL(genRecord);
	
	// load schema
    TRAP(trapErr, genRecord->LoadL(*cmdbSession));
	TESTL(trapErr == KErrNone);

	genRecord->SetRecordId(KTestRecordId6);
	
	
	//Load record
	genRecord->LoadL(*cmdbSession);

	TRAP(trapErr, recordTag = static_cast<CMDBField<TUint32>*>(genRecord->GetFieldByIdL(KCDTIdRecordTag)));
	TESTL(trapErr == KErrNone);
	
	TRAP(trapErr, recordName = static_cast<CMDBField<TDesC>*>(genRecord->GetFieldByIdL(KCDTIdRecordName)));
	TESTL(trapErr == KErrNone);
	
	TRAP(trapErr, testInteger = static_cast<CMDBField<TUint32>*>(genRecord->GetFieldByIdL(KCDTIdTestIntegerField)));
	TESTL(trapErr == KErrNone);
	        	
	TRAP(trapErr, testString = static_cast<CMDBField<TDesC>*>(genRecord->GetFieldByIdL(KCDTIdTestStringField)));
	TESTL(trapErr == KErrNone);
		
    // Read a field
    TGenericTypeName roundTripStr;
    roundTripStr.Copy(*recordName);

    TESTL(roundTripStr == KRecordName6);
    INFO_PRINTF2(_L("The name of record 6 is \"%S\""), &roundTripStr);

    TRAP(trapErr, genRecord->DeleteL(*cmdbSession));
	TESTL(trapErr == KErrNone);

	_LIT(KNull, "");
	
    // Read a field, calling RefreshL() on the recordName already in the container.
	recordName->SetMaxLengthL(KNull().Length());
    *recordName = KNull;

    TRAP(trapErr, recordName->LoadL(*cmdbSession));
    roundTripStr.Copy(*recordName);
	TESTL(trapErr == KErrNotFound && roundTripStr == KNull);
    INFO_PRINTF2(_L("The name of the record 6 is \"%S\" (it should have disappeared)"), &roundTripStr);
	
	SetTestStepResult(EPass);		
			
	CleanupStack::PopAndDestroy(genRecord);
	CleanupStack::PopAndDestroy(cmdbSession);
	
	return TestStepResult(); 
	}

//----------------------------------------------------------------------------------------------------------------
_LIT(KLongestPossibleTableName, "A table name which is 64 characters; that is the limit defined");

CCommsdat87Step::CCommsdat87Step()
/**
 * Constructor
 */
	{
	SetTestStepName(KCommsdat87Step);                                                                                           
	}

TVerdict CCommsdat87Step::doTestStepL()
/**
 * @return - TVerdict code
 * Testing medium texts as table names
 */
	{
    SetTestStepResult(EFail);	
	INFO_PRINTF1(_L("Testing long texts as table names in CCommsdat87Step::doTestStepL() "));
	
	SetTestStepResult(EFail);	
	INFO_PRINTF1(_L("CCommsdat87Step::doTestStepL()"));
	
	// Create a new table
#ifdef SYMBIAN_NON_SEAMLESS_NETWORK_BEARER_MOBILITY
 	CMDBSession *cmdbSession = CMDBSession::NewL(KCDVersion1_2);
#else
 	CMDBSession *cmdbSession = CMDBSession::NewL(KCDVersion1_1);
#endif
	CleanupStack::PushL(cmdbSession); 

	CMDBGenericRecord* testRecord = static_cast<CMDBGenericRecord*>(CCDRecordBase::RecordFactoryL(KCDNewTableRequest));
	CleanupStack::PushL(testRecord); 
    testRecord->InitializeL(KLongestPossibleTableName(), TestGenRecordInfo);	
         
	TRAPD(storeErr, testRecord->StoreL(*cmdbSession));
	
	CleanupStack::PopAndDestroy(testRecord);
	
	testRecord = static_cast<CMDBGenericRecord *>(CCDRecordBase::RecordFactoryL(KCDTIdTestRecord/*todo..hardcoded testRecord->TypeId() & KCDMaskShowRecordType*/));
	CleanupStack::PushL(testRecord);
	
	TRAPD(loadErr,testRecord->LoadL(*cmdbSession));
	
	if(storeErr == KErrNone && loadErr == KErrNone)
		{
		SetTestStepResult(EPass);	
		}
	
	CleanupStack::PopAndDestroy(testRecord);
    CleanupStack::PopAndDestroy(cmdbSession);
	return TestStepResult();
	}
	
//----------------------------------------------------------------------------------------------------------------
CCommsdat88Step::CCommsdat88Step()
/**
 * Constructor
 */
	{
	SetTestStepName(KCommsdat88Step);                                                                                           
	}

TVerdict CCommsdat88Step::doTestStepL()
/**
 * @return - TVerdict code
 * Testing testing medium texts as field names via GetFiledByNameL
 */
	{
    SetTestStepResult(EFail);	
	INFO_PRINTF1(_L("Testing testing long texts as field names in CCommsdat88Step::doTestStepL() "));
	
#ifdef SYMBIAN_NON_SEAMLESS_NETWORK_BEARER_MOBILITY
 	CMDBSession *cmdbSession = CMDBSession::NewL(KCDVersion1_2);
#else
 	CMDBSession *cmdbSession = CMDBSession::NewL(KCDVersion1_1);
#endif
	CleanupStack::PushL(cmdbSession); 
		
	CMDBGenericRecord* testRecord = static_cast<CMDBGenericRecord*>(CCDRecordBase::RecordFactoryL(KCDTIdTestRecord));
	CleanupStack::PushL(testRecord); 
	testRecord->LoadL(*cmdbSession); //Load the schema
	
	testRecord->SetRecordId(TestRecordId);//Test step fails if you remove this...
	
	TRAPD(loadErr, testRecord->LoadL(*cmdbSession));
	INFO_PRINTF2(_L("loadErr : %d"), loadErr);
	
	_LIT(KTestMediumString, "MediumString");
	
	TInt ignore = 0;
	CMDBField<TDesC>* testMediumString = static_cast<CMDBField<TDesC>*>(testRecord->GetFieldByNameL(KCDTypeNameMediumStringField, ignore));
	CMDBField<TInt>* testInteger = static_cast<CMDBField<TInt>*>(testRecord->GetFieldByNameL(KCDTypeNameIntegerField, ignore));
	TPtrC result(*testMediumString); 
	
	INFO_PRINTF2(_L("text : %S"), &result);
	
	if(loadErr == KErrNone && result == KTestMediumString)
		{
		SetTestStepResult(EPass);	
		}
	
	CleanupStack::PopAndDestroy(testRecord);
    CleanupStack::PopAndDestroy(cmdbSession);
	return TestStepResult();
	}
	
//----------------------------------------------------------------------------------------------------------------
CCommsdat89Step::CCommsdat89Step()
/**
 * Constructor
 */
	{
	SetTestStepName(KCommsdat89Step);                                                                                           
	}

TVerdict CCommsdat89Step::doTestStepL()
/**
 * @return - TVerdict code
 * Testing testing medium texts as field names via GetFiledByNameL
 */
	{
    SetTestStepResult(EFail);
    
	INFO_PRINTF1(_L("Testing testing long texts as field names in CCommsdat89Step::doTestStepL() "));
	
#ifdef SYMBIAN_NON_SEAMLESS_NETWORK_BEARER_MOBILITY
 	CMDBSession *cmdbSession = CMDBSession::NewL(KCDVersion1_2);
#else
 	CMDBSession *cmdbSession = CMDBSession::NewL(KCDVersion1_1);
#endif
	CleanupStack::PushL(cmdbSession);
    
	//to test you wouldnt delete the schema stored above
	CMDBRecordSet<CMDBGenericRecord>* GenericRecordSet1 = new(ELeave) CMDBRecordSet<CMDBGenericRecord>(KCDTIdTestRecord);
	CleanupStack::PushL(GenericRecordSet1);
	GenericRecordSet1->LoadL(*cmdbSession);

    DoTableProtectionL(GenericRecordSet1, cmdbSession, ETrue);

	INFO_PRINTF1(_L("Table Protected!"));

    TInt err(KErrNotFound);
    
    if(GenericRecordSet1->iRecords.Count())
    	{
	    TRAP(err, GenericRecordSet1->iRecords[0]->DeleteL(*cmdbSession);)
        }
    
    if(err != KErrPermissionDenied) 
        {
        return TestStepResult();
        }

   	INFO_PRINTF1(_L("Delete Protected Record Failed!"));

	//First load the schema
	CMDBGenericRecord* testRecord = static_cast<CMDBGenericRecord *>(CCDRecordBase::RecordFactoryL(KCDTIdTestRecord));
	CleanupStack::PushL(testRecord);
	
	testRecord->LoadL(*cmdbSession);

	//Store a record
    testRecord->SetRecordId(KCDNewRecordRequest);

	CMDBField<TUint32>* recordTag = static_cast<CMDBField<TUint32>*>(testRecord->GetFieldByIdL(KCDTIdRecordTag));
	*recordTag = TestTagValue1;
	_LIT(KRecordName, "Test Record 5");
	CMDBField<TDesC>* recordName = static_cast<CMDBField<TDesC>*>(testRecord->GetFieldByIdL(KCDTIdRecordName));
	recordName->SetMaxLengthL(KRecordName().Length());
	*recordName = KRecordName;
	CMDBField<TUint32>* testInteger = static_cast<CMDBField<TUint32>*>(testRecord->GetFieldByIdL(KCDTIdTestIntegerField));
	*testInteger = TestIntegerVal;	       	
	_LIT(KTestString, "TestString RecordSet!!");
	CMDBField<TDesC>* testString = static_cast<CMDBField<TDesC>*>(testRecord->GetFieldByIdL(KCDTIdTestStringField));
	testString->SetMaxLengthL(KTestString().Length());
	*testString = KTestString;
	
    TRAP(err, testRecord->StoreL(*cmdbSession);)
	
    if(err != KErrPermissionDenied) 
        {
        return TestStepResult();
        }

   	INFO_PRINTF1(_L("Insertion of Record in Protected Table Failed!"));

    cmdbSession->SetAttributeMask(ECDProtectedWrite);

	TRAP(err, testRecord->StoreL(*cmdbSession);)

    if(err == KErrNone)
        {
        SetTestStepResult(EPass);
       	INFO_PRINTF1(_L("Insertion of Record in Protected Table with proper Attributes Succeded!"));
        }
    else
        {
       	INFO_PRINTF2(_L("ERR: Insertion of Record in Protected Table with proper Attributes Failed with Err=%d!"), err);
        }

	CleanupStack::PopAndDestroy(testRecord);
	CleanupStack::PopAndDestroy(GenericRecordSet1);
    CleanupStack::PopAndDestroy(cmdbSession);

	return TestStepResult();
	}
	
//----------------------------------------------------------------------------------------------------------------

CCommsdat90Step::CCommsdat90Step()
/**
 * Constructor
 */
	{
	SetTestStepName(KCommsdat90Step);
	}

TVerdict CCommsdat90Step::doTestStepL()
/**
 * @return - TVerdict code
 *	Test for generic record recordset [Expects  table and a record to be in the db ]
 */
	{
	SetTestStepResult(EFail);	
	INFO_PRINTF1(_L("CCommsdat90Step::doTestStepL()"));

#ifdef SYMBIAN_NON_SEAMLESS_NETWORK_BEARER_MOBILITY
 	CMDBSession *cmdbSession = CMDBSession::NewL(KCDVersion1_2);
#else
 	CMDBSession *cmdbSession = CMDBSession::NewL(KCDVersion1_1);
#endif
	CleanupStack::PushL(cmdbSession);
	
	TInt err(KErrNotFound);	

	// getting generic table by name. Tbale is not protected
	CMDBGenericRecord* tempUserDefinedRecord = static_cast<CMDBGenericRecord*>(CCDRecordBase::RecordFactoryL(0));
	CleanupStack::PushL(tempUserDefinedRecord);
	tempUserDefinedRecord->InitializeL(KCDTypeNameTestTable(), NULL);
	tempUserDefinedRecord->LoadL(*cmdbSession);

	TMDBElementId recordSetId = 0;
	CMDBRecordSetBase* recordSetContainer = NULL;

	// Get the Id that we're interested in...
	recordSetId = tempUserDefinedRecord->TableId();

	recordSetContainer = new (ELeave) CMDBRecordSet<CMDBGenericRecord>(recordSetId);
	CleanupStack::PushL(recordSetContainer);

	// do table protection
	DoTableProtectionL(recordSetContainer, cmdbSession, ETrue);

	INFO_PRINTF1(_L("Table Protected!"));

	CleanupStack::PopAndDestroy(recordSetContainer);
	CleanupStack::PopAndDestroy(tempUserDefinedRecord);

	// trying to get table again
	tempUserDefinedRecord = static_cast<CMDBGenericRecord*>(CCDRecordBase::RecordFactoryL(0));
	CleanupStack::PushL(tempUserDefinedRecord);
	tempUserDefinedRecord->InitializeL(KCDTypeNameTestTable(), NULL);
	TRAP(err,tempUserDefinedRecord->LoadL(*cmdbSession));

    if(err == KErrNone)
        {
        SetTestStepResult(EPass);
       	INFO_PRINTF1(_L("Reading of Protected Generic Table Succeded!"));
        }
    else
        {
       	INFO_PRINTF2(_L("ERR: Reading of Protected Generic Table Failed with Err=%d!"), err);
        }

    CleanupStack::PopAndDestroy(tempUserDefinedRecord);
	CleanupStack::PopAndDestroy(cmdbSession);

	return TestStepResult();
	}
	
//----------------------------------------------------------------------------------------------------------------
/* This test step tests a generic record which contains 2 link fields. The 1st link field
 * links to a generic IAP table and the 2nd one links to another generic record.
 * 
 * The structure of the tables:
 * 
 * 
 * 		 |------------>	IAP record
 *       |
 * LinkTestRecord
 *       |
 * 		 |----------->	LinkedTestRecord
 *
 */ 

_LIT(KCDTypeNameLinkTestTable, "TestLinkTable");
class CCDLinkTestRecord : public CCDRecordBase
{
public :

// Constructor
CCDLinkTestRecord(TMDBElementId aElementId)
    : CCDRecordBase(aElementId), 
      iIAP((aElementId & KCDMaskShowRecordType) | KCDTIdTestIAPLinkField),
      iOtherGenericRecord((aElementId & KCDMaskShowRecordType) | KCDTIdTestOtherGenRecLinkField),
      iRecordTag((aElementId & KCDMaskShowRecordType) | KCDTIdRecordTag),
      iRecordName((aElementId & KCDMaskShowRecordType) | KCDTIdRecordName)
    {}

const SRecordTypeInfo* GetRecordInfo(){return iRecordInfo;}

DATA_VTABLE

public :

CMDBField<TInt>	iIAP;
CMDBField<TInt>	iOtherGenericRecord;

CMDBField<TInt>	 iRecordTag;  
CMDBField<TDesC> iRecordName;   

private:

	static const SRecordTypeInfo iRecordInfo[];	
};


const SRecordTypeInfo CCDLinkTestRecord::iRecordInfo[]=
	{
	{ KCDTIdRecordTag,	        		EInt,		ENoAttrs,   KCDTypeNameRecordTag },
	{ KCDTIdRecordName,     			EText,		ENotNull,   KCDTypeNameRecordName },
	{ KCDTIdTestIAPLinkField,			ELink,		ENoAttrs,   KCDTypeNameTestIAPLinkField },
	{ KCDTIdTestOtherGenRecLinkField,	ELink,  	ENoAttrs,   KCDTypeNameTestOtherGenRecField },
	{ 0, 0, ENoAttrs, KCDNull }
	};	


START_ATTRIBUTE_TABLE(CCDLinkTestRecord, KCDTIdLinkTestRecord,  KCDTIdLinkTestRecord )
X_REGISTER_ATTRIBUTE(CCDLinkTestRecord,   iIAP,					TMDBLinkNum)
X_REGISTER_ATTRIBUTE(CCDLinkTestRecord,   iOtherGenericRecord,  TMDBLinkNum)
X_REGISTER_ATTRIBUTE(CCDLinkTestRecord,   iRecordTag,    		TMDBNum )
X_REGISTER_ATTRIBUTE(CCDLinkTestRecord,   iRecordName,   		TMDBText )
END_ATTRIBUTE_TABLE()

//
_LIT(KCDTypeNameLinkedTestTable, "TestLinkedTable");
class CCDLinkedTestRecord : public CCDRecordBase
{
public :

// Constructor
CCDLinkedTestRecord(TMDBElementId aElementId)
    : CCDRecordBase(aElementId), 
      iSomeField((aElementId & KCDMaskShowRecordType) | KCDTIdTestSomeField),
      iRecordTag((aElementId & KCDMaskShowRecordType) | KCDTIdRecordTag),
      iRecordName((aElementId & KCDMaskShowRecordType) | KCDTIdRecordName)
    {}

const SRecordTypeInfo* GetRecordInfo(){return iRecordInfo;}

DATA_VTABLE

public :
CMDBField<TInt>	iSomeField;

CMDBField<TInt>	 iRecordTag;  
CMDBField<TDesC> iRecordName;   

private:

	static const SRecordTypeInfo iRecordInfo[];	
};


const SRecordTypeInfo CCDLinkedTestRecord::iRecordInfo[]=
	{
	{ KCDTIdRecordTag,	    EInt,		ENoAttrs,   KCDTypeNameRecordTag },
	{ KCDTIdRecordName,    	EText,		ENotNull,   KCDTypeNameRecordName },
	{ KCDTIdTestSomeField,	EInt,		ENoAttrs,   KCDTypeNameTestSomeField },
	{ 0, 0, ENoAttrs, KCDNull }
	};	


START_ATTRIBUTE_TABLE(CCDLinkedTestRecord, KCDTIdLinkedTestRecord,  KCDTIdLinkedTestRecord )
X_REGISTER_ATTRIBUTE(CCDLinkedTestRecord,   iSomeField,			TMDBNum)
X_REGISTER_ATTRIBUTE(CCDLinkedTestRecord,   iRecordTag,    		TMDBNum )
X_REGISTER_ATTRIBUTE(CCDLinkedTestRecord,   iRecordName,   		TMDBText )
END_ATTRIBUTE_TABLE()

CCommsdat91Step::CCommsdat91Step()
/**
 * Constructor
 */
	{
	SetTestStepName(KCommsdat91Step);
	}

CCommsdat91Step::~CCommsdat91Step()
	{
	delete iDBSession;
	}

TVerdict CCommsdat91Step::doTestStepL()
/**
 * @return - TVerdict code
 *	Test for generic record recordset [Expects  table and a record to be in the db ]
 */
	{
	SetTestStepResult(EFail);	
	INFO_PRINTF1(_L("CCommsdat91Step::doTestStepL()"));

#ifdef SYMBIAN_NON_SEAMLESS_NETWORK_BEARER_MOBILITY
	iDBSession = CMDBSession::NewL(KCDVersion1_2);
#else
	iDBSession = CMDBSession::NewL(KCDVersion1_1);
#endif
	
	INFO_PRINTF1(_L("Creating the IAP record"));
	IAPCreateL();
	
	INFO_PRINTF1(_L("Creating the linked generic record"));
	GenericLinkedCreateL();
	
	INFO_PRINTF1(_L("Creating the link generic record"));
	GenericLinkCreateL();
	
	CMDBGenericRecord* record4 = static_cast<CMDBGenericRecord *>(CCDRecordBase::RecordFactoryL(KCDTIdLinkTestRecord));
	CleanupStack::PushL(record4);
	record4->SetRecordId(1);
	record4->LoadL(*iDBSession); 
	CMDBField<TUint32>* IAPField = static_cast<CMDBField<TUint32>*>(record4->GetFieldByIdL(KCDTIdTestIAPLinkField));
	CMDBField<TUint32>* genRecField = static_cast<CMDBField<TUint32>*>(record4->GetFieldByIdL(KCDTIdTestOtherGenRecLinkField));
	
	if (*IAPField == 2 &&
		*genRecField == 1610612992)
		{
		INFO_PRINTF1(_L("OK, the linked field values are as expected!"));
		SetTestStepResult(EPass);
		}
	
	CleanupStack::PopAndDestroy(record4);
	
	return TestStepResult();
	}

void CCommsdat91Step::IAPCreateL()
	{
	CCDIAPRecord* newIapRecord = static_cast<CCDIAPRecord*>(CCDRecordBase::RecordFactoryL(KCDTIdIAPRecord));
	CleanupStack::PushL(newIapRecord);
	
	_LIT(KIAPName1, "TestIAP1");
	_LIT(KModemBearerType, "ModemBearer");
	_LIT(KLANServiceType, "LANService");
	
  	// create a new request
	newIapRecord->SetRecordId(KCDNewRecordRequest);
    newIapRecord->iRecordName.SetMaxLengthL(KIAPName1().Length());
    newIapRecord->iRecordName = KIAPName1;
  	newIapRecord->iNetworkWeighting = 14;    // dummy value
  	newIapRecord->iBearer = 1;
  	newIapRecord->iBearerType.SetMaxLengthL(KModemBearerType().Length());
  	newIapRecord->iBearerType = KModemBearerType;
  	newIapRecord->iService = 1;
  	newIapRecord->iServiceType.SetMaxLengthL(KLANServiceType().Length());
  	newIapRecord->iServiceType = KLANServiceType;    
	newIapRecord->StoreL(*iDBSession);
	
	iIAPRecId = newIapRecord->RecordId();
	
	CleanupStack::PopAndDestroy(newIapRecord);
	}

void CCommsdat91Step::GenericLinkedCreateL()
	{
	CMDBGenericRecord* testRecord = static_cast<CMDBGenericRecord*>(CCDRecordBase::RecordFactoryL(KCDNewTableRequest));
	CleanupStack::PushL(testRecord); 
    testRecord->InitializeL(KCDTypeNameLinkedTestTable(), TestGenLinkedkRecordInfo);	
         
	testRecord->StoreL(*iDBSession);
	TMDBElementId id = testRecord->TableId();
	
	// Check the value of the TableId that has been assigned
	TESTL(id == KCDTIdLinkedTestRecord);
	CleanupStack::PopAndDestroy(testRecord);
	
	//Firs load the schema
	testRecord = static_cast<CMDBGenericRecord *>(CCDRecordBase::RecordFactoryL(KCDTIdLinkedTestRecord));
	CleanupStack::PushL(testRecord);
	
	testRecord->LoadL(*iDBSession);
			
	INFO_PRINTF1(_L("Storing a new Generic Linked record "));	
 
	//Store a record
    testRecord->SetRecordId(KCDNewRecordRequest);
   
	CMDBField<TUint32>* recordTag = static_cast<CMDBField<TUint32>*>(testRecord->GetFieldByIdL(KCDTIdRecordTag));
	*recordTag = TestTagValue1;
	
	_LIT(KRecordName, "Test Linked Record 1");
	CMDBField<TDesC>* recordName = static_cast<CMDBField<TDesC>*>(testRecord->GetFieldByIdL(KCDTIdRecordName));
	recordName->SetMaxLengthL(KRecordName().Length());
	*recordName = KRecordName;
	
	CMDBField<TUint32>* testInteger = static_cast<CMDBField<TUint32>*>(testRecord->GetFieldByIdL(KCDTIdTestSomeField));
	*testInteger = TestIntegerVal;
	
	testRecord->StoreL(*iDBSession);
	
	iLinkedGenericId = testRecord->ElementId();
	
	CleanupStack::PopAndDestroy(testRecord);
	}

void CCommsdat91Step::GenericLinkCreateL()
	{
	CMDBGenericRecord* testRecord = static_cast<CMDBGenericRecord*>(CCDRecordBase::RecordFactoryL(KCDNewTableRequest));
	CleanupStack::PushL(testRecord); 
    testRecord->InitializeL(KCDTypeNameLinkTestTable(), TestGenLinkRecordInfo);	
         
	testRecord->StoreL(*iDBSession);
	TMDBElementId id = testRecord->TableId();
	
	// Check the value of the TableId that has been assigned
	TESTL(id == KCDTIdLinkTestRecord);
	CleanupStack::PopAndDestroy(testRecord);
	
	//Firs load the schema
	testRecord = static_cast<CMDBGenericRecord *>(CCDRecordBase::RecordFactoryL(KCDTIdLinkTestRecord));
	CleanupStack::PushL(testRecord);
	
	testRecord->LoadL(*iDBSession);
			
	INFO_PRINTF1(_L("Storing a new Generic Link record "));	
 
	//Store a record
    testRecord->SetRecordId(KCDNewRecordRequest);

	CMDBField<TUint32>* recordTag = static_cast<CMDBField<TUint32>*>(testRecord->GetFieldByIdL(KCDTIdRecordTag));
	*recordTag = TestTagValue1;
	
	_LIT(KRecordName, "Test Linked Record 1");
	CMDBField<TDesC>* recordName = static_cast<CMDBField<TDesC>*>(testRecord->GetFieldByIdL(KCDTIdRecordName));
	recordName->SetMaxLengthL(KRecordName().Length());
	*recordName = KRecordName;
	
	CMDBField<TUint32>* testIAPLink = static_cast<CMDBField<TUint32>*>(testRecord->GetFieldByIdL(KCDTIdTestIAPLinkField));
	//This link contains only the record ID of the linked record...
	*testIAPLink = iIAPRecId;
	
	CMDBField<TUint32>* testGenRecLink = static_cast<CMDBField<TUint32>*>(testRecord->GetFieldByIdL(KCDTIdTestOtherGenRecLinkField));
	//This link contains the full elementID of the linked generic record...
	TMDBElementId fullLink = (iLinkedGenericId & KCDMaskShowRecordType) | (iLinkedGenericId & KCDMaskShowRecordId);
	*testGenRecLink = fullLink;
		
	testRecord->StoreL(*iDBSession);
	
	iLinkedGenericId = testRecord->ElementId();
	
	CleanupStack::PopAndDestroy(testRecord);
	}

CCommsdat92Step::CCommsdat92Step()
/**
 * Constructor
 */
	{
	SetTestStepName(KCommsdat92Step);
	}

CCommsdat92Step::~CCommsdat92Step()
	{
	delete iDBSession;
	}

TVerdict CCommsdat92Step::doTestStepL()
/**
 * @return - TVerdict code
 *	Test that placeholder isn't being returned when you try to load a field
 */
	{
	SetTestStepResult(EFail);	
	INFO_PRINTF1(_L("CCommsdat92Step::doTestStepL()"));

#ifdef SYMBIAN_NON_SEAMLESS_NETWORK_BEARER_MOBILITY
	iDBSession = CMDBSession::NewL(KCDVersion1_2);
#else
	iDBSession = CMDBSession::NewL(KCDVersion1_1);
#endif

	CMDBGenericRecord* record4 = static_cast<CMDBGenericRecord *>(CCDRecordBase::RecordFactoryL(0));
	CleanupStack::PushL(record4);
    record4->InitializeL(KCDTypeNameTestTable(), NULL);
	record4->LoadL(*iDBSession);

	CMDBField<TUint32>* testInteger = static_cast<CMDBField<TUint32>*>(record4->GetFieldByIdL(KCDTIdTestIntegerField));
	testInteger->SetL(1);

	TInt err = record4->FindL(*iDBSession);	
	
	CleanupStack::PopAndDestroy(record4);

	SetTestStepResult(EPass);	
	return TestStepResult();
	}
