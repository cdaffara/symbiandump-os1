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



#ifndef __CPPUNIT_TESTRESULT_H
#define __CPPUNIT_TESTRESULT_H

#include <e32base.h>

class MTest;
class CAssertFailure;
class CTestFailure;


/*
 * A CTestResult collects the results of executing a test case. It is an 
 * instance of the Collecting Parameter pattern.
 *
 * The test framework distinguishes between failures and errors.
 * A failure is anticipated and checked for with assertions. Errors are
 * unanticipated problems that are caused by "leaves" that are not generated
 * by the framework.
 *
 * see MTest
 */

class CTestResult : public CBase
	{
public:

	IMPORT_C static CTestResult* NewLC();
	IMPORT_C static CTestResult* NewL();

    IMPORT_C ~CTestResult ();

    IMPORT_C TInt TestCount ();
    IMPORT_C RPointerArray<CTestFailure>& Errors ();
    IMPORT_C RPointerArray<CTestFailure>& Failures ();
    IMPORT_C TBool WasSuccessful ();

	void IncrementTestCount ();
    void AddErrorL (MTest& aTest, TInt aError);
    void AddFailureL (MTest& aTest, CAssertFailure* aAssertFailure);

private:

	void ConstructL ();
	CTestResult ();

    RPointerArray<CTestFailure> iErrors;
    RPointerArray<CTestFailure> iFailures;
	TInt iTestCount; 
	};

#endif


