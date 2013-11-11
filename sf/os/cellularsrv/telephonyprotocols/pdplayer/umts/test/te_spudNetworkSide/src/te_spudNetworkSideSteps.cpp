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
// Implements the integration tests for the SPUD using RawIpNif
// 
//

/**
 @file
 @internalComponent
*/

#include <e32math.h>
#include "te_spudNetworkSideSteps.h"
#include <c32comm.h> // uniquely for the call to StartC32WithCMISuppressions
#include "spudNetSideQos.h"
#include <simtsy.h>
#include <commdbconnpref.h>
#include <nifvar.h> // Nifman Progress notifications
#include <in_sock.h>
#include <es_enum.h> 
#include <comms-infras/es_config.h>
#include <nifman.h>

#ifndef SYMBIAN_NON_SEAMLESS_NETWORK_BEARER_MOBILITY
#include <networking/qoslib.h>
#else
#include <commsdat.h>
#include <commsdattypesv1_1.h>
#include <commsdattypeinfov1_1_internal.h>
#endif

#ifdef SYMBIAN_NON_SEAMLESS_NETWORK_BEARER_MOBILITY
using namespace CommsDat;
#define PDPIAP 2
#endif

using namespace te_spudNetworkSide; 

_LIT(KCsyName,"PKTLOOPBACK");
_LIT8(KCommWriteData, "Data to send");
_LIT8(KCommWriteData2, "Second data to send");
// length of a buffer long enough to read KCommWriteData
const TUint KCommReadBufSize = 64;

// These aren't defined in a common header file.
static const RProperty::TType KUidPSCsyWriteResultCategoryKeyType = RProperty::EInt;
static const TUid KUidPSCsyWriteResultCategory = {0x10206869}; //KUidLoopbackCSYMin + 5
	
const TTimeIntervalMicroSeconds32 KReadDelay(10000000);
const TUint KTimeToStartSecondary(1000000*5); // give it 5 seconds in case logging is on

/**
 Implements the pure virtual doTestStepPreambleL defined in CTestStep. 
 Used to disable the phone book synchronizer
 which may cause the tests to fail.
 
 
 @return EPass if successful.
 @leave If Phonebook synchronizer disabling fails. 
 */
TVerdict CSpudNetSideTestBase::doTestStepPreambleL()
	{	
	_LIT(KPhbkSyncCMI, "phbsync.cmi");
	TInt err = StartC32WithCMISuppressions(KPhbkSyncCMI);
	TESTL(KErrNone == err || KErrAlreadyExists == err);
	INFO_PRINTF1(_L("Test Step Preamble: disabled Phonebook Synchronizer."));	
	return EPass;
	}

#ifdef SYMBIAN_NON_SEAMLESS_NETWORK_BEARER_MOBILITY
TVerdict CSpudNetSideTestBase::doTestStepPostambleL()
	{
	ClearPolicySelector2QosParametersTableL();
	return TestStepResult();
	}
#endif

void CSpudNetSideTestBase::FailNextPktLoopbackCsyWriteL(TInt aPortNum, TInt aErrorCode)
	{
	TInt dummy(0);
	TInt ret = RProperty::Get(KUidPSCsyWriteResultCategory, aPortNum, dummy);
	if(ret == KErrNotFound)
		{
		RProperty::Define(KUidPSCsyWriteResultCategory, aPortNum, KUidPSCsyWriteResultCategoryKeyType);
		}
	TestL(RProperty::Set(KUidPSCsyWriteResultCategory, aPortNum, aErrorCode), _L("Set PSCsy write result"));
  	INFO_PRINTF3(_L("Next write on [PKTLOOPBACK::%d] is going to fail with [%d]"), aPortNum, aErrorCode);
  	}

void CSpudNetSideTestBase::ClearNextPktLoopbackCsyWriteL(TInt aPortNum)
    {
    TInt dummy(0);
    TInt ret = RProperty::Get(KUidPSCsyWriteResultCategory, aPortNum, dummy);
    if(ret != KErrNotFound)
        {
        RProperty::Set(KUidPSCsyWriteResultCategory, aPortNum, KErrNone);
        }
    INFO_PRINTF2(_L("Reset to KErrNone for the next write on [PKTLOOPBACK::%d] port"), aPortNum);
    }

/**
Blocks until Nifman progress notification is received.


@param aReqStatus Request status to wait on for the notification
@param aProgress Nifman progress notification code, for sanity check. This is the progress that was subscribed for.
@param aErrCode expected progress error code.
@leave if the error code received is not equal to the expected.
*/
void CSpudNetSideTestBase::WaitForProgressNotificationL(TRequestStatus& aReqStatus, TInt aProgress, TInt aErrCode)
	{
	INFO_PRINTF3(_L("Waiting for Nifman Progress: stage[%d] with error[%d]"), aProgress, aErrCode);
	User::WaitForRequest(aReqStatus);
	if(KErrNone != aReqStatus.Int())
		{
		INFO_PRINTF2(_L("Unexpected error on progress notification: [%d]"), aReqStatus.Int());
		User::Leave(aReqStatus.Int());
		}
	
	INFO_PRINTF3(_L("Received Progress [%d], error[%d]"), iProgressBuf().iStage, iProgressBuf().iError);
		
	if(aErrCode != iProgressBuf().iError)
		{
		User::Leave(KErrArgument);
		}
	
	ASSERT(aProgress == iProgressBuf().iStage); // Error in Nifman Progress Notification mechanism
	}
	
/**
Retrieves the PDP context type (EPrimaryContext or ESecondaryContext) from the configuration file

@param aContextTypeName the name of the context to retrieve the type for
@return the type of fhe context
@leave if this setting is not in the configuration file, or if the type is unrecognized 
*/	
TContextType CSpudNetSideTestBase::RetrieveContextTypeFromConfigL(const TDesC& aContextTypeName)
	{
	TPtrC eventStr;
	if(!GetStringFromConfig(ConfigSection(), aContextTypeName, eventStr))
		{
		INFO_PRINTF2(_L("Context config type[%S] not in config. Leaving with KErrNotFound"), &aContextTypeName);
		User::Leave(KErrNotFound);
		}
	
	INFO_PRINTF3(_L("Context type is [%S] for request [%S]"), &eventStr, &aContextTypeName);	
		
	// convert the enum
	TContextType contextType(EPrimaryContext);
	if(eventStr.Compare(TPtrC(_L("EPrimaryContext"))) == 0)
		{
		contextType =  EPrimaryContext;
		}
	else if(eventStr.Compare(TPtrC(_L("ESecondaryContext"))) == 0)
		{
		contextType =  ESecondaryContext;
		}
	else
		{
		INFO_PRINTF2(_L("Context type [%S] does not match 'EPrimaryContext' or 'ESecondaryContext'"), &eventStr);
		User::Leave(KErrCorrupt);
		}

	return contextType;
	}
	
/**
Retrieves the COMM role (DTE / DCE) from config

@param aCommRoleName the name of the parameter to retrieve the role for
@return the comm role
@leave the setting is not found or if the role is unrecognized 
*/	
TCommRole CSpudNetSideTestBase::RetrieveCommRoleFromConfigL(const TDesC& aCommRoleName)
	{
	TPtrC roleStr;
	if(!GetStringFromConfig(ConfigSection(), aCommRoleName, roleStr))
		{
		INFO_PRINTF2(_L("Comm role[%S] not in config. Leaving with KErrNotFound"), &aCommRoleName);
		User::Leave(KErrNotFound);
		}
	
	INFO_PRINTF3(_L("Comm role is [%S] for request [%S]"), &roleStr, &aCommRoleName);
		
	// convert the enum
	TCommRole commRole(ECommRoleDTE);
	if(roleStr.Compare(TPtrC(_L("ECommRoleDTE"))) == 0)
		{
		commRole =  ECommRoleDTE;
		}
	else if(roleStr.Compare(TPtrC(_L("ECommRoleDCE"))) == 0)
		{
		commRole =  ECommRoleDCE;
		}
	else
		{
		INFO_PRINTF2(_L("Comm role [%S] does not match 'ECommRoleDTE' or 'ECommRoleDCE'"), &roleStr);
		User::Leave(KErrCorrupt);
		}

	return commRole;
	}

/**
Retrieves the value of the network side event from the configuration file

@param the name of the even parameter
@return the network side event value (actually an index in publish-subscribe section in the SIM.TSY config.txt)	
@leave if the setting cannot be found 
*/
TInt CSpudNetSideTestBase::RetrieveNetSideEventValFromConfigL(const TDesC& aEventValName)
	{
	TInt val(0);
	if(!GetIntFromConfig(ConfigSection(), aEventValName, val))
		{
		User::Leave(KErrNotFound);
		}
	return val;
	}
	
/**
Retrieves the type of the network side event (QoS Change, ContextStatus change, etc)

@param aEventName event parameter name
@return the event type
@leave if parameter cannot be found, or if the value is unrecognized 
*/		
TEtelRequestType CSpudNetSideTestBase::RetrieveNetSideEventRequestFromConfigL(const TDesC& aEventName)
	{
	TPtrC eventStr;
	if(!GetStringFromConfig(ConfigSection(), aEventName, eventStr))
		{
		INFO_PRINTF2(_L("NetSide event [%S] not in config. Leaving with KErrNotFound"), &aEventName);	
		User::Leave(KErrNotFound);
		}
	
	INFO_PRINTF3(_L("NetSide event is [%S] for request [%S]"), &eventStr, &aEventName);	
		
	// convert the enum
	TEtelRequestType request(EInvalidRequest);
	if(eventStr.Compare(TPtrC(_L("ENetworkQoSChange"))) == 0)
		{
		request =  ENetworkQoSChange;
		}
	else if(eventStr.Compare(TPtrC(_L("ENetworkChangeRegStatus"))) == 0)
		{
		request =  ENetworkChangeRegStatus;
		}
	else if(eventStr.Compare(TPtrC(_L("EContextStatusChange"))) == 0)
		{
		request =  EContextStatusChange;
		}
	else
		{
		ERR_PRINTF2(_L("Unknown event [%S] requested. Leaving with KErrArgument"), &eventStr);
		User::Leave(KErrArgument);
		}
	return request;
	}

/**
 Sets the section for the simtsy to use in its config.txt file.
 The section that will be used is testX, where X is the parameter aTestNumber
 */
void CSpudNetSideTestBase::SetSimTsyTestNumberL(TInt aTestNumber)
  	{
	TestL(RProperty::Set(KUidPSSimTsyCategory, KPSSimTsyTestNumber, aTestNumber), _L("Set Sim TSY test number"));
  	}

/**
Gets the mandatory settings from the test configuration

@leave if not found 
*/
void CSpudNetSideTestBase::RetrieveMandatorySettingsFromConfigL()
	{
	if(!GetIntFromConfig(ConfigSection(), _L("SimTsyTestNum"), iSimTsyTestNum))
		{
		ERR_PRINTF1(_L("Failed to read mandatory settings (SimTsyTestNum, etc) from the config file."));
		User::Leave(KErrNotFound);
		}
	}

#ifdef SYMBIAN_NON_SEAMLESS_NETWORK_BEARER_MOBILITY
/**
Read Qos Parameter Settings from config

@leave if not found 
*/
void CSpudNetSideTestBase::ReadQosParameterSettingsFromConfigL()
	{
	_LIT(KPolicySelector, "PolicySelector");
	_LIT(KParametersType, "ParametersType");
	_LIT(KGenericQosRecord, "GenericQosRecord");
	_LIT(KUmtsR99QoSAndOnTableRecord, "UmtsR99QoSAndOnTableRecord");
	_LIT(KParametersRecordId, "ParametersRecordId");
	
	TPtrC parameterType;
	TInt record;
	
	if(!GetStringFromConfig(ConfigSection(), KParametersType, parameterType))
		{
		ERR_PRINTF1(_L("Failed to read mandatory settings (ParametersType, etc) from the config file."));
		User::Leave(KErrNotFound);
		}

	if(!GetIntFromConfig(ConfigSection(), KParametersRecordId, record))
		{
		ERR_PRINTF1(_L("Failed to read mandatory settings (ParametersRecord, etc) from the config file."));
		User::Leave(KErrNotFound);
		}

	if(!GetIntFromConfig(ConfigSection(), KPolicySelector, iPolicySelector))
		{
		ERR_PRINTF1(_L("Failed to read mandatory settings (PolicySelector, etc) from the config file."));
		User::Leave(KErrNotFound);
		}

	if (parameterType.Compare(KGenericQosRecord()) == 0)
		{
		iQosParametersRecord = KCDTIdGenericQosRecord | record;
		}
	else if (parameterType.Compare(KUmtsR99QoSAndOnTableRecord()) == 0)
		{
		iQosParametersRecord = KCDTIdUmtsR99QoSAndOnTableRecord | record;
		}
	else
		{
		ERR_PRINTF2(_L("Invalid value for ParametersType. %S"), &parameterType);
		User::Leave(KErrArgument);
		}
	}
