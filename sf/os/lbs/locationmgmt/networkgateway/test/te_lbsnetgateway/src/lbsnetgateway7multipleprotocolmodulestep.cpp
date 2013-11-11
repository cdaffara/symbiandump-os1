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
// Example CTestStep derived implementation
// 
//

/**
 @file lbsnetgateway7MultiplePMStep.cpp
*/
#include "lbsnetgateway7multipleprotocolmodulestep.h"
#include "Te_lbsnetgatewaySuiteDefs.h"
#include "lbsnetinternalapi.h"
#include <simtsy.h>
#include <etelmm.h>

/*
 * Values used in test messages.
 */
_LIT8(KRequesterId, "08450000000");
_LIT8(KClientName, "abcdefghijkl.com");
_LIT8(KClientExternalId, "test_externalclientid");


/**
 * Destructor
 */
Clbsnetgateway7MultiplePMStep::~Clbsnetgateway7MultiplePMStep()
	{
	}

/**
 * Constructor
 */
Clbsnetgateway7MultiplePMStep::Clbsnetgateway7MultiplePMStep()
	{
	// MUST call SetTestStepName in the constructor as the controlling
	// framework uses the test step name immediately following construction to set
	// up the step's unique logging ID.
	SetTestStepName(Klbsnetgateway7MultiplePMStep);
	}

/**
 * @return - TVerdict code
 * Override of base class virtual
 */
TVerdict Clbsnetgateway7MultiplePMStep::doTestStepPreambleL()
	{
	INFO_PRINTF1(_L("Clbsnetgateway7MultiplePMStep::doTestStepPreambleL() >>"));
	// uncomment the following 3 lines if you have common pre setting to all the test steps in there
	CTe_lbsnetgatewaySuiteStepBase::doTestStepPreambleL();
	if (TestStepResult()!=EPass)
	   return   TestStepResult();
	// process some pre setting to this test step then set SetTestStepResult to EFail or Epass.
	
	// Connect to NRH, stub PM interfaces
	iNetGatewayChannel = CNetGatewayChannel::NewL(*this, RLbsNetChannel::EChannelNRH2NG);

    InitialiseUidArray();
    InitialiseStubPMChannels();
    
   	SetTestStepResult(EPass);
	INFO_PRINTF1(_L("Clbsnetgateway7MultiplePMStep::doTestStepPreambleL() <<"));
	return TestStepResult();
	}

/**
 * @return - TVerdict code
 * Override of base class pure virtual
 * Our implementation only gets called if the base class doTestStepPreambleL() did
 * not leave. That being the case, the current test result value will be EPass.
 */
TVerdict Clbsnetgateway7MultiplePMStep::doTestStepL()
{
	INFO_PRINTF1(_L("*** Clbsnetgateway7MultiplePMStep::doTestStepL() >> "));
	  if (TestStepResult()==EPass)
		{
		
		User::LeaveIfError(RProperty::Get(KUidPSSimTsyCategory, 
									  KPSSimTsyRegStatChange,
									  iCurrentNetworkStatus));
		
		switch(iCurrentNetworkStatus)
			{
			case RMobilePhone::ERegisteredOnHomeNetwork:
				INFO_PRINTF1(_L("*** Sending over Home PMs"));
				SendMessagesOverMultiplePms(iUidHomeArray);
				INFO_PRINTF1(_L("*** Sent..."));
				break;
			
			case RMobilePhone::ERegisteredRoaming:
				INFO_PRINTF1(_L("*** Sending over Roaming PMs"));
				SendMessagesOverMultiplePms(iUidRoamingArray);
				INFO_PRINTF1(_L("*** Sent..."));
				break;
			
			default:
				INFO_PRINTF2(_L("*** ERROR: Phone in unexpected registration mode %d"),iCurrentNetworkStatus);
				SetTestStepResult(EFail);
			}
				

		
		
		}
	 return TestStepResult();
	}


/**
 * @return - TVerdict code
 * Override of base class virtual
 */
TVerdict Clbsnetgateway7MultiplePMStep::doTestStepPostambleL()
	{
	//INFO_PRINTF1(_L("Please delete this line or modify it. I am in Test Step Postamble in Class Clbsnetgateway7MultiplePMStep"));
	// process something post setting to the test step

	// Close NRH, A-GPS, stub PM interfaces
	CloseStubPMChannels();
	delete iNetGatewayChannel;
		
	// uncomment the following line if you have common post setting to all the test steps in there
	CTe_lbsnetgatewaySuiteStepBase::doTestStepPostambleL();
	// uncomment the following line if you have post process or remove the following line if no post process
	// SetTestStepResult(EPass);		// or EFail
	return TestStepResult();
	}

void Clbsnetgateway7MultiplePMStep::ProcessNetChannelMessage(RLbsNetChannel::TLbsNetChannelId aChannelId, const TLbsNetInternalMsgBase& aMessage)
	{
	TEST(aChannelId == RLbsNetChannel::EChannelNRH2NG);
	TRAP_IGNORE(TestMessagesAreEquivalentL(aMessage, iNetProtocolMessage));
	CActiveScheduler::Stop();
	}
	
void Clbsnetgateway7MultiplePMStep::ProcessNetProtocolMessage(const TNetGatewayMsg& aMessage)
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


