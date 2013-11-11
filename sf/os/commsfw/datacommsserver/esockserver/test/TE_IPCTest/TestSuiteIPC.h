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
// Contains declaration of CTestSuiteIPC that is 
// container class for all the IPC test steps
// 
//

#if (!defined __TESTSUITEIPC_H__)
#define __TESTSUITEIPC_H__

#include <test/testexecuteserverbase.h>
#include <es_sock.h>

//Some Macros for adding test steps

#define START_TEST_STEP_LIST

#define ADD_TEST_STEP(c) \
	if (aStepName == c::GetTestName()) \
		{ \
		c* ret = new(ELeave) c; \
		ret->SetTestStepName(c::GetTestName()); \
		return ret; \
		}\
else 

#define END_TEST_STEP_LIST \
		{ \
		return NULL; \
		}

//---------------------------------

class  CTestServerIPC : public CTestServer
	{
	public:
		CTestServerIPC();
		~CTestServerIPC();
		static CTestServerIPC* NewL();
		virtual CTestStep* CreateTestStep(const TDesC& aStepName);
		
	private:
	};
#endif /* __TESTSUITEIPC_H__ */

