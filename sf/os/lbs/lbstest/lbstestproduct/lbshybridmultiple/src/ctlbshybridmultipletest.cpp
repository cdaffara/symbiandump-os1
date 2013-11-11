// Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// Runs the actual multiple test step
//

// LBS includes. 
#include <lbs/lbsnetprotocolbase.h>
#include <lbs/lbsassistancedatabuilderset.h>

// LBS test includes.
#include "ctlbshybridmultipletest.h"
#include <lbs/test/tlbsutils.h>
#include "argutils.h"
#include <lbs/test/activeyield.h>


/**
Static Constructor
*/
CT_LbsHybridMultipleTest* CT_LbsHybridMultipleTest::New(CT_LbsHybridMultipleServer& aParent)
	{
	// Note that we do not use ELeave.
	// This means that having insufficient memory will return NULL;
	CT_LbsHybridMultipleTest* testStep = new CT_LbsHybridMultipleTest(aParent);
	if (testStep)
		{
		TInt err = KErrNone;

		TRAP(err, testStep->ConstructL());
		if (err)
			{
			delete testStep;
			testStep = NULL;
			}
		}
	return testStep;
	}


/**
 * Constructor
 */
CT_LbsHybridMultipleTest::CT_LbsHybridMultipleTest(CT_LbsHybridMultipleServer& aParent) : CT_LbsHybridMultipleStep(aParent),
iCheckForNewSequencesToStart(ETrue)
	{
	SetTestStepName(KLbsHybridMultipleTest);
	}


void CT_LbsHybridMultipleTest::ConstructL()
	{
	// Create the base class objects.
	CT_LbsHybridMultipleStep::ConstructL();
	iController = CLbsPrivacyController::NewL(*this);
	iCheckNewSequenceIdle = CIdle::NewL(CActive::EPriorityStandard);
	}

/**
 * Destructor
 */
CT_LbsHybridMultipleTest::~CT_LbsHybridMultipleTest()
	{
	if(iCheckNewSequenceIdle)
		{
		iCheckNewSequenceIdle->Cancel();
		delete iCheckNewSequenceIdle;
		}
	delete iController;
	}

/** Actual test step, which starts the test on the first sequence
 */
TVerdict CT_LbsHybridMultipleTest::doTestStepL()
	{
	INFO_PRINTF1(_L(""));
	INFO_PRINTF1(_L("---------------------------------- Starting Test ----------------------------------"));
	
	//Start off the master sequence; sequence 0
	iActiveSequences = 1;
	iSequences[0]->SignalCallbackIdleStart();
	//This will cause the sequence to start its session; sending and receiving messages to LBS.  The other
	// sequences should start as this sequence progresses.

	//Wait for the test to complete.  When all the sequences have finished, the active scheduler will
	// be stopped (from StopTest()) causing this test to complete.
	CActiveScheduler::Start();
	
	return TestStepResult();
	}

/** Stops the active scheduler causing the test to finish
 * 
 * @param aError If less than KErrNone, it will set the test step result to fail
 */
void CT_LbsHybridMultipleTest::StopTest(TInt aError)
	{
	//Check to see if we are ending the test in error
	if(aError < 0)
		{		
		//Fail the test step
		SetTestStepResult(EFail);
		}
	
	CActiveScheduler::Stop();
	}

/** Finds out which sequence relates to the session ID and checks that the received message was expected
 * 
 * @param aSessionId Session ID that was past into the message
 * @param aMessageType ID of the message that was received by the PM
 * 
 * @return The ID of the sequence that this message relates to, or KErrNotFound
 */
TInt CT_LbsHybridMultipleTest::WasMessageExpectedSessionId(TInt aSessionId, TNetProtocolResponseType aMessageType)
	{	
	//Loop through all the sequences
	for(TInt i=0; i<iSequences.Count(); ++i)
		{
		//Get the session id and the current sequence action
		TLbsNetSessionId sessionId = iSequences[i]->GetSessionId();
		TInt currentAction = iSequences[i]->GetCurrentAction();
		
		//Check the session ID of any sequence that is currently active with the session id past in
		if((sessionId.SessionNum() == aSessionId) && (currentAction != KErrArgument))
			{
			//Found the sequence with the requested session id, now check if the message type was expected
			if(currentAction == aMessageType)
				{
				//Message type was found, return sequence number
				return i;
				}
			else
				{
				//Message not expected by this sequence
				INFO_PRINTF3(_L("Error - Received unexpected message %d for session Id: %d"), aMessageType, aSessionId);
				INFO_PRINTF3(_L(" -> On Sequence: %d, at position: %d"), i , iSequences[i]->GetCurrentPosition());
				return KErrNotFound;
				}
			}
		}
	
	//Error, session ID not found in any sequence
	INFO_PRINTF3(_L("Error - Received unexpected session Id (%d) for message: %d"), aSessionId, aMessageType);
	
	return KErrNotFound;
	}


