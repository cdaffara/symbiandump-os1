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
 @file PositionerTestStep.cpp
 @internalTechnology
*/
#include "positionerteststep.h"
#include "te_locsrvsuitedefs.h"
#include "tserverstartparams.h" // for TServerStartParams
#include "cserverlaunch.h"

CPositionerTestStep::~CPositionerTestStep()
/**
 * Destructor
 */
	{
	}

CPositionerTestStep::CPositionerTestStep()
/**
 * Constructor
 */
	{
	SetTestStepName(KPositionerTestStep);
	}

TVerdict CPositionerTestStep::doTestStepPreambleL()
/**
 * @return - TVerdict code
 * Override of base class virtual
 */
	{
	
	
	_LIT(KTestName, "CPositionerTestStep::doTestStepPreambleL()");
	INFO_PRINTF1(KTestName);
	
	// The Property is used the modify the behaviour of the testLocMonitorApi for different
	// test cases
	TInt err;
	static _LIT_SECURITY_POLICY_PASS(KAllowAllPolicy);

	err = RProperty::Define(KLocSrvTestSuite,KLbsLocMonitorTestLKPAreaKey,RProperty::EInt, KAllowAllPolicy, KAllowAllPolicy);
	if (err != KErrNone && err != KErrAlreadyExists)
		{
		User::Leave(err);
		}

	err = RProperty::Define(KLocSrvTestSuite,KLbsLocMonitorTestLKPosKey,RProperty::EInt, KAllowAllPolicy, KAllowAllPolicy);
	if (err != KErrNone && err != KErrAlreadyExists)
		{
		User::Leave(err);
		}
	
	CTe_locsrvSuiteStepBase::doTestStepPreambleL();
	
	
	SetTestStepResult(EPass);
	return TestStepResult();
	}



TInt CPositionerTestStep::StartEposServer()
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
	
	_LIT(KErrCode, "server launch return value %d");
	ERR_PRINTF2(KErrCode, err);
	
	return err;
	}



TVerdict CPositionerTestStep::doTestStepL()
/**
 * @return - TVerdict code
 * Override of base class pure virtual
 * Our implementation only gets called if the base class doTestStepPreambleL() did
 * not leave. That being the case, the current test result value will be EPass.
 */
	{
	
	_LIT(KTestName, "CPositionerTestStep::doTestStepL()");
	INFO_PRINTF1(KTestName);
	
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
				_LIT(KTestName, "Attempting to start EposServer");
				INFO_PRINTF1(KTestName);
				err = StartEposServer();
				}

			// If the test location server has been started successfully or is already 
			// running then run the teststep.
			if ((err==KErrNone)||(err==KErrAlreadyExists))
				{
				_LIT(KTestName, "Call test by index");
				INFO_PRINTF1(KTestName);
				TestByIndexL(testIndex);
				}
			else
				{
				_LIT(KTestName, "Failed to start EposServer");
				INFO_PRINTF1(KTestName);
				SetTestStepResult(EFail);
				}

			}
		}
	return TestStepResult();
	}

void CPositionerTestStep::TestByIndexL(TInt aIndex)
    {
    switch (aIndex)
        {
 
        case 1:
        	_LIT(KTestName1, "Get Last Known Position (LKP) Area");
        	INFO_PRINTF1(KTestName1);
        	TdLocSrv_Positioner_GetLKPAreaL(aIndex);
        	break;
        
        case 2:
        	_LIT(KTestName2, "GetLKPArea - Cancel request");
        	INFO_PRINTF1(KTestName2);
        	TdLocSrv_Positioner_GetLKPArea_CancelL(aIndex);
        	break;

        case 3:
        	_LIT(KTestName3, "GetLKPArea - Extended Area Info");
        	INFO_PRINTF1(KTestName3);
        	TdLocSrv_Positioner_GetLKPArea_ExtendedL(aIndex);
        	break;

        case 4:
        	_LIT(KTestName4, "GetLKPArea & Clear Database req in parallel");
        	INFO_PRINTF1(KTestName4);
        	TdLocSrv_Positioner_GetLKPArea_And_ClearDBL(aIndex);
        	break;
        	
        case 5:
        	_LIT(KTestName5, "GetLKPArea - 3 clients - Cancel");
        	INFO_PRINTF1(KTestName5);
            TdLocSrv_Positioner_GetLKPArea_3Clients_CancelL(aIndex);
        	break;
        	
        case 6:
        	_LIT(KTestName6, "GetLKPArea & Clear database - 3 clients");
        	INFO_PRINTF1(KTestName6);
            TdLocSrv_Positioner_GetLKPArea_And_ClearDB_3ClientsL(aIndex);
            break;
            
       case 7:
        	_LIT(KTestName7, "GetLKPArea - Area information not found in the database");
        	INFO_PRINTF1(KTestName7);
        	TdLocSrv_Positioner_GetLKPArea_NotFoundL(aIndex);
        	break;
        	
        case 8:
        	_LIT(KTestName8, "GetLKPArea Extended Info - Incorrect arg from Client");
        	INFO_PRINTF1(KTestName8);
            TdLocSrv_Positioner_GetLKPArea_Extended_IncorrectArgL(aIndex);
        	break;
        	
        case 9:
        	_LIT(KTestName9, "GetLKPArea - LocMonitor SubsessionOpen Fail");
        	INFO_PRINTF1(KTestName9);
        	TdLocSrv_Positioner_GetLKPArea_LocMonitor_SubSessionOpenFailL(aIndex);
        	break;
        	
        case 10:
        	_LIT(KTestName10, "GetLKPArea - Request outstanding - CloseSubsession");
        	INFO_PRINTF1(KTestName10);
        	TdLocSrv_Positioner_GetLKPAreaPending_CloseSubSessionHandleL(aIndex);
            break;
        
        case 11:
        	_LIT(KTestName11, "GetLKPArea - Request Timedout");
        	INFO_PRINTF1(KTestName11);
        	TdLocSrv_Positioner_GetLKPArea_TimedOutL(aIndex);
            break;
            

       case 12:
       		_LIT(KTestName12, "GetLKPArea - 10Clients");
       		INFO_PRINTF1(KTestName12);
       		TdLocSrv_Positioner_GetLKPArea_10ClientsL(aIndex);
       		break;

    	   // For extensions to the last known position area test set
    	   //        case 13
    	   //        case 14
    	   //        case 15
    	   //        case 16
       	   //        case 17
       	   //        case 18
       	   //        case 19
       	   //        case 20
            
        case 21:
        	_LIT(KTestName21, "Get Last Known Position request");
        	INFO_PRINTF1(KTestName21);
        	TdLocSrv_Positioner_GetLKPositionL(aIndex);
            break;
            
        case 22:
          	_LIT(KTestName22, "GetLKPosition - Cancel request");
           	INFO_PRINTF1(KTestName22);
           	TdLocSrv_Positioner_GetLKPosition_CancelL(aIndex);
           	break; 
           	
        case 23:
        	_LIT(KTestName23, "GetLKPosition & Clear Database req in parallel");
        	INFO_PRINTF1(KTestName23);
        	TdLocSrv_Positioner_GetLKPosition_And_ClearDBL(aIndex);
        	break;

        case 24:
          	_LIT(KTestName24, "GetLKPosition - 3 clients - Cancel");
          	INFO_PRINTF1(KTestName24);
          	TdLocSrv_Positioner_GetLKPosition_3Clients_CancelL(aIndex);
          	break;
          	
        case 25:
          	_LIT(KTestName25, "GetLKPosition & Clear database - 3 clients");
          	INFO_PRINTF1(KTestName25);
          	TdLocSrv_Positioner_GetLKPosition_And_ClearDB_3ClientsL(aIndex);
            break;
            
        case 26:
          	_LIT(KTestName26, "GetLKPosition & LKPosition not found in the database");
          	INFO_PRINTF1(KTestName26);
          	TdLocSrv_Positioner_GetLKPosition_NotFoundL(aIndex);
        	break;
        
        case 27:
            _LIT(KTestName27, "GetLKPosition and GetLKPositionArea - 3 clients");
            INFO_PRINTF1(KTestName27);
            TdLocSrv_Positioner_GetLKPosition_AndLKPArea_3ClientsL(aIndex);
            break;

        case 28:
          	_LIT(KTestName28, "GetLKPosition Info - Incorrect arg from Client");
          	INFO_PRINTF1(KTestName28);
          	TdLocSrv_Positioner_GetLKPosition_IncorrectArgL(aIndex);
          	break;

         case 29:
            _LIT(KTestName29, "GetLKPosition - LocMonitor SubsessionOpen Fail");
            INFO_PRINTF1(KTestName29);
            TdLocSrv_Positioner_GetLKPosition_LocMonitor_SubSessionOpenFailL(aIndex);
            break;
          
          case 30:
           	_LIT(KTestName30, "GetLKPosition - Request outstanding - CloseSubsession");
           	INFO_PRINTF1(KTestName30);
           	TdLocSrv_Positioner_GetLKPositionPending_CloseSubSessionHandleL(aIndex);
              break;
              
          case 31:
             _LIT(KTestName31, "GetLKPosition - Request TimedOut");
             INFO_PRINTF1(KTestName31);
             TdLocSrv_Positioner_GetLKPosition_TimedOutL(aIndex);
        	  break;
          
          case 32:
          		_LIT(KTestName32, "GetLKPosition - 10Clients");
          		INFO_PRINTF1(KTestName32);
          		TdLocSrv_Positioner_GetLKPosition_10ClientsL(aIndex);
          		break;
//          case 33 // For future extension to the last known position tests
//          case 34
//          case 35
//          case 36
//          case 37          		
//          case 38
//          case 39
              
          case 40:
            _LIT(KTestName40, "GetLKPosition, GetLKPositionArea & Clear database - 3 clients");
            INFO_PRINTF1(KTestName40);
            TdLocSrv_Positioner_GetLKPosition_LKPArea_AndClearDB_3ClientsL(aIndex);
            break;
            
        default: 
            _LIT(KTestName60, "This test step doesn't exist");
            ERR_PRINTF1(KTestName60);
            SetTestStepResult(EFail);
            break;
            
        }
    }