/*
 * Looping version of the test harnesses send functionality for sending basic messages
 */
void Clbsnetgateway7MultiplePMStep::SendMessagesOverMultiplePms(TLbsAdminProtocolModulesInfo::TLbsProtocolModuleIdArray aUidArray)
	{
	//
	// Test the sending and receiving of each type of NRH message.
	// All messages in both directions (NRH->NG->PM) and
	// (PM->NG->NRH) are tested and we organise them in type of session.
	// Otherwise the gateway doesn't know where to send them.
	//
	
	//
	// Section 1 : MTLR  operations
	//
	//
	// 1) ProcessPrivacyRequest()
	//
	
	// Reset loop counter/PM UID index depending on Home/Roaming network status

	ResetLoopCounter();
	
	INFO_PRINTF1(_L("* -------------------------------------------"));
	SNetMsgProcessPrivacyRequest* privReq = reinterpret_cast<SNetMsgProcessPrivacyRequest*>(iMsg.Data());
	new (privReq) SNetMsgProcessPrivacyRequest;
	iMsg.iType = ENetMsgProcessPrivacyRequest;
	privReq->iEmergency = EFalse;
	iPrivacy.SetRequestAdvice(TLbsNetPosRequestPrivacy::ERequestAdviceVerify);
	iPrivacy.SetRequestAction(TLbsNetPosRequestPrivacy::ERequestActionAllow);
	privReq->iPrivacy = iPrivacy;
	privReq->iRequestInfo.SetRequesterId(KRequesterId);
	privReq->iRequestInfo.SetClientName(KClientName);
	privReq->iRequestInfo.SetClientExternalId(KClientExternalId);
	
	
	for( ; iLoopCounter<iMaxLoopCounter ; iLoopCounter++ )
		{
		// Same message, different protocol entry point....
		privReq->iSessionId = TLbsNetSessionId(aUidArray[iPmUidIndex], 20001010);
		iNetProtocolMessage = iMsg; // Allows test message comparison
		INFO_PRINTF4(_L("* Sending PrivReq message %d of 10 to PM with UID %X, on RChannel %d"),iLoopCounter+1,aUidArray[iPmUidIndex],iLoopCounter);
		iStubPMChannelArray[iLoopCounter].SendNetProtocolMessageL(iMsg);
		iPmUidIndex++;
		CActiveScheduler::Start();
		}		
	
	
	ResetLoopCounter();
	
	
	//
	// 2) ProcessLocationRequest
	//

	INFO_PRINTF1(_L("* -------------------------------------------"));
	SNetMsgProcessLocationRequest* locReq = reinterpret_cast<SNetMsgProcessLocationRequest*>(iMsg.Data());
	iMsg.iType = ENetMsgProcessLocationRequest;
	locReq->iEmergency = EFalse;
	locReq->iService = MLbsNetworkProtocolObserver::EServiceMobileTerminated;
	locReq->iQuality.SetMaxFixTime(TTimeIntervalMicroSeconds(30000000));
	locReq->iQuality.SetMinHorizontalAccuracy(TReal32(250.0));
	locReq->iQuality.SetMinVerticalAccuracy(TReal32(250.0));
	TLbsNetPosMethod methodArray[2];
	methodArray[0].SetPosMethod(TUid::Uid(0x1028224E), (TPositionModuleInfo::ETechnologyTerminal 
														| TPositionModuleInfo::ETechnologyAssisted));
	methodArray[1].SetPosMethod(TUid::Uid(0x1028224E), (TPositionModuleInfo::ETechnologyNetwork 
														| TPositionModuleInfo::ETechnologyAssisted));
	locReq->iMethod.SetPosMethods(methodArray, (sizeof(methodArray) / sizeof(TLbsNetPosMethod)));
	for( ; iLoopCounter<iMaxLoopCounter ; iLoopCounter++ )
		{
		// Same message, different protocol entry point....
		locReq->iSessionId = TLbsNetSessionId(aUidArray[iPmUidIndex], 20002020);
		iNetProtocolMessage = iMsg; // Allows test message comparison
		INFO_PRINTF4(_L("* Sending ProcessLocationRequest message %d of 10 on PM with UID %X on RChannel %d"),iLoopCounter+1,aUidArray[iPmUidIndex],iLoopCounter);
		iStubPMChannelArray[iLoopCounter].SendNetProtocolMessageL(iMsg);
		iPmUidIndex++;
		CActiveScheduler::Start();
		}	
	INFO_PRINTF1(_L("* -------------------------------------------"));	
	}

/*
 * Util method for calculating upper/lower bound loop values for testing.
 */
void Clbsnetgateway7MultiplePMStep::ResetLoopCounter()
	{
	iPmUidIndex=0;
	if(iCurrentNetworkStatus == RMobilePhone::ERegisteredOnHomeNetwork)
		{
		iLoopCounter=KTe_LbsNetGatewayTestProtocolIndex;
		iMaxLoopCounter=KLbsAdminMaxProtocolModuleIds+KTe_LbsNetGatewayTestProtocolIndex;
		}
	else
		{
		iLoopCounter=KTe_LbsNetGatewayTestProtocolIndex11;
		iMaxLoopCounter=KLbsAdminMaxProtocolModuleIds+KTe_LbsNetGatewayTestProtocolIndex11;
		}
	}
