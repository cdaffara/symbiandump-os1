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



#include "TestFrameWork/testSuite.h"
#include <StifTestModule.h>


CTestSuite* CTestSuite::NewLC (const TDesC8& aName)
    {
    CTestSuite* self=new(ELeave) CTestSuite();
    CleanupStack::PushL(self);
    self->ConstructL(aName);
    return self;
    }


CTestSuite* CTestSuite::NewL (const TDesC8& aName)
    {
    CTestSuite* self=NewLC(aName);
    CleanupStack::Pop();
    return self;
    }


void CTestSuite::ConstructL (const TDesC8& aName)
    {
    iName = aName.AllocL();
    }


CTestSuite::~CTestSuite () 
    {
    iTests.ResetAndDestroy();
    delete iName;
    }


void CTestSuite::addTestL (MTest* aTest)
    {
    User::LeaveIfError (iTests.Append(aTest));
    }


// Runs the tests and collects their result in a TestResult.
// Deprecated.
// Version with index should be used instead.

void CTestSuite::ExecuteL (TTestResult& aResult)
    {
    for (TInt i=0; i < iTests.Count(); i++)
        {
        iTests[i]->ExecuteL(aResult);
        }
    }


// Counts the number of test cases that will be run by this test.
TInt CTestSuite::CountTestCases ()
    {
    TInt count = 0;
    for (TInt i=0; i < iTests.Count(); i++)
        {
        count += iTests[i]->CountTestCases ();
        }
    return count;
    }


const TDesC8& CTestSuite::Name ()
    {
    return *iName;
    }

#if 1
void CTestSuite::ExecuteTestL(TTestResult& aResult,
                              TInt aIndex)
    { 
    for (TInt i=0; i< iTests.Count(); i++)
        {
        TInt count = iTests[i]->CountTestCases();
        if ( aIndex > ( count - 1 ) )
            {
            aIndex -= count;
            }
        else
            {
            iTests[i]->ExecuteTestL(aResult, aIndex);
            return ;
            }
        }
    }

const TDesC8& CTestSuite::TestCaseName (TInt aIndex)
    {
    for (TInt i=0; i< iTests.Count(); i++)
        {
        TInt count = iTests[i]->CountTestCases();
        if ( aIndex > ( count - 1 ) )
            {
            aIndex -= count;
            }
        else
            {
            return ( iTests[i]->TestCaseName(aIndex) ) ;
            }
        }
    // It's an error if we reached that point.
    return(KNullDesC8);
    }
#endif
