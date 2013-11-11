// Copyright (c) 2004-2010 Nokia Corporation and/or its subsidiary(-ies).
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
// Test framework for SPUD TEL and SPUD FSM unit tests
// 
//

/**
 @file
 @internalComponent
*/

#include <e32property.h>
#include <c32comm.h>
#include <e32def.h>
#include <e32std.h>

#include <in_iface.h>
#include <simtsy.h>

#include "SpudUnitTestStep.h"
#include "InputRequestListener.h"

using namespace EtelDriver;

CSpudUnitTestStepBase::CSpudUnitTestStepBase(TInt aTestNumber, TBool aUseTestPdpFsmInterface) : iTestNumber(aTestNumber), iUseTestPdpFsmInterface(aUseTestPdpFsmInterface)
	{}

/**
Sets the section for the simtsy to use in its c:\config.txt file.
The section that will be used is testX, where X is the parameter
aTestNumber.

@param aTestNumber Simtsy configuration section number, which will be
                   set to ensure simtsy loads the correct parameters on
                   when it is loaded.
@leave The function leaves if the Sim.TSY test number property is not
       defined; e.g. make sure StartC32 has been called.
*/
void CSpudUnitTestStepBase::SetSimTsyTestNumberL(TInt aTestNumber)
  	{
	User::LeaveIfError(RProperty::Set(KUidPSSimTsyCategory, KPSSimTsyTestNumber, aTestNumber));
  	}

/**
 Initializes the test framework, including creating the thread that will run the active 
 scheduler containing the components under test (SPUD or SPUDTEL)
 */
TVerdict CSpudUnitTestStepBase::doTestStepPreambleL()
	{
	SetSimTsyTestNumberL(iTestNumber);

	// create CTestRequestListener, to be used to contain parameters to be passed
	// between the two threads
	switch (iTestNumber)
		{
		case KSimTsyTestNumberR5OrR99R4:
#ifdef SYMBIAN_NETWORKING_UMTSR5
			iListener = CInputRequestListener::NewL(iUseTestPdpFsmInterface, RThread().Id(), TPacketDataConfigBase::KConfigRel5);
#else
            iListener = CInputRequestListener::NewL(iUseTestPdpFsmInterface, RThread().Id(), TPacketDataConfigBase::KConfigRel99Rel4);
#endif
			break;
		default:
			iListener = CInputRequestListener::NewL(iUseTestPdpFsmInterface, RThread().Id(), TPacketDataConfigBase::KConfigGPRS);
			break;
		}

	// creating and start the thread that will run the active scheduler containing SPUD or SPUDTEL
	TActiveSchedulerThreadParams params;
	params.iListener = iListener;	
 	_LIT(activeSchedulerThreadName, "ActiveSchedulerThread_");
 	TBuf<255> buf(activeSchedulerThreadName);
 	buf.AppendNum(iTestNumber);
 	TInt ret = iActiveSchedulerThread.Create(buf, ActiveSchedulerThread, KDefaultStackSize, NULL, &params);
 	TESTEL(ret == KErrNone, ret);
	iActiveSchedulerThread.Resume();
	
	// wait for the thread to initialize before sending it any requests
	User::WaitForRequest(iListener->iThreadInitialized);

	return TestStepResult();
	}
	
/**
 Cleans up anything allocated in the preamble
 */
TVerdict CSpudUnitTestStepBase::doTestStepPostambleL()
	{
	// if we complete the listener's status with an error, the RunL will stop the 
	// active scheduler and the thread will clean up any resources and exit
	TRequestStatus *status = &iListener->iStatus;
	iActiveSchedulerThread.RequestComplete(status, KErrCancel);

	// wait until the thread has cleaned up (don't kill the thread, it will die by itself)
	User::WaitForRequest(iListener->iThreadDestructed);	
	iActiveSchedulerThread.Close();
	
	delete iListener;
	iListener = NULL;

	return EPass;
	}
	
/**
 Initializes the packet service to query current parameters
*/
void CSpudUnitTestStepBase::InitPhoneAndPacketServiceL()
	{
	TESTL(iTelServer.Connect() == KErrNone);
	TESTL(iTelServer.LoadPhoneModule( _L("SIM") ) == KErrNone);
	TESTL(iPhone.Open ( iTelServer, _L("SimulatorPhone") ) == KErrNone);
	TESTL(iPacketService.Open(iPhone) == KErrNone);
	}
	