/** Finds out whether any of the sequences (using aPmId) expected the following message
 * 
 * @param aPmId ID of the PM that the message was received on (on sequences related to this id are checked)
 * @param aMessageType ID of the message that was received by the PM
 * 
 * @return The ID of the sequence that this message relates to, or KErrNotFound
 */
TInt CT_LbsHybridMultipleTest::WasMessageExpectedPmId(TInt aPmId, TNetProtocolResponseType aMessageType)
	{
	//Loop through all sequences
	for(TInt i=0; i<iSequences.Count(); ++i)
		{
		//Check to see whether any sequences the are using PM ID, aPmId, are expecting this message
		if((iSequences[i]->GetPmId() == aPmId) && (iSequences[i]->GetCurrentAction() == aMessageType))
			{
			//Return the sequenceId
			return i;
			}
		}
	
	//Error, message was not found in any of the sequences
	INFO_PRINTF2(_L("Was not expecting message %d in any sequence"), aMessageType);
	return KErrNotFound;
	}

/* Called when one of the stub PM's receives a message from the NG
 * 
 * @param aPmId The ID of the PM that received this message
 * @param aMessageType The ID of the message type received from the PM
 */
void CT_LbsHybridMultipleTest::OnHandleNGMessage(TUint aPmId, TInt aMessageType)
	{
	//Get the correct proxy
	CLbsTestNgMessageHandler* proxy = LookupProtocolModuleProxy(aPmId);
	if (!proxy)
	    {
        INFO_PRINTF2(_L("ERROR: No NG proxy setup for PM%d"), aPmId);
	    User::Leave(KErrNotFound);
	    }
	
	TInt cleanupCnt;
	TInt sequenceId = KErrNotFound;
	
	//For each of the different message types, find out whether the message type was expected by the
	// sequence (if a session Id is sent with the message) or by any sequence (if the session Id is not
	// present).  Then confirm the message parameters are expected and then move the correct sequence
	// onto the next message.
	switch(aMessageType)
		{	
		// >> AdviceSystemStatus() [2009]
		case ENetMsgGetCurrentCapabilitiesResponse:
			{
			sequenceId = WasMessageExpectedPmId(aPmId, ENetMsgGetCurrentCapabilitiesResponse);
			INFO_PRINTF2(_L("-> AdviceSystemStatus() [2009] Sq: %d"), sequenceId);
			
			if(sequenceId != KErrNotFound)
				{
				CLbsNetworkProtocolBase::TLbsSystemStatus status;
				cleanupCnt = proxy->ProtocolProxy()->GetArgsLC(ENetMsgGetCurrentCapabilitiesResponse, &status);
				TESTL(status == CLbsNetworkProtocolBase::ESystemStatusNone);
				CleanupStack::PopAndDestroy(cleanupCnt);
				}
			
			break;
			}
		
		// >> RespondPrivacyRequest() [2000]
		case ENetMsgRespondPrivacyRequest:
			{						
			TLbsNetSessionId* getSessionId = NULL;
			CLbsNetworkProtocolBase::TLbsPrivacyResponse getPrivacy;
			cleanupCnt = proxy->ProtocolProxy()->GetArgsLC(ENetMsgRespondPrivacyRequest, &getSessionId, &getPrivacy);
			
			sequenceId = WasMessageExpectedSessionId(getSessionId->SessionNum(), ENetMsgRespondPrivacyRequest);
			INFO_PRINTF3(_L("-> RespondPrivacyRequest(%d) [2000] Sq: %d"), getPrivacy, sequenceId);
			
			if(sequenceId != KErrNotFound)
				{
				iSequences[sequenceId]->CheckRespondPrivacyRequest(getPrivacy);
				}
			CleanupStack::PopAndDestroy(cleanupCnt);//getSessionId
				
			break;
			}
		
		// >> RequestAssistanceData() [2004]
		case ENetMsgRequestAssistanceData:
			{
			sequenceId = WasMessageExpectedPmId(aPmId, ENetMsgRequestAssistanceData);
			
			//Request Assistance Data is a special case.  Depending upon the timing of when the messages
			// will be sent/received, it is not possible to predict in advance whether this message will
			// always arrive in some of the later sequences.  As such, a RequestAssistanceData message can
			// arrive and even if it is not expected by any of the sequences, it will not fail the test.
			
			if(sequenceId != KErrNotFound)
				{
				INFO_PRINTF2(_L("-> RequestAssistanceData() [2004] Sq: %d"), sequenceId);
				TLbsAsistanceDataGroup dataGroup;
				cleanupCnt = proxy->ProtocolProxy()->GetArgsLC(ENetMsgRequestAssistanceData, &dataGroup);
				CleanupStack::PopAndDestroy(cleanupCnt);
				}
			else
				{
				//Message was not expected by any sequence.  Manually set the Net Proxy off listening
				// again, so the sequence can receive the message it expected.
				INFO_PRINTF2(_L("-> RequestAssistanceData() [2004] Sq: unknown, PM: %d"), aPmId);
				proxy->WaitForResponseL(80 * 1000 * 1000);
				
				//Break out of the function now
				return;
				}
			
			break;
			}
			
		// >> ResponsdLocationRequest() [2001]
		case ENetMsgRespondLocationRequest:
			{
			TLbsNetSessionId* getSessionId = NULL;
			getSessionId = NULL;
			TInt getReason = KErrNone;
			TPositionSatelliteInfo* getPositionInfo = NULL;
			cleanupCnt = proxy->ProtocolProxy()->GetArgsLC(ENetMsgRespondLocationRequest, &getSessionId, &getReason, &getPositionInfo);
			sequenceId = WasMessageExpectedSessionId(getSessionId->SessionNum(), ENetMsgRespondLocationRequest);
			INFO_PRINTF3(_L("-> RespondLocationRequest(%d) [2001] Sq: %d"), getReason, sequenceId);
			CleanupStack::PopAndDestroy(cleanupCnt);
			break;
			}
			
		// >> RequestSelfLocation() [2005]
		case ENetMsgRequestSelfLocation:
			{
			TLbsNetSessionId* sessionId = NULL;
			TLbsNetPosRequestOptionsAssistance*	opts = NULL;
			cleanupCnt = proxy->ProtocolProxy()->GetArgsLC(ENetMsgRequestSelfLocation, &sessionId, &opts);

			sequenceId = WasMessageExpectedPmId(aPmId, ENetMsgRequestSelfLocation);
			INFO_PRINTF2(_L("-> RequestSelfLocation() [2005] Sq: %d"), sequenceId);
			
			if(sequenceId != KErrNotFound)
				{
				iSequences[sequenceId]->CheckSelfLocationRequest(sessionId);
				}
			
			CleanupStack::PopAndDestroy(cleanupCnt);
			break;
			}
			
		// >> CancelSelfLocation() [2006]
		case ENetMsgCancelSelfLocation:
			{
			TLbsNetSessionId* sessionId = NULL;
			TInt reason = 0;
			cleanupCnt = proxy->ProtocolProxy()->GetArgsLC(ENetMsgCancelSelfLocation, &sessionId, &reason);
			
			sequenceId = WasMessageExpectedSessionId(sessionId->SessionNum(), ENetMsgRequestSelfLocation);
			INFO_PRINTF2(_L("-> CancelSelfLocation() [2006] Sq: %d"), sequenceId);

			CleanupStack::PopAndDestroy(cleanupCnt);
			
			break;
			}
			
		// >> RequestTransmitLocation() [2002]
		case ENetMsgRequestTransmitLocation:
			{
			HBufC16* getThirdParty = NULL;
			TLbsNetSessionId* getSessionId = NULL;
			TInt getPriority(0);
			TInt cleanupCnt;
			cleanupCnt = proxy->ProtocolProxy()->GetArgsLC(ENetMsgRequestTransmitLocation, &getSessionId, &getThirdParty, &getPriority);
			
			sequenceId = WasMessageExpectedPmId(aPmId, ENetMsgRequestTransmitLocation);
			INFO_PRINTF2(_L("-> RequestTransmitLocation() [2002] Sq: %d"), sequenceId);
			
			if(sequenceId != KErrNotFound)
				{
				iSequences[sequenceId]->CheckRequestTransmitLocation(getSessionId, getPriority, *getThirdParty);
				}
			
			CleanupStack::PopAndDestroy(cleanupCnt);
			break;
			}
			
		// >> CancelTransmitLocation() [2003]
		case ENetMsgCancelTransmitLocation:
			{
			TLbsNetSessionId* sessionId = NULL;
			TInt reason = 0;
			
			cleanupCnt = proxy->ProtocolProxy()->GetArgsLC(ENetMsgCancelTransmitLocation, &sessionId, &reason);
			
			sequenceId = WasMessageExpectedSessionId(sessionId->SessionNum(), ENetMsgCancelTransmitLocation);
			INFO_PRINTF2(_L("-> CancelTransmitLocation() [2003] Sq: %d"), sequenceId);

			CleanupStack::PopAndDestroy(cleanupCnt);
			
			break;
			}
			
		// >> RequestNetworkLocation() [2007]
		case ENetMsgRequestNetworkLocation:
			{
            TLbsNetSessionId* sessionId = NULL;
            TLbsNetPosRequestOptionsAssistance* opts = NULL;
            cleanupCnt = proxy->ProtocolProxy()->GetArgsLC(ENetMsgRequestNetworkLocation, &sessionId, &opts);

            sequenceId = WasMessageExpectedPmId(aPmId, ENetMsgRequestNetworkLocation);
			
			INFO_PRINTF2(_L("-> RequestNetworkLocation() [2007] Sq: %d"), sequenceId);
			
            if(sequenceId != KErrNotFound)
                {
                iSequences[sequenceId]->CheckNetworkLocationRequest(sessionId);
                }
			
			break;
			}
			
		// >> CancelNetworkLocation [2008]
		case ENetMsgCancelNetworkLocation:
			{
			sequenceId = WasMessageExpectedPmId(aPmId, ENetMsgCancelNetworkLocation);
			
			INFO_PRINTF2(_L("-> CancelNetworkLocation() [2008] Sq: %d"), sequenceId);
			
			//NOTE: Currently no handling implemented for this message type
			
			break;
			}
			
		// Timeout occurred waiting for a NG Message
		case -1000:
			{
			INFO_PRINTF2(_L("Error - Timeout occurred waiting for NG message on PM: %d"), aPmId);
			break;
			}
			
		//Error occurred, not a recognised message.  Fail the test.
		default:
			{
			INFO_PRINTF3(_L("Error - Received invalid NG message: %d on PM: %d"), aMessageType, aPmId);
			return StopTest(KErrArgument);
			}
		}
	
	//If the message was expected, continue the test
	if(sequenceId != KErrNotFound)
		{
		iSequences[sequenceId]->SignalCallbackIdleStart();
		}
	else
		{
		// when an unexpected message arrives merley ignore and wait for next one!
		proxy->WaitForResponseL(80 * 1000 * 1000);
		}
	}