#endif


/**
 Implements the pure virtual doTestStepL defined in CTestStep. 
 Allows the base class to execute before any of the derived 
 tests is called.
 
 @leave If any of the called methods leaves.
 */
TVerdict CSpudNetSideTestBase::doTestStepL()
	{
	// Initialize the mandatory objects, without which there is not point in running the tests.
	RetrieveMandatorySettingsFromConfigL();	
		
	TRAPD(esockConnErr, TestL(iEsock.Connect(), _L("RSockeServ::Connect")));
	if(KErrNone != esockConnErr)
		{
		INFO_PRINTF1(_L("ESock thread is dead. Most likely, it was crashed by the previous test case. Check the Root Server logs."));
		SetTestStepResult(EInconclusive);
		return TestStepResult();
		}
	
	// Sim.Tsy test section to use with the test
	INFO_PRINTF2(_L("Sim TSY test number: [%d]"), iSimTsyTestNum);
	SetSimTsyTestNumberL(iSimTsyTestNum);
	
	ASSERT(!CActiveScheduler::Current()); // We should not have an AS at this point.
	CActiveScheduler* testSched = new(ELeave) CActiveScheduler;
	CleanupStack::PushL(testSched);
	CActiveScheduler::Install(testSched);
	
	
	// Test sequence itself:
	TVerdict testResult = EFail;		
	TRAPD(err, testResult = RunTestStepL());
	if(KErrNone == err)
		{
		INFO_PRINTF1(_L("Test Step Completion."));
		SetTestStepResult(testResult);
		}
	else
		{
		ERR_PRINTF2(_L("Test Step Failure: the step left with [%d]"), err);
		SetTestStepResult(EFail);
		}
		

	// It's OK to close these more than once.
	iLoopbackPort.Close();
	iCommServer.Close();

#ifndef SYMBIAN_NON_SEAMLESS_NETWORK_BEARER_MOBILITY
	iQoSPolicy_1.Close();
#endif
	iSocketJoin.Close();

	iSocket.Close();
	iInterface.Close();
	
	iEsock.Close();
	
	CActiveScheduler::Install(NULL); // uninstall the test scheduler
	CleanupStack::PopAndDestroy(testSched);
	
	return TestStepResult();	
	}

#ifndef SYMBIAN_NON_SEAMLESS_NETWORK_BEARER_MOBILITY
/**
Blocks until a QoS event is received

@param aQosEventString the name of config parameter holding the expected event
@param aReasonCodeString the name of the config parameter holding the expected reason
@leave if the received event is not what was expected 
*/
void CSpudNetSideTestBase::WaitForQoSEventL(const TDesC& aQosEventString, const TDesC& aReasonCodeString)
	{

	TQoSEvent expQosEvent = RetrieveQosEventFromConfigL(aQosEventString);
	if(expQosEvent == EQoSEventFailure || expQosEvent == EQoSEventAdapt || expQosEvent == EQoSEventJoin)
		{
		TInt reason = RetrieveNetSideEventValFromConfigL(aReasonCodeString);
		WaitForQoSEventL(expQosEvent,reason);
		}
	else
		{
		WaitForQoSEventL(expQosEvent,KErrNone);
		}
	}

/**
Blocks until a QoS event is received

@param aQosEventString the name of config parameter holding the expecting event
@param aReasonCode the expected reason
@leave if the received event is not what was expected 
*/
void CSpudNetSideTestBase::WaitForQoSEventL(const TDesC& aQosEventString, TInt aReasonCode)
	{

	TQoSEvent expQosEvent = RetrieveQosEventFromConfigL(aQosEventString);
	WaitForQoSEventL(expQosEvent,aReasonCode);
	}

/**
Blocks until a QoS event is received

@param aQosEventString the expected event
@param aReasonCode the expected reason
@leave if the received event is not what was expected 
*/
void CSpudNetSideTestBase::WaitForQoSEventL(TQoSEvent aQosEvent, TInt aReasonCode)
	{
	INFO_PRINTF1(_L("Waiting for QoS Event: starting ActiveScheduler"));
	CActiveScheduler::Start();
	// We have received QoS event at this point. 
	TPtrC expectedEvent(QoSEventToText(aQosEvent));
	TPtrC receivedEvent(QoSEventToText(iReceivedQosEvent));
	if(aQosEvent == EQoSEventFailure || aQosEvent == EQoSEventAdapt || aQosEvent == EQoSEventJoin)
		{
		INFO_PRINTF5(_L("Qos Event [%S] and Reason [%d] expected, [%S] and Reason [%d] received"), &expectedEvent,aReasonCode, &receivedEvent,iQosReason );
		if(aQosEvent == iReceivedQosEvent && iQosReason == aReasonCode)	
			{
			INFO_PRINTF1(_L("OK. correct event and reason received."));
			}
		else
			{
			INFO_PRINTF1(_L("Failure: unexpected event or reason"));
			User::Leave(KErrCorrupt);
			}
		}
	else
		{
		INFO_PRINTF3(_L("Qos Event [%S] expected, [%S] received"), &expectedEvent, &receivedEvent);
		if(aQosEvent == iReceivedQosEvent)	
			{
			INFO_PRINTF1(_L("OK. correct event received."));
			}
		else
			{
			INFO_PRINTF1(_L("Failure: unexpected event"));
			User::Leave(KErrCorrupt);
			}
		}
	}

/**
 Wrapper for WaitForOptionalQoSEventL(TQoSEvent aQosEvent, TInt aReasonCode, TQoSEvent aOptionalQosEvent, TInt aOptionalReasonCode)
*/
void CSpudNetSideTestBase::WaitForOptionalQoSEventL(const TDesC& aQosEventString, const TDesC& aReasonCodeString, const TDesC& aOptionalQosEventString, const TDesC& aOptionalReasonCodeString)
	{
	TQoSEvent optionalEvent = (TQoSEvent)0;
	TRAPD(err, optionalEvent = RetrieveQosEventFromConfigL(aOptionalQosEventString));
	if (KErrArgument == err)
		{
		// Could not find any optional events
		WaitForQoSEventL(aQosEventString,aReasonCodeString);
		}
	else
		{
		// found an optional event
		WaitForOptionalQoSEventL(aQosEventString,aReasonCodeString,optionalEvent,aOptionalReasonCodeString);
		}
	}
	
/**
 Wrapper for WaitForOptionalQoSEventL(TQoSEvent aQosEvent, TInt aReasonCode, TQoSEvent aOptionalQosEvent, TInt aOptionalReasonCode)
*/
void CSpudNetSideTestBase::WaitForOptionalQoSEventL(const TDesC& aQosEventString, const TDesC& aReasonCodeString, TQoSEvent aOptionalQosEvent, const TDesC& aOptionalReasonCodeString)
	{
	TQoSEvent expQosEvent = RetrieveQosEventFromConfigL(aQosEventString);
	TInt expQosReason = KErrNone;
	TInt optionalExpQosReason = KErrNone;
	if(expQosEvent == EQoSEventFailure || expQosEvent == EQoSEventAdapt || expQosEvent == EQoSEventJoin)
		{
		expQosReason = RetrieveNetSideEventValFromConfigL(aReasonCodeString);
		}
	if(aOptionalQosEvent == EQoSEventFailure || aOptionalQosEvent == EQoSEventAdapt || aOptionalQosEvent == EQoSEventJoin)
		{
		optionalExpQosReason = RetrieveNetSideEventValFromConfigL(aOptionalReasonCodeString);
		}
	WaitForOptionalQoSEventL(expQosEvent, expQosReason, aOptionalQosEvent, optionalExpQosReason);
	}
	
/**
Blocks until a QoS event is received

@param aQosEventString the expected event
@param aReasonCode the expected reason
@leave if the received event is not what was expected 
*/
void CSpudNetSideTestBase::WaitForOptionalQoSEventL(TQoSEvent aQosEvent, TInt aReasonCode, TQoSEvent aOptionalQosEvent, TInt aOptionalReasonCode)
	{
	INFO_PRINTF1(_L("Waiting for optional QoS Event: starting ActiveScheduler"));
	CActiveScheduler::Start();
	// We have received QoS event at this point. 
	TPtrC expectedEvent(QoSEventToText(aQosEvent));
	TPtrC receivedEvent(QoSEventToText(iReceivedQosEvent));
	if(iReceivedQosEvent == EQoSEventFailure || iReceivedQosEvent == EQoSEventAdapt || iReceivedQosEvent == EQoSEventJoin)
		{
		INFO_PRINTF5(_L("Qos Event [%S] and Reason [%d] expected, [%S] and Reason [%d] received"), &expectedEvent,aReasonCode, &receivedEvent,iQosReason );
		if(aQosEvent == iReceivedQosEvent && iQosReason == aReasonCode)
			{
			INFO_PRINTF1(_L("OK. correct event and reason received."));
			return;
			}
		else if (aOptionalQosEvent != iReceivedQosEvent || iQosReason != aOptionalReasonCode)
			{
			INFO_PRINTF1(_L("Failure: unexpected event or reason"));
			User::Leave(KErrCorrupt);
			}
		INFO_PRINTF1(_L("OK. optional event and reason received."));
		// fall through and wait for non-optional event
		}
	else
		{
		INFO_PRINTF3(_L("Qos Event [%S] expected, [%S] received"), &expectedEvent, &receivedEvent);
		if(aQosEvent == iReceivedQosEvent)	
			{
			INFO_PRINTF1(_L("OK. correct event received."));
			return;
			}
		else if (aOptionalQosEvent != iReceivedQosEvent)
			{
			INFO_PRINTF1(_L("Failure: unexpected event"));
			User::Leave(KErrCorrupt);
			}
		// fall through and wait for non-optional event
		}

	INFO_PRINTF1(_L("Waiting for second QoS Event: starting ActiveScheduler"));
	CActiveScheduler::Start();
	// We have received QoS event at this point. 
	if(aQosEvent == EQoSEventFailure || aQosEvent == EQoSEventAdapt || aQosEvent == EQoSEventJoin)
		{
		INFO_PRINTF5(_L("Qos Event [%S] and Reason [%d] expected, [%S] and Reason [%d] received"), &expectedEvent,aReasonCode, &receivedEvent,iQosReason );
		if(aQosEvent == iReceivedQosEvent && iQosReason == aReasonCode)	
			{
			INFO_PRINTF1(_L("OK. correct event and reason received."));
			}
		else
			{
			INFO_PRINTF1(_L("Failure: unexpected event or reason"));
			User::Leave(KErrCorrupt);
			}
		}
	else
		{
		INFO_PRINTF3(_L("Qos Event [%S] expected, [%S] received"), &expectedEvent, &receivedEvent);
		if(aQosEvent == iReceivedQosEvent)	
			{
			INFO_PRINTF1(_L("OK. correct event received."));
			}
		else
			{
			INFO_PRINTF1(_L("Failure: unexpected event"));
			User::Leave(KErrCorrupt);
			}
		}
	}

/**
Upcall from the QoS framework on reception of a QoS event
*/
void CSpudNetSideTestBase::Event(const CQoSEventBase& aEvent)
	{
	CActiveScheduler::Stop();
	
	INFO_PRINTF1(_L("QoS Event received: stopping ActiveScheduler"));
	iReceivedQosEvent = (TQoSEvent)aEvent.EventType();
	switch(aEvent.EventType())
		{
			case EQoSEventFailure:
			{
		        iQosReason = (static_cast<const CQoSFailureEvent*>(&aEvent))->Reason();
		 
		        INFO_PRINTF2(_L("Receive CQoSFailureEvent reason: %d"),iQosReason );
				break;
			}
			
			case EQoSEventAdapt:
			{
		        iQosReason = (static_cast<const CQoSAdaptEvent*>(&aEvent))->Reason();
		 
		        INFO_PRINTF2(_L("Receive CQoSAdaptEvent reason: %d"),iQosReason );
				break;
			}
			
			case EQoSEventJoin:
			{
		        iQosReason = (static_cast<const CQoSJoinEvent*>(&aEvent))->Reason();
		 
		        INFO_PRINTF2(_L("Receive CQoSJoinEvent reason: %d"),iQosReason );
				break;
			}
			
			default:
				break;	 	 
		}
	}