void CSpudUnitTestStepBase::DestroyPhoneAndPacketService()
	{
	iPacketService.Close();
	iPhone.Close();
	iTelServer.Close();
	}

// Set up to not use Test PdpFsmInterface (and use the spud fsm's RPdpFsmInterface)
CSpudFsmUnitTestStepBase::CSpudFsmUnitTestStepBase(TInt aTestNumber) : CSpudUnitTestStepBase(aTestNumber, EFalse)
	{}
	
// Set up to use Test PdpFsmInterface
CSpudTelUnitTestStepBase::CSpudTelUnitTestStepBase(TInt aTestNumber) : CSpudUnitTestStepBase(aTestNumber, ETrue)
	{}

/**
 Must be called after simtsy has been initialized.
 Instructs the simtsy to complete a given request/notification. The supported commands are given in TEtelRequestType.
 The parameter supplied in aNewValue instructs the simtsy what data from the config.txt to use in the completion of the request.
 The aNewValue is the index to the entry to be used to complete the request for the current test step section, ie if aNewValue
 is 0, iTestNumber is 3, and aEtelCommand is ENetworkQoSChange, the entry will complete any NotifyNetworkQoSChange calls
 with data from the first QosProfileReqR99 entry of section [test3] in the config.txt file
*/
void CSpudUnitTestStepBase::EtelRequest(TEtelRequestType aEtelCommand, TInt aNewValue)
	{
	INFO_PRINTF3(_L("CSpudUnitTestStepBase::EtelRequest: aEtelCommand = %d, aNewValue = %d"), aEtelCommand, aNewValue);

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
		// log error
		return;
		}

  	// simtsy will listen for any changes to the property, and complete the corresponding request
	TInt ret = RProperty::Set(KUidPSSimTsyCategory, key, aNewValue);
	TEST(ret == KErrNone);
	}

/**
 Calling this will instruct the CTestRequestListener to call REtelDriverInput::CancelPdpNotifications on SPUDTEL
*/
void CSpudTelUnitTestStepBase::CancelPdpRequest(TContextId aPdpId)
	{
	INFO_PRINTF2(_L("CSpudUnitTestStepBase::EtelDriverCancelPdpRequest: aPdpId = %d"), aPdpId);
	// wait until iListener to finish with the iControlData member so that 2 requests
	// immediately after one another will still work
	User::WaitForRequest(iListener->iReadyForInputRequest);
	iListener->iReadyForInputRequest = KRequestPending;

	// set the parameters to pass to REtelDriverInput::Input
	iListener->iEtelDriverCancelRequestPdpId = aPdpId;

	// complete iListener's status, causing its RunL to call CancelPdp on SPUDTEL
	TRequestStatus *status = &iListener->iStatus;
	iActiveSchedulerThread.RequestComplete(status, CInputRequestListener::EEtelDriverCancelPdpRequest);
	}

/**
 Calling this will instruct the CTestRequestListener to call REtelDriverInput::Input on SPUDTEL
*/
void CSpudTelUnitTestStepBase::EtelDriverInputRequest(TContextId aPdpId, TEtelInput aOperation)
	{
	INFO_PRINTF3(_L("CSpudUnitTestStepBase::EtelDriverInputRequest: aPdpId = %d, aOperation = %d"), aPdpId, aOperation);
	// wait until iListener to finish with the iControlData member so that 2 requests
	// immediately after one another will still work
	User::WaitForRequest(iListener->iReadyForInputRequest);
	iListener->iReadyForInputRequest = KRequestPending;

	// set the parameters to pass to REtelDriverInput::Input
	iListener->iEtelDriverInputRequestData.iPdpId = aPdpId;
	iListener->iEtelDriverInputRequestData.iOperation = aOperation;

	// complete iListener's status, causing its RunL to call Input on SPUDTEL
	TRequestStatus *status = &iListener->iStatus;
	iActiveSchedulerThread.RequestComplete(status, CInputRequestListener::EEtelDriverInputRequest);
	}

