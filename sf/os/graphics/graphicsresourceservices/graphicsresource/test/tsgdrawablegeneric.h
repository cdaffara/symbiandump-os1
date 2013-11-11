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

#ifndef TSGDRAWABLEGENERIC_H
#define TSGDRAWABLEGENERIC_H

#include "tgraphicsresourceteststepbase.h"

class CTSgDrawableGeneric : public CTSgTestStepBase
	{
public:
	CTSgDrawableGeneric();
	~CTSgDrawableGeneric();
private:
	// From CTestStep
	virtual TVerdict doTestStepL();

	//positive tests
	void TestOpenImageAsDrawableL();	
	void TestGetDrawableDrawableIdL();	
	//negative tests
	void TestOpenDrawableInvalidL();
	void TestCloseDrawableWithoutOpenL();	
	//out of memory tests
	void DoMemoryTestsL();
	
	//panic tests
	void TestPanicDrawableOpenNoDriverL();
	};

_LIT(KTSgDrawableGeneric,"TSgDrawableGeneric");
	
#endif /*TSGDRAWABLEGENERIC_H*/
