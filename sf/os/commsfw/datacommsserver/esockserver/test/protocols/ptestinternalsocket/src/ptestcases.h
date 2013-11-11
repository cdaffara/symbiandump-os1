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
//

#if !defined(PTESTCASES_H)
#define PTESTCASES_H

#include <e32base.h>
#include <test/testexecutestepbase.h>
#include "proxyprotocol.h"
#include <comms-infras/eintsock.h>
#include <es_mbuf.h>
#include "agregate.h"

NONSHARABLE_CLASS(CEIntSockTestBase) : protected CActive, public MTestStepNotify
	{
	public:
		static void StartTestL(const TDesC& aTestName, CProxyProvd* aProvd);
		virtual void RunL();
		virtual void AboutToShutdown();
	
	protected:		
		CEIntSockTestBase(CProxyProvd* aProvd);
		virtual ~CEIntSockTestBase();
		
		//Utilities function to be use in RunTest()
		void Reschedule();
		CTestExecuteLogger iLogger;
						
	private:
		//Implemented you own of these		
		virtual TVerdict RunTestL() = 0;
		virtual void Cleanup();
		
		void DoCancel();
		void ConstructL();
		void SetLoggerL();
		CTestExecuteLogger& Logger();
		void FinishTest(TVerdict result);
		static CEIntSockTestBase* CreateTestL(const TDesC& aTestName, CProxyProvd* aProvd);
		
	private:
		CProxyProvd* iProvd;
		TBool iReschedule; //Determines whether rescheduling is required
	};
	
inline CTestExecuteLogger& CEIntSockTestBase::Logger()
{
	return iLogger;
}


NONSHARABLE_CLASS(CEIntSockTest1) : public CEIntSockTestBase
	{
	public:
		static CEIntSockTestBase* NewL(CProxyProvd* aProvd);
		
	private:
		CEIntSockTest1(CProxyProvd* aProvd);
		virtual TVerdict RunTestL();
		void Cleanup();
		RInternalSocket iSocket;
	};
	
NONSHARABLE_CLASS(CEIntSockTest2) : public CEIntSockTestBase
	{
	public:
		static CEIntSockTestBase* NewL(CProxyProvd* aProvd);
		
	private:
		CEIntSockTest2(CProxyProvd* aProvd);
		virtual TVerdict RunTestL();
		void Cleanup();
		RInternalSocket iSocket;
	};
	
NONSHARABLE_CLASS(CEIntSockTest3) : public CEIntSockTestBase
	{
	public:
		static CEIntSockTestBase* NewL(CProxyProvd* aProvd);
		
	private:
		CEIntSockTest3(CProxyProvd* aProvd);
		virtual TVerdict RunTestL();
		void Cleanup();
		RInternalSocket iSocket;
	};
	
NONSHARABLE_CLASS(CEIntSockTest4) : public CEIntSockTestBase
	{
	public:
		static CEIntSockTestBase* NewL(CProxyProvd* aProvd);
		
	private:
		CEIntSockTest4(CProxyProvd* aProvd);
		virtual TVerdict RunTestL();
		void Cleanup();
		RInternalSocket iSocket;
	};
	
NONSHARABLE_CLASS(CEIntSockTest5) : public CEIntSockTestBase
	{
	public:
		static CEIntSockTestBase* NewL(CProxyProvd* aProvd);
		
	private:
		enum TState {EState1,EState2,EState3,EState4};
		CEIntSockTest5(CProxyProvd* aProvd);
		virtual TVerdict RunTestL();
		void Cleanup();
		RInternalSocket iSocket;
		TState iState;
		TBuf8<20> iBuf;
		TInetAddr iAddress;
		TPtr8 iRecvBufPtr;
	};
	
NONSHARABLE_CLASS(CEIntSockTest6) : public CEIntSockTestBase
	{
	public:
		static CEIntSockTestBase* NewL(CProxyProvd* aProvd);
		
	private:
		enum TState {EState1,EState2,EState3,EState4};
		CEIntSockTest6(CProxyProvd* aProvd);
		virtual TVerdict RunTestL();
		void Cleanup();
		RInternalSocket iSocket;
		TState iState;
		TBuf8<20> iBuf;
		TInetAddr iAddress;
	};
	