TVerdict CPositionerTestStep::doTestStepPostambleL()
/**
 * @return - TVerdict code
 * Override of base class virtual
 */
	{
	
	// Delete the keys property used to configure the test location monitor
	// to respond to different test cases
	RProperty property;
	TInt err = property.Delete(KLocSrvTestSuite, KLbsLocMonitorTestLKPAreaKey);
	if (err != KErrNone)
		{
		User::Leave(err);
		}

	err = property.Delete(KLocSrvTestSuite, KLbsLocMonitorTestLKPosKey);	
	if (err != KErrNone)
		{
		User::Leave(err);
		}

	
	return TestStepResult();
	}


//extern TInt SetAuthenticRequestor(RPositioner& aPositioner);
//
//HPositionGenericInfo* CreatePSY6DelayInstructionC(TTimeIntervalMicroSeconds aDelay)
//{
////    HPositionGenericInfo* genInfo = HPositionGenericInfo::NewLC();
////    genInfo->SetRequestedField(KPSY6FieldDelayProcessing);
////    genInfo->SetValue(KPSY6FieldDelayProcessing, aDelay);
////    return genInfo;
//}
//
//extern TBool RequestTimedOut(TRequestStatus&, TUint);

///////////////////////////////////////////////////
// 
// RPositioner
//
//  Open/Close a connection
//
///////////////////////////////////////////////////



void SetTestCasePropertyL(const TUid TestServerUid, const TUint KeyType, TInt aIndex)
	{
	// The Property is used the modify the behaviour of the testLocMonitorApi for different
	// test cases
	TInt err = RProperty::Set(TestServerUid, KeyType, aIndex);
	if (err != KErrNone)
		{
		User::Leave(err);
		}
	}


void CPositionerTestStep::CheckPosition(TPositionInfo aPosInfo)
	{
	TPosition position;
	aPosInfo.GetPosition(position);

	if (Math::IsNaN(position.Latitude())  || 
			Math::IsNaN(position.Longitude()) || Math::IsNaN(position.Altitude()))
		{
		ERR_PRINTF1(KLastKnownPositionCorrupted);
		SetTestStepResult(EFail);
		}

	if ( (position.Latitude()!=DUMMY_LAST_KNOWN_LATITUDE1) || (position.Longitude()!=DUMMY_LAST_KNOWN_LONGITUDE1) ||
			 (position.Altitude()!=DUMMY_LAST_KNOWN_ALTITUDE1) )
		{
		SetTestStepResult(EFail);
		}
	}

void CPositionerTestStep::CheckPositionAreaInfo(TPositionInfo aPosInfo, TPositionAreaInfo aPosAreaInfo)
	{
	TPosition position;
	aPosInfo.GetPosition(position);

	if (Math::IsNaN(position.Latitude())  || 
			Math::IsNaN(position.Longitude()))
		{
		ERR_PRINTF1(KLastKnownPositionCorrupted);
		SetTestStepResult(EFail);
		}

	if ( (position.Latitude()!=DUMMY_LAST_KNOWN_LATITUDE1) || (position.Longitude()!=DUMMY_LAST_KNOWN_LONGITUDE1) ||
			 (position.Altitude()!=DUMMY_LAST_KNOWN_ALTITUDE1)||(aPosAreaInfo.Area() !=  TPositionAreaInfo::EAreaCity))
		{
		SetTestStepResult(EFail);
		}
	}


TVerdict CPositionerTestStep::TdLocSrv_Positioner_GetLKPositionL(TInt aIndex)
	{

	SetTestCasePropertyL(KLocSrvTestSuite,KLbsLocMonitorTestLKPosKey,aIndex);
		
		// Check the correct message is always sent to the server upon a call
		// to this function and that the return value is always that of the
		// completed message.

		RPositionServer server;
		User::LeaveIfError(server.Connect());
		CleanupClosePushL(server);

		RPositioner positioner;
		TInt err = positioner.Open(server);
		
		if(KErrNone != err)
		{
		ERR_PRINTF1(KFailedOpenPositioner);
		SetTestStepResult(EFail);
		return TestStepResult();
		}
		CleanupClosePushL(positioner);	

		TPositionInfo posInfo;
		TRequestStatus reqStatus;

		positioner.GetLastKnownPosition(posInfo, reqStatus);
		User::WaitForRequest(reqStatus);
		
		
		if (KErrNone == reqStatus.Int())
			{
			CheckPosition(posInfo);
			}
		else
			{
			// feature is not supported
			ERR_PRINTF1(KFailedGetLastKnownPosition);
			SetTestStepResult(EFail);
			}
		
		CleanupStack::PopAndDestroy(2, &server);
		return TestStepResult();
	}


// 1.25.1 GetLastKnownPositionArea
TVerdict CPositionerTestStep::TdLocSrv_Positioner_GetLKPAreaL(TInt aIndex)
	{
	
	SetTestCasePropertyL(KLocSrvTestSuite,KLbsLocMonitorTestLKPAreaKey,aIndex);
	
	// Check the correct message is always sent to the server upon a call
	// to this function and that the return value is always that of the
	// completed message.
	__UHEAP_MARK;
	
	
	RPositionServer server;
	User::LeaveIfError(server.Connect());
	CleanupClosePushL(server);

	RPositioner positioner;
	TInt err = positioner.Open(server);
	if(KErrNone != err)
	{
	_LIT(KError,"Failed to  open positioner with error : %d");
	ERR_PRINTF2(KError, err);
	
	ERR_PRINTF1(KFailedOpenPositioner);
	SetTestStepResult(EFail);
	return TestStepResult();
	}
	CleanupClosePushL(positioner);	

	TPositionInfo posInfo;
	TPositionAreaInfo posAreaInfo;
	TRequestStatus reqStatus;

	positioner.GetLastKnownPositionArea(posInfo, posAreaInfo, reqStatus);
	User::WaitForRequest(reqStatus);
	
//	TInt modprop; 
//	RProperty::Get(KLocSrvTestSuite,KLbsLocMonitorTestLKPAreaKey,modprop); 
	
	if (KErrNone == reqStatus.Int())
		{
		CheckPositionAreaInfo(posInfo, posAreaInfo);
		}
	else
		{
		// feature is not supported
		ERR_PRINTF1(KFailedGetLastKnownPosition);
		SetTestStepResult(EFail);
		}
	
	CleanupStack::PopAndDestroy(2, &server);
	
	__UHEAP_MARKEND;	
	return TestStepResult();
	}

// 1.26.1 GetLastKnownPositionAreaCancel
TVerdict CPositionerTestStep::TdLocSrv_Positioner_GetLKPArea_CancelL(TInt aIndex)
	{

	SetTestCasePropertyL(KLocSrvTestSuite,KLbsLocMonitorTestLKPAreaKey,aIndex);
	
	
	// Check the correct message is always sent to the server upon a call
	// to this function and that the return value is always that of the
	// completed message.

	RPositionServer server;
	User::LeaveIfError(server.Connect());
	CleanupClosePushL(server);

	RPositioner positioner;
	TInt err = positioner.Open(server);
	if(KErrNone != err)
	{
	ERR_PRINTF1(KFailedOpenPositioner);
	SetTestStepResult(EFail);
	return TestStepResult();
	}
	CleanupClosePushL(positioner);	
	
	TPositionInfo posInfo;
	TPositionAreaInfo posAreaInfo;
	TRequestStatus reqStatus;

	positioner.GetLastKnownPositionArea(posInfo, posAreaInfo, reqStatus);
	
	User::After(KGetLKPAReqPropagateDelay); 
	// Delay to ensure the GetLastKnownPositionArea request reaches the server before a cancel is sent
	
    TBool otherReqPending =( reqStatus.Int()==KRequestPending) ;
    _LIT(KIncorrectTestSetup,"Test timing fault");
    if (!otherReqPending)
    	{
    	// Increase the delay in the test location monitor as we require the above 
    	// requests to be outstanding when an emptylastknownpositionstore request 
    	// is issued [the panic occurs when the above requests have been completed before
    	// an emptylastknownpositionstore request ]
    	User::Panic(KIncorrectTestSetup, KErrGeneral);
    	}
	TInt cancelresult = positioner.CancelRequest(RPositionerSubSessionBase::EReqGetLastKnownPositionArea);
	User::WaitForRequest(reqStatus);
	
	CheckExpectedResult(cancelresult, KErrNone,  KFailedCancelGLKPArea);
	CheckExpectedResult(reqStatus.Int(), KErrCancel,  KFailedGLKPAreaReqCancel);
	
	CleanupStack::PopAndDestroy(2, &server);
	return TestStepResult();
	}

