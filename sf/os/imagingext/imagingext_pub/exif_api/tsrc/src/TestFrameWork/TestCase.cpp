/*
* Copyright (c) 2002 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description: ExifLibTest
*
*/


#include <utf.h>

#include "TestFrameWork/testCase.h"
#include "TestFrameWork/AssertFailure.h"
#include <StifTestModule.h>

// Assertion failure message formats:
_LIT8(KIntsNotEqualFormat,"expected: %d but was: %d");
_LIT8(KRealsNotEqualFormat,"expected: %g but was: %g");                                                    
_LIT8(KDesCsNotEqualFormat,"expected: '%S' but was: '%S'");


// A unique error code:
const TInt KErrCppUnitAssertionFailed=(-99999999);



void CTestCase::ConstructL (const TDesC8& aName)
    {
    iName = aName.AllocL();
    User::LeaveIfError(Dll::SetTls(0));
    }


CTestCase::CTestCase ()
: iAllocFailureType(RHeap::ENone),
iAllocFailureRate(0)
    {
    }


CTestCase::~CTestCase () 
    { 
    delete iName;
    }


// From MTest:
void CTestCase::ExecuteL (TTestResult& aResult)
    {
    TInt error = ExecuteImplL();
    aResult.iResult = error;
    // add the possible failure or error to the result
    
    if (error == KErrCppUnitAssertionFailed)
        {
        CAssertFailure* assertFailure = AssertFailureFromTlsL ();
        CleanupStack::PushL(assertFailure);
        TBuf16 <0x80> convertBuf;
        convertBuf.Copy(assertFailure->What());
        aResult.iResultDes.Append(convertBuf);
        aResult.iResultDes.AppendFormat(_L(" at Line %i of"), assertFailure->LineNumber()); 
        convertBuf.Copy(assertFailure->FileName());
        aResult.iResultDes.Append(convertBuf);
        CleanupStack::PopAndDestroy(assertFailure); 
        }
    }

// From MTest:
TInt CTestCase::CountTestCases ()
    {
    return 1;
    }


// From MTest:
const TDesC8& CTestCase::Name ()
    {
    return *iName;
    }



#if 1
// From MTest:
void CTestCase::ExecuteTestL(TTestResult& aResult,
                             TInt /*aIndex */)
    { 
    ExecuteL(aResult);
    }

const TDesC8& CTestCase::TestCaseName (TInt /*aIndex*/) 
    {
    return Name();
    }
#endif



// Check for a failed general assertion 
void CTestCase::AssertL (TBool aCondition,
                         const TDesC8& aConditionExpression,
                         TInt aLineNumber,
                         const TDesC8& aFileName)
    {
    if (!aCondition)
        {
        AllocFailureSimulation(EFalse);
        AssertFailureToTlsL (aConditionExpression,aLineNumber,aFileName);
        User::Leave (KErrCppUnitAssertionFailed);
        }
    }


// Check for a failed equality assertion
void CTestCase::AssertEqualsL (TInt anExpected, 
                               TInt anActual, 
                               TInt aLineNumber,
                               const TDesC8& aFileName)
    {
    if (anExpected != anActual)
        {
        AllocFailureSimulation(EFalse);
        HBufC8* msg = HBufC8::NewLC (KIntsNotEqualFormat().Size()+100);
        msg->Des().Format(KIntsNotEqualFormat, anExpected, anActual);
        AssertFailureToTlsL(*msg,aLineNumber,aFileName);
        CleanupStack::PopAndDestroy(); // msg
        User::Leave (KErrCppUnitAssertionFailed);
        }
    }


// Check for a failed equality assertion
void CTestCase::AssertEqualsL (TReal anExpected, 
                               TReal anActual, 
                               TReal aDelta,
                               TInt aLineNumber,
                               const TDesC8& aFileName)
    {
    if (Abs(anExpected-anActual) > aDelta)
        {
        AllocFailureSimulation(EFalse);
        HBufC8* msg = HBufC8::NewLC (KRealsNotEqualFormat().Size()+100);
        msg->Des().Format(KRealsNotEqualFormat, anExpected, anActual);
        AssertFailureToTlsL(*msg,aLineNumber,aFileName);
        CleanupStack::PopAndDestroy(); // msg
        User::Leave (KErrCppUnitAssertionFailed);
        }
    }


