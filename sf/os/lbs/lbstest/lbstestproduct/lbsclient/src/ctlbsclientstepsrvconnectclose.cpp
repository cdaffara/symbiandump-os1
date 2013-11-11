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
// @file ctlbsclientstep_srvconnectclose.cpp
// This is the class implementation for the Server Connect Close Tests
// 
//
 
#include "ctlbsclientstepsrvconnectclose.h"

#include <lbs.h>

/**
 * Destructor
 */
CT_LbsClientStep_SrvConnectClose::~CT_LbsClientStep_SrvConnectClose()
	{
	}


/**
 * Constructor
 */
CT_LbsClientStep_SrvConnectClose::CT_LbsClientStep_SrvConnectClose(CT_LbsClientServer& aParent) : CT_LbsClientStep(aParent)
	{
	SetTestStepName(KLbsClientStep_SrvConnectClose);
	}


/**
Static Constructor
*/
CT_LbsClientStep_SrvConnectClose* CT_LbsClientStep_SrvConnectClose::New(CT_LbsClientServer& aParent)
	{
	return new CT_LbsClientStep_SrvConnectClose(aParent);
	// Note the lack of ELeave.
	// This means that having insufficient memory will return NULL;
	}



/**
 * @return - TVerdict code
 * Override of base class pure virtual
 * Our implementation only gets called if the base class doTestStepPreambleL() did
 * not leave. That being the case, the current test result value will be EPass.
 */
TVerdict CT_LbsClientStep_SrvConnectClose::doTestStepL()
	{
	// Generic test step used to test the LBS Client server open and close APIs.
	INFO_PRINTF1(_L("&gt;&gt;CT_LbsClientStep_SrvConnectClose::doTestStepL()"));

	if (TestStepResult() == EPass)
		{
		TInt testCaseId;
		if (GetIntFromConfig(ConfigSection(), KTestCaseId, testCaseId))
			{		
			// Carryout common test action.
			TInt err = KErrNone;
			err = iServer.Connect();	// Function under test.
			
			if (KErrNone == err)
				{
				INFO_PRINTF1(_L("server opened"));
				CleanupClosePushL(iServer);			
				// Carryout unique test actions.
						switch (testCaseId)
						{
							// Test case LBS-ConnectClose-0001
							case 1:
								// Only requires create, connect and close for RPositionServer object
								// Connect and close occur outside the switch.
								break;
						
							// Test case LBS-ConnectClose-0002
							case 2:
								{
								OpenNetSim();
								RPositioner positioner;
								User::LeaveIfError(positioner.Open(iServer));
								TPositionInfo posInfo;
								TRequestStatus status;
								
								TPositionUpdateOptions optsA;
								TTimeIntervalMicroSeconds interval = 60 * 1000000;	// 1 minute interval
								// note: do 2 NPUDs because the first may complete before we close
								optsA.SetUpdateInterval(interval);
								positioner.NotifyPositionUpdate(posInfo, status);
								User::WaitForRequest(status);
								
								// this will not complete quickly, so we'll def have an outstanding request when we close
								positioner.NotifyPositionUpdate(posInfo, status);
								positioner.Close();
								CloseNetSim();
								}
								break;
						
							// Test case LBS-ConnectClose-0003
							case 3:
								{
								// Will panic, not interested in any return err.
								iServer.Connect();
								}
								break;
								
							// Test case LBS-ConnectClose-0004
							case 4:
								{
								RPositionServer server2;
	
								err = server2.Connect();	
								if(err != KErrNone)
									{
									INFO_PRINTF1(_L("<FONT><B>Server2 connect() didn't return KErrNone</B></FONT>"));
									SetTestStepResult(EFail);
									}
							
								else
									{
									server2.Close();
									}
								}
								break;
							//Test case LBS-ConnectClose-0005
							case 5:
								{
								// look in the kernels process list for the server
								_LIT(KLocServerName, "*locserver*"); // NB you MUST use this server name for the location server. 
															   // see KPositionServerName in the location server code (server side)
								TFindThread serverSearcher(KLocServerName);
								TFileName matchingFileName;
								TInt matchCount = 0;
								while(serverSearcher.Next(matchingFileName) == KErrNone)
								  {
								   // we have a match, there should only be one
								   matchCount++;
								  }
								// start checking
								if(matchCount!=1)
								  {
								   /* fail the test - we somehow have more than one location server running. 
									  May have been one left over from a previous test */	                    
								   INFO_PRINTF1(_L("somehow have more than one location server running"));
								   SetTestStepResult(EFail); 
								  }
								RThread iserverThread;
								TInt err;
								err=iserverThread.Open(matchingFileName);
								User::LeaveIfError(err);
								TThreadId iserverThreadId = iserverThread.Id();
								// log the thread ID
								// we now try and explicitly start a 2nd copy of the position server. 
								//We expect not to get a 2nd copy, but to //connect to the existing copy. 
								// create an connect to the lcoation server
								RPositionServer server2;
								User::LeaveIfError(server2.Connect());
								CleanupClosePushL(server2);
								// search again
								serverSearcher.Find(KLocServerName);
								matchCount = 0;
								while(serverSearcher.Next(matchingFileName) == KErrNone)
									{
									// we have a match, there should only be one
									 matchCount++;
									}
								// sanity check
							   if(matchCount!=1)
								{
								 /*SDA 3_50 Failure here
								  fail the test - we have two servers.*/
								 INFO_PRINTF1(_L("we have two servers"));
								 SetTestStepResult(EFail); 
								 
								}
							  else
								{
								// we have one server so check the thread its match
								RThread server2Thread;
								err=server2Thread.Open(matchingFileName);
								User::LeaveIfError(err);
								TThreadId server2ThreadId = server2Thread.Id();
							 
								if(iserverThreadId != server2ThreadId)
								  {
								   /*different thread ids - somehow
									 fail the test here */
								   INFO_PRINTF1(_L("Different thread ids "));
								   SetTestStepResult(EFail); 
								  }
								}
								CleanupStack::PopAndDestroy(&server2);
							   }
							   break;
							//Test case LBS-ConnectClose-0010
							case 10:
								{
								// this test only valid on variant2 api:
								__ASSERT_DEBUG(iExpectedApiBehaviour == EApiVariant2, User::Panic(_L("Unrecognised 'Expected Api Behaviour'"), 1));
								INFO_PRINTF1(_L("<FONT><B>LBsroot has been started by EPosserver in this mode. </B></FONT>"));
								}
							break;
						default:
						User::Leave(KErrArgument);
						break;
							
						}
				// Carryout final common test action.			
				CleanupStack::PopAndDestroy(&iServer);					
				}
			else if(testCaseId == 10)
				{
				// this test only valid on variant2 api:
				__ASSERT_DEBUG(iExpectedApiBehaviour == EApiVariant2, User::Panic(_L("Unrecognised 'Expected Api Behaviour'"), 1));
				if(err != KErrNotReady)
					{
					INFO_PRINTF2(_L("<FONT><B>Didn't get KErrNotReady as expected. Err = %d</B></FONT>"), err);
					SetTestStepResult(EFail);
					}
				}
			else
				{
				INFO_PRINTF1(_L("<FONT><B>server failed to open</B></FONT>"));
				SetTestStepResult(EFail);
				}
			}
		else
			{				
			INFO_PRINTF1(_L("<FONT><B>Failed to read from ini file correctly</B></FONT>"));
			SetTestStepResult(EFail);
			}
		}
	INFO_PRINTF1(_L("&lt;&lt;CT_LbsClientStep_SrvConnectClose::doTestStepL()"));

	return TestStepResult();
	}