#endif // SYMBIAN_NON_SEAMLESS_NETWORK_BEARER_MOBILITY

/**
Logs a message and leaves on error 

@param aErrCode error code to check
@param aMsg message to log 
@leave if aError is other than KErrNone
*/
void CSpudNetSideTestBase::TestL(TInt aErrCode, const TDesC& aMsg)
	{	
	TestL(aErrCode, KErrNone, aMsg);
	}

/**
Used to verify that an error code is what expected, and log the associated comment

@param aErrCode the error code to check
@param aExpErrCode the expected error code
@param aMsg the message to log before testing
@leave if aErrCode != aExpErrCode
*/	
void CSpudNetSideTestBase::TestL(TInt aErrCode, TInt aExpErrCode, const TDesC& aMsg)
	{	
	if(aExpErrCode == aErrCode)
		{
		INFO_PRINTF3(_L("[%S]. err[%d], as expected. OK."), &aMsg, aErrCode);
		}
	else
		{
		ERR_PRINTF5(_L("Failed: [%S]. err[%d], expected [%d]. Leaving with [%d])."), &aMsg, aErrCode, aExpErrCode, aErrCode);
		User::Leave(aErrCode);
		}
	}

/**
Used to verify that a boolean is as expected, and log the associated comment

@param aBool the boolean to check
@param aMsg the message to log before testing
@leave if aBool == EFalse
*/	
void CSpudNetSideTestBase::TestBooleanTrueL(TBool aBool, const TDesC& aMsg)
	{
	if(aBool)
		{
		INFO_PRINTF2(_L("[%S]. Value is true, as expected. OK."), &aMsg);
		}
	else
		{
		ERR_PRINTF2(_L("Failed: [%S]. Value is false. Leaving with KErrGeneral)."), &aMsg);
		User::Leave(KErrGeneral);
		}
	}
	
/**
Starts only the primary PDP context by explicitly starting a network interface.

@leave if the interface start error is not what is expected. 
*/
void CSpudNetSideTestBase::StartPrimaryOnlyL()
	{
	TInt primaryCreationErr(KErrNone);
	TInt primaryIapId(0);
	if(!GetIntFromConfig(ConfigSection(), _L("PrimaryIapId1"), primaryIapId) ||
	   !GetIntFromConfig(ConfigSection(), _L("PrimaryActivationErr1"), primaryCreationErr))
		{
		User::Leave(KErrNotFound);
		}
	
	TCommDbConnPref iap1prefs;
    iap1prefs.SetIapId(primaryIapId);	
	TestL(iInterface.Open(iEsock), _L("RConnection::Open on interface"));
	TestL(iInterface.Start(iap1prefs),primaryCreationErr, _L("RConnection::Start on interface"));
	}

/**
Opens a socket, binds it, connects it to the destination.
N.B. This socket is NOT explicitly bound to the primary PDP context

@leave if the socket cannot be opened.
*/
void CSpudNetSideTestBase::OpenPrimarySocketL()
	{
	TestL(iSocket.Open(iEsock, KAfInet, KSockDatagram, KProtocolInetUdp), _L("RSocket::Open"));
	TInetAddr localAddr;
	localAddr.SetPort(KConfiguredTftFilter1DestPort);
	TestL(iSocket.Bind(localAddr), _L("Binding the local Socket"));
	
	
	TInetAddr dstAddr;
	dstAddr.SetPort(KConfiguredTftFilter1SrcPort);
	dstAddr.Input(KConfiguredTftFilter1SrcAddr);
	
	TRequestStatus status;
	iSocket.Connect(dstAddr, status);
	User::WaitForRequest(status);
	TestL(status.Int(), _L("RSocket::Connect status opening primary socket"));
	}

/**
Causes GUQoS to activate a secondary PDP context

@leave if the secondary cannot be activated 
*/
void CSpudNetSideTestBase::StartSecondaryL()
	{
#ifdef SYMBIAN_NON_SEAMLESS_NETWORK_BEARER_MOBILITY
	// get IAP no. from config, added for R4 and R5 tests,
	// If there isn't an IAP number in the .ini file, default to PDPIAP
	TInt iap;
	if(!GetIntFromConfig(ConfigSection(), _L("PrimaryIapId1"), iap))
			{
			iap = PDPIAP;
			}

	VerifySubconnectionCountL(_L("SubConnectionCount1"), iap);

	ReadQosParameterSettingsFromConfigL();
#endif
	
	InitiateSecondaryStartL();

#ifndef SYMBIAN_NON_SEAMLESS_NETWORK_BEARER_MOBILITY
	WaitForQoSEventL(_L("SecondaryActivationEvent2"),_L("SecondaryActivationEvent2Reason"));
#else
	User::After(KTimeToStartSecondary*4);
	VerifySubconnectionCountL(_L("SubConnectionCount2"), iap);
#endif
	}

#ifdef SYMBIAN_NON_SEAMLESS_NETWORK_BEARER_MOBILITY
void CSpudNetSideTestBase::VerifySubconnectionCountL(const TDesC& aSubConnDesc, TUint aIap)
	{
	TInt count;
	if(!GetIntFromConfig(ConfigSection(), aSubConnDesc, count))
		{
		ERR_PRINTF2(_L("Failed to subconnection count (%S) from the config file."), &aSubConnDesc);
		User::Leave(KErrNotFound);
		}
	VerifySubconnectionCountL(count, aIap);
	}

void CSpudNetSideTestBase::VerifySubconnectionCountL(TUint aSubConnectionCount, TUint aIap)
	{
	TUint count;

	RConnection conn;

	User::LeaveIfError(conn.Open(iEsock));
	CleanupClosePushL(conn);
	
	User::LeaveIfError(conn.EnumerateConnections(count));
	if (count)
	    {
	    TConnectionInfo tconninfo;
	    TPckg<TConnectionInfo> conninfo(tconninfo);
	    
        while(count>0)
            {            
    	    User::LeaveIfError(conn.GetConnectionInfo(count, conninfo));
    	    if (tconninfo.iIapId == aIap)
        	    {
        	    break;   
        	    }
        	    
        	count--;
            }
            
        if (count)
            {
    	    User::LeaveIfError(conn.Attach(conninfo, RConnection::EAttachTypeMonitor));
    	    User::After(5000);
    	    User::LeaveIfError(conn.EnumerateSubConnections(count));
            }
	    }
	CleanupStack::PopAndDestroy(&conn);
	
	// if count is 0, then there are 0 connection and therefore 0 subconnections
	INFO_PRINTF3(_L("Expected Subconnection count [%d]  Actual Subconnection count [%d]"), aSubConnectionCount, count);
	TestL(count == aSubConnectionCount, ETrue, _L("Comparing subconnection count."));
	}

void CSpudNetSideTestBase::ClearPolicySelector2QosParametersTableL()
	{
	CommsDat::CMDBSession* dbSession;
#ifdef SYMBIAN_NON_SEAMLESS_NETWORK_BEARER_MOBILITY
	dbSession = CMDBSession::NewL(KCDVersion1_2);
#else
	dbSession = CMDBSession::NewL(KCDVersion1_1);
#endif
	CleanupStack::PushL(dbSession);

	/* delete all records for this policy selector */
	CMDBRecordSet<CCDPolicySelector2ParamsRecord>* records =
		new(ELeave) CMDBRecordSet<CCDPolicySelector2ParamsRecord>(KCDTIdPolicySelector2ParamsRecord);
	CleanupStack::PushL(records);
	
	TRAPD(err, records->LoadL(*dbSession));
	if (err == KErrNone)
		records->DeleteL(*dbSession);
	CleanupStack::PopAndDestroy(records);
	
	CleanupStack::PopAndDestroy(dbSession);
	}

void CSpudNetSideTestBase::UpdatePolicySelector2QosParametersTableL(TUint aPolicySelector, TUint aQosParameter)
	{
	ClearPolicySelector2QosParametersTableL();
		
	CommsDat::CMDBSession* dbSession;
#ifdef SYMBIAN_NON_SEAMLESS_NETWORK_BEARER_MOBILITY
	dbSession = CMDBSession::NewL(KCDVersion1_2);
#else
	dbSession = CMDBSession::NewL(KCDVersion1_1);
#endif

	CleanupStack::PushL(dbSession);
	
	CCDPolicySelector2ParamsRecord* newrec =
		static_cast<CCDPolicySelector2ParamsRecord*>(CCDRecordBase::RecordFactoryL(KCDTIdPolicySelector2ParamsRecord));
	CleanupStack::PushL(newrec);
	newrec->SetRecordId(KCDNewRecordRequest);
	newrec->iPolicySelectorId = aPolicySelector;
	newrec->iParamsId = aQosParameter;

	newrec->StoreL(*dbSession);
	CleanupStack::PopAndDestroy(newrec);

	CleanupStack::PopAndDestroy(dbSession);
	}
#endif

/**
Initiates the start (creation & activation) of a secondary PDP context, but does not wait/verify that the context
was actually created / activated 

@leave if the start cannot be initiated: a socket cannot be opened, etc
*/
void CSpudNetSideTestBase::InitiateSecondaryStartL()
	{
#ifdef SYMBIAN_NON_SEAMLESS_NETWORK_BEARER_MOBILITY
	ReadQosParameterSettingsFromConfigL();

	UpdatePolicySelector2QosParametersTableL(iPolicySelector, iQosParametersRecord);
#endif
	
	TestL(iSocket.Open(iEsock, KAfInet, KSockDatagram, KProtocolInetUdp), _L("RSocket::Open"));
	TInetAddr localAddr;
	localAddr.SetPort(KConfiguredTftFilter1DestPort);
	TestL(iSocket.Bind(localAddr), _L("Binding the local Socket"));
	
	TInetAddr dstAddr;
	dstAddr.SetPort(KConfiguredTftFilter1SrcPort);
	dstAddr.Input(KConfiguredTftFilter1SrcAddr);

	TRequestStatus status;
	iSocket.Connect(dstAddr, status);
	User::WaitForRequest(status);
	TestL(status.Int(), _L("Connecting to local socket"));

#ifndef SYMBIAN_NON_SEAMLESS_NETWORK_BEARER_MOBILITY
	TQoSSelector selector;
	selector.SetAddr(iSocket);
	TestL(iQoSPolicy_1.Open(selector), _L("Opening Policy"));
	TestL(iQoSPolicy_1.NotifyEvent(*this), _L("Requesting Event notification on QosChannel"));
	
	CQoSParameters *parameters = new(ELeave) CQoSParameters;
	CleanupStack::PushL(parameters);
	
	SetQoSParameters(*parameters);
	
	TestL(iQoSPolicy_1.SetQoS(*parameters), _L("Setting Qos Parameters"));
	
	WaitForQoSEventL(_L("SecondaryActivationEvent1"),KErrNone);

	CleanupStack::PopAndDestroy(parameters);
#endif
	}
	
	

#ifndef SYMBIAN_NON_SEAMLESS_NETWORK_BEARER_MOBILITY
//This method must be called after StartSecondaryL() is called.
void CSpudNetSideTestBase::ChangeQoSL()
	{

	CQoSParameters *parameters = new(ELeave) CQoSParameters;
	CleanupStack::PushL(parameters);
	 
	SetQoSParameters1(*parameters);
		
	TestL(iQoSPolicy_1.SetQoS(*parameters), _L("Setting Qos Parameters"));

	WaitForOptionalQoSEventL(_L("ExpQoSEvent1"), _L("ExpQoSEvent1Reason"), _L("OptionalQoSEvent1"), _L("OptionalQoSEvent1Reason"));
	
	WaitForQoSEventL(_L("ExpQoSEvent2"),_L("ExpQoSEvent2Reason"));
	
	CleanupStack::PopAndDestroy(parameters);
	}
