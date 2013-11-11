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
// The base class for the CTSY integration test suite.
// 
//

/**
 @file 
 @internalTechnology
*/

#include "tetelrequeststatus.h"
#include "cetelsessionmgr.h"
#include "cctsyintegrationtestsuitebase.h"
#include "rpsmaster.h"
#include "cctsyinidata.h"
#include "trpsfunctor.h"
#include "trpsfunctor.cpp" //CW does not let us put function body of a templated function into separate translation unit
	//It causes a link error where function is invoked. Instead we need to provide function body
	//in for use by the invoking translation unit.
/**
 * Default constructor.
 */
CCTSYIntegrationTestSuiteStepBase::CCTSYIntegrationTestSuiteStepBase(CEtelSessionMgr& aEtelSessionMgr)
	:iEtelSessionMgr(aEtelSessionMgr)
	{
	}

/**
 * Destructor.
 */
CCTSYIntegrationTestSuiteStepBase::~CCTSYIntegrationTestSuiteStepBase()
	{
	}


/**
 * Implementation of CTestStep base class virtual.
 *
 * It is used for doing all initialisation common to derived classes in here.
 * Make it able to leave if there are any errors here as there's no point in
 * trying to run a test step if anything fails.
 * The leave will be picked up by the framework.
 *
 * @return	TVerdict
 */
TVerdict CCTSYIntegrationTestSuiteStepBase::doTestStepPreambleL()
	{
	//cleanup RPS from previous test
	RPSCleanupL();
	
	iEtelSessionMgr.OpenL(ETrue);	// Open all session by default
	iOwnNumbersCache.CacheOwnNumbersL();

	SetTestStepResult(EPass);
	StartSetup();
	return TestStepResult();
	}

/** 
 * Implementation of CTestStep base class virtual
 * It is used for doing all after test treatment common to derived classes in here.
 * Make it able to leave.
 * The leave will be picked up by the framework.
 *
 * @return	TVerdict
 */
TVerdict CCTSYIntegrationTestSuiteStepBase::doTestStepPostambleL()
	{
	iEtelSessionMgr.Close();
	return TestStepResult();
	}

/**
*/
void CCTSYIntegrationTestSuiteStepBase::StartSetup()
	{
	iAssertResult = ETestSuiteError;
	iCheckResult = EInconclusive;
	}

/**
 * Called when the test starts.
*/
void CCTSYIntegrationTestSuiteStepBase::StartTest()
	{
	iAssertResult = EFail;
	iCheckResult = EFail;
	}

/**
 * Called when the test has finished and the test clean up begins.
*/
void CCTSYIntegrationTestSuiteStepBase::StartCleanup()
	{
	// don't report cleanup problems as failures
	iAssertResult = EPass;
	iCheckResult = EPass;
	}

/**
 */
void CCTSYIntegrationTestSuiteStepBase::SetAssertResult() 
	{
	if (iAssertResult != EPass)
		SetTestStepResult(iAssertResult); 
	}

/**
 */
void CCTSYIntegrationTestSuiteStepBase::SetCheckResultL() 
	{ 
	if (iCheckResult != EPass)
		SetTestStepResult(iCheckResult); 
	User::Leave(iCheckResult); 
	}

/**
 * This function should be used by manual tests to pause a test and
 * wait for user interaction.  The user is prompted to perform an
 * action and then the test will wait until the user presses a key
 * to indicate that the action has been performed.
 * 
 * @param aText The instructions to display to the user.
 * @param aTimeout Timeout.
 */
void CCTSYIntegrationTestSuiteStepBase::DisplayUserInteractionPromptL(const TDesC& aText, TTimeDuration aTimeOut)
	{
	_LIT(KPressAnyKeyToContinue, "\nPress any key to continue.");
	TBuf<255> instructionBuf(aText);
	instructionBuf.Append(KPressAnyKeyToContinue);
	DoPauseL(instructionBuf, aTimeOut);
	}

/**
 * Waits for asynchronous request using aRequestStatus to complete or for
 * a period of time aTimeOut to pass which ever is sooner.
 * 
 * @param aRequestStatus Request status to wait for.
 * @param aTimeOut Timeout for aRequestStatus.
 * 
 * @return KErrNone if aRequestStatus completes or KErrTimedOut if 
 * aRequestStatus did not complete within the aTimeOut period.
 */
