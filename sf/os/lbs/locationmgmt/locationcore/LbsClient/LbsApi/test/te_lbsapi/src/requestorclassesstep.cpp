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
 @file RequestorClassesStep.cpp
 @internalTechnology
*/
#include "requestorclassesstep.h"
#include "te_lbsapisuitedefs.h"

#include "lcfsbucommondefinitions.h"

#include <lbs.h>
#include <lbssatellite.h>

#include <s32mem.h>

CRequestorClassesStep::~CRequestorClassesStep()
/**
 * Destructor
 */
	{
	}

CRequestorClassesStep::CRequestorClassesStep()
/**
 * Constructor
 */
	{
	// **MUST** call SetTestStepName in the constructor as the controlling
	// framework uses the test step name immediately following construction to set
	// up the step's unique logging ID.
	SetTestStepName(KRequestorClassesStep);
	}

TVerdict CRequestorClassesStep::doTestStepPreambleL()
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


TVerdict CRequestorClassesStep::doTestStepL()
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



TVerdict CRequestorClassesStep::doTestStepPostambleL()
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


void CRequestorClassesStep::StartL (TInt aIndex)
	{
	switch (aIndex)
		{
	case 0:
		RRequestorStack_StreamingL ();
		break;
	case 1:
		RRequestorStack_Streaming2L ();
		break;
	case 2:
		CRequestor_ConstructionL ();
		break;
	case 3:
		CRequestor_SetL ();
		break;
	case 4:
		CRequestor_Construction2L ();
		break;
	case 5:
		CRequestor_StreamingL ();
		break;
	case 6:
		CRequestor_Streaming2L ();
		break;
	default:
		break;
		}
	}


//
//
// RRequestorStack
//
//

void ResetAndDestroyRequestorStack(TAny* aArray)
    {
    RRequestorStack* stack = static_cast<RRequestorStack*>(aArray);
    stack->ResetAndDestroy();
    }

// 8.1.1. Streaming
void CRequestorClassesStep::RRequestorStack_StreamingL()
	{
    DECLARE_ERROR_LOGGING;
    StandardPrepareL();

    RRequestorStack requestorStack;
    CleanupStack::PushL(TCleanupItem(ResetAndDestroyRequestorStack, &requestorStack));

    _LIT(KDummyLit1, "Hello");
    const CRequestor::TRequestorType KDummyRequestorType1(CRequestor::ERequestorService);
    const CRequestor::TRequestorFormat KDummyRequestorFormat1(CRequestor::EFormatTelephone);

    CRequestor* requestor = CRequestor::NewLC(KDummyRequestorType1, KDummyRequestorFormat1, KDummyLit1);
    User::LeaveIfError(requestorStack.Append(requestor));        //take ownership of requestor
    CleanupStack::Pop(requestor);
    requestor = NULL;        //Just to make it clear we don't one requestor.

    _LIT(KDummyLit2, "Goodbye");
    const CRequestor::TRequestorType KDummyRequestorType2(CRequestor::ERequestorContact);
    const CRequestor::TRequestorFormat KDummyRequestorFormat2(CRequestor::EFormatUrl);

    requestor = CRequestor::NewLC(KDummyRequestorType2, KDummyRequestorFormat2, KDummyLit2);
    User::LeaveIfError(requestorStack.Append(requestor));        //take ownership of requestor
    CleanupStack::Pop(requestor);
    requestor = NULL;        //Just to make it clear we don't one requestor.

    //All info in requestorStack. Now stream out and back into a new requestor stack.

    CBufFlat* buffer = CBufFlat::NewL(512);        //Abritrary size chosen
    CleanupStack::PushL(buffer);
    RBufWriteStream writeStream;
    writeStream.Open(*buffer);
    writeStream.PushL();
    // externalize
    requestorStack.ExternalizeL(writeStream);
    writeStream.CommitL();
    writeStream.Pop();
    //requestorStack now piped into buffer.
    //get rid of requestorStack.
    CleanupStack::Pop(buffer);
    CleanupStack::PopAndDestroy(&requestorStack);
    CleanupStack::PushL(buffer);

    RBufReadStream readStream;
    readStream.Open(*buffer);
    readStream.PushL();

    RRequestorStack requestorStack2;
    CleanupStack::PushL(TCleanupItem(ResetAndDestroyRequestorStack, &requestorStack2));
    // internalize
    requestorStack2.InternalizeL(readStream);
    //requestorStack2 should now be initialised.

    CHECK_EQUAL_(2, requestorStack2.Count());

    requestor = requestorStack2[0];
    CHECK(!TPtrC(KDummyLit1).Compare(requestor->RequestorData()));
    CHECK_EQUAL_(requestor->RequestorType(), KDummyRequestorType1);
    CHECK_EQUAL_(requestor->RequestorFormat(), KDummyRequestorFormat1);

    requestor = requestorStack2[1];
    CHECK(!TPtrC(KDummyLit2).Compare(requestor->RequestorData()));
    CHECK_EQUAL_(requestor->RequestorType(), KDummyRequestorType2);
    CHECK_EQUAL_(requestor->RequestorFormat(), KDummyRequestorFormat2);

    CleanupStack::PopAndDestroy(3, buffer);

    StandardCleanup();
	}


