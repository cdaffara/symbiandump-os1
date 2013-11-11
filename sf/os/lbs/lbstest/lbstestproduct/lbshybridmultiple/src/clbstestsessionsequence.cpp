// Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// Description: Stores information for a test sequence
// 
//

#include "clbstestsessionsequence.h"

/** 2nd Phase constructor
 * 
 * @param aNgProxy Reference to the Net Protocol Proxy
 * @param aTestCallback MHybridMultipleTestCallBack, used to access the main test
 * @param aSequenceNum Number of this sequence
 * @param aConfigFileName File name of the test ini file
 * @param aConfigSection Test config section
 */
CTestSessionSequence* CTestSessionSequence::NewL(CLbsTestNgMessageHandler& aNgProxy, MHybridMultipleTestCallBack& aTestCallback,
		TInt aSequenceNum, const TDesC& aConfigFileName, const TDesC& aConfigSection)
	{
	CTestSessionSequence* self = new (ELeave) CTestSessionSequence(aNgProxy, aTestCallback);
	CleanupStack::PushL(self);
	self->ConstructL(aSequenceNum, aConfigFileName, aConfigSection);
	CleanupStack::Pop(self);
	return self;
	}

/** Destructor
 */
CTestSessionSequence::~CTestSessionSequence()
	{
	if(iCallbackIdle)
		{
		iCallbackIdle->Cancel();
		delete iCallbackIdle;
		}
	iStateSequence.Close();
	delete iIniFile;
	}
	
/** Standard C Constructor
 */
CTestSessionSequence::CTestSessionSequence(CLbsTestNgMessageHandler& aNgProxy, MHybridMultipleTestCallBack& aTestCallback)
:iCurrentPosition(-1),
iStartPosition(0),
iNgProxy(aNgProxy),
iTestCallback(aTestCallback)
	{
	}
	
/** Second phase construction
 */
void CTestSessionSequence::ConstructL(TInt aSequenceNum, const TDesC& aConfigFileName, const TDesC& aConfigSection)
	{
	iCallbackIdle = CIdle::NewL((CActive::EPriorityStandard));
	
	//Open up the testdata ini file
	iIniFile = CIniData::NewL(aConfigFileName);
	
	//Extract the data for sequence number: aSequenceNum
	iSequenceId = aSequenceNum;
	
	LogTestStatement(_L("----------------------------- Sequence %d -----------------------------"), 1, iSequenceId);
	
	//Read in the Session ID
	TBuf<11> sessionIdName;
	sessionIdName.Format(_L("SessionId%d"), iSequenceId);
	TInt sessionNum;
	if(iIniFile->FindVar(aConfigSection, sessionIdName, sessionNum))
		{
		iSessionId.SetSessionNum(sessionNum);
		LogTestStatement(_L("SessionId = %d"), 1, sessionNum);
		}
	
	//Set the Session ID owner to the correct PM ID
	iPmId = iNgProxy.GetPmId();
	_LIT(KPmIniFileName, "c:\\testdata\\configs\\pminfo.ini");
	CIniData* pmIniFile = CIniData::NewL(KPmIniFileName);
	CleanupStack::PushL(pmIniFile);
	
	TBuf<4> pmName;
	pmName.Format(_L("PM%d"), iPmId);
	
	TUint32 pmUid;
	TPtrC pmIdBuf;
	pmIniFile->FindVar(pmName, _L("UID"), pmIdBuf);
	TLex lexId(pmIdBuf);
	User::LeaveIfError(lexId.Val(pmUid, EHex));
	iSessionId.SetSessionOwner(TUid::Uid(pmUid));
	LogTestStatement(_L("PMUid = %X"), 1, pmUid);
	CleanupStack::PopAndDestroy(pmIniFile);
		
	// Get the starting position of the sequence, if it is not the first 'master' sequence
	TInt startPosition = 0;
	if(aSequenceNum != 0)	//The first sequence is always the master sequence, hence always set the start position to 0
		{
		TBuf<15> startPositionName;
		startPositionName.Format(_L("StartPosition%d"), iSequenceId);
		iIniFile->FindVar(aConfigSection, startPositionName, startPosition);
		}
	iStartPosition = startPosition;
	LogTestStatement(_L("StartPosition = %d"), 1, startPosition);
	
	// Read in the actual sequence 
	TBuf<10> sequenceName;
	sequenceName.Format(_L("Sequence%d"), iSequenceId);
	TPtrC sequenceString;
	iIniFile->FindVar(aConfigSection, sequenceName, sequenceString);
	
	//Create sequence log buffer, to allow the sequence of messages to be logged to the TestExecute file
	TBuf<256> sequenceLog;
	sequenceLog.Append(_L("Sequence = "));
	
	//Loop through the string extracting the values and appending them to the sequence array
	TLex lex(sequenceString);
	while(!lex.Eos())
		{
		TPtrC token = lex.NextToken();
		TLex con(token);
		TInt value;
		if (con.Val(value) == KErrNone)
			{
			iStateSequence.Append(value);
			sequenceLog.AppendNum(value);
			sequenceLog.Append(_L(" "));
			}
		}
	iTestCallback.LogTestStatement(sequenceLog);
	}