TInt CCTSYIntegrationTestSuiteStepBase::WaitForRequestWithTimeOut
		(TEtelRequestBase& aRequestStatus, TTimeDuration aTimeOut)
	{
	RTimer timer;
	TRequestStatus timerRequest;
	timer.CreateLocal();
	timer.After(timerRequest, aTimeOut);

	// Wait for the request to complete or until we time out
	User::WaitForRequest(timerRequest, aRequestStatus);

	// we must cancel the callers request if it is still outstanding
	if (aRequestStatus.Int() == KRequestPending)
		{
		WARN_PRINTF1(_L("WaitForRequestWithTimeOut - cancelling pending request"));
		aRequestStatus.Cancel();
		}

	// If timer is no longer pending => we have timed out
	if (timerRequest != KRequestPending)
		{
		INFO_PRINTF1(_L("WaitForRequestWithTimeOut *** Timed out ***"));
		return KErrTimedOut;
		}

	timer.Cancel();
	User::WaitForRequest(timerRequest);
	
	return KErrNone;
	}
	
void CCTSYIntegrationTestSuiteStepBase::RPSCleanupL()
	{
	if(iRPS)
		{	    
	    _LIT(KRpsCallTxt, "RPS: CLEANUP");
	    INFO_PRINTF1(KRpsCallTxt);	   	
	   	iRPS->CleanupL();
		}
	}

void CCTSYIntegrationTestSuiteStepBase::ConsolePrintL(const TDesC& aText )
	{
	CConsoleBase* con = NULL;
	TRAPD(err, con = Console::NewL(_L("Interactive Print"), TSize(KConsFullScreen, KConsFullScreen)));
	TEST(err == KErrNone);

	CleanupStack::PushL(con);
	con->Printf(_L("%S"), &aText);
	CleanupStack::PopAndDestroy(); // con
	
	}


TInt CCTSYIntegrationTestSuiteStepBase::DoPauseL(const TDesC& aText, TTimeDuration aTimeout /* = ETimeMedium */)
/**
 Performs a pause, usually to allow user to intervene in Manual tests
 
 @param		aText - text for prompt
 @param		aTimeout - 
 @return	KErrNone if user pressed a key
 */
	{
	TInt ret = KErrNone;
		
	CConsoleBase* con = NULL;
	TRAPD(err, con = Console::NewL(_L("Interactive Test"), TSize(KConsFullScreen, KConsFullScreen)));

	INFO_PRINTF2(_L("Console status = %d"), err);
	TEST(err == KErrNone);
	CleanupStack::PushL(con);
	
	TConsoleReadRequestStatus readRequest(*con);
	//add to cleanup stack
	CleanupStack::PushL(readRequest);
	con->Printf(_L("%S (timeout %d secs) ..."), &aText, aTimeout / KOneSecond);
	con->Read(readRequest);
	
	ret = WaitForRequestWithTimeOut(readRequest, aTimeout);	
		
	if (ret == KErrTimedOut)
		{
		WARN_PRINTF1(_L("[doPause] No keypress detected, timeout! Manual action may not have occurred."));
		}

	if (readRequest.Int() == KRequestPending)	
		{
		readRequest.Cancel();
		}
	
	CleanupStack::PopAndDestroy(); // readRequest
	CleanupStack::PopAndDestroy(); // con
	
	return ret;
	}
	
TInt CCTSYIntegrationTestSuiteStepBase::ExecuteRemoteRequestL(MRpsFunctorBase& aRpsFunctor, const TDesC& aRpsCallMsg)
	{
	TInt ret = KErrNone;
	if(iRPS) //if RPS is being used then dispatch the request to the RPS slave				
		{												
		INFO_PRINTF1(aRpsCallMsg);	
		aRpsFunctor.ExecuteRpsRequestL();
		}												
	else //if RPS not being used then default to dislay request text through UI.
		{
		INFO_PRINTF1(aRpsFunctor.DisplayToClientTxt());							
		ret = DoPauseL(aRpsFunctor.DisplayToClientTxt());						    
		}												
	return ret;		
	}
	
TPtrC CCTSYIntegrationTestSuiteStepBase::Number(TEtelLine aLine)
	{
	TPtrC ptr;
    switch(aLine)
		{
	case EDataLine:
		ptr.Set(iOwnNumbersCache.OwnNumbers().OwnDataNumber());
		break;
	case EFaxLine:
		ptr.Set(iOwnNumbersCache.OwnNumbers().OwnFaxNumber());
		break;
	case EVoiceLine:
		default:
		ptr.Set(iOwnNumbersCache.OwnNumbers().OwnVoiceNumber());
		}
		
	return ptr;
	}
	