NONSHARABLE_CLASS(CEIntSockTest7) : public CEIntSockTestBase
	{
	public:
		static CEIntSockTestBase* NewL(CProxyProvd* aProvd);
		
	private:
		enum TState {EState1,EState2,EState3,EState4};
		CEIntSockTest7(CProxyProvd* aProvd);
		virtual TVerdict RunTestL();
		void Cleanup();
		RInternalSocket iSocket;
		TState iState;
		TBuf8<20> iBuf;
		RMBufChain iSendChain;
		RMBufChain iRecvChain;
		TInetAddr iAddress;
	};
	
NONSHARABLE_CLASS(CEIntSockTest8) : public CEIntSockTestBase
	{
	public:
		static CEIntSockTestBase* NewL(CProxyProvd* aProvd);
		
	private:
		enum TState {EState1,EState2,EState3,EState4};
		CEIntSockTest8(CProxyProvd* aProvd);
		virtual TVerdict RunTestL();
		void Cleanup();
		RInternalSocket iSocket;
		TState iState;
		TBuf8<20> iBuf;
		RMBufChain iSendChain;
		RMBufChain iRecvChain;
		TInetAddr iAddress;		
	};
	
NONSHARABLE_CLASS(CEIntSockTest9) : public CEIntSockTestBase
	{
	public:
		static CEIntSockTestBase* NewL(CProxyProvd* aProvd);
		
	private:
		CEIntSockTest9(CProxyProvd* aProvd);
		virtual TVerdict RunTestL();
		void Cleanup();
		RInternalSocket iSocket;
	};
	
NONSHARABLE_CLASS(CEIntSockTest10) : public CEIntSockTestBase
	{
	public:
		static CEIntSockTestBase* NewL(CProxyProvd* aProvd);
		
	private:
		enum TState {EState1,EState2};
		CEIntSockTest10(CProxyProvd* aProvd);
		virtual TVerdict RunTestL();
		void Cleanup();
		RInternalSocket iSocket;
		TState iState;
	};
	
NONSHARABLE_CLASS(CEIntSockTest11) : public CEIntSockTestBase
	{
	public:
		static CEIntSockTestBase* NewL(CProxyProvd* aProvd);
		
	private:
		enum TState {EState1,EState2,EState3};
		CEIntSockTest11(CProxyProvd* aProvd);
		virtual TVerdict RunTestL();
		void Cleanup();
		RInternalSocket iSocket;
		TState iState;
	};		
	
NONSHARABLE_CLASS(CEIntSockTest12) : public CEIntSockTestBase
	{
	public:
		static CEIntSockTestBase* NewL(CProxyProvd* aProvd);
		
	private:
		enum TState {EState1,EState2,EState3};
		CEIntSockTest12(CProxyProvd* aProvd);
		virtual TVerdict RunTestL();
		void Cleanup();
		RInternalSocket iSocket;
		RInternalSocket iNewSock;
		TState iState;
		TInetAddr iAddress;
	};	
	
NONSHARABLE_CLASS(CEIntSockTest13) : public CEIntSockTestBase
	{
	public:
		static CEIntSockTestBase* NewL(CProxyProvd* aProvd);
		
	private:
		enum TState {EState1,EState2,EState3};
		CEIntSockTest13(CProxyProvd* aProvd);
		virtual TVerdict RunTestL();
		void Cleanup();
		RInternalSocket iSocket;
		TState iState;
		TInetAddr iAddress;
	};
	
NONSHARABLE_CLASS(CEIntSockTest14) : public CEIntSockTestBase
	{
	public:
		static CEIntSockTestBase* NewL(CProxyProvd* aProvd);
		~CEIntSockTest14();
		
	private:
		enum TState {EState1,EState2,EState3,EState4,EState5,EState6};
		CEIntSockTest14(CProxyProvd* aProvd);
		void ConstructL();
		virtual TVerdict RunTestL();
		void Cleanup();
		RInternalSocket iSocket;
		RInternalSocket iNewSock;
		TBuf8<20> iBuf;
		CAggregateCollection* iColl;
		CAggregateItem* iSendItem;
		CAggregateItem* iIoctlItem;
		CAggregateItem* iConnectItem;		
		TState iState;
		TInetAddr		iAddress;
	};		
	
NONSHARABLE_CLASS(CEIntSockTest15) : public CEIntSockTestBase
	{
	public:
		static CEIntSockTestBase* NewL(CProxyProvd* aProvd);
		
	private:
		enum TState {EState1,EState2,EState3};
		CEIntSockTest15(CProxyProvd* aProvd);
		virtual TVerdict RunTestL();
		void Cleanup();
		RInternalSocket iSocket;
		TBuf8<20> iBuf;
		TState iState;
		TInetAddr iAddress;
	};
	