// Check equality of the contents of two 8-bit descriptors 
void CTestCase::AssertEqualsL (const TDesC8& anExpected,
                               const TDesC8& anActual,
                               TInt aLineNumber,
                               const TDesC8& aFileName)
    {
    if (anExpected.Compare(anActual) != 0)
        {
        AllocFailureSimulation(EFalse);
        HBufC8* msg = NotEqualsMessageLC (anExpected, anActual);
        AssertFailureToTlsL(*msg,aLineNumber,aFileName);
        CleanupStack::PopAndDestroy(); // msg
        User::Leave (KErrCppUnitAssertionFailed);
        }
    }


// Check equality of the contents of two 16-bit descriptors 
void CTestCase::AssertEqualsL (const TDesC16& aExpected,
                               const TDesC16& aActual,
                               TInt aLineNumber,
                               const TDesC8& aFileName)
    {
    if (aExpected.Compare(aActual) != 0)
        {
        AllocFailureSimulation(EFalse);
        HBufC8* msg = NotEqualsMessageLC (aExpected, aActual);
        AssertFailureToTlsL(*msg,aLineNumber,aFileName);
        CleanupStack::PopAndDestroy(); // msg
        User::Leave (KErrCppUnitAssertionFailed);
        }
    }


void CTestCase::AllocFailureSimulation (TBool aSwitchedOn)
    {
    if (aSwitchedOn)
        {
        __UHEAP_SETFAIL (iAllocFailureType, iAllocFailureRate);
        }
    else
        {
        __UHEAP_RESET;
        }
    }


TInt CTestCase::ExecuteImplL ()
    {
    
    __UHEAP_MARK;
    TRAPD (setupError, setUpL());
    if (setupError != KErrNone)
        {
        tearDown();
        __UHEAP_MARKEND;
        User::Leave(setupError);
        }
    
    TRAPD (executionError, executeTestL());
    
    tearDown();
    __UHEAP_MARKENDC(HeapCellsReservedByAssertFailure());

    return executionError;
    }


// Construct the message and put it in the cleanup stack
HBufC8* CTestCase::NotEqualsMessageLC (const TDesC8& aExpected,
                                       const TDesC8& aActual)
    {
    TInt size = KDesCsNotEqualFormat().Size()+aExpected.Size()+aActual.Size();
    HBufC8 *msg = HBufC8::NewLC(size);
    msg->Des().Format(KDesCsNotEqualFormat, &aExpected, &aActual);
    return msg;
    }


// Construct the message and put it in the cleanup stack
HBufC8* CTestCase::NotEqualsMessageLC (const TDesC16& aExpected,
                                       const TDesC16& aActual)
    {
    TInt length = KDesCsNotEqualFormat().Length() + 
        aExpected.Length() + aActual.Length();
    
    HBufC8* msg = HBufC8::NewLC(length);
    
    // Convert 16-bit to 8-bit to ensure readability
    // of the output possibly directed to a file.
    HBufC8* expected = HBufC8::NewLC(aExpected.Length());
    expected->Des().Copy(aExpected);
    
    HBufC8* actual = HBufC8::NewLC(aActual.Length());
    actual->Des().Copy(aActual);
    
    msg->Des().Format(KDesCsNotEqualFormat,expected,actual);
    
    // pop and destroy actual and expected
    CleanupStack::PopAndDestroy(2);
    return msg;
    }


void CTestCase::AssertFailureToTlsL (const TDesC8& aMessage,
                                     TInt  aLineNumber,
                                     const TDesC8& aFileName)
    {
    TInt cellsBefore = User::CountAllocCells();
    CAssertFailure* assertFailure = 
        CAssertFailure::NewLC(aMessage,aLineNumber,aFileName);
    User::LeaveIfError(Dll::SetTls(assertFailure));
    CleanupStack::Pop(); // assertFailure
    TInt cellsAfter = User::CountAllocCells();
    assertFailure->SetMyHeapCellCount(cellsAfter-cellsBefore);
    }


CAssertFailure* CTestCase::AssertFailureFromTlsL ()
    {
    CAssertFailure* assertFailure = static_cast<CAssertFailure*>(Dll::Tls());
    CAssertFailure* copy = CAssertFailure::NewL(*assertFailure);
    delete assertFailure;
    Dll::SetTls(0);
    return copy;
    }


TInt CTestCase::HeapCellsReservedByAssertFailure ()
    {
    if (Dll::Tls() == 0) return 0;
    CAssertFailure* assertFailure = static_cast<CAssertFailure*>(Dll::Tls());
    return assertFailure->MyHeapCellCount();
    }
