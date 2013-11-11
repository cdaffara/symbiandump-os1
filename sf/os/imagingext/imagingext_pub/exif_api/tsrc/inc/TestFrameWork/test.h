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



#ifndef __CPPUNIT_MTEST_H
#define __CPPUNIT_MTEST_H

#include <e32base.h>
#include <StifTestModule.h>
class CCppUnitLog;


// A MTest can be run and collect its results. See CTestResult.
//
class MTest
	{
public:

	virtual ~MTest() { }

    virtual void ExecuteL (TTestResult& aResult) = 0;
    
	virtual TInt CountTestCases () = 0;
    
	virtual const TDesC8& Name () = 0;

    // Same functions with an Index.
    virtual void ExecuteTestL(TTestResult& aResult,
                      TInt aIndex) = 0;

    virtual const TDesC8& TestCaseName (TInt aIndex) = 0;
	};

// All the polymorphic DLLs containing tests should use the following UID:
//
const TInt KCppUnitTestDllUidValue=0x101F5380;
const TUid KCppUnitTestDllUid={KCppUnitTestDllUidValue};

#endif

