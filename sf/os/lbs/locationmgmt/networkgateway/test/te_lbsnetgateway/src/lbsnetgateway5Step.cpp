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
 @file lbsnetgateway5Step.cpp
*/

#include <e32debug.h>

#include "lbsnetgateway5Step.h"
#include "Te_lbsnetgatewaySuiteDefs.h"


const TLbsNetSessionId KNetworkLocationSessionId = TLbsNetSessionId(KLbsNetLocManagerUid, 0x10000001);
const TLbsNetSessionIdInt KNetworkLocationSessionIdInt = TLbsNetSessionIdInt(KLbsNetLocManagerUid, 0x10000001);


Clbsnetgateway5Step::~Clbsnetgateway5Step()
/**
 * Destructor
 */
	{
	}

Clbsnetgateway5Step::Clbsnetgateway5Step() :
/**
 * Constructor
 */
 	iNetworkRequestMsg(KNetworkLocationSessionIdInt,
					   KErrNone,
					   TPositionInfo(),
					   TTime())
	{
	// **MUST** call SetTestStepName in the constructor as the controlling
	// framework uses the test step name immediately following construction to set
	// up the step's unique logging ID.
	SetTestStepName(Klbsnetgateway5Step);
	}

TVerdict Clbsnetgateway5Step::doTestStepPreambleL()
/**
 * @return - TVerdict code
 * Override of base class virtual
 */
	{
	// uncomment the following 3 lines if you have common pre setting to all the test steps in there
	CTe_lbsnetgatewaySuiteStepBase::doTestStepPreambleL();
	if (TestStepResult()!=EPass)
	   return   TestStepResult();
	// process some pre setting to this test step then set SetTestStepResult to EFail or Epass.
	
	// Connect to NLM, stub PM interfaces
	iNetGatewayChannel = CNetGatewayChannel::NewL(*this, RLbsNetChannel::EChannelNRH2NG);
	iNetworkLocationChannel = CNetworkLocationChannel::NewL(*this);

    InitialiseUidArray();
    iStubPMChannelArray = new RNetGatewayProtocolTestChannel;	
    iStubPMChannelArray->OpenL(KNetProtocolChannelKey, KTestAppChannelKey, *this, KTe_LbsNetGatewayTestProtocolIndex);
	iReferencePositionUpdates.OpenL(RLbsNetworkPositionUpdates::ENetworkPositionReference);
	
	SetTestStepResult(EPass);
	return TestStepResult();
	}


TVerdict Clbsnetgateway5Step::doTestStepL()
/**
 * @return - TVerdict code
 * Override of base class pure virtual
 * Our implementation only gets called if the base class doTestStepPreambleL() did
 * not leave. That being the case, the current test result value will be EPass.
 */
	{
	  if (TestStepResult()==EPass)
		{
		//
		// This test simulates the sequence of messages that
		// go through the Network Gateway during a 'normal'
		// Network (Based) Location request. That is, a 
		// request from the Network Location Manager for 
		// a network-based (e.g. cell-based) location.
		//
		// This test plays the role of the Network Location 
		// Manager (NLM), Network Request Handler (NRH) and Network
		// Protocol Module (PM), sending and receiving messages
		// from the Network Gateway (NG).
		//
		// The expected sequence of messages is:
		//
		// 1) NLM sends a network location request to NG; 
		//		NG calls RequestNetworkBasedLocation() on PM
		// 2) PM replies by calling ProcessNetworkBasedLocation(),
		//		ProcessAssistanceData() and ProcessLocationRequest().
		//		NG processes each of these by: publishing the network-based
		//		location on RLbsReferencePositionUpdates, 
		//		sending the assistance data to the A-GPS manager (ignored for this test),
		//		sending the location request to the NRH.
		// 3) NRH responds to the location request with a
		//		location response, which the NG sends to the PM.
		// 4) PM receives the location response and then finishes the
		//		session by calling ProcessSessionComplete(). 
		//		The NG sends a 'session complete' message to both the
		//		NLM and the NRH. (This completes the location request in the NRH
		//		and the original network location request in the NLM.)
		// 5) NLM reads the published network-based reference location
		//		from the internal api (RLbsReferencePositionUpdates)
		//

		StartTest();

		}
	  return TestStepResult();
	}



