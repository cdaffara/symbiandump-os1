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
// Contains declaration of CTestStepSocket class that is
// base class for all test step classes in this test.
// 
//

#if (!defined __TESTSTEPSOCKET_H__)
#define __TESTSTEPSOCKET_H__
#include <in_sock.h>
#include <test/testexecutestepbase.h>


class CTestStepSocket : public CTestStep
	{
	public:
		CTestStepSocket();
		~CTestStepSocket();
		
	struct TSocketThreadArg
		{
		CTestStepSocket* iHandle;
		RSemaphore* iSem;
		TUint iNumSockets;
		};
	
	protected:
			
		const TPtrC& EpocErrorToText(const TInt aErrCode);
		enum TVerdict doTestStepL();
		virtual enum TVerdict InternalDoTestStepL() =0;
		
		// test threads
		static TInt ConnectThread(TAny * anArg);
		static TInt SocketThread(TAny * anArg);
		static TInt BadHandleThread(TAny* anArg);
		static TInt ReadTwiceThread(TAny* anArg);
		static TInt BadDescriptorThread(TAny* anArg);
		
		static TInt OptimalConnect(RSocketServ &aSrv);

		// Helpers for test threads
		static void DoCreateSubsessionsL(CTestStepSocket *aTestStep, TInt aNumSubsess);
		
		static const TUint KNumTestSockets;
		static const TInt KNumExhaustiveSockets;
		static const TInt KNumStretchOpens;
		
	private:
		TPtrC iErrText;
		TBuf<16> iErrBuf;		// size must at least satisfy the number of digits used to represent a TInt
	};
	

class CSocketTestStep_OOMCapable : public CTestStepSocket
	{
public:
	~CSocketTestStep_OOMCapable()
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
		
	TBool CheckForHeapFailNext();
	virtual enum TVerdict InternalDoTestStepL() =0;
private:
	RSocketServ iOOMSess;
	};



#endif /* __TESTSTEPSOCKET_H__ */

