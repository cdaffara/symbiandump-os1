/*
* Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description: 
* Example CTestStep derived implementation
*
*/


/**
 @file ServerTestStep.cpp
 @internalTechnology
*/
#include "serverteststep.h"
//To launch the test version of the location server that uses the test location monitor API
#include "tserverstartparams.h" // for TServerStartParams
#include "cserverlaunch.h"



CServerTestStep::~CServerTestStep()
/**
 * Destructor
 */
	{
	}

CServerTestStep::CServerTestStep()
/**
 * Constructor
 */
	{
	SetTestStepName(KServerTestStep);
	}

TInt CServerTestStep::StartEposServer()
	{

	// Start the unit-test version of the Location Server that uses
	// the test locaiton monitor API.
	// Note: The server name still actually needs to be the same
	//       as the 'real' Location server, otherwise clients will not
	//       be able to connect it (and they will then start the 
	//		 'real' one, which messes up this test).
	
	_LIT(KPositionServerName, "!PosServer.exe"); 
	_LIT(KTestLocServerFileName, "testlocserver.exe"); 
	
	TServerStartParams params;
	
	params.SetServerFileName(KTestLocServerFileName);
	params.SetServerName(KPositionServerName);
	params.SetNumberOfServerSlots(KServerFrameworkDefaultMessageSlots);
	params.SetServerUids(KNullUid, KNullUid, TUid::Uid(0x102869E5));

	TRAPD(err,User::LeaveIfError(CServerLaunch::ServerLaunch(params)));
	
	return err;
	}


TVerdict CServerTestStep::doTestStepPreambleL()
/**
 * @return - TVerdict code
 * Override of base class virtual
 */
	{
	
	// The Property is used the modify the behaviour of the testLocMonitorApi for different
	// test cases
	TInt err;
	static _LIT_SECURITY_POLICY_PASS(KAllowAllPolicy);
	err = RProperty::Define(KLocSrvTestSuite, KLbsLocMonitorTestSessionKey,
			RProperty::EInt, KAllowAllPolicy, KAllowAllPolicy);
	if (err != KErrNone && err != KErrAlreadyExists)
		{
		User::Leave(err);
		}

	
	CTe_locsrvSuiteStepBase::doTestStepPreambleL();
	SetTestStepResult(EPass);
	return TestStepResult();
	}


TVerdict CServerTestStep::doTestStepL()
/**
 * @return - TVerdict code
 * Override of base class pure virtual
 * Our implementation only gets called if the base class doTestStepPreambleL() did
 * not leave. That being the case, the current test result value will be EPass.
 */
	{
	if (TestStepResult()==EPass)
		{
		TInt testIndex = KErrNotFound;
		if(!GetIntFromConfig(ConfigSection(),KTe_locsrvSuiteInt,testIndex))
			{
			User::Leave(KErrNotFound);
			}
		else
			{
			// Check if the test location server is running
			_LIT(KPositionServerName, "!PosServer"); 
			TFindServer findServer(KPositionServerName);
			TInt err;
			TFullName name;
			err = findServer.Next(name);
			if (err != KErrNone)
				{
				// If it is not running, then start the test version of the 
				// location server
				err = StartEposServer();
				}

			// If the test location server has been started successfully or is already 
			// running then run the teststep.
			if ((err==KErrNone)||(err==KErrAlreadyExists))
				{
				TestByIndexL(testIndex);
				}
			else
				{
				SetTestStepResult(EFail);
				}
			}
		}
	    return TestStepResult();
	    }

