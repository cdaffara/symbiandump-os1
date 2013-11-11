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
 @file Te_lbsnetinternaltestSuiteStepBase.cpp
*/

#include "Te_lbsnetinternaltestSuiteStepBase.h"
#include "Te_lbsnetinternaltestSuiteDefs.h"

// Device driver constants

TVerdict CTe_lbsnetinternaltestSuiteStepBase::doTestStepPreambleL()
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
	
	iScheduler = new (ELeave) CActiveScheduler;
	CActiveScheduler::Install(iScheduler);
		
	RLbsNetChannel::InitializeL(RLbsNetChannel::EChannelNG2NRH);
	RLbsNetChannel::InitializeL(RLbsNetChannel::EChannelNRH2NG);
	RLbsNetChannel::InitializeL(RLbsNetChannel::EChannelNG2SUPLPSY);
	RLbsNetChannel::InitializeL(RLbsNetChannel::EChannelSUPLPSY2NG);
	RLbsNetworkLocationRequests::InitializeL();
	RLbsNetworkLocationResponses::InitializeL();
	
	SetTestStepResult(EPass);
	return TestStepResult();
	}

TVerdict CTe_lbsnetinternaltestSuiteStepBase::doTestStepPostambleL()
/**
 * @return - TVerdict
 * Implementation of CTestStep base class virtual
 * It is used for doing all after test treatment common to derived classes in here.
 * Make it being able to leave
 * The leave will be picked up by the framework.
 */
	{

	// process some common post setting to test steps then set SetTestStepResult to EFail or Epass.
	
	RLbsNetworkLocationResponses::ShutDownL();
	RLbsNetworkLocationRequests::ShutDownL();
	RLbsNetChannel::ShutDownL(RLbsNetChannel::EChannelNRH2NG);
	RLbsNetChannel::ShutDownL(RLbsNetChannel::EChannelNG2NRH);
	RLbsNetChannel::ShutDownL(RLbsNetChannel::EChannelSUPLPSY2NG);
	RLbsNetChannel::ShutDownL(RLbsNetChannel::EChannelNG2SUPLPSY);
	
	delete iScheduler;
	
	//SetTestStepResult(EPass);  // or EFail
	return TestStepResult();
	}

CTe_lbsnetinternaltestSuiteStepBase::~CTe_lbsnetinternaltestSuiteStepBase()
	{
	}

CTe_lbsnetinternaltestSuiteStepBase::CTe_lbsnetinternaltestSuiteStepBase()
	{
	}

