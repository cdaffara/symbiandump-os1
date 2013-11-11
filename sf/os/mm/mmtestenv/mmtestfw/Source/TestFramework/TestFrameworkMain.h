// Copyright (c) 2002-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef __TESTFRAMEWORKMAIN_H__
#define __TESTFRAMEWORKMAIN_H__

#include <e32base.h>
#include "TestFramework.h"


/**
 *
 * Main class for Test Framework
 *
 * @xxxx
 *
 */
class CTestFrameworkMain : public CBase
	{
public:
	static CTestFrameworkMain* NewLC();
	~CTestFrameworkMain();
	void StartTestingL(const TDesC& aCmdLine);
	CLog* LogClient() const;

private:
	CTestFrameworkMain();
	void ConstructL();

	void RunTestScriptL(const TDesC& aCmdLine);
	void UsageL();

private:
	CLog* iLogClient;
	TUint iLogMode;
	CTestUtils* iTestUtils;
	TInt64 iGuardTimer;
	TBuf<64> iTestMatchString;
	};

#endif // __TESTFRAMEWORKMAIN_H__