void CServerTestStep::TestByIndexL(TInt aIndex)
    {
    switch (aIndex)
        {
         case 1:
        	_LIT(KTestName1, "Close Session with outstanding Clear database request");
        	INFO_PRINTF1(KTestName1);
        	TdLocSrv_Server_ClearDBPending_CloseSessionHandleL(aIndex);
        	break;
       
        case 2:
        	_LIT(KTestName2, "Clear database - LocMonitor SessionOpen Fail");
        	INFO_PRINTF1(KTestName2);
        	TdLocSrv_Server_ClearDB_LocMonitor_SessionOpenFailL(aIndex);
        	break;
        
        case 3:
        	_LIT(KTestName3, "Two Clear database requests");
        	INFO_PRINTF1(KTestName3);
        	TdLocSrv_Server_ClearDB_2ClientsL(aIndex);
        	break;
      
        case 4:
        	_LIT(KTestName4, "Clear database followed by cancel");
        	INFO_PRINTF1(KTestName4);
        	TdLocSrv_Server_ClearDB_AndCancelL(aIndex);
        	break;
        	
        case 5:
        	_LIT(KTestName5, "Two Clear database requests followed by cancel from one client ");
        	INFO_PRINTF1(KTestName5);
        	TdLocSrv_Server_ClearDB_AndCancel_2ClientsL(aIndex);
        	break;
        	
        case 6:
        	_LIT(KTestName6, "Clear database request times out ");
        	INFO_PRINTF1(KTestName6);
        	TdLocSrv_Server_ClearDB_TimedOutL(aIndex);
        	break;
        
        case 7:
        	_LIT(KTestName7, "Clear database request 10Clients ");
        	INFO_PRINTF1(KTestName7);
        	TdLocSrv_Server_ClearDB_10ClientsL(aIndex);
        	break;
        	
        default: 
            _LIT(KTestName15, "This test step doesn't exist");
            ERR_PRINTF1(KTestName15);
            SetTestStepResult(EFail);
            break;
        }
    }

TVerdict CServerTestStep::doTestStepPostambleL()
/**
 * @return - TVerdict code
 * Override of base class virtual
 */
	{
	
	// Delete the keys property used to configure the test location monitor
	// to respond to different test cases
	RProperty property;
	TInt err = property.Delete(KLocSrvTestSuite,KLbsLocMonitorTestSessionKey);
	if (err != KErrNone)
		{
		User::Leave(err);
		}
	
	return TestStepResult();
	
	}

extern TInt SetAuthenticRequestor(RPositioner& aPositioner);

///////////////////////////////////////////////////
// 
// RPositionServer
//
///////////////////////////////////////////////////


void SetServerTestCasePropertyL(const TUid TestServerUid, const TUint KeyType, TInt aIndex)
	{
	// The Property is used the modify the behaviour of the testLocMonitorApi for different
	// test cases
	TInt err = RProperty::Set(TestServerUid, KeyType, aIndex);
	if (err != KErrNone)
		{
		User::Leave(err);
		}
	}

TInt CloseSessionWithClearDBReqsL(TAny* /*aPtr*/)
	{
	RPositionServer server;
	User::LeaveIfError(server.Connect());
	CleanupClosePushL(server);

	TRequestStatus reqStatus;
	server.EmptyLastKnownPositionStore(reqStatus);
	User::After(KClearDBReqPropagateDelay);
	
	
    TBool otherReqPending = (reqStatus.Int()==KRequestPending);
    _LIT(KIncorrectTestSetup,"Test timing fault");
    if (!otherReqPending)
    	{
    	// Increase the delay in the test location monitor as we require the above 
    	// requests to be outstanding when an emptylastknownpositionstore request 
    	// is issued [the panic occurs when the above requests have been completed before
    	// an emptylastknownpositionstore request ]
    	User::Panic(KIncorrectTestSetup, KErrGeneral);
    	}
    
	server.Close(); // Has to PANIC as there is an outstanding request
	User::After(KHalfSecond);
	CleanupStack::Pop(&server);
	return KErrGeneral;
	}

TVerdict CServerTestStep::TdLocSrv_Server_ClearDBPending_CloseSessionHandleL(TInt aIndex)
	{
	SetServerTestCasePropertyL(KLocSrvTestSuite, KLbsLocMonitorTestSessionKey, aIndex);
	
    DO_PANIC_TEST_L(CloseSessionWithClearDBReqsL, KPosClientFault, 
    		EPositionRequestsNotCancelled, KDefaultTestTimeout);
    
    return TestStepResult();
	}
	