#endif	

	
/**
Initialize a port on a loopback CSY (of some sort)

@leave if the port cannot be opened
*/	
void CSpudNetSideTestBase::InitLoopbackL(const TDesC& aLoopbackPortName)
	{
	TestL(iCommServer.Connect(), _L("Connecting to Comm server"));
	TestL(iCommServer.LoadCommModule(KCsyName), _L("Loading CSY"));

	TCommRole commRole = RetrieveCommRoleFromConfigL(_L("CommRole"));
	TestL(iLoopbackPort.Open(iCommServer, aLoopbackPortName, ECommExclusive, commRole), _L("Opening loopback port"));
	}
	
void CSpudNetSideTestBase::SendUntilTimeoutL()
	{
	TBuf8<KCommReadBufSize> commReadBuf;
	ASSERT(iLoopbackPort.SubSessionHandle());

	TRequestStatus sendStatus, readStatus;
	TInt maxIterations = 10;
	// we try sending a packet a number of times, because the first packet may be sent
	// before Etel has notified the SPUD of a change in context status
	do
		{
		// small delay to allow Etel to notify SPUD of change in status
		User::After(800000);
		iSocket.Send(KCommWriteData, 0, sendStatus);
		User::WaitForRequest(sendStatus);
		TestL(sendStatus.Int(), _L("Socket send status"));

		// ensure the packet was sent by reading from the loopback COMM port
		// opposite the current context's COMM port
		iLoopbackPort.Read(readStatus, KReadDelay, commReadBuf);
		User::WaitForRequest(readStatus);
		maxIterations--;
		} while ( (readStatus == KErrNone) && (maxIterations > 0) );
	
	TestL(readStatus.Int(), KErrCancel, _L("Send timed-out"));
	}
	
void CSpudNetSideTestBase::SendSinglePacketL()
	{
	TBuf8<KCommReadBufSize> commReadBuf;
	ASSERT(iLoopbackPort.SubSessionHandle());

	TRequestStatus sendStatus, readStatus;
	iSocket.Send(KCommWriteData, 0, sendStatus);
	User::WaitForRequest(sendStatus);
	TestL(sendStatus.Int(), _L("Primary socket send status"));
	
	// ensure the packet was sent by reading from the loopback COMM port
	// opposite the current context's COMM port
	iLoopbackPort.Read(readStatus, KReadDelay, commReadBuf);
	User::WaitForRequest(readStatus);

	TestL(readStatus.Int(), _L("Send status"));
	// make sure the received length is greater than the sent length
	TInt sentLength = ((TDesC8)KCommWriteData).Length();
	TestL((commReadBuf.Length() >= sentLength) ? KErrNone : KErrCorrupt, _L("Received length is greater than sent length"));
	}
	
void CSpudNetSideTestBase::StopPrimary()
	{
	iLoopbackPort.Close();
	iCommServer.Close();
	iSocket.Close();
	}

/**
Stops the secondary PDP context. 
After this method returns, GUQoS has initiated its timeout before actually deleting the context. 
That is, when this method returns, the context itself is still active 

@leave if the procedure cannot be completed successfully.
*/
void CSpudNetSideTestBase::StopSecondaryL()
	{
	iLoopbackPort.Close();
	iCommServer.Close();

#ifndef SYMBIAN_NON_SEAMLESS_NETWORK_BEARER_MOBILITY
	TestL(iQoSPolicy_1.CancelNotifyEvent(*this), _L("Cancelling QoS Notify"));
	TestL(iQoSPolicy_1.Close(), _L("Closing QoS Channel"));
#endif
	iSocket.Close();
	}
	

/**
Retrieves a QoS event from the config file, based on the parameter name

@param aQosEventReq the name of the parameter
@leave if the event cannot be retrieved, or is unrecognized.
*/
#ifndef SYMBIAN_NON_SEAMLESS_NETWORK_BEARER_MOBILITY
enum TQoSEvent CSpudNetSideTestBase::RetrieveQosEventFromConfigL(const TDesC& aQosEventReq)
	{
	TPtrC qosEventName;
	if(!GetStringFromConfig(ConfigSection(), aQosEventReq, qosEventName))
		{
		ERR_PRINTF2(_L("Failed to read [%S] from the config file."), &aQosEventReq);
		User::Leave(KErrNotFound);
		}
		
	if(qosEventName.Compare(TPtrC(_L("EQoSEventFailure"))) == 0)
		{
		return EQoSEventFailure;
		}
	else if(qosEventName.Compare(TPtrC(_L("EQoSEventConfirm"))) == 0)
		{
		return EQoSEventConfirm;
		}
	else if(qosEventName.Compare(TPtrC(_L("EQoSEventAdapt"))) == 0)
		{
		return EQoSEventAdapt;
		}
	else if(qosEventName.Compare(TPtrC(_L("EQoSEventJoin"))) == 0)
		{
		return EQoSEventJoin;
		}
	else if(qosEventName.Compare(TPtrC(_L("EQoSEventAddPolicy"))) == 0)
		{
		return EQoSEventAddPolicy;
		}
	else
		{
		ERR_PRINTF2(_L("Unrecognized event[%S] requested. Leaving with KErrArgument."), &qosEventName);
		User::Leave(KErrArgument);
		}
	return EQoSEventConfirm;
	};
#endif

/**
 Must be called after simtsy has been initialized.
 Instructs the simtsy to complete a given request/notification. The supported commands are given in TEtelRequestType.
 The parameter supplied in aNewValue instructs the simtsy what data from the config.txt to use in the completion of the request.
 The aNewValue is the index to the entry to be used to complete the request for the current test step section, ie if aNewValue
 is 0, iTestNumber is 3, and aEtelCommand is ENetworkQoSChange, the entry will complete any NotifyNetworkQoSChange calls
 with data from the first QosProfileReqR99 entry of section [test3] in the config.txt file
*/
void CSpudNetSideTestBase::EtelRequestL(TEtelRequestType aEtelCommand, TInt aNewValue)
	{
	INFO_PRINTF3(_L("EtelRequest: aEtelCommand = %d, aNewValue = %d"), aEtelCommand, aNewValue);

	TUint key(0);
	
	switch (aEtelCommand)
		{
	case ENetworkQoSChange:
		key = KPSSimTsyNetworkQoSChange;
		break;
	case ENetworkChangeRegStatus:
		key = KPSSimtsyPacketServiceNtwkRegStatusChange;
		break;
	case EContextStatusChange:
		key = KPSSimtsyPacketContextStatusChange;
		break;
	default:
		ASSERT(EFalse);		
		}

  	// simtsy will listen for any changes to the property, and complete the corresponding request
	TestL(RProperty::Set(KUidPSSimTsyCategory, key, aNewValue), _L("Request to trigger SimTsy event issued."));
	}

/**
Dummy test step: Used to make sure that ESock server did not crash 

@leave does not leave
*/
enum TVerdict CESockCheck::RunTestStepL()
	{
	INFO_PRINTF1(_L("SUCCESS: Could not detect Esock crash as a result of the previous test."));
	return EPass;
	}

/**
Tests creation of the primary PDP context by explicitly starting the interface 

@leave if the test fails.
*/
enum TVerdict CSpudPrimary::RunTestStepL()
	{
	StartPrimaryOnlyL();	
	iInterface.Stop();
	iInterface.Close();	
	
	return EPass;
	}

/**
Test operation of the primary PDP context when there is a network side event

@leave if the test fails.
*/	
enum TVerdict CSpudPrimaryEvent::RunTestStepL()
	{
	TInt idx				 = RetrieveNetSideEventValFromConfigL(_L("QoSEventReq1Idx"));
	TEtelRequestType request = RetrieveNetSideEventRequestFromConfigL(_L("QoSEventReq1"));
	
	TInt expProgress(0);
	TInt expErrorCode(0);
	if(!GetIntFromConfig(ConfigSection(), _L("ExpectedNifProgress"), expProgress) ||
	   !GetIntFromConfig(ConfigSection(), _L("ExpectedNifError"), expErrorCode))
		{
		User::Leave(KErrNotFound);
		}
	
	StartPrimaryOnlyL();	
	User::After(KTimeToStartSecondary);

	TRequestStatus progressReqSt;	
	iInterface.ProgressNotification(iProgressBuf, progressReqSt, static_cast<TUint>(expProgress));
	
	EtelRequestL(request, idx); // Will fire after we register for notifications	
	WaitForProgressNotificationL(progressReqSt, expProgress, expErrorCode); // We can wait here forever. Set timeout on test step.
	if(KLinkLayerClosed == expProgress) // The Nif is expected to shutdown, so if we are here, then the NIF is down.
		{
		// N.B.
		// At this point the NIF is down, and all the associated data structures in ESock server / Nifman are in 
		// the process of being deleted, or have been deleted already.
		// Calling *anything* on the NIF itself is no longer possible, because there is no NIF.
		// Typically, ESock will react with KErrNotReady(-18) to these attempts.
		// Because of timing issues (ESock thread vs. TestExecute thread), especially on hardware,
		// ESock client-side objects may not be yet aware of that, and return KErrNone.  This makes testing for the 
		// return error codes problematic.
		// In any case, calling Stop() on the NIF is inappropriate. Close() closes the client-side handle,
		// which is OK.
		iInterface.Close();
		}
	else // The interface was not stopped. We should stop it now and verity that is was OK.
		{
		TestL(iInterface.Stop(), _L("Shutting down the interface using RConnection::Stop()"));
		}
	
	return EPass;
	}

/**
Test of the primary PDP context progress. 

@leave if the test fails.
*/
enum TVerdict CSpudPrimaryContextEventProgress::RunTestStepL()
    {    
    StartPrimaryOnlyL();
    /*
     * After opening & starting the connection, the connection goes through various states:
     * Those are KStartingSelection->KFinishedSelection->KPsdStartingConfiguration->KPsdFinishedConfiguration
     * ->KStartingConnection->KConnectionOpen->KLinkLayerOpen.
     * But if DHCP is enabled, then after KConnectionOpen the connection goes through these states.
     * KConfigDaemonLoading->KConfigDaemonLoaded->KConfigDaemonStartingRegistration
     * ->KConfigDaemonFinishedRegistration->KLinkLayerOpen.
    */
    TRequestStatus progressReqSt;   
    iInterface.ProgressNotification(iProgressBuf, progressReqSt);
    
    User::WaitForRequest(progressReqSt);
    TestL(iProgressBuf().iStage, KStartingSelection,  _L("RConnection Stage KStartingSelection"));
    
    iInterface.ProgressNotification(iProgressBuf, progressReqSt);
    User::WaitForRequest(progressReqSt);
    TestL(iProgressBuf().iStage, KFinishedSelection,  _L("RConnection Stage KFinishedSelection"));
    
    iInterface.ProgressNotification(iProgressBuf, progressReqSt);
    User::WaitForRequest(progressReqSt);
    TestL(iProgressBuf().iStage, KPsdStartingConfiguration,  _L("RConnection Stage KPsdStartingConfiguration"));
    
    iInterface.ProgressNotification(iProgressBuf, progressReqSt);
    User::WaitForRequest(progressReqSt);
    TestL(iProgressBuf().iStage, KPsdFinishedConfiguration,  _L("RConnection Stage KPsdFinishedConfiguration"));
    
    iInterface.ProgressNotification(iProgressBuf, progressReqSt);
    User::WaitForRequest(progressReqSt);
    TestL(iProgressBuf().iStage, KPsdStartingActivation,  _L("RConnection Stage KPsdStartingActivation"));
    
    iInterface.ProgressNotification(iProgressBuf, progressReqSt);
    User::WaitForRequest(progressReqSt);
    TestL(iProgressBuf().iStage, KPsdFinishedActivation,  _L("RConnection Stage KPsdFinishedActivation"));
    
    iInterface.ProgressNotification(iProgressBuf, progressReqSt);
    User::WaitForRequest(progressReqSt);
    TestL(iProgressBuf().iStage, KLinkLayerOpen,  _L("RConnection Stage KLinkLayerOpen"));

    iInterface.Stop();

    iInterface.ProgressNotification(iProgressBuf, progressReqSt);
    User::WaitForRequest(progressReqSt);
    TestL(iProgressBuf().iStage, KPsdStartingDeactivation,  _L("RConnection Stage KPsdStartingDeactivation"));

    iInterface.ProgressNotification(iProgressBuf, progressReqSt);
    User::WaitForRequest(progressReqSt);
    TestL(iProgressBuf().iStage, KPsdFinishedDeactivation,  _L("RConnection Stage KPsdFinishedDeactivation"));

    iInterface.ProgressNotification(iProgressBuf, progressReqSt);
    User::WaitForRequest(progressReqSt);
    TestL(iProgressBuf().iStage, KLinkLayerClosed,  _L("RConnection Stage KLinkLayerClosed"));

    iInterface.Close(); 

    return EPass;
    }