// Set aNumber to the relevant phone number based on the line type (either data, fax or voice line)
// If Using RPS then the number will be retrieved from RPS, otherwise it will read the number from the 
// ctsyintegration_data.ini file.
void CCTSYIntegrationTestSuiteStepBase::GetRPSNumber(TEtelLine aLine, TPtrC& aNumber) 
	{

	if (UsingRps())
		{
		 switch(aLine)
			{
			case EDataLine:
				aNumber.Set(iRPS->RpsSlaveTelNumData());
				break;
			case EFaxLine:
				aNumber.Set(iRPS->RpsSlaveTelNumFax());
				break;
			case EVoiceLine:
				default:
				aNumber.Set(iRPS->RpsSlaveTelNum());
			}		
		}
	else
		{
		 switch(aLine)
			{
			case EDataLine:
				GetStringFromConfig(KIniOwnNumSection,KIniOwnDataNumber,aNumber);
				break;
			case EFaxLine:
				GetStringFromConfig(KIniOwnNumSection,KIniOwnFaxNumber,aNumber);
				break;
			case EVoiceLine:
				default:
				GetStringFromConfig(KIniOwnNumSection, KIniOwnVoiceNumber1, aNumber);
			}		
		}
	}

//RPS Dispatch requests. 
//IF RPS off then will defualt to user interaction.

// Requests RPS to dial out to the specified number
//aLine line to use - voice, data, fax
TInt CCTSYIntegrationTestSuiteStepBase::RPSDialNumberL(const TDesC& aNumToDial, TEtelLine aLine)
	{
    
    _LIT(KDisplayToClientTxt, "Hit a key and dial the number");
    _LIT(KRpsCallTxt, "RPS: RPSDialNumberL");
    
    TRpsFunctor<const TDesC&, TEtelLine, MRpsFunctorBase::TNotUsed> rpsFunctor(iRPS, KDisplayToClientTxt, aLine, &CRPSMaster::CallNumberL, aNumToDial, aLine);
       	
   	return ExecuteRemoteRequestL(rpsFunctor, KRpsCallTxt);												
	}

//RPS Dispatch requests. 
//IF RPS off then will defualt to user interaction.

// Requests an incoming call.
//aLine line to use - voice, data, fax
TInt CCTSYIntegrationTestSuiteStepBase::RPSRequestIncomingCallL(TEtelLine aLine)
	{
    
    _LIT(KDisplayToClientTxt, "Hit a key to place the incoming %S call yourself...");
    _LIT(KRpsCallTxt, "RPS: REQUESTINCOMINGCALLL");
    
    TRpsFunctor<const TDesC&, TEtelLine, MRpsFunctorBase::TNotUsed> rpsFunctor(iRPS, KDisplayToClientTxt, aLine, &CRPSMaster::CallNumberL, Number(aLine), aLine);
       	
   	return ExecuteRemoteRequestL(rpsFunctor, KRpsCallTxt);												
	}
	
//
// REQUESTINCOMING_CANCEL_BEFORE_CONNECTL().
// Requests an incoming call which will get cancelled (ie the caller will end the call) after the delay.
// There is no gaurantee that the call will not get answered by the network before the delay completes,
// so the delay should be kept to a minimum.
// aLine	the line to operate on
// aDelay	A delay in seconds
//
TInt CCTSYIntegrationTestSuiteStepBase::RPSRequestIncomingCancelBeforeConnectL(TEtelLine aLine, TInt aDelay)
	{	
    
    _LIT(KDisplayToClientTxt, "Place an incoming %S call and hangup before TSY answers. Press any key to continue.");
    _LIT(KRpsCallTxt, "RPS: REQUESTINCOMING_CANCEL_BEFORE_CONNECTL");
    
    TRpsFunctor<const TDesC&, TEtelLine, TInt> rpsFunctor(iRPS, KDisplayToClientTxt, aLine, &CRPSMaster::CallNumberHangUpBeforeConnectL, Number(aLine), aLine, aDelay);
       	
   	return ExecuteRemoteRequestL(rpsFunctor, KRpsCallTxt);
	}
	
