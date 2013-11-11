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
// Defines the test steps for C32 BCA.
// 
//

/**
 @file
 @internalComponent
*/

#ifndef TE_C32BCAUNITSTEPS_H
#define TE_C32BCAUNITSTEPS_H


#include <test/testexecutestepbase.h>
#include "BcaTestProxy.h"
#include <c32comm.h>

/** Encapsulates all C32Bca tests */
namespace te_c32BcaUnitTests
	{
	/**
	 Base class for all C32Bca tests.
	 
	 @internalComponent
	 */
	class CC32BcaTestBase : public CTestStep
		{
	public:
		CC32BcaTestBase();
		~CC32BcaTestBase();
		TVerdict doTestStepPreambleL();
		enum TVerdict doTestStepL();
		
	protected:
		void LoadUutL();
		void LoadPeerL();
		
		void UnloadUut();
		void UnloadPeer();
		
		void CheckBuffersEqualL(const TDesC16& aBuf1,const TDesC16& aBuf2);
		void CheckBuffersEqualL(const TDesC8& aBuf1,const TDesC8& aBuf2);

		void SetProxyCsyL(MBcaProxy& aProxy, const TDesC& aCsy);
		void SetProxyIapIdL(MBcaProxy& aProxy, TUint32 aIapId);
		
		void OpenProxyL(MBcaProxy& aProxy, const TDesC& aPortName);
		void OpenProxyL(MBcaProxy& aProxy, TUint32 aIapId, const TDesC& aPortName);
		void LoadOpenUutAndPeerL();
		void ShutdownUnloadUutAndPeerL();
		
		void OpenRealCommPortL(RComm& aCommPort, const TDesC& aCsyName, const TDesC& aPortName, TCommRole aCommRole = ECommRoleDTE);
		
		void TestEnvironmentCleanup();
		void LogAndLeaveIfErrorL(TInt errCode, const TDesC& aMsg);
		
		void DoDuplexReadWriteL(TInt aIterations);
		
		void LoadBcaSettingsFromConfigFileL();
		void LoadBcaIapSettingsFromConfigFileL();
		void LoadC32SettingsFromConfigFileL();
		TCommRole LoadExpectedCommRoleFromConfigFileL();
		void LoadCsyNameFromConfigFileL();
		void ForceLoadC32LddAndPdd(const TDesC& aLddName, const TDesC& aPddName);
		
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
		
		/** C32 Csy name used for the test */
		TPtrC iCsyName;
		
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
		
		/** C32 Comm server session */
		RCommServ iCommServ;
		
		/** Real C32 Comm Port */
		RComm iCommPort;
		
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
		
		/** Buffer to be used with Ioctl */
		TControlBuf iControlBuf1;
		/** Packaged buffer to Ioctl */
		TPckgBuf<TControlBuf> iControlBufPckg1;
		
		/** Buffer used for reading port name */
		TPortNameBuf iReadPort1Buf;
		/** Packaged port name */
		TPckgBuf<TPortNameBuf> iReadPort1BufPckg;
		
		/** Buffer used for setting the port name */
		TPortNameBuf iWritePort1Buf;
		
		/** Port name setting package */
		TPckgBuf<TPortNameBuf> iWritePort1BufPckg; 
		
		/** C32 physical device driver name. */
		TPtrC iC32PddName;
		/** C32 logical device driver name */
		TPtrC iC32LddName;
		/** C32 CSY name used with real comm port */
		TPtrC iC32CsyName;
		/** Real comm port (as opposed to the one used by the BCA */
		TPtrC iC32PortName;		
		}; 


	/** Used to define a literal variable. */
	#define SYMBIAN_TEST_LIT(name) _LIT(K##name, #name)

	/** Define a literal from a classname and can be as the keyword class. */
	#define SYMBIAN_TEST_CLASS(className) SYMBIAN_TEST_LIT(className); class className 

	/** Define a basic declaration for testClass as a subclass of CC32BcaTestBase 
	and define a literal from testClass */
	#define SYMBIAN_C32BCA_TEST_CLASS(testClass) SYMBIAN_TEST_CLASS(testClass) : public CC32BcaTestBase \
				{ \
			public: \
				inline testClass() {SetTestStepName(K##testClass);}; \
				virtual TVerdict RunTestStepL(); \
				}; 
				
				
	/** Test C32 BCA in Closed state */
	SYMBIAN_C32BCA_TEST_CLASS(CClosedState1)
	SYMBIAN_C32BCA_TEST_CLASS(CClosedState2)

	/** Test C32 BCA in Shutdown state */
	SYMBIAN_C32BCA_TEST_CLASS(CShutdownState1)
	
	/** Test C32 BCA in Open state */
	SYMBIAN_C32BCA_TEST_CLASS(COpenState1)
	SYMBIAN_C32BCA_TEST_CLASS(COpenState2)

	/** Test C32 BCA in Reading state */ 
	SYMBIAN_C32BCA_TEST_CLASS(CReadingState1)
	SYMBIAN_C32BCA_TEST_CLASS(CReadingState2)
	SYMBIAN_C32BCA_TEST_CLASS(CReadingState3)

	/** Test C32 BCA in Writing state */
	SYMBIAN_C32BCA_TEST_CLASS(CWritingState1)
	SYMBIAN_C32BCA_TEST_CLASS(CWritingState2)
	SYMBIAN_C32BCA_TEST_CLASS(CWritingState3)

	/** Test C32 BCA in ReadingWriting state */
	SYMBIAN_C32BCA_TEST_CLASS(CReadingWritingState1)
	SYMBIAN_C32BCA_TEST_CLASS(CReadingWritingState2)
	SYMBIAN_C32BCA_TEST_CLASS(CReadingWritingState3)
	SYMBIAN_C32BCA_TEST_CLASS(CReadingWritingState4)
	SYMBIAN_C32BCA_TEST_CLASS(CReadingWritingState5)

	/** Test C32 BCA in Controlling state */
	SYMBIAN_C32BCA_TEST_CLASS(CIoctlingState1)
	SYMBIAN_C32BCA_TEST_CLASS(CIoctlingState2)


	/** Test C32 BCA Packet Boundary Preservation */
	SYMBIAN_C32BCA_TEST_CLASS(CPacketBoundary1)

	/** Test C32 BCA Ioctl options */
	SYMBIAN_C32BCA_TEST_CLASS(CIoctlOption1)
	SYMBIAN_C32BCA_TEST_CLASS(CIoctlOption2)
	SYMBIAN_C32BCA_TEST_CLASS(CIoctlInvalidOption)
	SYMBIAN_C32BCA_TEST_CLASS(CIoctlSerialPort1)
	SYMBIAN_C32BCA_TEST_CLASS(CIoctlSerialPort2) 
	SYMBIAN_C32BCA_TEST_CLASS(CIoctlSerialPort3)  
	SYMBIAN_C32BCA_TEST_CLASS(CIoctlSerialPort4) 

	/** Test C32 BCA handling Invalid CommDB configuration */
	SYMBIAN_C32BCA_TEST_CLASS(CCommDbNoChannelId)
	SYMBIAN_C32BCA_TEST_CLASS(CNoChannelIdNoIapId)
	
	/** Test C32 BCA handling of hidden IAP records   */
	SYMBIAN_C32BCA_TEST_CLASS(CHiddenIAPRecord)
	
	/** Test C32 BCA handling of Port Comm Role */
	SYMBIAN_C32BCA_TEST_CLASS(CCommDbModemCommRole)
	
	/** Test C32 BCA deletion (release) functionality */
	SYMBIAN_C32BCA_TEST_CLASS(CCleanupBcaRelease)	
	
	/** Test C32 BCA open functionality with the different combinations of ChannelId*/
	SYMBIAN_C32BCA_TEST_CLASS(CValidChannelId)
	
	
	} // namespace te_c32BcaUnitTests
#endif 
