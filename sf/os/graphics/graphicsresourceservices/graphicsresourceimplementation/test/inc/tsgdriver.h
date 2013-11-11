// Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef TSGDRIVER_H
#define TSGDRIVER_H

#include "tgraphicsresourceteststepbase.h"

/**
Class containing all the positive, negative and panic tests for RSgDriver.

KErrNoMemory testing can optionally be enabled.
 */
class CTSgDriver : public CTSgTestStepBase
	{
public:
	CTSgDriver(TBool aConformanceTests);
	~CTSgDriver();
private:
	// From CTestStep
	virtual TVerdict doTestStepL();
	void TestVersion();
	//negative tests
	void TestShutdownUninitialized();
	void TestMultipleOpens();
	void TestGetInterface();
	};

_LIT(KTSgDriver,"TSgDriver");

#endif /*TSGDRIVER_H*/
