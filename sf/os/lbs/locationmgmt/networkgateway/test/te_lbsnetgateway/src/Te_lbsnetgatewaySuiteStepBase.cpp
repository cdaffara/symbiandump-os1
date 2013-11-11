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

/**
 @file Te_lbsnetgatewaySuiteStepBase.cpp
*/

#include "Te_lbsnetgatewaySuiteStepBase.h"
#include "Te_lbsnetgatewaySuiteDefs.h"
#include "cprocesslaunch.h"
#include "tprocessstartparams.h"
#include "lbscommoninternaldatatypes.h"

#include <lbs/lbsgpsmeasurement.h>


_LIT(KNetGatewayExeName, "lbsnetgateway.exe");
//_LIT(KLbsNetgatewaySuiteStepBase,"LbsNetgatewaySuiteStepBase");

// Device driver constants

TVerdict CTe_lbsnetgatewaySuiteStepBase::doTestStepPreambleL()
/**
 * @return - TVerdict
 * Implementation of CTestStep base class virtual
 * It is used for doing all initialisation common to derived classes in here.
 * Make it being able to leave if there are any errors here as there's no point in
 * trying to run a test step if anything fails.
 * The leave will be picked up by the framework.
 */
	{

	// process some common pre setting to test steps then set SetTestStepResult to EFail or Epass.
	//INFO_PRINTF1(_L("Please delete this line or modify me!! I am in doTestStepPreambleL() of the class CTe_lbsnetgatewaySuiteStepBase!"));
	
	// Create Active Scheduler
	iScheduler = new (ELeave) CActiveScheduler;
	CActiveScheduler::Install(iScheduler);
	
	iSchedWait = new(ELeave) CActiveSchedulerWait;
	
	iAdmin = CLbsAdmin::NewL();
	
	SetTestStepResult(EPass);
	return TestStepResult();
	}

TVerdict CTe_lbsnetgatewaySuiteStepBase::doTestStepPostambleL()
/**
 * @return - TVerdict
 * Implementation of CTestStep base class virtual
 * It is used for doing all after test treatment common to derived classes in here.
 * Make it being able to leave
 * The leave will be picked up by the framework.
 */
	{

	// process some common post setting to test steps then set SetTestStepResult to EFail or Epass.
	//INFO_PRINTF1(_L("Please delete this line or modify me!! I am in doTestStepPostambleL() of the class CTe_lbsnetgatewaySuiteStepBase!"));
	
	delete iAdmin;
	iAdmin = NULL;
	
	delete iSchedWait;
	
	// remove active scheduler
	delete iScheduler;
	
	//SetTestStepResult(EPass);  // or EFail
	return TestStepResult();
	}

CTe_lbsnetgatewaySuiteStepBase::~CTe_lbsnetgatewaySuiteStepBase()
	{
	}

CTe_lbsnetgatewaySuiteStepBase::CTe_lbsnetgatewaySuiteStepBase()
	{
	}

void CTe_lbsnetgatewaySuiteStepBase::ProcessNetChannelMessage(RLbsNetChannel::TLbsNetChannelId aChannelId, const TLbsNetInternalMsgBase& aMessage)
	{
	INFO_PRINTF1(_L("CTe_lbsnetgatewaySuiteStepBase::ProcessNetChannelMessage"));
	if (aChannelId != RLbsNetChannel::EChannelNRH2NG)
		{
		INFO_PRINTF2(_L("Unexpected messahe channel id: %d"), aChannelId);
		SetTestStepResult(EFail);
		}
		
	INFO_PRINTF2(_L("Message Type (%d)"), aMessage.Type());	
	switch (aMessage.Type())
		{
		case TLbsNetLocationRequestMsg::ELocationRequest:
			{
			const TLbsNetLocationRequestMsg& msg = static_cast<const TLbsNetLocationRequestMsg&>(aMessage);
			INFO_PRINTF3(_L("SessionId (%d, %d)"), msg.SessionId().SessionOwner().iUid, 
												   msg.SessionId().SessionNum());
			INFO_PRINTF2(_L("Service Type (%d)"), msg.Service());
			INFO_PRINTF2(_L("IsEmergency (%d)"), msg.IsEmergency());
			//INFO_PRINTF2(_L("Quality (%d)"), msg.Quality());
			break;
			}
		case TLbsNetLocationRequestMsg::ELocationResponse:
			{
			INFO_PRINTF1(_L("Unexpected message type!\n"));
			break;
			}
		case TLbsNetLocationRequestMsg::ETransmitLocationRequest:
			{
			INFO_PRINTF1(_L("Unexpected message type!\n"));
			break;
			}
		case TLbsNetLocationRequestMsg::ETransmitLocationCancel:
			{
			INFO_PRINTF1(_L("Unexpected message type!\n"));
			break;
			}			
		case TLbsNetLocationRequestMsg::EPrivacyRequest:
			{
			const TLbsNetMtLrRequestMsg& msg = static_cast<const TLbsNetMtLrRequestMsg&>(aMessage);
			INFO_PRINTF3(_L("SessionId (%d, %d)"), msg.SessionId().SessionOwner().iUid, 
												   msg.SessionId().SessionNum());
			//INFO_PRINTF2(_L("Privacy (%d)"), msg.Privacy());
			//INFO_PRINTF2(_L("RequestInfo (%S)"), msg.RequestInfo());
			break;
			}
		case TLbsNetLocationRequestMsg::EPrivacyResponse:
			{
			INFO_PRINTF1(_L("Unexpected message type!\n"));
			break;
			}
		case TLbsNetLocationRequestMsg::ESessionComplete:
			{
			const TLbsNetSessionCompleteMsg& msg = static_cast<const TLbsNetSessionCompleteMsg&>(aMessage);
			INFO_PRINTF3(_L("SessionId (%d, %d)"), msg.SessionId().SessionOwner().iUid, 
												   msg.SessionId().SessionNum());
			INFO_PRINTF2(_L("Reason (%d)"), msg.Reason());
			break;
			}
		default:
			{
			INFO_PRINTF1(_L("Unknown message type!"));
			break;
			}
		}
	}