// 1.27.1 GetLastKnownPositionAreaExtended
TVerdict CPositionerTestStep::TdLocSrv_Positioner_GetLKPArea_ExtendedL(TInt aIndex)
	{
	
	SetTestCasePropertyL(KLocSrvTestSuite,KLbsLocMonitorTestLKPAreaKey,aIndex);
	
	// Check the correct message is always sent to the server upon a call
	// to this function and that the return value is always that of the
	// completed message.

	RPositionServer server;
	User::LeaveIfError(server.Connect());
	CleanupClosePushL(server);

	RPositioner positioner;
	TInt err = positioner.Open(server);
	if(KErrNone != err)
	{
	ERR_PRINTF1(KFailedOpenPositioner);
	SetTestStepResult(EFail);
	return TestStepResult();
	}
	CleanupClosePushL(positioner);	
	
	TPositionInfo posInfo;
	TPositionAreaExtendedInfo posAreaInfo;
	TRequestStatus reqStatus;

	positioner.GetLastKnownPositionArea(posInfo, posAreaInfo, reqStatus);
	User::WaitForRequest(reqStatus);
	
	if (KErrNone == reqStatus.Int())
		{
		
		CheckPositionAreaInfo(posInfo, posAreaInfo);
		
		// The test value for area information is EAreaCity as seen in
		// testLocMonitorApi - Therefore all the following four fields
		// must match [refer LbsAreaInfo.h]
		TBool areaExtendedInfo = ((posAreaInfo.LocationAreaCodeMatch())&&
								(posAreaInfo.MobileCountryCodeMatch())&&
								(posAreaInfo.MobileNetworkCodeMatch())&&
								(posAreaInfo.CellIdMatch()));
		if (!areaExtendedInfo)
			{
			SetTestStepResult(EFail);
			}
		}
	else
		{
		// feature is not supported
		ERR_PRINTF1(KFailedGetLastKnownPosition);
		SetTestStepResult(EFail);
		}
	
	CleanupStack::PopAndDestroy(2, &server);
	return TestStepResult();
	
	}


TVerdict CPositionerTestStep::TdLocSrv_Positioner_GetLKPosition_CancelL(TInt aIndex)
	{

	SetTestCasePropertyL(KLocSrvTestSuite,KLbsLocMonitorTestLKPosKey,aIndex);
	
	// Check the correct message is always sent to the server upon a call
	// to this function and that the return value is always that of the
	// completed message.

	RPositionServer server;
	User::LeaveIfError(server.Connect());
	CleanupClosePushL(server);

	RPositioner positioner;
	TInt err = positioner.Open(server);
	if(KErrNone != err)
	{
	ERR_PRINTF1(KFailedOpenPositioner);
	SetTestStepResult(EFail);
	return TestStepResult();
	}
	CleanupClosePushL(positioner);	
	
	TPositionInfo posInfo;
	TRequestStatus reqStatus;

	positioner.GetLastKnownPosition(posInfo, reqStatus);
	User::After(KGetLKPosReqPropagateDelay); 
	// Delay to ensure the GetLastKnownPosition request reaches the server before a cancel is sent
	
    TBool otherReqPending =( reqStatus.Int()==KRequestPending );
    _LIT(KIncorrectTestSetup,"Test timing fault");
    if (!otherReqPending)
    	{
    	// Increase the delay in the test location monitor as we require the above 
    	// requests to be outstanding when an emptylastknownpositionstore request 
    	// is issued [the panic occurs when the above requests have been completed before
    	// an emptylastknownpositionstore request ]
    	User::Panic(KIncorrectTestSetup, KErrGeneral);
    	}
    
	TInt cancelresult = positioner.CancelRequest(RPositionerSubSessionBase::EReqGetLastKnownPosition);
	User::WaitForRequest(reqStatus);
	
	CheckExpectedResult(cancelresult, KErrNone,  KFailedCancelGLKPos);
	CheckExpectedResult(reqStatus.Int(), KErrCancel,  KFailedGLKPosReqCancel);
	
	CleanupStack::PopAndDestroy(2, &server);
	return TestStepResult();
	}

TVerdict CPositionerTestStep::TdLocSrv_Positioner_GetLKPosition_And_ClearDBL(TInt aIndex)
	{
	
	SetTestCasePropertyL(KLocSrvTestSuite,KLbsLocMonitorTestLKPosKey,aIndex);

	
    RPositionServer server;
    TInt err = server.Connect();
    CleanupClosePushL(server);
    
    if(err != KErrNone)
        {
        ERR_PRINTF1(KFailedConnectServer);
        SetTestStepResult(EFail);
        return TestStepResult();
        }

	    RPositioner positioner;
	    User::LeaveIfError(positioner.Open(server));
	    CleanupClosePushL(positioner);
    
	    TPositionInfo posInfo;	TRequestStatus reqStatus;
	    positioner.GetLastKnownPosition(posInfo,reqStatus);
		
	    User::After(KGetLKPosReqPropagateDelay); 
		// Delay to ensure the GetLastKnownPosition request reaches the server before a cancel is sent

	    TBool otherReqPending =( reqStatus.Int()==KRequestPending) ;
	    _LIT(KIncorrectTestSetup,"Test timing fault");
	    if (!otherReqPending)
	    	{
	    	// Increase the delay in the test location monitor as we require the above 
	    	// requests to be outstanding when an emptylastknownpositionstore request 
	    	// is issued [the panic occurs when the above requests have been completed before
	    	// an emptylastknownpositionstore request ]
	    	User::Panic(KIncorrectTestSetup, KErrGeneral);
	    	}
	    
	    TRequestStatus clearDbReqStatus;
	    server.EmptyLastKnownPositionStore(clearDbReqStatus);
	    
	    User::WaitForRequest(reqStatus);
	    User::WaitForRequest(clearDbReqStatus);

		CheckExpectedResult(clearDbReqStatus.Int(),KErrNone,  KFailedClearDB);
		CheckExpectedResult(reqStatus.Int(),KErrCancel,  KFailedCancelReqAfterClearDB);
		
		CleanupStack::PopAndDestroy(2,&server);

	    return TestStepResult();
	}

TVerdict CPositionerTestStep::TdLocSrv_Positioner_GetLKPArea_And_ClearDBL(TInt aIndex)
	{

	SetTestCasePropertyL(KLocSrvTestSuite,KLbsLocMonitorTestLKPAreaKey,aIndex);

    RPositionServer server;
    TInt err = server.Connect();
    CleanupClosePushL(server);
    
    if(err != KErrNone)
        {
        ERR_PRINTF1(KFailedConnectServer);
        SetTestStepResult(EFail);
        return TestStepResult();
        }

	    RPositioner positioner;
	    User::LeaveIfError(positioner.Open(server));
	    CleanupClosePushL(positioner);
    
	    TPositionInfo posInfo;	TPositionAreaInfo posAreaInfo;	TRequestStatus reqStatus;
	    positioner.GetLastKnownPositionArea(posInfo, posAreaInfo, reqStatus);
		
	    User::After(KGetLKPAReqPropagateDelay); 
		// Delay to ensure the GetLastKnownPositionArea request reaches the server before a cancel is sent

	    TBool otherReqPending =( reqStatus.Int()==KRequestPending );
	    _LIT(KIncorrectTestSetup,"Test timing fault");
	    if (!otherReqPending)
	    	{
	    	// Increase the delay in the test location monitor as we require the above 
	    	// requests to be outstanding when an emptylastknownpositionstore request 
	    	// is issued [the panic occurs when the above requests have been completed before
	    	// an emptylastknownpositionstore request ]
	    	User::Panic(KIncorrectTestSetup, KErrGeneral);
	    	}
	    
	    TRequestStatus clearDbReqStatus;
	    server.EmptyLastKnownPositionStore(clearDbReqStatus);
	    
	    User::WaitForRequest(reqStatus);
	    User::WaitForRequest(clearDbReqStatus);

		CheckExpectedResult(clearDbReqStatus.Int(),KErrNone,  KFailedClearDB);
		CheckExpectedResult(reqStatus.Int(),KErrCancel,  KFailedCancelReqAfterClearDB);
		
		CleanupStack::PopAndDestroy(2,&server);
	    return TestStepResult();
	}

TVerdict CPositionerTestStep::TdLocSrv_Positioner_GetLKPosition_IncorrectArgL(TInt aIndex)
	{

	SetTestCasePropertyL(KLocSrvTestSuite,KLbsLocMonitorTestLKPosKey,aIndex);
	
		
		// Check the correct message is always sent to the server upon a call
		// to this function and that the return value is always that of the
		// completed message.
		RPositionServer server;
		User::LeaveIfError(server.Connect());
		CleanupClosePushL(server);
	
		RPositioner positioner;
		TInt err = positioner.Open(server);
		
		if(KErrNone != err)
		{
		ERR_PRINTF1(KFailedOpenPositioner);
		SetTestStepResult(EFail);
		return TestStepResult();
		}
		CleanupClosePushL(positioner);	
		
		TPositionCourseInfo posInfo;// Incorrect argument - *** KErrArgument returned to the Client ***
		TRequestStatus reqStatus;
	
		positioner.GetLastKnownPosition(posInfo, reqStatus);
		User::WaitForRequest(reqStatus);
		
		CheckExpectedResult(reqStatus.Int(), KErrArgument,  KFailedSpotIncorrectArg);
		
		CleanupStack::PopAndDestroy(2, &server);
	
		return TestStepResult();
	}


