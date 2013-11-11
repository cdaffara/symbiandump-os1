/**
* Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:
*
*/



/**
 @file lcfsbucommondefinitions.h
 @internalTechnology
*/
#ifndef _LCF_SBU_COMMON_DEFINITIONS_H_
#define _LCF_SBU_COMMON_DEFINITIONS_H_


///////////////////////////////////////////////////////
//
// Constants
//
///////////////////////////////////////////////////////


#define DUMMY_REQUESTOR_TYPE_ACCEPTED1        (CRequestor::ERequestorService)
#define DUMMY_REQUESTOR_FORMAT_ACCEPTED1      (CRequestor::EFormatApplication)
#define DUMMY_REQUESTOR_NAME_ACCEPTED1        (_L("TestService"))

#define DUMMY_REQUESTOR_TYPE_ACCEPTED2        (CRequestor::ERequestorContact)
#define DUMMY_REQUESTOR_FORMAT_ACCEPTED2      (CRequestor::EFormatTelephone)
#define DUMMY_REQUESTOR_NAME_ACCEPTED2        (_L("+123121234567"))



const TInt KDefaultTestTimeout = 5*1000*1000;        //5 seconds should be more than enough.

//0.00001;
#define KErrorToleranceValue KMinTReal64    

_LIT(KNoClientFault, "Kill");


///////////////////////////////////////////////////////
//
// Other text
//
///////////////////////////////////////////////////////

// Indicate not yet implemented
_LIT(KNotImplemented, "Not yet implemented");

_LIT(KBadDiagVerifEventNum, "Wrong number of Verification events in Notifier");
_LIT(KBadDiagNotifEventNum, "Wrong number of Notification events in Notifier");

#define CHECK_EQUAL_(_expected, _actual) \
    ERROR(_expected != _actual, "Equal check failed")

#define CHECK(_condition) ERROR(!(_condition), "Check failed")


#define CHECK_REAL_EQUAL_TOL(aExpected, aActual, aTolerance) \
    FAIL(Math::IsNaN(aActual) || Math::IsNaN(aExpected), "One or two of compared values is NaN!") \
    CHECK(Abs(aActual - aExpected) < aTolerance)

#define CHECK_REAL_EQUAL(aExpected, aActual) \
    CHECK_REAL_EQUAL_TOL(aExpected, aActual, KErrorToleranceValue)

#define CHECK_PANIC(aCondition) \
    if (!(aCondition)) CheckPanic(__FILE__, __LINE__);

#define DO_PANIC_TEST_L(aThreadFunction, aExpectedExitCat, aExpectedExitReason, aTimeoutValue) \
    DO_PANIC_TEST_L_PTR(aThreadFunction, aExpectedExitCat, aExpectedExitReason, aTimeoutValue, NULL)

#define DO_PANIC_TEST_L_PTR(aThreadFunction, aExpectedExitCat, aExpectedExitReason, \
    aTimeoutValue, aAnyPtr) \
    { \
    TExitCategoryName exitName = TPtrC(aExpectedExitCat).Left(KMaxExitCategoryName); \
    DoPanicTestL(aThreadFunction, exitName, aExpectedExitReason, aTimeoutValue, aAnyPtr); \
    }

///////////////////////////////////////////////////////
//
// Here are macros for error logging 
// Those imply that are called from within of 
// TEF class's function (they use (INFO/ERR)PRINTF member)
//
///////////////////////////////////////////////////////

// LEAVES with _error_ if _condition_ is true
#define FAIL(condition, error)             \
    if (condition) {                        \
        errStr.Format(_L("%s (line %d)"), _S(error), __LINE__);    \
        ERR_PRINTF1(errStr);				\
		SetTestStepResult(EFail);           \
    }


// Must be included in the begining of function definitions
// in order to use following ("ERROR") macros
// (The string buffer needs to be defined once, otherwise,
// in case when a lot of errors are checked, stack may be
// overflown, and copilation will fail)
#define DECLARE_ERROR_STR                   TBuf<160> errStr;
#define DECLARE_MESSAGE_STR                 TBuf<160> message;

#define DECLARE_ERROR_LOGGING               DECLARE_ERROR_STR   DECLARE_MESSAGE_STR

#define START_ERROR_LOGGING                 iErrorCounter = 0;

// Logs a message, which is defined as "..." constant
#define LOG(msg)                            \
    {                                       \
        errStr.Format(_L("%s (line %d)"), _S(msg), __LINE__);    \
        INFO_PRINTF1(errStr);                  \
    }
#define LOG_DES(msg_des)                    \
    {                                       \
        errStr.Format(_L("%S (line %d)"), &msg_des, __LINE__);    \
        INFO_PRINTF1(errStr);                  \
    }

#define LOG_IIF(condition, msg1, msg2)      \
    if (condition) LOG(msg1) else LOG(msg2)

// Logs an error, which is defined as "..." constant
#define ERR(error)                          \
    {                                       \
        errStr.Format(_L("%s (line %d)"), _S(error), __LINE__);    \
        ERR_PRINTF1(errStr);             \
        SetTestStepResult(EFail);			\
    }
#define ERR_DES(error)                      \
    {                                       \
        errStr.Format(_L("%S (line %d)"), &error, __LINE__);    \
        ERR_PRINTF1(errStr);             \
        SetTestStepResult(EFail);			\
    }

// Logs an _error_ if _condition_ is true
#define ERROR(condition, error)             \
    if (condition) ERR(error)

// Logs an error, which defined as descriptor
#define ERROR_DES(condition, error_des)     \
    if (condition) ERR_DES(error_des)

#define ERROR_1(condition, msg_fmt_des, param1)  \
    message.Format(msg_fmt_des, param1);        \
    ERROR_DES(condition, message);

#define ERROR_2(condition, msg_fmt_des, param1, param2)  \
    message.Format(msg_fmt_des, param1, param2);        \
    ERROR_DES(condition, message);

#define ERROR_3(condition, msg_fmt_des, param1, param2, param3) \
    message.Format(msg_fmt_des, param1, param2, param3);        \
    ERROR_DES(condition, message);

#define IF_ERROR(error_code, msg)                               \
    { TInt32 _err = error_code;                                 \
    ERROR_2(_err != KErrNone, _L("%s (error=%d)"), _S(msg), _err); }

#define IF_ERROR_DES(error_code, msg_des)                       \
    { TInt32 _err = error_code;                                 \
    ERROR_2(_err != KErrNone, _L("%S (error=%d)"), &msg_des, _err); }

#define CHECK_EQUAL(_actual, _expected, _msg)                     \
    ERROR_3(_actual != _expected, _L("%s (actual=%d, expected=%d)"), _S(_msg), _actual, _expected);

#define CHECK_EQUAL_DES(_actual, _expected, _msg_des)           \
    ERROR_3(_actual != _expected, _L("%S (actual=%d, expected=%d)"), &_msg_des, _actual, _expected);


#endif // _LCF_SBU_COMMON_DEFINITIONS_H_