void CTe_lbsnetgatewaySuiteStepBase::AssistanceDataEvent(TInt aError, TLbsAsistanceDataGroup aMask)
	{
	INFO_PRINTF1(_L("CTe_lbsnetgatewaySuiteStepBase::AssistanceDataEvent"));
	INFO_PRINTF2(_L("Data group mask : (0x%x)"), aMask);
	INFO_PRINTF2(_L("Error code : (%d)"), aError);
	}

void CTe_lbsnetgatewaySuiteStepBase::ProcessNetProtocolMessage(const TNetGatewayMsg& aMessage)
	{
	INFO_PRINTF1(_L("CTe_lbsnetgatewaySuiteStepBase::ProcessNetProtocolMessage"));
	INFO_PRINTF2(_L("Message Type : (%d)"), aMessage.iType);
	switch (aMessage.iType)
		{
		case ENetMsgRespondPrivacyRequest:
			{
			const SNetMsgRespondPrivacyRequest* request = reinterpret_cast<const SNetMsgRespondPrivacyRequest*>(aMessage.Data());
			INFO_PRINTF3(_L("SessionId (%d, %d)"), request->iSessionId.SessionOwner().iUid, 
												   request->iSessionId.SessionNum());			
			break;
			}
		case ENetMsgRespondLocationRequest:
			{
			const SNetMsgRespondLocationRequest* request = reinterpret_cast<const SNetMsgRespondLocationRequest*>(aMessage.Data());
			INFO_PRINTF3(_L("SessionId (%d, %d)"), request->iSessionId.SessionOwner().iUid, 
												   request->iSessionId.SessionNum());
			INFO_PRINTF2(_L("Reason : (%d)"), request->iReason);
			// TODO printout iPositionInfo
			break;
			}
		case ENetMsgRequestTransmitLocation:
			{
			const SNetMsgRequestTransmitLocation* request = reinterpret_cast<const SNetMsgRequestTransmitLocation*>(aMessage.Data());
			INFO_PRINTF3(_L("SessionId (%d, %d)"), request->iSessionId.SessionOwner().iUid, 
												   request->iSessionId.SessionNum());
			INFO_PRINTF2(_L("Priority : (%d)"), request->iPriority);
			INFO_PRINTF2(_L("Destination : (%S)"), &request->iDestination);
			INFO_PRINTF2(_L("Technology type : (%d)"), request->iOptions.PosMode());
			break;
			}
		case ENetMsgCancelTransmitLocation:
			{
			const SNetMsgCancelTransmitLocation* cancel = reinterpret_cast<const SNetMsgCancelTransmitLocation*>(aMessage.Data());
			INFO_PRINTF3(_L("SessionId (%d, %d)"), cancel->iSessionId.SessionOwner().iUid, 
												   cancel->iSessionId.SessionNum());
			INFO_PRINTF2(_L("Reason : (%d)"), cancel->iReason);
			break;
			}
		case ENetMsgRequestAssistanceData:
			{
			const SNetMsgRequestAssistanceData* request = reinterpret_cast<const SNetMsgRequestAssistanceData*>(aMessage.Data());
			INFO_PRINTF2(_L("Request mask (0x%x)"), request->iDataRequestMask);
			break;
			}
		case ENetMsgRequestSelfLocation:
			{
			const SNetMsgRequestSelfLocation* request = reinterpret_cast<const SNetMsgRequestSelfLocation*>(aMessage.Data());
			INFO_PRINTF3(_L("SessionId (%d, %d)"), request->iSessionId.SessionOwner().iUid, 
												   request->iSessionId.SessionNum());
			INFO_PRINTF2(_L("New Client : (%d)"), request->iNewClient);
			INFO_PRINTF2(_L("Request mask : (0x%x)"), request->iDataRequestMask);
			break;
			}
		case ENetMsgCancelSelfLocation:
			{
			const SNetMsgCancelSelfLocation* cancel = reinterpret_cast<const SNetMsgCancelSelfLocation*>(aMessage.Data());
			INFO_PRINTF3(_L("SessionId (%d, %d)"), cancel->iSessionId.SessionOwner().iUid, 
												   cancel->iSessionId.SessionNum());
			INFO_PRINTF2(_L("Reason : (%d)"), cancel->iReason);
			break;
			}
		case ENetMsgRequestNetworkLocation:
			{
			break;
			}
		case ENetMsgCancelNetworkLocation:		
			{
			break;
			}
		case ENetMsgProcessPrivacyRequest:
			{
			const SNetMsgProcessPrivacyRequest* request = reinterpret_cast<const SNetMsgProcessPrivacyRequest*>(aMessage.Data());
			INFO_PRINTF3(_L("SessionId (%d, %d)"), request->iSessionId.SessionOwner().iUid, 
												   request->iSessionId.SessionNum());
			break;
			}
		case ENetMsgProcessLocationRequest:
			{
			const SNetMsgProcessLocationRequest* request = reinterpret_cast<const SNetMsgProcessLocationRequest*>(aMessage.Data());
			INFO_PRINTF3(_L("SessionId (%d, %d)"), request->iSessionId.SessionOwner().iUid, 
												   request->iSessionId.SessionNum());			
			break;
			}
		case ENetMsgProcessSessionComplete:
			{
			const SNetMsgProcessSessionComplete* complete = reinterpret_cast<const SNetMsgProcessSessionComplete*>(aMessage.Data());
			INFO_PRINTF3(_L("SessionId (%d, %d)"), complete->iSessionId.SessionOwner().iUid, 
												   complete->iSessionId.SessionNum());
			INFO_PRINTF2(_L("Reason : (%d)"), complete->iReason);			
			break;
			}
		case ENetMsgProcessAssistanceData:
			{
			break;
			}
		case ENetMsgProcessLocationUpdate:
			{
			break;
			}
		default:
			{
			INFO_PRINTF1(_L("Unknown message type!"));
			break;
			}
		}
	}