TInt ClearDBLocMonitorSessionFailL(TAny* /*aPtr*/)
	{

	// Check the correct message is always sent to the server upon a call
	// to this function and that the return value is always that of the
	// completed message.

	RPositionServer server;
	User::LeaveIfError(server.Connect());
	CleanupClosePushL(server);

	TRequestStatus reqStatus;

	server.EmptyLastKnownPositionStore(reqStatus);
	User::WaitForRequest(reqStatus);
	
//	server.Close();
	CleanupStack::PopAndDestroy(&server);
	return KErrGeneral;
	}

TVerdict CServerTestStep::TdLocSrv_Server_ClearDB_10ClientsL(TInt aIndex)
	{
	SetServerTestCasePropertyL(KLocSrvTestSuite, KLbsLocMonitorTestSessionKey, aIndex);

	RPositionServer server1;
	RPositionServer server2;
	RPositionServer server3;
	RPositionServer server4;
	RPositionServer server5;
	RPositionServer server6;
	RPositionServer server7;
	RPositionServer server8;
	RPositionServer server9;
	RPositionServer server10;
	
	User::LeaveIfError(server1.Connect());
	CleanupClosePushL(server1);
	User::LeaveIfError(server2.Connect());
	CleanupClosePushL(server2);
	User::LeaveIfError(server3.Connect());
	CleanupClosePushL(server3);
	User::LeaveIfError(server4.Connect());
	CleanupClosePushL(server4);
	User::LeaveIfError(server5.Connect());
	CleanupClosePushL(server5);
	User::LeaveIfError(server6.Connect());
	CleanupClosePushL(server6);
	User::LeaveIfError(server7.Connect());
	CleanupClosePushL(server7);
	User::LeaveIfError(server8.Connect());
	CleanupClosePushL(server8);
	User::LeaveIfError(server9.Connect());
	CleanupClosePushL(server9);
	User::LeaveIfError(server10.Connect());
	CleanupClosePushL(server10);

	
	TRequestStatus reqStatus1,reqStatus2,reqStatus3,reqStatus4,reqStatus5,reqStatus6,reqStatus7,
						reqStatus8,reqStatus9,reqStatus10;
	server1.EmptyLastKnownPositionStore(reqStatus1);
	User::After(KClearDBMultipleReqDelay);
	server2.EmptyLastKnownPositionStore(reqStatus2);
	User::After(KClearDBMultipleReqDelay);
	server3.EmptyLastKnownPositionStore(reqStatus3);
	User::After(KClearDBMultipleReqDelay);
	server4.EmptyLastKnownPositionStore(reqStatus4);
	User::After(KClearDBMultipleReqDelay);
	server5.EmptyLastKnownPositionStore(reqStatus5);
	User::After(KClearDBMultipleReqDelay);
	server6.EmptyLastKnownPositionStore(reqStatus6);
	User::After(KClearDBMultipleReqDelay);
	server7.EmptyLastKnownPositionStore(reqStatus7);
	User::After(KClearDBMultipleReqDelay);
	server8.EmptyLastKnownPositionStore(reqStatus8);
	User::After(KClearDBMultipleReqDelay);
	server9.EmptyLastKnownPositionStore(reqStatus9);
	User::After(KClearDBMultipleReqDelay);
	server10.EmptyLastKnownPositionStore(reqStatus10);
	User::After(KClearDBMultipleReqDelay);

	User::WaitForRequest(reqStatus1);
	User::WaitForRequest(reqStatus2);
	User::WaitForRequest(reqStatus3);
	User::WaitForRequest(reqStatus4);
	User::WaitForRequest(reqStatus5);
	User::WaitForRequest(reqStatus6);
	User::WaitForRequest(reqStatus7);
	User::WaitForRequest(reqStatus8);
	User::WaitForRequest(reqStatus9);
	User::WaitForRequest(reqStatus10);

	CheckExpectedResult(KErrNone, reqStatus1.Int(), KFailedClearDB);
	CheckExpectedResult(KErrNone, reqStatus2.Int(), KFailedClearDB);
	CheckExpectedResult(KErrNone, reqStatus3.Int(), KFailedClearDB);
	CheckExpectedResult(KErrNone, reqStatus4.Int(), KFailedClearDB);
	CheckExpectedResult(KErrNone, reqStatus5.Int(), KFailedClearDB);
	CheckExpectedResult(KErrNone, reqStatus6.Int(), KFailedClearDB);
	CheckExpectedResult(KErrNone, reqStatus7.Int(), KFailedClearDB);
	CheckExpectedResult(KErrNone, reqStatus8.Int(), KFailedClearDB);
	CheckExpectedResult(KErrNone, reqStatus9.Int(), KFailedClearDB);
	CheckExpectedResult(KErrNone, reqStatus10.Int(), KFailedClearDB);
	
	server1.EmptyLastKnownPositionStore(reqStatus1);
	User::After(KClearDBMultipleReqDelay);
	server2.EmptyLastKnownPositionStore(reqStatus2);
	User::After(KClearDBMultipleReqDelay);
	
	User::WaitForRequest(reqStatus1);
	User::WaitForRequest(reqStatus2);

	CheckExpectedResult(KErrNone, reqStatus1.Int(), KFailedClearDB);
	CheckExpectedResult(KErrNone, reqStatus2.Int(), KFailedClearDB);
	
	CleanupStack::PopAndDestroy(10,&server1);

	return TestStepResult();
	
	}




