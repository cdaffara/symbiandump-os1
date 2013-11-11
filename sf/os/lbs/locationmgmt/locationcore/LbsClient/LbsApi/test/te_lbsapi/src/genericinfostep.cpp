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
// Example CTestStep derived implementation
// 
//

/**
 @file GenericInfoStep.cpp
 @internalTechnology
*/
#include "genericinfostep.h"
#include "te_lbsapisuitedefs.h"

#include "lcfsbucommondefinitions.h"

#include <lbs.h>
#include <lbssatellite.h>

CGenericInfoStep::~CGenericInfoStep()
/**
 * Destructor
 */
	{
	}

CGenericInfoStep::CGenericInfoStep()
/**
 * Constructor
 */
	{
	// **MUST** call SetTestStepName in the constructor as the controlling
	// framework uses the test step name immediately following construction to set
	// up the step's unique logging ID.
	SetTestStepName(KGenericInfoStep);
	}

TVerdict CGenericInfoStep::doTestStepPreambleL()
/**
 * @return - TVerdict code
 * Override of base class virtual
 */
	{
	 CTe_LbsApiSuiteStepBase::doTestStepPreambleL();
	 if (TestStepResult()!=EPass)
	    return   TestStepResult();
	// process some pre setting to this test step then set SetTestStepResult to EFail or Epass.
	SetTestStepResult(EPass);
	return TestStepResult();
	}


TVerdict CGenericInfoStep::doTestStepL()
/**
 * @return - TVerdict code
 * Override of base class pure virtual
 * Our implementation only gets called if the base class doTestStepPreambleL() did
 * not leave. That being the case, the current test result value will be EPass.
 */
	{
	  if (TestStepResult()==EPass)
		{
		TInt test;
		if(!GetIntFromConfig(ConfigSection(),KTe_LbsApiSuiteInt,test)
			)
			{
			// Leave if there's any error.
			User::Leave(KErrNotFound);
			}
		SetTestStepResult(EPass);
		StartL(test);
		}
	return TestStepResult();
	}



TVerdict CGenericInfoStep::doTestStepPostambleL()
/**
 * @return - TVerdict code
 * Override of base class virtual
 */
	{
	// process something post setting to the test step
	// uncomment the following line if you have common post setting to all the test steps in there
	// CTe_LbsApiSuiteStepBase::doTestStepPostambleL();
	// uncomment the following line if you have post process or remove the following line if no post process
	// SetTestStepResult(EPass);		// or EFail
	return TestStepResult();
	}


void CGenericInfoStep::StartL (TInt aIndex)
	{
	switch (aIndex)
		{
	case 0:
		HPositionGenericInfo_ConstructionL ();
		break;
	case 1:
		HPositionGenericInfo_Construction2L ();
		break;
	case 2:
		HPositionGenericInfo_RequestingL ();
		break;
	case 3:
		HPositionGenericInfo_Requesting2L ();
		break;
	case 4:
		HPositionGenericInfo_GetL ();
		break;
	case 5:
		HPositionGenericInfo_GoodL ();
		break;
	case 6:
		HPositionGenericInfo_MismatchedL ();
		break;
	case 7:
		HPositionGenericInfo_HBufCL ();
		break;
	case 8:
		HPositionGenericInfo_BufferL ();
		break;
	case 9:
		HPositionGenericInfo_FieldsL ();
		break;
	case 10:
		HPositionGenericInfo_OverflowL ();
		break;
	case 11:
		HPositionGenericInfo_SetL ();
		break;
	default:
		break;

		}
	}




//
//
// HPositionGenericInfo
//
//  Construction
//
//