/** Logs the contents of the given TLbsNetInternalMsgBase to the test execute logger.
*/
void CTe_lbsnetinternaltestSuiteStepBase::LogMessageInfo(RLbsNetChannel::TLbsNetChannelId aChannelId, const TLbsNetInternalMsgBase& aMessage)
	{
	switch (aChannelId)
		{
		case (RLbsNetChannel::EChannelNRH2NG):
			INFO_PRINTF1(_L("Message Received on channel EChannelNRH2NG"));
			break;
			
		case (RLbsNetChannel::EChannelNG2NRH):
			INFO_PRINTF1(_L("Message Received on channel EChannelNG2NRH"));
			break;
			
		default:
			INFO_PRINTF2(_L("Message Received on an unknown channel - %d"), aChannelId);
			SetTestStepResult(EFail);
			break;
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
			const TLbsNetLocationResponseMsg& msg = static_cast<const TLbsNetLocationResponseMsg&>(aMessage);
			INFO_PRINTF3(_L("SessionId (%d, %d)"), msg.SessionId().SessionOwner().iUid, 
												   msg.SessionId().SessionNum());
			// TODO : INFO_PRINTF2(_L("SessionId (%d)"), msg.Position());
			// TODO : INFO_PRINTF2(_L("SessionId (%d)"), msg.TimeStamp());
			break;
			}
		case TLbsNetLocationRequestMsg::ETransmitLocationRequest:
			{
			const TLbsNetTransmitLocationRequestMsg& msg = static_cast<const TLbsNetTransmitLocationRequestMsg&>(aMessage);
			INFO_PRINTF3(_L("SessionId (%d, %d)"), msg.SessionId().SessionOwner().iUid, 
												   msg.SessionId().SessionNum());
			INFO_PRINTF2(_L("Destination (%S)"), &msg.Destination());
			INFO_PRINTF2(_L("Priority (%d)"), msg.Priority());
			break;
			}
		case TLbsNetLocationRequestMsg::EPrivacyRequest:
			{
			const TLbsNetMtLrRequestMsg& msg = static_cast<const TLbsNetMtLrRequestMsg&>(aMessage);
			INFO_PRINTF3(_L("SessionId (%d, %d)"), msg.SessionId().SessionOwner().iUid, 
												   msg.SessionId().SessionNum());
			INFO_PRINTF2(_L("Privacy Advice(%d)"), msg.Privacy().RequestAdvice());
			INFO_PRINTF2(_L("Privacy Action(%d)"), msg.Privacy().RequestAction());
			// TODO: output external requester info
			break;
			}
		case TLbsNetLocationRequestMsg::EPrivacyResponse:
			{
			const TLbsNetMtLrReponseMsg& msg = static_cast<const TLbsNetMtLrReponseMsg&>(aMessage);
			INFO_PRINTF3(_L("SessionId (%d, %d)"), msg.SessionId().SessionOwner().iUid, 
												   msg.SessionId().SessionNum());
			INFO_PRINTF2(_L("Response (%d)"), msg.Response());
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

/** Default message handler for messages from the network gateway. Simply logs the message data.
*/
void CTe_lbsnetinternaltestSuiteStepBase::ProcessNetChannelMessage(RLbsNetChannel::TLbsNetChannelId aChannelId, const TLbsNetInternalMsgBase& aMessage)
	{
	INFO_PRINTF1(_L("CTe_lbsnetinternaltestSuiteStepBase::ProcessChannelMessage : "));
	LogMessageInfo(aChannelId, aMessage);
	CActiveScheduler::Stop();
	}

void CTe_lbsnetinternaltestSuiteStepBase::TestNetInternalMessagesEquivalentL(
		const TLbsNetInternalMsgBase& aMsg,
		const TLbsNetInternalMsgBase& aMsg2)
	{
	TESTL(aMsg.Type() == aMsg2.Type());
	
	switch (aMsg.Type())
		{
		case TLbsNetInternalMsgBase::ELocationRequest:
			{
			const TLbsNetLocationRequestMsg& msg = static_cast<const TLbsNetLocationRequestMsg&>(aMsg);
			const TLbsNetLocationRequestMsg& msg2 = static_cast<const TLbsNetLocationRequestMsg&>(aMsg2);
			TESTL(msg.SessionId() == msg2.SessionId());
			TESTL(msg.IsEmergency() == msg2.IsEmergency());
			TESTL(msg.Service() == msg2.Service());
			TESTL(msg.Quality().MaxFixTime() == msg2.Quality().MaxFixTime());
			TESTL(msg.Quality().MinHorizontalAccuracy() == msg2.Quality().MinHorizontalAccuracy());
			TESTL(msg.Quality().MinVerticalAccuracy() == msg2.Quality().MinVerticalAccuracy());			
			break;
			}
		case TLbsNetInternalMsgBase::ELocationResponse:
			{
			const TLbsNetLocationResponseMsg& msg = static_cast<const TLbsNetLocationResponseMsg&>(aMsg);
			const TLbsNetLocationResponseMsg& msg2 = static_cast<const TLbsNetLocationResponseMsg&>(aMsg2);
			TESTL(msg.SessionId() == msg2.SessionId());
			TESTL(msg.TimeStamp() == msg2.TimeStamp());
			TESTL(msg.Reason() == msg2.Reason());
			const TPositionInfo& posInfo = static_cast<const TPositionInfo&>(msg.PositionInfo());
			const TPositionInfo& posInfo2 = static_cast<const TPositionInfo&>(msg2.PositionInfo());
			TPosition pos;
			TPosition pos2;
			posInfo.GetPosition(pos);
			posInfo2.GetPosition(pos2);
			TESTL(pos.Time() == pos2.Time());
			TESTL(pos.HorizontalAccuracy() == pos2.HorizontalAccuracy());
			TESTL(pos.VerticalAccuracy() == pos2.VerticalAccuracy());
			TESTL(pos.Latitude() == pos2.Latitude());
			TESTL(pos.Longitude() == pos2.Longitude());
			TESTL(pos.Altitude() == pos2.Altitude());
			TESTL(pos.Datum() == pos2.Datum());
			break;
			}
		case TLbsNetInternalMsgBase::ETransmitLocationRequest:
			{
			const TLbsNetTransmitLocationRequestMsg& msg = static_cast<const TLbsNetTransmitLocationRequestMsg&>(aMsg);
			const TLbsNetTransmitLocationRequestMsg& msg2 = static_cast<const TLbsNetTransmitLocationRequestMsg&>(aMsg2);
			TESTL(msg.SessionId() == msg2.SessionId());
			TESTL(msg.Priority() == msg2.Priority());
			TESTL(msg.Destination() == msg2.Destination());
			break;
			}
		case TLbsNetInternalMsgBase::ETransmitLocationCancel:
			{
			const TLbsNetTransmitLocationCancelMsg& msg = static_cast<const TLbsNetTransmitLocationCancelMsg&>(aMsg);
			const TLbsNetTransmitLocationCancelMsg& msg2 = static_cast<const TLbsNetTransmitLocationCancelMsg&>(aMsg2);
			TESTL(msg.SessionId() == msg2.SessionId());
			TESTL(msg.Reason() == msg2.Reason());
			break;
			}
		case TLbsNetInternalMsgBase::EPrivacyRequest:
			{
			const TLbsNetMtLrRequestMsg& msg = static_cast<const TLbsNetMtLrRequestMsg&>(aMsg);
			const TLbsNetMtLrRequestMsg& msg2 = static_cast<const TLbsNetMtLrRequestMsg&>(aMsg2);
			TESTL(msg.SessionId() == msg2.SessionId());
			TESTL(msg.IsEmergency() == msg2.IsEmergency());
			TESTL(msg.Privacy().RequestAdvice() == msg2.Privacy().RequestAdvice());
			TESTL(msg.Privacy().RequestAction() == msg2.Privacy().RequestAction());
			TBuf8<128> buf;
			TBuf8<128> buf2;
			msg.RequestInfo().GetRequesterId(buf);
			msg2.RequestInfo().GetRequesterId(buf2);
			TESTL(buf == buf2);
			msg.RequestInfo().GetClientName(buf);
			msg2.RequestInfo().GetClientName(buf2);
			TESTL(buf == buf2);
			msg.RequestInfo().GetClientExternalId(buf);
			msg2.RequestInfo().GetClientExternalId(buf2);
			TESTL(buf == buf2);
			break;
			}
		case TLbsNetInternalMsgBase::EPrivacyResponse:
			{
			const TLbsNetMtLrReponseMsg& msg = static_cast<const TLbsNetMtLrReponseMsg&>(aMsg);
			const TLbsNetMtLrReponseMsg& msg2 = static_cast<const TLbsNetMtLrReponseMsg&>(aMsg2);
			TESTL(msg.SessionId() == msg2.SessionId());
			TESTL(msg.Response() == msg2.Response());
			break;
			}
		case TLbsNetInternalMsgBase::ESessionComplete:
			{
			const TLbsNetSessionCompleteMsg& msg = static_cast<const TLbsNetSessionCompleteMsg&>(aMsg);
			const TLbsNetSessionCompleteMsg& msg2 = static_cast<const TLbsNetSessionCompleteMsg&>(aMsg2);
			TESTL(msg.SessionId() == msg2.SessionId());
			TESTL(msg.Reason() == msg2.Reason());
			break;
			}
		case TLbsNetInternalMsgBase::ECancelFromPrivacyController:
			{
			const TLbsNetCancelFromPrivacyControllerMsg& msg = static_cast<const TLbsNetCancelFromPrivacyControllerMsg&>(aMsg);
			const TLbsNetCancelFromPrivacyControllerMsg& msg2 = static_cast<const TLbsNetCancelFromPrivacyControllerMsg&>(aMsg2);
			TESTL(msg.SessionId() == msg2.SessionId());
			TESTL(msg.Reason() == msg2.Reason());
			break;
			}
		
		case TLbsNetInternalMsgBase::ECellLocationRequest:
            {
            const TLbsCellLocationRequestMsg& msg = static_cast<const TLbsCellLocationRequestMsg&>(aMsg);
            const TLbsCellLocationRequestMsg& msg2 = static_cast<const TLbsCellLocationRequestMsg&>(aMsg2);
            TESTL(msg.SessionId() == msg2.SessionId());
            TESTL(msg.Options().NewClientConnected() == msg2.Options().NewClientConnected());
            TESTL(msg.ProtocolModule() == msg2.ProtocolModule());
            break;
            }
	   case TLbsNetInternalMsgBase::ECellLocationCancel:
            {
            const TLbsCellLocationCancelMsg& msg = static_cast<const TLbsCellLocationCancelMsg&>(aMsg);
            const TLbsCellLocationCancelMsg& msg2 = static_cast<const TLbsCellLocationCancelMsg&>(aMsg2);
            TESTL(msg.SessionId() == msg2.SessionId());
            TESTL(msg.Reason() == msg2.Reason());
            break;
            }
		
	   case TLbsNetInternalMsgBase::ECellLocationResponse:
           {
           const TLbsCellLocationResponseMsg& msg = static_cast<const TLbsCellLocationResponseMsg&>(aMsg);
           const TLbsCellLocationResponseMsg& msg2 = static_cast<const TLbsCellLocationResponseMsg&>(aMsg2);
           TESTL(msg.SessionId() == msg2.SessionId());
           TESTL(msg.Reason() == msg2.Reason());
           const TPositionInfo& posInfo = static_cast<const TPositionInfo&>(msg.PositionInfo());
           const TPositionInfo& posInfo2 = static_cast<const TPositionInfo&>(msg2.PositionInfo());
           TPosition pos;
           TPosition pos2;
           posInfo.GetPosition(pos);
           posInfo2.GetPosition(pos2);
           TESTL(pos.Time() == pos2.Time());
           TESTL(pos.HorizontalAccuracy() == pos2.HorizontalAccuracy());
           TESTL(pos.VerticalAccuracy() == pos2.VerticalAccuracy());
           TESTL(pos.Latitude() == pos2.Latitude());
           TESTL(pos.Longitude() == pos2.Longitude());
           TESTL(pos.Altitude() == pos2.Altitude());
           TESTL(pos.Datum() == pos2.Datum());
           break;
           }
      default:
			{
			break;
			}
		}
	}