NONSHARABLE_CLASS(CEIntSockTest16) : public CEIntSockTestBase
	{
	public:
		static CEIntSockTestBase* NewL(CProxyProvd* aProvd);
		
	private:
		enum TState {EState1,EState2,EState3};
		CEIntSockTest16(CProxyProvd* aProvd);
		virtual TVerdict RunTestL();
		void Cleanup();
		RInternalSocket iSocket;
		TBuf8<20> iBuf;
		TState iState;
		TInetAddr iAddress;		
	};
	
NONSHARABLE_CLASS(CEIntSockTest17) : public CEIntSockTestBase
	{
	public:
		static CEIntSockTestBase* NewL(CProxyProvd* aProvd);
		
	private:
		enum TState {EState1,EState2,EState3,EState4,EState5};
		CEIntSockTest17(CProxyProvd* aProvd);
		~CEIntSockTest17();
		void ConstructL();
		virtual TVerdict RunTestL();
		void Cleanup();
		RInternalSocket iSocket;
		TBuf8<20> iBuf;
		CAggregateCollection* iColl;
		CAggregateItem* iSendItem;
		CAggregateItem* iRecvItem;
		TState iState;
		TInetAddr iAddress;		
	};

const TInt KTest18NumSockets = 3;
NONSHARABLE_CLASS(CEIntSockTest18) : public CEIntSockTestBase
	{
	public:
		static CEIntSockTestBase* NewL(CProxyProvd* aProvd);
		
	private:
		CEIntSockTest18(CProxyProvd* aProvd);
		virtual TVerdict RunTestL();
		void Cleanup();
		RInternalSocket iSockets[KTest18NumSockets];
	};	
	
const TInt KTest19NumSockets = 3;
NONSHARABLE_CLASS(CEIntSockTest19) : public CEIntSockTestBase
	{
	public:
		static CEIntSockTestBase* NewL(CProxyProvd* aProvd);
		
	private:
		CEIntSockTest19(CProxyProvd* aProvd);
		virtual TVerdict RunTestL();
		void Cleanup();
		RInternalSocket iSockets[KTest19NumSockets];
	};	

const TInt KTest20NumCloses = 3;
NONSHARABLE_CLASS(CEIntSockTest20) : public CEIntSockTestBase
	{
	public:
		static CEIntSockTestBase* NewL(CProxyProvd* aProvd);
		
	private:
		CEIntSockTest20(CProxyProvd* aProvd);
		virtual TVerdict RunTestL();
		void Cleanup();
		RInternalSocket iSocket;
	};
	
NONSHARABLE_CLASS(CEIntSockTest21) : public CEIntSockTestBase
	{
	public:
		static CEIntSockTestBase* NewL(CProxyProvd* aProvd);
		
	private:
		CEIntSockTest21(CProxyProvd* aProvd);
		virtual TVerdict RunTestL();
		void Cleanup();
		RInternalSocket iSocket;
	};
	
NONSHARABLE_CLASS(CEIntSockTest22) : public CEIntSockTestBase
	{
	public:
		static CEIntSockTestBase* NewL(CProxyProvd* aProvd);
		
	private:
		CEIntSockTest22(CProxyProvd* aProvd);
		virtual TVerdict RunTestL();
		void Cleanup();
		RInternalSocket iSocket;
	};
	
NONSHARABLE_CLASS(CEIntSockTest23) : public CEIntSockTestBase
	{
	public:
		static CEIntSockTestBase* NewL(CProxyProvd* aProvd);
		
	private:
		enum TState {EState1,EState2};
		CEIntSockTest23(CProxyProvd* aProvd);
		virtual TVerdict RunTestL();
		void Cleanup();
		RInternalSocket iSocket;
		TState iState;
		TBuf8<20> iOut;
		TBuf8<20> iIn;
		TInetAddr iAddress;
	};

NONSHARABLE_CLASS(CEIntSockTest25A) : public CEIntSockTestBase
	{
	public:
		static CEIntSockTestBase* NewL(CProxyProvd* aProvd);
		
	private:
		~CEIntSockTest25A();
		CEIntSockTest25A(CProxyProvd* aProvd);
		void ConstructL();
		virtual TVerdict RunTestL();
		void Cleanup();

		enum TState {EState1,EState2};
		TState iState;

		// the socket on which the operations in this test will happen
		RInternalSocket iSocket;
		
		CAggregateCollection* iColl;
		CAggregateItem* iConnectItem;
		CAggregateItem* iIoctlItem;
		TInetAddr iAddress;		
	};