/**
Test suspension of a PDP context.

@leave if the test fails.
*/	
enum TVerdict CSpudSuspendContext::RunTestStepL()
	{
	TInt idx1 = RetrieveNetSideEventValFromConfigL(_L("QoSEventReq1Idx"));

	// retrieve whether we are testing a primary or secondary context
	TContextType contextType = RetrieveContextTypeFromConfigL(_L("ContextType"));

	// retrieve the port opposite the context being tested from the loopback configuration file
	TPtrC loopbackPortName;
	if(!GetStringFromConfig(ConfigSection(), _L("LoopbackPort"), loopbackPortName))
		{
		INFO_PRINTF1(_L("NetSide event [LoopbackPort] not in config. Leaving with KErrNotFound"));
		User::Leave(KErrNotFound);
		}
		
	TRequestStatus status;

	if (EPrimaryContext == contextType)
		{
		// Open a primary context specifying the RConnection explicitly so we can also test
		// RConnection::ProgressNotification to ensure KDataTransferTemporarilyBlocked is sent
		// when the primary PDP context is suspended.
		StartPrimaryOnlyL();

		TestL(iSocket.Open(iEsock, KAfInet, KSockDatagram, KProtocolInetUdp, iInterface), _L("RSocket::Open"));
		TInetAddr localAddr;
		localAddr.SetPort(KConfiguredTftFilter1DestPort);
		TestL(iSocket.Bind(localAddr), _L("Binding the local Socket"));
		
		TInetAddr dstAddr;
		dstAddr.SetPort(KConfiguredTftFilter1SrcPort);

		TPtrC ip6Addr;
        if(GetStringFromConfig(ConfigSection(), _L("IP6Addr"), ip6Addr))
            {
            dstAddr.Input(ip6Addr);
            }
        else
            {
            dstAddr.Input(KConfiguredTftFilter1SrcAddr);
            }
		
		iSocket.Connect(dstAddr, status);
		User::WaitForRequest(status);
		TestL(status.Int(), _L("RSocket::Connect status opening primary socket"));
		}
	else if (ESecondaryContext == contextType)
		{
		StartSecondaryL();
		}
	else
		{
		User::Leave(KErrGeneral);
		}
	
	// start loopback port that packets for the current context will be sent to
	InitLoopbackL(loopbackPortName);
	
	// the SPUD may not have created the first context until the first packet is sent
	SendSinglePacketL();
	
	TNifProgressBuf progress;

	// Request a progress notification for a primary context - we should be notified if the PDP context is suspended.
	if (EPrimaryContext == contextType)
		{
		iInterface.ProgressNotification(progress, status);
		}
	
	// trigger suspending the context
	EtelRequestL(EContextStatusChange, idx1);
	
	// ensure that the context does not send packets
	SendUntilTimeoutL();

	// Keep waiting until a KDataTransferTemporarilyBlocked progress notification from a
	// primary context is received or the test step is timed out indicating failure.
	if (EPrimaryContext == contextType)
		{
		User::WaitForRequest( status );
		while( progress().iStage != KDataTransferTemporarilyBlocked )
			{
			iInterface.ProgressNotification(progress, status);
			User::WaitForRequest( status );
			}
		}
	
	TInt idx2 = RetrieveNetSideEventValFromConfigL(_L("QoSEventReq2Idx"));
	
	// trigger resuming the context
	EtelRequestL(EContextStatusChange, idx2);
	
	// ensure we can again send packets
	SendSinglePacketL();
	
	if (EPrimaryContext == contextType)
		{
		iInterface.Stop();
		iInterface.Close();	

		StopPrimary();
		}
	else if (ESecondaryContext == contextType)
		{
		StopSecondaryL();
		}
	
	return EPass;
	}

#ifndef SYMBIAN_NON_SEAMLESS_NETWORK_BEARER_MOBILITY
/**
Test operation of the secondary PDP context when there is a network side event

@leave if the test fails.
*/	
enum TVerdict CSpudSecondaryEvent::RunTestStepL()
	{
	
	TInt idx				 = RetrieveNetSideEventValFromConfigL(_L("QoSEventReq1Idx"));
	TEtelRequestType request = RetrieveNetSideEventRequestFromConfigL(_L("QoSEventReq1"));
	
	StartSecondaryL();  
	
	EtelRequestL(request, idx);

	WaitForOptionalQoSEventL(_L("ExpQoSEvent1"),_L("ExpQoSEvent1Reason"), _L("OptionalQoSEvent1"), _L("OptionalQoSEvent1Reason"));

	StopSecondaryL();	
		
	return EPass;
	}
#endif

#ifndef SYMBIAN_NON_SEAMLESS_NETWORK_BEARER_MOBILITY
/**
Test operation of the secondary PDP context when there are 2 network side events.

@leave if the test fails.
*/	
enum TVerdict CSpudTwoSecondaryEvents::RunTestStepL()
	{
	TInt idx1				  = RetrieveNetSideEventValFromConfigL(_L("QoSEventReq1Idx"));
	TEtelRequestType request1 = RetrieveNetSideEventRequestFromConfigL(_L("QoSEventReq1"));
	
	StartSecondaryL();
	EtelRequestL(request1, idx1);

	WaitForQoSEventL(_L("ExpQoSEvent1"),_L("ExpQoSEvent1Reason"));
	
	TInt idx2				  = RetrieveNetSideEventValFromConfigL(_L("QoSEventReq2Idx"));
	TEtelRequestType request2 = RetrieveNetSideEventRequestFromConfigL(_L("QoSEventReq2"));
	
	EtelRequestL(request2, idx2);

	StopSecondaryL();
	
	return EPass;
	};
#endif

#ifndef SYMBIAN_NON_SEAMLESS_NETWORK_BEARER_MOBILITY
/**
Test operation of the secondary PDP context when there is a network side event, but after
the context was stopped (i.e. it is being deactivated / deleted / etc)

@leave if the test fails.
*/
enum TVerdict CSpudSecondaryEvent2::RunTestStepL()
	{
	TInt idx1				  = RetrieveNetSideEventValFromConfigL(_L("QoSEventReq1Idx"));
	TEtelRequestType request1 = RetrieveNetSideEventRequestFromConfigL(_L("QoSEventReq1"));
	
	StartSecondaryL();
	EtelRequestL(request1, idx1);

	WaitForQoSEventL(_L("ExpQoSEvent1"),_L("ExpQoSEvent1Reason"));

	StopSecondaryL();
	
	TInt idx2				  = RetrieveNetSideEventValFromConfigL(_L("QoSEventReq2Idx"));
	TEtelRequestType request2 = RetrieveNetSideEventRequestFromConfigL(_L("QoSEventReq2"));
	
	StartSecondaryL(); // reopen the context on the same SPUD
	EtelRequestL(request2, idx2);

	WaitForQoSEventL(_L("ExpQoSEvent2"),_L("ExpQoSEvent2Reason"));

	StopSecondaryL();
	
	return EPass;
	};
#endif

/**
Test operation of the secondary PDP context when there are no network-side events.

@leave if the test fails.
*/	
enum TVerdict CSpudSecondary::RunTestStepL()
	{
	StartSecondaryL();
	StopSecondaryL();
	
	return EPass;	
	};
	
#ifndef SYMBIAN_NON_SEAMLESS_NETWORK_BEARER_MOBILITY
/**
Test operation of the secondary PDP context when QoS is changed after the context was activated

@leave if the test fails.
*/
enum TVerdict CSpudSecondaryChangeQoS::RunTestStepL()
	{
	INFO_PRINTF1(_L("CSpudSecondaryChangeQoS test start"));
	
	StartSecondaryL();
	ChangeQoSL();
	StopSecondaryL();
	return EPass;	
	};
#endif // SYMBIAN_NON_SEAMLESS_NETWORK_BEARER_MOBILITY

/**
Test the operation of the primary PDP context when there is an event when the lower NIF for the primary is being started

@leave if the test fails.
*/
enum TVerdict CSpudPrimaryEventOnLowerNifStarting::RunTestStepL()
	{
	TInt idx				 = RetrieveNetSideEventValFromConfigL(_L("QoSEventReq1Idx"));
	TEtelRequestType request = RetrieveNetSideEventRequestFromConfigL(_L("QoSEventReq1"));
	
	TInt expProgress(0);
	TInt expErrorCode(0);
	TInt primaryIapId(0);
	TInt millisBeforeEvent(0);
	if(!GetIntFromConfig(ConfigSection(), _L("PrimaryIapId1"), primaryIapId) ||
	   !GetIntFromConfig(ConfigSection(), _L("MillisBeforeEvent"), millisBeforeEvent) ||
	   !GetIntFromConfig(ConfigSection(), _L("ExpectedNifProgress"), expProgress) ||
	   !GetIntFromConfig(ConfigSection(), _L("ExpectedNifError"), expErrorCode)) 
		{
		User::Leave(KErrNotFound);
		}
	
	TRequestStatus startReq;
	TCommDbConnPref iap1prefs;
    iap1prefs.SetIapId(primaryIapId);	
	TestL(iInterface.Open(iEsock), _L("RConnection::Open on interface"));
	iInterface.Start(iap1prefs, startReq);
	
	
	//register for progress notification
	TRequestStatus progressReqSt;	
	iInterface.ProgressNotification(iProgressBuf, progressReqSt, KFinishedSelection);
	WaitForProgressNotificationL(progressReqSt, KFinishedSelection, KErrNone);
		
	iInterface.ProgressNotification(iProgressBuf, progressReqSt, expProgress);
		
	User::After(millisBeforeEvent);		
	EtelRequestL(request, idx);
	WaitForProgressNotificationL(progressReqSt, static_cast<TUint>(expProgress), expErrorCode);
	
	iInterface.Close(); 
	return EPass;
	};


/**
Test RConnection::Stop on Spud, when Spud is processing network-side event, and potentially there are
requests outstanding on Etel

@leave if the test fails.
*/
enum TVerdict CSpudPrimaryInterfaceEventStop::RunTestStepL()
	{
	TInt idx				 = RetrieveNetSideEventValFromConfigL(_L("QoSEventReq1Idx"));
	TEtelRequestType request = RetrieveNetSideEventRequestFromConfigL(_L("QoSEventReq1"));
	
	TInt primaryIapId(0);
	if(!GetIntFromConfig(ConfigSection(), _L("PrimaryIapId1"), primaryIapId)) 
		{
		User::Leave(KErrNotFound);
		}
	
	TRequestStatus startReq;
	TCommDbConnPref iap1prefs;
    iap1prefs.SetIapId(primaryIapId);	
	TestL(iInterface.Open(iEsock), _L("RConnection::Open on interface"));
	iInterface.Start(iap1prefs, startReq);
	
	TInt primStartErr(KErrNone);
	if(GetIntFromConfig(ConfigSection(), _L("PrimaryStartErr1"), primStartErr))
		{
		User::WaitForRequest(startReq);
		TestL(startReq.Int(), primStartErr, _L("Interface started."));
		}
		
	EtelRequestL(request, idx);
	
	StopInterfaceL(); // Stop the interface when SPUD is processing the Etel event. 
	return EPass;
	};

/**
Test RConnection:Stop on Spud, when the primary PDP context is being started.

@leave if the test fails.
*/
enum TVerdict CSpudPrimayStartingStop::RunTestStepL()
	{
	TInt primaryCreationErr(KErrNone);
	TInt primaryIapId(0);
	if(!GetIntFromConfig(ConfigSection(), _L("PrimaryIapId1"), primaryIapId) ||
	   !GetIntFromConfig(ConfigSection(), _L("PrimaryActivationErr1"), primaryCreationErr))
		{
		User::Leave(KErrNotFound);
		}
	TCommDbConnPref iap1prefs;
    iap1prefs.SetIapId(primaryIapId);	
	TestL(iInterface.Open(iEsock), _L("RConnection::Open on interface"));
	
	// start then stop the interface
	TRequestStatus startStatus;
	iInterface.Start(iap1prefs, startStatus);
	// wait for enough time to be initialising the phone
	User::After(5000000);
	iInterface.Stop();
	User::WaitForRequest(startStatus);
	TestL(startStatus.Int(), primaryCreationErr, _L("RConnection::Start on interface"));

	iInterface.Close();	
	
	return EPass;	
	}