TVerdict CPositionerTestStep::TdLocSrv_Positioner_GetLKPArea_Extended_IncorrectArgL(TInt aIndex)
	{

	SetTestCasePropertyL(KLocSrvTestSuite,KLbsLocMonitorTestLKPAreaKey,aIndex);
		
		// Check the correct message is always sent to the server upon a call
		// to this function and that the return value is always that of the
		// completed message.
		RPositionServer server;
		User::LeaveIfError(server.Connect());
		CleanupClosePushL(server);
	
		RPositioner positioner;
		TInt err = positioner.Open(server);
		
		if(KErrNone != err)
		{
		ERR_PRINTF1(KFailedOpenPositioner);
		SetTestStepResult(EFail);
		return TestStepResult();
		}
		CleanupClosePushL(positioner);	
		
		TPositionCourseInfo posInfo;// Incorrect argument - *** KErrArgument returned to the Client ***
		TPositionAreaExtendedInfo posAreaInfo; 
		TRequestStatus reqStatus,reqStatus2;
	
		positioner.GetLastKnownPositionArea(posInfo, posAreaInfo, reqStatus);
		User::WaitForRequest(reqStatus);
		
		CheckExpectedResult(reqStatus.Int(), KErrArgument,  KFailedSpotIncorrectArg);
		
		CleanupStack::PopAndDestroy(2, &server);
	
		return TestStepResult();
	}

TVerdict CPositionerTestStep::TdLocSrv_Positioner_GetLKPosition_LocMonitor_SubSessionOpenFailL(TInt aIndex)
	{
	
	
	SetTestCasePropertyL(KLocSrvTestSuite,KLbsLocMonitorTestLKPosKey,aIndex);
	
	// Check the correct message is always sent to the server upon a call
	// to this function and that the return value is always that of the
	// completed message.

	__UHEAP_MARK;
	
	RPositionServer server;
	User::LeaveIfError(server.Connect());
	CleanupClosePushL(server);

	RPositioner positioner;
	TInt err = positioner.Open(server);
	if(KErrNone != err)
	{
	ERR_PRINTF1(KFailedOpenPositioner);

	SetTestStepResult(EFail);
	return TestStepResult();
	}
	CleanupClosePushL(positioner);	
	

	TPositionInfo posInfo;
	TRequestStatus reqStatus;

	positioner.GetLastKnownPosition(posInfo, reqStatus);
	User::WaitForRequest(reqStatus);
	
	CheckExpectedResult(reqStatus.Int(), KErrCouldNotConnect,  KUnexpectedEvents);
	
	CleanupStack::PopAndDestroy(2, &server);
	
	__UHEAP_MARKEND;
	
	return TestStepResult();
	
	}

TVerdict CPositionerTestStep::TdLocSrv_Positioner_GetLKPArea_LocMonitor_SubSessionOpenFailL(TInt aIndex)
	{
	
	
	SetTestCasePropertyL(KLocSrvTestSuite,KLbsLocMonitorTestLKPAreaKey,aIndex);

	// Check the correct message is always sent to the server upon a call
	// to this function and that the return value is always that of the
	// completed message.

	__UHEAP_MARK;
	
	RPositionServer server;
	User::LeaveIfError(server.Connect());
	CleanupClosePushL(server);

	RPositioner positioner;
	TInt err = positioner.Open(server);
	if(KErrNone != err)
	{
	ERR_PRINTF1(KFailedOpenPositioner);

	SetTestStepResult(EFail);
	return TestStepResult();
	}
	CleanupClosePushL(positioner);	
	

	TPositionInfo posInfo;
	TPositionAreaInfo posAreaInfo;
	TRequestStatus reqStatus;

	positioner.GetLastKnownPositionArea(posInfo, posAreaInfo, reqStatus);
	User::WaitForRequest(reqStatus);
	
	CheckExpectedResult(reqStatus.Int(), KErrCouldNotConnect,  KUnexpectedEvents);
	
	CleanupStack::PopAndDestroy(2, &server);
	
	__UHEAP_MARKEND;
	
	return TestStepResult();
	
	}


TInt CloseSubSessionWithGetLKPAreaReqsL (TAny* /*aPtr*/)
	{
	RPositionServer server;
	User::LeaveIfError(server.Connect());
	CleanupClosePushL(server);

	RPositioner positioner;
	TInt err = positioner.Open(server);
	CleanupClosePushL(positioner);	
	
	TPositionInfo posInfo;
	TPositionAreaInfo posAreaInfo;
	TRequestStatus reqStatus;

	positioner.GetLastKnownPositionArea(posInfo, posAreaInfo, reqStatus);
	// Delay to ensure the GetLastKnownPositionArea request reaches the server before a cancel is sent
	User::After(KGetLKPAReqPropagateDelay);
	
	
    TBool otherReqPending = ( reqStatus.Int()==KRequestPending );
    _LIT(KIncorrectTestSetup,"Test timing fault");
    if (!otherReqPending)
    	{
    	// Increase the delay in the test location monitor as we require the above 
    	// requests to be outstanding when an emptylastknownpositionstore request 
    	// is issued [the panic occurs when the above requests have been completed before
    	// an emptylastknownpositionstore request ]
    	User::Panic(KIncorrectTestSetup, KErrGeneral);
    	}
	positioner.Close();
	
	// Delay to ensure the handles have been closed	
	User::After(KServerHandleCloseDelay);
	CleanupStack::Pop(&positioner);
	CleanupStack::PopAndDestroy(&server);
	return KErrGeneral;
	}

TVerdict CPositionerTestStep::TdLocSrv_Positioner_GetLKPAreaPending_CloseSubSessionHandleL(TInt aIndex)
	{

	SetTestCasePropertyL(KLocSrvTestSuite,KLbsLocMonitorTestLKPAreaKey,aIndex);
	
    DO_PANIC_TEST_L(CloseSubSessionWithGetLKPAreaReqsL, KPosClientFault, 
    		EPositionRequestsNotCancelled, KDefaultTestTimeout);
    
    return TestStepResult();
	
	}


TVerdict CPositionerTestStep::TdLocSrv_Positioner_GetLKPArea_10ClientsL(TInt aIndex)
	{

	SetTestCasePropertyL(KLocSrvTestSuite,KLbsLocMonitorTestLKPAreaKey,aIndex);

	// Check the correct message is always sent to the server upon a call
	// to this function and that the return value is always that of the
	// completed message.

	__UHEAP_MARK;
	
	RPositionServer server1;
	RPositionServer server2;

	User::LeaveIfError(server1.Connect());
	CleanupClosePushL(server1);
	User::LeaveIfError(server2.Connect());
	CleanupClosePushL(server2);
	
	RPositioner positioner1;
	RPositioner positioner2;
	RPositioner positioner3;
	RPositioner positioner4;
	RPositioner positioner5;
	RPositioner positioner6;
	RPositioner positioner7;
	RPositioner positioner8;
	RPositioner positioner9;
	RPositioner positioner10;

	User::LeaveIfError(positioner1.Open(server1));
	CleanupClosePushL(positioner1);	
	User::LeaveIfError(positioner2.Open(server1));
	CleanupClosePushL(positioner2);	
	User::LeaveIfError(positioner3.Open(server1));
	CleanupClosePushL(positioner3);	
	User::LeaveIfError(positioner4.Open(server1));
	CleanupClosePushL(positioner4);	
	User::LeaveIfError(positioner5.Open(server1));
	CleanupClosePushL(positioner5);	
	User::LeaveIfError(positioner6.Open(server2));
	CleanupClosePushL(positioner6);	
	User::LeaveIfError(positioner7.Open(server2));
	CleanupClosePushL(positioner7);	
	User::LeaveIfError(positioner8.Open(server2));
	CleanupClosePushL(positioner8);
	User::LeaveIfError(positioner9.Open(server2));
	CleanupClosePushL(positioner9);	
	User::LeaveIfError(positioner10.Open(server2));
	CleanupClosePushL(positioner10);

	TPositionInfo posInfo;
	TPositionAreaInfo posAreaInfo;
	TRequestStatus 	reqStatus1,reqStatus2,reqStatus3,reqStatus4,reqStatus5,
					reqStatus6, reqStatus7, reqStatus8,reqStatus9,reqStatus10;

	positioner1.GetLastKnownPositionArea(posInfo, posAreaInfo, reqStatus1);
	User::After(KGetLKPAMultipleReqDelay);
	positioner6.GetLastKnownPositionArea(posInfo, posAreaInfo, reqStatus6);
	User::After(KGetLKPAMultipleReqDelay);
	
	positioner2.GetLastKnownPositionArea(posInfo, posAreaInfo, reqStatus2);
	User::After(KGetLKPAMultipleReqDelay);
	positioner7.GetLastKnownPositionArea(posInfo, posAreaInfo, reqStatus7);
	User::After(KGetLKPAMultipleReqDelay);
	
	positioner3.GetLastKnownPositionArea(posInfo, posAreaInfo, reqStatus3);
	User::After(KGetLKPAMultipleReqDelay);
	positioner8.GetLastKnownPositionArea(posInfo, posAreaInfo, reqStatus8);
	User::After(KGetLKPAMultipleReqDelay);
	
	positioner4.GetLastKnownPositionArea(posInfo, posAreaInfo, reqStatus4);
	User::After(KGetLKPAMultipleReqDelay);
	positioner9.GetLastKnownPositionArea(posInfo, posAreaInfo, reqStatus9);
	User::After(KGetLKPAMultipleReqDelay);
	
	positioner5.GetLastKnownPositionArea(posInfo, posAreaInfo, reqStatus5);
	User::After(KGetLKPAMultipleReqDelay);
	positioner10.GetLastKnownPositionArea(posInfo, posAreaInfo, reqStatus10);
	User::After(KGetLKPAMultipleReqDelay);
	
	
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
	
	CheckExpectedResult(reqStatus1.Int(), KErrNone,  KFailedGLKPArea);
	CheckExpectedResult(reqStatus2.Int(), KErrNone,  KFailedGLKPArea);
	CheckExpectedResult(reqStatus3.Int(), KErrNone,  KFailedGLKPArea);
	CheckExpectedResult(reqStatus4.Int(), KErrNone,  KFailedGLKPArea);
	CheckExpectedResult(reqStatus5.Int(), KErrNone,  KFailedGLKPArea);
	CheckExpectedResult(reqStatus6.Int(), KErrNone,  KFailedGLKPArea);
	CheckExpectedResult(reqStatus7.Int(), KErrNone,  KFailedGLKPArea);
	CheckExpectedResult(reqStatus8.Int(), KErrNone,  KFailedGLKPArea);
	CheckExpectedResult(reqStatus9.Int(), KErrNone,  KFailedGLKPArea);
	CheckExpectedResult(reqStatus10.Int(), KErrNone,  KFailedGLKPArea);

	if (KErrNone == reqStatus10.Int())
		{
		CheckPositionAreaInfo(posInfo,posAreaInfo);
		}
	else
		{
		// feature is not supported
		ERR_PRINTF1(KFailedGetLastKnownPosition);
		SetTestStepResult(EFail);
		}
			
	positioner1.GetLastKnownPositionArea(posInfo, posAreaInfo, reqStatus1);
	User::After(KGetLKPAMultipleReqDelay);
	positioner6.GetLastKnownPositionArea(posInfo, posAreaInfo, reqStatus6);
	User::After(KGetLKPAMultipleReqDelay);
	
	User::WaitForRequest(reqStatus1);
	User::WaitForRequest(reqStatus6);
	
	CheckExpectedResult(reqStatus1.Int(), KErrNone,  KFailedGLKPArea);
	CheckExpectedResult(reqStatus6.Int(), KErrNone,  KFailedGLKPArea);
	

	if (KErrNone == reqStatus6.Int())
		{
		CheckPositionAreaInfo(posInfo,posAreaInfo);
		}
	else
		{
		// feature is not supported
		ERR_PRINTF1(KFailedGetLastKnownPosition);
		SetTestStepResult(EFail);
		}
	
	CleanupStack::PopAndDestroy(12, &server1);
	
	__UHEAP_MARKEND;
	
	return TestStepResult();
	
	
	}



