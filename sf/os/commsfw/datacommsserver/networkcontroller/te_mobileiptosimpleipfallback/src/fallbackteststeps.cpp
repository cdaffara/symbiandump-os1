// Copyright (c) 2004-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// Mobile IP to Simple IP fallback mechanism test classes implementation. 
// 
//

/**
 @file 
 @internalComponent
*/


#include "fallbackteststeps.h"
#include <c32comm.h> // call to StartC32WithCMISuppressions
#include <nifman.h>  // For Nifman progress notification codes.

// If "using namespace", Lint reports an error.
using te_mobile_ip_to_simple_ip_fallback::CFallbackStepBase;
using te_mobile_ip_to_simple_ip_fallback::CFallbackSucceedsStep;
using te_mobile_ip_to_simple_ip_fallback::CFallbackFailsStep;

using te_mobile_ip_to_simple_ip_fallback::KFallbackSucceedsStepName;
using te_mobile_ip_to_simple_ip_fallback::KFallbackFailsStepName;

namespace
	{
	// Connection attempt constants.
	const TInt KNoConnAttemptYet  = 0; 
	const TInt KFirstConnAttempt  = 1;
	const TInt KSecondConnAttempt = 2;
	
	// Progress Notification used to verify that Dummy MIP daemon has run
	// This constant must be the same as in DummyMipDaemon.
	const TInt KDummyMipDaemonProgress = KConfigDaemonStartingRegistration + 13;
	}



/**
 Logs an error and panics.
 
 If provided error code != KErrNone, logs error message and panics.
 Format of the message is:
 "ERROR: %%s,  error code= %%d", where 's' and 'd' are the message and the error code.
 
 @param aErrorCode the error code to check.
 @param aMessage   the message to log.
 */	
void CFallbackStepBase::LogAndPanicIfError(const TInt aErrorCode, const TDesC& aMessage)
	{
	if(KErrNone != aErrorCode)
		{
		INFO_PRINTF3(_L("ERROR: %s,  error code= %d"), aMessage.Ptr(), aErrorCode);
		SetTestStepResult(EFail);
		User::Panic(_L("Test Setup Error. "), aErrorCode);
		}
	}

/**
 Carries out the test TestExecute TestStep preamble.
 
 @return EFail always. 
 @post  Phonebook Synchronizer is disabled. TestStepResult is set to EFail.
 */	
TVerdict CFallbackStepBase::doTestStepPreambleL()
	{	
	_LIT(KPhbkSyncCMI, "phbsync.cmi");
	TInt err = StartC32WithCMISuppressions(KPhbkSyncCMI);
	TEST(err == KErrNone || err == KErrAlreadyExists);
	INFO_PRINTF1(_L("Test Step Preamble: disabled Phonebook Synchronizer. "));
	
	SetTestStepResult(EFail);
	return TestStepResult();
	}
	
/**
 Carries out cleanup which is not relevant to testing.
 
 @return Test verdict 
 */	
TVerdict CFallbackStepBase::doTestStepPostambleL()
	{	
	INFO_PRINTF1(_L("Postamble: closing RConnection and ESock"));
	iPppConn.Close();
	iEsock.Close();
	INFO_PRINTF1(_L("Test Completed."));

	return TestStepResult();
	}
	
	
	

/**
 C++ Constructor
 
 @post Test name is setup. 
 */	
CFallbackSucceedsStep::CFallbackSucceedsStep()
	{	
	SetTestStepName(KFallbackSucceedsStepName);	
	} 

/**
 Carries out the test.
 Executes the test sequence for each test case.
 
 
 @return Test Step verdict: EPass if fallback is successful and transparent.
 @pre    Phonebook Synchronizer is disabled.
 */
 