/** Main test function.  This handles the next message in the sequence (either send or receive a message)
 */
void CTestSessionSequence::HandleMessage()
	{
	TInt request = GetCurrentAction();
	if(request == KErrArgument)
		{
		//This sequence is finished, notify the test
		iTestCallback.SequenceFinished();
		return;
		}
	
	TBuf<256> logStatement;
	
	//Identify the next action that needs to be completed:
	// 1. Send Message (PM -> NG)
	// 2. Receive Message (NG -> PM)
	// 3. Receive Privacy Request
	// 4. Receive NPUD
	// 5. Receive X3P
	switch(request)
		{
		//------------------------------
		//Messages from test code to PM
		//------------------------------
		case ENetMsgProcessStatusUpdate:
			{
			LogTestStatement(_L("<- ProcessStatusUpdate() [1006] Sq: %d"), 1, iSequenceId);
			SendProcessStatusUpdate();
			break;
			}

		case ENetMsgProcessPrivacyRequest:
			{
			LogTestStatement(_L("<- ProcessPrivacyRequest() [1000] Sq: %d"), 1, iSequenceId);
			SendProcessPrivacyRequest();
			break;
			}

		case ENetMsgProcessLocationUpdate:
			{
			LogTestStatement(_L("<- ProcessLocationUpdate() [1004] Sq: %d"), 1, iSequenceId);
			SendProcessLocationUpdate();
			break;
			}
			
		case ENetMsgProcessAssistanceData:
			{
			LogTestStatement(_L("<- ProcessAssistanceData() [1003] Sq: %d"), 1, iSequenceId);
			SendProcessAssistanceData();
			break;
			}
			
		case ENetMsgProcessLocationRequest:
			{
			LogTestStatement(_L("<- ProcessLocationRequest() [1001] Sq: %d"), 1, iSequenceId);
			SendProcessLocationRequest();
			break;
			}
			
		case ENetMsgProcessSessionComplete:
			{
			LogTestStatement(_L("<- ProcessSessionComplete() [1002] Sq: %d"), 1, iSequenceId);
			SendProcessSessionComplete();
			break;
			}
			
		case ENetTestMsgEndSession:
			{
			LogTestStatement(_L("<- ProcessSessionComplete() [1100 (1002)] Sq: %d"), 1, iSequenceId);
			SendProcessSessionComplete();
			LogTestStatement(_L("<- ProcessStatusUpdate() [1100 (1006)] Sq: %d"), 1, iSequenceId);
			CTestSessionSequence::SendProcessStatusUpdate();
			break;
			}
			
		case ENetTestMsgProcessSessionCompleteError:
            {
            LogTestStatement(_L("<- ProcessSessionCompleteError() [1101] Sq: %d"), 1, iSequenceId);
            SendProcessSessionComplete(KErrNotFound);
            break;
            }		    
			
		//------------------------------
		//Messages from PM to test code
		//------------------------------
		case ENetMsgRespondPrivacyRequest:
		case ENetMsgRespondLocationRequest:
		case ENetMsgRequestTransmitLocation:
		case ENetMsgCancelTransmitLocation:
		case ENetMsgRequestAssistanceData:
		case ENetMsgRequestSelfLocation:
		case ENetMsgCancelSelfLocation:
		case ENetMsgRequestNetworkLocation:
		case ENetMsgCancelNetworkLocation:
		case ENetMsgGetCurrentCapabilitiesResponse:
			{
			//Set the current sequence to wait for a message from the NG/LBS
			WaitForMessage();
			
			break;
			}
			
		//--------------------------------
		//Messages from Privacy Controller
		//--------------------------------
		case EProcessNetworkLocationRequest:
		case EProcessNetworkPositionUpdate:
		case EProcessRequestComplete:
			{
			//Do nothing. This sequence is now in a waiting state.  The callback (Privacy Handler) methods
			// in this class should get called by LBS.  When they are called they will allow the sequence
			// to move onto the next message.
			break;
			}
		
		//--------------------------------------
		//Messages from Test to LBS Self Locate
		//--------------------------------------	
		case ERequestSelfLocation:
			{
			LogTestStatement(_L("<- SelfLocationRequest [4000] Sq: %d"), 1, iSequenceId);
			RequestLocationUpdate();
			break;
			}
			
		//----------------------------------
		// Messages from LBS Self Locate API
		//----------------------------------
		case ENotifyPositionUpdate:
		case EGetLastKnownLocation:
			{
			//Do nothing.  This sequence is now in a waiting state.  The callback (NPUD) methods in the
			// CTestMolrSessionSequence should get called by LBS.  When they are called they will allow
			// the sequence to move onto the next message.
			break;
			}
			
		//------------------------------
		//Messages from Test to LBS X3P
		//------------------------------
		case ETransmitPosition:
			{
			LogTestStatement(_L("<- TransmitPosition [6000] Sq: %d"), 1, iSequenceId);
			TransmitPosition();
			break;
			}
			
		//--------------------------
		// Messages from LBS X3P API
		//--------------------------
		case EX3PRefPositionUpdate:
		case EX3PPositionUpdate:
			{
			//Do nothing.  This sequence is now in a waiting state.  The callback (X3P) methods in the
			// CTestX3PSessionSequence should get called by LBS.  When they are called they will allow
			// the sequence to move onto the next message.
			break;
			}
		
		//Error
		default:
			{
			//Error
			LogTestStatement(_L("Error - Invalid sequence entry number: %d"), 1, iSequenceId);
			iTestCallback.StopTest(KErrArgument);
			}
			
		} //End Switch Statement
	
	//If the above Handle Message sent a message to LBS and the sequence is not currently
	// waiting for anything, execute the next statement in the sequence (so this is only
	// required for certain message types).
	switch(request)
		{
		case ENetMsgProcessPrivacyRequest:
		case ENetMsgProcessLocationRequest:
		case ENetMsgProcessSessionComplete:
		case ENetMsgProcessAssistanceData:
		case ENetMsgProcessLocationUpdate:
		case ENetMsgGetCurrentCapabilitiesRequest:
		case ENetMsgProcessStatusUpdate:
		case ERequestSelfLocation:
		case ENetTestMsgEndSession:
		case ENetTestMsgProcessSessionCompleteError:
		case ETransmitPosition:
			{
			SignalCallbackIdleStart();
			break;
			}
		}
	
	//Check to see whether any new sequences need to be started
	if(iSequenceId == 0)
		{
		iTestCallback.SignalCheckForNewSequences();
		}
	}
	
