// Copyright (c) 2003-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef __UNITTEST_H__
#define __UNITTEST_H__

#include <e32std.h>
#include <e32test.h>


#ifdef ACCEPTANCE_TEST
_LIT(KTestName, "AcceptanceTest");
#else
_LIT(KTestName, "UnitTest");
#endif //ACCEPTANCE_TEST
GLREF_D RTest test;

enum TPanic
	{
	EPanicRequestNotWaitedFor
	};

_LIT(KProcName, "LBS-Unit-Test");
#define PANIC(aReason)    User::Panic(KProcName, aReason)

_LIT(KNewLine, "\r\n");
_LIT8(KNewLine8, "\r\n");

_LIT(KLogPath, "C:\\LbsTestLogs\\");
#ifdef ACCEPTANCE_TEST
_LIT(KLogFileName, "ApiAcceptanceTestLog.txt");
#else
_LIT(KLogFileName, "ApiUnitTestLog.txt");
#endif

// EXP_WIDE_STR is needed to turn __FILE__ into a unicode string.
// eg. TPtrC16 ptrToFile(EXP_WIDE_STR(__FILE__))
#define WIDE_STR(XXX)        L ## XXX
#define EXP_WIDE_STR(XXX)    WIDE_STR(XXX)

#ifndef VERBOSE
#define VERBOSE 1
#endif //VERBOSE

#if VERBOSE
#define VERBOSE_OUTPUT(aDes)  test.Printf(aDes)
#define VERBOSE_OUTPUT2(aDes, aParm1)  test.Printf(aDes, aParm1)
#define VERBOSE_OUTPUT3(aDes, aParm1, aParm2)  test.Printf(aDes, aParm1, aParm2)
#define VERBOSE_OUTPUT4(aDes, aParm1, aParm2, aParm3)  test.Printf(aDes, aParm1, aParm2, aParm3)
#else
#define VERBOSE_OUTPUT(aDes)
#define VERBOSE_OUTPUT2(aDes, aParm1)
#define VERBOSE_OUTPUT3(aDes, aParm1, aParm2)
#define VERBOSE_OUTPUT4(aDes, aParm1, aParm2, aParm3)
#endif // VERBOSE

#endif // __UNITTEST_H__