TVerdict Clbsnetgateway5Step::doTestStepPostambleL()
/**
 * @return - TVerdict code
 * Override of base class virtual
 */
	{
	// process something post setting to the test step

	iReferencePositionUpdates.Close();	
	iStubPMChannelArray->Close();
	delete iNetworkLocationChannel;
	delete iNetGatewayChannel;
	
	// uncomment the following line if you have common post setting to all the test steps in there
	CTe_lbsnetgatewaySuiteStepBase::doTestStepPostambleL();
	// uncomment the following line if you have post process or remove the following line if no post process
	// SetTestStepResult(EPass);		// or EFail
	return TestStepResult();
	}

void Clbsnetgateway5Step::StartTest()
	{
	// Set the reference position that will be sent/read for this test
	iReferencePosition.SetAccuracy(TReal32(50.0), TReal32(50.0));
	iReferencePosition.SetCoordinate(TReal64(11145.6), TReal64(4567.3), TReal32(99.0));
	iReferencePosition.SetCurrentTime();
	
	iReceivedNetRequestSessionComplete = EFalse;
	iReceivedNetLocationSessionComplete = EFalse;
	iState = ETestStateStart;
	TRAPD(err, GoToNextStateL());
	if(err!=KErrNone)
		{
		User::Invariant();
		}
	CActiveScheduler::Start();	
	}

void Clbsnetgateway5Step::GoToNextStateL()
	{
	INFO_PRINTF2(_L("Current state: %d, GoToNextStateL()>>"), iState);
	switch (iState)
		{
		case ETestStateStart:
			{
			// Send the Network Locaiton Request to the NG
			TLbsNetPosRequestQualityInt quality;
			iNetworkLocationMsg = new (ELeave) TLbsNetLocNetworkLocationRequestMsg(
												KNetworkLocationSessionIdInt,
												ETrue, 
												quality);
			User::LeaveIfError(iNetworkLocationChannel->SendNetworkLocationMessage(*iNetworkLocationMsg));			
			
			iState = ETestStateWaitForNetworkLocationRequest;
			break;
			}
		case ETestStateWaitForNetworkLocationRequest:
			{
			// Simulate the protocol module receiving a 
			// Measurement Control from the network;
			// need to send a reference position, 
			// assistance data, location request
			
			// Reference Position
			SNetMsgProcessLocationUpdate netBasedLoc;
			netBasedLoc.iSessionId = KNetworkLocationSessionId;
			
			netBasedLoc.iReferenceLocation.SetPosition(iReferencePosition);
			iTestPmMsg.iType = ENetMsgProcessLocationUpdate;
			Mem::Copy(iTestPmMsg.Data(), &netBasedLoc, sizeof(SNetMsgProcessLocationUpdate));
			iStubPMChannelArray->SendNetProtocolMessageL(iTestPmMsg);
			
			iState = ETestStateWaitForRefPosition;
			break;
			}
			
		case ETestStateWaitForRefPosition:
			{
			// Assistance Data
			// TODO: not integral to this test, do it later.
			
			// Location Request			
			TLbsNetPosRequestQuality quality;
			TLbsNetPosRequestMethod methods;
			SNetMsgProcessLocationRequest locReq;
			locReq.iSessionId = KNetworkLocationSessionId;
			locReq.iEmergency = EFalse;
			locReq.iService = MLbsNetworkProtocolObserver::EServiceNetworkLocation;
			locReq.iQuality = quality;
			locReq.iMethod = methods;
			iTestPmMsg.iType = ENetMsgProcessLocationRequest;
			Mem::Copy(iTestPmMsg.Data(), &locReq, sizeof(SNetMsgProcessLocationRequest));
			iStubPMChannelArray->SendNetProtocolMessageL(iTestPmMsg);
			
			iState = ETestStateWaitForLocationRequest;
			break;
			}
		case ETestStateWaitForLocationRequest:
			{
			TTime actualTime;
			actualTime.UniversalTime();

			// Send the response to the location request using the stored reference position
			TLbsNetLocationResponseMsg msg(KNetworkLocationSessionIdInt,
										   KErrNone,
										   iReceivedReferencePosition,
										   actualTime);
			iNetworkRequestMsg = msg;
			
			// Need to change state before sending the message,
			// in case the observer function (ProcessNetProtocolMessage) 
			// get called before it returns.
			iState = ETestStateWaitForLocationResponse;
			
			iNetGatewayChannel->SendMessageAndWaitForResponse(iNetworkRequestMsg);
			break;
			}
		case ETestStateWaitForLocationResponse:
			{
			// Got the location response; now send a complete 
			// message to finish the session
			SNetMsgProcessSessionComplete sessComp;
			sessComp.iSessionId = KNetworkLocationSessionId;
			sessComp.iReason = KErrNone;
			iTestPmMsg.iType = ENetMsgProcessSessionComplete;
			Mem::Copy(iTestPmMsg.Data(), &sessComp, sizeof(SNetMsgProcessSessionComplete));
			
			//The state is set beforehand here to avoid errors related with race conditions. 
			//As a result of race conditions we may receive ProcessNetworkLocationResponseMessage call
			//while calling the SendNetProtocolMessageL method. This is a quick fix only.
			iState = ETestStateWaitForSessionComplete;
			iStubPMChannelArray->SendNetProtocolMessageL(iTestPmMsg);
						
			break;
			}
		case ETestStateWaitForSessionComplete:
			{
			// Got both session complete messages; now get the
			// reference position and check that it is correct
			TPositionInfo posInfo;
			TTime startTime;
			TTime actualTime;
			TLbsNetSessionIdInt sessionId;
			TInt positionReceived;
	        positionReceived = iReferencePositionUpdates.GetPositionInfo(sessionId, posInfo, startTime, actualTime);
			if(positionReceived != KErrNone)
			    {
			    INFO_PRINTF2(_L("ERROR: call to iReferencePositionUpdates.GetPositionInfo() failed with error code %d"), positionReceived);
			    User::Leave(KErrArgument);  
			    }
			TPosition pos;
			posInfo.GetPosition(pos);
			TestPositionsAreEqualL(iReferencePosition, pos);
			
			CActiveScheduler::Stop();
			
			iState = ETestStateFinish;
			break;
			}
		case ETestStateUnknown:
		case ETestStateFinish:
		default:
			{
			User::Leave(KErrArgument);	
			}
		}
	INFO_PRINTF2(_L("Current state: %d, GoToNextStateL()<<"), iState);
	}

