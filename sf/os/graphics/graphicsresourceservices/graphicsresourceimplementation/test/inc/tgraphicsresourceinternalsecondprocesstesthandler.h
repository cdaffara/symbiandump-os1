// Copyright (c) 2009-2010 Nokia Corporation and/or its subsidiary(-ies).
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
#ifndef TGRAPHICSRESOURCEINTERNALSECONDPROCESSTESTHANDLER_H
#define TGRAPHICSRESOURCEINTERNALSECONDPROCESSTESTHANDLER_H

#include <e32base.h>
#include <sgresource/sgresource.h>
#include <sgresource/sgimage.h>
#include "tgraphicsresourceinternalextensions.h"
#include "tgraphicsresourceinternalsecondprocessenums.h"

/**
Runs individual tests within the second process for the Graphics Resource
internal tests.
 */
class CTSgResInternalSecondProcessTestHandler : public CBase
	{
public:
	static CTSgResInternalSecondProcessTestHandler* NewLC();
	TInt RunTestCaseL(const TSgResIntTestInfo& aInfo);
	void OpenDriverL();

private:
	CTSgResInternalSecondProcessTestHandler();
	~CTSgResInternalSecondProcessTestHandler();

	//Test functions that initialise the second process to run their tests.
	void TestDriverMemoryLeakL();
	void TestDriverShutdownMemoryLeakL();
	TInt TestDrawableOOM();
	TInt TestImageOOM(const TSgResIntTestInfo& aInfo);
	void DoImageMemoryTestsL(const TSgResIntTestInfo& aInfo);
	void DoDrawableMemoryTestsL();
	TInt TestDriverInitializeAndShutdownL();
	TInt TestDriverInitializeAndShutdownManyTimes();
	TInt TestResourceProfiling(const TSgResIntTestInfo& aInfo);
	
	void TestOpenImageL(TSgDrawableId aId);

private:
	RSgDriver iSgDriver;
	MSgDriver_Test* iTestExt;
	MSgDriver_Profiling* iProfExt;
	};

#endif // TGRAPHICSRESOURCEINTERNALSECONDPROCESSTESTHANDLER_H