TVerdict  CServerTestStep::TdLocSrv_Server_ClearDB_LocMonitor_SessionOpenFailL(TInt aIndex)
	{

	SetServerTestCasePropertyL(KLocSrvTestSuite, KLbsLocMonitorTestSessionKey, aIndex);
	
	
    DO_PANIC_TEST_L(ClearDBLocMonitorSessionFailL, KLeaveInPanicThread, KErrServerTerminated, KDefaultTestTimeout);
    
  
	return TestStepResult();
	}

TVerdict  CServerTestStep::TdLocSrv_Server_ClearDB_2ClientsL(TInt aIndex)
	{

	SetServerTestCasePropertyL(KLocSrvTestSuite, KLbsLocMonitorTestSessionKey, aIndex);

	RPositionServer server1;
	TInt err = server1.Connect();
	CleanupClosePushL(server1);

	if(err != KErrNone)
		{
		ERR_PRINTF1(KFailedConnectServer);
		SetTestStepResult(EFail);
		return TestStepResult();
		}

	RPositionServer server2;
	err = server2.Connect();
	CleanupClosePushL(server2);

	if(err != KErrNone)
		{
		ERR_PRINTF1(KFailedConnectServer);
		SetTestStepResult(EFail);
		return TestStepResult();
		}


	TRequestStatus reqStatus1;
	server1.EmptyLastKnownPositionStore(reqStatus1);
	User::After(KClearDBMultipleReqDelay);

	TRequestStatus reqStatus2;
	server2.EmptyLastKnownPositionStore(reqStatus2);

	User::WaitForRequest(reqStatus1);
	User::WaitForRequest(reqStatus2);

	CheckExpectedResult(KErrNone, reqStatus1.Int(), KFailedClearDB);
	CheckExpectedResult(KErrNone, reqStatus2.Int(), KFailedClearDB);

	//	    server1.Close();
	//	    server2.Close();
	CleanupStack::PopAndDestroy(2,&server1);

	return TestStepResult();
	}