void Clbsnetgateway5Step::ProcessNetProtocolMessage(const TNetGatewayMsg& aMessage)
	{
	TInt err = KErrNone;
	INFO_PRINTF2(_L("Current state: %d, <i>ProcessNetProtocolMessage()</i>>>"), iState);
	switch (iState)
		{
		case ETestStateWaitForNetworkLocationRequest:
			{
			TRAP(err, TestMessagesAreEquivalentL(*iNetworkLocationMsg, aMessage));
			delete iNetworkLocationMsg;
			TRAP(err, GoToNextStateL());
			break;
			}
		case ETestStateWaitForLocationResponse:
			{
			TRAP(err, TestMessagesAreEquivalentL(iNetworkRequestMsg, aMessage));
			TRAP(err, GoToNextStateL());
			break;
			}
		case ETestStateStart:
		case ETestStateWaitForLocationRequest:
		case ETestStateWaitForSessionComplete:
		case ETestStateFinish:
		case ETestStateUnknown:
		default:
			{
			TEST(EFail);
			err = KErrArgument;	
			}
		}
	
	if (err != KErrNone)
		{
		INFO_PRINTF2(_L("TEST FAILED! Current State : %d"), iState);
		SetTestStepResult(EFail);
		CActiveScheduler::Stop();
		iState = ETestStateFinish;
		}
	INFO_PRINTF2(_L("Current state: %d, <i>ProcessNetProtocolMessage()</i><<"), iState);
	}
	
