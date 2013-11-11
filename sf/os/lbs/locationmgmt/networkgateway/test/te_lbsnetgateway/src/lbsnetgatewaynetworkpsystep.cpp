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
// Tests the CPsyRequestChannel of the Network Gateway
// 
//

/**
 @file LbsNetGatewayNetworkPsyStep.cpp
*/
#include "lbsnetgatewaynetworkpsystep.h"
#include "Te_lbsnetgatewaySuiteDefs.h"
#include "lbsnetinternalapi.h"

const TLbsCellLocationCancelMsg KTransmitLocCancel(TLbsNetSessionIdInt(KLbsSuplPsyUid, 0x00000200), 
                                                           KErrCancel);

const TLbsNetSessionId KPsySessionId = TLbsNetSessionId(KLbsSuplPsyUid, 0x00000100);

CLbsNetGatewayNetworkPsyStep::~CLbsNetGatewayNetworkPsyStep()
/**
 * Destructor
 */
    {
    }

/**
 * Constructor
 */
CLbsNetGatewayNetworkPsyStep::CLbsNetGatewayNetworkPsyStep()
    {
    SetTestStepName(KLbsNetgatewayNetworkPsyStep);
    }

/**
 * @return - TVerdict code
 * Override of base class virtual
 */
TVerdict CLbsNetGatewayNetworkPsyStep::doTestStepPreambleL()
    {
     CTe_lbsnetgatewaySuiteStepBase::doTestStepPreambleL();
    if (TestStepResult()!=EPass)
       return   TestStepResult();
    
    // Set the reference position that will be sent/read for this test
    iReferencePosition.SetAccuracy(TReal32(50.0), TReal32(50.0));
    iReferencePosition.SetCoordinate(TReal64(11145.6), TReal64(4567.3), TReal32(99.0));
    iReferencePosition.SetCurrentTime();

    // Create the SUPL PSY to NG channel
    iPsyGatewayChannel = CPsyToNgChannel::NewL(*this, RLbsNetChannel::EChannelSUPLPSY2NG);
    
    // Create the stub PM interfaces
    iStubPMChannelArray = new RNetGatewayProtocolTestChannel;
    iStubPMChannelArray->OpenL(KNetProtocolChannelKey, KTestAppChannelKey, *this, KTe_LbsNetGatewayTestProtocolIndex);
    
    SetTestStepResult(EPass);
    return TestStepResult();
    }


/**
 * @return - TVerdict code
 * Override of base class pure virtual
 * 
 * Sends a location request and expects a reference position to be returned.
 * Session is ended with a session complete from the network.
 * 
 * Then tests that canceling a location request from the PSY is processed 
 * properly in the NG. Session is ended with a session complete from the
 * network.
 */
