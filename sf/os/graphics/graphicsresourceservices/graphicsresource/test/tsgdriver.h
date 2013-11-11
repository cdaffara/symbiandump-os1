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

class CTSgDriver : public CTSgTestStepBase
	{
public:
	CTSgDriver();
	~CTSgDriver();
private:
	// From CTestStep
	virtual TVerdict doTestStepL();
	
private:
	//positive tests
	void TestInitializationAndShutdown();
	void TestInitializeShutdownManyTimes();	
	
	//negative tests
	void TestShutdownUninitialized();
	
	//panic tests
	void TestPanicResourceCountNoDriverL();
	void TestPanicAllocMarkStartNoDriverL();
	void TestPanicAllocMarkEndNoDriverL();
	void TestPanicSetAllocFailNoDriverL();
	void TestShutdownMemoryLeakL();
	void TestDriverMemoryLeakL();
	};

_LIT(KTSgDriver,"TSgDriver");

#endif /*TSGDRIVER_H*/