// 3.2.1. Construction
void CGenericInfoStep::HPositionGenericInfo_ConstructionL()
	{
    DECLARE_ERROR_LOGGING;
    StandardPrepareL();

    HPositionGenericInfo *genInfo, *genInfoL, *genInfoLC, *genInfoDef;
    const TInt KBufSize = 8;
    const TInt KMaxFields = 2;

    __UHEAP_MARK;

    genInfo     = HPositionGenericInfo::New(KBufSize, KMaxFields);
    genInfoL    = HPositionGenericInfo::NewL(KBufSize, KMaxFields);
    genInfoLC   = HPositionGenericInfo::NewLC(KBufSize, KMaxFields);
    genInfoDef  = HPositionGenericInfo::New();

    __UHEAP_MARK;

    //check genInfo not NULL
    CHECK(genInfo);
    CHECK(genInfoL);
    CHECK(genInfoLC);
    CHECK(genInfoDef);

    // position class
    CHECK(genInfo->PositionClassType() == (EPositionInfoClass|EPositionGenericInfoClass));
    CHECK(genInfo->PositionClassSize() >= sizeof(HPositionGenericInfo) + KBufSize + KMaxFields);

    CHECK(genInfoL->PositionClassType() == (EPositionInfoClass|EPositionGenericInfoClass));
    CHECK(genInfoL->PositionClassSize() >= sizeof(HPositionGenericInfo) + KBufSize + KMaxFields);

    CHECK(genInfoLC->PositionClassType() == (EPositionInfoClass|EPositionGenericInfoClass));
    CHECK(genInfoLC->PositionClassSize() >= sizeof(HPositionGenericInfo) + KBufSize + KMaxFields);

    CHECK(genInfoDef->PositionClassType() == (EPositionInfoClass|EPositionGenericInfoClass));
    CHECK(genInfoDef->PositionClassSize() >= sizeof(HPositionGenericInfo) + KBufSize + KMaxFields);

    // sizes
    CHECK(genInfo->BufferSize() == KBufSize);
    CHECK(genInfo->MaxFields() == KMaxFields);

    CHECK(genInfoL->BufferSize() == KBufSize);
    CHECK(genInfoL->MaxFields() == KMaxFields);

    CHECK(genInfoLC->BufferSize() == KBufSize);
    CHECK(genInfoLC->MaxFields() == KMaxFields);

    CHECK(genInfoDef->BufferSize() == KPositionGenericInfoDefaultBufferSize);
    CHECK(genInfoDef->MaxFields() == KPositionGenericInfoDefaultMaxFields);
    CHECK_EQUAL_(KPositionGenericInfoDefaultBufferSize,  genInfoDef->BufferSize());
    CHECK_EQUAL_(KPositionGenericInfoDefaultMaxFields,   genInfoDef->MaxFields());

    // requested fields
    for(TUint16 i=EPositionFieldNone; i<EPositionFieldIdLast; i++)
    	{
        CHECK(!genInfo->IsRequestedField(i));
        CHECK(!genInfoL->IsRequestedField(i));
        CHECK(!genInfoLC->IsRequestedField(i));
        CHECK(!genInfoDef->IsRequestedField(i));
    	}
    // data fields
    for(TUint16 j=EPositionFieldNone; j<EPositionFieldIdLast; j++)
    	{
        CHECK(!genInfo->IsFieldAvailable(j));
        CHECK(!genInfoL->IsFieldAvailable(j));
        CHECK(!genInfoLC->IsFieldAvailable(j));
        CHECK(!genInfoDef->IsFieldAvailable(j));
    	}
    // field ids
    CHECK_EQUAL_(genInfo->FirstRequestedFieldId(), TPositionFieldId(EPositionFieldNone));
    CHECK_EQUAL_(genInfoL->FirstRequestedFieldId(), TPositionFieldId(EPositionFieldNone));
    CHECK_EQUAL_(genInfoLC->FirstRequestedFieldId(), TPositionFieldId(EPositionFieldNone));
    CHECK_EQUAL_(genInfoDef->FirstRequestedFieldId(), TPositionFieldId(EPositionFieldNone));
    for(TUint16 k = EPositionFieldNone; k < EPositionFieldIdLast; k++)
    	{
        CHECK_EQUAL_(genInfo->NextRequestedFieldId(k), TPositionFieldId(EPositionFieldNone));
        CHECK_EQUAL_(genInfoL->NextRequestedFieldId(k), TPositionFieldId(EPositionFieldNone));
        CHECK_EQUAL_(genInfoLC->NextRequestedFieldId(k), TPositionFieldId(EPositionFieldNone));
        CHECK_EQUAL_(genInfoDef->NextRequestedFieldId(k), TPositionFieldId(EPositionFieldNone));
    	}

    __UHEAP_MARKEND;

    delete genInfo;
    delete genInfoL;
    CleanupStack::PopAndDestroy(genInfoLC);
    delete genInfoDef;

    __UHEAP_MARKEND;

    StandardCleanup();
	}


// 3.2.2. NoMemory
// see NoMemoryTests.cpp

// 3.2.3 Invalid sizes
struct THPGIdata
    {
    TInt defBufSize;
    TInt defMaxFields;
    };

TInt Construction_New_Invalid(TAny* aPtr)
	{
    THPGIdata& data = *(static_cast<THPGIdata*>(aPtr));
    HPositionGenericInfo* posInfo = HPositionGenericInfo::New(data.defBufSize, data.defMaxFields);
    delete posInfo;
    return KErrGeneral;
	}


TInt Construction_NewL_Invalid(TAny* aPtr)
	{
    THPGIdata& data = *(static_cast<THPGIdata*>(aPtr));
    HPositionGenericInfo* posInfo = NULL;
    TInt err;
    TRAP(err, posInfo = HPositionGenericInfo::NewL(data.defBufSize, data.defMaxFields));
    delete posInfo;
    return KErrGeneral;
	}


TInt Construction_NewLC_Invalid(TAny* aPtr)
	{
    THPGIdata& data = *(static_cast<THPGIdata*>(aPtr));
    TInt err;
    TRAP(err, 
        HPositionGenericInfo* posInfo = HPositionGenericInfo::NewLC(data.defBufSize, data.defMaxFields);
        CleanupStack::PopAndDestroy(posInfo);
        );
    return KErrGeneral;
	}

#define TEST_CONSTRUCTION_NEW_INVALID(aPanicReason, aData)              \
        DO_PANIC_TEST_L_PTR(Construction_New_Invalid, KPosClientFault,   \
                aPanicReason, KDefaultTestTimeout, aData);

#define TEST_CONSTRUCTION_NEWL_INVALID(aPanicReason, aData)             \
        DO_PANIC_TEST_L_PTR(Construction_NewL_Invalid, KPosClientFault,  \
                aPanicReason, KDefaultTestTimeout, aData);

#define TEST_CONSTRUCTION_NEWLC_INVALID(aPanicReason, aData)            \
        DO_PANIC_TEST_L_PTR(Construction_NewLC_Invalid, KPosClientFault, \
                aPanicReason, KDefaultTestTimeout, aData);

void CGenericInfoStep::HPositionGenericInfo_Construction2L()
	{
    DECLARE_ERROR_LOGGING;
    StandardPrepareL();

    THPGIdata data;

    // zero max fields
    data.defBufSize = 1;
    data.defMaxFields = 0;
    TEST_CONSTRUCTION_NEW_INVALID(EPositionGenericInfoZeroMaxFields, &data);
    TEST_CONSTRUCTION_NEWL_INVALID(EPositionGenericInfoZeroMaxFields, &data);
    TEST_CONSTRUCTION_NEWLC_INVALID(EPositionGenericInfoZeroMaxFields, &data);

    // negative max fields
    data.defBufSize = 1;
    data.defMaxFields = -1;
    TEST_CONSTRUCTION_NEW_INVALID(EPositionGenericInfoZeroMaxFields, &data);
    TEST_CONSTRUCTION_NEWL_INVALID(EPositionGenericInfoZeroMaxFields, &data);
    TEST_CONSTRUCTION_NEWLC_INVALID(EPositionGenericInfoZeroMaxFields, &data);

    // zero buffer
    data.defBufSize = 0;
    data.defMaxFields = 1;
    TEST_CONSTRUCTION_NEW_INVALID(EPositionGenericInfoZeroBufferSize, &data);
    TEST_CONSTRUCTION_NEWL_INVALID(EPositionGenericInfoZeroBufferSize, &data);
    TEST_CONSTRUCTION_NEWLC_INVALID(EPositionGenericInfoZeroBufferSize, &data);

    // negative buffer
    data.defBufSize = -1;
    data.defMaxFields = 1;
    TEST_CONSTRUCTION_NEW_INVALID(EPositionGenericInfoZeroBufferSize, &data);
    TEST_CONSTRUCTION_NEWL_INVALID(EPositionGenericInfoZeroBufferSize, &data);
    TEST_CONSTRUCTION_NEWLC_INVALID(EPositionGenericInfoZeroBufferSize, &data);

    StandardCleanup();
	}


