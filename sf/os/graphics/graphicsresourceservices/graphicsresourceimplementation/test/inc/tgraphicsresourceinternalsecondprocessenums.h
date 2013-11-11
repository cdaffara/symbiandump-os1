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
#ifndef TGRAPHICSRESOURCEINTERNALSECONDPROCESSENUMS_H
#define TGRAPHICSRESOURCEINTERNALSECONDPROCESSENUMS_H

#include <e32base.h>
#include <sgresource/sgresource.h>

//The name of the executable being used to run the second process tests
_LIT(KInternalTestsSecondProcess, "tgraphicsresourceinternalsecondprocess2.exe");

//The expected panic type for Memory leaks within Graphics Resource
_LIT(KSgAllocPanic, "SGALLOC:????????");

/**
Enum representing the current test case. Passed to the second process via TSgResInTestInfo.
*/
enum TSgResInternalTestCase
	{
	ESgResIntDriverMemoryLeak,
	ESgResIntDrawableOOM,
	ESgResIntImageOOM,
	ESgResIntInitializeAndShutdown,
	ESgResIntInitializeAndShutdownManyTimes,
	ESgResIntResourceProfiling,
	};

/**
Container to send the current test case to the second process.
 */
struct TSgResIntTestInfo
	{
	TSgResInternalTestCase iTestCase;
	TInt iGlobalGraphicsMemory;
	TInt iGlobalResourceCount;
	TSgDrawableId iDrawableId;
	};

#endif /* TGRAPHICSRESOURCEINTERNALSECONDPROCESSENUMS_H */
