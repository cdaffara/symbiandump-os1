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
// This contains CTestSuiteEsock 
// This is the container class for all the Esock test steps
// 
//

#if (!defined __TESTSUITESOCK_H__)
#define __TESTSUITESOCK_H__

#include <test/testexecuteserverbase.h>
#include <es_sock.h>

#include <c32comm.h>

#if defined (__WINS__)
#define PDD_NAME _L("ECDRV")
#define LDD_NAME _L("ECOMM")
#else
#define PDD_NAME _L("EUART1")
#define LDD_NAME _L("ECOMM")
#endif

//Some Macros for adding test steps

#define START_TEST_STEP_LIST

#define ADD_TEST_STEP(c) \
	if (aStepName == c::GetTestName()) \
		{ \
		c* ret = new(ELeave) c; \
		ret->iEsockSuite = this; \
		ret->SetTestStepName(c::GetTestName()); \
		return ret; \
		}

#define END_TEST_STEP_LIST \
		return NULL; 

//---------------------------------------------------------


class  CTestSuiteEsock : public CTestServer
    {
	public:
		void InitialiseL( void );
		~CTestSuiteEsock();
		
		//	void AddTestStepL( CTestStepEsock * ptrTestStep );
		TPtrC GetVersion( void );
		
		TInt AddSocketToList(RSocket& aSocket);
		TInt AddSocketToListL(RSocket& aSocket);
		void CloseSocket(TInt aIndex);
		TInt GetSocketListCount();
		RSocket& GetSocketHandle(TInt anIndex);
		
		static CTestSuiteEsock* NewL();
		virtual CTestStep* CreateTestStep(const TDesC& aStepName);
		
		RSocketServ  iSocketServer;
		//CArrayFixFlat<RSocket *> * iSocketList;
		RArray<RSocket> iSocketArray;
		
	private:
		RHeap* iTestSuiteHeap;
    };

// these values have been copied from Testenv.h
const TInt KSmallBufSize = 256;
const TInt KLargeBufSize = 2048;
const TInt KHugeBufSize = 16384;

// ESOCK test macros copied from Testenv.h
typedef TBuf8<KLargeBufSize> TLargeBuf;
#define IPADDR(a,b,c,d) (TUint32)(((a)<<24)|((b)<<16)|((c)<<8)|(d))


#endif /* __TESTSUITESOCK_H__ */


