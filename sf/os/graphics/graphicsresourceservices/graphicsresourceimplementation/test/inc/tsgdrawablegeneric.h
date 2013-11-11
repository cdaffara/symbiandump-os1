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
/**
@file
@test
@internalComponent
 */
#ifndef TSGDRAWABLEGENERIC_H
#define TSGDRAWABLEGENERIC_H

#include "tgraphicsresourceteststepbase.h"

/**
Class containing all the positive, negative and panic tests for RSgDrawable.

KErrNoMemory testing can optionally be enabled.
 */
class CTSgDrawableGeneric : public CTSgTestStepBase
	{
public:
	CTSgDrawableGeneric(TBool aConformanceTests);
	~CTSgDrawableGeneric();

	//positive tests
	void TestOpenImageAsDrawableL();	
	void TestGetDrawableDrawableIdL();	
	//negative tests
	void TestOpenDrawableInvalidL();
	void TestCloseDrawableWithoutOpenL();	

private:
	// From CTestStep
	virtual TVerdict doTestStepL();
	
	//panic tests
	void TestPanicDrawableOpenNoDriverL();
	};

_LIT(KTSgDrawableGeneric,"TSgDrawableGeneric");
	
#endif /*TSGDRAWABLEGENERIC_H*/