/**
 Wait for RPdpFsmInterface::Input to be called with a non-notification event
*/
void CSpudTelUnitTestStepBase::WaitForEtelDriverInputResponse(TContextId aPdpId, PdpFsm::TEtelSignal aSignal, TInt aError)
	{
	INFO_PRINTF3(_L("CSpudUnitTestStepBase::WaitForEtelDriverInputResponse: aPdpId = %d, aSignal = %d"), aPdpId, aSignal);
	
	// the implementation of RPdpFsmInterface::Input that is called by the Etel driver will complete the iEtelDriverInputResponseStatus request
	User::WaitForRequest(iListener->iEtelDriverInputResponseStatus);
	
	// make sure the parameters are as expected
	TEST(iListener->iEtelDriverInputResponseStatus == aError);
	TEST(iListener->iEtelDriverInputResponseData.iPdpId == aPdpId);
	TEST(iListener->iEtelDriverInputResponseData.iSignal == aSignal);
	
	iListener->iEtelDriverInputResponseStatus = KRequestPending;
	}
	
/**
 Wait for RPdpFsmInterface::Input to be called with aOperation EConfigGPRSChangeNetwork
*/
void CSpudTelUnitTestStepBase::WaitForNextContextConfigNotification(TContextId aPdpId, const RPacketContext::TContextConfigGPRS& aContextConfigGPRS)
	{
	INFO_PRINTF2(_L("CSpudUnitTestStepBase::WaitForNextContextConfigNotification: aPdpId = %d"), aPdpId);
	
	// the implementation of RPdpFsmInterface::Input that is called by the Etel driver will complete the iEtelDriverConfigGPRSNotificationStatus request
	User::WaitForRequest(iListener->iEtelDriverConfigGPRSNotificationStatus);
	
	INFO_PRINTF2(_L("CSpudUnitTestStepBase::WaitForNextContextConfigNotification: iListener->iNotifyContextConfigData.iPdpId = %d"), iListener->iNotifyContextConfigData.iPdpId);
	// make sure the parameters are as expected
	TEST(iListener->iEtelDriverConfigGPRSNotificationStatus == KErrNone);
	TEST(iListener->iNotifyContextConfigData.iPdpId == aPdpId);
	TEST(CompareContextConfigGPRS(iListener->iNotifyContextConfigData.iContextConfigGPRS, aContextConfigGPRS));
	
	iListener->iEtelDriverConfigGPRSNotificationStatus = KRequestPending;
	}

/**
 Wait for RPdpFsmInterface::Input to be called with aOperation EConfigGPRSChangeNetwork
 RPacketContext::TContextConfig_R5 overload
*/
void CSpudTelUnitTestStepBase::WaitForNextContextConfigNotification(TContextId aPdpId, const RPacketContext::TContextConfig_R5& aContextConfigR5)
	{
	INFO_PRINTF2(_L("CSpudUnitTestStepBase::WaitForNextContextConfigNotification: aPdpId = %d"), aPdpId);
	
	// the implementation of RPdpFsmInterface::Input that is called by the Etel driver will complete the iEtelDriverConfigGPRSNotificationStatus request
	User::WaitForRequest(iListener->iEtelDriverConfigGPRSNotificationStatus);
	
	INFO_PRINTF2(_L("CSpudUnitTestStepBase::WaitForNextContextConfigNotification: iListener->iNotifyContextConfigData.iPdpId = %d"), iListener->iNotifyContextConfigData.iPdpId);
	// make sure the parameters are as expected
	TEST(iListener->iEtelDriverConfigGPRSNotificationStatus == KErrNone);
	TEST(iListener->iNotifyContextConfigData.iPdpId == aPdpId);
	TEST(CompareContextConfigR5(iListener->iNotifyContextConfigData.iContextConfig_R5, aContextConfigR5));
	
	iListener->iEtelDriverConfigGPRSNotificationStatus = KRequestPending;
	}