/** Checks to see whether any of the other sequences should be started.
 */
void CT_LbsHybridMultipleTest::CheckForNewSequenceToStart()
	{
	TBool check = EFalse;
	//Check for other sequences to start
	for(TInt i=1; i<iSequences.Count(); ++i)
		{
		//Check to see whether this sequence has been started
		TInt currentPosition = iSequences[i]->GetCurrentPosition();
		if(iSequences[i]->GetCurrentPosition() >= 0)
			{
			//Sequence is started, move onto next sequence
			continue;
			}
		
		//There are still sequences that need to be started, keep checking for new sequences
		check = ETrue;
		
		//Check to see whether this sequence needs to be started
		if(iSequences[0]->GetCurrentPosition() >= iSequences[i]->GetStartPosition())
			{
			iActiveSequences++;
			iSequences[i]->SignalCallbackIdleStart();
			//Break, don't check any other sequences.  This sequence has to be given a chance
			//  to start before trying to start any other sequences.
			break;
			}
		}
	
	// Set the check bool for future checks.  If check is not set to ETrue above then no sequences
	//  are still waiting to be started, so there is no point in checking anymore.
	iCheckForNewSequencesToStart = check;
	}


TInt CT_LbsHybridMultipleTest::HandlerCompleteCallback(TAny* aAny)
    {
    reinterpret_cast<CT_LbsHybridMultipleTest*>(aAny)->HandlerCompleteNotify();
        
    return KErrNone;
    }