// 8.1.2. Streaming with invalid stream
TInt RRequestorStack_Streaming_Invalid(TAny* /*aPtr*/)
	{
    RBufReadStream readStream;

    // buffer
        CBufFlat* buffer = CBufFlat::NewL(512);        //Arbitrary size chosen
    RBufWriteStream writeStream; // make sure it is empty
    writeStream.Open(*buffer);
    writeStream.WriteUint16L(12345); // put dummy value there (different from KRequestorStackVersion)
    writeStream.Close();
    writeStream.Release();

    // read-stream
    readStream.Open(*buffer);

    TInt err;
    TRAP(err,
        // internalize it
        RRequestorStack requestorStack2;
        CleanupStack::PushL(TCleanupItem(ResetAndDestroyRequestorStack, &requestorStack2));

        requestorStack2.InternalizeL(readStream);   //This should panic

        CleanupStack::Pop();
    	);

    readStream.Release();
    delete buffer;

    return KErrGeneral;
	}


void CRequestorClassesStep::RRequestorStack_Streaming2L()
	{
    DECLARE_ERROR_LOGGING;
    StandardPrepareL();

    // attempt to internalize from invalid stream
    DO_PANIC_TEST_L(
        RRequestorStack_Streaming_Invalid,
        KPosClientFault,
        EPositionInvalidClassType,
        KDefaultTestTimeout);

    StandardCleanup();
	}


// 8.1.3. Streaming with No Memory
// See NoMemoryTests.cpp

//
//
// CRequestor
//
//

// 8.2.1. Construction
void CRequestorClassesStep::CRequestor_ConstructionL()
	{
    DECLARE_ERROR_LOGGING;
    StandardPrepareL();

    _LIT(KDummyLit, "Hello");
    const CRequestor::TRequestorType KDummyRequestorType(CRequestor::ERequestorService);
    const CRequestor::TRequestorFormat KDummyRequestorFormat(CRequestor::EFormatTelephone);

    CRequestor* requestor = NULL;

    requestor = CRequestor::NewL(KDummyRequestorType, KDummyRequestorFormat, KDummyLit);
    CHECK(requestor->RequestorData().Compare(KDummyLit) == 0);
    CHECK(requestor->RequestorType()   == KDummyRequestorType);
    CHECK(requestor->RequestorFormat() == KDummyRequestorFormat);
    delete requestor;

    requestor = CRequestor::NewLC(KDummyRequestorType, KDummyRequestorFormat, KDummyLit);
    CHECK(requestor->RequestorData().Compare(KDummyLit) == 0);
    CHECK(requestor->RequestorType()   == KDummyRequestorType);
    CHECK(requestor->RequestorFormat() == KDummyRequestorFormat);
    CleanupStack::PopAndDestroy(requestor);

    requestor = static_cast<CRequestor*>( User::LeaveIfNull(CRequestor::New(KDummyRequestorType, KDummyRequestorFormat, KDummyLit)));
    CHECK(requestor->RequestorData().Compare(KDummyLit) == 0);
    CHECK(requestor->RequestorType()   == KDummyRequestorType);
    CHECK(requestor->RequestorFormat() == KDummyRequestorFormat);
    delete requestor;

    StandardCleanup();
	}