void Clbsnetgateway5Step::ProcessNetworkLocationResponseMessage(const TLbsNetLocMsgBase& aMessage)
	{
	INFO_PRINTF2(_L("Current state: %d, ProcessNetworkLocationResponseMessage()>>"), iState);
	TInt err = KErrNone;

	switch (iState)
		{
		case ETestStateWaitForSessionComplete:
			{
			TRAP(err, TestMessagesAreEquivalentL(aMessage, iTestPmMsg));
			if (err == KErrNone)
				{
				iReceivedNetLocationSessionComplete = ETrue;
				}
			
			if (iReceivedNetRequestSessionComplete
				&& iReceivedNetLocationSessionComplete)
				{
				TRAP(err, GoToNextStateL());
				}
			break;
			}
		case ETestStateStart:
		case ETestStateWaitForNetworkLocationRequest:
		case ETestStateWaitForLocationRequest:
		case ETestStateWaitForLocationResponse:
		case ETestStateFinish:		
		case ETestStateUnknown:
		default:
			{
			TEST(FALSE);
			err = KErrArgument;
			}
		}

	if (err != KErrNone)
		{
		INFO_PRINTF2(_L("TEST FAILED! Current State : %d"), iState);
		SetTestStepResult(EFail);
		CActiveScheduler::Stop();
		iState = ETestStateFinish;
		}
	INFO_PRINTF2(_L("Current state: %d, ProcessNetworkLocationResponseMessage()<<"), iState);
	}

void Clbsnetgateway5Step::ProcessNetChannelMessage(RLbsNetChannel::TLbsNetChannelId aChannelId, const TLbsNetInternalMsgBase& aMessage)
	{
	INFO_PRINTF2(_L("Current state: %d, <b>ProcessNetChannelMessage()</b>>>"), iState);
	TEST(aChannelId == RLbsNetChannel::EChannelNRH2NG);
	TInt err = KErrNone;

	switch (iState)
		{
		case ETestStateWaitForLocationRequest:
			{
			TRAP(err, TestMessagesAreEquivalentL(aMessage, iTestPmMsg));
			TRAP(err, GoToNextStateL());
			break;
			}
		case ETestStateWaitForSessionComplete:
			{
			TRAP(err, TestMessagesAreEquivalentL(aMessage, iTestPmMsg));
			if (err == KErrNone)
				{
				iReceivedNetRequestSessionComplete = ETrue;
				}
			
			if (iReceivedNetRequestSessionComplete
				&& iReceivedNetLocationSessionComplete)
				{
				TRAP(err, GoToNextStateL());
				}
			break;
			}
		case ETestStateWaitForRefPosition:
			{
			const TLbsNetLocationUpdateMsg* msg = static_cast<const TLbsNetLocationUpdateMsg*>(&aMessage);

			// Store the reference position
			iReceivedReferencePosition.SetPosition(iReferencePosition);

			// Check the reference position is as expected
			const TPositionInfo* posInfo = static_cast<const TPositionInfo*>(&msg->PositionInfo());
			TPosition myTPos;
			posInfo->GetPosition(myTPos);
			TestPositionsAreEqualL(iReferencePosition, myTPos);

			TRAP(err, GoToNextStateL());
			break;
			}
		
		case ETestStateStart:
		case ETestStateWaitForNetworkLocationRequest:
		case ETestStateWaitForLocationResponse:
		case ETestStateFinish:
		case ETestStateUnknown:
		default:
			{
			TEST(FALSE);
			err = KErrArgument;
			}
		}

	if (err != KErrNone)
		{
		INFO_PRINTF2(_L("TEST FAILED! Current State : %d"), iState);
		SetTestStepResult(EFail);
		CActiveScheduler::Stop();
		iState = ETestStateFinish;
		}
	INFO_PRINTF2(_L("Current state: %d, <b>ProcessNetChannelMessage()</b><<"), iState);
	}

void Clbsnetgateway5Step::TestPositionsAreEqualL(const TPosition& pos1, const TPosition& pos2)
	{
	INFO_PRINTF1(_L("                  TestPositionsAreEqualL>>"));
	TESTL(pos1.Time() == pos2.Time());
	TESTL(pos1.HorizontalAccuracy() == pos2.HorizontalAccuracy());
	TESTL(pos1.VerticalAccuracy() == pos2.VerticalAccuracy());
	TESTL(pos1.Latitude() == pos2.Latitude());
	TESTL(pos1.Longitude() == pos2.Longitude());
	TESTL(pos1.Altitude() == pos2.Altitude());
	TESTL(pos1.Datum() == pos2.Datum());	
	INFO_PRINTF1(_L("                  TestPositionsAreEqualL<<"));
	}