/** Returns the session ID of the sequence
 */
TLbsNetSessionId CTestSessionSequence::GetSessionId()
	{
	return iSessionId;
	}

/** Returns the ID of the PM used by this session sequence
 */
TInt CTestSessionSequence::GetPmId()
	{
	return iPmId;
	}

/** Returns the current position of the sequence
 */
TInt CTestSessionSequence::GetCurrentPosition()
	{
	return iCurrentPosition;
	}

/** Increments the current position of the sequence to the next entry
 */
void CTestSessionSequence::IncrementPosition()
	{
	iCurrentPosition++;
	}

/** Returns the starting position of the sequence
 */
TInt CTestSessionSequence::GetStartPosition()
	{
	return iStartPosition;
	}

/** Gets the current action from the sequence
 * Note: The position is NOT incremented automatically.  This needs to be manually done
 * with a call to IncrementPosition() call.  This allows the test to see what the next
 * entry in the sequence is, without consuming that entry.
 */
TInt CTestSessionSequence::GetCurrentAction()
	{
	if((iCurrentPosition >= iStateSequence.Count()) || (iCurrentPosition < 0))
		{
		return KErrArgument;
		}
	return iStateSequence[iCurrentPosition];
	}

/** Prints out the remaining actions in the sequence.  This should only be used at the
 *   end of the test.  If the test fails, it calls this function to print out what the
 *   sequence was still expecting which can help with debugging where the test stopped.
 */