/** Handler Complete
 */
void CT_LbsHybridMultipleTest::HandlerCompleteNotify()
    {
    CheckForNewSequenceToStart();
    }

/** Uses the TestExecute logger to allow the CTestSessionSequence class to log messages to
 *    the TestExecute log file.
 * 
 * @param aMessage Contains the message that should be logged
 */
void CT_LbsHybridMultipleTest::LogTestStatement(const TDesC& aMessage)
	{
	INFO_PRINTF2(_L("%S"), &aMessage);
	}

/** Signals the the sequence has finished.  Check to see if the test should now end
 */
void CT_LbsHybridMultipleTest::SequenceFinished()
	{
	//This sequence is finished
	iActiveSequences--;
	//Check to see whether any other sequences are active
	if(iActiveSequences > 0)
		{
		return;
		}
	else
		{
		//No active sequences, end the test
		return StopTest(0);
		}
	}

/** Signal from the Test Sequence that the test should check for more sequences to start
 */
void CT_LbsHybridMultipleTest::SignalCheckForNewSequences()
	{
	if(iCheckForNewSequencesToStart)
		{
		iCheckNewSequenceIdle->Start(TCallBack(HandlerCompleteCallback, this));
		}
	}

/* >> ProcessNetworkLocationRequest()
*/
void CT_LbsHybridMultipleTest::ProcessNetworkLocationRequest(TUint aRequestId, const TLbsExternalRequestInfo& /*aRequestInfo*/, const TNotificationType& /*aNotificationType*/)
	{
	//Loop through the sequences identifying which sequence this privacy request belongs to
	TInt i;
	for(i=0; i<iSequences.Count(); ++i)
		{
		if(iSequences[i]->GetSessionId().SessionNum() == aRequestId)
			{
			INFO_PRINTF2(_L("--> ProcessNetworkLocationRequest() [3000] Sq: %d"), i);
			
			if(iSequences[i]->GetCurrentAction() == EProcessNetworkLocationRequest)
				{
				iSequences[i]->SignalCallbackIdleStart();
				}
			
			break;
			}
		}
	
	INFO_PRINTF2(_L("<-- RespondNetworkLocationRequest() Sq: %d"), i);
	iController->RespondNetworkLocationRequest(aRequestId, CLbsPrivacyController::ERequestAccepted);
	}
   