// 8.2.2. Setters and Getters
void CRequestorClassesStep::CRequestor_SetL()
	{
    DECLARE_ERROR_LOGGING;
    StandardPrepareL();

    // Get
    _LIT(KDummyLit, "Hello");
    const CRequestor::TRequestorType KDummyRequestorType(CRequestor::ERequestorService);
    const CRequestor::TRequestorFormat KDummyRequestorFormat(CRequestor::EFormatTelephone);
    CRequestor* requestor = CRequestor::NewL(KDummyRequestorType, KDummyRequestorFormat, KDummyLit);
    CleanupStack::PushL(requestor);

    TPtrC ptr;
    CRequestor::TRequestorType requestorType;
    CRequestor::TRequestorFormat requestorFormat;

    requestor->GetRequestor(requestorType, requestorFormat, ptr);

    CHECK(ptr.Compare(KDummyLit)    == 0);
    CHECK(requestorType             == KDummyRequestorType);
    CHECK(requestorFormat           == KDummyRequestorFormat);

    // Set
    _LIT(KNewDummyLit, "Goodbye");
    const CRequestor::TRequestorType KNewDummyRequestorType(CRequestor::ERequestorContact);
    const CRequestor::TRequestorFormat KNewDummyRequestorFormat(CRequestor::EFormatUrl);
    requestor->SetRequestorL(KNewDummyRequestorType, KNewDummyRequestorFormat, KNewDummyLit);

    // check as properties
    CHECK(requestor->RequestorData().Compare(KNewDummyLit) == 0);
    CHECK(requestor->RequestorType()   == KNewDummyRequestorType);
    CHECK(requestor->RequestorFormat() == KNewDummyRequestorFormat);

    // check by GetRequestor
    requestor->GetRequestor(requestorType, requestorFormat, ptr);

    CHECK(ptr.Compare(KNewDummyLit) == 0);
    CHECK(requestorType             == KNewDummyRequestorType);
    CHECK(requestorFormat           == KNewDummyRequestorFormat);

    CleanupStack::PopAndDestroy();

    // Now set much longer requestor data, to check if
    // doesn't matter :-)
    TBuf<1> shortData;
    shortData.Append('$');
    requestor = CRequestor::NewLC(KDummyRequestorType, KDummyRequestorFormat, shortData);

    TBuf<512> longData;
    longData.Fill('@', longData.MaxLength());
    requestor->SetRequestorL(KNewDummyRequestorType, KNewDummyRequestorFormat, longData);

    // check it
    CHECK(requestor->RequestorData().Compare(longData) == 0);
    CHECK(requestor->RequestorType()   == KNewDummyRequestorType);
    CHECK(requestor->RequestorFormat() == KNewDummyRequestorFormat);

    // set smaller one again
    requestor->SetRequestorL(KDummyRequestorType, KDummyRequestorFormat, shortData);
    CHECK(requestor->RequestorData().Compare(shortData) == 0);

    CleanupStack::PopAndDestroy();

    StandardCleanup();
	}


/*
// 8.2.3. Constructors with invalid parameters
void CRequestorClassesStep::CRequestor_Construction3L()
{
    // there are no relevant parameters to pass invalid values.
    // the only one is wrong stream, see 8.3.4 CRequestor_Streaming_Invalid
}
*/

// 8.2.4. Constructors with No Memory
// See NoMemoryTests.cpp

// 8.2.5. SetRequestor with No Memory
// See NoMemoryTests.cpp


// 8.3.1. Construction from stream
void CRequestorClassesStep::CRequestor_Construction2L()
	{
    DECLARE_ERROR_LOGGING;
    StandardPrepareL();

    _LIT(KDummyLit, "Hello");
    const CRequestor::TRequestorType KDummyRequestorType(CRequestor::ERequestorService);
    const CRequestor::TRequestorFormat KDummyRequestorFormat(CRequestor::EFormatTelephone);
    CRequestor* requestor1 = CRequestor::NewLC(KDummyRequestorType, KDummyRequestorFormat, KDummyLit);

    //Make a write stream
    HBufC8* buffer = HBufC8::NewLC(0x100);        //Should be plenty big enough
    TPtr8 ptr = buffer->Des();
    RDesWriteStream writeStream(ptr);        //Opens the write stream
    CleanupClosePushL(writeStream);
    requestor1->ExternalizeL(writeStream);
    writeStream.CommitL();
    CleanupStack::PopAndDestroy(&writeStream);        //Close the write stream

    //Make a read stream
    RDesReadStream readStream(*buffer);
    CleanupClosePushL(readStream);

    // construct
    CRequestor* requestor2 = CRequestor::NewL(readStream);

    CHECK(!TPtrC(KDummyLit).Compare(requestor2->RequestorData()));
    CHECK_EQUAL_(requestor2->RequestorType(), KDummyRequestorType);
    CHECK_EQUAL_(requestor2->RequestorFormat(), KDummyRequestorFormat);

    delete requestor2;
    CleanupStack::PopAndDestroy(3, requestor1);        //readStream, buffer, requestor1

    StandardCleanup();
	}