/**
Test RConnection:Stop on Spud, when the secondary PDP context is being started.

@leave if the test fails.
*/
enum TVerdict CSpudSecondaryStartingStop::RunTestStepL()
	{

	InitiateSecondaryStartL();
	User::After(KTimeToStartSecondary);
	// We don't know for sure when the secondary is being activated (as opposed to TFT set, etc).
	// So, we must set the stop wait + activation wait to be large enough to mask the timing 
	// differences on different platforms, especially, hw vs emulator.
	StopInterfaceL();  // QoSConfirm never comes, we stop the interface before it.
	
	// Don't leave QoS lingering.
#ifndef SYMBIAN_NON_SEAMLESS_NETWORK_BEARER_MOBILITY
	TestL(iQoSPolicy_1.CancelNotifyEvent(*this), _L("Cancelling QoS Notify"));
	TestL(iQoSPolicy_1.Close(), _L("Closing QoS Channel"));
#endif
	
	iSocket.Close();
	
	return EPass;	
	}


/**
Test RConnection:Stop on Spud, when the lower NIF for secondary PDP context has reported LinkLayerDown, and Spud
has requests outstading to handle it.

@leave if the test fails.
*/
enum TVerdict CSpudSecondaryLowerNifDownStop::RunTestStepL()
	{
	StartSecondaryL();
	
	TInt loopbackPort;
	if(!GetIntFromConfig(ConfigSection(), _L("SecondaryLoopbackPort"), loopbackPort))
		{
		User::Leave(KErrNotFound);
		}
	FailNextPktLoopbackCsyWriteL(loopbackPort, KErrCompletion);
	TRequestStatus sendStatus;
	iSocket.Send(KCommWriteData, 0, sendStatus);
	User::WaitForRequest(sendStatus);
	TestL(sendStatus.Int(), _L("Sent data on socket to cause Lower NIF LinkLayerDown"));
	
	// LinkLayerDown was called, secondary context deletion is outstanding. 
	// We sneak in and stop the entire interface.
	// We don't bother listening for QoS failure in this particular test.
	StopInterfaceL();
	
	StopSecondaryL();
	
	ClearNextPktLoopbackCsyWriteL(loopbackPort);
	return EPass;
	}

/**
Test RConnection:Stop on Spud, when the secondary PDP context is being deleted by GUQoS due to idle timeout.

@leave if the test fails.
*/
enum TVerdict CSpudSecondaryInterfaceEventStop::RunTestStepL()
	{
	StartSecondaryL();
	
	// Cleanup
#ifndef SYMBIAN_NON_SEAMLESS_NETWORK_BEARER_MOBILITY
	TestL(iQoSPolicy_1.CancelNotifyEvent(*this), _L("Cancelling QoS Notify"));
	TestL(iQoSPolicy_1.Close(), _L("Closing QoS Channel"));
#endif
	iSocket.Close();
	
	StopInterfaceL(); // Stop the entire interface.
	
	return EPass;
	}

/**
Test the effects of expiry of GUQoS timeout & deletion of the secondary interface, once RConnection:Stop on Spud
was called already.

@leave if the test fails.
*/
enum TVerdict CSpudSecondaryInterfaceStop::RunTestStepL()
	{
	StartSecondaryL();
	StopInterfaceL(); // Stop the entire interface.
	
	// Cleanup
#ifndef SYMBIAN_NON_SEAMLESS_NETWORK_BEARER_MOBILITY
	TestL(iQoSPolicy_1.CancelNotifyEvent(*this), _L("Cancelling QoS Notify"));
	TestL(iQoSPolicy_1.Close(), _L("Closing QoS Channel"));
#endif
	iSocket.Close();
	
	return EPass;
	};


/**
Test RConnection:Stop on Spud, when the primary PDP context is being deleted as a result of LinkLayerDown from the 
lower NIF.

@leave if the test fails.
*/
enum TVerdict CSpudPrimaryDeletionInterfaceStop::RunTestStepL()
	{
	StartPrimaryOnlyL(); // bring the primary up.
	TestL(iSocket.Open(iEsock, KAfInet, KSockDatagram, KProtocolInetUdp, iInterface), _L("RSocket::Open"));
	
	TInetAddr dstAddr;
	dstAddr.SetPort(KConfiguredTftFilter1SrcPort);
	dstAddr.Input(KConfiguredTftFilter1SrcAddr);
	
	TRequestStatus status;
	iSocket.Connect(dstAddr, status);
	User::WaitForRequest(status);
	TestL(status.Int(), _L("Socket connected to destination."));
	
	TInt loopbackPort;
	if(!GetIntFromConfig(ConfigSection(), _L("PrimaryLoopbackPort"), loopbackPort))
		{
		User::Leave(KErrNotFound);
		}
	FailNextPktLoopbackCsyWriteL(loopbackPort, KErrCompletion);
		
	TRequestStatus sendStatus;
	iSocket.Send(KCommWriteData, 0, sendStatus);
	User::WaitForRequest(sendStatus);
	TestL(sendStatus.Int(), 0, _L("Socket send status")); // cause the lower NIF send to fail.
	// Keep Nifman in Long idle timeout. This is more realistic scenario.
	// Now the lower NIF has fired LinkLayerDown, and the primary context is being deleted.
	// Sim TSY delays the completion of the deletion request. 
	// In the meanwhile, we sneak in and stop the interface.
	
	StopInterfaceL();
	ClearNextPktLoopbackCsyWriteL(loopbackPort);
	return EPass;	
	};

/**
Test RConnection:Stop on Spud, when only the primary PDP context exists and is active (i.e. no secondaries)

@leave if the test fails.
*/	
enum TVerdict CSpudPrimaryInterfaceStop::RunTestStepL()
	{
	StartPrimaryOnlyL();
	StopInterfaceL();
	return EPass;
	}

/**
Stops the interface using the specified method:
RConnection:Stop with EStopAuthoritative / EStopNormal, or Nifman idle timeout.
The type of stop is read from the config file.
The method connects to the interface to issue the Stop if necessary.
The method listens for KLinkLayerClosed progress notification with the specified error.

@leave if KLinkLayerClosed is not received correctly, or received with unexpected error code.
*/
void CSpudNetSideTestBase::StopInterfaceL()
	{
	TInt millisBeforeStop(0);
	TInt ifaceStopErrCode(KErrNone);
	TPtrC ifaceStopTypeStr;
	if(!GetIntFromConfig(ConfigSection(), _L("MillisBeforeStop"), millisBeforeStop) ||
	   !GetIntFromConfig(ConfigSection(), _L("IfaceStopErrCode"), ifaceStopErrCode) ||
	   !GetStringFromConfig(ConfigSection(), _L("IfaceStopType"), ifaceStopTypeStr) )
		{
		User::Leave(KErrNotFound);
		}
	INFO_PRINTF4(_L("RConnection::Stop: pause[%d], Type[%S], Error Code[%d]"), 
	millisBeforeStop, &ifaceStopTypeStr, ifaceStopErrCode);	
	
	// Attach monitor
	RConnection ifaceMonitor;
	TestL(ifaceMonitor.Open(iEsock), _L("Opening RConnection for monitoring"));
	CleanupClosePushL(ifaceMonitor);
	
	TUint ifaceCount = 0;
	TestL(ifaceMonitor.EnumerateConnections(ifaceCount), _L("Enumerating interfaces"));
	ASSERT(1 == ifaceCount); // We have only 1 spud instance
	
	TConnectionInfo info;
	TPckg<TConnectionInfo> ifaceInfo(info);
	static const TInt KIfaceIdx(1); // we have one and only iface.
	TestL(ifaceMonitor.GetConnectionInfo(KIfaceIdx, ifaceInfo), _L("Getting connection info"));
	
	RConnection controlConn;
	TestL(controlConn.Open(iEsock), _L("Opening Normal Connection"));
	CleanupClosePushL(controlConn);
	
	TestL(controlConn.Attach(ifaceInfo, RConnection::EAttachTypeNormal), _L("Attaching Normally"));
	TestL(ifaceMonitor.Attach(ifaceInfo, RConnection::EAttachTypeMonitor), _L("Attaching as Monitor"));
	
	
	//register for progress notification
	TRequestStatus progressReqSt;	
	ifaceMonitor.ProgressNotification(iProgressBuf, progressReqSt, static_cast<TUint>(KLinkLayerClosed));
	
	
	User::After(millisBeforeStop);
	INFO_PRINTF1(_L("About to stop interface."));	
	// Shutdown the interface and get the resulting progress
	if(ifaceStopTypeStr.Compare(TPtrC(_L("EStopNormal"))) == 0)
		{
		TestL(controlConn.Stop(RConnection::EStopNormal), _L("Normal Stop on the Interface"));
		}
	else if(ifaceStopTypeStr.Compare(TPtrC(_L("EStopAuthoritative"))) == 0)
		{
		TestL(controlConn.Stop(RConnection::EStopAuthoritative),_L("Authoritative Stop on the Interface"));
		}
	else if(ifaceStopTypeStr.Compare(TPtrC(_L("NifmanIdleTimeout")))== 0)
		{
		controlConn.Close();	// Let Nifman short idle timeout take the interface down for us.
		// The monitor connection does not affect the timeout.
		}
	else 
		{
		User::Leave(KErrArgument);
		}
	
		
	WaitForProgressNotificationL(progressReqSt, KLinkLayerClosed, ifaceStopErrCode);
	CleanupStack::PopAndDestroy(&controlConn);
	CleanupStack::PopAndDestroy(&ifaceMonitor);
	}
	
	
//------------------------------------------------------
// TODO?: read Qos Parameters from config file.
// not really necessary, because it is easier to use a different section in config.txt
// Must correspond to Sim.tsy config.txt file. These parameters are
// negotiated by GuQoS and then the negotiated 
// parameters are used for comparison by Sim.tsy
//------------------------------------------------------

/**
Set the QoS parameters to value set 0
*/
#ifndef SYMBIAN_NON_SEAMLESS_NETWORK_BEARER_MOBILITY
void CSpudNetSideTestBase::SetQoSParameters(CQoSParameters& aParameters)
{
	aParameters.SetUpLinkMaximumBurstSize(1024);
	aParameters.SetUpLinkMaximumPacketSize(768);
	aParameters.SetUplinkBandwidth(1024);
	aParameters.SetUpLinkAveragePacketSize(768);
	aParameters.SetUpLinkPriority(3);	
	aParameters.SetUpLinkDelay(250);  

	aParameters.SetDownLinkMaximumBurstSize(1024);		
	aParameters.SetDownLinkMaximumPacketSize(512);
	aParameters.SetDownlinkBandwidth(1024);
	aParameters.SetDownLinkAveragePacketSize(512);
	aParameters.SetDownLinkPriority(KQoSLowestPriority);
	aParameters.SetDownLinkDelay(250);
}

/**
Set the QoS parameters to value set 1
*/
void CSpudNetSideTestBase::SetQoSParameters1(CQoSParameters& aParameters)
{
	aParameters.SetUplinkBandwidth(1020);
	aParameters.SetUpLinkMaximumBurstSize(896);
	aParameters.SetUpLinkMaximumPacketSize(768);
	aParameters.SetUpLinkDelay(250);
	aParameters.SetUpLinkPriority(3);

    aParameters.SetUpLinkAveragePacketSize(400);

	aParameters.SetDownlinkBandwidth(1020);
	aParameters.SetDownLinkMaximumBurstSize(768);
	aParameters.SetDownLinkMaximumPacketSize(512);
	aParameters.SetDownLinkDelay(250);
	aParameters.SetDownLinkPriority(KQoSLowestPriority);

    aParameters.SetDownLinkAveragePacketSize(400);
}
#endif