//
//
// HPositionGenericInfo
//
//  Requesting Fields
//
//

// 3.3.1. Requesting Fields, ony-by-one
void CGenericInfoStep::HPositionGenericInfo_RequestingL()
	{
    DECLARE_ERROR_LOGGING;
    StandardPrepareL();

    __UHEAP_MARK;

    HPositionGenericInfo* genInfo = HPositionGenericInfo::NewL();
    TUint maxField = genInfo->MaxFields();

    __UHEAP_MARK;

    // request
    CHECK_EQUAL_(KErrNone,           genInfo->SetRequestedField(TPositionFieldId(EPositionFieldNone-1))); // just very big value
    CHECK_EQUAL_(KErrArgument,       genInfo->SetRequestedField(EPositionFieldNone));
    CHECK_EQUAL_(EPositionFieldNone, genInfo->NextRequestedFieldId(TPositionFieldId(EPositionFieldNone-1)));

    TUint i = 0;
    for(i = EPositionFieldNone + 1; i < maxField; i++) // first is EPositionFieldNone-1
    	{
        CHECK_EQUAL_(KErrNone,       genInfo->SetRequestedField(TPositionFieldId(i)));
    	}
    CHECK_EQUAL_(KErrNone,           genInfo->SetRequestedField(TPositionFieldId(i-1))); // double setting doesn't fail
    CHECK_EQUAL_(KErrOverflow,       genInfo->SetRequestedField(TPositionFieldId(maxField + 1)));

    // check requested
    CHECK(genInfo->IsRequestedField(TPositionFieldId(EPositionFieldNone - 1)));
    CHECK(!genInfo->IsRequestedField(EPositionFieldNone));
    for(i = EPositionFieldNone + 1; i < maxField; i++)  // first is EPositionFieldNone-1
    	{
        CHECK(genInfo->IsRequestedField(TPositionFieldId(i)));
    	}
    CHECK(!genInfo->IsRequestedField(TPositionFieldId(maxField + 1)));

    // enumerate requested fields
    TUint fieldsCount = 0;
    for(TPositionFieldId field = genInfo->FirstRequestedFieldId();
        field > EPositionFieldNone;
        field = genInfo->NextRequestedFieldId(field))
    	{
        CHECK(genInfo->IsRequestedField(field));
        fieldsCount++;
    	}
    CHECK(fieldsCount == maxField); // check count here

    // clear all
    genInfo->ClearRequestedFields();

    CHECK_EQUAL_(EPositionFieldNone, genInfo->FirstRequestedFieldId());

    for(i = EPositionFieldNone + 1; i < maxField + 1; i++)
    	{
        CHECK(!genInfo->IsRequestedField(TPositionFieldId(i)));
        CHECK_EQUAL_(EPositionFieldNone, genInfo->NextRequestedFieldId(TPositionFieldId(i)));
    	}

    __UHEAP_MARKEND;

    delete genInfo;

    __UHEAP_MARKEND;

    StandardCleanup();
	}


// 3.3.2 Requesting Array of fields
void CGenericInfoStep::HPositionGenericInfo_Requesting2L()
	{
    DECLARE_ERROR_LOGGING;
    StandardPrepareL();

    __UHEAP_MARK;
    HPositionGenericInfo* genInfo = HPositionGenericInfo::NewL();
    TUint maxField = genInfo->MaxFields();
    __UHEAP_MARK;

    TPositionFieldId* fieldIds = new TPositionFieldId [maxField + 2];
    TPositionFieldId fid = 0;
    for (fid = 0; fid <= maxField; fid++)
    	{
        fieldIds[fid] = TPositionFieldId(fid + 1);
    	};
    fieldIds[maxField + 1] = EPositionFieldNone; // 1, 2, 3, 4, 5, ...  31,32,33,EPositionFieldNone

    CHECK_EQUAL_(KErrOverflow, genInfo->SetRequestedFields(fieldIds));
    fieldIds[maxField] = EPositionFieldNone; // 32 is replaced with EPositionFieldNone
    // now it should work
    CHECK_EQUAL_(KErrNone, genInfo->SetRequestedFields(fieldIds));

    // check requested
    CHECK(!genInfo->IsRequestedField(TPositionFieldId(EPositionFieldNone-1)));
    CHECK(!genInfo->IsRequestedField(EPositionFieldNone));
    CHECK_EQUAL_(EPositionFieldNone, genInfo->NextRequestedFieldId(TPositionFieldId(EPositionFieldNone-1)));
    for(fid = EPositionFieldNone + 1; fid < maxField + 1; fid++)
    	{
        CHECK(genInfo->IsRequestedField(fid));
    	}
    CHECK(!genInfo->IsRequestedField(TPositionFieldId(maxField + 1)));
    CHECK_EQUAL_(EPositionFieldNone, genInfo->NextRequestedFieldId(TPositionFieldId(maxField + 1)));

    // enumerate requested fields
    TUint fieldsCount = 0;
    for(TPositionFieldId field = genInfo->FirstRequestedFieldId();
        field > EPositionFieldNone;
        field = genInfo->NextRequestedFieldId(field))
    	{
        CHECK(genInfo->IsRequestedField(field));
        fieldsCount++;
    	}
    CHECK(fieldsCount == maxField); // check count here

    // clear all
    genInfo->ClearRequestedFields();

    CHECK_EQUAL_(EPositionFieldNone, genInfo->FirstRequestedFieldId());

    for(fid = EPositionFieldNone + 1; fid < maxField + 1; fid++)
    	{
        CHECK(!genInfo->IsRequestedField(fid));
        CHECK_EQUAL_(EPositionFieldNone, genInfo->NextRequestedFieldId(fid));
    	}

    delete fieldIds;
    __UHEAP_MARKEND;
    delete genInfo;
    __UHEAP_MARKEND;

    StandardCleanup();
	}


