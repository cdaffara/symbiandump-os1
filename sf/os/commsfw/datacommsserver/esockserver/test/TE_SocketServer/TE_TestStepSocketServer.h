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
//

#if (!defined __TESTSTEPSOCKET_H__)
#define __TESTSTEPSOCKET_H__
#include <in_sock.h>
#include <e32property.h>
#include <test/testexecutestepbase.h>  

const TInt KLargeNumSockets = 60;


class CTestStepSocketServer : public CTestStep
	{
	public:
		//	virtual TDesC GetTestName() = 0;
		CTestStepSocketServer();
		~CTestStepSocketServer();
		
		//	CTestSuiteSocket * iEsockSuite;// Pointer to the suite which owns this test
		enum TVerdict doTestStepPreambleL( void );
		enum TVerdict doTestStepL( void );
		enum TVerdict doTestStepPostambleL( void );
		
	protected:
		virtual enum TVerdict InternalDoTestStepL( void ) = 0;
		
		const TPtrC& EpocErrorToText(const TInt aErrCode);
		
		
		static TInt OptimalConnect(RSocketServ &aSrv);
		static TInt IllegalOptimalConnect(RSocketServ &aSrv);
		static TInt PanicProtocolModule(RSocket &aSock);
		static void NonCompletingIoclt(RSocket &aSock, TRequestStatus &aStat);
		static TInt PanicDealer(RSocketServ &aSockServ);
		static TInt BlockHardOnClose(RSocket &aSock);
		static void SlowIoctlReturn(RSocket &aSock, TRequestStatus &aStat, TBool aBlockPermanently);
		TInt CheckForModuleL(const TDesC8 &aModuleName);
		TInt OpenSocketProtocolStarted();
		void OpenMultipleSocketsL(RSocketServ &aSS,RSocket *aSockets,TInt aNumSockets);
		
	private:
		TPtrC iErrText;
		TBuf<16> iErrBuff;		// size must at least satisfy the number of digits used to represent a TInt
		RProperty iHangingModuleFlag; 
		//This use as a kind of semaphore describing the number of prt that have been put
		//into a hanging state and need to be unlocked before they can unload and the next
		//test can begin
	};

class CSocketServerTestStep_OOMCapable : public CTestStepSocketServer
	{
public:
	~CSocketServerTestStep_OOMCapable()
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
#endif /* __TESTSTEPSOCKET_H__ */