/** >> ProcessNetworkLocationUpdate()
 */
void CT_LbsHybridMultipleTest::ProcessNetworkPositionUpdate(TUint aRequestId, const TPositionInfo& /*aPosInfo*/)
	{
	for(TInt i=0; i<iSequences.Count(); ++i)
		{
		if(iSequences[i]->GetSessionId().SessionNum() == aRequestId)
			{
			INFO_PRINTF2(_L("--> ProcessNetworkPositionUpdate() [3001] Sq: %d"), i);
			
			if(iSequences[i]->GetCurrentAction() == EProcessNetworkPositionUpdate)
				{
				iSequences[i]->SignalCallbackIdleStart();
				}
			break;
			}
		}
	}

/** >> ProcessRequestComplete()
 */
void CT_LbsHybridMultipleTest::ProcessRequestComplete(TUint aRequestId, TInt aReason)
	{	
	for(TInt i=0; i<iSequences.Count(); ++i)
		{
		if(iSequences[i]->GetSessionId().SessionNum() == aRequestId)
			{
			INFO_PRINTF3(_L("--> ProcessRequestComplete(%d) [3002] Sq: %d"), aReason, i);
			
			if(iSequences[i]->GetCurrentAction() == EProcessRequestComplete)
				{
				iSequences[i]->SignalCallbackIdleStart();
				}
			break;
			}
		}
	}