// 3.3.3 NextRequestedField for unrequested
// Implemented as part of 3.3.1 and 3.3.2

//
//
// HPositionGenericInfo
//
//  Field Data
//
//


// 3.4.1 Get/SetValue
void CGenericInfoStep::HPositionGenericInfo_GetL()
	{
    DECLARE_ERROR_LOGGING;
    StandardPrepareL();

    // SetData, Check data Availablity, Get, Check Data, Clear,
    // Check data unAvailability

    __UHEAP_MARK;

    HPositionGenericInfo* genInfo = HPositionGenericInfo::NewL();

    const TUint8 testUint8 = 0x25;									//Id == 1
    const TUint16 testUint16 = 0x9811;								//Id == 2
    const TUint32 testUint32 = 0xDEADBEEF;							//Id == 3
    const TInt8 testInt8 = 23;										//Id == 4
    const TInt16 testInt16 = 3452;									//Id == 5
    const TInt32 testInt32 = 2345234;								//Id == 6
    const TInt64 testInt64(0x0EAEFACEBEEFFACELL);					//Id == 7
    const TReal32 testReal32 = TReal32(34.123);						//Id == 8
    const TReal64 testReal64 = 3425.1234;							//Id == 9
    const TTimeIntervalMicroSeconds testTimeInterval(TInt64(2345));	//Id == 10
    const TTime testTime(TInt64(2345756));							//Id == 11
    _LIT8(KEightBitDes, "Hello to all you lovely people.");			//Id == 12
    _LIT16(KUnicodeDes, "Who likes a bit of Heavy Metal?");			//Id == 13

    // Add Data

    __UHEAP_MARK;

    CHECK_EQUAL_(KErrNone, genInfo->SetValue(1, testUint8));
    CHECK_EQUAL_(KErrNone, genInfo->SetValue(2, testUint16));
    CHECK_EQUAL_(KErrNone, genInfo->SetValue(3, testUint32));
    CHECK_EQUAL_(KErrNone, genInfo->SetValue(4, testInt8));
    CHECK_EQUAL_(KErrNone, genInfo->SetValue(5, testInt16));
    CHECK_EQUAL_(KErrNone, genInfo->SetValue(6, testInt32));
    CHECK_EQUAL_(KErrNone, genInfo->SetValue(7, testInt64));
    CHECK_EQUAL_(KErrNone, genInfo->SetValue(8, testReal32));
    CHECK_EQUAL_(KErrNone, genInfo->SetValue(9, testReal64));
    CHECK_EQUAL_(KErrNone, genInfo->SetValue(10, testTimeInterval));
    CHECK_EQUAL_(KErrNone, genInfo->SetValue(11, testTime));
    CHECK_EQUAL_(KErrNone, genInfo->SetValue(12, KEightBitDes));
    CHECK_EQUAL_(KErrNone, genInfo->SetValue(13, KUnicodeDes));

    // check that set fields are available ...
    for(TUint16 i=1; i<14; i++)
        {
        CHECK(genInfo->IsFieldAvailable(i));
        }
    // ... and unset are not
    for(TUint16 j=14; j<genInfo->MaxFields(); j++)
        {
        CHECK(!genInfo->IsFieldAvailable(j));
        }

    TUint8 checkUint8;
    TUint16 checkUint16;
    TUint32 checkUint32;
    TInt8 checkInt8;
    TInt16 checkInt16;
    TInt32 checkInt32;
    TInt64 checkInt64;
    TReal32 checkReal32;
    TReal64 checkReal64;
    TTimeIntervalMicroSeconds checkTimeInterval;
    TTime checkTime;
    //Descriptors - test with TPrtC and TBuf objects.
    TPtrC8 ptr8;
    TBuf8<48> buf8;
    TPtrC16 ptr16;
    TBuf16<48> buf16;

    // Check that data is available in fact

    CHECK_EQUAL_(KErrNone, genInfo->GetValue(1, checkUint8));
    CHECK_EQUAL_(KErrNone, genInfo->GetValue(2, checkUint16));
    CHECK_EQUAL_(KErrNone, genInfo->GetValue(3, checkUint32));
    CHECK_EQUAL_(KErrNone, genInfo->GetValue(4, checkInt8));
    CHECK_EQUAL_(KErrNone, genInfo->GetValue(5, checkInt16));
    CHECK_EQUAL_(KErrNone, genInfo->GetValue(6, checkInt32));
    CHECK_EQUAL_(KErrNone, genInfo->GetValue(7, checkInt64));
    CHECK_EQUAL_(KErrNone, genInfo->GetValue(8, checkReal32));
    CHECK_EQUAL_(KErrNone, genInfo->GetValue(9, checkReal64));
    CHECK_EQUAL_(KErrNone, genInfo->GetValue(10, checkTimeInterval));
    CHECK_EQUAL_(KErrNone, genInfo->GetValue(11, checkTime));
    CHECK_EQUAL_(KErrNone, genInfo->GetValue(12, ptr8));
    CHECK_EQUAL_(KErrNone, genInfo->GetValue(12, buf8));
    CHECK_EQUAL_(KErrNone, genInfo->GetValue(13, ptr16));
    CHECK_EQUAL_(KErrNone, genInfo->GetValue(13, buf16));

    // check data is returned correctly

    CHECK_EQUAL_(testUint8, checkUint8);
    CHECK_EQUAL_(testUint16, checkUint16);
    CHECK_EQUAL_(testUint32, checkUint32);
    CHECK_EQUAL_(testInt8, checkInt8);
    CHECK_EQUAL_(testInt16, checkInt16);
    CHECK_EQUAL_(testInt32, checkInt32);
    CHECK_EQUAL_(testInt64, checkInt64);
    CHECK_REAL_EQUAL(checkReal32, testReal32);
    CHECK_REAL_EQUAL(checkReal64, testReal64);
    CHECK(testTimeInterval == checkTimeInterval);
    CHECK(testTime == checkTime);
    CHECK(!ptr8.Compare(KEightBitDes));
    CHECK(!buf8.Compare(KEightBitDes));
    CHECK(!ptr16.Compare(KUnicodeDes));
    CHECK(!buf16.Compare(KUnicodeDes));

    //Do some checks for bad ids.
    CHECK_EQUAL_(KErrArgument, genInfo->SetValue(EPositionFieldNone, TUint8(1)));
    CHECK_EQUAL_(KErrArgument, genInfo->GetValue(EPositionFieldNone, checkInt8));

    // Clear Data
    genInfo->ClearPositionData();
    __UHEAP_MARKEND;

    // check that fields are not available any more
    for(TUint16 k=1; k<genInfo->MaxFields(); k++)
    	{
        CHECK(!genInfo->IsFieldAvailable(k));
    	}

    // check that values are not available any more

    CHECK_EQUAL_(KErrNotFound, genInfo->GetValue(1, checkUint8));
    CHECK_EQUAL_(KErrNotFound, genInfo->GetValue(2, checkUint16));
    CHECK_EQUAL_(KErrNotFound, genInfo->GetValue(3, checkUint32));
    CHECK_EQUAL_(KErrNotFound, genInfo->GetValue(4, checkInt8));
    CHECK_EQUAL_(KErrNotFound, genInfo->GetValue(5, checkInt16));
    CHECK_EQUAL_(KErrNotFound, genInfo->GetValue(6, checkInt32));
    CHECK_EQUAL_(KErrNotFound, genInfo->GetValue(7, checkInt64));
    CHECK_EQUAL_(KErrNotFound, genInfo->GetValue(8, checkReal32));
    CHECK_EQUAL_(KErrNotFound, genInfo->GetValue(9, checkReal64));
    CHECK_EQUAL_(KErrNotFound, genInfo->GetValue(10, checkTimeInterval));
    CHECK_EQUAL_(KErrNotFound, genInfo->GetValue(11, checkTime));
    CHECK_EQUAL_(KErrNotFound, genInfo->GetValue(12, ptr8));
    CHECK_EQUAL_(KErrNotFound, genInfo->GetValue(12, buf8));
    CHECK_EQUAL_(KErrNotFound, genInfo->GetValue(13, ptr16));
    CHECK_EQUAL_(KErrNotFound, genInfo->GetValue(13, buf16));

    delete genInfo;
    __UHEAP_MARKEND;

    StandardCleanup();
	}