TVerdict CFallbackStepBase::doTestStepL()
	{
	TInt testIterations = 0;
	GetIntFromConfig(ConfigSection(), _L("TestIterations"), testIterations);
		
	
	TBool doLogStressTest = EFalse;
	GetBoolFromConfig(ConfigSection(), _L("LogStressTest"), doLogStressTest);
	
	// Run once with logging:
	TVerdict loggedRunVerdict = doTestSequenceL(ETrue, -1);
	
	// Now run "stress" tests, without logging
	INFO_PRINTF2(_L("Running stress testing: [%d] iterations"), testIterations);
	TInt stressRunFailures = 0;
	for(TInt curIteration = 0; curIteration < testIterations; curIteration++)
		{
		if(EPass != doTestSequenceL(doLogStressTest, curIteration))
			{
			++stressRunFailures;
			}
		
		if(doLogStressTest) INFO_PRINTF1(_L("============================================================"));	
		if(curIteration % 50 == 0)
			{
			INFO_PRINTF2(_L("Executed iteration[%d]."), curIteration);
			}
		
		User::After(2 * 1000000); // Give nifman a chance to reclaim memory.
								  // If this wait is missing, Nifman eventually runs out of memory and tests crash.
		}
	if(EPass == loggedRunVerdict && 
		   0 == stressRunFailures
	   )
		{
		SetTestStepResult(EPass);
		} 
	else
		{
		SetTestStepResult(EFail);
		}
	
	INFO_PRINTF3(_L("Stress run summary: test iterations= %d. Failures= %d"), testIterations, stressRunFailures);
	
	return TestStepResult();
	}
	
	
	
/**
 Carries out the test.
 Verifies that with proper CommDB configuration, when Mobile IP registartion fails, 
 the fallback to Simple IP is transparent.
 
 CommDB is configured with the "fallback" IAP to succeed, by using ConnectionPreferences table.
 
 @return Test Step verdict: EPass if fallback is transparent.
 @pre  Phonebook Synchronizer is disabled.
 */		
TVerdict CFallbackSucceedsStep::doTestSequenceL(const TBool aLog, const TInt aIteration)
	{
	iTestSequenceVerdict = EFail;	

	LogAndPanicIfError(iEsock.Connect(),      _L("RSocketServ::Connect"));
	LogAndPanicIfError(iPppConn.Open(iEsock), _L("RConnection::Open"));	
	
	TRequestStatus pppConnReq;
	iPppConn.Start(pppConnReq); // We ignore the status, because we use Progress notifications instead.
	if(aLog) INFO_PRINTF1(_L("Mobile IP over PPP Connection Requested via CommDB ConnectionPreferences Table"));
	
	TInt connAttempt = KNoConnAttemptYet;
	TBool pppIsRunning = ETrue; // The test runs until ppp is stopped by us.
	TNifProgressBuf pppProgress; // Accept Nifman progress notifications.		
	TRequestStatus  pppProgressRequest; // Progress notification request is asynchronous.
	
	TBool dummyMipDaemonHasRun = EFalse;
	
	do // process Nifman progress notifications.
		{
		iPppConn.ProgressNotification(pppProgress, pppProgressRequest); // Request progress notification.
		User::WaitForRequest(pppProgressRequest); // Wait for progress notification from Nifman
		
		if(KErrNone != pppProgress().iError)
			{
			iTestSequenceVerdict = EFail;
			INFO_PRINTF3(_L("Error Progress Notification: stage= %d error= %d"), pppProgress().iStage, pppProgress().iError);
			INFO_PRINTF2(_L("ERROR[iteration= %d]: Unexpected Error Progress Notification before second preference IAP is attempted. This means Fallback to Simple IP is not transparent."), aIteration);		
			
			break; // Error is test failure, so we just terminate. PPP stage is irrelevant.
			}
		
		TInt pppStopErr = KErrGeneral; // return of RConnection::Stop (on PPP).
										   //	We must define it here, otherwise THUMB build fails.
		switch(pppProgress().iStage) // We are interested in the stage of PPP link.
			{
			case KDummyMipDaemonProgress:
				dummyMipDaemonHasRun = ETrue;
				if(aLog) INFO_PRINTF1(_L("KConfigDaemonStartingRegistration + 13"));
				break;				
			
			case KLinkLayerOpen:	// PPP LCP and IPCP negotiation are successful.				
				if(aLog) INFO_PRINTF2(_L("KLinkLayerOpen: Simple IP PPP successful at connection attempt[%d]"),connAttempt);
				// Determine test outcome based on which connection attempt is successful, 
				if(KSecondConnAttempt == connAttempt) // We expect to succeed at the second attempt
					{
					if(dummyMipDaemonHasRun)
						{
						if(aLog) INFO_PRINTF1(_L("SUCCESS: Mobile IP to Simple IP Fallback is completed."));
						pppStopErr = iPppConn.Stop();
						if(aLog) INFO_PRINTF2(_L("Stopped PPP Nif with error= %d"),pppStopErr);
						if(KErrNone == pppStopErr)
							{						
							//---------------------------------------------------------------------------------------
							// If we get here, then PPP negotiation (LCP and IPCP) completed succesfully.
							// That is, IP address was negotiated successfully. 
							// I.E., getting KLinkLayerOpen progress is our test for proper IP address assignment. 
							// (It is assumed that PPP & Nifman function correctly.)
							//----------------------------------------------------------------------------------------
						
							iTestSequenceVerdict = EPass;
							}
						}
					else
						{
						INFO_PRINTF2(_L("ERROR[iteration= %d]: The fallback has succeeded, but the dummy MIP daemon has not run.Is CommDB configured correctly?"), aIteration);
						iTestSequenceVerdict = EFail;
						}
					}
				else if(KFirstConnAttempt == connAttempt) // Test Failure: first connection attempt succeeded.
					{
					INFO_PRINTF2(_L("ERROR[iteration= %d]: first connection attempt succeeded unespectedly. Is CommDB configured correctly? Has the daemon succeeded?"), aIteration);
					iTestSequenceVerdict = EFail;
					}
				else
					{
					INFO_PRINTF3(_L("ERROR[iteration= %d]: connection attempt %d succeeded unexpectedly. Is CommDB confirued correctly?"), connAttempt, aIteration);
					iTestSequenceVerdict = EFail;						
					}
					
				pppIsRunning = EFalse;	 // The testing is complete at this point.									
				break;
				
				
			case KFinishedSelection: 
				++connAttempt; //KFinishedSelection signals the beginning of connection attempt
				if(aLog) INFO_PRINTF2(_L("KFinishedSelection: Connection attempt [%d] is staring."), connAttempt);
				break;
				
			default: // Other irrelevant progress notifications.
				//NB: This statement should be enabled for debug purposes only.
				//	If used for testing, it slows down the test, and thus occasionally causes progress notification queue
				//	to overlow, resulting in lost notifications and unexpected test failures.
				//if(aLog) INFO_PRINTF3(_L("Other progress: stage= %d error= %d. Ignored."), pppProgress().iStage, pppProgress().iError);
				break;
			}			
		}
	while(pppIsRunning); // Until we choose to stop PPP, we ask for and receive Nifman progress notifiactions.
	iPppConn.CancelProgressNotification();
	iPppConn.Stop(); // we are executing multiple tests, so we need to do cleanup after each one.	
	
	iPppConn.Close();// Give nifman a chance to reclaim all the memory associated with the connection.
	iEsock.Close();
	
	
	return iTestSequenceVerdict;
	}