TVerdict CServerTestStep::TdLocSrv_Server_ClearDB_AndCancelL(TInt aIndex)
	{
	
	SetServerTestCasePropertyL(KLocSrvTestSuite, KLbsLocMonitorTestSessionKey, aIndex);

	RPositionServer server1;
	TInt err = server1.Connect();
	CleanupClosePushL(server1);

	if(err != KErrNone)
		{
		ERR_PRINTF1(KFailedConnectServer);
		SetTestStepResult(EFail);
		return TestStepResult();
		}


	TRequestStatus reqStatus1;
	server1.EmptyLastKnownPositionStore(reqStatus1);
	User::After(KClearDBReqPropagateDelay);

    TBool otherReqPending = (reqStatus1.Int()==KRequestPending);
    _LIT(KIncorrectTestSetup,"Test timing fault");
    if (!otherReqPending)
    	{
    	// Increase the delay in the test location monitor as we require the above 
    	// requests to be outstanding when an emptylastknownpositionstore request 
    	// is issued [the panic occurs when the above requests have been completed before
    	// an emptylastknownpositionstore request ]
    	User::Panic(KIncorrectTestSetup, KErrGeneral);
    	}
    
	TInt cancelresult = server1.CancelRequest(RPositionServer::EReqEmptyLastKnownPositionStore);

	User::WaitForRequest(reqStatus1);

	CheckExpectedResult(KErrNone, cancelresult, KFailedCancelClearDB);
	CheckExpectedResult(KErrCancel, reqStatus1.Int(), KFailedClearDBCancel);

	//	    server1.Close();
	//	    server2.Close();
	CleanupStack::PopAndDestroy(&server1);

	return TestStepResult();
	}

TVerdict CServerTestStep::TdLocSrv_Server_ClearDB_AndCancel_2ClientsL(TInt aIndex)
	{
	SetServerTestCasePropertyL(KLocSrvTestSuite, KLbsLocMonitorTestSessionKey, aIndex);

	RPositionServer server1;
	TInt err = server1.Connect();
	CleanupClosePushL(server1);

	if(err != KErrNone)
		{
		ERR_PRINTF1(KFailedConnectServer);
		SetTestStepResult(EFail);
		return TestStepResult();
		}

	RPositionServer server2;
	err = server2.Connect();
	CleanupClosePushL(server2);

	if(err != KErrNone)
		{
		ERR_PRINTF1(KFailedConnectServer);
		SetTestStepResult(EFail);
		return TestStepResult();
		}


	TRequestStatus reqStatus1;
	server1.EmptyLastKnownPositionStore(reqStatus1);
	User::After(KClearDBMultipleReqDelay);

	TRequestStatus reqStatus2;
	server2.EmptyLastKnownPositionStore(reqStatus2);
	User::After(KClearDBReqPropagateDelay);

    TBool otherReqPending = ((reqStatus1.Int()==KRequestPending)&&(reqStatus2.Int()==KRequestPending));
    _LIT(KIncorrectTestSetup,"Test timing fault");
    if (!otherReqPending)
    	{
    	// Increase the delay in the test location monitor as we require the above 
    	// requests to be outstanding when an emptylastknownpositionstore request 
    	// is issued [the panic occurs when the above requests have been completed before
    	// an emptylastknownpositionstore request ]
    	User::Panic(KIncorrectTestSetup, KErrGeneral);
    	}
	
	
	TInt cancelresult = server2.CancelRequest(RPositionServer::EReqEmptyLastKnownPositionStore);

	User::WaitForRequest(reqStatus1);
	User::WaitForRequest(reqStatus2);

	CheckExpectedResult(cancelresult, KErrNone, KFailedCancelClearDB);
	CheckExpectedResult(reqStatus2.Int(), KErrCancel, KFailedClearDBCancel);

	CheckExpectedResult(reqStatus1.Int(), KErrNone, KFailedClearDB);
	//	    server1.Close();
	//	    server2.Close();
	CleanupStack::PopAndDestroy(2,&server1);

	return TestStepResult();
	}



TVerdict CServerTestStep::TdLocSrv_Server_ClearDB_TimedOutL(TInt aIndex)
	{
	SetServerTestCasePropertyL(KLocSrvTestSuite, KLbsLocMonitorTestSessionKey, aIndex);

	RPositionServer server1;
	TInt err = server1.Connect();
	CleanupClosePushL(server1);

	if(err != KErrNone)
		{
		ERR_PRINTF1(KFailedConnectServer);
		SetTestStepResult(EFail);
		return TestStepResult();
		}

	TRequestStatus reqStatus1;
	server1.EmptyLastKnownPositionStore(reqStatus1);

	User::WaitForRequest(reqStatus1);

	CheckExpectedResult(reqStatus1.Int(), KErrTimedOut, KWrongErrorCode);

	CleanupStack::PopAndDestroy(&server1);

	return TestStepResult();
	
	
	}