/**
 Starts a primary context by starting and interface, and starts PPP instances opposite the primary context
 @leave If any of the above steps fail
*/
TVerdict CSpudPppPrimary::RunTestStepL()
	{
	TInt primarySpudIapId, firstPppIapId;
	
	TestBooleanTrueL(GetIntFromConfig(ConfigSection(), _L("PrimarySpudIapId"), primarySpudIapId), _L("Get Iap ID for first Ppp instance"));
	TestBooleanTrueL(GetIntFromConfig(ConfigSection(), _L("PppIapId1"), firstPppIapId), _L("Get Iap ID for second Ppp instance"));

	TCommDbConnPref primarySpudPrefs, firstPppPrefs;
	primarySpudPrefs.SetIapId(primarySpudIapId);	
	firstPppPrefs.SetIapId(firstPppIapId);	
	RConnection pppInterface;
	TestL(iInterface.Open(iEsock), _L("RConnection::Open on SPUD interface"));
	TestL(pppInterface.Open(iEsock), _L("RConnection::Open on PPP interface"));
	
	TRequestStatus spudStatus, pppStatus;
	iInterface.Start(primarySpudPrefs, spudStatus);
	pppInterface.Start(firstPppPrefs, pppStatus);
	User::WaitForRequest(spudStatus);
	User::WaitForRequest(pppStatus);
	TestL(spudStatus.Int(), KErrNone, _L("RConnection::Start on SPUD interface"));
	TestL(pppStatus.Int(), KErrNone, _L("RConnection::Start on PPP interface"));
	
	return EPass;
	}
	
/** Class that sets up and starts an RConnection, then receives its request completion event as an active object */
class CConnectionStart : public CActive
	{
	public:
	~CConnectionStart();
	static CConnectionStart* NewLC(RSocketServ& aEsock, CSpudNetSideTestBase& aTestStep, TInt aIapId);
	void RunL();
	void DoCancel()
		{}
	RConnection iInterface;
	
	private:
	CConnectionStart(CSpudNetSideTestBase& aTestStep);
	void ConstructL(RSocketServ& aEsock, TInt aIapId);
	CSpudNetSideTestBase& iTestStep;
	TCommDbConnPref iPrefs;
	};

CConnectionStart::CConnectionStart(CSpudNetSideTestBase& aTestStep)
	: CActive(EPriorityStandard), iTestStep(aTestStep)
	{}
		
CConnectionStart::~CConnectionStart() 
	{
	iInterface.Close();

	Cancel();
	}
		
/**
 Starts a secondary context by creating a socket with QoS, starts a rawipnif instance opposite
 the secondary context, deletes the primary context then sends data on the primary context's socket
 Since the primary context is down, the data should get routed onto the secondary context
 @leave If any of the above steps fail
*/

TBool CSpudNetSideTestBase::SpudDeletePrimaryPdpL()
{
	RTimer GuardTimer;
	GuardTimer.CreateLocal();			// for this thread
	TRequestStatus TimerStatus;			// status to monitor timer
	const TUint KWaitTimeout = 5000000;

	TRequestStatus recvStatus, sendStatus;

	TInetAddr spudPrimaryPort, spudSecondaryPort, secondPppPort;
	spudSecondaryPort.SetPort(KConfiguredTftFilter1DestPort);
	spudPrimaryPort.SetPort(KConfiguredTftFilter2DestPort);
	secondPppPort.SetPort(KConfiguredTftFilter1SrcPort);
	
	RSocket primarySocket; // the secondary socket
	TestL(primarySocket.Open(iEsock, KAfInet, KSockDatagram, KProtocolInetUdp), _L("RSocket::Open for primary context's socket"));
	CleanupClosePushL<RSocket>(primarySocket);
	TestL(primarySocket.Bind(spudPrimaryPort), _L("Binding the local Socket for the primary context"));

	// set the dest addr to the address of the IAP corresponding to IapId entry from the config file
	TInetAddr dstAddr;
	dstAddr.SetPort(KConfiguredTftFilter1SrcPort);
	dstAddr.Input(KConfiguredTftFilter1SrcAddr);
	TRequestStatus status;
	primarySocket.Connect(dstAddr, status);
	User::WaitForRequest(status);
	TestL(status.Int(), _L("Connecting to local socket for the primary context"));

	// start a secondary context with QoS parameters connected to the same address as above
	// it will use the iSocket member variable to connect to the address
	

	TInt defaultIapID = 2;
	//the default IapID is usually defined in commsdb with value of 2.
	//this allows us to change the default IapID, as in R4_and_R5 test the default ID is 11.
	if(!GetIntFromConfig(ConfigSection(), _L("DefaultIapId"), defaultIapID))
        	{
	        defaultIapID = PDPIAP;
        	}

#ifdef SYMBIAN_NON_SEAMLESS_NETWORK_BEARER_MOBILITY
	User::After(10*KTimeToStartSecondary);
	VerifySubconnectionCountL(_L("SubConnectionCount1"), defaultIapID);
#endif
	InitiateSecondaryStartL();
	
	TInt oppositeSecondaryIapId;
	TestBooleanTrueL(GetIntFromConfig(ConfigSection(), _L("IapId"), oppositeSecondaryIapId), _L("Get Iap ID for the rawipnif instance opposite the secondary context"));

	// start rawipnif instance opposite the SPUD's secondary context, and open a socket on it
	CConnectionStart *secondaryIfStart = CConnectionStart::NewLC(iEsock, *this, oppositeSecondaryIapId);

    TRequestStatus progressReqSt;   
    
	secondaryIfStart->iInterface.ProgressNotification(iProgressBuf, progressReqSt, 7000);
	WaitForProgressNotificationL(progressReqSt, 7000, 0); // We can wait here forever. Set timeout on test step.

#ifndef SYMBIAN_NON_SEAMLESS_NETWORK_BEARER_MOBILITY
	WaitForQoSEventL(_L("SecondaryActivationEvent2"), _L("SecondaryActivationEvent2Reason"));
#else
	User::After(KTimeToStartSecondary);
	VerifySubconnectionCountL(_L("SubConnectionCount2"), defaultIapID);
#endif
	
	RSocket oppositeSecondarySocket;
	TestL(oppositeSecondarySocket.Open(iEsock, KAfInet, KSockDatagram, KProtocolInetUdp, secondaryIfStart->iInterface), _L("Opening socket opposite secondary context"));
	CleanupClosePushL<RSocket>(oppositeSecondarySocket);
	User::LeaveIfError(oppositeSecondarySocket.Bind(secondPppPort));
		
	// send data through the SPUD's secondary context to the rawipnif instance
	TBuf8<KCommReadBufSize> commReadBuf;
	oppositeSecondarySocket.RecvFrom(commReadBuf, spudSecondaryPort, 0, recvStatus);

	iSocket.Send(KCommWriteData, 0, sendStatus);
	User::WaitForRequest(sendStatus);

	TestL(sendStatus.Int(), _L("Send status on secondary context"));

	GuardTimer.After(TimerStatus,KWaitTimeout); 
	//User::WaitForRequest(recvStatus);
	User::WaitForRequest(recvStatus, TimerStatus);

	GuardTimer.Cancel();
	User::WaitForAnyRequest();

	TestL(recvStatus.Int(), _L("Receive status from secondary context"));
	TestL(commReadBuf.Compare(KCommWriteData), _L("Sent data matches received data on secondary context"));

	// cause a network event, deleting the primary context
	TInt idx = RetrieveNetSideEventValFromConfigL(_L("QoSEventReq1Idx"));
	EtelRequestL(EContextStatusChange, idx);
	
	// pause to allow notification of the deletion to get through
	User::After(1000000);

	// send data through the SPUD's primary context
	// with the primary context deleted, the remaining secondary context should become the default context
	// and this data should get sent through the secondary context
	primarySocket.Send(KCommWriteData, 0, sendStatus);
	User::WaitForRequest(sendStatus);
	TestL(sendStatus.Int(), _L("Send status on primary context"));
	// GuQoS will drop the previous packet, and be triggered to modify the secondary context to become the default context
	// pause to allow GuQos to do this
	User::After(1000000);
	// this packet should make it through
	oppositeSecondarySocket.RecvFrom(commReadBuf, spudPrimaryPort, 0, recvStatus);
	primarySocket.Send(KCommWriteData, 0, sendStatus);
	User::WaitForRequest(sendStatus);
	TestL(sendStatus.Int(), _L("Send status on primary context"));
	User::WaitForRequest(recvStatus);
	TestL(recvStatus.Int(), _L("Receive status from primary context"));
	TestL(commReadBuf.Compare(KCommWriteData), _L("Sent data matches received data from primary context"));
	
	// make sure sending on the secondary still works
	oppositeSecondarySocket.RecvFrom(commReadBuf, spudSecondaryPort, 0, recvStatus);
	iSocket.Send(KCommWriteData, 0, sendStatus);
	User::WaitForRequest(sendStatus);
	TestL(sendStatus.Int(), _L("Send status from secondary context"));
	User::WaitForRequest(recvStatus);
	TestL(recvStatus.Int(), _L("Receive status from secondary context"));
	TestL(commReadBuf.Compare(KCommWriteData), _L("Sent data matches received data from secondary context"));

	// cleanup
	CleanupStack::PopAndDestroy(&oppositeSecondarySocket);

	if (secondaryIfStart->IsActive())
		{
		secondaryIfStart->Deque();
		}
	CleanupStack::PopAndDestroy(secondaryIfStart);
	CleanupStack::PopAndDestroy(&primarySocket);

	return ETrue;
}

TVerdict CSpudDeletePrimary::RunTestStepL()
{
	if(SpudDeletePrimaryPdpL())
	  {
	  return EPass;
	  }
	 
	 return EFail;
}


/**
 Opens and starts a connection, using aIapId as the preferred IAP
 @param aEsock The socket server to open the connection on
 @param aTestStep The test steps logging is used
 @param aIapId The IAP to use for the connection
 @leave Any error returned from RConnection::Open, or KErrNoMemory if new(ELeave) fails
*/
CConnectionStart* CConnectionStart::NewLC(RSocketServ& aEsock, CSpudNetSideTestBase& aTestStep, TInt aIapId)
	{
	CConnectionStart *me = new(ELeave) CConnectionStart(aTestStep);
	CleanupStack::PushL(me);
	me->ConstructL(aEsock, aIapId);
	return me;
	}
	
void CConnectionStart::ConstructL(RSocketServ& aEsock, TInt aIapId)
	{
	CActiveScheduler::Add(this);
	SetActive();
    iPrefs.SetIapId(aIapId);
	iTestStep.TestL(iInterface.Open(aEsock), _L("RConnection::Open on interface"));
	iInterface.Start(iPrefs, iStatus);	
	}

/**
 Tests that the RConnection::Start succeeded
 @leave Leaves with the status of the RConnection::Start if the status is not KErrNone
*/
void CConnectionStart::RunL() 
	{
	iTestStep.TestL(iStatus.Int(), _L("RConnection::Start status"));
	}

