// Copyright (c) 2004-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// Test server declaration for packet loopback test steps
// 
//

/**
 @file
*/

#if (!defined __LOOPBACK_TEST_SERVER_H__)
#define __LOOPBACK_TEST_SERVER_H__

#include <test/testexecuteserverbase.h>

/**
 Implementation of CTestServer for loopback tests
*/
class CLoopbackTestServer : public CTestServer
	{
public:
	static CLoopbackTestServer* NewL();
	virtual CTestStep* CreateTestStep(const TDesC& aStepName);
private:
	};

#endif // __LOOPBACK_TEST_SERVER__

