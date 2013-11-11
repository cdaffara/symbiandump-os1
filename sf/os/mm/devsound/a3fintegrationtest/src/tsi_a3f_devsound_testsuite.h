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
// This file contains the test suite class declaration for A3F DevSound Integration tests.
// 
//

#include "tsi_a3f_devsound_teststep.h"

#ifndef TSI_A3F_DEVSOUND_TESTSUITE_H
#define TSI_A3F_DEVSOUND_TESTSUITE_H

/*
 *
 * CTestSuiteA3FDevSound
 * - Test suite class
 *
 */
class CTestSuiteA3FDevSound : public CTestSuite
	{
public:
	virtual ~CTestSuiteA3FDevSound();

	// From CTestSuite
	// Creates all the test steps and stores them inside CTestSuiteA3FDevSound
	void InitialiseL();

	// From CTestSuite, gets test suite version
	TPtrC GetVersion() const;

	static CTestSuiteA3FDevSound* NewL();
	IMPORT_C static CTestSuiteA3FDevSound* NewTestSuiteL();

private:
	CTestSuiteA3FDevSound();

	};

#endif // TSI_A3F_DEVSOUND_TESTSUITE_H