// 3.4.2 Check Overflow doesn't harm existing data
#define CHECK_GOOD_OVERFLOW(type, val1, val2)                   \
	{                                                               \
    __UHEAP_MARK;                                               \
    HPositionGenericInfo* genInfo =                             \
            HPositionGenericInfo::NewL(sizeof(type));           \
    __UHEAP_MARK;                                               \
    CHECK_EQUAL_(KErrNone, genInfo->SetValue(1, type(val1)));    \
    CHECK(genInfo->IsFieldAvailable(1));                        \
    CHECK_EQUAL_(KErrPositionBufferOverflow, genInfo->SetValue(2, type(val2)));\
    CHECK(genInfo->IsFieldAvailable(1));                        \
    CHECK(!genInfo->IsFieldAvailable(2));                       \
    CHECK_EQUAL_(KErrNone, genInfo->SetValue(1, type(val2)));    \
    CHECK(genInfo->IsFieldAvailable(1));                        \
    type var;                                                   \
    CHECK_EQUAL_(KErrNone, genInfo->GetValue(1, var));           \
    CHECK(var == type(val2));                                   \
    __UHEAP_MARKEND;                                            \
    delete genInfo;                                             \
    __UHEAP_MARKEND;                                            \
	}


void CGenericInfoStep::HPositionGenericInfo_GoodL()
	{
    // we create a buffer with size precisely of size of tested
    // data type.
    // Then a value1 of this type is added to info. No error
    // Another value2 of same type is added - overflow.
    // check that value1 is available, value2 - is not
    // Reset field1 with value2, check it is available and correct

    DECLARE_ERROR_LOGGING;
    StandardPrepareL();

    CHECK_GOOD_OVERFLOW(TInt8, 1, 2);
    CHECK_GOOD_OVERFLOW(TInt16, 1, 2);
    CHECK_GOOD_OVERFLOW(TInt32, 1, 2);
    CHECK_GOOD_OVERFLOW(TInt64, 1, 2);
    CHECK_GOOD_OVERFLOW(TUint8, 1, 2);
    CHECK_GOOD_OVERFLOW(TUint16, 1, 2);
    CHECK_GOOD_OVERFLOW(TUint32, 1, 2);
    CHECK_GOOD_OVERFLOW(TReal32, 1, 2);
    CHECK_GOOD_OVERFLOW(TReal64, 1, 2);
    CHECK_GOOD_OVERFLOW(TTime, 1, 2);
    CHECK_GOOD_OVERFLOW(TTimeIntervalMicroSeconds, 1, 2);

    // a bit different for descriptors
    // TBuf for descriptors
    TBufC8<1> buf81(_L8("1"));
    TBufC8<1> buf82(_L8("2"));
    TBufC16<1> buf161(_L16("1"));
    TBufC16<1> buf162(_L16("2"));

    //CHECK_GOOD_OVERFLOW(TDesC8, buf81, buf82);
    	{
        __UHEAP_MARK;
        HPositionGenericInfo* genInfo = HPositionGenericInfo::NewL(sizeof(buf81));
        __UHEAP_MARK;
        CHECK_EQUAL_(KErrNone, genInfo->SetValue(1, buf81));
        CHECK(genInfo->IsFieldAvailable(1));
        CHECK_EQUAL_(KErrPositionBufferOverflow, genInfo->SetValue(2, buf82));
        CHECK(genInfo->IsFieldAvailable(1));
        CHECK(!genInfo->IsFieldAvailable(2));
        CHECK_EQUAL_(KErrNone, genInfo->SetValue(1, buf82));
        CHECK(genInfo->IsFieldAvailable(1));
        TBuf8<256> var;
        CHECK_EQUAL_(KErrNone, genInfo->GetValue(1, var));
        CHECK(var == buf82);
        __UHEAP_MARKEND;
        delete genInfo;
        __UHEAP_MARKEND;
    	}

    //CHECK_GOOD_OVERFLOW(TDesC16, buf161, buf162);
    	{
        __UHEAP_MARK;
        HPositionGenericInfo* genInfo = HPositionGenericInfo::NewL(sizeof(buf161));
        __UHEAP_MARK;
        CHECK_EQUAL_(KErrNone, genInfo->SetValue(1, buf161));
        CHECK(genInfo->IsFieldAvailable(1));
        CHECK_EQUAL_(KErrPositionBufferOverflow, genInfo->SetValue(2, buf162));
        CHECK(genInfo->IsFieldAvailable(1));
        CHECK(!genInfo->IsFieldAvailable(2));
        CHECK_EQUAL_(KErrNone, genInfo->SetValue(1, buf162));
        CHECK(genInfo->IsFieldAvailable(1));
        TBuf16<256> var;
        CHECK_EQUAL_(KErrNone, genInfo->GetValue(1, var));
        CHECK(var == buf162);
        __UHEAP_MARKEND;
        delete genInfo;
        __UHEAP_MARKEND;
    	}

    StandardCleanup();
	}


