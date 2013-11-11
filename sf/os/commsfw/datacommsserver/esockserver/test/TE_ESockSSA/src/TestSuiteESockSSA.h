// Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// Contains declaration of CTestServerESockSSA that is 
// container class for all the Socket test steps
// 
//

#if (!defined __TESTSUITESOCKET_H__)
#define __TESTSUITESOCKET_H__

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

class  CTestServerESockSSA : public CTestServer
	{
	public:
		CTestServerESockSSA();
		~CTestServerESockSSA();
		static CTestServerESockSSA* NewL();
		virtual CTestStep* CreateTestStep(const TDesC& aStepName);
		
		//	TPtrC GetVersion( void );
		
		RSocketServ iSocketServer;
		
	private:
	};

#endif /* __TESTSUITESOCKET_H__ */