void CTestSessionSequence::PrintRemainingActions()
	{
	TInt currentAction = GetCurrentAction();
	LogTestStatement(_L("Sequence %d; PM ID %d:"), 2, iSequenceId, iPmId);
	
	if(currentAction == KErrArgument)
		{
		//This sequence is finished
		iTestCallback.LogTestStatement(_L(" - Test Sequence Completed"));
		return;
		}
		
	//Loop through the sequence, logging all the remaining actions
	TBuf<256> sequenceListLog;
	sequenceListLog.Append(_L(" - "));
	while(currentAction != KErrArgument)
		{
		sequenceListLog.AppendNum(currentAction);
		sequenceListLog.Append(_L(" "));
		
		//Increment the position and get the current action
		IncrementPosition();
		currentAction = GetCurrentAction();
		}
	iTestCallback.LogTestStatement(sequenceListLog);
	}

/** Simply calls the NG Protocol Proxy to wait for a message from LBS
 */
void CTestSessionSequence::WaitForMessage()
	{
	//All timeout's are now 60 seconds
	iNgProxy.WaitForResponseL(80 * 1000 * 1000);
	}

//----------------------------------------------
// NG Message Implementation functions
//----------------------------------------------

/* << ProcessAssistanceData()
 */
void CTestSessionSequence::SendProcessAssistanceData()
	{
	TLbsAsistanceDataGroup dataRequestMask = EAssistanceDataReferenceLocation;
	RLbsAssistanceDataBuilderSet assistanceData;
	ArgUtils::PopulateLC(assistanceData);
	TInt reason = KErrNone;
	iNgProxy.ProtocolProxy()->CallL(ENetMsgProcessAssistanceData, &dataRequestMask, &assistanceData, &reason);
	CleanupStack::PopAndDestroy(); //assistanceData
	}

/* << ProcessSessionComplete()
 */
void CTestSessionSequence::SendProcessSessionComplete(TInt aReason)
	{
	iNgProxy.ProtocolProxy()->CallL(ENetMsgProcessSessionComplete, &iSessionId, &aReason);
	}

/* << ProcessLocationUpdate()
 */
void CTestSessionSequence::SendProcessLocationUpdate()
	{
	TPositionInfo positionInfo = ArgUtils::ReferencePositionInfo();
	iNgProxy.ProtocolProxy()->CallL(ENetMsgProcessLocationUpdate, &iSessionId, &positionInfo);
	}

/** << ProcessPrivacyRequest()
 */
void CTestSessionSequence::SendProcessPrivacyRequest()
	{	//Implementation is MTLR specific
	iTestCallback.StopTest(KErrArgument);
	}

/* << ProcessStatusUpdate()
 */
void CTestSessionSequence::SendProcessStatusUpdate()
	{
	MLbsNetworkProtocolObserver::TLbsNetProtocolServiceMask activeServiceMask = MLbsNetworkProtocolObserver::EServiceNone;
	iNgProxy.ProtocolProxy()->CallL(ENetMsgProcessStatusUpdate, &activeServiceMask);
	}

/* << ProcessLocationRequest()
 */
void CTestSessionSequence::SendProcessLocationRequest()
	{
	//Requires Specific implementation
	iTestCallback.StopTest(KErrArgument);
	}

//----------------------------------------------
// NG Message Checker functions
//----------------------------------------------

/** Checks SelfLocationRequest()
 */
