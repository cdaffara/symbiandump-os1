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
// This contains CTestCaseEsock which is the base class for all 
// the ESOCK suite test cases
// 
//

#if (!defined __ESOCKTESTSTEP_H__)
#define __ESOCKTESTSTEP_H__
#include <in_sock.h>
#include <test/testexecutestepbase.h>
#include "TestSuiteEsock.h"

class CEasyTestStep : public CTestStep
	{
	public:
		virtual TVerdict doTestStepPreambleL()
			{
			SetTestStepResult(easyTestStepPreambleL());
			return TestStepResult(); 
			}
		virtual TVerdict doTestStepL()
			{
			MergeVerdict(easyTestStepL());
			return TestStepResult(); 
			}
		virtual TVerdict doTestStepPostambleL()
			{
			MergeVerdict(easyTestStepPostambleL());
			return TestStepResult(); 
			}
		
		virtual TVerdict easyTestStepPreambleL()
			{ return EPass; }
		virtual TVerdict easyTestStepL() = 0;
		virtual TVerdict easyTestStepPostambleL()
			{ return EPass; }
		
		void MergeVerdict(TVerdict aVerdict)
			{
			if(TestStepResult() == EPass)
				{
				SetTestStepResult(aVerdict);
				}
			}
	};

class CTestStepEsock : public CEasyTestStep
    {
	
	public :
		enum { ipAddress1 =0 , ipAddress2 , ipAddress3 , ipAddress4 , ipAddress5 , ipAddress7, ipAddress8, ipAddress9 } ip_table;
	public:
		// Close sockets in Postamble.
		TVerdict easyTestStepPostambleL();
		
		const TDesC& SectionName(const TDesC& aSectName);
		
		// config file access - Esock specific
		TBool GetIpAddressFromConfig(const TDesC &aSectName,const TDesC &aKeyName,TInetAddr &anAddr);
		
		void StripeDes32(TDes8 &aBuf, TInt aStartPos, TInt anEndPos);
		void StripeMem32(TUint *aBuf, TInt aStartPos, TInt aEndPos);
		void StripeMem(TUint8 *aBuf, TInt aStartPos, TInt anEndPos, TUint aStartChar, TUint anEndChar, TInt aOffset=0);
		void StripeDes(TDes8 &aBuf, TInt aStartPos, TInt anEndPos, TUint aStartChar, TUint anEndChar, TInt aOffset=0);
		
		// common socket operations
		TInt OpenListeningSocketL(TInetAddr anAddr, TInt &aSockIndex, TInt aPort=1, TInt aSize=5);
		TInt OpenActiveSocketL(TInetAddr anAddr, TInt &aSockIndex, TInt aPort=1);
		TInt AcceptConnectionL(TInt &aSockIndex, TInt aListenIndex);
		void CloseSockets(TUint aLoglevel = 0);
		TInt OpenSockets(TUint addrFamily, TUint aSockType, TUint aProtocol, TUint aNumSockets = 1);
		TInt OpenSockets(const TDesC &aName, TUint aNumSockets = 1);
		TInt OpenSockets(TUint aNumSockets = 1);
		TInt OpenMinSockets(const TDesC &aSectName, TUint addrFamily, TUint aSockType, TUint aProtocol, TUint aMinSockets = 1);
		TInt OpenConnectionsL(const TDesC &aSectName, TInt &aSockIndex2, TInt &aSockIndex3);
		
		TInt SetRoute(TInt aOp, TUint32 aDest, TUint32 aNetMask, TUint32 aGateway, TUint32 aInterface, TInt aMetric);
		TInt ListRoutes();
		
		void GetHostByNameL(TNameRecord& aRecord, const TDesC& aHost);
		void GetHostByAddrL(TNameRecord& aRecord, const TInetAddr& aAddr);
		void GetHostNameL();
		void SetHostNameL(const TDesC& aHost);
		void GetHostByNameCancelL(TNameRecord& aRecord, const TDesC& aHost);
		
		// IPv4
		TBool GetAppendixB1();
		TBool GetAppendixB2();
		TBool GetAppendixB3();
		TBool GetAppendixB4();
		TBool GetAppendixB5();
		TBool GetAppendixB6();
		
		// IPv6
		TBool GetAppendixC1();
		void GetAppendixC2();
		TBool GetAppendixC3();
		
		// pointer to suite which owns this test 
		CTestSuiteEsock* iEsockSuite;
		
		RArray<TInetAddr> appendixInArray, appendixOutArray; 
		
		RHeap* TestHeap;
		
	private:
    };
    
  class CESockTestStep_OOMCapable : public CTestStepEsock
	{
public:
	~CESockTestStep_OOMCapable()
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
	// virtual enum TVerdict InternalDoTestStepL() =0;
private:
	RSocketServ iOOMSess;
	};


#endif /* __ESOCKTESTSTEP_H__ */