NONSHARABLE_CLASS(CEIntSockTest25B) : public CEIntSockTestBase
	{
	public:
		static CEIntSockTestBase* NewL(CProxyProvd* aProvd);
				
	private:
		~CEIntSockTest25B();
		CEIntSockTest25B(CProxyProvd* aProvd);
		void ConstructL();
		virtual TVerdict RunTestL();
		void Cleanup();
		

		enum TState {EState1,EState2};
		TState iState;

		// the socket on which the operations in this test will happen
		RInternalSocket iSocket;
		RInternalSocket iNewSock;
		
		CAggregateCollection* iColl;
		CAggregateItem* iAcceptItem;
		CAggregateItem* iIoctlItem;
	};
	
NONSHARABLE_CLASS(CEIntSockTest25C) : public CEIntSockTestBase
	{
	public:
		static CEIntSockTestBase* NewL(CProxyProvd* aProvd);
		
	private:
		void ConstructL();
		~CEIntSockTest25C();
		CEIntSockTest25C(CProxyProvd* aProvd);
		virtual TVerdict RunTestL();
		void Cleanup();
		
		enum TState {EState1,EState2,EState3};
		TState iState;

		// the socket on which the operations in this test will happen
		RInternalSocket iSocket;
		TBuf8<20> iRecvBuf;

		CAggregateCollection* iColl;
		CAggregateItem* iRecvItem;
		CAggregateItem* iIoctlItem;
		TInetAddr iAddress;		
	};
	
NONSHARABLE_CLASS(CEIntSockTest26) : public CEIntSockTestBase
	{
	public:
		static CEIntSockTestBase* NewL(CProxyProvd* aProvd);
		
	private:
		CEIntSockTest26(CProxyProvd* aProvd);
		virtual TVerdict RunTestL();
		void Cleanup();
		
		enum TState {EState1,EState2,EState3,EState4,EState5};
		TState iState;

		// the socket on which the operations in this test will happen
		RInternalSocket iSocket;
		TBuf8<20> iBuf;
		TInetAddr iAddress;		
	};

NONSHARABLE_CLASS(CEIntSockTest27) : public CEIntSockTestBase
	{
	public:
		static CEIntSockTestBase* NewL(CProxyProvd* aProvd);
		
	private:
		CEIntSockTest27(CProxyProvd* aProvd);
		virtual TVerdict RunTestL();
		void Cleanup();
		
		enum TState {EState1,EState2,EState3,EState4,EState5};
		TState iState;

		// the socket on which the operations in this test will happen
		RInternalSocket iSocket;
		TBuf8<20> iBuf;
		TBuf8<40> iDisconnectBuf;
		
		TInetAddr iAddress;		
	};
	
NONSHARABLE_CLASS(CEIntSockTest28A) : public CEIntSockTestBase
	{
	public:
		static CEIntSockTestBase* NewL(CProxyProvd* aProvd);
		
	private:
		enum TState {EState1,EState2};
		CEIntSockTest28A(CProxyProvd* aProvd);
		virtual TVerdict RunTestL();
		void Cleanup();
		RInternalSocket iSocket;
		TState iState;
		TBuf8<20> iBuf;
		TInetAddr iAddress;		
	};
	
NONSHARABLE_CLASS(CEIntSockTest28B) : public CEIntSockTestBase
	{
	public:
		static CEIntSockTestBase* NewL(CProxyProvd* aProvd);
		
	private:
		enum TState {EState1,EState2,EState3,EState4};
		CEIntSockTest28B(CProxyProvd* aProvd);
		virtual TVerdict RunTestL();
		void Cleanup();
		RInternalSocket iSocket;
		RInternalSocket iNewSock;
		TState iState;
		TBuf8<20> iBuf;
	};

NONSHARABLE_CLASS(CEIntSockTest29) : public CEIntSockTestBase
	{
	public:
		static CEIntSockTestBase* NewL(CProxyProvd* aProvd);
		
	private:
		enum TState {EState1,EState2,EState3,EState4};
		CEIntSockTest29(CProxyProvd* aProvd);
		virtual TVerdict RunTestL();
		void Cleanup();
		RInternalSocket iSocket;
		TState iState;
		TSockXfrLength iVolume;
		TBuf8<20> iBuf;
		TInetAddr iAddress;		
	};