/**
 Wait for RPdpFsmInterface::Input to be called with aOperation EConfigGPRSChangeNetwork
 RPacketContext::TContextConfigR99_R4 overload
*/
void CSpudTelUnitTestStepBase::WaitForNextContextConfigNotification(TContextId aPdpId, const RPacketContext::TContextConfigR99_R4& aContextConfigR99R4)
	{
	INFO_PRINTF2(_L("CSpudUnitTestStepBase::WaitForNextContextConfigNotification: aPdpId = %d"), aPdpId);
	
	// the implementation of RPdpFsmInterface::Input that is called by the Etel driver will complete the iEtelDriverConfigGPRSNotificationStatus request
	User::WaitForRequest(iListener->iEtelDriverConfigGPRSNotificationStatus);
	
	INFO_PRINTF2(_L("CSpudUnitTestStepBase::WaitForNextContextConfigNotification: iListener->iNotifyContextConfigData.iPdpId = %d"), iListener->iNotifyContextConfigData.iPdpId);
	// make sure the parameters are as expected
	TEST(iListener->iEtelDriverConfigGPRSNotificationStatus == KErrNone);
	TEST(iListener->iNotifyContextConfigData.iPdpId == aPdpId);
	TEST(CompareContextConfigR99R4(iListener->iNotifyContextConfigData.iContextConfigR99_R4, aContextConfigR99R4));
	
	iListener->iEtelDriverConfigGPRSNotificationStatus = KRequestPending;
	}


/**
 Wait for RPdpFsmInterface::Input to be called with aOperation EContextStatusChangeNetwork
*/
void CSpudTelUnitTestStepBase::WaitForGivenEtelContextStatusNotification(TContextId aPdpId, RPacketContext::TContextStatus aContextStatus)
	{
	INFO_PRINTF3(_L("CSpudUnitTestStepBase::WaitForGivenContextStatusNotification: aPdpId = %d, aContextStatus = %d"), aPdpId, aContextStatus);
	
	do
		{
	// the implementation of RPdpFsmInterface::Input that is called by the Etel driver will complete the iEtelDriverContextStatusNotificationStatus request
		User::WaitForRequest(iListener->iEtelDriverContextStatusNotificationStatus);
		
		INFO_PRINTF3(_L("CSpudUnitTestStepBase::WaitForGivenContextStatusNotification: iPdpId = %d, iContextStatus = %d"), iListener->iNotifyContextConfigData.iPdpId, iListener->iNotifyContextStatusData.iContextStatus);
		} while (iListener->iNotifyContextStatusData.iPdpId != aPdpId || iListener->iNotifyContextStatusData.iContextStatus != aContextStatus);
	
	// make sure the parameters are as expected
	TEST(iListener->iEtelDriverContextStatusNotificationStatus == KErrNone);
	TEST(iListener->iNotifyContextStatusData.iPdpId == aPdpId);
	TEST(iListener->iNotifyContextStatusData.iContextStatus == aContextStatus);
	
	iListener->iEtelDriverContextStatusNotificationStatus = KRequestPending;
	}

/**
 Wait for RPdpFsmInterface::Input to be called with aOperation EQoSProfileChangeNetwork
*/
void CSpudTelUnitTestStepBase::WaitForNextQosNotification(TContextId aPdpId, const RPacketQoS::TQoSR99_R4Negotiated& aQoSR99_R4Negotiated)
	{
	INFO_PRINTF2(_L("CSpudUnitTestStepBase::WaitForNextQosNotification: aPdpId = %d"), aPdpId);
	
	// the implementation of RPdpFsmInterface::Input that is called by the Etel driver will complete the iEtelDriverQosNotificationStatus request
	User::WaitForRequest(iListener->iEtelDriverQosNotificationStatus);
	
	// make sure the parameters are as expected
	TEST(iListener->iEtelDriverQosNotificationStatus == KErrNone);
	TEST(iListener->iNotifyQosNegotiatedData.iPdpId == aPdpId);
	TEST(CompareUMTSQoSNeg(iListener->iNotifyQosNegotiatedData.iNegotiated.NegotiatedQoSR99_R4(), aQoSR99_R4Negotiated));
	iListener->iEtelDriverQosNotificationStatus = KRequestPending;
	}


