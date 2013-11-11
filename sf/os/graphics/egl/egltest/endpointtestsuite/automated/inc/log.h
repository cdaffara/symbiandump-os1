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

/**
 @file
 @test
 @internalComponent - Internal Symbian test code
*/

#ifndef __LOG_H__
#define __LOG_H__


//Enumeration used in TRemoteTestResult
//to describe the outcome of the test.
enum TRemoteTestVerdict
    {
    ERtvPass,
    ERtvFail,
    ERtvInconclusive,
    ERtvAbort,
    ERtvPanic,
    ERtvTimeout,
    ERtvUnknownTestUid
    };


//This is the base class for all remote test steps. Derived classes should implement
//DoRemoteTestStepL(), and return the result of the test as a TVerdict.
//Logging is provided with Log(), Logging messages are limitted to 86 chars and
//any longer messages are silently truncated. Log will only work when in the actual
//test step. Use the Logger Macros instead of using Log() directly.
class MLog
    {
public:
    virtual void Log(const TText8* aFile, TInt aLine, TInt aSeverity, TRefByValue<const TDesC> aFmt, ...)=0;
    };

#endif // __LOG_H__