// 3.4.3 Mismatched data types
TInt Do_Mismatched_Data_Types1(TAny* /*aPtr*/)
	{
    HPositionGenericInfo* posInfo = HPositionGenericInfo::New();
    if(posInfo==NULL)
            return KErrGeneral;        //Failure will get picked up.
    if(posInfo->SetValue(1, TUint8(12))!=KErrNone)
    	{
        delete posInfo;
        return KErrGeneral;
    	}
    TBuf<10> buf;
    (void)posInfo->GetValue(1, buf);        //This line should panic. We ignore error, since failure gets picked up.
    delete posInfo;
    return KErrGeneral;
	}


void CGenericInfoStep::HPositionGenericInfo_MismatchedL()
	{
    DECLARE_ERROR_LOGGING;
    StandardPrepareL();

    DO_PANIC_TEST_L(Do_Mismatched_Data_Types1, KPosClientFault,
            EPositionGenericInfoMismatchDataType, KDefaultTestTimeout);

    StandardCleanup();
	}


// 3.4.4 Test HBufC
void CGenericInfoStep::HPositionGenericInfo_HBufCL()
	{
    DECLARE_ERROR_LOGGING;
    StandardPrepareL();

    HBufC* hbuf = HBufC::NewL(256);
    const TBuf<20> bufdata(_L("HBufC"));
    *hbuf = (TDesC&)bufdata;

    __UHEAP_MARK;
    HPositionGenericInfo* genInfo = HPositionGenericInfo::NewL(sizeof(*hbuf) + hbuf->Size());
    __UHEAP_MARK;

    CHECK_EQUAL_(KErrNone, genInfo->SetValue(1, *hbuf));
    CHECK(genInfo->IsFieldAvailable(1));
    CHECK_EQUAL_(KErrPositionBufferOverflow, genInfo->SetValue(2, *hbuf));
    CHECK(genInfo->IsFieldAvailable(1));
    CHECK(!genInfo->IsFieldAvailable(2));

    TBuf<20> newbuf;
    // load again
    CHECK(KErrNone == genInfo->GetValue(1, newbuf));
    CHECK(newbuf == bufdata);

    __UHEAP_MARKEND;
    delete genInfo;
    __UHEAP_MARKEND;

    delete hbuf;

    StandardCleanup();
	}