/**
 Starts a secondary context by creating a socket with QoS, starts PPP instances opposite the PPP instances
 from the primary and secondary contexts, then sends data between them
 @leave If any of the above steps fail
*/
TVerdict CSpudPppSecondarySend::RunTestStepL()
	{
	TRequestStatus connectStatus, recvStatus, sendStatus;

	TInetAddr spudSecondaryPort, spudSecondaryAddr, secondPppPort;
	spudSecondaryPort.SetPort(KConfiguredTftFilter1DestPort);
	spudSecondaryAddr.SetPort(KConfiguredTftFilter1DestPort);
	spudSecondaryAddr.Input(KConfiguredTftFilter2DestAddr);
	secondPppPort.SetPort(KConfiguredTftFilter1SrcPort);

#ifdef SYMBIAN_NON_SEAMLESS_NETWORK_BEARER_MOBILITY
	VerifySubconnectionCountL(_L("SubConnectionCount1"), PDPIAP);
#endif

   // start a secondary context
    InitiateSecondaryStartL();

	TInt primaryIapId, secondaryIapId;
	TestBooleanTrueL(GetIntFromConfig(ConfigSection(), _L("PppIapId1"), primaryIapId), _L("Get Iap ID for first Ppp instance"));
	TestBooleanTrueL(GetIntFromConfig(ConfigSection(), _L("PppIapId2"), secondaryIapId), _L("Get Iap ID for second Ppp instance"));

	// start 2 more PPP instances opposite the SPUD's primary and secondary context PPP instances
	CConnectionStart *primaryIfStart = CConnectionStart::NewLC(iEsock, *this, primaryIapId);
	CConnectionStart *secondaryIfStart = CConnectionStart::NewLC(iEsock, *this, secondaryIapId);

	TRequestStatus progressReqSt;   
	primaryIfStart->iInterface.ProgressNotification(iProgressBuf, progressReqSt, KConnectionUp );

	WaitForProgressNotificationL(progressReqSt, KConnectionUp, 0);

	secondaryIfStart->iInterface.ProgressNotification(iProgressBuf, progressReqSt, KConnectionUp);
	WaitForProgressNotificationL(progressReqSt, KConnectionUp, 0);


#ifndef SYMBIAN_NON_SEAMLESS_NETWORK_BEARER_MOBILITY
	WaitForQoSEventL(_L("SecondaryActivationEvent2"), _L("SecondaryActivationEvent2Reason"));
#else
	VerifySubconnectionCountL(_L("SubConnectionCount2"), PDPIAP);
#endif

	// set up a socket from the second PPP instance (that connects to the SPUD's secondary context)
	RSocket secondPppSocket;
	User::LeaveIfError(secondPppSocket.Open(iEsock, KAfInet, KSockDatagram, KProtocolInetUdp, secondaryIfStart->iInterface));
	CleanupClosePushL<RSocket>(secondPppSocket);
	User::LeaveIfError(secondPppSocket.Bind(secondPppPort));
	secondPppSocket.Connect(spudSecondaryAddr, connectStatus);
	User::WaitForRequest(connectStatus);
	TestL(connectStatus.Int(), _L("Connecting to local socket"));
		
	// send data through the SPUD's secondary context to the second PPP instance
	TBuf8<KCommReadBufSize> commReadBuf;
	secondPppSocket.RecvFrom(commReadBuf, spudSecondaryPort, 0, recvStatus);
	iSocket.Send(KCommWriteData, 0, sendStatus);
	User::WaitForRequest(sendStatus);
	User::WaitForRequest(recvStatus);
	TestL(commReadBuf.Compare(KCommWriteData), _L("Sent data matches received data"));
	
	// send data in the opposite direction
	iSocket.RecvFrom(commReadBuf, secondPppPort, 0, recvStatus);
	secondPppSocket.Send(KCommWriteData2, 0, sendStatus);
	User::WaitForRequest(sendStatus);
	User::WaitForRequest(recvStatus);
	TestL(commReadBuf.Compare(KCommWriteData2), _L("Sent data matches received data"));
	

	// cleanup
	CleanupStack::PopAndDestroy(&secondPppSocket);
	if (primaryIfStart->IsActive())
		{
		primaryIfStart->Deque();
		}
	if (secondaryIfStart->IsActive())
		{
		secondaryIfStart->Deque();
		}

	CleanupStack::PopAndDestroy(secondaryIfStart);
	CleanupStack::PopAndDestroy(primaryIfStart);

	return EPass;
	}
/**
Tests creation of multiple primary PDP contexts by explicitly starting the interfaces

@leave if the test fails.
*/
enum TVerdict CSpudMultiPrimary::RunTestStepL()
	{
	TInt primaryCreationErr(KErrNone);
	TInt primaryIapId(0);
	TInt maximumConnections = 0;
	

	if (!GetIntFromConfig(ConfigSection(), _L("PrimaryActivationErr1"), primaryCreationErr))
		{
		User::Leave(KErrNotFound);
		}
		
    _LIT(KNumberOfIapsLit, "NumberOfIaps");
	if (!GetIntFromConfig(ConfigSection(), KNumberOfIapsLit, maximumConnections))
		{
		User::Leave(KErrNotFound);
		}


	// Opening multiple connections, we're not reusing the 'iInterface' member.
	//
	RPointerArray<RConnection> interfaces;

    _LIT(KPrimaryIapFormatLit, "PrimaryIapId%d");
	for (TInt i = 0; i < maximumConnections; i++)
	    {

        TBuf<32> primaryIap;
        primaryIap.Format(KPrimaryIapFormatLit, i + 1);
    	if (!GetIntFromConfig(ConfigSection(), primaryIap, primaryIapId))
    		{
    		User::Leave(KErrNotFound);
    		}
    		

    	INFO_PRINTF2(_L("Creating RConnection object %d"), primaryIapId);	

        RConnection* p = new (ELeave) RConnection();	


	    interfaces.Append(p);
        CleanupStack::PushL(p);
        
	    
    	TCommDbConnPref iap1prefs;
        iap1prefs.SetIapId(primaryIapId);	
        
    	INFO_PRINTF2(_L("Test starting Interface IAP ID == %d"), primaryIapId);	
    	
    	TestL(p->Open(iEsock), _L("RConnection::Open the interface"));
    	TestL(p->Start(iap1prefs),primaryCreationErr, _L("RConnection::Start the interface"));
	    }

	//check if all contexts are still there
	for (TInt i = 0; i < maximumConnections; i++)
	    {
        TBuf<32> primaryIap;
        primaryIap.Format(KPrimaryIapFormatLit, i + 1);
    	if (!GetIntFromConfig(ConfigSection(), primaryIap, primaryIapId))
    		{
    		User::Leave(KErrNotFound);
    		}
		INFO_PRINTF2(_L("Verify context #%d is still there"), i);
		VerifySubconnectionCountL(2, primaryIapId);
		INFO_PRINTF2(_L("Context #%d is still there"), i);
	    }

	
	for (TInt i = (interfaces.Count() - 1); i >= 0; i--)
	    {
	    RConnection* p = interfaces[i];
	    interfaces.Remove(i);
	    
    	p->Stop();
    	p->Close();	
    	
        // empty destructor for 'RConnection' 	    
        //
        CleanupStack::PopAndDestroy(p);
    	}
	
	interfaces.Reset();

	return EPass;
	}

enum TVerdict CIoctlAddressRetrieve::RunTestStepL()
    {
    StartPrimaryOnlyL();    
    
    TPtrC addresse;
    TBool checkSipAddress = ETrue;
    if(!GetStringFromConfig(ConfigSection(), _L("ExpectedAddress"), addresse))            
        {
        checkSipAddress = EFalse;
        }
    
    TInt expectedError = KErrNone;
    if(!GetIntFromConfig(ConfigSection(), _L("SipAddressExpectedError"), expectedError))            
        {
        expectedError = KErrNone;
        }
    
    TInt sipAddressIndex = 0;
    if(!GetIntFromConfig(ConfigSection(), _L("IndexOfTheSIPAddressToGet"), sipAddressIndex))            
        {
        User::Leave(KErrNotFound);
        }
    
    //call ioctl
    TRequestStatus status;
    
    INFO_PRINTF1(_L("Calling ioctl.."));
    
    // Request the Sip address available
    TSipServerAddrBuf sipServerAddr;
    sipServerAddr().index = sipAddressIndex;
    iInterface.Ioctl(KCOLConfiguration, KConnGetSipServerAddr, status, &sipServerAddr);
    User::WaitForRequest(status);
    
    INFO_PRINTF2(_L("SIP address retrieval completed with Err[%d]..."), status.Int());
    if(status.Int() == expectedError)
        {
        if (expectedError == KErrNone && checkSipAddress)
            {
            THostName addr;
            sipServerAddr().address.Output(addr);
        
            // Verify the retrieved address is correct
            TPtrC addressToCompare;
        
            addressToCompare.Set(addresse);
            
            INFO_PRINTF2(_L("Received SIP address: %S"), &addr);
            if(addr.CompareF(addressToCompare) != 0)
                {
                INFO_PRINTF2(_L("Expected SIP address %S!"), &addressToCompare);
                TESTEL(0, KErrNotFound);
                }
            }
        else
            {
            INFO_PRINTF1(_L("As Expected"));
            }
        }
    else
        {
        INFO_PRINTF1(_L("Unexpected"));
        User::Leave(status.Int());
        }
    
    iInterface.Stop();
    iInterface.Close(); 

    return EPass;
    }

enum TVerdict CRawIpMinMaxMMU::RunTestStepL()
    {
    //Start 1st context
    RConnection* conn1 = new (ELeave) RConnection();
    CleanupClosePushL<RConnection>(*conn1);

    TInt primaryIapId;
    if (!GetIntFromConfig(ConfigSection(), _L("PrimaryIapId1"), primaryIapId))
        {
        User::Leave(KErrNotFound);
        }

    TCommDbConnPref conn1Pref;
    conn1Pref.SetIapId(primaryIapId);
    
    INFO_PRINTF2(_L("Test starting Interface IAP ID == %d"), primaryIapId); 
    TestL(conn1->Open(iEsock), _L("RConnection::Open the interface"));
    TestL(conn1->Start(conn1Pref), _L("RConnection::Start the interface"));

    //Start 2nd context
    RConnection* conn2 = new (ELeave) RConnection();
    CleanupClosePushL<RConnection>(*conn2);

    if (!GetIntFromConfig(ConfigSection(), _L("PrimaryIapId2"), primaryIapId))
        {
        User::Leave(KErrNotFound);
        }

    TCommDbConnPref conn2Pref;
    conn2Pref.SetIapId(primaryIapId);
    
    INFO_PRINTF2(_L("Test starting Interface IAP ID == %d"), primaryIapId);     
    TestL(conn2->Open(iEsock), _L("RConnection::Open the interface"));
    TestL(conn2->Start(conn2Pref), _L("RConnection::Start the interface"));

    //Open & Connect 1st Socket
    RSocket sock1;
    TestL(sock1.Open(iEsock, KAfInet, KSockDatagram, KProtocolInetUdp, *conn1), _L("RSocket::Open"));
    CleanupClosePushL<RSocket>(sock1);
    TInetAddr localAddr;
    localAddr.SetPort(KConfiguredTftFilter1DestPort);
    TestL(sock1.Bind(localAddr), _L("Binding the local Socket"));
    
    TInetAddr dstAddr;
    dstAddr.SetPort(KConfiguredTftFilter1SrcPort);
    dstAddr.Input(KConfiguredTftFilter1SrcAddr);

    TRequestStatus status;
    sock1.Connect(dstAddr, status);
    User::WaitForRequest(status);
    TestL(status.Int(), _L("RSocket::Connect status opening 1st socket"));

    //Open & Bind 2nd Socket
    RSocket sock2;
    TestL(sock2.Open(iEsock, KAfInet, KSockDatagram, KProtocolInetUdp, *conn2), _L("RSocket::Open"));
    CleanupClosePushL<RSocket>(sock2);
    localAddr.SetPort(KConfiguredTftFilter1SrcPort);
    TestL(sock2.Bind(localAddr), _L("Binding the local Socket"));

    //then send & receive data.
    const TInt KMaxMMU = 4000;
    TBuf8<KMaxMMU> *sendBuf = new(ELeave) TBuf8<KMaxMMU>();
    CleanupStack::PushL(sendBuf);
    
    TBuf8<KMaxMMU> *recvBuf = new(ELeave) TBuf8<KMaxMMU>();
    CleanupStack::PushL(recvBuf);

    TBuf8<KMaxMMU> *recvBuf2 = new(ELeave) TBuf8<KMaxMMU>();
    CleanupStack::PushL(recvBuf2);

    TInt end;
    if (!GetIntFromConfig(ConfigSection(), _L("TestMaxMMU"), end))
        {
        User::Leave(KErrNotFound);
        }
    
    TInt begin;
    if (!GetIntFromConfig(ConfigSection(), _L("TestMinMMU"), begin))
        {
        User::Leave(KErrNotFound);
        }

    if (begin > end)
        {
        User::Leave(KErrArgument);
        }

    INFO_PRINTF3(_L("Sending data frame size from (%d) to (%d)"), begin, end); 
    TRequestStatus readStatus;

    for(TInt j=begin;j<=end;j++)
        {
        sendBuf->Zero();
        for (TInt i=0;i<j;i++)
            {
            sendBuf->Append(Math::Random() & 0x7f);
            }

        recvBuf->Zero();
        sock2.Read(*recvBuf, readStatus);
        sock1.Write(*sendBuf, status);
        
        User::WaitForRequest(status);
        TESTL(status.Int() == 0);
        User::WaitForRequest(readStatus);
        TESTL(readStatus.Int() == 0);
        if (sendBuf->Length() > recvBuf->Length())
            {
            recvBuf2->Zero();
            sock2.Read(*recvBuf2, readStatus);
            User::WaitForRequest(readStatus);
            TESTL(readStatus.Int() == 0);
            recvBuf->Append(*recvBuf2);
            TESTL(sendBuf->Length() == recvBuf->Length());
            }
        TESTL(recvBuf->Compare(*sendBuf) == 0);
        }

    CleanupStack::PopAndDestroy(7); //conn1, conn2, sock1, sock2, sendBuf, recvBuf, recvBuf2

    return EPass;
    }