void CTestSessionSequence::CheckSelfLocationRequest(TLbsNetSessionId* /*aSessionId*/)
	{
	//Should only be received on MOLR requests
	iTestCallback.StopTest(KErrArgument);
	}

/** Checks RequestTransmitLocation()
 */
void CTestSessionSequence::CheckRequestTransmitLocation(TLbsNetSessionId* /*aSessionId*/, TInt /*aPriority*/, const TDesC16& /*aThirdParty*/)
	{
	//Should only be received on X3P requests
	iTestCallback.StopTest(KErrArgument);
	}

/** Checks NetworkLocationRequest()
 */
void CTestSessionSequence::CheckNetworkLocationRequest(TLbsNetSessionId* /*aSessionId*/)
    {
    //Should only be received on MOLR requests
    iTestCallback.StopTest(KErrArgument);
    }

/** Checks RespondPrivacyRequest()
 */
void CTestSessionSequence::CheckRespondPrivacyRequest(CLbsNetworkProtocolBase::TLbsPrivacyResponse /*aPrivacyResponse*/)
	{
	//Should only be received on MTLR requests
	iTestCallback.StopTest(KErrArgument);
	}

//----------------------------------------------
// MOLR Self Location Requests
//----------------------------------------------

/** Notify Position Update
 */
void CTestSessionSequence::RequestLocationUpdate()
	{
	//Incorrectly received a request to make a location request
	iTestCallback.StopTest(KErrArgument);
	}

//----------------------------------------------
// X3P Transmit Location Requests
//----------------------------------------------

void CTestSessionSequence::TransmitPosition()
	{
	//Incorrectly received a request to make a X3P request
	iTestCallback.StopTest(KErrArgument);
	}


//------------------------------
// Idle Callback Functions
//------------------------------

void CTestSessionSequence::SignalCallbackIdleStart()
	{
	//The previous action was sent or received correctly.  Always call IncrementPosition so  
	// that the this deals with the next expected action.
	IncrementPosition();
	
	//Check to make sure the CIdle object is not currently active
	if(!iCallbackIdle->IsActive())
		{
		iCallbackIdle->Start(TCallBack(HandlerCompleteCallback, this));
		}
	else
		{
		//If it is still active, re-call the NG Proxy to listen for another message.  The reason for this:
		// If this sequence's CIdle object is active, this sequence is "between" actions.  It has already
		// been told to deal with the next action, but the CIdle has not been completed and so the HandleMessage()
		// function has not been called.  HOWEVER a message must have arrived on the NG-PM interface that is meant
		// for this sequence AND since this function was called, it was an expected message.  SO this sequence has
		// "stolen" a WaitForResponse call from another sequence.  As such, manually make another call from this
		// sequence on the same PM to satisfy the action on that other sequence.
		iNgProxy.WaitForResponseL(80 * 1000 * 1000);
		}
	}

TInt CTestSessionSequence::HandlerCompleteCallback(TAny* aAny)
	{
	 reinterpret_cast<CTestSessionSequence*>(aAny)->HandlerCompleteNotify();
	        
	 return KErrNone;
	}

void CTestSessionSequence::HandlerCompleteNotify()
	{
	HandleMessage();
	}

/** Simple TestSessionSequence logging function.  It uses the TestCallBack and passes
 *   in a string to log via the TestExecute logger.
 */
void CTestSessionSequence::LogTestStatement(const TDesC& aMessage, TInt aCount, ...)
	{
	TBuf<256> logStatement;
	RArray<TInt> values;
	CleanupClosePushL(values);
	
	VA_LIST list;
	VA_START(list, aCount);
	for(TInt i=0; i<aCount; ++i)
		{
		values.Append(VA_ARG(list, TInt));
		}
	
	switch(aCount)
		{
		case 1:
			logStatement.Format(aMessage, values[0]);
			break;
			
		case 2:
			logStatement.Format(aMessage, values[0], values[1]);
			break;
			
		case 3:
			logStatement.Format(aMessage, values[0], values[1], values[2]);
			break;
		}
	
	iTestCallback.LogTestStatement(logStatement);
	
	CleanupStack::PopAndDestroy(&values);
	VA_END(list);
	}


