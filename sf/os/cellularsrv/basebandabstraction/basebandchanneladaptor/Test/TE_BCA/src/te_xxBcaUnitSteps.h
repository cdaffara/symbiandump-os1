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
// Defines the test steps for xxBCA.
// 
//

/**
 @file
 @internalComponent
*/

#ifndef TE_xxBCAUNITSTEPS_H
#define TE_xxBCAUNITSTEPS_H


#include <test/TestExecuteStepBase.h>
#include "BcaTestProxy.h"
#include <c32comm.h>

/** Encapsulates all xxBca tests */
namespace te_xxBcaUnitTests
	{
	/**
	 Base class for all xxBca tests.
	 
	 @internalComponent
	 */
	class CxxBcaTestBase : public CTestStep
		{
	public:
		CxxBcaTestBase();
		~CxxBcaTestBase();
		TVerdict doTestStepPreambleL();
		enum TVerdict doTestStepL();
		
	protected:
		void LoadUutL();
		void LoadPeerL();
		
		void UnloadUut();
		void UnloadPeer();
		
		void CheckBuffersEqualL(const TDesC16& aBuf1,const TDesC16& aBuf2);
		void CheckBuffersEqualL(const TDesC8& aBuf1,const TDesC8& aBuf2);

		void SetProxyIapIdL(MBcaProxy& aProxy, TUint32 aIapId);
		
		
		void TestEnvironmentCleanup();
		void LogAndLeaveIfErrorL(TInt errCode, const TDesC& aMsg);
		
		
		void LoadBcaSettingsFromConfigFileL();
		void LoadBcaIapSettingsFromConfigFileL();
		
		MBcaFactory* NewBcaFactoryL(TDllAutoClose& bcaDll);
		void CreateBcaAndLeaveL(TDllAutoClose& bcaDll);
		
		/**
		Executes the Test Step 
		 
		@return Verdict of the test step: EPass / EFail;
        		EAborted if the test case could not be executed properly;
        		EInconclusive if it could not be determined with certainty that the test case has succeeded.
		@leave if the test fails		 
		*/
		virtual TVerdict RunTestStepL() = 0;

	protected:
		
		
		/** Unit Under Test BCA */
		MBcaProxy* iUut;
		/** UUT C32 port name */
		TPtrC iUutPortName;
		
		/** UUT name for logging */
		TPtrC iUutName;
		/** CommDB IAP ID for this test */
		TUint32 iUutIapId;
		
		/** Peer: used to excersize UUT */
		MBcaProxy* iPeer;
		/** Peer C32 Port name */
		TPtrC iPeerPortName;
		/** Peer name for logging */
		TPtrC iPeerName;
		/** CommDB IAP ID for this test */
		TUint32 iPeerIapId;
		
		
		/** Buffer sizes constants */
		enum TBufferSize
			{
			/* Minimal buffer */
			EBufferSizeSmall = 16
			};
	    
	   
	    
	    /** Buffer to be written only */
	    TBuf8<EBufferSizeSmall> iWriteBuf1;
	    /** Buffer to be read only */
		TBuf8<EBufferSizeSmall> iReadBuf1;
		
		/* Second writing buffer */
		TBuf8<EBufferSizeSmall> iWriteBuf2;
		/** Second reading buffer */
		TBuf8<EBufferSizeSmall> iReadBuf2;
		
	
		}; 


	/** Used to define a literal variable. */
	#define SYMBIAN_TEST_LIT(name) _LIT(K##name, #name)

	/** Define a literal from a classname and can be as the keyword class. */
	#define SYMBIAN_TEST_CLASS(className) SYMBIAN_TEST_LIT(className); class className 

	/** Define a basic declaration for testClass as a subclass of CxxBcaTestBase 
	and define a literal from testClass */
	#define SYMBIAN_xxBCA_TEST_CLASS(testClass) SYMBIAN_TEST_CLASS(testClass) : public CxxBcaTestBase \
				{ \
			public: \
				inline testClass() {SetTestStepName(K##testClass);}; \
				virtual TVerdict RunTestStepL(); \
				}; 
				
				
	/** Test xxBCA in Closed state */
	SYMBIAN_xxBCA_TEST_CLASS(CClosedState1)

	/** Test xxBCA in Shutdown state */
	SYMBIAN_xxBCA_TEST_CLASS(CShutdownState1)
	SYMBIAN_xxBCA_TEST_CLASS(CShutdownState2)

	/** Test xxBCA in Opening state */
	SYMBIAN_xxBCA_TEST_CLASS(COpeningState1)
	SYMBIAN_xxBCA_TEST_CLASS(COpeningState2)
	SYMBIAN_xxBCA_TEST_CLASS(COpeningState3)


	/** Test xxBCA in Open state */
	SYMBIAN_xxBCA_TEST_CLASS(COpenState1)
	SYMBIAN_xxBCA_TEST_CLASS(COpenState2)  
	SYMBIAN_xxBCA_TEST_CLASS(COpenState3)
	SYMBIAN_xxBCA_TEST_CLASS(COpenState4)

	/** Test xxBCA in Reading state */
	SYMBIAN_xxBCA_TEST_CLASS(CReadingState1)
	SYMBIAN_xxBCA_TEST_CLASS(CReadingState2)
	SYMBIAN_xxBCA_TEST_CLASS(CReadingState3)

	/** Test xxBCA in Writing state */
	SYMBIAN_xxBCA_TEST_CLASS(CWritingState1)
	SYMBIAN_xxBCA_TEST_CLASS(CWritingState2)
	SYMBIAN_xxBCA_TEST_CLASS(CWritingState3)

	/** Test xxBCA in ReadingWriting state */
	SYMBIAN_xxBCA_TEST_CLASS(CReadingWritingState1)
	SYMBIAN_xxBCA_TEST_CLASS(CReadingWritingState2)
	SYMBIAN_xxBCA_TEST_CLASS(CReadingWritingState3)
	SYMBIAN_xxBCA_TEST_CLASS(CReadingWritingState4)


	/** Test xxBCA in Controlling state */
	SYMBIAN_xxBCA_TEST_CLASS(CIoctlingState1)
	SYMBIAN_xxBCA_TEST_CLASS(CIoctlingState2)
	
	/** Test xxBCA deletion (release) functinality */
	SYMBIAN_xxBCA_TEST_CLASS(CCleanupBcaRelease)	

	} // namespace te_xxBcaUnitTests
#endif 