/* Default handler for receiving Network Location Responses.

These should only ever be 'Complete' messages - the other two types
('Request' and 'Cancel' are sent the other way, i.e. to the Network
Gateway, not from it).
*/
void CTe_lbsnetgatewaySuiteStepBase::ProcessNetworkLocationResponseMessage(const TLbsNetLocMsgBase& aMessage)
	{
	INFO_PRINTF1(_L("CTe_lbsnetgatewaySuiteStepBase::ProcessNetworkLocationResponseMessage"));
	INFO_PRINTF2(_L("Message Type : (%d)"), aMessage.Type());
	switch (aMessage.Type())
		{
		case TLbsNetLocMsgBase::ENetLocMsgNetworkLocationComplete:
			{
			const TLbsNetLocNetworkLocationCompleteMsg& msg = static_cast<const TLbsNetLocNetworkLocationCompleteMsg&>(aMessage);
			INFO_PRINTF2(_L("Reason : %d"), msg.Reason());
			break;
			}
		case TLbsNetLocMsgBase::ENetLocMsgNetworkLocationRequest:
		case TLbsNetLocMsgBase::ENetLocMsgNetworkLocationCancel:
		case TLbsNetLocMsgBase::ENetLocMsgUnknown:
		default:
			{
			INFO_PRINTF2(_L("Unexpected message type! (%d)"), aMessage.Type());
			break;
			}
		}
	}

/* Starts the LbsNetGateway executable that is being tested.
*/
TInt CTe_lbsnetgatewaySuiteStepBase::StartNetGateway()
	{
	// use the process launch code
	TProcessStartParams params;
	_LIT(KDummyProcessName, "lbsnetgateway");
	params.SetProcessFileName(KNetGatewayExeName);
	params.SetProcessName(KDummyProcessName);
	params.SetRendezvousRequired(ETrue);
	TInt r = CProcessLaunch::ProcessLaunch(params);
	// Let PMs load
	User::After(5*1000*1000);
	return r;
	}

/* Forces the shutdown of the LbsNetGateway executable.
*/
TInt CTe_lbsnetgatewaySuiteStepBase::StopNetGateway()
	{
	// Shutdown the network gateway via the process 
	// supervisor api that LbsRoot uses.
	TRAPD(err, iProcessSupervisor.OpenL(KLbsNetworkGatewayUid));
	if (err == KErrNone)
		{
		// Find the network gateway manager process
		RProcess process;
		TFullName fullName;
		TFindProcess findProcess;
		_LIT(KGatewayProcessPattern, "lbsnetgateway.exe*");
		TBool runningInstanceFound = EFalse;
		
		findProcess.Find(KGatewayProcessPattern);
		while (err = findProcess.Next(fullName), err == KErrNone)
			{
			TInt processOpenErr = process.Open(findProcess);
			if (processOpenErr == KErrNone)
				{
				TExitType exitType = process.ExitType();
				if (exitType == EExitPending)
					{
					// found a running instance of the process,
					// so assume everything is ok;
					runningInstanceFound = ETrue;
					break;
					}
				}
				process.Close();
			}
		
		if (runningInstanceFound)
			{
			TRequestStatus status;
			process.Logon(status);
			iProcessSupervisor.CloseDownProcess();
			User::WaitForRequest(status);
			process.Close();
			iProcessSupervisor.Close();
			}
		}
	
	return err;
	}