/**
 Wait for RPdpFsmInterface::Input to be called with aOperation EServiceStatusChangeNetwork
*/
void CSpudTelUnitTestStepBase::WaitForNextServiceNotificationStatus(RPacketService::TStatus aServiceStatus)
	{
	INFO_PRINTF2(_L("CSpudUnitTestStepBase::WaitForNextServiceNotificationStatus: aServiceStatus = %d"), aServiceStatus);
	
	// the implementation of RPdpFsmInterface::Input that is called by the Etel driver will complete the iEtelDriverServiceNotificationStatus request
	User::WaitForRequest(iListener->iEtelDriverServiceNotificationStatus);
	
	INFO_PRINTF2(_L("CSpudUnitTestStepBase::WaitForNextServiceNotificationStatus: aServiceStatus = %d"), aServiceStatus);
	// make sure the parameters are as expected
	TEST(iListener->iEtelDriverServiceNotificationStatus == KErrNone);
	TEST(iListener->iPacketServiceStatus == aServiceStatus);
	
	iListener->iEtelDriverServiceNotificationStatus = KRequestPending;
	}

/**
 Calling this will instruct the CTestRequestListener to call REtelDriverInput::Input on SPUDTEL
*/
void CSpudFsmUnitTestStepBase::FsmInputRequest(TContextId aPdpId, TInt aOperation, TInt aParam)
	{
	INFO_PRINTF4(_L("CSpudUnitTestStepBase::FsmInputRequest: aPdpId = %d, aOperation = %d, aParam = %d"), aPdpId, aOperation, aParam);

	// wait until iListener to finish with the iControlData member so that 2 requests
	// immediately after one another will still work
	User::WaitForRequest(iListener->iReadyForInputRequest);
	iListener->iReadyForInputRequest = KRequestPending;

	// set the parameters to pass to REtelDriverInput::Input
	iListener->iFsmInputRequestData.iPdpId = aPdpId;
	iListener->iFsmInputRequestData.iOperation = aOperation;
	iListener->iFsmInputRequestData.iParam = aParam;

	// complete iListener's status, causing its RunL to call Input on SPUDTEL
	TRequestStatus *status = &iListener->iStatus;
	iActiveSchedulerThread.RequestComplete(status, CInputRequestListener::EFsmInputRequest);
	}

/**
 Wait for MSpudManInterface::Input to be called with a non-notification event
*/
void CSpudFsmUnitTestStepBase::WaitForFsmInputResponse(TInt aPdpId, TInt aEvent, TInt aParam)
	{
	INFO_PRINTF4(_L("CSpudUnitTestStepBase::WaitForFsmInputResponse: aPdpId = %d, aOperation = %d, aParam = %d"), aPdpId, aEvent, aParam);
	
	// the implementation of MSpudManInterface::Input that is called by the Etel driver will complete the iFsmInputResponseStatus request
	User::WaitForRequest(iListener->iFsmInputResponseStatus);
	
	// make sure the parameters are as expected
	TEST(iListener->iFsmInputResponseStatus == KErrNone);
	TEST(iListener->iFsmInputResponseData.iPdpId == aPdpId);
	TEST(iListener->iFsmInputResponseData.iEvent == aEvent);
	TEST(iListener->iFsmInputResponseData.iParam == aParam);
	
	iListener->iFsmInputResponseStatus = KRequestPending;
	}


TContextId CSpudFsmUnitTestStepBase::FsmObjectCreate()
    {
	INFO_PRINTF1(_L("CSpudUnitTestStepBase::FsmObjectCreate()"));

	// wait until iListener to finish with the iControlData member so that 2 requests
	// immediately after one another will still work
	//
	User::WaitForRequest(iListener->iReadyForInputRequest);
	iListener->iReadyForInputRequest = KRequestPending;

	// Set the parameters to pass to CPdpFsmFactory
	//
	iListener->iFsmInputRequestData.iPdpId = 0;

	// complete iListener's status, causing its RunL to call Input on SPUDTEL
	TRequestStatus *status = &iListener->iStatus;
	iActiveSchedulerThread.RequestComplete(status, CInputRequestListener::EFsmObjectCreate);

	return iListener->iFsmInputRequestData.iPdpId;
    }
    
    
TInt CSpudFsmUnitTestStepBase::WaitForFsmObjectCreateResponse(TInt aPdpId)
    {
	INFO_PRINTF2(_L("CSpudUnitTestStepBase::WaitForEFsmObjectCreateRespons(aPdpId = %d)"), aPdpId);
	
	// The Test listener will complete the iFsmObjectCreateResponse request.
	//
	User::WaitForRequest(iListener->iFsmObjectCreateResponse);
	

	// Make sure the parameters are as expected
	
	TEST(iListener->iFsmObjectCreateResponse == KErrNone);


	iListener->iFsmObjectCreateResponse = KRequestPending;
    return iListener->iFsmInputResponseData.iPdpId;	
    }


