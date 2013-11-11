/*
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
* @file
* 
*
*/



#ifndef __TEF_LOG_EXTENSIONS__
#define __TEF_LOG_EXTENSIONS__

#include <test/tefunit.h>

/*
This file contains extensions to the TEF macros used for logging, defined in
tefunit.h.
*/

/*
Wrapper to check for an error, then print it and leave with it if not KErrNone.
do ... while (0) ensures the macro can be treated like a single statement. A
macro is used to get the line number information.
Note that p1 will be evaluated once and only once.
*/
#define PRINT_ON_ERROR2_L(test, p1, p2)	\
	do \
		{ \
		TInt result = (test); \
		if (result != KErrNone) \
			{ \
			Logger().LogExtra(((TText8*)__FILE__), __LINE__, ESevrErr, (p1), (p2)); \
			User::Leave(result); \
			} \
		} \
	while (0)

#define PRINT_ON_ERROR3_L(test, p1, p2, p3)	\
	do \
		{ \
		TInt result = (test); \
		if (result != KErrNone) \
			{ \
			Logger().LogExtra(((TText8*)__FILE__), __LINE__, ESevrErr, (p1), (p2), (p3)); \
			User::Leave(result); \
			} \
		} \
	while (0)

/*
Modified version of ASSERT_EQUALS() that will log the expression tested, the
expected result and the actual one.
*/
#define ASSERT_CONDITION(c)	_LIT(KExpression, c);

#define ASSERT_EQUALS_X(aExpression, aExpected) \
	do \
		{ \
		TInt result = (TInt)(aExpression); \
		TInt expected = (TInt)(aExpected); \
		if (result != expected) \
			{ \
			ASSERT_CONDITION(#aExpression); \
			Logger().LogExtra(((TText8*)__FILE__), __LINE__, ESevrErr, _L("%S == %d, not %d"), &KExpression(), result, expected ); \
			User::Leave(KErrTEFUnitFail); \
			} \
		} \
	while (0)

#define ASSERT_NOT_EQUALS_X(aExpression, aUnexpected) \
	do \
		{ \
		TInt result = (TInt)(aExpression); \
		TInt unexpected = (TInt)(aUnexpected); \
		if (result == unexpected) \
			{ \
			ASSERT_CONDITION(#aExpression); \
			Logger().LogExtra(((TText8*)__FILE__), __LINE__, ESevrErr, _L("%S == %d, unexpectedly"), &KExpression(), result ); \
			User::Leave(KErrTEFUnitFail); \
			} \
		} \
	while (0)

#define ASSERT_TRUE_X(aExpression) \
	do \
		{ \
		TBool result = (TBool)(aExpression); \
		if (!result) \
			{ \
			ASSERT_CONDITION(#aExpression); \
			Logger().LogExtra(((TText8*)__FILE__), __LINE__, ESevrErr, _L("%S false"), &KExpression() ); \
			User::Leave(KErrTEFUnitFail); \
			} \
		} \
	while (0)

#endif // __TEF_LOG_EXTENSIONS__