NONSHARABLE_CLASS(CEIntSockTest30) : public CEIntSockTestBase
	{
	public:
		static CEIntSockTestBase* NewL(CProxyProvd* aProvd);
		
	private:
		enum TState {EState1,EState2,EState3,EState4};
		CEIntSockTest30(CProxyProvd* aProvd);
		virtual TVerdict RunTestL();
		void Cleanup();
		RInternalSocket iSocket;
		TState iState;
		TSockXfrLength iVolume;
		TBuf8<20> iBuf;
		TInetAddr iAddress;	
		TPtr8 iRecvBufPtr;	
	};

NONSHARABLE_CLASS(CEIntSockTest31) : public CEIntSockTestBase
	{
	public:
		static CEIntSockTestBase* NewL(CProxyProvd* aProvd);
		
	private:
		enum TState {EState1,EState2,EState3,EState4};
		CEIntSockTest31(CProxyProvd* aProvd);
		virtual TVerdict RunTestL();
		void Cleanup();
		RInternalSocket iSocket;
		TState iState;
		TSockXfrLength iVolume;
		TBuf8<20> iBuf;
		RMBufChain iRecvChain;
		RMBufChain iSendChain;
		TInetAddr iAddress;		
	};

NONSHARABLE_CLASS(CEIntSockTest32) : public CEIntSockTestBase
	{
	public:
		static CEIntSockTestBase* NewL(CProxyProvd* aProvd);
		
	private:
		enum TState {EState1,EState2,EState3,EState4};
		CEIntSockTest32(CProxyProvd* aProvd);
		virtual TVerdict RunTestL();
		void Cleanup();
		RInternalSocket iSocket;
		TState iState;
		TSockXfrLength iVolume;
		TBuf8<20> iBuf;
		RMBufChain iRecvChain;
		RMBufChain iSendChain;	
		TInetAddr iAddress;					
	};

NONSHARABLE_CLASS(CEIntSockTest33) : public CEIntSockTestBase
	{
	public:
		static CEIntSockTestBase* NewL(CProxyProvd* aProvd);
		
	private:
		enum TState {EState1,EState2,EState3,EState4};
		CEIntSockTest33(CProxyProvd* aProvd);
		virtual TVerdict RunTestL();
		void Cleanup();
		RInternalSocket iSocket;
		TState iState;
		TBuf8<20> iBuf;
		TInetAddr iAddress;				
		TPtr8 iReadBufPtr;
	};
	
NONSHARABLE_CLASS(CEIntSockTest34) : public CEIntSockTestBase
	{
	public:
		static CEIntSockTestBase* NewL(CProxyProvd* aProvd);
		
	private:
		enum TState {EState1,EState2,EState3};
		CEIntSockTest34(CProxyProvd* aProvd);
		virtual TVerdict RunTestL();
		void Cleanup();
		RInternalSocket iSocket;
		TState iState;
		TInetAddr iAddress;				
	};
	
NONSHARABLE_CLASS(CEIntSockTest35) : public CEIntSockTestBase
	{
	public:
		static CEIntSockTestBase* NewL(CProxyProvd* aProvd);
		
	private:
		enum TState {EState1, EState2, EState3, EState4 };
		CEIntSockTest35(CProxyProvd* aProvd);
		virtual TVerdict RunTestL();
		void Cleanup();
		RInternalSocket iSocket;
		TState iState;
		static const TInt KExpectedError = -4567; // Something random.
		TInetAddr iAddress;			
	};

NONSHARABLE_CLASS(CEIntSockTest36) : public CEIntSockTestBase
	{
	public:
		static CEIntSockTestBase* NewL(CProxyProvd* aProvd);
		
	private:
		enum TState {EState1, EState2, EState3, EState4};

		CEIntSockTest36(CProxyProvd* aProvd);
		virtual TVerdict RunTestL();
		void Cleanup();

		RInternalSocket iSocket;
		TState iState;
		TBuf8<20> iBuf;
		TInetAddr iAddress;			
	};

NONSHARABLE_CLASS(CEIntSockTest37) : public CEIntSockTestBase
	{
	public:
		static CEIntSockTestBase* NewL(CProxyProvd* aProvd);
		
	private:
		enum TState {EState1, EState2, EState3, EState4};

		CEIntSockTest37(CProxyProvd* aProvd);
		virtual TVerdict RunTestL();
		void Cleanup();

		RInternalSocket iSocket;
		TState iState;
		TBuf8<20> iBuf;
		RMBufChain iSendChain;
		RMBufChain iRecvChain;
		TInetAddr iAddress;			
	};