// 3.4.5 Buffer refill
void CGenericInfoStep::HPositionGenericInfo_BufferL()
	{
    // Fill the buffer fully and refill it again, from last element to first one,
    // in both cases set values from one with lowest size to biggest
    // No overflow should be generated

    DECLARE_ERROR_LOGGING;
    StandardPrepareL();

    const TInt8 int8 = 23;									//Id == 1
    const TUint8 uint8 = 0x25;								//Id == 2
    const TInt16 int16 = 3452;								//Id == 3
    const TUint16 uint16 = 0x9811;							//Id == 4
    const TInt32 int32 = 2345234;							//Id == 5
    const TUint32 uint32 = 0xDEADBEEF;						//Id == 6
    const TReal32 real32 = TReal32(34.123);					//Id == 7
    const TInt64 int64(0x0EAEFACEBEEFFACELL);				//Id == 8
    const TReal64 real64 = 3425.1234;						//Id == 9
    const TTimeIntervalMicroSeconds interval(TInt64(2345));	//Id == 10
    const TTime time(TInt64(2345756));						//Id == 11

    // calculate size
    TUint size =
        sizeof(int8) + sizeof(int16) + sizeof(int32) + sizeof(int64) +
        sizeof(uint8) + sizeof(uint16) + sizeof(uint32) +
        sizeof(real32) + sizeof(real64) +
        sizeof(time) + sizeof(interval);

    // create buffer
    __UHEAP_MARK;
    HPositionGenericInfo* genInfo = HPositionGenericInfo::NewL(size);
    __UHEAP_MARK;

    // fill with data. Note the sequence
    CHECK_EQUAL_(KErrNone, genInfo->SetValue(1, int8));
    CHECK_EQUAL_(KErrNone, genInfo->SetValue(2, uint8));
    CHECK_EQUAL_(KErrNone, genInfo->SetValue(3, int16));
    CHECK_EQUAL_(KErrNone, genInfo->SetValue(4, uint16));
    CHECK_EQUAL_(KErrNone, genInfo->SetValue(5, int32));
    CHECK_EQUAL_(KErrNone, genInfo->SetValue(6, uint32));
    CHECK_EQUAL_(KErrNone, genInfo->SetValue(7, real32));
    CHECK_EQUAL_(KErrNone, genInfo->SetValue(8, int64));
    CHECK_EQUAL_(KErrNone, genInfo->SetValue(9, real64));
    CHECK_EQUAL_(KErrNone, genInfo->SetValue(10, interval));
    CHECK_EQUAL_(KErrNone, genInfo->SetValue(11, time));

    // check that there is no more space
    CHECK_EQUAL_(KErrPositionBufferOverflow, genInfo->SetValue(12, int8));

    // check all are set
    for(TUint16 i = 1; i < 12; i++)
    {
        CHECK(genInfo->IsFieldAvailable(i));
    }
    // check that 12th field is not really added
    CHECK(!genInfo->IsFieldAvailable(12));

    // now fill in reverse sequence
    CHECK_EQUAL_(KErrNone, genInfo->SetValue(11, int8));
    CHECK_EQUAL_(KErrNone, genInfo->SetValue(10, uint8));
    CHECK_EQUAL_(KErrNone, genInfo->SetValue(9, int16));
    CHECK_EQUAL_(KErrNone, genInfo->SetValue(8, uint16));
    CHECK_EQUAL_(KErrNone, genInfo->SetValue(7, int32));
    CHECK_EQUAL_(KErrNone, genInfo->SetValue(6, uint32));
    CHECK_EQUAL_(KErrNone, genInfo->SetValue(5, real32));
    CHECK_EQUAL_(KErrNone, genInfo->SetValue(4, int64));
    CHECK_EQUAL_(KErrNone, genInfo->SetValue(3, real64));
    CHECK_EQUAL_(KErrNone, genInfo->SetValue(2, interval));
    CHECK_EQUAL_(KErrNone, genInfo->SetValue(1, time));

    // check that there is no more space again
    CHECK_EQUAL_(KErrPositionBufferOverflow, genInfo->SetValue(12, int8));

    // Note, that no Overflow error must be generated
    // because every time a field is replaced with value
    // that is of less or same size that is stored in the field!!!

    // check all are set
    for(TUint16 j = 1; j < 12; j++)
    {
        CHECK(genInfo->IsFieldAvailable(j));
    }
    // except last one
    CHECK(!genInfo->IsFieldAvailable(12));

    // read and check them
    TInt8 tint8;
    TUint8 tuint8;
    TInt16 tint16;
    TUint16 tuint16;
    TInt32 tint32;
    TUint32 tuint32;
    TReal32 treal32;
    TInt64 tint64;
    TReal64 treal64;
    TTimeIntervalMicroSeconds tinterval;
    TTime ttime;

    CHECK_EQUAL_(KErrNone, genInfo->GetValue(11, tint8));
    CHECK_EQUAL_(KErrNone, genInfo->GetValue(10, tuint8));
    CHECK_EQUAL_(KErrNone, genInfo->GetValue(9, tint16));
    CHECK_EQUAL_(KErrNone, genInfo->GetValue(8, tuint16));
    CHECK_EQUAL_(KErrNone, genInfo->GetValue(7, tint32));
    CHECK_EQUAL_(KErrNone, genInfo->GetValue(6, tuint32));
    CHECK_EQUAL_(KErrNone, genInfo->GetValue(5, treal32));
    CHECK_EQUAL_(KErrNone, genInfo->GetValue(4, tint64));
    CHECK_EQUAL_(KErrNone, genInfo->GetValue(3, treal64));
    CHECK_EQUAL_(KErrNone, genInfo->GetValue(2, tinterval));
    CHECK_EQUAL_(KErrNone, genInfo->GetValue(1, ttime));

    CHECK_EQUAL_(tint8, int8);
    CHECK_EQUAL_(tuint8, uint8);
    CHECK_EQUAL_(tint16, int16);
    CHECK_EQUAL_(tuint16, uint16);
    CHECK_EQUAL_(tint32, int32);
    CHECK_EQUAL_(tuint32, uint32);
    CHECK_EQUAL_(treal32, real32);
    CHECK_EQUAL_(tint64, int64);
    CHECK_EQUAL_(treal64, real64);
    CHECK_EQUAL_(tinterval, interval);
    CHECK_EQUAL_(ttime, time);

    genInfo->ClearPositionData();

    __UHEAP_MARKEND;
    delete genInfo;
    __UHEAP_MARKEND;

    StandardCleanup();
	}