TInt CloseSubSessionWithGetLKPosReqsL (TAny* /*aPtr*/)
	{
	RPositionServer server;
	User::LeaveIfError(server.Connect());
	CleanupClosePushL(server);

	RPositioner positioner;
	TInt err = positioner.Open(server);
	CleanupClosePushL(positioner);	
	
	TPositionInfo posInfo;
	TRequestStatus reqStatus;

	positioner.GetLastKnownPosition(posInfo, reqStatus);
	// Delay to ensure the GetLastKnownPosition request reaches the server before a cancel is sent
	User::After(KGetLKPosReqPropagateDelay);
	
    TBool otherReqPending =(reqStatus.Int()==KRequestPending);
    _LIT(KIncorrectTestSetup,"Test timing fault");
    if (!otherReqPending)
    	{
    	// Increase the delay in the test location monitor as we require the above 
    	// requests to be outstanding when an emptylastknownpositionstore request 
    	// is issued [the panic occurs when the above requests have been completed before
    	// an emptylastknownpositionstore request ]
    	User::Panic(KIncorrectTestSetup, KErrGeneral);
    	}
	positioner.Close();
	
	// Delay to ensure the handles have been closed	
	User::After(KServerHandleCloseDelay);
	CleanupStack::Pop(&positioner);
	CleanupStack::PopAndDestroy(&server);
	return KErrGeneral;
	}

TVerdict CPositionerTestStep::TdLocSrv_Positioner_GetLKPositionPending_CloseSubSessionHandleL(TInt aIndex)
	{

	SetTestCasePropertyL(KLocSrvTestSuite,KLbsLocMonitorTestLKPosKey,aIndex);
	
    DO_PANIC_TEST_L(CloseSubSessionWithGetLKPosReqsL, KPosClientFault, 
    		EPositionRequestsNotCancelled, KDefaultTestTimeout);
    
    return TestStepResult();
	
	}


TVerdict CPositionerTestStep::TdLocSrv_Positioner_GetLKPosition_10ClientsL(TInt aIndex)
	{

	SetTestCasePropertyL(KLocSrvTestSuite,KLbsLocMonitorTestLKPosKey,aIndex);

	// Check the correct message is always sent to the server upon a call
	// to this function and that the return value is always that of the
	// completed message.

	__UHEAP_MARK;
	
	RPositionServer server1;
	RPositionServer server2;

	User::LeaveIfError(server1.Connect());
	CleanupClosePushL(server1);
	User::LeaveIfError(server2.Connect());
	CleanupClosePushL(server2);
	
	RPositioner positioner1;
	RPositioner positioner2;
	RPositioner positioner3;
	RPositioner positioner4;
	RPositioner positioner5;
	RPositioner positioner6;
	RPositioner positioner7;
	RPositioner positioner8;
	RPositioner positioner9;
	RPositioner positioner10;

	User::LeaveIfError(positioner1.Open(server1));
	CleanupClosePushL(positioner1);	
	User::LeaveIfError(positioner2.Open(server1));
	CleanupClosePushL(positioner2);	
	User::LeaveIfError(positioner3.Open(server1));
	CleanupClosePushL(positioner3);	
	User::LeaveIfError(positioner4.Open(server1));
	CleanupClosePushL(positioner4);	
	User::LeaveIfError(positioner5.Open(server1));
	CleanupClosePushL(positioner5);	
	User::LeaveIfError(positioner6.Open(server2));
	CleanupClosePushL(positioner6);	
	User::LeaveIfError(positioner7.Open(server2));
	CleanupClosePushL(positioner7);	
	User::LeaveIfError(positioner8.Open(server2));
	CleanupClosePushL(positioner8);
	User::LeaveIfError(positioner9.Open(server2));
	CleanupClosePushL(positioner9);	
	User::LeaveIfError(positioner10.Open(server2));
	CleanupClosePushL(positioner10);

	TPositionInfo posInfo;
	TRequestStatus 	reqStatus1,reqStatus2,reqStatus3,reqStatus4,reqStatus5,
					reqStatus6, reqStatus7, reqStatus8,reqStatus9,reqStatus10;

	positioner1.GetLastKnownPosition(posInfo, reqStatus1);
	User::After(KGetLKPosMultipleReqDelay);
	positioner6.GetLastKnownPosition(posInfo, reqStatus6);
	User::After(KGetLKPosMultipleReqDelay);
	
	positioner2.GetLastKnownPosition(posInfo, reqStatus2);
	User::After(KGetLKPosMultipleReqDelay);
	positioner7.GetLastKnownPosition(posInfo, reqStatus7);
	User::After(KGetLKPosMultipleReqDelay);

	positioner3.GetLastKnownPosition(posInfo, reqStatus3);
	User::After(KGetLKPosMultipleReqDelay);
	positioner8.GetLastKnownPosition(posInfo, reqStatus8);
	User::After(KGetLKPosMultipleReqDelay);

	positioner4.GetLastKnownPosition(posInfo, reqStatus4);
	User::After(KGetLKPosMultipleReqDelay);
	positioner9.GetLastKnownPosition(posInfo, reqStatus9);
	User::After(KGetLKPosMultipleReqDelay);

	positioner5.GetLastKnownPosition(posInfo, reqStatus5);
	User::After(KGetLKPosMultipleReqDelay);
	positioner10.GetLastKnownPosition(posInfo, reqStatus10);
	User::After(KGetLKPosMultipleReqDelay);
	
	
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
	
	CheckExpectedResult(reqStatus1.Int(), KErrNone,  KFailedGLKPos);
	CheckExpectedResult(reqStatus2.Int(), KErrNone,  KFailedGLKPos);
	CheckExpectedResult(reqStatus3.Int(), KErrNone,  KFailedGLKPos);
	CheckExpectedResult(reqStatus4.Int(), KErrNone,  KFailedGLKPos);
	CheckExpectedResult(reqStatus5.Int(), KErrNone,  KFailedGLKPos);
	CheckExpectedResult(reqStatus6.Int(), KErrNone,  KFailedGLKPos);
	CheckExpectedResult(reqStatus7.Int(), KErrNone,  KFailedGLKPos);
	CheckExpectedResult(reqStatus8.Int(), KErrNone,  KFailedGLKPos);
	CheckExpectedResult(reqStatus9.Int(), KErrNone,  KFailedGLKPos);
	CheckExpectedResult(reqStatus10.Int(), KErrNone,  KFailedGLKPos);

	if (KErrNone == reqStatus10.Int())
		{
		CheckPosition(posInfo);
		}
	else
		{
		// feature is not supported
		ERR_PRINTF1(KFailedGetLastKnownPosition);
		SetTestStepResult(EFail);
		}
			
	positioner1.GetLastKnownPosition(posInfo, reqStatus1);
	User::After(KGetLKPosMultipleReqDelay);
	positioner6.GetLastKnownPosition(posInfo, reqStatus6);
	User::After(KGetLKPosMultipleReqDelay);
	
	User::WaitForRequest(reqStatus1);
	User::WaitForRequest(reqStatus6);
	
	CheckExpectedResult(reqStatus1.Int(), KErrNone,  KFailedGLKPos);
	CheckExpectedResult(reqStatus6.Int(), KErrNone,  KFailedGLKPos);

	if (KErrNone == reqStatus6.Int())
		{
		CheckPosition(posInfo);
		}

	
	CleanupStack::PopAndDestroy(12, &server1);
	
	__UHEAP_MARKEND;
	
	return TestStepResult();
	
	
	}