//	
// Requests an incoming call which will get hung up by the remote party after the specified delay once
// the call is answered. The incoming call must be answered else the operation will fail.
// aLine the line to operate on
// aDelay	A delay in seconds
//
TInt CCTSYIntegrationTestSuiteStepBase::RPSRequestIncomingHangupAfterConnectL(TEtelLine aLine, TInt aDelay)
	{

    _LIT(KDisplayToClientTxt, "Place an incoming %S call and hangup after the TSY answers. Press any key to continue.");
    _LIT(KRpsCallTxt, "RPS: REQUESTINCOMING_HANGUP_AFTER_CONNECTL");
    
    TRpsFunctor<const TDesC&, TEtelLine, TInt> rpsFunctor(iRPS, KDisplayToClientTxt, aLine, &CRPSMaster::CallNumberHangUpAfterConnectL, Number(aLine), aLine, aDelay);
       	
   	return ExecuteRemoteRequestL(rpsFunctor, KRpsCallTxt);
	}	
	
//	
// Requests the remote party to expect a call which is to be answered by the remote party, and then
// wait for a delay before it hangs up.
// A call must be made to the remote party after issuing this request and before any other request
// is made, else this request will fail.
// aLine the line to operate on
// aDelay	A delay in seconds
//
TInt CCTSYIntegrationTestSuiteStepBase::RPSAnswerNextCallThenHangupL(TEtelLine aLine, TInt aDelay)
	{	
    
    _LIT(KDisplayToClientTxt, "Answer next %S call and then hangup. Press any key to continue.");
    _LIT(KRpsCallTxt, "RPS: ANSWERNEXTCALL_THEN_HANGUPL");
    
    TRpsFunctor<TEtelLine, TInt, MRpsFunctorBase::TNotUsed> rpsFunctor(iRPS, KDisplayToClientTxt, aLine, &CRPSMaster::AnswerNextCallHangUpL, aLine, aDelay);
   	
   	return ExecuteRemoteRequestL(rpsFunctor, KRpsCallTxt);
	}
	
//	
// Requests the remote party to expect a call which is to be answered by the remote party, and then
// wait for a delay before it hangs up.
// A call must be made to the remote party after issuing this request and before any other request
// is made, else this request will fail.
// aLine the line to operate on
//
TInt CCTSYIntegrationTestSuiteStepBase::RPSAnswerNextCallL(TEtelLine aLine)
	{
    
    _LIT(KDisplayToClientTxt, "Answer next %S call. Press any key to continue.");
    _LIT(KRpsCallTxt, "RPS: ANSWERNEXTCALLL");
    
    TRpsFunctor<TEtelLine, MRpsFunctorBase::TNotUsed, MRpsFunctorBase::TNotUsed> rpsFunctor(iRPS, KDisplayToClientTxt, aLine, &CRPSMaster::AnswerNextCallL, aLine);
   	
   	return ExecuteRemoteRequestL(rpsFunctor, KRpsCallTxt);
   	}

//	
// Requests the remote party to expect a call which is to be answered by the remote party, and then
// wait for a delay before it puts the call on hold.
// A call must be made to the remote party after issuing this request and before any other request
// is made, else this request will fail.
// The call at the remote party needs to be hung up (which can be achieved using the request HANGUP()).
// There is no gaurantee the call supports or is in a state to be put on hold. The user is advised to
// check the call state reflects it has been put on hold successfully.
//
// aLine the line to operate on
// aDelay	A delay in seconds
//
TInt CCTSYIntegrationTestSuiteStepBase::RPSAnswerNextCallThenHoldL(TEtelLine aLine, TInt aDelay)
	{
    
    _LIT(KDisplayToClientTxt, "Answer next %S call and then hold. Press any key to continue.");
    _LIT(KRpsCallTxt, "RPS: ANSWERNEXTCALL_THEN_HOLDL");
    
    TRpsFunctor<TEtelLine, TInt, MRpsFunctorBase::TNotUsed> rpsFunctor(iRPS, KDisplayToClientTxt, aLine, &CRPSMaster::AnswerNextCallHoldL, aLine, aDelay);
   	
   	return ExecuteRemoteRequestL(rpsFunctor, KRpsCallTxt);
   	}	

