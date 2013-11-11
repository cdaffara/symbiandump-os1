// Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// This file contains interface declerations for reading configuration data
// by test components exposing the test-step's protected interface
// 
//

/**
 @file
 @test
 @internalComponent - Internal Symbian test code 
*/
#ifndef TEST_STEP_CONF_H_
#define TEST_STEP_CONF_H_

#include <test/testexecutestepbase.h>


/**
 A configuration context reader interface, required to cleanly expose the test-step
 configuration data to external readers
 */
class MTestStepConfigurationContext
	{
public:
	virtual TBool GetInt(const TDesC& aName, TInt& aData) const = 0;
	virtual TBool GetBool(const TDesC& aName, TBool& aData) const = 0;
	virtual TBool GetString(const TDesC& aName, TPtrC& aData) const = 0;
	virtual TBool HasInt(const TDesC& aName) const = 0;
	virtual TBool HasBool(const TDesC& aName) const = 0;
	virtual TBool HasString(const TDesC& aName) const = 0;
	};

/**
A TestStep factory for creating configuration contexts
*/
class MTestStepConfigurationContextFactory
	{
public:
	virtual MTestStepConfigurationContext* GetConfigurationContextLC(const TDesC& aContextName) = 0;
	};


#endif /*TEST_STEP_CONF_H_*/
