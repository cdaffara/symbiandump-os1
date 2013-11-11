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
// te_spudNetworkSideSteps.h
// Defines the Network-side events & error conditions tests for SPUD.
// 
//

/**
 @file
 @internalComponent
*/

#ifndef TE_SPUD_RSUBCON_STEPS
#define TE_SPUD_RSUBCON_STEPS


#include <test/testexecutestepbase.h>

#include <es_sock.h>
#include <in_sock.h>
#include <commdbconnpref.h>

/**
A driver for running an arbitrary interface (over the loopback), 
to be used when connected tests cannot be run; i.e. in lieu of WinTunnel */


/**
Base class for all Spud - RSubConnection Tests. */
class CSpudRSubConnTestStepBase : 
	public CTestStep
	{
public:
	TVerdict doTestStepPreambleL();
	enum TVerdict doTestStepL();
		
	// Utility
	void TestBooleanTrueL(TBool aBool, const TDesC& aMsg);
	void TestL(TInt aErrCode, const TDesC& aMsg);
	void TestL(TInt aErrCode, TInt aExpErrCode, const TDesC& aMsg);
		
		
	
protected:
	/**
	Executes the Test Step 
		 
	@return Verdict of the test step: EPass / EFail;
       		EAborted if the test case could not be executed properly;
       		EInconclusive if it could not be determined with certainty that the test case has succeeded.
	@leave if the test fails		 
	*/
	virtual TVerdict RunTestStepL() = 0;

	
	void OpenAndStartInterfaceL(RConnection& aConn, TCommDbConnPref& aPref, TRequestStatus& aSt, const TDesC& aLogMsg);
	void WaitForCompletionL(TRequestStatus& aSt, TInt aExpErr, const TDesC& aLogMsg);

	void ReceiveOnSocketL(RSocket& aSocket, const TDesC& aLogMsg);
	void RecvSendOnSocketL(RSocket& aSocket, const TDesC& aLogMsg);
	void OpenAndBindSocketL(RSocket& aSock, RConnection& aIface, TUint aLocalPort, const TDesC& aLogMsg);

	TBool ApplyQoSParametersL(RSubConnection& aPdpContext);
	void ApplyQoSParametersSet2L(RSubConnection& aPdpContext);

	void WriteReadOnSocketL(RSocket& aSocket, const TDesC& aLogMsg);
	void SendRecvOnSocketL(RSocket& aSocket, TInetAddr& aDstAddr, const TDesC& aLogMsg);

	void VerifyBuffersEqualL(const TDesC8& aBuf1, const TDesC8& aBuf2);

	/** Our connection to ESock */
	RSocketServ iESock;
	
	/**
	The SPUD interface with 1 primary & 1 secondary pdp contexts */
	RConnection iSpud;
	
	/**
	Secondary PDP context */
	RSubConnection iSpudScndCtx1;
		
	/** SPUD primary PDP context SAP */
	RSocket iPrimCtxSock1;
		
	/** SPUD secondary PDP context SAP */
	RSocket iScndCtxSock1;
	};
		
		

	/** Used to define a literal variable. */
	#define SYMBIAN_TEST_LIT(name) _LIT(K##name, #name)

	/** Define a literal from a classname and can be as the keyword class. */
	#define SYMBIAN_TEST_CLASS(className) SYMBIAN_TEST_LIT(className); class className 

	/** Define a basic declaration for testClass as a subclass of CSpudNetSideTestBase 
	and define a literal from testClass */
	#define SYMBIAN_SPUD_TEST_CLASS(testClass) SYMBIAN_TEST_CLASS(testClass) : public CSpudRSubConnTestStepBase \
				{ \
			public: \
				inline testClass() {SetTestStepName(K##testClass);}; \
				virtual TVerdict RunTestStepL(); \
				}; 
	
	
// Test Steps
SYMBIAN_SPUD_TEST_CLASS(CUdp1)
SYMBIAN_SPUD_TEST_CLASS(CUdp2)
SYMBIAN_SPUD_TEST_CLASS(CLoopbackPpp1)
SYMBIAN_SPUD_TEST_CLASS(CUpperFlowOnLowerNifDown)
SYMBIAN_SPUD_TEST_CLASS(CSpudPppPrimaryStop)
#endif 
