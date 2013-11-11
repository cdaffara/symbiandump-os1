// Copyright (c) 2005-2010 Nokia Corporation and/or its subsidiary(-ies).
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
// Defines the Network-side events & error conditions tests for SPUD.
// 
//

/**
 @file
 @internalComponent
*/

#ifndef TE_SPUD_NETWORK_SIDE_STEPS_H
#define TE_SPUD_NETWORK_SIDE_STEPS_H


#include <test/testexecutestepbase.h>
#include <c32comm.h>
#include <es_sock.h>
#ifndef SYMBIAN_NON_SEAMLESS_NETWORK_BEARER_MOBILITY
#include <qoslib.h>
#ifdef SYMBIAN_ENABLE_SPLIT_HEADERS
#include <networking/qoslib_internal.h>
#endif
#endif

/** Encapsulates all Spud Network Side tests */
namespace te_spudNetworkSide
	{
	
	const TUint KConfiguredTftFilter1DestPort	= 1031;
	const TUint KConfiguredTftFilter1SrcPort 	= 3461;
	const TUint KConfiguredTftFilter2DestPort	= 1032;	
	
	_LIT(KConfiguredTftFilter2DestAddr, "192.168.1.2");
	_LIT(KConfiguredTftFilter1SrcAddr, "192.168.1.1");

		
	/** Identifiers for each of the simtsy parameters that can be changed using publish and subscribe */
	enum TEtelRequestType
		{
		/** Change QoS parameters */
		ENetworkQoSChange,
		/** Change network status */
		ENetworkChangeRegStatus,
		/** Change context status */
		EContextStatusChange,
		/** Change context config parameters */
		EContextConfigChange,
		/**Invalid Request */
		EInvalidRequest,
		};

	enum TContextType
		{
		EPrimaryContext,
		ESecondaryContext
		};
		
	/**
	 Base class for all Spud Network Side Tests.
	 
	 @internalComponent
	 */
#ifndef SYMBIAN_NON_SEAMLESS_NETWORK_BEARER_MOBILITY
	class CSpudNetSideTestBase : public CTestStep, public MQoSObserver
#else
    class CSpudNetSideTestBase : public CTestStep
#endif
		{
	public:
	
		TVerdict doTestStepPreambleL();
#ifdef SYMBIAN_NON_SEAMLESS_NETWORK_BEARER_MOBILITY
		TVerdict doTestStepPostambleL();
#endif
		
		enum TVerdict doTestStepL();

#ifndef SYMBIAN_NON_SEAMLESS_NETWORK_BEARER_MOBILITY
		// MQoSObserver	
		void Event(const CQoSEventBase& aEvent);
#endif
		
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

		// utility
#ifndef SYMBIAN_NON_SEAMLESS_NETWORK_BEARER_MOBILITY
		void WaitForQoSEventL(TQoSEvent aQosEvent, TInt aReasonCode);
		void WaitForOptionalQoSEventL(TQoSEvent aQosEvent, TInt aReasonCode, TQoSEvent aOptionalQosEvent, TInt aOptionalReasonCode);
		void WaitForOptionalQoSEventL(const TDesC& aQosEventString, const TDesC& aReasonCodeString, TQoSEvent aOptionalQosEvent, const TDesC& aOptionalReasonCodeString);

		void WaitForQoSEventL(const TDesC& aQosEventString, TInt aReasonCode);
		void WaitForQoSEventL(const TDesC& aQosEventString, const TDesC& aReasonCodeString);
		void WaitForOptionalQoSEventL(const TDesC& aQosEventString, const TDesC& aReasonCodeString, const TDesC& aOptionalQosEventString, const TDesC& aOptionalReasonCodeString);

		void SetQoSParameters(CQoSParameters& aParameters);
		void SetQoSParameters1(CQoSParameters& aParameters);
#endif
		void SetSimTsyTestNumberL(TInt aTestNumber);
				
		void StartPrimaryOnlyL();
		void OpenPrimarySocketL();
		void SendUntilTimeoutL();
		void SendSinglePacketL();
		void StopPrimary();
		
		TBool SpudDeletePrimaryPdpL();
		
		void StartSecondaryL();
		void StopSecondaryL();
		void RetrieveMandatorySettingsFromConfigL();
#ifdef SYMBIAN_NON_SEAMLESS_NETWORK_BEARER_MOBILITY
		void ReadQosParameterSettingsFromConfigL();

		void VerifySubconnectionCountL(const TDesC& aSubConnDesc, TUint aIap);
		void VerifySubconnectionCountL(TUint aSubConnectionCount, TUint aIap);
		void UpdatePolicySelector2QosParametersTableL(TUint aPolicySelector, TUint aQosParameter);
		void ClearPolicySelector2QosParametersTableL();
#endif
		
		void FailNextPktLoopbackCsyWriteL(TInt aPortNum, TInt aErrorCode);
		void ClearNextPktLoopbackCsyWriteL(TInt aPortNum);
		TEtelRequestType RetrieveNetSideEventRequestFromConfigL(const TDesC& aEventName);
		void EtelRequestL(TEtelRequestType aEtelCommand, TInt aNewValue);
		TInt RetrieveNetSideEventValFromConfigL(const TDesC& aEventValName);
#ifndef SYMBIAN_NON_SEAMLESS_NETWORK_BEARER_MOBILITY
		enum TQoSEvent RetrieveQosEventFromConfigL(const TDesC& aQosEventName);
#endif
		TContextType RetrieveContextTypeFromConfigL(const TDesC& aContextTypeName);
		TCommRole RetrieveCommRoleFromConfigL(const TDesC& aCommRoleName);
		void WaitForProgressNotificationL(TRequestStatus& aStatus, TInt aProgress, TInt aErrCode);
		void StopInterfaceL();
		
		void InitLoopbackL(const TDesC& aLoopbackPortName);
				
		virtual void InitiateSecondaryStartL();
#ifndef SYMBIAN_NON_SEAMLESS_NETWORK_BEARER_MOBILITY
		virtual void ChangeQoSL();
#endif
		
		// Data
		TInt iSimTsyTestNum;
#ifndef SYMBIAN_NON_SEAMLESS_NETWORK_BEARER_MOBILITY
		TQoSEvent iReceivedQosEvent;
#else
		TInt iPolicySelector;
		TUint iQosParametersRecord;
#endif
    	TInt iQosReason;
    	
		RSocketServ iEsock;
		RSocket iSocket;
		/** Socket used in RQoSChannel Join*/
		RSocket iSocketJoin;
#ifndef SYMBIAN_NON_SEAMLESS_NETWORK_BEARER_MOBILITY
		RQoSPolicy iQoSPolicy_1;
#endif
		TNifProgressBuf iProgressBuf;
		TRequestStatus iQosEventReq;
		
		RConnection iInterface;
		
		RCommServ iCommServer;
		RComm iLoopbackPort;
		
		};
	
		
	/** Used to define a literal variable. */
	#define SYMBIAN_TEST_LIT(name) _LIT(K##name, #name)

	/** Define a literal from a classname and can be as the keyword class. */
	#define SYMBIAN_TEST_CLASS(className) SYMBIAN_TEST_LIT(className); class className 

	/** Define a basic declaration for testClass as a subclass of CSpudNetSideTestBase 
	and define a literal from testClass */
	#define SYMBIAN_SPUD_TEST_CLASS(testClass) SYMBIAN_TEST_CLASS(testClass) : public CSpudNetSideTestBase \
				{ \
			public: \
				inline testClass() {SetTestStepName(K##testClass);}; \
				virtual TVerdict RunTestStepL(); \
				}; 
					

	// Test Steps
	SYMBIAN_SPUD_TEST_CLASS(CESockCheck)
	SYMBIAN_SPUD_TEST_CLASS(CSpudPrimary)
	SYMBIAN_SPUD_TEST_CLASS(CSpudPrimaryEvent)
	SYMBIAN_SPUD_TEST_CLASS(CSpudPrimaryContextEventProgress)
	SYMBIAN_SPUD_TEST_CLASS(CSpudDeletePrimary)
	SYMBIAN_SPUD_TEST_CLASS(CSpudPrimaryEventOnLowerNifStarting)
	SYMBIAN_SPUD_TEST_CLASS(CSpudSecondary)
	SYMBIAN_SPUD_TEST_CLASS(CSpudSuspendContext)
		
#ifndef SYMBIAN_NON_SEAMLESS_NETWORK_BEARER_MOBILITY
	SYMBIAN_SPUD_TEST_CLASS(CSpudSecondaryChangeQoS)
	SYMBIAN_SPUD_TEST_CLASS(CSpudSecondaryEvent)
	SYMBIAN_SPUD_TEST_CLASS(CSpudTwoSecondaryEvents)
	SYMBIAN_SPUD_TEST_CLASS(CSpudSecondaryEvent2)		
#endif
	
	// Nifman side testing
	SYMBIAN_SPUD_TEST_CLASS(CSpudPrimaryInterfaceStop)
	SYMBIAN_SPUD_TEST_CLASS(CSpudPrimaryInterfaceEventStop)
	SYMBIAN_SPUD_TEST_CLASS(CSpudPrimaryDeletionInterfaceStop)
		
	SYMBIAN_SPUD_TEST_CLASS(CSpudSecondaryInterfaceStop)
	SYMBIAN_SPUD_TEST_CLASS(CSpudSecondaryInterfaceEventStop)
	SYMBIAN_SPUD_TEST_CLASS(CSpudPrimayStartingStop)
	SYMBIAN_SPUD_TEST_CLASS(CSpudSecondaryStartingStop)
	SYMBIAN_SPUD_TEST_CLASS(CSpudSecondaryLowerNifDownStop)	
	SYMBIAN_SPUD_TEST_CLASS(CSpudPppPrimary)
	SYMBIAN_SPUD_TEST_CLASS(CSpudPppSecondarySend)
	SYMBIAN_SPUD_TEST_CLASS(CSpudMultiPrimary)
	
	//SIP server address retrieve
	SYMBIAN_SPUD_TEST_CLASS(CIoctlAddressRetrieve)
	SYMBIAN_SPUD_TEST_CLASS(CRawIpMinMaxMMU)
	} 
	
#endif 