NONSHARABLE_CLASS(CEIntSockTest38) : public CEIntSockTestBase
	{
	public:
		static CEIntSockTestBase* NewL(CProxyProvd* aProvd);
		
	private:
		enum TState {EState1, EState2, EState3, EState4, EState5};

		CEIntSockTest38(CProxyProvd* aProvd);

		virtual TVerdict RunTestL();
		void Cleanup();

		RInternalSocket iSocket;
		TState iState;
		TBuf8<20> iBuf;
		TInetAddr iAddress;			
	};

NONSHARABLE_CLASS(CEIntSockTest39) : public CEIntSockTestBase
	{
	public:
		static CEIntSockTestBase* NewL(CProxyProvd* aProvd);
		
	private:
		enum TState {EState1, EState2, EState3};

		CEIntSockTest39(CProxyProvd* aProvd);

		virtual TVerdict RunTestL();
		void Cleanup();

		RInternalSocket iSocket;
		TState iState;
	};

NONSHARABLE_CLASS(CEIntSockTest40) : public CEIntSockTestBase
	{
	public:
		static CEIntSockTestBase* NewL(CProxyProvd* aProvd);
		
	private:
		enum TState {EState1, EState2, EState3, EState4};

		CEIntSockTest40(CProxyProvd* aProvd);

		virtual TVerdict RunTestL();
		void Cleanup();

		RInternalSocket iSocket;
		TState iState;
	};

NONSHARABLE_CLASS(CEIntSockTest41) : public CEIntSockTestBase
	{
	public:
		static CEIntSockTestBase* NewL(CProxyProvd* aProvd);
		
	private:
		enum TState {EState1, EState2, EState3};

		CEIntSockTest41(CProxyProvd* aProvd);

		virtual TVerdict RunTestL();
		void Cleanup();
		
		TBuf8<20>	iBuf;
		RInternalSocket iSocket;
		TState iState;
		TInetAddr iRemoteAddr;
		TInetAddr iDestAddr;
	};

NONSHARABLE_CLASS(CEIntSockTest42) : public CEIntSockTestBase
	{
	public:
		static CEIntSockTestBase* NewL(CProxyProvd* aProvd);
		
	private:
		enum TState {EState1, EState2, EState3};

		CEIntSockTest42(CProxyProvd* aProvd);

		virtual TVerdict RunTestL();
		void Cleanup();
		
		RMBufChain iSendChain;
		RMBufChain iRecvChain;
		TBuf8<20> iBuf;
		RInternalSocket iSocket;
		TState iState;
		TInetAddr iRemoteAddr;
		TInetAddr iDestAddr;
	};

NONSHARABLE_CLASS(CEIntSockTest43) : public CEIntSockTestBase
	{
	public:
		static CEIntSockTestBase* NewL(CProxyProvd* aProvd);
		
	private:
		enum TState {EState1, EState2, EState3};

		CEIntSockTest43(CProxyProvd* aProvd);

		virtual TVerdict RunTestL();
		void Cleanup();
		
		TBuf8<20>	iBuf;
		RInternalSocket iSocket;
		TState iState;
		TInetAddr iRemoteAddr;
		TInetAddr iDestAddr;
		TSockXfrLength iLength;
	};

NONSHARABLE_CLASS(CEIntSockTest44) : public CEIntSockTestBase
	{
	public:
		static CEIntSockTestBase* NewL(CProxyProvd* aProvd);
		
	private:
		enum TState {EState1, EState2, EState3, EState4};

		CEIntSockTest44(CProxyProvd* aProvd);

		virtual TVerdict RunTestL();
		void Cleanup();
		
		RMBufChain iSendChain;
		RMBufChain iRecvChain;
		TBuf8<20> iBuf;
		RInternalSocket iSocket;
		TState iState;
		TInetAddr iRemoteAddr;
		TInetAddr iDestAddr;
		TSockXfrLength iLength;		
	};

NONSHARABLE_CLASS(CEIntSockTest45) : public CEIntSockTestBase
	{
	public:
		static CEIntSockTestBase* NewL(CProxyProvd* aProvd);
		
	private:
		enum TState {EState1,EState2,EState3,EState4};
		CEIntSockTest45(CProxyProvd* aProvd);
		virtual TVerdict RunTestL();
		void Cleanup();
		RInternalSocket iSocket;
		TState iState;
		TBuf8<20> iBuf;
		RMBufChain iSendChain;
		RMBufChain iRecvChain;
		TInetAddr iDestAddr;
	};