/**
 Wait for MSpudManInterface::Input to be called with aOperation KContextBlockedEvent
*/
void CSpudFsmUnitTestStepBase::WaitForFsmSuspendedNotification(TContextId aPdpId)
	{
	INFO_PRINTF2(_L("CSpudUnitTestStepBase::WaitForFsmSuspendedNotification: aPdpId = %d"), aPdpId);
	
	// the implementation of MSpudManInterface::Input that is called by the Etel driver will complete the iFsmContextSuspendedStatus request
	User::WaitForRequest(iListener->iFsmContextSuspendedStatus);
	
	// make sure the parameters are as expected
	TEST(iListener->iFsmContextSuspendedStatus == KErrNone);
	TEST(iListener->iContextBlockedEventPdpId == aPdpId);
	
	iListener->iFsmContextSuspendedStatus = KRequestPending;
	}

/**
 Wait for MSpudManInterface::Input to be called with aOperation KContextUnblockedEvent
*/
void CSpudFsmUnitTestStepBase::WaitForFsmResumedNotification(TContextId aPdpId)
	{
	INFO_PRINTF2(_L("CSpudUnitTestStepBase::WaitForEtelDriverInputResponse: aPdpId = %d"), aPdpId);
	
	// the implementation of MSpudManInterface::Input that is called by the Etel driver will complete the iFsmContextResumedStatus request
	User::WaitForRequest(iListener->iFsmContextResumedStatus);
	
	// make sure the parameters are as expected
	TEST(iListener->iFsmContextResumedStatus == KErrNone);
	TEST(iListener->iContextUnblockedEventPdpId == aPdpId);
	
	iListener->iFsmContextResumedStatus = KRequestPending;
	}
	
/**
 Wait for MSpudManInterface::Input to be called with aOperation KNetworkStatusEvent
*/
void CSpudFsmUnitTestStepBase::WaitForFsmServiceNotificationStatus()
	{
	INFO_PRINTF1(_L("CSpudUnitTestStepBase::WaitForGivenFsmServiceNotificationStatus"));
	
	// the implementation of MSpudManInterface::Input that is called by the Etel driver will complete the iFsmNetworkStatusStatus request
	User::WaitForRequest(iListener->iFsmNetworkStatusStatus);
	
	// make sure the parameters are as expected
	TEST(iListener->iFsmNetworkStatusStatus == KErrNone);
	// already checked iPdpId and iStatus from exiting condition of do while loop
	
	iListener->iFsmNetworkStatusStatus = KRequestPending;
	}

/**
 Wait for MSpudManInterface::Input to be called with aOperation KContextParametersChangeEvent
*/
void CSpudFsmUnitTestStepBase::WaitForFsmContextConfigNotification(TContextId aPdpId, const RPacketContext::TContextConfigGPRS& aContextConfig)
	{
	INFO_PRINTF2(_L("CSpudUnitTestStepBase::WaitForFsmContextConfigNotification: aPdpId = %d"), aPdpId);
	
	// the implementation of MSpudManInterface::Input that is called by the Etel driver will complete the iFsmContextParameterChangeStatus request
	User::WaitForRequest(iListener->iFsmContextParameterChangeStatus);
	
	// make sure the parameters are as expected
	TEST(iListener->iFsmContextParameterChangeStatus == KErrNone);
	TEST(iListener->iContextParametersEventData.iPdpId == aPdpId);
	TEST(CompareContextConfigGPRS(aContextConfig, iListener->iContextParametersEventData.iContextConfig));
	
	iListener->iFsmContextParameterChangeStatus = KRequestPending;
	}