/* Function to compare the contents of messages that are sent from
   the NRH or SUPL PSY and received in the PM, or vice-versa.
*/
void CTe_lbsnetgatewaySuiteStepBase::TestMessagesAreEquivalentL(
		const TLbsNetInternalMsgBase& aNrhMsg,
		const TNetGatewayMsg& aPmMsg)
	{
	switch (aNrhMsg.Type())
		{
		// SUPL PSY Messages
        case TLbsNetInternalMsgBase::ECellLocationRequest:
            {
            TESTL(aPmMsg.iType == ENetMsgRequestNetworkLocation);
            const SNetMsgRequestTransmitLocation* data = reinterpret_cast<const SNetMsgRequestTransmitLocation*>(aPmMsg.Data());
            const TLbsCellLocationRequestMsg& msg = static_cast<const TLbsCellLocationRequestMsg&>(aNrhMsg);
            TESTL(data->iSessionId == TLbsNetSessionId(msg.SessionId().SessionOwner(), msg.SessionId().SessionNum()));
            //TESTL(data->iPriority == msg.Priority());

            break;
            }
        case TLbsNetInternalMsgBase::ECellLocationResponse:
            {
            TESTL(aPmMsg.iType == ENetMsgProcessLocationUpdate);
            const SNetMsgProcessLocationUpdate* data = reinterpret_cast<const SNetMsgProcessLocationUpdate*>(aPmMsg.Data());
            const TLbsCellLocationResponseMsg& msg = static_cast<const TLbsCellLocationResponseMsg&>(aNrhMsg);
            TESTL(data->iSessionId == TLbsNetSessionId(msg.SessionId().SessionOwner(), msg.SessionId().SessionNum()));
            TESTL(TestPositionInfosAreEqual(data->iReferenceLocation, msg.PositionInfo()));
            break;
            }
        case TLbsNetInternalMsgBase::ECellLocationCancel:
            {
            TESTL(aPmMsg.iType == ENetMsgCancelNetworkLocation);
            const SNetMsgCancelTransmitLocation* data = reinterpret_cast<const SNetMsgCancelTransmitLocation*>(aPmMsg.Data());
            const TLbsCellLocationCancelMsg& msg = static_cast<const TLbsCellLocationCancelMsg&>(aNrhMsg);
            TESTL(data->iSessionId == TLbsNetSessionId(msg.SessionId().SessionOwner(), msg.SessionId().SessionNum()));
            TESTL(data->iReason == msg.Reason());
            break;
            }

		// NRH Messages
		case TLbsNetInternalMsgBase::ELocationResponse:
			{
			TESTL(aPmMsg.iType == ENetMsgRespondLocationRequest);
			const SNetMsgRespondLocationRequest* data = reinterpret_cast<const SNetMsgRespondLocationRequest*>(aPmMsg.Data());
			const TLbsNetLocationResponseMsg& msg = static_cast<const TLbsNetLocationResponseMsg&>(aNrhMsg);
			TESTL(data->iSessionId == TLbsNetSessionId(msg.SessionId().SessionOwner(), msg.SessionId().SessionNum()));
			TESTL(data->iReason == msg.Reason());
			TESTL(TestPositionInfosAreEqual(data->iPositionInfo, msg.PositionInfo()));
			break;
			}
		case TLbsNetInternalMsgBase::ETransmitLocationRequest:
			{
			TESTL(aPmMsg.iType == ENetMsgRequestTransmitLocation);
			const SNetMsgRequestTransmitLocation* data = reinterpret_cast<const SNetMsgRequestTransmitLocation*>(aPmMsg.Data());
			const TLbsNetTransmitLocationRequestMsg& msg = static_cast<const TLbsNetTransmitLocationRequestMsg&>(aNrhMsg);
			TESTL(data->iSessionId == TLbsNetSessionId(msg.SessionId().SessionOwner(), msg.SessionId().SessionNum()));
			TESTL(data->iPriority == msg.Priority());
			// TODO: Protocol Module SPI expects a TDes (16bit), but it should be a TDes8 (8bit)?
			TBuf<128> buf;
			buf.Copy(msg.Destination());
			TESTL(data->iDestination == buf);
			TESTL(data->iOptions.PosMode() == msg.Options().PosMode());
			break;
			}
		case TLbsNetInternalMsgBase::ECancelFromPrivacyController:
			{
			TESTL(aPmMsg.iType == ENetMsgCancelExternalLocation);
			const SNetMsgCancelExternalLocation* data = reinterpret_cast<const SNetMsgCancelExternalLocation*>(aPmMsg.Data());
			const TLbsNetCancelFromPrivacyControllerMsg& msg = static_cast<const TLbsNetCancelFromPrivacyControllerMsg&>(aNrhMsg);
			TESTL(data->iSessionId == TLbsNetSessionId(msg.SessionId().SessionOwner(), msg.SessionId().SessionNum()));
			TESTL(data->iReason == msg.Reason());
			break;
			}
		case TLbsNetInternalMsgBase::ETransmitLocationCancel:
			{
			TESTL(aPmMsg.iType == ENetMsgCancelTransmitLocation);
			const SNetMsgCancelTransmitLocation* data = reinterpret_cast<const SNetMsgCancelTransmitLocation*>(aPmMsg.Data());
			const TLbsNetTransmitLocationCancelMsg& msg = static_cast<const TLbsNetTransmitLocationCancelMsg&>(aNrhMsg);
			TESTL(data->iSessionId == TLbsNetSessionId(msg.SessionId().SessionOwner(), msg.SessionId().SessionNum()));
			TESTL(data->iReason == msg.Reason());
			break;
			}
		case TLbsNetInternalMsgBase::EPrivacyResponse:
			{
			TESTL(aPmMsg.iType == ENetMsgRespondPrivacyRequest);
			const SNetMsgRespondPrivacyRequest* data = reinterpret_cast<const SNetMsgRespondPrivacyRequest*>(aPmMsg.Data());
			const TLbsNetMtLrReponseMsg& msg = static_cast<const TLbsNetMtLrReponseMsg&>(aNrhMsg);
			TESTL(data->iSessionId == TLbsNetSessionId(msg.SessionId().SessionOwner(), msg.SessionId().SessionNum()));
			TESTL(data->iResponse == msg.Response());
			break;
			}
		case TLbsNetInternalMsgBase::ELocationRequest:
			{
			TESTL(aPmMsg.iType == ENetMsgProcessLocationRequest);
			const SNetMsgProcessLocationRequest* locReq = reinterpret_cast<const SNetMsgProcessLocationRequest*>(aPmMsg.Data());
			const TLbsNetLocationRequestMsg& msg = static_cast<const TLbsNetLocationRequestMsg&>(aNrhMsg);
			TESTL(locReq->iSessionId == TLbsNetSessionId(msg.SessionId().SessionOwner(), msg.SessionId().SessionNum()));
			TESTL(locReq->iEmergency == msg.IsEmergency());
			TESTL(locReq->iService == msg.Service());
			TESTL(locReq->iQuality.MaxFixTime() == msg.Quality().MaxFixTime());
			TESTL(locReq->iQuality.MinHorizontalAccuracy() == msg.Quality().MinHorizontalAccuracy());
			TESTL(locReq->iQuality.MinVerticalAccuracy() == msg.Quality().MinVerticalAccuracy());
			TESTL(locReq->iMethod.NumPosMethods() == msg.Method().NumPosMethods());
			for (TInt i = 0; i < locReq->iMethod.NumPosMethods(); i++)
				{
				TLbsNetPosMethod locReqMethod;
				TLbsNetPosMethodInt msgMethod;
				locReq->iMethod.GetPosMethod(i, locReqMethod);
				msg.Method().GetPosMethod(i, msgMethod);
				TESTL(locReqMethod.PosMeans() == msgMethod.PosMeans());
				TESTL(locReqMethod.PosMode() == msgMethod.PosMode());
				}
			break;
			}
		case TLbsNetInternalMsgBase::EPrivacyRequest:
			{
			TESTL(aPmMsg.iType == ENetMsgProcessPrivacyRequest);
			const SNetMsgProcessPrivacyRequest* privReq = reinterpret_cast<const SNetMsgProcessPrivacyRequest*>(aPmMsg.Data());
			const TLbsNetMtLrRequestMsg& msg = static_cast<const TLbsNetMtLrRequestMsg&>(aNrhMsg);
			TESTL(privReq->iSessionId == TLbsNetSessionId(msg.SessionId().SessionOwner(), msg.SessionId().SessionNum()));
			TESTL(privReq->iEmergency == msg.IsEmergency());
			TESTL(privReq->iPrivacy.RequestAdvice() == msg.Privacy().RequestAdvice());
			TESTL(privReq->iPrivacy.RequestAction() == msg.Privacy().RequestAction());
			TBuf8<128> rxBuf;
			TBuf8<128> txBuf;
			privReq->iRequestInfo.GetRequesterId(txBuf);
			msg.RequestInfo().GetRequesterId(rxBuf);
			TESTL(txBuf == rxBuf);
			privReq->iRequestInfo.GetClientName(txBuf);
			msg.RequestInfo().GetClientName(rxBuf);
			TESTL(txBuf == rxBuf);
			privReq->iRequestInfo.GetClientExternalId(txBuf);
			msg.RequestInfo().GetClientExternalId(rxBuf);
			TESTL(txBuf == rxBuf);			
			break;
			}
		case TLbsNetInternalMsgBase::ESessionComplete:
			{
			TESTL(aPmMsg.iType == ENetMsgProcessSessionComplete);
			const SNetMsgProcessSessionComplete* sessComp = reinterpret_cast<const SNetMsgProcessSessionComplete*>(aPmMsg.Data());
			const TLbsNetSessionCompleteMsg& msg = static_cast<const TLbsNetSessionCompleteMsg&>(aNrhMsg);
			TESTL(sessComp->iSessionId == TLbsNetSessionId(msg.SessionId().SessionOwner(), msg.SessionId().SessionNum()));
			TESTL(sessComp->iReason == msg.Reason());
			break;
			}
			
		case TLbsNetInternalMsgBase::ENetworkLocationUpdate:
			{
			TESTL(aPmMsg.iType == ENetMsgProcessLocationUpdate);
			}
			break;
		default:
			{
			INFO_PRINTF1(_L("Unexpected call to CTe_lbsnetgatewaySuiteStepBase::TestMessagesAreEquivalent!"));
			TESTL(EFalse);
			break;
			}
		}
		
	if (EFail == TestStepResult())
		{
		CTe_lbsnetgatewaySuiteStepBase::ProcessNetChannelMessage(RLbsNetChannel::EChannelNRH2NG, aNrhMsg);
		CTe_lbsnetgatewaySuiteStepBase::ProcessNetProtocolMessage(aPmMsg);
		}
	}
	
