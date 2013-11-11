// Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// Example file/test code to demonstrate how to develop a TestExecute Server
// Developers should take this project as a template and substitute their own
// for (WINS && !EKA2) versions will be xxxServer.Dll and require a thread to be started
// in the process of the client. The client initialises the server by calling the
// one and only ordinal.
// 
//

/**
 @file Te_manSuiteServer.cpp
*/

#include "lbsagpsngmsgs.h"
#include "Te_manSuiteServer.h"
#include "man0Step.h"
#include "man1Step.h"
#include "man2Step.h"
#include "man3Step.h"
#include "man4Step.h"
#include "man6Step.h"
#include "man7Step.h"
#include "man8Step.h"
#include "man9Step.h"
#include "man10Step.h"
#include "man11Step.h"
#include "man12step.h"
#include "man13step.h"
#include "man14step.h"
#include "man20step.h"

TPtrC NgMsgNames[]={
	_L("EUnknown"),
	_L("ELocationRequest"),
	_L("ELocationResponse"),
	_L("ETransmitLocationRequest"),
	_L("ETransmitLocationCancel"),
	_L("EPrivacyRequest"),
	_L("EPrivacyResponse"),
	_L("ESessionComplete"),
	_L("ESelfLocationRequest"),
	_L("ESelfLocationCancel"),
	_L("EAssistanceDataRequest"),
	_L("EAssistanceDataResponse"),
	_L("ESystemStatusAdvice"),
	_L("Unrecognised")};

//-----------------------------------------------------------------------------------------
NGDataBufferClass::NGDataBufferClass() :
	iReceivedMsg(reinterpret_cast<TLbsNetInternalMsgBase&>(iReceivedMsgBuffer))
	{
	}
	
void NGDataBufferClass::ProcessNetChannelMessage(RLbsNetChannel::TLbsNetChannelId aChannelId, const TLbsNetInternalMsgBase& aMessage)
	{
	Mem::Copy(&iReceivedMsgBuffer[0], &aMessage, aMessage.Size());
	iReceivedChannelId = aChannelId;
	RThread().Rendezvous(KErrNone);
	RThread().Suspend();
	return;
	}
	
TInt NGDataBufferClass::SendNgMsg(const TLbsNetInternalMsgBase& aMsg)
	{
	TRequestStatus status;
	iNGInt.SendMessage(aMsg, status);
	User::WaitForRequest(status);
	return status.Int();
	}

NGDataBufferClass	ngDataBuffer;

//-----------------------------------------------------------------------------------------
void MonitorThreadFuncStartL()
	{
	ngDataBuffer.iNGInt.OpenL(RLbsNetChannel::EChannelNG2AGPS, ngDataBuffer);
	CActiveScheduler::Start();
	}

TInt MonitorThreadFunc(TAny*)
	{
	CActiveScheduler* rootScheduler = new CActiveScheduler;
	CTrapCleanup* theCleanup=CTrapCleanup::New();
	CActiveScheduler::Install(rootScheduler);
	
	TRAPD(err, MonitorThreadFuncStartL());
	
	ngDataBuffer.iNGInt.Close();
	delete theCleanup;
	delete rootScheduler;
	return err;
	}

_LIT(KServerName,"Te_manSuite");
CTe_manSuite* CTe_manSuite::NewL()
/**
 * @return - Instance of the test server
 * Same code for Secure and non-secure variants
 * Called inside the MainL() function to create and start the
 * CTestServer derived server.
 */
	{
	CTe_manSuite * server = new (ELeave) CTe_manSuite();
	CleanupStack::PushL(server);

	server->ConstructL(KServerName);
	CleanupStack::Pop(server);
	return server;
	}

void CTe_manSuite::ConstructL(const TDesC& aName)
	{
	CTestServer::ConstructL(aName);
	}

void CTe_manSuite::SetupNGInterface()
	{
	TInt ret = ngDataBuffer.iThread.Create(_L("NG monitor"), MonitorThreadFunc, 8000, NULL, this);
	ngDataBuffer.iThread.Resume();
	}