/**
Wait for MSpudManInterface::Input to be called with aOperation KContextParametersChangeEvent
RPacketContext::TContextConfig_R5 overload
*/
void CSpudFsmUnitTestStepBase::WaitForFsmContextConfigNotification(TContextId aPdpId, const RPacketContext::TContextConfig_R5& aContextConfig)
	{
	INFO_PRINTF2(_L("CSpudUnitTestStepBase::WaitForFsmContextConfigNotification: aPdpId = %d"), aPdpId);
	
	// the implementation of MSpudManInterface::Input that is called by the Etel driver will complete the iFsmContextParameterChangeStatus request
	User::WaitForRequest(iListener->iFsmContextParameterChangeStatus);
	
	// make sure the parameters are as expected
	TEST(iListener->iFsmContextParameterChangeStatus == KErrNone);
	TEST(iListener->iContextParametersEventData.iPdpId == aPdpId);
	TEST(CompareContextConfigR5(aContextConfig, iListener->iContextParametersEventData.iContextConfig_R5));
	
	iListener->iFsmContextParameterChangeStatus = KRequestPending;
	}

/**
Wait for MSpudManInterface::Input to be called with aOperation KContextParametersChangeEvent
RPacketContext::TContextConfigR99_R4 overload
*/
void CSpudFsmUnitTestStepBase::WaitForFsmContextConfigNotification(TContextId aPdpId, const RPacketContext::TContextConfigR99_R4& aContextConfig)
	{
	INFO_PRINTF2(_L("CSpudUnitTestStepBase::WaitForFsmContextConfigNotification: aPdpId = %d"), aPdpId);
	
	// the implementation of MSpudManInterface::Input that is called by the Etel driver will complete the iFsmContextParameterChangeStatus request
	User::WaitForRequest(iListener->iFsmContextParameterChangeStatus);
	
	// make sure the parameters are as expected
	TEST(iListener->iFsmContextParameterChangeStatus == KErrNone);
	TEST(iListener->iContextParametersEventData.iPdpId == aPdpId);
	TEST(CompareContextConfigR99R4(aContextConfig, iListener->iContextParametersEventData.iContextConfigR99_R4));
	
	iListener->iFsmContextParameterChangeStatus = KRequestPending;
	}


/**
 Wait for MSpudManInterface::Input to be called with aOperation KContextParametersChangeEvent
*/
void CSpudFsmUnitTestStepBase::WaitForFsmQosChangeNotification(TContextId aPdpId, const RPacketQoS::TQoSR99_R4Negotiated& aQosParams)
	{
	INFO_PRINTF2(_L("CSpudUnitTestStepBase::WaitForFsmQosChangeNotification: aPdpId = %d"), aPdpId);
	
	// the implementation of MSpudManInterface::Input that is called by the Etel driver will complete the iFsmContextParameterChangeStatus request
	User::WaitForRequest(iListener->iFsmContextParameterChangeStatus);
	
	// make sure the parameters are as expected
	TEST(iListener->iFsmContextParameterChangeStatus == KErrNone);
	TEST(iListener->iContextParametersEventData.iPdpId == aPdpId);
	TEST(CompareUMTSQoSNeg(aQosParams, iListener->iContextParametersEventData.iNegotiated.NegotiatedQoSR99_R4()));
	
	iListener->iFsmContextParameterChangeStatus = KRequestPending;
	}


/**
 Wait for context aPacketContext to have the given status
*/
void CSpudFsmUnitTestStepBase::WaitForGivenContextStatus(RPacketContext& aPacketContext, RPacketContext::TContextStatus aStatus)
	{
	INFO_PRINTF2(_L("CSpudUnitTestStepBase::WaitForGivenContextStatus: aStatus = %d"), aStatus);
	
 	TRequestStatus status;
	RPacketContext::TContextStatus contextStatus = RPacketContext::EStatusUnknown;
	RPacketContext::TContextStatus contextStatus1 = RPacketContext::EStatusUnknown;

	aPacketContext.NotifyStatusChange(status, contextStatus);
	TEST(aPacketContext.GetStatus(contextStatus1) == KErrNone);
	INFO_PRINTF2(_L("CSpudUnitTestStepBase::WaitForGivenContextStatus: contextStatus1 = %d"), contextStatus1);
	if (contextStatus1 != RPacketContext::EStatusDeleted)
		{
		while (contextStatus != aStatus)
			{
			User::WaitForRequest(status);
			INFO_PRINTF2(_L("CSpudUnitTestStepBase::WaitForGivenContextStatus: contextStatus = %d"), contextStatus);
			TEST(status == KErrNone);
			}
		}
	}