void CTe_lbsnetgatewaySuiteStepBase::TestMessagesAreEquivalentL(const TLbsNetLocMsgBase& aNetLocMsg,
															    const TNetGatewayMsg& aPmMsg)
	{	
	switch (aNetLocMsg.Type())
		{
		case TLbsNetLocMsgBase::ENetLocMsgNetworkLocationRequest:
			{
			TESTL(aPmMsg.iType == ENetMsgRequestNetworkLocation);
			// TODO: Further tests
			break;
			}
		case TLbsNetLocMsgBase::ENetLocMsgNetworkLocationCancel:
			{
			TESTL(aPmMsg.iType == ENetMsgCancelNetworkLocation);
			// TODO: Further tests
			break;
			}
		case TLbsNetLocMsgBase::ENetLocMsgNetworkLocationComplete:
			{
			TESTL(aPmMsg.iType == ENetMsgProcessSessionComplete);
			const TLbsNetLocNetworkLocationCompleteMsg& msg = static_cast<const TLbsNetLocNetworkLocationCompleteMsg&>(aNetLocMsg);
			const SNetMsgProcessSessionComplete* comp = reinterpret_cast<const SNetMsgProcessSessionComplete*>(aPmMsg.Data());
			TESTL(comp->iSessionId == TLbsNetSessionId(msg.SessionId().SessionOwner(), msg.SessionId().SessionNum()));
			TESTL(comp->iReason == msg.Reason());			
			break;
			}		
		case TLbsNetLocMsgBase::ENetLocMsgUnknown:
		default:
			{
			INFO_PRINTF1(_L("Unexpected call to CTe_lbsnetgatewaySuiteStepBase::TestMessagesAreEquivalent!"));
			TESTL(EFalse);
			break;
			}
		}

	if (EFail == TestStepResult())
		{
		CTe_lbsnetgatewaySuiteStepBase::ProcessNetworkLocationResponseMessage(aNetLocMsg);
		CTe_lbsnetgatewaySuiteStepBase::ProcessNetProtocolMessage(aPmMsg);
		}	
	}

