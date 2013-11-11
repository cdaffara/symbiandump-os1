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
// This contains CommDb Unit Test Cases 0064.XX

// EPOC includes
#include <e32base.h>
#include <commdb.h>


// Test system includes
//#include <networking/log.h>
//#include <networking/teststep.h>
#include "Teststepcommdb.h"
#include "Step_064_xx.h"

#include <commsdattypeinfov1_1.h>
#include <commsdattypesv1_1.h>
#include <commsdat_partner.h>


// table name
_LIT(KCDTypeNameTestTable, "Test");

#define	KCDTypeNameBinaryFieldTest		      _S("BinaryTest")

// record type id
const TMDBElementId KCDTIdTestRecord         = 0x5F800000;
// field type ids
const TMDBElementId KCDTIdTestBinaryField    = 0x00150000;


static const SGenericRecordTypeInfo TestGenRecordInfoBinary[] =
    {
	MDB_GENERIC_TYPE_INFO(KCDTIdRecordTag,	     	EUint32,    ENoAttrs,   KCDTypeNameRecordTag),
    MDB_GENERIC_TYPE_INFO(KCDTIdRecordName,	     	EText,	    ENotNull,   KCDTypeNameRecordName),
	MDB_GENERIC_TYPE_INFO(KCDTIdTestBinaryField,	EDesC8,  	ENoAttrs,   KCDTypeNameBinaryFieldTest),
    MDB_GENERIC_TYPE_INFO(0, 0, ENoAttrs, KCDNull) 
    };



NONSHARABLE_CLASS(CCDTestBinaryRecord) : public CCDRecordBase
	{
public :

	// Constructor
    CCDTestBinaryRecord(TMDBElementId aElementId)
        : CCDRecordBase(aElementId), 
          iBinaryField((aElementId & KCDMaskShowRecordType) | KCDTIdTestBinaryField),
          iRecordTag((aElementId & KCDMaskShowRecordType) | KCDTIdRecordTag),
          iRecordName((aElementId & KCDMaskShowRecordType) | KCDTIdRecordName)
        {}
        
    const SRecordTypeInfo* GetRecordInfo(){return iRecordInfo;}    

	DATA_VTABLE

public :

	CMDBField<TDesC8>	iBinaryField;
	CMDBField<TInt>	 	iRecordTag;  
	CMDBField<TDesC> 	iRecordName;   
	
private:
    
		static const SRecordTypeInfo iRecordInfo[];	
	};
	

	
	
const SRecordTypeInfo CCDTestBinaryRecord::iRecordInfo[]=
  	{
		{ KCDTIdRecordTag,	        EInt,		ENoAttrs,   KCDTypeNameRecordTag },
  		{ KCDTIdRecordName,     	EText,		ENotNull,   KCDTypeNameRecordName },
		{ KCDTIdTestBinaryField,	EDesC8,		ENotNull,   KCDTypeNameBinaryFieldTest },
  		{ 0, 0, ENoAttrs, KCDNull }
  	};	


START_ATTRIBUTE_TABLE(CCDTestBinaryRecord, KCDTIdTestRecord,  KCDTIdTestRecord )
	X_REGISTER_ATTRIBUTE(CCDTestBinaryRecord,   iBinaryField, TMDBBin)
	X_REGISTER_ATTRIBUTE(CCDTestBinaryRecord,   iRecordTag,    TMDBNum )
	X_REGISTER_ATTRIBUTE(CCDTestBinaryRecord,   iRecordName,   TMDBText )
END_ATTRIBUTE_TABLE()


_LIT8(KTestBinary, "a long binary string,a long binary string,a long binary string,a long binary string,a long binary string,a long binary string,a long binary string,end");





/////////////////////
//	Test step 064.001
/////////////////////

// constructor
CCommDbTest064_01::CCommDbTest064_01()
	{
	// store the name of this test case
	SetTestStepName(_L("step_064_01"));
	}

// destructor
CCommDbTest064_01::~CCommDbTest064_01()
	{
	}


TInt CCommDbTest064_01::executeStepL()
	{
	TInt ret=KErrGeneral;
	
	//set up a generic record with a binary field	
	INFO_PRINTF1(_L("Creating  a user defined table"));

	CMDBSession *cmdbSession = CMDBSession::NewL(KCDVersion1_1);

	CleanupStack::PushL(cmdbSession); 

	CMDBGenericRecord* testRecord = static_cast<CMDBGenericRecord*>(CCDRecordBase::RecordFactoryL(KCDNewTableRequest));
	CleanupStack::PushL(testRecord); 
    testRecord->InitializeL(KCDTypeNameTestTable(), TestGenRecordInfoBinary);	
         
	testRecord->StoreL(*cmdbSession);
	CleanupStack::PopAndDestroy(testRecord);	
	CleanupStack::PopAndDestroy(cmdbSession);
	
	
	//now use commdbshim api to read and write to the binary field
	CCommsDatabase*		theDb;
	CCommsDbTableView*	theView;
	TUint32 recordId;

	//CDBLEAVE( iTheView->PutRecordChanges(), KErrNone );
	theDb = CCommsDatabase::NewL();
	theView = theDb->OpenTableLC( KCDTypeNameTestTable );
	CleanupStack::Pop();
	
	TBuf8<KMaxLongTextLength> value(KTestBinary);
	CDBLEAVE(theView->InsertRecord(recordId), KErrNone );
	theView->WriteTextL(TPtrC(KCDTypeNameBinaryFieldTest), value );
	CDBLEAVE(theView->PutRecordChanges(), KErrNone );
	CDBLEAVE(theView->GotoFirstRecord(), KErrNone );

	TBuf8<KMaxLongTextLength> value2;
	theView->ReadTextL( TPtrC(KCDTypeNameBinaryFieldTest), value2 );

 	if( value2.Compare( value ) == 0)
		{
		ret = KErrNone;
		}

	delete theView;
	delete theDb;

	return ret;
	}


TVerdict CCommDbTest064_01::doTestStepL( )
	{
	SetTestStepResult(EFail);
	
	INFO_PRINTF1(_L("Step 064.01 called "));
	if ( executeStepL() == KErrNone )
		{
		SetTestStepResult(EPass);
		}
		
	return TestStepResult();
	}