// 8.3.2. Externalize/Internalize
void CRequestorClassesStep::CRequestor_StreamingL()
	{
    DECLARE_ERROR_LOGGING;
    StandardPrepareL();

    _LIT(KDummyLit1, "Hello");
    const CRequestor::TRequestorType KDummyRequestorType1(CRequestor::ERequestorService);
    const CRequestor::TRequestorFormat KDummyRequestorFormat1(CRequestor::EFormatTelephone);
    CRequestor* requestor1 = CRequestor::NewLC(KDummyRequestorType1, KDummyRequestorFormat1, KDummyLit1);

    // set r2 with other values
    _LIT(KDummyLit2, "Goodbye");
    const CRequestor::TRequestorType KDummyRequestorType2(CRequestor::ERequestorContact);
    const CRequestor::TRequestorFormat KDummyRequestorFormat2(CRequestor::EFormatUrl);
    CRequestor* requestor2 = CRequestor::NewLC(KDummyRequestorType2, KDummyRequestorFormat2, KDummyLit2);

    // Make a write stream
    HBufC8* buffer = HBufC8::NewLC(0x100);        //Should be plenty big enough
    TPtr8 ptr = buffer->Des();
    RDesWriteStream writeStream(ptr);        //Opens the write stream
    CleanupClosePushL(writeStream);

    // externalize r1
    writeStream << *requestor1;
    writeStream.CommitL();
    CleanupStack::PopAndDestroy(&writeStream);        //Close the write stream

    RDesReadStream readStream(*buffer);
    CleanupClosePushL(readStream);
    // internalize r2
    requestor2->InternalizeL(readStream);
    CleanupStack::PopAndDestroy(&readStream);        //Close the readStream

    // check r2
    CHECK(requestor2->RequestorData().Compare(KDummyLit1) == 0);
    CHECK(requestor2->RequestorType()   == KDummyRequestorType1);
    CHECK(requestor2->RequestorFormat() == KDummyRequestorFormat1);

    CleanupStack::PopAndDestroy(3, requestor1);

    StandardCleanup();
	}


// 8.3.3. Streaming with NoMemory
// See NoMemoryTests.cpp

// 8.3.4. Invalid stream
TInt CRequestor_Internalize_Invalid_Stream(TAny* /*aPtr*/)
	{
    CRequestor* requestor = NULL;
    RBufReadStream readStream;

    // buffer
    CBufFlat* buffer = CBufFlat::NewL(512);        //Abritrary size chosen
    RBufWriteStream writeStream; // make sure it is empty
    writeStream.Open(*buffer);
    writeStream.WriteUint16L(12345); // put dummy value there (different from KRequestorVersion)
    writeStream.Close();
    writeStream.Release();
    readStream.Open(*buffer);

    TInt err;
    TRAP(err,
                // internalize it
            requestor = CRequestor::New(DUMMY_REQUESTOR_TYPE_ACCEPTED1, DUMMY_REQUESTOR_FORMAT_ACCEPTED1, DUMMY_REQUESTOR_NAME_ACCEPTED1);
                CleanupStack::PushL(requestor);

            requestor->InternalizeL(readStream);   //This should panic

            CleanupStack::PopAndDestroy();
    	);
    User::LeaveIfError(err);

    readStream.Release();
    delete buffer;

    return KErrGeneral;
	}


TInt CRequestor_Construct_Invalid_Stream(TAny* /*aPtr*/)
	{
    CRequestor* requestor = NULL;
    RBufReadStream readStream;

    // buffer
    CBufFlat* buffer = CBufFlat::NewL(512);        //Arbitrary size chosen
    RBufWriteStream writeStream; // make sure it is empty
    writeStream.Open(*buffer);
    writeStream.WriteUint16L(12345); // put dummy value there (different from KRequestorVersion)
    writeStream.Close();
    writeStream.Release();

    // read-stream
    readStream.Open(*buffer);

    TInt err;
    TRAP(err,
        // internalize it
        requestor = CRequestor::NewL(readStream);   //This should panic
    	);

    delete requestor;
    readStream.Release();
    delete buffer;

    return KErrGeneral;
	}


void CRequestorClassesStep::CRequestor_Streaming2L()
	{
    DECLARE_ERROR_LOGGING;
    StandardPrepareL();

    // attempt to internalize from invalid stream
    DO_PANIC_TEST_L(
        CRequestor_Internalize_Invalid_Stream,
        KPosClientFault,
        EPositionInvalidClassType,
        KDefaultTestTimeout);
    // attempt to construct from invalid stream
    DO_PANIC_TEST_L(
        CRequestor_Construct_Invalid_Stream,
        KPosClientFault,
        EPositionInvalidClassType,
        KDefaultTestTimeout);

    StandardCleanup();
	}

