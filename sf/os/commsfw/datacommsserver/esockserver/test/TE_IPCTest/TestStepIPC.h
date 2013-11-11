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
// Contains declaration of CTestStepIPC class that is
// base class for all test step classes in this test.
// 
//

#if (!defined __TESTSTEPIPC_H__)
#define __TESTSTEPIPC_H__
#include <in_sock.h>
#include <test/testexecutestepbase.h>


class CTestStepIPC : public CTestStep
	{
	public:
		CTestStepIPC();
		~CTestStepIPC();
		enum TVerdict doTestStepL();
		//	CTestSuiteIPC * iEsockSuite;// Pointer to the suite which owns this test
	protected:
		virtual enum TVerdict InternalDoTestStepL() = 0;
		static TInt OptimalConnect(RSocketServ& aSrv);
		TBufC<12> iProt;	 //Will contain protocol name: "IPC stream"
		
	};

class CIPCTestStep_OOMCapable : public CTestStepIPC
	{
public:
	~CIPCTestStep_OOMCapable()
		{
		iOOMSess.Close();
		}
		
	void SetHeapFailNext(TInt aFailureIndex)
		{
		if(iOOMSess.Handle() == KNullHandle)
			{
			iOOMSess.Connect();
			}
		if(iOOMSess.Handle() != KNullHandle)
			{
			iOOMSess.__DbgFailNext(aFailureIndex - 1);	// want to start with 0th
			}
		}
		
	void ResetHeapFailNext()
		{
		if(iOOMSess.Handle() != KNullHandle)
			{
			iOOMSess.__DbgFailNext(-1);
			}
		}
		
	TBool CheckForHeapFailNext()
		{
		if(iOOMSess.Handle() != KNullHandle)
			{
			// If ESOCK heaps are still in FailNext mode then we try to flag the termination
			// of the loop to TEF by setting the local heap to fail. Because this might get
			// "eaten" inside TEF before it checks we walk a failure point here too
			return iOOMSess.__DbgCheckFailNext();
			}
		return ETrue;
		}
	
	virtual enum TVerdict InternalDoTestStepL() =0;
private:
	RSocketServ iOOMSess;
	};


#endif /* __TESTSTEPIPC_H__ */