//	
// Requests the remote party to expect a call which is not to be answered by the remote party.
// A call must be made to the remote party after issuing this request and before any other request
// is made, else this request will fail.
//
// aLine the line to operate on
//
TInt CCTSYIntegrationTestSuiteStepBase::RPSIgnoreNextCallL(TEtelLine aLine)
	{
    
    _LIT(KDisplayToClientTxt, "Dont answer next %S call. Press any key to continue.");
    _LIT(KRpsCallTxt, "RPS: IGNORE_NEXT_CALLL");
    
    TRpsFunctor<TEtelLine, MRpsFunctorBase::TNotUsed, MRpsFunctorBase::TNotUsed> rpsFunctor(iRPS, KDisplayToClientTxt, aLine, &CRPSMaster::IgnoreNextCallL, aLine);
   	
   	return ExecuteRemoteRequestL(rpsFunctor, KRpsCallTxt);
   	}


//
// Requests the remote party to wait for a delay before it puts the default call (call1) on hold.
// This make no assumption on whether the remote party currently has an active call.
// It is up to the user to ensure a call exists and is in a state to be put on hold.
// The user is advised to check the call state reflects it has been put on hold successfully.

// aDelay	A delay in seconds
//

TInt CCTSYIntegrationTestSuiteStepBase::RPSHoldL(TEtelLine aLine, TInt aDelay)
	{	
    
    _LIT(KDisplayToClientTxt, "Place active %S call on hold. Press any key to continue.");
    _LIT(KRpsCallTxt, "RPS: HOLDL");
    
    TRpsFunctor<TEtelLine, TInt, MRpsFunctorBase::TNotUsed> rpsFunctor(iRPS, KDisplayToClientTxt, aLine, &CRPSMaster::HoldL, aLine, aDelay);
   	
   	return ExecuteRemoteRequestL(rpsFunctor, KRpsCallTxt);
   	}

//
// Requests the remote party to wait for a delay before it puts the default call (call1) on hold.
// This make no assumption on whether the remote party currently has an active call.
// It is up to the user to ensure a call exists and is in a state to be put on hold.
// The user is advised to check the call state reflects it has been put on hold successfully.
// aDelay	A delay in seconds
//
TInt CCTSYIntegrationTestSuiteStepBase::RPSResumeL(TEtelLine aLine, TInt aDelay)
	{
    
    _LIT(KDisplayToClientTxt, "Resume the held %S call. Press any key to continue.");
    _LIT(KRpsCallTxt, "RPS: RESUMEL");
    
    TRpsFunctor<TEtelLine, TInt, MRpsFunctorBase::TNotUsed> rpsFunctor(iRPS, KDisplayToClientTxt, aLine, &CRPSMaster::ResumeL, aLine, aDelay);
   	
   	return ExecuteRemoteRequestL(rpsFunctor, KRpsCallTxt);
   	}	

//
// HANGUPL().
// Requests the remote party wait for a delay before and then hang up the default call (call1).
// This make no assumption on whether the remote party currently has an active call, or whether
// it is in a state to be hung up.
// It is up to the user to ensure a call exists and is in a state to be hung up.
//
// aLine the line to operate on
// aDelay	A delay in seconds
//
TInt CCTSYIntegrationTestSuiteStepBase::RPSHangupL(TEtelLine aLine, TInt aDelay)
	{
    
    _LIT(KDisplayToClientTxt, "Hangup the %S call. Press any key to continue.");
    _LIT(KRpsCallTxt, "RPS: HANGUPL");
    
    TRpsFunctor<TEtelLine, TInt, MRpsFunctorBase::TNotUsed> rpsFunctor(iRPS, KDisplayToClientTxt, aLine, &CRPSMaster::HangUpL, aLine, aDelay);
   	
   	return ExecuteRemoteRequestL(rpsFunctor, KRpsCallTxt);
	}
	
TInt CCTSYIntegrationTestSuiteStepBase::RPSHangUpNextCallBeforeConnectL(TEtelLine aLine)
	{

    _LIT(KDisplayToClientTxt, "Reject the next incoming %S call. Press any key to continue.");
    _LIT(KRpsCallTxt, "RPS: HANGUPNEXTCALLBEFORECONNECTL");
    
    TRpsFunctor<TEtelLine, MRpsFunctorBase::TNotUsed, MRpsFunctorBase::TNotUsed> rpsFunctor(iRPS, KDisplayToClientTxt, aLine, &CRPSMaster::HangUpNextCallBeforeConnectL, aLine);
   	
   	return ExecuteRemoteRequestL(rpsFunctor, KRpsCallTxt);
	}

