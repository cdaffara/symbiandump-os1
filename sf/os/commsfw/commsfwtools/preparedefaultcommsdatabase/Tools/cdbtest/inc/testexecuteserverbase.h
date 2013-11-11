/**
* Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description:
* This contains CTestServer. This is a replacement for the file defined by TestExecute 
* and is intended for use by the testcommsdat test harness. This is used in an environment 
* that will not be using the TestExecute framework.
* 
*
*/



/**
 @file testexecuteserverbase.h
*/
#ifndef TESTEXECUTESERVERBASE_H
#define TESTEXECUTESERVERBASE_H

#include <e32base.h>
#include "testexecutestepbase.h"

class CTestServer : public CBase
/**
@publishedPartner
@test
*/
	{
public:
	IMPORT_C virtual ~CTestServer();
	IMPORT_C virtual void ConstructL(const TDesC& aName);
	
	/**
	 * Creates the test step.
	 *
	 * @param aStepName - test step name to run.
	 *
	 * @return - A CTestStep derived instance
	 */
	virtual CTestStep* CreateTestStep(const TDesC& aStepName) = 0;

	};

#endif