TVerdict CLbsNetGatewayNetworkPsyStep::doTestStepL()
    {
    INFO_PRINTF1(_L("*** CLbsNetGatewayNetworkPsyStep::doTestStepL() >> </b>"));
      if (TestStepResult()==EPass)
        {
        TLbsNetPosRequestOptionsInt options;
        options.SetNewClientConnected(ETrue);
        
        // 1) Send a location request using the SUPL PSY channel and test it comes
        //    out looking the same at the PM i.e. via the NG
        //
        TLbsCellLocationRequestMsg* netTransmitReq = new (ELeave) TLbsCellLocationRequestMsg(
                TLbsNetSessionIdInt(KLbsSuplPsyUid, 0x00000100),
                KTe_LbsNetGatewayTestProtocolUid,
                options);

        iNetInternalMessage = netTransmitReq;
        INFO_PRINTF1(_L("*** iPsyGatewayChannel->SendMessageAndWaitForResponse(LocationRequest) </b>"));
        iPsyGatewayChannel->SendMessageAndWaitForResponse(*netTransmitReq);
        delete netTransmitReq;

        // 2) Reply with reference position
        //
        SNetMsgProcessLocationUpdate netBasedLoc;
        netBasedLoc.iSessionId = KPsySessionId;
        
        netBasedLoc.iReferenceLocation.SetPosition(iReferencePosition);
        iNetProtocolMessage.iType = ENetMsgProcessLocationUpdate;
        Mem::Copy(iNetProtocolMessage.Data(), &netBasedLoc, sizeof(SNetMsgProcessLocationUpdate));
        INFO_PRINTF1(_L("*** iStubPMChannel.SendNetProtocolMessageL(RefPos) </b>"));
        iStubPMChannelArray->SendNetProtocolMessageL(iNetProtocolMessage);
        INFO_PRINTF1(_L("*** CActiveScheduler::Start() </b>"));
        CActiveScheduler::Start();
        
        // 3) ProcessSessionComplete
        //
        SNetMsgProcessSessionComplete* sessComp = reinterpret_cast<SNetMsgProcessSessionComplete*>(iMsg.Data());
        iMsg.iType = ENetMsgProcessSessionComplete;
        sessComp->iSessionId = KPsySessionId;
        sessComp->iReason = KErrNone;
        iNetProtocolMessage = iMsg;
        INFO_PRINTF1(_L("*** iStubPMChannel.SendNetProtocolMessageL(SessionComplete) </b>"));
        iStubPMChannelArray->SendNetProtocolMessageL(iMsg);
        INFO_PRINTF1(_L("*** CActiveScheduler::Start() </b>"));
        CActiveScheduler::Start();


        // 4) Send a location request using the SUPL PSY channel and test it comes
        //    out looking the same at the PM i.e. via the NG
        //
        TLbsCellLocationRequestMsg* netTransmitReq2 = new (ELeave) TLbsCellLocationRequestMsg(
                TLbsNetSessionIdInt(KLbsSuplPsyUid, 0x00000200),
                KTe_LbsNetGatewayTestProtocolUid,
                options);

        iNetInternalMessage = netTransmitReq2;
        INFO_PRINTF1(_L("*** iPsyGatewayChannel->SendMessageAndWaitForResponse(LocationRequest) </b>"));
        iPsyGatewayChannel->SendMessageAndWaitForResponse(*netTransmitReq2);
        delete netTransmitReq2;
        
        
        // 5) Cancel the location request
        //
        iNetInternalMessage = &KTransmitLocCancel;
        INFO_PRINTF1(_L("*** iPsyGatewayChannel->SendMessageAndWaitForResponse(KTransmitLocCancel) </b>"));
        iPsyGatewayChannel->SendMessageAndWaitForResponse(KTransmitLocCancel);
        

        // 6) ProcessSessionComplete
        //
        sessComp->iSessionId = TLbsNetSessionId(KLbsSuplPsyUid, 0x00000200);
        sessComp->iReason = KErrNone;
        iNetProtocolMessage = iMsg;
        INFO_PRINTF1(_L("*** iStubPMChannel.SendNetProtocolMessageL(SessionComplete) </b>"));
        iStubPMChannelArray->SendNetProtocolMessageL(iMsg);
        INFO_PRINTF1(_L("*** CActiveScheduler::Start() </b>"));
        CActiveScheduler::Start();
        }

      INFO_PRINTF1(_L("*** CLbsNetGatewayNetworkPsyStep::doTestStepL() << "));
      return TestStepResult();
    }



/**
 * @return - TVerdict code
 * Override of base class virtual
 */
TVerdict CLbsNetGatewayNetworkPsyStep::doTestStepPostambleL()
    {
    // Close PSY and stub PM interfaces
    iStubPMChannelArray->Close();
    delete iPsyGatewayChannel;
        
    CTe_lbsnetgatewaySuiteStepBase::doTestStepPostambleL();

    return TestStepResult();
    }

void CLbsNetGatewayNetworkPsyStep::ProcessNetChannelMessage(RLbsNetChannel::TLbsNetChannelId aChannelId, const TLbsNetInternalMsgBase& aMessage)
    {
    TEST(aChannelId == RLbsNetChannel::EChannelSUPLPSY2NG);
    TRAP_IGNORE(TestMessagesAreEquivalentL(aMessage, iNetProtocolMessage));
    CActiveScheduler::Stop();
    }
    
void CLbsNetGatewayNetworkPsyStep::ProcessNetProtocolMessage(const TNetGatewayMsg& aMessage)
    {
    switch (aMessage.iType)
        {
        case ENetMsgGetCurrentCapabilitiesResponse:
            {
            TEST(iNetProtocolMessage.iType == ENetMsgGetCurrentCapabilitiesRequest);
            CActiveScheduler::Stop();
            break;
            }
        case ENetMsgProcessPrivacyRequest:
        case ENetMsgProcessLocationRequest:
        case ENetMsgProcessSessionComplete:
        case ENetMsgProcessAssistanceData:
        case ENetMsgProcessLocationUpdate:
        case ENetMsgGetCurrentCapabilitiesRequest:
        case ENetMsgProcessStatusUpdate:
        case ENetMsgRespondPrivacyRequest:
        case ENetMsgRespondLocationRequest:
        case ENetMsgRequestTransmitLocation:
        case ENetMsgCancelTransmitLocation:
        case ENetMsgRequestAssistanceData:
        case ENetMsgRequestSelfLocation:
        case ENetMsgCancelSelfLocation:
        case ENetMsgRequestNetworkLocation:
        case ENetMsgCancelNetworkLocation:
        case ENetMsgCancelExternalLocation:
        default:
            {
            // Check that the incoming message carries the same information as
            // the message that was sent
            TRAP_IGNORE(TestMessagesAreEquivalentL(*iNetInternalMessage, aMessage));            
            }
        }
    }