NONSHARABLE_CLASS(CEIntSockTest46) : public CEIntSockTestBase
	{
	public:
		static CEIntSockTestBase* NewL(CProxyProvd* aProvd);
		
	private:
		enum TState {EState1,EState2,EState3,EState4};
		CEIntSockTest46(CProxyProvd* aProvd);
		virtual TVerdict RunTestL();
		void Cleanup();
		RInternalSocket iSocket;
		TState iState;
		TBuf8<20> iBuf;		
		TSockXfrLength iLength;	
		TInetAddr 	iDestAddr;	
	};

NONSHARABLE_CLASS(CEIntSockTest47) : public CEIntSockTestBase
	{
	public:
		static CEIntSockTestBase* NewL(CProxyProvd* aProvd);
		
	private:
		enum TState {EState1,EState2};
		CEIntSockTest47(CProxyProvd* aProvd);
		virtual TVerdict RunTestL();
		void Cleanup();
		RInternalSocket iSocket;
		TState iState;
	};

NONSHARABLE_CLASS(CEIntSockTest48) : public CEIntSockTestBase
	{
	public:
		static CEIntSockTestBase* NewL(CProxyProvd* aProvd);
		
	private:
		enum TState {EState1,EState2,EState3,EState4};
		CEIntSockTest48(CProxyProvd* aProvd);
		virtual TVerdict RunTestL();
		void Cleanup();
		RInternalSocket iSocket;
		TState iState;
	};

NONSHARABLE_CLASS(CEIntSockTest49) : public CEIntSockTestBase
	{
	public:
		static CEIntSockTestBase* NewL(CProxyProvd* aProvd);
		
	private:
		enum TState {EState1,EState2,EState3,EState4,EState5};
		CEIntSockTest49(CProxyProvd* aProvd);
		virtual TVerdict RunTestL();
		void Cleanup();
		RInternalSocket iSocket;
		TState iState;
		TInetAddr iAddress;
	};

NONSHARABLE_CLASS(CEIntSockTest50) : public CEIntSockTestBase
	{
	public:
		static CEIntSockTestBase* NewL(CProxyProvd* aProvd);
		
	private:
		enum TState {EState1,EState2};
		CEIntSockTest50(CProxyProvd* aProvd);
		virtual TVerdict RunTestL();
		void Cleanup();
		RInternalSocket iSocket;
		TState iState;
	};

NONSHARABLE_CLASS(CEIntSockTest51) : public CEIntSockTestBase
	{
	public:
		static CEIntSockTestBase* NewL(CProxyProvd* aProvd);
		
	private:
		enum TState {EState1,EState2,EState3,EState4};
		CEIntSockTest51(CProxyProvd* aProvd);
		virtual TVerdict RunTestL();
		void Cleanup();
		RInternalSocket iSocket;
		TState iState;
	};

NONSHARABLE_CLASS(CEIntSockTest52) : public CEIntSockTestBase
	{
	public:
		static CEIntSockTestBase* NewL(CProxyProvd* aProvd);
		
	private:
		enum TState {EState1,EState2,EState3,EState4};
		CEIntSockTest52(CProxyProvd* aProvd);
		virtual TVerdict RunTestL();
		void Cleanup();
		RInternalSocket iAcceptingSocket;
		RInternalSocket iSocket;		
		TState iState;
	};


NONSHARABLE_CLASS(CEIntSockTest53) : public CEIntSockTestBase
	{
	public:
		static CEIntSockTestBase* NewL(CProxyProvd* aProvd);
		
	private:
		void ConstructL();
		~CEIntSockTest53();
		CEIntSockTest53(CProxyProvd* aProvd);
		virtual TVerdict RunTestL();
		void Cleanup();
		
		enum TState {EState1,EState2,EState3,EState4,EState5};
		TState iState;

		// the socket on which the operations in this test will happen
		RInternalSocket iSocket;
		TBuf8<10> iRecvBuf;
		TBuf8<20> iSendBuf;

		CAggregateCollection* iColl;
		CAggregateItem* iRecvItem;
		CAggregateItem* iSendItem;
		
		TInetAddr		iAddress;
	};

NONSHARABLE_CLASS(CEIntSockTest54) : public CEIntSockTestBase
	{
	public:
		static CEIntSockTestBase* NewL(CProxyProvd* aProvd);
		
	private:
		enum TState {EState1,EState2,EState3};
		CEIntSockTest54(CProxyProvd* aProvd);
		virtual TVerdict RunTestL();
		void Cleanup();
		RInternalSocket iSocket;		
		TState iState;
		TBuf8<10> iRecvBuf;		
		TInetAddr	iAddress;
		TSockXfrLength iLen;			

	};