TVerdict CPositionerTestStep::TdLocSrv_Positioner_GetLKPArea_3Clients_CancelL(TInt aIndex)
	{
	
	
	SetTestCasePropertyL(KLocSrvTestSuite,KLbsLocMonitorTestLKPAreaKey,aIndex);

	    RPositionServer server1;
	    TInt err = server1.Connect();
	    CleanupClosePushL(server1);
	    
	    if(err != KErrNone)
	        {
	        ERR_PRINTF1(KFailedConnectServer);
	        SetTestStepResult(EFail);
	        return TestStepResult();
	        }

	    RPositioner positioner1;
	    User::LeaveIfError(positioner1.Open(server1));
	    CleanupClosePushL(positioner1);

	    RPositioner positioner2;
	    User::LeaveIfError(positioner2.Open(server1));
	    CleanupClosePushL(positioner2);

	    RPositionServer server2;
	    err = server2.Connect();
	    CleanupClosePushL(server2);
	    
	    if(err != KErrNone)
	        {
	        ERR_PRINTF1(KFailedConnectServer);
	        SetTestStepResult(EFail);
	        return TestStepResult();
	        }
	    RPositioner positioner3;
	    User::LeaveIfError(positioner3.Open(server2));
	    CleanupClosePushL(positioner3);
	    
	    TPositionInfo posInfo1;	TPositionAreaInfo posAreaInfo1;	TRequestStatus reqStatus1;
	    positioner1.GetLastKnownPositionArea(posInfo1, posAreaInfo1, reqStatus1);
	    User::After(KGetLKPAMultipleReqDelay);
	    
	    TPositionInfo posInfo2;	TPositionAreaInfo posAreaInfo2;	TRequestStatus reqStatus2;
	    positioner2.GetLastKnownPositionArea(posInfo2, posAreaInfo2, reqStatus2);
	    User::After(KGetLKPAMultipleReqDelay);
	    
	    TPositionInfo posInfo3;	TPositionAreaInfo posAreaInfo3;	TRequestStatus reqStatus3;
	    positioner3.GetLastKnownPositionArea(posInfo3, posAreaInfo3, reqStatus3);
	    User::After(KGetLKPAReqPropagateDelay); 
		// Delay to ensure the GetLastKnownPositionArea request reaches the server before a cancel is sent
	   
		
	    TBool otherReqPending =( (reqStatus1.Int()==KRequestPending) && (reqStatus2.Int()==KRequestPending) && (reqStatus3.Int()==KRequestPending) );
	    _LIT(KIncorrectTestSetup,"Test timing fault");
	    if (!otherReqPending)
	    	{
	    	// Increase the delay in the test location monitor as we require the above 
	    	// requests to be outstanding when an emptylastknownpositionstore request 
	    	// is issued [the panic occurs when the above requests have been completed before
	    	// an emptylastknownpositionstore request ]
	    	User::Panic(KIncorrectTestSetup, KErrGeneral);
	    	}
	    
		TInt cancelresult = positioner1.CancelRequest(RPositionerSubSessionBase::EReqGetLastKnownPositionArea);
		
	    User::WaitForRequest(reqStatus1);
	    User::WaitForRequest(reqStatus2);
	    User::WaitForRequest(reqStatus3);
	    
		CheckExpectedResult(cancelresult, KErrNone, KFailedCancelGLKPArea);
		CheckExpectedResult(reqStatus1.Int(), KErrCancel,  KFailedGLKPAreaReqCancel);
		CheckExpectedResult(reqStatus2.Int(), KErrNone,  KFailedGLKPArea);
		CheckExpectedResult(reqStatus3.Int(), KErrNone,  KFailedGLKPArea);

		if (KErrNone == reqStatus2.Int())
			{
			CheckPositionAreaInfo(posInfo2, posAreaInfo2);
			}
		else
			{
			// feature is not supported
			ERR_PRINTF1(KFailedGetLastKnownPosition);
			SetTestStepResult(EFail);
			}
		

		if (KErrNone == reqStatus3.Int())
			{
			CheckPositionAreaInfo(posInfo3, posAreaInfo3);
			}
		else
			{
			// feature is not supported
			ERR_PRINTF1(KFailedGetLastKnownPosition);
			SetTestStepResult(EFail);
			}
		
		
		CleanupStack::Pop(5,&server1);

	    return TestStepResult();
	}


TVerdict CPositionerTestStep::TdLocSrv_Positioner_GetLKPosition_3Clients_CancelL(TInt aIndex)
	{

	SetTestCasePropertyL(KLocSrvTestSuite,KLbsLocMonitorTestLKPosKey,aIndex);


	    RPositionServer server1;
	    TInt err = server1.Connect();
	    CleanupClosePushL(server1);
	    
	    if(err != KErrNone)
	        {
	        ERR_PRINTF1(KFailedConnectServer);
	        SetTestStepResult(EFail);
	        return TestStepResult();
	        }

	    RPositioner positioner1;
	    User::LeaveIfError(positioner1.Open(server1));
	    CleanupClosePushL(positioner1);

	    RPositioner positioner2;
	    User::LeaveIfError(positioner2.Open(server1));
	    CleanupClosePushL(positioner2);

	    RPositionServer server2;
	    err = server2.Connect();
	    CleanupClosePushL(server2);
	    
	    if(err != KErrNone)
	        {
	        ERR_PRINTF1(KFailedConnectServer);
	        SetTestStepResult(EFail);
	        return TestStepResult();
	        }
	    RPositioner positioner3;
	    User::LeaveIfError(positioner3.Open(server2));
	    CleanupClosePushL(positioner3);
	    
	    TPositionInfo posInfo1;	TRequestStatus reqStatus1;
	    positioner1.GetLastKnownPosition(posInfo1, reqStatus1);
	    User::After(KGetLKPosMultipleReqDelay);
	    
	    TPositionInfo posInfo2;	TRequestStatus reqStatus2;
	    positioner2.GetLastKnownPosition(posInfo2, reqStatus2);
	    User::After(KGetLKPosMultipleReqDelay);
	    
	    TPositionInfo posInfo3;	TRequestStatus reqStatus3;
	    positioner3.GetLastKnownPosition(posInfo3, reqStatus3);
	    User::After(KGetLKPosReqPropagateDelay); 
		// Delay to ensure the GetLastKnownPosition request reaches the server before a cancel is sent
	   
	    TBool otherReqPending =( (reqStatus1.Int()==KRequestPending) && (reqStatus2.Int()==KRequestPending) && (reqStatus3.Int()==KRequestPending) );
	    _LIT(KIncorrectTestSetup,"Test timing fault");
	    if (!otherReqPending)
	    	{
	    	// Increase the delay in the test location monitor as we require the above 
	    	// requests to be outstanding when an emptylastknownpositionstore request 
	    	// is issued [the panic occurs when the above requests have been completed before
	    	// an emptylastknownpositionstore request ]
	    	User::Panic(KIncorrectTestSetup, KErrGeneral);
	    	}
		TInt cancelresult = positioner1.CancelRequest(RPositionerSubSessionBase::EReqGetLastKnownPosition);
		
	    User::WaitForRequest(reqStatus1);
	    User::WaitForRequest(reqStatus2);
	    User::WaitForRequest(reqStatus3);
	    
		CheckExpectedResult(cancelresult, KErrNone, KFailedCancelGLKPos);
		CheckExpectedResult(reqStatus1.Int(), KErrCancel,  KFailedGLKPosReqCancel);
		CheckExpectedResult(reqStatus2.Int(), KErrNone,  KFailedGLKPos);
		CheckExpectedResult(reqStatus3.Int(), KErrNone,  KFailedGLKPos);

		// Check if the received last known position in positioner2 is valid.
		if (KErrNone == reqStatus2.Int())
			{
			CheckPosition(posInfo2);
			}
		else
			{
			// feature is not supported
			ERR_PRINTF1(KFailedGetLastKnownPosition);
			SetTestStepResult(EFail);
			}
		
		// Check if the received last known position in positioner3 is valid.
		if (KErrNone == reqStatus3.Int())
			{
			CheckPosition(posInfo3);
			}
		else
			{
			// feature is not supported
			ERR_PRINTF1(KFailedGetLastKnownPosition);
			SetTestStepResult(EFail);
			}		

	    CleanupStack::Pop(5,&server1);

	    return TestStepResult();
	}


