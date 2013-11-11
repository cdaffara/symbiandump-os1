/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Declaration of test tool constants
*
*/


#ifndef __TFTYPES_H__
#define __TFTYPES_H__

#include <e32base.h>

/**
* Test framework test case types
*/
enum TTFATestType
    {
    ETFATestTypeCase        = 1,
    ETFATestTypeSuite       = 2,
    ETFTestTypeStubWrapper  = 3,
    ETFTestTypeStubRepeater = 4
    };

/**
* Test framework panic codes
*/
enum TTFPanic
    {
    ETFPanicInvalidTestName         = 1,
    ETFPanicInvalidTestCaseNumber   = 2,
    ETFPanicTlsExists               = 3,
    ETFPanicTlsMissing              = 4,
    ETFPanicFactory                 = 5,
    ETFPanicDuplicateCase           = 6,
    ETFPanicStubMissing             = 7,
    ETFPanicDuplicateStub           = 8,
    ETFPanicComponentMissing        = 9,
    ETFPanicInvalidStubType         = 10
    };

/**
* Test case flags
*/
enum TTFTestCaseFlags
    {
    ETFMayLeakMemory                = 1,
    ETFMayLeakRequests              = 2,
    ETFMayLeakHandles               = 4
    };

/**
* Panic category
*/    
_LIT( KTFName, "Test Framework" );

/**
* Maximum test case name length
*/
const TInt KTFAMaxTestNameLength        = 128;

/**
* Test case error codes
*/
const TInt KTFErrIgnore                 = -10000;
const TInt KTFErrTestNotActive          = -10001;
const TInt KTFErrNoTestCase             = -10002;
const TInt KTFErrNoStub                 = -10003;

#endif