/**
 C++ Constructor
 
 @post Test name is setup. 
 */	 
CFallbackFailsStep::CFallbackFailsStep()
	{	
	SetTestStepName(KFallbackFailsStepName);	
	}

	
/**
 Carries out the test.
 Verifies that if fallback to Simple IP fails, the failure is graceful.
 
 CommDB is configured with the "fallback" IAP to fail, by using invalid PPP
 authentication name and password in CDMA2000PacketService table.
 
 @return Test Step verdict: EPass if fallback fails gracefully.
 @pre  Phonebook Synchronizer is disabled.
 */	
TVerdict CFallbackFailsStep::doTestSequenceL(const TBool aLog, const TInt aIteration)
	{
	iTestSequenceVerdict = EFail;
	
	
	LogAndPanicIfError(iEsock.Connect(),      _L("RSocketServ::Connect"));
	LogAndPanicIfError(iPppConn.Open(iEsock), _L("RConnection::Open"));	
	
	
	TRequestStatus pppConnReq;
	iPppConn.Start(pppConnReq); // We ignore the status, because we use Progress notifications instead.
	if(aLog) INFO_PRINTF1(_L("Mobile IP over PPP Connection Requested via CommDB ConnectionPreferences Table"));
	
	TInt connAttempt = KNoConnAttemptYet; 
	TBool pppIsRunning = ETrue; 	// The test runs until ppp is stopped by us.
	TNifProgressBuf pppProgress;	// Accept Nifman progress notifications.			
	TRequestStatus  pppProgressRequest; // Progress request notification is anynchronous.
	
	TBool dummyMipDaemonHasRun = EFalse;
	
	do // accept Nifman progress notifications.
		{
		iPppConn.ProgressNotification(pppProgress, pppProgressRequest);
		User::WaitForRequest(pppProgressRequest); // Wait for progress notification from Nifman
		
		if(KErrNone == pppProgress().iError)  // Success progress notification. 
			{
			switch(pppProgress().iStage) // We are interested in the stage of PPP link.
				{
				case KFinishedSelection: 
				++connAttempt; //KStartung selection signals the beginning of connection attempt
				if(aLog) INFO_PRINTF2(_L("KFinishedSelection. Connection attempt [%d] is starting"), connAttempt);
				break;
				
				case KDummyMipDaemonProgress:
					dummyMipDaemonHasRun = ETrue;
					if(aLog) INFO_PRINTF1(_L("KConfigDaemonStartingRegistration + 13"));
					break;
				
				case KLinkLayerOpen:	// We do not expect Nif to go up. If we do get here, the test fails.										
					INFO_PRINTF1(_L("KLinkLayerOpen"));								
					INFO_PRINTF2(_L("ERROR[iteration= %d]: Link Layer opened unexpectedly. Are CommDB ModemBearer table and Wintunnel configured correctly for failure?"), aIteration);
					iTestSequenceVerdict = EFail;
					iPppConn.Stop(); // We don't care if this operation is successful.
					pppIsRunning = EFalse;
					break;						
				
				case KLinkLayerClosed: // We get this at the end of the second connection attempt.
					ASSERT(KFirstConnAttempt == connAttempt || KSecondConnAttempt == connAttempt);
					if(aLog) INFO_PRINTF2(_L("KLinkLayerClosed. Connection attempt [%d] failed."), connAttempt);
					break;

				default: // Other irrelevant progress notifications.
					//NB: This statement should be enabled for debug purposes only.
					//If used for testing, it slows down the test, and thus occasionally causes progress notification queue
					//to overlow, resulting in lost notifications and unexpected test failures. 
					//if(aLog) INFO_PRINTF3(_L("Other progress: stage= %d error= %d. Ignored."), pppProgress().iStage, pppProgress().iError);
					
					break;				
				}			
			}
		else // Error progress notification. The connection attempt this happens at determines success of failure of the test.
			{
			if(aLog) INFO_PRINTF3(_L("Error Progress Notification: stage= %d error= %d"), pppProgress().iStage, pppProgress().iError);
			
			ASSERT(KFirstConnAttempt == connAttempt || KSecondConnAttempt == connAttempt);
			
			if(KFirstConnAttempt == connAttempt) // Error progress notification at first attempt == Test failure.
				{							  // Because we have an error even before attempting fallback.
				iTestSequenceVerdict = EFail;				
				INFO_PRINTF2(_L("ERROR[iteration= %d]: Unexpected Error Progress Notification before second preference IAP is attempted. This means Fallback to Simple IP is not transparent."), aIteration);
				}
			else  // Error progress notification, but at second attempt. This is expected and is OK.
				{
				if(KErrCouldNotConnect == pppProgress().iError)
					{				
					if(dummyMipDaemonHasRun)
						{
						if(aLog) INFO_PRINTF1(_L("SUCCESS: Fallback to Simple IP failed gracefully."));			
						iTestSequenceVerdict = EPass;
						}
					else
						{
						INFO_PRINTF1(_L("ERROR: the dummy MIP Daemon has not run. The test sequence has not executed properly. Check CommDB."));				
						iTestSequenceVerdict = EFail;	
						}
					}
				else
					{						
					iTestSequenceVerdict = EFail;
					INFO_PRINTF2(_L("ERROR: unexpected error Progress Notification KErrCouldNotConnect[-34], received error [%d]. Test sequence did not execute as expected. Did Nifman progress notification mechanism change?"),pppProgress().iError);				
					}
				//--------------------------------------------------------------------------------------
				// We do not try to close the PPP link because it was never open in the first place.
				// The progress error is most likely KErrServerTerminated(-15)
				// Calling RConnection::Stop() usually returns KErrNotReady(-18).
				// These error codes are not tested, because Nifman progress notification mechanism
				// and thus error codes may change.
				//--------------------------------------------------------------------------------------
				}			
			
			pppIsRunning = EFalse; // Once we have an error notification, we stop regardless of the stage. 			
			}			
		}
	while(pppIsRunning);	// Until we choose to stop PPP, we ask for and receive Nifman progress notifications.		
	
	iPppConn.CancelProgressNotification();
	iPppConn.Stop();
	iPppConn.Close();// Give nifman a chance to reclaim all the memory associated with the connection.
	iEsock.Close();
	
	return iTestSequenceVerdict;
	}







	