NONSHARABLE_CLASS(CEIntSockTest55) : public CEIntSockTestBase
	{
	public:
		static CEIntSockTestBase* NewL(CProxyProvd* aProvd);
		
	private:
		enum TState {EState1,EState2};
		CEIntSockTest55(CProxyProvd* aProvd);
		virtual TVerdict RunTestL();
		void Cleanup();
		RInternalSocket iSocket;		
		TState iState;
		TBuf8<10> iSendBuf;		
		TInetAddr	iAddress;
	};

NONSHARABLE_CLASS(CEIntSockTest56) : public CEIntSockTestBase
	{
	public:
		static CEIntSockTestBase* NewL(CProxyProvd* aProvd);
		
	private:
		enum TState {EState1,EState2};
		CEIntSockTest56(CProxyProvd* aProvd);
		virtual TVerdict RunTestL();
		void Cleanup();
		RInternalSocket iSocket;		
		TState iState;
		TBuf8<10> iSendBuf;		
	};

NONSHARABLE_CLASS(CEIntSockTest57) : public CEIntSockTestBase
	{
	public:
		static CEIntSockTestBase* NewL(CProxyProvd* aProvd);
		
	private:
		enum TState {EState1,EState2,EState3,EState4,EState5,EState6};
		CEIntSockTest57(CProxyProvd* aProvd);
		~CEIntSockTest57();
		void ConstructL();
		virtual TVerdict RunTestL();
		void Cleanup();
		RInternalSocket iSocket;		
		TState iState;
		TBuf8<10> iSendBuf;		
		TBuf8<10> iRecvBuf;
		TSockXfrLength iRecvLen;
		CAggregateCollection* iColl;
		CAggregateItem* iShutdownItem;
		CAggregateItem* iRecvItem;	
		CAggregateItem* iSendItem;					
	};
	
NONSHARABLE_CLASS(CEIntSockTest58) : public CEIntSockTestBase
	{
	public:
		static CEIntSockTestBase* NewL(CProxyProvd* aProvd);
		
	private:
		enum TState {EState1,EState2,EState3};
		CEIntSockTest58(CProxyProvd* aProvd);
		virtual TVerdict RunTestL();
		void Cleanup();
		RInternalSocket iAcceptingSocket;
		RInternalSocket iSocket;		
		TState iState;
	};

NONSHARABLE_CLASS(CEIntSockTest59) : public CEIntSockTestBase
	{
	public:
		static CEIntSockTestBase* NewL(CProxyProvd* aProvd);
		
	private:
		enum TState {EState1, EState2, EState3,EState4};

		CEIntSockTest59(CProxyProvd* aProvd);

		virtual TVerdict RunTestL();
		void Cleanup();
		
		RMBufChain iSendChain;
		RMBufChain iRecvChain;
		TBuf8<20> iBuf;
		RInternalSocket iSocket;
		TState iState;
		TInetAddr iRemoteAddr;
		TInetAddr iDestAddr;
		TSockXfrLength iLength;		
	};

NONSHARABLE_CLASS(CEIntSockTest60) : public CEIntSockTestBase
	{
	public:
		static CEIntSockTestBase* NewL(CProxyProvd* aProvd);
		
	private:
		enum TState {EState1,EState2,EState3,EState4,EState5,EState6,EState7,EState8};
		CEIntSockTest60(CProxyProvd* aProvd);
		~CEIntSockTest60();
		void ConstructL();
		virtual TVerdict RunTestL();
		void Cleanup();
		RInternalSocket iSocketIn;		
		RInternalSocket iSocketOut;				
		TState iState;
		TBuf8<64> iRecvBuf;		
		TBuf8<64> iRecvBuf2;
		TBuf8<64> iSendBuf;
		TSockXfrLength iRecvLen;
		CAggregateCollection* iColl;
		CAggregateItem* iRecvItem;	
		CAggregateItem* iSendItem;	
		TInetAddr		iInetAddrTo;
		TInetAddr		iInetAddrFrom;				
	};


NONSHARABLE_CLASS(CEIntSockTest61) : public CEIntSockTestBase
    {
    public:
        static CEIntSockTestBase* NewL(CProxyProvd* aProvd);
        
    private:
        CEIntSockTest61(CProxyProvd* aProvd);
        virtual TVerdict RunTestL();
        void Cleanup();
    };

#endif //PTESTCASES_H