TBool CTe_lbsnetgatewaySuiteStepBase::TestPositionInfosAreEqual(
		const TPositionInfoBase& aPosInfo1,
		const TPositionInfoBase& aPosInfo2)
	{
	TBool equal(EFalse);
	
	equal = (aPosInfo1.PositionClassType() == aPosInfo2.PositionClassType());
	equal &= (aPosInfo1.PositionClassSize() == aPosInfo2.PositionClassSize());
	equal &= (aPosInfo1.ModuleId().iUid == aPosInfo2.ModuleId().iUid);
	equal &= (aPosInfo1.PositionMode() == aPosInfo2.PositionMode());
	equal &= (aPosInfo1.PositionModeReason() == aPosInfo2.PositionModeReason());
	equal &= (aPosInfo1.UpdateType() == aPosInfo2.UpdateType());
	
	// Test the value of the TPositionInfo data if it is there
	if ((aPosInfo1.PositionClassType() & EPositionInfoClass) == EPositionInfoClass)
		{
		TPosition pos1;
		TPosition pos2;
		reinterpret_cast<const TPositionInfo&>(aPosInfo1).GetPosition(pos1);
		reinterpret_cast<const TPositionInfo&>(aPosInfo2).GetPosition(pos2);
		
		equal &= TestTReal64Equal(pos1.Latitude(), pos2.Latitude());
		equal &= TestTReal64Equal(pos1.Longitude(), pos2.Longitude());
		equal &= TestTReal32Equal(pos1.Altitude(), pos1.Altitude());
		equal &= TestTReal32Equal(pos1.HorizontalAccuracy(), pos1.HorizontalAccuracy());
		equal &= TestTReal32Equal(pos1.VerticalAccuracy(), pos1.VerticalAccuracy());
		equal &= pos1.Time() == pos2.Time();
		}
			
	// Test the value of the TPositionCourseInfo data if it is there
	if ((aPosInfo1.PositionClassType() & EPositionCourseInfoClass) == EPositionCourseInfoClass)
		{
		TCourse course1;
		TCourse course2;
		reinterpret_cast<const TPositionCourseInfo&>(aPosInfo1).GetCourse(course1);
		reinterpret_cast<const TPositionCourseInfo&>(aPosInfo2).GetCourse(course2);
		
		equal &= TestTReal32Equal(course1.Heading(), course2.Heading());
		equal &= TestTReal32Equal(course1.Speed(), course2.Speed());
		equal &= TestTReal32Equal(course1.HeadingAccuracy(), course2.HeadingAccuracy());
		equal &= TestTReal32Equal(course1.SpeedAccuracy(), course2.SpeedAccuracy());
		}

	// Test the value of the TPositionSatelliteInfo data if it is there
	if ((aPosInfo1.PositionClassType() & EPositionSatelliteInfoClass) == EPositionSatelliteInfoClass)
		{
		const TPositionSatelliteInfo& satPosInfo1 = reinterpret_cast<const TPositionSatelliteInfo&>(aPosInfo1);
		const TPositionSatelliteInfo& satPosInfo2 = reinterpret_cast<const TPositionSatelliteInfo&>(aPosInfo2);
		
		equal &= satPosInfo1.NumSatellitesInView() == satPosInfo2.NumSatellitesInView();
		equal &= satPosInfo1.NumSatellitesUsed() == satPosInfo2.NumSatellitesUsed();
		equal &= TestTReal32Equal(satPosInfo1.HorizontalDoP(), satPosInfo2.HorizontalDoP());
		equal &= TestTReal32Equal(satPosInfo1.VerticalDoP(), satPosInfo2.VerticalDoP());
		equal &= TestTReal32Equal(satPosInfo1.TimeDoP(), satPosInfo2.TimeDoP());
		equal &= satPosInfo1.SatelliteTime() == satPosInfo2.SatelliteTime();
		
		//TODO: check each set of satellite data
		}

	// Test the value of the TPositionGpsMeasurementInfo data if it is there
	if ((aPosInfo1.PositionClassType() & EPositionGpsMeasurementInfoClass) == EPositionGpsMeasurementInfoClass)
		{
		const TPositionGpsMeasurementInfo& gpsPosInfo1 = reinterpret_cast<const TPositionGpsMeasurementInfo&>(aPosInfo1);
		const TPositionGpsMeasurementInfo& gpsPosInfo2 = reinterpret_cast<const TPositionGpsMeasurementInfo&>(aPosInfo2);
		
		equal &= gpsPosInfo1.NumMeasurements() == gpsPosInfo2.NumMeasurements();
		equal &= gpsPosInfo1.GpsTimeOfWeek() == gpsPosInfo2.GpsTimeOfWeek();
		
		//TODO: check each set of measurement data
		}
		
	return equal;
	}

/* Test two TReal32's are equal. Includes extra test if both are NaN.

Need to do this because if both numbers are NaN then (aReal1 == aReal2)
returns EFalse (why?!?).
*/
TBool CTe_lbsnetgatewaySuiteStepBase::TestTReal32Equal(TReal32 aReal1, TReal32 aReal2)
	{
	return ((aReal1 == aReal2) || (Math::IsNaN(aReal1) && Math::IsNaN(aReal2)));
	}