// Secure variants much simpler
// For EKA2, just an E32Main and a MainL()
LOCAL_C void MainL()
/**
 * Secure variant
 * Much simpler, uses the new Rendezvous() call to sync with the client
 */
	{
	// Leave the hooks in for platform security
#if (defined __DATA_CAGING__)
	RProcess().DataCaging(RProcess::EDataCagingOn);
	RProcess().DataCaging(RProcess::ESecureApiOn);
#endif
	CActiveScheduler* sched=NULL;
	sched=new(ELeave) CActiveScheduler;
	CActiveScheduler::Install(sched);
	CTe_manSuite* server = NULL;
	// Create the CTestServer derived server
	TRAPD(err,server = CTe_manSuite::NewL());
	if(!err)
		{
		// Sync with the client and enter the active scheduler
		RProcess::Rendezvous(KErrNone);
		sched->Start();
		}
	delete server;
	delete sched;
	}



GLDEF_C TInt E32Main()
/**
 * @return - Standard Epoc error code on process exit
 * Secure variant only
 * Process entry point. Called by client using RProcess API
 */
	{
	__UHEAP_MARK;
	CTrapCleanup* cleanup = CTrapCleanup::New();
	if(cleanup == NULL)
		{
		return KErrNoMemory;
		}
	TRAPD(err,MainL());
	delete cleanup;
	__UHEAP_MARKEND;
	return err;
    }


CTestStep* CTe_manSuite::CreateTestStep(const TDesC& aStepName)
/**
 * @return - A CTestStep derived instance
 * Secure and non-secure variants
 * Implementation of CTestServer pure virtual
 */
	{
	CTestStep* testStep = NULL;
              if(aStepName == Kman0Step)
                            testStep = new Cman0Step();
              else if(aStepName == Kman1Step)
                            testStep = new Cman1Step();
              else if(aStepName == Kman2Step)
                            testStep = new Cman2Step();
              else if(aStepName == Kman3Step)
                            testStep = new Cman3Step();
              else if(aStepName == Kman4Step)
                            testStep = new Cman4Step();
              else if(aStepName == Kman6Step)
                            testStep = new Cman6Step();
              else if(aStepName == Kman7Step)
                            testStep = new Cman7Step();
              else if(aStepName == Kman8Step)
                            testStep = new Cman8Step();
              else if(aStepName == Kman9Step)
                            testStep = new Cman9Step();
              else if(aStepName == Kman10Step)
                            testStep = new Cman10Step();
              else if(aStepName == Kman11Step)
                            testStep = new Cman11Step();
              else if(aStepName == Kman12Step)
                            testStep = new Cman12Step();
              else if(aStepName == Kman13Step)
                            testStep = new Cman13Step();
			  else if(aStepName == Kman14Step)
                            testStep = new Cman14Step();
              else if(aStepName == Kman20Step)
              				testStep = new Cman20Step();
    iCurrentStep = testStep;
    CTe_manSuiteStepBase* setup = reinterpret_cast<CTe_manSuiteStepBase*>(testStep);
    setup->SetNGInterface(this);
	return testStep;
	}

//NG Functions

void CTe_manSuite::WaitForNgMsg()
	{
	TRequestStatus statusNg;
	
	ngDataBuffer.iThread.Resume();
	ngDataBuffer.iThread.Rendezvous(statusNg);
	User::WaitForRequest(statusNg);
	}
	