TVerdict CPositionerTestStep::TdLocSrv_Positioner_GetLKPArea_And_ClearDB_3ClientsL(TInt aIndex)
	{

	SetTestCasePropertyL(KLocSrvTestSuite,KLbsLocMonitorTestLKPAreaKey,aIndex);


	    RPositionServer server1;
	    TInt err = server1.Connect();
	    CleanupClosePushL(server1);
	    
	    if(err != KErrNone)
	        {
	        ERR_PRINTF1(KFailedConnectServer);
	        SetTestStepResult(EFail);
	        return TestStepResult();
	        }

	    RPositioner positioner1;
	    User::LeaveIfError(positioner1.Open(server1));
	    CleanupClosePushL(positioner1);

	    RPositioner positioner2;
	    User::LeaveIfError(positioner2.Open(server1));
	    CleanupClosePushL(positioner2);

	    RPositionServer server2;
	    err = server2.Connect();
	    CleanupClosePushL(server2);
	    
	    if(err != KErrNone)
	        {
	        ERR_PRINTF1(KFailedConnectServer);
	        SetTestStepResult(EFail);
	        return TestStepResult();
	        }
	    RPositioner positioner3;
	    User::LeaveIfError(positioner3.Open(server2));
	    CleanupClosePushL(positioner3);
	    
	    TPositionInfo posInfo1;	TPositionAreaInfo posAreaInfo1;	TRequestStatus reqStatus1;
	    positioner1.GetLastKnownPositionArea(posInfo1, posAreaInfo1, reqStatus1);
	    User::After(KGetLKPAMultipleReqDelay);
	    
	    TPositionInfo posInfo2;	TPositionAreaInfo posAreaInfo2;	TRequestStatus reqStatus2;
	    positioner2.GetLastKnownPositionArea(posInfo2, posAreaInfo2, reqStatus2);
	    User::After(KGetLKPAMultipleReqDelay);
	    
	    TPositionInfo posInfo3;	TPositionAreaInfo posAreaInfo3;	TRequestStatus reqStatus3;
	    positioner3.GetLastKnownPositionArea(posInfo3, posAreaInfo3, reqStatus3);
	    User::After(KGetLKPAReqPropagateDelay);
	   
	   
	    TBool otherReqPending =( (reqStatus1.Int()==KRequestPending) && (reqStatus2.Int()==KRequestPending) && (reqStatus3.Int()==KRequestPending) );
	    _LIT(KIncorrectTestSetup,"Test timing fault");
	    if (!otherReqPending)
	    	{
	    	// Increase the delay in the test location monitor as we require the above 
	    	// requests to be outstanding when an emptylastknownpositionstore request 
	    	// is issued [the panic occurs when the above requests have been completed before
	    	// an emptylastknownpositionstore request ]
	    	User::Panic(KIncorrectTestSetup, KErrGeneral);
	    	}
	    
	    TRequestStatus reqStatus4;
	    server2.EmptyLastKnownPositionStore(reqStatus4);
	    
	    User::WaitForRequest(reqStatus1);
	    User::WaitForRequest(reqStatus2);
	    User::WaitForRequest(reqStatus3);
	    User::WaitForRequest(reqStatus4);

	    
		CheckExpectedResult(reqStatus1.Int(), KErrCancel,  KFailedGLKPAreaReqCancelDBCleared);
		CheckExpectedResult(reqStatus2.Int(), KErrCancel,  KFailedGLKPAreaReqCancelDBCleared);
		CheckExpectedResult(reqStatus3.Int(), KErrCancel,  KFailedGLKPAreaReqCancelDBCleared);
		CheckExpectedResult(reqStatus4.Int(), KErrNone,  KFailedClearDB);
		
	    CleanupStack::PopAndDestroy(5,&server1);

	    return TestStepResult();
	}

TVerdict CPositionerTestStep::TdLocSrv_Positioner_GetLKPosition_And_ClearDB_3ClientsL(TInt aIndex)
	{

	SetTestCasePropertyL(KLocSrvTestSuite,KLbsLocMonitorTestLKPosKey,aIndex);

	    RPositionServer server1;
	    TInt err = server1.Connect();
	    CleanupClosePushL(server1);
	    
	    if(err != KErrNone)
	        {
	        ERR_PRINTF1(KFailedConnectServer);
	        SetTestStepResult(EFail);
	        return TestStepResult();
	        }

	    RPositioner positioner1;
	    User::LeaveIfError(positioner1.Open(server1));
	    CleanupClosePushL(positioner1);

	    RPositioner positioner2;
	    User::LeaveIfError(positioner2.Open(server1));
	    CleanupClosePushL(positioner2);

	    RPositionServer server2;
	    err = server2.Connect();
	    CleanupClosePushL(server2);
	    
	    if(err != KErrNone)
	        {
	        ERR_PRINTF1(KFailedConnectServer);
	        SetTestStepResult(EFail);
	        return TestStepResult();
	        }
	    RPositioner positioner3;
	    User::LeaveIfError(positioner3.Open(server2));
	    CleanupClosePushL(positioner3);
	    
	    TPositionInfo posInfo1;	TRequestStatus reqStatus1;
	    positioner1.GetLastKnownPosition(posInfo1,reqStatus1);
	    User::After(KGetLKPosMultipleReqDelay);
	    
	    TPositionInfo posInfo2;	TRequestStatus reqStatus2;
	    positioner2.GetLastKnownPosition(posInfo2,reqStatus2);
	    User::After(KGetLKPosMultipleReqDelay);
	    
	    TPositionInfo posInfo3;	TRequestStatus reqStatus3;
	    positioner3.GetLastKnownPosition(posInfo3, reqStatus3);
	    User::After(KGetLKPosReqPropagateDelay);
	   
	    TBool otherReqPending =( (reqStatus1.Int()==KRequestPending) && (reqStatus2.Int()==KRequestPending) && (reqStatus3.Int()==KRequestPending) );
	    _LIT(KIncorrectTestSetup,"Test timing fault");
	    if (!otherReqPending)
	    	{
	    	// Increase the delay in the test location monitor as we require the above 
	    	// requests to be outstanding when an emptylastknownpositionstore request 
	    	// is issued [the panic occurs when the above requests have been completed before
	    	// an emptylastknownpositionstore request ]
	    	User::Panic(KIncorrectTestSetup, KErrGeneral);
	    	}
	    TRequestStatus reqStatus4;
	    server2.EmptyLastKnownPositionStore(reqStatus4);
	    
	    User::WaitForRequest(reqStatus1);
	    User::WaitForRequest(reqStatus2);
	    User::WaitForRequest(reqStatus3);
	    User::WaitForRequest(reqStatus4);

	    
		CheckExpectedResult(reqStatus1.Int(), KErrCancel,  KFailedGLKPosReqCancelDBCleared);
		CheckExpectedResult(reqStatus2.Int(), KErrCancel,  KFailedGLKPosReqCancelDBCleared);
		CheckExpectedResult(reqStatus3.Int(), KErrCancel,  KFailedGLKPosReqCancelDBCleared);
		CheckExpectedResult(reqStatus4.Int(), KErrNone,  KFailedClearDB);
		
	    CleanupStack::PopAndDestroy(5,&server1);

	    return TestStepResult();
	}


TVerdict CPositionerTestStep::TdLocSrv_Positioner_GetLKPosition_AndLKPArea_3ClientsL(TInt aIndex)
	{

	SetTestCasePropertyL(KLocSrvTestSuite,KLbsLocMonitorTestLKPAreaKey,aIndex);

	    RPositionServer server1;
	    TInt err = server1.Connect();
	    CleanupClosePushL(server1);
	    
	    if(err != KErrNone)
	        {
	        ERR_PRINTF1(KFailedConnectServer);
	        SetTestStepResult(EFail);
	        return TestStepResult();
	        }

	    RPositioner positioner1;
	    User::LeaveIfError(positioner1.Open(server1));
	    CleanupClosePushL(positioner1);

	    RPositioner positioner2;
	    User::LeaveIfError(positioner2.Open(server1));
	    CleanupClosePushL(positioner2);

	    RPositionServer server2;
	    err = server2.Connect();
	    CleanupClosePushL(server2);
	    
	    if(err != KErrNone)
	        {
	        ERR_PRINTF1(KFailedConnectServer);
	        SetTestStepResult(EFail);
	        return TestStepResult();
	        }
	    RPositioner positioner3;
	    User::LeaveIfError(positioner3.Open(server2));
	    CleanupClosePushL(positioner3);
	    
	    TPositionInfo posInfo1;	TRequestStatus reqStatus1;
	    positioner1.GetLastKnownPosition(posInfo1,reqStatus1);
	    User::After(KGetLKPosMultipleReqDelay);
	    
	    TPositionInfo posInfo2;	TRequestStatus reqStatus2;
	    positioner2.GetLastKnownPosition(posInfo2,reqStatus2);
	    User::After(KGetLKPosMultipleReqDelay);
	    
	    TPositionInfo posInfo3;	TPositionAreaInfo areaInfo3; TRequestStatus reqStatus3;
	    positioner3.GetLastKnownPositionArea(posInfo3, areaInfo3, reqStatus3);
	    User::After(KGetLKPAReqPropagateDelay);
	   
    
	    User::WaitForRequest(reqStatus1);
	    User::WaitForRequest(reqStatus2);
	    User::WaitForRequest(reqStatus3);
	    
		CheckExpectedResult(reqStatus1.Int(), KErrNone,  KFailedGLKPos);
		CheckExpectedResult(reqStatus2.Int(), KErrNone,  KFailedGLKPos);
		CheckExpectedResult(reqStatus3.Int(), KErrNone,  KFailedGLKPArea);
		
		
		// Check if the received last known position in positioner1 is valid.
		if (KErrNone == reqStatus1.Int())
			{
			CheckPosition(posInfo1);
			}
		else
			{
			// feature is not supported
			ERR_PRINTF1(KFailedGetLastKnownPosition);
			SetTestStepResult(EFail);
			}	
		
		// Check if the received last known position in positioner2 is valid.
		if (KErrNone == reqStatus2.Int())
			{
			CheckPosition(posInfo2);
			}
		else
			{
			// feature is not supported
			ERR_PRINTF1(KFailedGetLastKnownPosition);
			SetTestStepResult(EFail);
			}	

		// Check if the received last known position in positioner3 is valid.
		if (KErrNone == reqStatus3.Int())
				{
				CheckPositionAreaInfo(posInfo3, areaInfo3);
				}
			else
				{
				// feature is not supported
				ERR_PRINTF1(KFailedGetLastKnownPosition);
				SetTestStepResult(EFail);
				}
		
	    CleanupStack::PopAndDestroy(5,&server1);
	    return TestStepResult();
	}