TInt CCTSYIntegrationTestSuiteStepBase::RPSEchoDataL(const TDesC& aDataToSend)
	{
	 _LIT(KDisplayToClientTxt, "Echo incoming data back to device.");
     _LIT(KRpsCallTxt, "RPS: SENDDATABACKONDATAPORTL");
     
    TRpsFunctor<const TDesC&, MRpsFunctorBase::TNotUsed, MRpsFunctorBase::TNotUsed> rpsFunctor(iRPS, KDisplayToClientTxt, TEtelLine(-1), &CRPSMaster::SendDataBackOnDataPortL, aDataToSend);
   	
   	return ExecuteRemoteRequestL(rpsFunctor, KRpsCallTxt);
	}
	
//
// RPSRejectNextCallL().
// Requests the remote party to reject the next incoming call.
//
// aLine the line to operate on
//	
TInt CCTSYIntegrationTestSuiteStepBase::RPSRejectNextCallL(TEtelLine aLine)
	{
	 _LIT(KDisplayToClientTxt, "Reject the next incoming call");
     _LIT(KRpsCallTxt, "RPS: REJECTNEXTCALLL");
     
    TRpsFunctor<TEtelLine, MRpsFunctorBase::TNotUsed,MRpsFunctorBase::TNotUsed> rpsFunctor(iRPS, KDisplayToClientTxt, aLine, &CRPSMaster::RejectNextCallL, aLine);
   	
   	return ExecuteRemoteRequestL(rpsFunctor, KRpsCallTxt);
	}

TBool assert_equals_des16(CTestExecuteLogger& aLogger, const TText8* aFile, TInt aLine, TInt aSeverity,
	const TDesC& aRes, const TDesC& aExp, const TDesC& aMsg)
	{
	if(aRes != aExp)
		{
		TPtrC exp;
		TPtrC res;
		
		exp.Set(aExp.Ptr(), aExp.Length());
		res.Set(aRes.Ptr(), aRes.Length());
		
		//truncate to 20 characters, to avoid logging too big strings
		if(exp.Length() > KMaxLogCharLength)
			{
			exp.Set(exp.Ptr(), KMaxLogCharLength);
			}
		
		if(res.Length() > KMaxLogCharLength)
			{
			res.Set(res.Ptr(), KMaxLogCharLength);
			}
		
		aLogger.LogExtra(aFile, aLine, aSeverity, KAssertErrorEqualsTextDes, &res, &exp, &aMsg);
		return EFalse;
		}
	return ETrue;
	}
		

TBool assert_equals_des8(CTestExecuteLogger& aLogger, const TText8* aFile, TInt aLine, TInt aSeverity, 
	const TDesC8& aRes, const TDesC8& aExp, const TDesC& aMsg)
	{
	if(aRes != aExp)
		{
		TBuf<20> tempRes;
		TBuf<20> tempExp;
		
		TPtrC8 exp;
		TPtrC8 res;
		
		exp.Set(aExp.Ptr(), aExp.Length());
		res.Set(aRes.Ptr(), aRes.Length());
		
		if(exp.Length() > KMaxLogCharLength)
			{
			exp.Set(exp.Ptr(), KMaxLogCharLength);
			}
		
		if(res.Length() > KMaxLogCharLength)
			{
			res.Set(res.Ptr(), KMaxLogCharLength);
			}

		//8-bit copy to unicode for logging
		tempRes.Copy(res);
		tempExp.Copy(exp);
	
		aLogger.LogExtra(aFile, aLine, aSeverity, KAssertErrorEqualsTextDes, &tempRes, &tempExp, &aMsg);
		return EFalse;
		}
	return ETrue;
	}

void CCTSYIntegrationTestSuiteStepBase::AsyncExec(CThreadExec::MFunctor* aFunction)
/**
 *   Execute a function asycnhronously.
 *   @param aFunction the function to execute
 */
	{
	iEtelSessionMgr.AsyncExec(aFunction);
	}

void CCTSYIntegrationTestSuiteStepBase::SyncExec(CThreadExec::MFunctor* aFunction)
/**
 *   Execute a function sycnhronously from another thread.
 *   @param aFunction the function to execute
 */
	{
	iEtelSessionMgr.SyncExec(aFunction);
	}