// 3.4.6 Buffer refill 2
void CGenericInfoStep::HPositionGenericInfo_FieldsL()
	{
    // Allocate 8-byte buffer. Insert 1-byte value, 2-bytes value
    // and 4-bytes value. 1 byte remains free.
    // Replace second field (2-bytes) with 1-byte value.
    // No 2 bytes free.
    // Put 2-bytes value to the end of buffer. There is place certainly.
    // Will there right reallocation be done? Check availability and values

    DECLARE_ERROR_LOGGING;
    StandardPrepareL();

    const TInt8     f8 = 1;        //Id == 1
    const TInt16    f16 = 2;       //Id == 2
    const TInt32    f32 = 3;       //Id == 3
    const TInt8     newf16 = 5;    //Id == 2/2
    const TInt16    f8_2 = 4;      //Id == 4

    // calculate size
    TUint size = sizeof(f8) + sizeof(f16) + sizeof(f32); // 7 bytes

    // create buffer
    __UHEAP_MARK;
    HPositionGenericInfo* genInfo = HPositionGenericInfo::NewL(size + 1); // 8 bytes
    __UHEAP_MARK;

    // fill with data. Note the sequence
    CHECK_EQUAL_(KErrNone, genInfo->SetValue(1, f8));
    CHECK_EQUAL_(KErrNone, genInfo->SetValue(2, f16));
    CHECK_EQUAL_(KErrNone, genInfo->SetValue(3, f32));

    // check them
    CHECK(genInfo->IsFieldAvailable(1));
    CHECK(genInfo->IsFieldAvailable(2));
    CHECK(genInfo->IsFieldAvailable(3));

    // replace second one
    CHECK_EQUAL_(KErrNone, genInfo->SetValue(2, newf16)); // 2 bytes free
    CHECK(genInfo->IsFieldAvailable(2));

    // add 4th field
    CHECK_EQUAL_(KErrNone, genInfo->SetValue(4, f8_2)); // 0 bytes free
    CHECK(genInfo->IsFieldAvailable(4));

    // 5th ?
    CHECK_EQUAL_(KErrPositionBufferOverflow, genInfo->SetValue(5, f8_2)); // should fail
    CHECK(!genInfo->IsFieldAvailable(5));

    // check values
    TInt8     tf8 = 7;    //Id == 1
    TInt8     tf16 = 7;   //Id == 2
    TInt32    tf32 = 7;   //Id == 3
    TInt16    tf8_2 = 7;  //Id == 4

    CHECK_EQUAL_(KErrNone, genInfo->GetValue(1, tf8));
    CHECK_EQUAL_(KErrNone, genInfo->GetValue(2, tf16));
    CHECK_EQUAL_(KErrNone, genInfo->GetValue(3, tf32));
    CHECK_EQUAL_(KErrNone, genInfo->GetValue(4, tf8_2));

    CHECK(tf8 == f8);
    CHECK(tf16 == newf16);
    CHECK(tf32 == f32);
    CHECK(tf8_2 == f8_2);

    genInfo->ClearPositionData();

    __UHEAP_MARKEND;
    delete genInfo;
    __UHEAP_MARKEND;

    // Here we do another test. Emulate situation
    // when even compression doesn't help
    // (the test is discovered after using CTC++)

    __UHEAP_MARK;
    genInfo = HPositionGenericInfo::NewL(5); // 5 bytes
    __UHEAP_MARK;

    // add fields
    CHECK_EQUAL_(KErrNone, genInfo->SetValue(1, TUint8(1)));     // values are equal to
    CHECK_EQUAL_(KErrNone, genInfo->SetValue(2, TUint16(2)));    // field indices
    CHECK_EQUAL_(KErrNone, genInfo->SetValue(3, TUint8(3))); // one byte remains free
    // check them
    CHECK(genInfo->IsFieldAvailable(1));
    CHECK(genInfo->IsFieldAvailable(2));
    CHECK(genInfo->IsFieldAvailable(3));

    // free one byte in the middle
    CHECK_EQUAL_(KErrNone, genInfo->SetValue(2, TUint8(2)));
    // now 2 bytes available
    // add 4 bytes still
    CHECK_EQUAL_(KErrPositionBufferOverflow, genInfo->SetValue(2, TUint32(4)));

    TUint8 testUint = 0;
    // check all is ok still
    // ... and other two fields
    CHECK_EQUAL_(KErrNone, genInfo->GetValue(1, testUint));
    CHECK(testUint == 1);
    CHECK_EQUAL_(KErrNone, genInfo->GetValue(3, testUint));
    CHECK(testUint == 3);

    genInfo->ClearPositionData();

    __UHEAP_MARKEND;
    delete genInfo;
    __UHEAP_MARKEND;

    StandardCleanup();
	}

// 3.4.7 Descriptors overflow
void CGenericInfoStep::HPositionGenericInfo_OverflowL()
	{
    DECLARE_ERROR_LOGGING;
    StandardPrepareL();

    HPositionGenericInfo* genInfo = HPositionGenericInfo::NewL();

    // Byte
    _LIT8(KBigNarrowDes, "The universe is estimated to be approx 13Gyr old");
    TBuf8<10> smallNarrowBuf;
    smallNarrowBuf.SetLength(5);

    genInfo->SetValue(1, KBigNarrowDes);

    CHECK_EQUAL_(5, smallNarrowBuf.Length());
    CHECK_EQUAL_(KErrOverflow, genInfo->GetValue(1, smallNarrowBuf));
    CHECK_EQUAL_(0, smallNarrowBuf.Length());

    // Unicode
    _LIT16(KBigUnicodeDes, "There is a greater mass of ants than of humans on earth");
    TBuf16<10> smallUnicodeBuf;
    smallUnicodeBuf.SetLength(5);

    genInfo->SetValue(1, KBigUnicodeDes);

    CHECK_EQUAL_(5, smallUnicodeBuf.Length());
    CHECK_EQUAL_(KErrOverflow, genInfo->GetValue(1, smallUnicodeBuf));
    CHECK_EQUAL_(0, smallUnicodeBuf.Length());

    delete genInfo;

    StandardCleanup();
	}

// 3.4.8 Set too many fields
void CGenericInfoStep::HPositionGenericInfo_SetL()
	{
    DECLARE_ERROR_LOGGING;
    StandardPrepareL();

    __UHEAP_MARK;
    HPositionGenericInfo* genInfo = HPositionGenericInfo::NewL(0x40);
    __UHEAP_MARK;

    for(TUint16 i = 1; i < genInfo->MaxFields() + 1; i++)
    	{
        CHECK_EQUAL_(KErrNone, genInfo->SetValue(TPositionFieldId(i), TUint8(i)));
    	}
    CHECK_EQUAL_(KErrOverflow, genInfo->SetValue(
    TPositionFieldId(genInfo->MaxFields() + 1), TUint8(1))); // some value

    // Now check things are ok
    for (TInt j = 2; j < genInfo->MaxFields() + 1; j++)
    	{
    	TUint8 testUint = 0;
        CHECK_EQUAL_(KErrNone, genInfo->GetValue(TPositionFieldId(j), testUint));
        CHECK_EQUAL_(j, testUint);
    	}

    genInfo->ClearPositionData();
    __UHEAP_MARKEND;
    delete genInfo;
    __UHEAP_MARKEND;

    StandardCleanup();
	}