TVerdict CPositionerTestStep::TdLocSrv_Positioner_GetLKPosition_LKPArea_AndClearDB_3ClientsL(TInt aIndex)
	{

	SetTestCasePropertyL(KLocSrvTestSuite,KLbsLocMonitorTestLKPAreaKey,aIndex);

	__UHEAP_MARK;
	
	    RPositionServer server1;
	    TInt err = server1.Connect();
	    CleanupClosePushL(server1);
	    
	    if(err != KErrNone)
	        {
	        ERR_PRINTF1(KFailedConnectServer);
	        SetTestStepResult(EFail);
	        return TestStepResult();
	        }

	    RPositioner positioner1;
	    User::LeaveIfError(positioner1.Open(server1));
	    CleanupClosePushL(positioner1);

	    RPositioner positioner2;
	    User::LeaveIfError(positioner2.Open(server1));
	    CleanupClosePushL(positioner2);

	    RPositionServer server2;
	    err = server2.Connect();
	    CleanupClosePushL(server2);
	    
	    if(err != KErrNone)
	        {
	        ERR_PRINTF1(KFailedConnectServer);
	        SetTestStepResult(EFail);
	        return TestStepResult();
	        }
	    RPositioner positioner3;
	    User::LeaveIfError(positioner3.Open(server2));
	    CleanupClosePushL(positioner3);
	    
	    TPositionInfo posInfo1;	TRequestStatus reqStatus1;
	    positioner1.GetLastKnownPosition(posInfo1,reqStatus1);
	    User::After(KGetLKPosMultipleReqDelay);
	    
	    TPositionInfo posInfo2;	TRequestStatus reqStatus2;
	    positioner2.GetLastKnownPosition(posInfo2,reqStatus2);
	    User::After(KGetLKPosMultipleReqDelay);
	    
	    TPositionInfo posInfo3;	TPositionAreaInfo areaInfo3; TRequestStatus reqStatus3;
	    positioner3.GetLastKnownPositionArea(posInfo3, areaInfo3, reqStatus3);
	    User::After(KGetLKPAMultipleReqDelay);
	   
	    TBool otherReqPending = ((reqStatus1.Int()==KRequestPending)&&(reqStatus2.Int()==KRequestPending)&&(reqStatus3.Int()==KRequestPending));
	    _LIT(KIncorrectTestSetup,"Test timing fault");
	    if (!otherReqPending)
	    	{
	    	// Increase the delay in the test location monitor as we require the above 
	    	// requests to be outstanding when an emptylastknownpositionstore request 
	    	// is issued [the panic occurs when the above requests have been completed before
	    	// an emptylastknownpositionstore request ]
	    	User::Panic(KIncorrectTestSetup, KErrGeneral);
	    	}
	    
	    TRequestStatus reqStatus4;
	    server2.EmptyLastKnownPositionStore(reqStatus4);
	    
	    User::WaitForRequest(reqStatus1);
	    User::WaitForRequest(reqStatus2);
	    User::WaitForRequest(reqStatus3);
	    User::WaitForRequest(reqStatus4);

		CheckExpectedResult(reqStatus1.Int(), KErrCancel,  KFailedGLKPosReqCancelDBCleared);
		CheckExpectedResult(reqStatus2.Int(), KErrCancel,  KFailedGLKPosReqCancelDBCleared);
		CheckExpectedResult(reqStatus3.Int(), KErrCancel,  KFailedGLKPosReqCancelDBCleared);
		CheckExpectedResult(reqStatus4.Int(), KErrNone,    KFailedClearDB);
		
	    CleanupStack::PopAndDestroy(5,&server1);

	    __UHEAP_MARKEND;
	    
	    return TestStepResult();
	}



TVerdict CPositionerTestStep::TdLocSrv_Positioner_GetLKPArea_NotFoundL(TInt aIndex)
	{

	SetTestCasePropertyL(KLocSrvTestSuite,KLbsLocMonitorTestLKPAreaKey,aIndex);
	
	// Check the correct message is always sent to the server upon a call
	// to this function and that the return value is always that of the
	// completed message.

	RPositionServer server;
	User::LeaveIfError(server.Connect());
	CleanupClosePushL(server);

	RPositioner positioner;
	TInt err = positioner.Open(server);
	if(KErrNone != err)
	{
	ERR_PRINTF1(KFailedOpenPositioner);
	SetTestStepResult(EFail);
	return TestStepResult();
	}
	CleanupClosePushL(positioner);	

	TPositionInfo posInfo;
	TPositionAreaInfo posAreaInfo;
	TRequestStatus reqStatus;

	positioner.GetLastKnownPositionArea(posInfo, posAreaInfo, reqStatus);
	User::WaitForRequest(reqStatus);
	
	if (KErrNotFound != reqStatus.Int())
		{
		// feature is not supported
		ERR_PRINTF1(KFailedGetLastKnownPosition);
		SetTestStepResult(EFail);
		}
	CleanupStack::PopAndDestroy(2, &server);
	return TestStepResult();
	
	}



TVerdict CPositionerTestStep::TdLocSrv_Positioner_GetLKPArea_TimedOutL(TInt aIndex)
	{

	SetTestCasePropertyL(KLocSrvTestSuite,KLbsLocMonitorTestLKPAreaKey,aIndex);
	
	// Check the correct message is always sent to the server upon a call
	// to this function and that the return value is always that of the
	// completed message.

	RPositionServer server;
	User::LeaveIfError(server.Connect());
	CleanupClosePushL(server);

	RPositioner positioner;
	TInt err = positioner.Open(server);
	if(KErrNone != err)
	{
	ERR_PRINTF1(KFailedOpenPositioner);
	SetTestStepResult(EFail);
	return TestStepResult();
	}
	CleanupClosePushL(positioner);	

	TPositionInfo posInfo;
	TPositionAreaInfo posAreaInfo;
	TRequestStatus reqStatus;

	positioner.GetLastKnownPositionArea(posInfo, posAreaInfo, reqStatus);
	User::WaitForRequest(reqStatus);
	
	CheckExpectedResult(reqStatus.Int(), KErrTimedOut, KWrongErrorCode);
	
	CleanupStack::PopAndDestroy(2, &server);
	return TestStepResult();
	
	}


TVerdict CPositionerTestStep::TdLocSrv_Positioner_GetLKPosition_NotFoundL(TInt aIndex)
	{

	SetTestCasePropertyL(KLocSrvTestSuite,KLbsLocMonitorTestLKPosKey,aIndex);
		
		// Check the correct message is always sent to the server upon a call
		// to this function and that the return value is always that of the
		// completed message.

		RPositionServer server;
		User::LeaveIfError(server.Connect());
		CleanupClosePushL(server);

		RPositioner positioner;
		TInt err = positioner.Open(server);
		if(KErrNone != err)
		{
		ERR_PRINTF1(KFailedOpenPositioner);
		SetTestStepResult(EFail);
		return TestStepResult();
		}
		CleanupClosePushL(positioner);	

		TPositionInfo posInfo;
		TRequestStatus reqStatus;

		positioner.GetLastKnownPosition(posInfo, reqStatus);
		User::WaitForRequest(reqStatus);
		
		if (KErrNotFound != reqStatus.Int())
			{
			// feature is not supported
			ERR_PRINTF1(KFailedGetLastKnownPosition);
			SetTestStepResult(EFail);
			}
		
		CleanupStack::PopAndDestroy(2, &server);
		return TestStepResult();
	}



TVerdict CPositionerTestStep::TdLocSrv_Positioner_GetLKPosition_TimedOutL(TInt aIndex)
	{

	SetTestCasePropertyL(KLocSrvTestSuite,KLbsLocMonitorTestLKPosKey,aIndex);
		
		// Check the correct message is always sent to the server upon a call
		// to this function and that the return value is always that of the
		// completed message.

		RPositionServer server;
		User::LeaveIfError(server.Connect());
		CleanupClosePushL(server);

		RPositioner positioner;
		TInt err = positioner.Open(server);
		if(KErrNone != err)
		{
		ERR_PRINTF1(KFailedOpenPositioner);
		SetTestStepResult(EFail);
		return TestStepResult();
		}
		CleanupClosePushL(positioner);	

		TPositionInfo posInfo;
		TRequestStatus reqStatus;

		positioner.GetLastKnownPosition(posInfo, reqStatus);
		User::WaitForRequest(reqStatus);
		
		CheckExpectedResult(reqStatus.Int(), KErrTimedOut, KWrongErrorCode);
		
		CleanupStack::PopAndDestroy(2, &server);
		return TestStepResult();
	}



