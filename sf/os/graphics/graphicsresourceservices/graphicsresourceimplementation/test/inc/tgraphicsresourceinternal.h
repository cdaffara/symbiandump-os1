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
@internalTechnology
 */

#ifndef TGRAPHICSRESOURCEINTERNAL_H
#define TGRAPHICSRESOURCEINTERNAL_H

#include "tgraphicsresourceinternalteststepbase.h"

/**
Test class to run memory leak, out of memory and resource count tests
using the internal SgResource extensions MSgDriver_Test and MSgDriver_Profiling.
*/
class CTGraphicsResourceInternal : public CTGraphicsResourceInternalBase
	{
public:
	CTGraphicsResourceInternal();
	~CTGraphicsResourceInternal();

private:
	// From CTestStep
	virtual TVerdict doTestStepL();

	// Test Cases
	void TestDriverMemoryLeakL();
	void TestOOML();
	void TestInitializeAndShutdownL();
	void TestInitializeAndShutdownManyTimesL();
	void TestResourceProfilingL();
	void TestUsageBitsL();

	};

_LIT(KTGraphicsResourceInternalStep,"TGraphicsResourceInternal");

#endif // TGRAPHICSRESOURCEINTERNAL_H