/**
@param aMsgType Expected message type
@param aFlag Expected NewClient or Tracking flag (dependent on aMsgType)

*/
void CTe_manSuite::WaitAndValidateNgMsg(TLbsNetInternalMsgBase::TLbsNetInternalMsgType aMsgType, TBool aFlag)
	{
	WaitForNgMsg();
	iCurrentStep->INFO_PRINTF1(_L("   Received message from the AGPS Manager over the RLbsNetChannel"));
			
	// validate channel
	if (ngDataBuffer.iReceivedChannelId != RLbsNetChannel::EChannelNG2AGPS)
		{
		iCurrentStep->SetTestStepResult(EFail);
		iCurrentStep->ERR_PRINTF2(_L("   <font color=FF0000><b>Incorrect Channel Id: %d</b></font>"), ngDataBuffer.iReceivedChannelId);
		}

	// Validate Msg Type
	INFO_PRINTF2(_L("      Exp msg type: %S"), NgMsgTypeToName(aMsgType));
	INFO_PRINTF2(_L("      Rcv msg type: %S"), NgMsgTypeToName(ngDataBuffer.iReceivedMsg.Type()));
	if (ngDataBuffer.iReceivedMsg.Type() != aMsgType)
		{
		iCurrentStep->SetTestStepResult(EFail);
		iCurrentStep->ERR_PRINTF1(_L("   <font color=FF0000><b>Message type does not match.</b></font>"));
		}
	else
		{
		TInt err = KErrNone;
		// Validate flags
		if (ngDataBuffer.iReceivedMsg.Type() == TLbsNetInternalMsgBase::ESystemStatusAdvice)
			{
			TLbsNetSystemStatusAdviceMsg& statMsg = reinterpret_cast<TLbsNetSystemStatusAdviceMsg&>
												   (ngDataBuffer.iReceivedMsg);
															   
												   iCurrentStep->INFO_PRINTF2(_L("      Exp Tracking flag: %d"), aFlag);
			iCurrentStep->INFO_PRINTF2(_L("      Rcv Tracking flag: %d"), statMsg.Tracking());
			err = aFlag == statMsg.Tracking() ? KErrNone : KErrGeneral;
			}
		else if (ngDataBuffer.iReceivedMsg.Type() == TLbsNetInternalMsgBase::ESelfLocationRequest)
			{
			TLbsNetSelfLocationRequestMsg& reqMsg = reinterpret_cast<TLbsNetSelfLocationRequestMsg&>
												   (ngDataBuffer.iReceivedMsg);
			// Store the session Id so later we can respond with the same one
			iSessionId = reqMsg.SessionId();

			iCurrentStep->INFO_PRINTF2(_L("      Exp NewClient flag: %d"), aFlag);
			iCurrentStep->INFO_PRINTF2(_L("      Rcv NewClient flag: %d"), reqMsg.Options().NewClientConnected());
			err = aFlag == reqMsg.Options().NewClientConnected() ? KErrNone : KErrGeneral;
			}
			
		if (err != KErrNone)
			{
			iCurrentStep->SetTestStepResult(EFail);
			iCurrentStep->ERR_PRINTF1(_L("   <font color=FF0000><b>Flags do not match.</b></font>"));
			}
		}
	}
	
void CTe_manSuite::SendNgMsg(const TLbsNetInternalMsgBase& aMsg)
	{
	TInt err;

	if (aMsg.Type() == TLbsNetInternalMsgBase::ESessionComplete)
		{
		// Replace the session Id with the on from the most recent SelfLocationRequest
		const TLbsNetSessionCompleteAgpsMsg& reqMsg = reinterpret_cast<const TLbsNetSessionCompleteAgpsMsg&>(aMsg);
		TInt xxx = reqMsg.Reason();
		TLbsNetSessionCompleteAgpsMsg newMsg(iSessionId, reqMsg.Reason());
		err = ngDataBuffer.SendNgMsg(newMsg);
		}
	else
		{
		err = ngDataBuffer.SendNgMsg(aMsg);
		}
		
	if (err != KErrNone)
		{
		iCurrentStep->SetTestStepResult(EFail);
		iCurrentStep->ERR_PRINTF2(_L("   <font color=FF0000><b>Sending NG message failed. Error: %d.</b></font>"), err);
		}
	}

//-----------------------------------------------------------------------------------------
TPtrC* CTe_manSuite::NgMsgTypeToName(TLbsNetInternalMsgBase::TLbsNetInternalMsgType aMsgType)
	{
	if (aMsgType <= TLbsNetInternalMsgBase::ESystemStatusAdvice)
		{
		return &NgMsgNames[aMsgType];
		}
	else
		{
		iCurrentStep->SetTestStepResult(EFail);
		iCurrentStep->ERR_PRINTF2(_L("   <font color=FF0000><b>Unrecognized msg type: %d</b></font>"), ngDataBuffer.iReceivedMsg.Type());
		return &NgMsgNames[TLbsNetInternalMsgBase::ESystemStatusAdvice + 1];
		}
	}