/* Test two TReal64's are equal. Includes extra test if both are NaN.

Need to do this because if both numbers are NaN then (aReal1 == aReal2)
returns EFalse (why?!?).
*/
TBool CTe_lbsnetgatewaySuiteStepBase::TestTReal64Equal(TReal64 aReal1, TReal64 aReal2)
	{
	return ((aReal1 == aReal2) || (TRealX(aReal1).IsNaN() && TRealX(aReal2).IsNaN()));
	}

/* Parse the .ini file for this test step to get LbsAdmin settings
*/
void CTe_lbsnetgatewaySuiteStepBase::ParseAdminSettingsFromIniL()
	{
	CLbsAdmin* admin = CLbsAdmin::NewL();
	CleanupStack::PushL(admin);
	
	_LIT(KHomeProtocolModuleUid, "HomeProtocolModuleUid");
	TPtrC testModuleIdPtr;
	TUint32 testModuleId;
	if (GetStringFromConfig(ConfigSection(), KHomeProtocolModuleUid, testModuleIdPtr))
		{
		TLex lex(testModuleIdPtr);
		User::LeaveIfError(lex.Val(testModuleId, EHex));
		}
	else
		{
		// Set the Uid of the stub protocol module plugin
		// to be used for these unit tests.
		testModuleId = 0x10282250;
		}
	
	TLbsAdminProtocolModulesInfo info;
	TLbsAdminProtocolModulesInfo::TLbsProtocolModuleIdArray uidArray;
	uidArray[0] = TUid::Uid(testModuleId);
	info.SetModuleIds(uidArray,1);
	User::LeaveIfError(admin->Set(KLbsSettingHomeProtocolModule, info));
	User::LeaveIfError(admin->Set(KLbsSettingRoamingProtocolModule, info));
	
	_LIT(KRoamingProtocolModuleUid, "RoamingProtocolModuleUid");
	if (GetStringFromConfig(ConfigSection(), KRoamingProtocolModuleUid, testModuleIdPtr))
		{
		TLex lex(testModuleIdPtr);
		User::LeaveIfError(lex.Val(testModuleId, EHex));
		}
	else
		{
		// Set the Uid of the stub protocol module plugin
		// to be used for these unit tests.
		testModuleId = 0x10282250;
		}
	User::LeaveIfError(admin->Set(KLbsSettingRoamingProtocolModule, TUid::Uid(testModuleId)));
	
	_LIT(KMaxNumPrivacyRequestsKeyName, "MaxNumPrivacyRequests");
	TInt maxNumPrivacyReqs;
	if (GetIntFromConfig(ConfigSection(), KMaxNumPrivacyRequestsKeyName, maxNumPrivacyReqs))
		{
		User::LeaveIfError(admin->Set(KLbsSettingMaximumExternalLocateRequests, (TUint)maxNumPrivacyReqs));
		}
	
	CleanupStack::PopAndDestroy(admin);
	}

//
// CNetGatewayChannel
//

CTe_lbsnetgatewaySuiteStepBase::CNetGatewayChannel::CNetGatewayChannel() :
	CActive(EPriorityHigh)
	{
	CActiveScheduler::Add(this);
	}
	
CTe_lbsnetgatewaySuiteStepBase::CNetGatewayChannel::~CNetGatewayChannel()
	{
	Cancel();
	iNetGatewayChannel.Close();
	}
	
CTe_lbsnetgatewaySuiteStepBase::CNetGatewayChannel* CTe_lbsnetgatewaySuiteStepBase::CNetGatewayChannel::NewL(MLbsNetChannelObserver& aObserver, RLbsNetChannel::TLbsNetChannelId aChannelId)
	{
	CNetGatewayChannel* self = new (ELeave) CNetGatewayChannel();
	CleanupStack::PushL(self);
	self->ConstructL(aObserver, aChannelId);
	CleanupStack::Pop(self);
	return self;
	}
	
void CTe_lbsnetgatewaySuiteStepBase::CNetGatewayChannel::ConstructL(MLbsNetChannelObserver& aObserver, RLbsNetChannel::TLbsNetChannelId aChannelId)
	{
	iNetGatewayChannel.OpenL(aChannelId, aObserver);
	}
		
void CTe_lbsnetgatewaySuiteStepBase::CNetGatewayChannel::RunL()
	{
	User::LeaveIfError(iStatus.Int());
	if (iActiveWait.IsStarted())
		{
		iActiveWait.AsyncStop();
		}
	}
	
void CTe_lbsnetgatewaySuiteStepBase::CNetGatewayChannel::DoCancel()
	{
	iNetGatewayChannel.CancelSendMessageNotification();
	}

void CTe_lbsnetgatewaySuiteStepBase::CNetGatewayChannel::SendMessageAndWaitForResponse(const TLbsNetInternalMsgBase& aMessage)
	{
	iNetGatewayChannel.SendMessage(aMessage, iStatus);
	SetActive();
	iActiveWait.Start();
	}

//
// CPsyToNgChannel
//
CTe_lbsnetgatewaySuiteStepBase::CPsyToNgChannel::CPsyToNgChannel() :
    CActive(EPriorityHigh)
    {
    CActiveScheduler::Add(this);
    }
    
CTe_lbsnetgatewaySuiteStepBase::CPsyToNgChannel::~CPsyToNgChannel()
    {
    Cancel();
    iPsyGatewayChannel.Close();
    }
    
CTe_lbsnetgatewaySuiteStepBase::CPsyToNgChannel* CTe_lbsnetgatewaySuiteStepBase::CPsyToNgChannel::NewL(MLbsNetChannelObserver& aObserver, RLbsNetChannel::TLbsNetChannelId aChannelId)
    {
    CPsyToNgChannel* self = new (ELeave) CPsyToNgChannel();
    CleanupStack::PushL(self);
    self->ConstructL(aObserver, aChannelId);
    CleanupStack::Pop(self);
    return self;
    }
    
void CTe_lbsnetgatewaySuiteStepBase::CPsyToNgChannel::ConstructL(MLbsNetChannelObserver& aObserver, RLbsNetChannel::TLbsNetChannelId aChannelId)
    {
    iPsyGatewayChannel.OpenL(aChannelId, aObserver);
    }
        
void CTe_lbsnetgatewaySuiteStepBase::CPsyToNgChannel::RunL()
    {
    User::LeaveIfError(iStatus.Int());
    if (iPsyActiveWait.IsStarted())
        {
        iPsyActiveWait.AsyncStop();
        }
    }
    
void CTe_lbsnetgatewaySuiteStepBase::CPsyToNgChannel::DoCancel()
    {
    iPsyGatewayChannel.CancelSendMessageNotification();
    }

void CTe_lbsnetgatewaySuiteStepBase::CPsyToNgChannel::SendMessageAndWaitForResponse(const TLbsNetInternalMsgBase& aMessage)
    {
    iPsyGatewayChannel.SendMessage(aMessage, iStatus);
    SetActive();
    iPsyActiveWait.Start();
    }

//
// CNetworkLocationChannel
//

CNetworkLocationChannel::CNetworkLocationChannel(MNetworkLocationChannelObserver& aObserver) :
	CActive(EPriorityHigh),
	iObserver(aObserver)
	{
	CActiveScheduler::Add(this);
	}
	
CNetworkLocationChannel::~CNetworkLocationChannel()
	{
	Cancel();
	iResponses.Close();
	iRequests.Close();
	}
	
CNetworkLocationChannel* CNetworkLocationChannel::NewL(MNetworkLocationChannelObserver& aObserver)
	{
	CNetworkLocationChannel* self = new (ELeave) CNetworkLocationChannel(aObserver);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;
	}
	
void CNetworkLocationChannel::ConstructL()
	{
	iRequests.OpenL(KTe_lbsnetgatewaySuiteUid);
	iResponses.OpenL(KTe_lbsnetgatewaySuiteUid);
	NotifyNetworkLocationMessage();
	}
		
void CNetworkLocationChannel::RunL()
	{
	User::LeaveIfError(iStatus.Int());
	
	NotifyNetworkLocationMessage();
	// This is the largest network location message type, to allow
	// us to retrieve any message from the buffer.
	// Really, this interface needs to be refactored...
	const TLbsNetSessionIdInt KInvalidSessionId;
	TLbsNetLocNetworkLocationRequestMsg msg(KInvalidSessionId,
											EFalse,
											TLbsNetPosRequestQualityInt());
	User::LeaveIfError(iResponses.GetNetworkLocationResponse(msg));
	iObserver.ProcessNetworkLocationResponseMessage(msg);
	}
	
void CNetworkLocationChannel::DoCancel()
	{
	iResponses.CancelNotifyNetworkLocationResponseUpdate();
	}

TInt CNetworkLocationChannel::SendNetworkLocationMessage(const TLbsNetLocMsgBase& aMessage)
	{
	return iRequests.SetNetworkLocationRequest(aMessage);
	}

void CNetworkLocationChannel::NotifyNetworkLocationMessage()
	{
	__ASSERT_DEBUG(!IsActive(), User::Invariant());
	iResponses.NotifyNetworkLocationResponseUpdate(iStatus);
	SetActive();
	}



/*
 * Initialise arrays of PM UIDs to beused in testing. 
 */

void CTe_lbsnetgatewaySuiteStepBase::InitialiseUidArray()
    {
    // Initialise array of Uids to make loops clean
    
    TInt32 IntOfUid = KTe_LbsNetGatewayTestProtocolUid3.iUid;
    
    iUidHomeArray[0] = KTe_LbsNetGatewayTestProtocolUid;
    iUidHomeArray[1] = KTe_LbsNetGatewayTestProtocolUid2;
    iUidRoamingArray[0] = KTe_LbsNetGatewayTestProtocolUid11;
    iUidRoamingArray[1] = KTe_LbsNetGatewayTestProtocolUid12;
    
    // From this point on values are in sequence so we looop and increment
    for(TInt count = 2 ; count <= 10 ; count++)
        {
        iUidHomeArray[count] = TUid::Uid(IntOfUid);
        iUidRoamingArray[count] = TUid::Uid(IntOfUid+10);
        IntOfUid++;
        }

    }


/*
 * Loop to initialise the communication channels for each PM
 * Creates communication mechanism for each protocol loaded.
 * Allows us to send a message to a specific protocol.
 * Each index is related to an implementation Uid.
 */
void CTe_lbsnetgatewaySuiteStepBase::InitialiseStubPMChannels()
    {
   
    iStubPMChannelArray = new RNetGatewayProtocolTestChannel[20];
    for(TUint i=KTe_LbsNetGatewayTestProtocolIndex ; i<=KTe_LbsNetGatewayTestProtocolIndex20 ; i++)
        {
        iStubPMChannelArray[i].OpenL(KNetProtocolChannelKey, KTestAppChannelKey, *this, i);
        }
    }

/*
 * Loop to close the communication channels for each PM
 */
void CTe_lbsnetgatewaySuiteStepBase::CloseStubPMChannels()
    {
    for(TUint i=KTe_LbsNetGatewayTestProtocolIndex ; i<=KTe_LbsNetGatewayTestProtocolIndex20 ; i++)
        {
            iStubPMChannelArray[i].Close();
        }
    }
