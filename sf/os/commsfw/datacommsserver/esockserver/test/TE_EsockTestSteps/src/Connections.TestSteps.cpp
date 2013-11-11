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
//

/**
 @file Connections.TestSteps.cpp
*/

#include "SubConnections.TestSteps.h"
#include "Connections.TestSteps.h"
#include "SocketServer.TestSteps.h"
#include "Sockets.TestSteps.h"
#include <cdbcols.h>
#include <commsdattypeinfov1_1.h>
#include <comms-infras/es_commsdataobject.h>
#include <comms-infras/connectionqueryset.h>


#ifdef _DEBUG
// Panic category for "absolutely impossible!" vanilla ASSERT()-type panics from this module
// (if it could happen through user error then you should give it an explicit, documented, category + code)
_LIT(KSpecAssert_ESockTestCnctnsT, "ESockTestCnctnsT");
#endif


// Create Connection
//------------------

CCreateRConnectionStep::CCreateRConnectionStep(CCEsockTestBase*& aEsockTest)
:   CTe_EsockStepBase(aEsockTest)
	{
	SetTestStepName(KCreateRConnectionStep);
	}

TVerdict CCreateRConnectionStep::doTestStepPreambleL()
	{
	SetTestStepResult(EFail);

	if (iEsockTest==NULL)
	    iEsockTest = new (ELeave) CCEsockTestBase;

	SetTestStepResult(EPass);
	return TestStepResult();
	}

TInt CCreateRConnectionStep::ConfigureFromIni()
	{
	iParams.Reset();

	// Read in appropriate fields
	if((GetStringFromConfig(iSection, KTe_ConnectionName, iParams.iConnectionName) != 1)
		|| (iParams.iConnectionName.Length() == 0))
		{
		INFO_PRINTF1(_L("Couldn't find appropriate field in config file"));
		return KErrNotFound;
		}

    // All ok if we got this far
    return KErrNone;
	}

TVerdict CCreateRConnectionStep::doSingleTestStep()
	{
    TInt error = iEsockTest->CreateConnection(iParams.iConnectionName);
    if (error!=KErrNone)
        {
        INFO_PRINTF2(_L("Could not create connection (%S)."),&iParams.iConnectionName);
        INFO_PRINTF2(_L("Error: %d."),error);
		SetTestStepResult(EFail);
        }
	return TestStepResult();
	}


// Open Connection
//----------------

COpenRConnectionStep::COpenRConnectionStep(CCEsockTestBase*& aEsockTest)
:   CTe_EsockStepBase(aEsockTest)
	{
	SetTestStepName(KOpenRConnectionStep);
	}

TInt COpenRConnectionStep::ConfigureFromIni()
	{
	iParams.Reset();

	// Read in appropriate fields
	if((GetStringFromConfig(iSection, KTe_ConnectionName, iParams.iConnectionName) != 1)
		|| (iParams.iConnectionName.Length() == 0))
		{
		INFO_PRINTF1(_L("Couldn't find appropriate field in config file"));
		return KErrNotFound;
		}

    if (GetStringFromConfig(iSection, KTe_SocketServName, iParams.iSockServName) != 1)
        {
        INFO_PRINTF2(_L("%S: Socket server name missing."), &iParams.iConnectionName);
        return EFalse;
        }

    // Optional
    // Reads the protocol family to use
    if (!GetIntFromConfig(iSection, KTe_ConnectionType, iParams.iConnectionType))
        {
        iParams.iConnectionType = -1;
        }

    // All ok if we got this far
    return KErrNone;
	}

TVerdict COpenRConnectionStep::doSingleTestStep()
	{
    TInt error = iEsockTest->OpenConnection(iParams);
    if (error!=KErrNone)
        {
        INFO_PRINTF2(_L("Could not open connection (%S)."),&iParams.iConnectionName);
        INFO_PRINTF2(_L("Error: %d."),error);
        return EFail;
        }
	return EPass;
	}


// Start Connection
//-----------------

CStartRConnectionStep::CStartRConnectionStep(CCEsockTestBase*& aEsockTest)
:   CTe_EsockStepBase(aEsockTest)
	{
	SetTestStepName(KStartRConnectionStep);
	}

TInt CStartRConnectionStep::ConfigureFromIni()
	{
	iParams.Reset();
	TPtrC temp;

	// Read in appropriate fields
	if((GetStringFromConfig(iSection, KTe_ConnectionName, iParams.iConnectionName) != 1)
		|| (iParams.iConnectionName.Length() == 0))
		{
		INFO_PRINTF1(_L("Couldn't find appropriate field in config file"));
		return KErrNotFound;
		}
	
    GetStringFromConfig(iSection, KTe_SocketServName, iParams.iSockServName);
    // Optional
    // Reads the protocol family to use
    if (!GetIntFromConfig(iSection, KTe_ConnectionType, iParams.iConnectionType))
        {
          iParams.iConnectionType = -1;
        }

	TPtrC16 bearerSet;
	TBool bearerPresent = ((GetStringFromConfig(iSection,KTe_BearerName,bearerSet)==1)
		&& (bearerSet.Length()>0));

	TBool IAPpresent = (GetIntFromConfig(iSection,KTe_IAPIndexName,iParams.iIAP)==1);
	TBool SNAPpresent = (GetIntFromConfig(iSection,KTe_SNAPIndexName,iParams.iSNAP)==1);
	TBool ConnPrefPresent = (GetIntFromConfig(iSection,KTe_ConnPrefListIndexName,iParams.iSNAP)==1);
	TBool DummyPresent = (GetIntFromConfig(iSection,KTe_DummyPrefIndexName,iParams.iDummy)==1);
	TBool AsynchPresent = (GetStringFromConfig(iSection,KTe_AsynchIndexName,temp)==1);

	iParams.iStartWithOldPreferences = IAPpresent;
	iParams.iStartWithSnapPreferences = SNAPpresent;
	iParams.iStartWithConPrefList = ConnPrefPresent;
	iParams.iStartWithDummy = DummyPresent;
	iParams.iAsynch = AsynchPresent;

	GetBoolFromConfig(iSection, KTe_AutoStartPrefName, iParams.iStartAuto);

	if (iParams.iStartWithOldPreferences && iParams.iStartWithSnapPreferences)
		{
		INFO_PRINTF2(_L("Connection preferences for (%S) have both snap and IAP."),&iParams.iConnectionName);
		return KErrNotFound;
		}

	//with preferences?
	if (iParams.iStartWithOldPreferences)
		{
		if(!bearerPresent)
			{
			iParams.iBearer = KCommDbBearerUnknown;
			}
		else
			{
			if (bearerSet.Compare(KTe_CommDbBearerCSD)==0)
				{ iParams.iBearer = KCommDbBearerCSD; }
			else if (bearerSet.Compare(KTe_CommDbBearerWcdma)==0)
				{ iParams.iBearer = KCommDbBearerWcdma; }
			else if (bearerSet.Compare(KTe_CommDbBearerLAN)==0)
				{ iParams.iBearer = KCommDbBearerLAN; }
			else
				{
				INFO_PRINTF3(_L("%S: Bearer type (%S) not recognised."),&iParams.iConnectionName,&bearerSet);
				return KErrNotFound;
				}
			}
		}

	// What error are we expecting?
	if (!GetIntFromConfig(iSection, KExpectedError,iExpectedError))
		{
		iExpectedError = KExpectedErrorNotUsed;
		}
	else
		{
		INFO_PRINTF2(_L("Error to be expected: %d"),iExpectedError);
		}

    // All ok if we got this far
    return KErrNone;
	}

TVerdict CStartRConnectionStep::doSingleTestStep()
	{
	// Default to failing
	SetTestStepResult(EFail);

	TInt ret = iEsockTest->StartConnection(iParams);

	// Log any error found
	if(iExpectedError == KExpectedErrorNotUsed)
		{
		SetTestStepError(ret);

		if(ret == KErrNone)
			{
			SetTestStepResult(EPass);
			}
		else
			{
			INFO_PRINTF2(_L("Could not start connection (%S)."), &iParams.iConnectionName);
			INFO_PRINTF2(_L("Error: %d"), ret);
			}
		}
	else
		{
		// We are expecting a special kind of error
		INFO_PRINTF3(_L("Expected error used. expected:%d, observed:%d"), iExpectedError, ret);
		SetTestStepResult((ret == iExpectedError) ? EPass : EFail);
		SetTestStepError((ret == iExpectedError) ? KErrNone : ret);
		}

	return TestStepResult();
	}









// CStartStopCrazyLoopRConnectionStep
//-----------------

CStartStopCrazyLoopRConnectionStep::CStartStopCrazyLoopRConnectionStep(CCEsockTestBase*& aEsockTest)
:   CStartRConnectionStep(aEsockTest)
    {
    SetTestStepName(KStartStopCrazyLoopRConnectionStep);
    }

TInt CStartStopCrazyLoopRConnectionStep::CalibrateStart()
    {
    TTime timeBegin;
    TTime timeEnd;

    TRequestStatus* pConnectionStartStatus = iEsockTest->iRequestStatuses.Find(iParams.iConnectionName);
    if (pConnectionStartStatus == NULL)
        {
        return KErrCorrupt;
        }
    
    if (iEsockTest->OpenConnection(iParams) != KErrNone)
        {
        INFO_PRINTF1(_L("Can't reopen connection, most likely you didn't supply the session name"));
        return KErrCorrupt;
        }
    
    timeBegin.HomeTime();
    TInt error = iEsockTest->StartConnection(iParams);
    if (error != KErrNone)
        {
        return error;
        }
    User::WaitForRequest(*pConnectionStartStatus);
    if (pConnectionStartStatus->Int() != KErrNone)
        {
        return pConnectionStartStatus->Int();
        }    
    timeEnd.HomeTime();
    iEsockTest->StopConnection(iParams);
    iEsockTest->CloseConnection(iParams.iConnectionName);
    
    return timeEnd.MicroSecondsFrom(timeBegin).Int64();
    }

CStartStopCrazyLoopRConnectionStep::~CStartStopCrazyLoopRConnectionStep()
    {
    iTimer.Close();
    }

TVerdict CStartStopCrazyLoopRConnectionStep::doSingleTestStep()
    {
    // Default to failing
    SetTestStepResult(EFail);
    iParams.iAsynch = ETrue; //force async.
    TInt error = iTimer.CreateLocal();
    if (error!=KErrNone)
        {
        INFO_PRINTF2(_L("Creating RTimer object failed with %d"), error);
        return TestStepResult();
        }
    
    ipConnectionStartStatus = new TRequestStatus;
    if (ipConnectionStartStatus == NULL)
        {
        INFO_PRINTF1(_L("Heap allocation for TRequestStatus failed"));
        return TestStepResult();
        }
    error = iEsockTest->iRequestStatuses.Add(ipConnectionStartStatus, iParams.iConnectionName); //And will be owned here
    if (error!=KErrNone)
        {
        INFO_PRINTF2(_L("Failed "), error);
        return TestStepResult();
        }
    
    INFO_PRINTF1(_L("Calibrating timer by running full start"));
    //Well, we're running the full start twice as the first run is likely to take more than the representative time.
    TInt timerIncrement = Min<TInt>(CalibrateStart(), CalibrateStart());
    if (timerIncrement < 0)
        {
        INFO_PRINTF2(_L("Full start failed, unable to calibrate, test failed with %d"), timerIncrement);
        return TestStepResult();
        }
    INFO_PRINTF3(_L("Start took %dus, will divide by %d and use as the increment"), timerIncrement, EPlannedLoops);
    
    timerIncrement /= EPlannedLoops;

    
    /*=======================================================
     * The loop
     */
    TInt timerVal = 1;
    TInt i = 0;
    INFO_PRINTF1(_L("Beginning crazy loop"));
    while (ETrue)
        {
        if (++i > (2 * EPlannedLoops))
            {
            INFO_PRINTF1(_L("The test has been executing far too many iterations than can be reasonably expected, aborting.."));
            return TestStepResult();
            }
        INFO_PRINTF2(_L("[Loop %d], Starting a new loop=============================="), i);
        INFO_PRINTF2(_L("***********Re-openning connection"), i);
        if (iEsockTest->OpenConnection(iParams) != KErrNone)
            {
            INFO_PRINTF1(_L("Can't reopen connection, most likely you didn't supply the session name"));
            return TestStepResult();
            }
        
        error = doLoopStep(timerVal);
        if ( error == KErrUnderflow )
            {
            if ( i < EMandatoryLoops )
                {
                INFO_PRINTF1(_L("The test didn't execute enough iterations and hence hasn't tested what needed to be tested"));
                return TestStepResult();
                }
            break; //The only EPass exit condition
            }
        else if ( error != KErrNone )
            {
            INFO_PRINTF2(_L("Loop test has failed with %d"), error);
            return TestStepResult();
            }
        timerVal += timerIncrement;
        
        INFO_PRINTF2(_L("***********Closing connection"), i);
        iEsockTest->CloseConnection(iParams.iConnectionName);
        }

    SetTestStepResult(EPass);
    return TestStepResult();
    }

TInt CStartStopCrazyLoopRConnectionStep::doLoopStep(TInt aTimerVal)
    {
    TRequestStatus timerRequestStatus;
    
    INFO_PRINTF1(_L("***********Starting connection (asynch)"));  
    TInt error = iEsockTest->StartConnection(iParams);
    if (error != KErrNone)
        {
        INFO_PRINTF1(_L("Starting connection failed, aborting"));
        return error;
        }
    INFO_PRINTF2(_L("***********Setting timer to %dus .zzz...."), aTimerVal);
    iTimer.After(timerRequestStatus,aTimerVal);
    User::WaitForRequest(timerRequestStatus, *ipConnectionStartStatus);
    if (timerRequestStatus.Int() == KRequestPending)
        {
        INFO_PRINTF2(_L("Connection Start completed with %d and before the timer"), ipConnectionStartStatus->Int());
        iTimer.Cancel();
        User::WaitForRequest(timerRequestStatus);
        return ipConnectionStartStatus->Int() == KErrNone ? KErrUnderflow : ipConnectionStartStatus->Int(); //Whether this is fatal for the test or not, will be determined by the caller;
        }
    
    INFO_PRINTF1(_L("*********** ....zzz. Stopping connection"));
    iEsockTest->StopConnection(iParams);
    User::WaitForRequest(*ipConnectionStartStatus);
    INFO_PRINTF1(_L("***********Connection stopped")); 
    return KErrNone;
    }


// CStartCloseCrazyLoopRConnectionStep
//-----------------

CStartCloseCrazyLoopRConnectionStep::CStartCloseCrazyLoopRConnectionStep(CCEsockTestBase*& aEsockTest)
:   CStartStopCrazyLoopRConnectionStep(aEsockTest)
    {
    SetTestStepName(KStartCloseCrazyLoopRConnectionStep);
    }

TInt CStartCloseCrazyLoopRConnectionStep::doLoopStep(TInt aTimerVal)
    {
    TRequestStatus timerRequestStatus;
    
    INFO_PRINTF1(_L("***********Starting connection (asynch)"));  
    TInt error = iEsockTest->StartConnection(iParams);
    if (error != KErrNone)
        {
        INFO_PRINTF1(_L("Starting connection failed, aborting"));
        return error;
        }
    INFO_PRINTF2(_L("***********Setting timer to %dus .zzz...."), aTimerVal);
    iTimer.After(timerRequestStatus,aTimerVal);
    User::WaitForRequest(timerRequestStatus, *ipConnectionStartStatus);
    if (timerRequestStatus.Int() == KRequestPending)
        {
        INFO_PRINTF2(_L("Connection Start completed with %d and before the timer"), ipConnectionStartStatus->Int());
        iTimer.Cancel();
        User::WaitForRequest(timerRequestStatus);
        return ipConnectionStartStatus->Int() == KErrNone ? KErrUnderflow : ipConnectionStartStatus->Int(); //Whether this is fatal for the test or not, will be determined by the caller;
        }
    return KErrNone;
    }




// Stop Connection
//----------------

CStopRConnectionStep::CStopRConnectionStep(CCEsockTestBase*& aEsockTest)
:   CTe_EsockStepBase(aEsockTest)
	{
	SetTestStepName(KStopRConnectionStep);
	}

TInt CStopRConnectionStep::ConfigureFromIni()
	{
	iParams.Reset();

	// Read in appropriate fields
	if((GetStringFromConfig(iSection, KTe_ConnectionName, iParams.iConnectionName) != 1)
		|| (iParams.iConnectionName.Length() == 0))
		{
		INFO_PRINTF1(_L("Couldn't find appropriate field in config file"));
		return KErrNotFound;
		}

    TPtrC shutdownType;
    if (GetStringFromConfig(iSection,KTe_ConnectionStopTypeName,shutdownType)!=1)
        {
        INFO_PRINTF2(_L("%S: Stop type missing."),&iParams.iConnectionName);
        return KErrNotFound;
        }

    if (shutdownType.Compare(KTe_ConnStopNormal)==0)
	    { iParams.iConnStopType = RConnection::EStopNormal; }
    else if (shutdownType.Compare(KTe_ConnStopAuthoritative)==0)
	    { iParams.iConnStopType = RConnection::EStopAuthoritative; }
    else
        {
        INFO_PRINTF3(_L("%S: Stop type (%S) not recognised."),&iParams.iConnectionName,&shutdownType);
        return KErrNotFound;
        }

    // All ok if we got this far
    return KErrNone;
	}

TVerdict CStopRConnectionStep::doSingleTestStep()
	{
        TInt error = iEsockTest->StopConnection(iParams);
        if (error!=KErrNone)
            {
            INFO_PRINTF2(_L("Could not stop connection (%S)."),&iParams.iConnectionName);
            INFO_PRINTF2(_L("Error: %d."),error);
            return EFail;
            }
	return EPass;
	}


// Close Connection
//-----------------

CCloseRConnectionStep::CCloseRConnectionStep(CCEsockTestBase*& aEsockTest)
:   CTe_EsockStepBase(aEsockTest)
	{
	SetTestStepName(KCloseRConnectionStep);
	}

TInt CCloseRConnectionStep::ConfigureFromIni()
	{
	if((GetStringFromConfig(iSection, KTe_ConnectionName, iParams.iConnectionName) != 1)
		|| (iParams.iConnectionName.Length() == 0))
		{
		INFO_PRINTF1(_L("Couldn't find appropriate field in config file"));
		return KErrNotFound;
		}

    // All ok if we got this far
    return KErrNone;
	}

TVerdict CCloseRConnectionStep::doSingleTestStep()
	{
    TInt error = iEsockTest->CloseConnection(iParams.iConnectionName);
    if (error!=KErrNone)
        {
        INFO_PRINTF2(_L("Could not close connection (%S)."),&iParams.iConnectionName);
        INFO_PRINTF2(_L("Error: %d."),error);
		return EFail;
		}
	return EPass;
	}



// Wait for Incoming Connection
//-----------------------------

CWaitForIncomingConnectionStep::CWaitForIncomingConnectionStep(CCEsockTestBase*& aEsockTest)
:   CTe_EsockStepBase(aEsockTest)
	{
	SetTestStepName(KWaitForIncomingConnectionStep);
	}

TInt CWaitForIncomingConnectionStep::ConfigureFromIni()
    {
	//clean parameters from previous runs
	iParams.Reset();

    //try reading the next socket's name
    if (GetStringFromConfig(iSection,KTe_SubConnectionName,iParams.iSubConnectionName)!=1
        || iParams.iSubConnectionName.Length()==0)
            return KErrNotFound;

    if (GetStringFromConfig(iSection,KTe_ConnectionName,iParams.iConnectionName)!=1)
        {
        INFO_PRINTF2(_L("%S: Connection name missing."),&iParams.iConnectionName);
        return KErrNotFound;
        }

    //optional
    GetStringFromConfig(iSection,KTe_ConnectionStatus,subConnStatus);
    GetStringFromConfig(iSection,KTe_IncomingConnTimerSet,timerSet);

	return KErrNone;
    }

TVerdict CWaitForIncomingConnectionStep::doSingleTestStep()
	{
    /*
    These scenarios are tested here
    1.Timer expires since there is no incoming connection.
    2.Cancel the waitforincoming connection.
    */
    const int timerVal = 3000;
    TInt error;
    TInt errCancel;
    TRequestStatus reqStat;

    //Set the timer if subConnStatus is TimerOn
    if (timerSet.Compare(KTe_IncomingConnTimerStatus) == 0)
        {
        iTimer.CreateLocal();
        iTimer.After(reqStat,timerVal);
	    }

    TRequestStatus reqStat1;
    TInt error1 = iEsockTest->WaitforIncomingConnection(iParams,reqStat1);

    if (error1!=KErrNone)
        {
        INFO_PRINTF2(_L("Error in wait for incoming connection: %d."),error1);
        return EFail;
        }
	//check whether the cancel status is set or not.
	if ((subConnStatus.Compare(KTe_ConnCancelStatus)==0) && (reqStat1 != KErrNone))
    	{
        errCancel = iEsockTest->CancelIncomingConnection(iParams);
        if (errCancel != KErrNone)
            {
        	INFO_PRINTF2(_L("Error in cancelling the incoming connection: %d."),errCancel);
            }
    	}

	User::WaitForRequest(reqStat,reqStat1);

	error  = reqStat.Int();
	error1 = reqStat1.Int();

	//check whether the timer is expired.
   	if ((timerSet.Compare(KTe_IncomingConnTimerStatus)==0) && (error == KErrNone))
    	{
    	INFO_PRINTF1(_L("Timer was called since there was no incoming connection"));
    	iTimer.Close();
    	return EPass;
   		}
   	//check whether the KErrArgument is returned since the RSubConnection is already opened.
   	else if ((subConnStatus.Compare(KTe_ConnOpenStatus)==0) && (error1 == KErrArgument))
    	{
    	INFO_PRINTF1(_L("Returned KErrArgument since RsubCon was opened prior to Waitforincoming connection "));
    	return EPass;
   		}
   	//check whether KErrCancel is returned for cancelling incoming connection.
   	else if ((subConnStatus.Compare(KTe_ConnCancelStatus)==0) && (error1 == KErrCancel))
    	{
    	INFO_PRINTF1(_L("Successfully cancelled the incoming connection"));
    	return EPass;
   		}

	if (error1!=KErrNone)
		{
		INFO_PRINTF2(_L("Error in wait for incoming connection: %d."),error1);
    	return EFail;
		}

    return EPass;
	}


// Reject Incoming Connection
//---------------------------

CRejectIncomingConnectionStep::CRejectIncomingConnectionStep(CCEsockTestBase*& aEsockTest)
:   CTe_EsockStepBase(aEsockTest)
	{
	SetTestStepName(KRejectIncomingConnectionStep);
	}

TInt CRejectIncomingConnectionStep::ConfigureFromIni()
    {
	//clean parameters from previous runs
	iParams.Reset();

    //try reading from ini file
    if (GetStringFromConfig(iSection, KTe_SubConnectionName, iParams.iSubConnectionName)!=1)
        {
        return KErrNotFound;
        }

	return KErrNone;
    }

TVerdict CRejectIncomingConnectionStep::doSingleTestStep()
	{

    TInt error = iEsockTest->RejectIncomingConnection(iParams);
   	if (error!=KErrNone)
	    {
		INFO_PRINTF2(_L("Error in rejecting the  incoming connection: %d."),error);
	    return EFail;
		}
	return EPass;
	}


// Accept Incoming Connection
//---------------------------

CAcceptIncomingConnectionStep::CAcceptIncomingConnectionStep(CCEsockTestBase*& aEsockTest)
:   CTe_EsockStepBase(aEsockTest)
	{
	SetTestStepName(KAcceptIncomingConnectionStep);
	}

TInt CAcceptIncomingConnectionStep::ConfigureFromIni()
	{
	iParams.Reset();

	// Read in appropriate fields
	if((GetStringFromConfig(iSection, KTe_SubConnectionName, iParams.iSubConnectionName) != 1)
		|| (iParams.iSubConnectionName.Length() == 0))
		{
		INFO_PRINTF1(_L("Couldn't find appropriate field in config file"));
		return KErrNotFound;
		}

    // All ok if we got this far
    return KErrNone;
	}

TVerdict CAcceptIncomingConnectionStep::doSingleTestStep()
	{

    TInt error = iEsockTest->AcceptIncomingConnection(iParams);
   	if (error!=KErrNone)
		{
		INFO_PRINTF2(_L("Error in accepting the  incoming connection: %d."),error);
	    return EFail;
		}
	return EPass;
	}


// Async Incoming Connection
//--------------------------

CAsyncAcceptIncomingConnectionStep::CAsyncAcceptIncomingConnectionStep(CCEsockTestBase*& aEsockTest)
:   CTe_EsockStepBase(aEsockTest)
	{
	SetTestStepName(KAsyncAcceptIncomingConnectionStep);
	}

TInt CAsyncAcceptIncomingConnectionStep::ConfigureFromIni()
    {
	//clean parameters from previous runs
	iParams.Reset();

    //try reading the next socket's name
    if (GetStringFromConfig(iSection,KTe_SubConnectionName,iParams.iSubConnectionName)!=1
        || iParams.iSubConnectionName.Length()==0)
            return KErrNotFound;

    if (GetStringFromConfig(iSection,KTe_ConnectionName,iParams.iConnectionName)!=1)
        {
        INFO_PRINTF2(_L("%S: Connection name missing."),&iParams.iConnectionName);
        return KErrNotFound;
        }

    //optional
    GetStringFromConfig(iSection,KTe_ConnectionStatus,subConnStatus);
    GetStringFromConfig(iSection,KTe_IncomingConnTimerSet,timerSet);

	return KErrNone;
    }

TVerdict CAsyncAcceptIncomingConnectionStep::doSingleTestStep()
	{
    /*
    These scenarios are tested here
    1.Timer expires since there is no incoming connection.
    2.Cancel the waitforincoming connection.
    */
    const int timerVal = 30000;
    TInt error;
    TRequestStatus reqStat;

    //Set the timer if subConnStatus is TimerOn
    if (timerSet.Compare(KTe_IncomingConnTimerStatus) == 0)
        {
        TInt err = iTimer.CreateLocal();
        __ASSERT_DEBUG(err == KErrNone, User::Panic(KSpecAssert_ESockTestCnctnsT, 1));
        iTimer.After(reqStat,timerVal);
	    }

    TRequestStatus reqStat1;
    TInt error1 = iEsockTest->AsyncAcceptIncomingConnection(iParams,reqStat1);

    if (error1!=KErrNone)
        {
        INFO_PRINTF2(_L("Error in wait for incoming connection: %d."),error1);
        return EFail;
        }

	User::WaitForRequest(reqStat,reqStat1);

	error  = reqStat.Int();
	error1 = reqStat1.Int();

	//check whether the timer is expired.
   	if ((timerSet.Compare(KTe_IncomingConnTimerStatus)==0) && (error == KErrNone))
    	{
    	INFO_PRINTF1(_L("Timer was called since there was no incoming SubConnection"));
    	iTimer.Close();
    	return EPass;
   		}
   	//check whether the KErrArgument is returned since the RSubConnection is already opened.
   	else if ((subConnStatus.Compare(KTe_ConnOpenStatus)==0) && (error1 == KErrArgument))
    	{
    	INFO_PRINTF1(_L("Returned KErrArgument since RsubCon was opened prior to Waitforincoming connection "));
    	iTimer.Close();
    	return EPass;
   		}

	else if (error1!=KErrNone)
		{
		INFO_PRINTF2(_L("Error accepting subconnection: %d."),error1);
		iTimer.Close();
    	return EFail;
		}

    return EPass;
	}


// Await connection start complete
//---------------------

CAwaitRConnectionStartComplete::CAwaitRConnectionStartComplete(CCEsockTestBase*& aEsockTest)
:   CTe_EsockStepBase(aEsockTest)
	{
	SetTestStepName(KAwaitRConnectionStartComplete);
	}

TInt CAwaitRConnectionStartComplete::ConfigureFromIni()
    {
	//clean parameters from previous runs
	iParams.Reset();

    //try reading the connection's name
    if (!GetStringFromConfig(iSection,KTe_ConnectionName,iParams.iConnectionName)
        || iParams.iConnectionName.Length()==0)
		{
		INFO_PRINTF1(_L("Couldn't find appropriate field in config file"));
		return KErrNotFound;
		}

    if (!GetIntFromConfig(iSection,KExpectedError,iExpectedError))
       {
       iExpectedError = KErrNone;
       }
    else
       {
       INFO_PRINTF2(_L("Error to be expected: %d"),iExpectedError);
       }
    return KErrNone;
    }

TVerdict CAwaitRConnectionStartComplete::doSingleTestStep()
	{
    INFO_PRINTF2(_L("Awaiting Connection (%S) to start."),&iParams.iConnectionName);
    TInt error = iEsockTest->AwaitRequestStatus(iParams.iConnectionName);
	if ( error == iExpectedError)
		{
        INFO_PRINTF3(_L("Connection (%S) start completed as expected (%d)"),&iParams.iConnectionName, error);
		return EPass;
		}
    INFO_PRINTF4(_L("Connection (%S) start completed with (%d), which is different to expected (%d)"),&iParams.iConnectionName, error, iExpectedError);
	return EFail;
    }

// Attach to connection
//---------------------

CattachtorconnectionStep::CattachtorconnectionStep(CCEsockTestBase*& aEsockTest)
:   CTe_EsockStepBase(aEsockTest)
	{
	SetTestStepName(KattachtorconnectionStep);
	}

TInt CattachtorconnectionStep::ConfigureFromIni()
    {
	//clean parameters from previous runs
	iParams.Reset();

    //try reading the connection's name
    if (!GetStringFromConfig(iSection,KTe_ConnectionName,iParams.iConnectionName)
        || iParams.iConnectionName.Length()==0)
		{
		INFO_PRINTF1(_L("Couldn't find appropriate field in config file"));
		return KErrNotFound;
		}

    // Get the attach-type
    TPtrC attachType;
    if (!GetStringFromConfig(iSection,KTe_ConnectionAttachTypeName,attachType))
        {
        INFO_PRINTF2(_L("%S: Attach type missing."),&iParams.iConnectionName);
        return KErrNotFound;
        }
    else if(attachType.Compare(KTe_ConnAttachTypeNormal) == KErrNone)
    	{
    	iParams.iConnAttachType = RConnection::EAttachTypeNormal;
    	}
    else if(attachType.Compare(KTe_ConnAttachTypeMonitor) == KErrNone)
    	{
    	iParams.iConnAttachType = RConnection::EAttachTypeMonitor;
    	}
    else
  		{
    	INFO_PRINTF3(_L("%S: Attach Type (%S) not recognised."),&iParams.iConnectionName,&attachType);
        return KErrNotFound;
    	}

	if((GetStringFromConfig(iSection, KTe_AttachToConnectionName, iParams.iAttachToConnectionName) != 1)
		|| (iParams.iAttachToConnectionName.Length() == 0))
		{
		INFO_PRINTF2(_L("%S: Both IAP Id and NET Id are missing or zero. Attempting to attach to the first interface to be enumerated."), &iParams.iConnectionName);

	    // Get IAP and NET Ids
	    GetIntFromConfig(iSection,KTe_IAPIndexName,iParams.iIAP);
	    GetIntFromConfig(iSection,KTe_NetworkIndexName,iParams.iNET);

	    if(iParams.iIAP == 0 && iParams.iNET)
	    	{
	    	INFO_PRINTF2(_L("%S: Both IAP Id and NET Id are missing or zero. Attempting to attach to the first interface to be enumerated."), &iParams.iConnectionName);
	    	}
		}

    if (!GetIntFromConfig(iSection,KExpectedError,iExpectedError))
       {
       	iExpectedError = KErrNone;
       }
    else
       {
       	INFO_PRINTF2(_L("Error to be expected: %d"),iExpectedError);
       }
    return KErrNone;
    }

TVerdict CattachtorconnectionStep::doSingleTestStep()
	{
	TInt error = iEsockTest->AttachToConnection(iParams);
	if (error!=KErrNone)
        {
        INFO_PRINTF2(_L("Could not attach to connection (%S)."),&iParams.iConnectionName);
        INFO_PRINTF2(_L("Error: %d."),error);
        return EFail;
        }
	return EPass;
    }


// Enumerate connections
//----------------------

CEnumerateConnectionsStep::CEnumerateConnectionsStep(CCEsockTestBase*& aEsockTest)
:   CTe_EsockStepBase(aEsockTest)
	{
	SetTestStepName(KEnumerateConnectionsStep);
	}

TInt CEnumerateConnectionsStep::ConfigureFromIni()
	{
	// Read in appropriate fields
	if((GetStringFromConfig(iSection, KTe_ConnectionName, iParams.iConnectionName) != 1)
		|| (iParams.iConnectionName.Length() == 0))
		{
		INFO_PRINTF3(KErrString_MissingConfigFileField, &iSection, &KTe_ConnectionName);
		return KErrNotFound;
		}

	// Get the expected connection count to validate against
	TInt connCount;
	if(GetIntFromConfig(iSection, KExpectedConnectionCount, connCount) != 1)
		{
		INFO_PRINTF3(KErrString_MissingConfigFileField, &iSection, &KExpectedConnectionCount);
		return KErrNotFound;
		}
	else if(connCount < 0)
		{
		INFO_PRINTF3(KErrString_UnsignedConfigValueLessThanZero, &iSection, &KExpectedConnectionCount);
		return KErrArgument;
		}
	else
		{
		iParams.iExpectedConnCount = connCount;
		}

    // All ok if we got this far
    return KErrNone;
	}

TVerdict CEnumerateConnectionsStep::doSingleTestStep()
	{
	TUint connectionCount;

	// Find the connection to apply the enumerate call to
	RConnection* connection = iEsockTest->FindConnection(iParams.iConnectionName);

	// Enumerate the current connection count
	if(connection)
		{
		// Fetch the connection count
		TInt err = connection->EnumerateConnections(connectionCount);

		if (err != KErrNone)
			{
			INFO_PRINTF2(_L("Error when enumerating connections (%d)."), &err);
			return EFail;
			}

		// Compare the connection count with that expected
		if (connectionCount == iParams.iExpectedConnCount)
			{
			INFO_PRINTF3(_L("Current connection count (%d) equal to that expected (%d)."), connectionCount, iParams.iExpectedConnCount);
			}
		else
			{
			INFO_PRINTF3(_L("Current connection count (%d) not equal to expected count. (%d)"), connectionCount, iParams.iExpectedConnCount);
			return EFail;
			}
		}
	else
		{
		INFO_PRINTF2(_L("Could not find connection (%S)."), &iParams.iConnectionName);
		return EFail;
		}

	// Test passed if we got this far
	return EPass;
	}



CCreatePublishSubscribeVar::CCreatePublishSubscribeVar(CCEsockTestBase*& aEsockTest)
:   CTe_EsockStepBase(aEsockTest)
	{
	SetTestStepName(KCreatePublishSubscribeVar);
	}


TInt CCreatePublishSubscribeVar::ConfigureFromIni()
	{
	    //try reading the UID that im going to ignor name
	if (!GetIntFromConfig(iSection,KTe_CreatePublishSubscribeVarUid,iUid))
			{
			INFO_PRINTF1(_L("Couldn't find appropriate UID field in config file"));
			return KErrNotFound;
			}

	if (!GetIntFromConfig(iSection,KTe_CreatePublishSubscribeVarValue,iValue))
			{
			INFO_PRINTF1(_L("Couldn't find appropriate Value field in config file"));
			return KErrNotFound;
			}

	if (!GetIntFromConfig(iSection,KTe_CreatePublishSubscribeVarKey,iKey))
			{
			// The key field was added at a later date, report but don't error.
			INFO_PRINTF1(_L("Couldn't find appropriate Key field in config file, defaulting to 0"));
			}

    return KErrNone;
	}

TVerdict CCreatePublishSubscribeVar::doTestStepPreambleL()
	{
	return EPass;
	}

TVerdict CCreatePublishSubscribeVar::doSingleTestStep()
	{
	RProperty property;
	TInt result = property.Define(TUid::Uid(iUid),iKey,RProperty::EInt);
	result = property.Attach(TUid::Uid(iUid), iKey);
	if(result == KErrNone)
		{
		result = property.Set(iValue);
		if(result == KErrNone)
			return EPass;
		}
	return EFail;
	}

//Check a publish subscribe var
/**
Class implementing CreateRConnectionStep

@internalComponent
*/
CCheckPublishSubscribeVar::CCheckPublishSubscribeVar(CCEsockTestBase*& aEsockTest)
:   CTe_EsockStepBase(aEsockTest)
	{
	SetTestStepName(KCheckPublishSubscribeVar);
	}

TInt CCheckPublishSubscribeVar::ConfigureFromIni()
	{
    //try reading the UID that im not going to use name
	if (!GetIntFromConfig(iSection,KTe_CheckPublishSubscribeVarUid,iUid))
		{
		INFO_PRINTF1(_L("Couldn't find appropriate UID field in config file"));
		return KErrNotFound;
		}

	if (!GetIntFromConfig(iSection,KTe_CheckPublishSubscribeVarValue,iValue))
		{
		INFO_PRINTF1(_L("Couldn't find appropriate UID field in config file"));
		return KErrNotFound;
		}

	if (!GetIntFromConfig(iSection,KTe_CheckPublishSubscribeVarKey,iKey))
		{
		// The key field was added at a later date, report but don't error.
		INFO_PRINTF1(_L("Couldn't find appropriate Key field in config file, defaulting to 0"));
		}

	return KErrNone;
	}

TVerdict CCheckPublishSubscribeVar::doTestStepPreambleL()
	{
	return EPass;
	}

TVerdict CCheckPublishSubscribeVar::doSingleTestStep()
	{
	RProperty property;
	TInt result = property.Attach(TUid::Uid(iUid), iKey);
	if(result == KErrNone)
		{
		TInt existingValue;
		result = property.Get(TUid::Uid(iUid), iKey, existingValue);
		if(result == KErrNone && iValue == existingValue)
			return EPass;
		}
	return EFail;
	}

// Progress notifications
//-----------------------

CRegisterProgressNotificationStep::CRegisterProgressNotificationStep(CCEsockTestBase*& aEsockTest)
:   CTe_EsockStepBase(aEsockTest)
	{
	SetTestStepName(KRegisterProgressNotificationStep);
	}

TInt CRegisterProgressNotificationStep::ConfigureFromIni()
	{
	//clean parameters from previous runs
	iParams.Reset();

	//try reading the next event container's name
	if (GetStringFromConfig(iSection,KTe_ConnEventsName,iParams.iEventName)!=1
	    || iParams.iEventName.Length()==0)
		return KErrNotFound;

	if (GetStringFromConfig(iSection,KTe_ConnectionName,iParams.iConnectionName)!=1)
		{
		INFO_PRINTF2(_L("%S: Connection name missing."),&iParams.iEventName);
		return KErrNotFound;
		}

	TPtrC eventName;
    TInt event;
    if (GetIntFromConfig(iSection,KTe_SelectedProgress, event)==1)
        {
        iParams.iEventMask = event;
        }
    else if (GetStringFromConfig(iSection,KTe_SelectedProgress,eventName)==1)
		{
		if (eventName.Compare(KTe_LinkLayerOpen)==0)
			{ iParams.iEventMask = KLinkLayerOpen; }
		else if (eventName.Compare(KTe_LinkLayerClosed)==0)
			{ iParams.iEventMask = KLinkLayerClosed; }
		else
			{
			INFO_PRINTF3(_L("%S: Event type (%S) not recognised."),&iParams.iEventName,&eventName);
			return KErrNotFound;
			}
		}
	else
		{
		INFO_PRINTF1(_L("Event type missing."));
		return KErrNotFound;
		}

	INFO_PRINTF3(_L("%S: Events to be registered: 0x%x"),&iParams.iEventName,iParams.iEventMask);
	return KErrNone;

	}

TVerdict CRegisterProgressNotificationStep::doSingleTestStep()
	{
	TInt error = iEsockTest->RegisterForProgressNotificationEvent(iParams);
	if (error!=KErrNone)
		{
		INFO_PRINTF2(_L("Could not register for rsubconnection events (%S)."),&iParams.iEventName);
		INFO_PRINTF2(_L("Error: %d"),error);
		return EFail;
		}

	return EPass;
	}

// Check for Progress notification
//-------------------------------

CCheckProgressNotificationStep::CCheckProgressNotificationStep(CCEsockTestBase*& aEsockTest)
:   CTe_EsockStepBase(aEsockTest)
	{
	SetTestStepName(KCheckProgressNotificationStep);
	}

TInt CCheckProgressNotificationStep::ConfigureFromIni()
	{
	//clean parameters from previous runs
	iParams.Reset();

	//try reading the next event container's name
	if (GetStringFromConfig(iSection,KTe_ConnEventsName,iParams.iEventName)!=1
	    || iParams.iEventName.Length()==0)
		return KErrNotFound;

	TPtrC eventName;
    TInt event;
    if (GetIntFromConfig(iSection,KTe_SelectedProgress, event)==1)
        {
        iParams.iEventMask = event;
        }
    else if (GetStringFromConfig(iSection,KTe_SelectedProgress,eventName)==1)
		{
		if (eventName.Compare(KTe_LinkLayerOpen)==0)
			{ iParams.iEventMask = KLinkLayerOpen; }
		else if (eventName.Compare(KTe_LinkLayerClosed)==0)
			{ iParams.iEventMask = KLinkLayerClosed; }
		else
			{
			INFO_PRINTF3(_L("%S: Event type (%S) not recognised."),&iParams.iEventName,&eventName);
			return KErrNotFound;
			}
		}
	else
		{
		INFO_PRINTF1(_L("Event type missing."));
		return KErrNotFound;
		}

	if (!GetIntFromConfig(iSection, KExpectedError, iExpectedError))
		{
		iExpectedError = KExpectedErrorNotUsed;
		}

	if (iExpectedError != KExpectedErrorNotUsed)
		{
			INFO_PRINTF2(_L("Error to be expected: %d"),iExpectedError);
		}

	// All ok if we got this far
	return KErrNone;
	}


TVerdict CCheckProgressNotificationStep::doSingleTestStep()
	{
	TNifProgress* event = NULL;
	TInt err = iEsockTest->ReceiveProgressNotificationEvent(event, iParams.iEventName);

	if (event == NULL || err != KErrNone)
		{
		INFO_PRINTF2(_L("%S: Did not receive any event!"),&iParams.iEventName);
		INFO_PRINTF2(_L("The error code returned was %d."),err);
		return EFail;
		}

	TInt eventId = event->iStage;

	if (eventId == KLinkLayerOpen )
		{
		INFO_PRINTF1(_L("Received event: KLinkLayerOpen"));
		}
	else if (eventId == KLinkLayerClosed )
		{
		INFO_PRINTF1(_L("Received event: KLinkLayerClosed"));
		}
	else
		{
		INFO_PRINTF2(_L("Received event: Unknown (%d)"),eventId);
		}

	if (eventId != iParams.iEventMask)
		{
		INFO_PRINTF2(_L("Did not receive an expected event with %S."),&iParams.iEventName);
		return EFail;
		}


	if(iExpectedError != event->iError && iExpectedError < KExpectedErrorNotUsed)
		{
		INFO_PRINTF3(_L("Expected Error %d does not match returned Error %d"), iExpectedError, event->iError);
		return EFail;
		}

	else if (iExpectedError != KExpectedErrorNotUsed)
		{
		INFO_PRINTF3(_L("Expected Error %d matched returned Error %d"), iExpectedError, event->iError);
		}

	return EPass;
	}

// Check for Progress notification (negative test)
//-------------------------------

CCheckNegativeProgressNotificationStep::CCheckNegativeProgressNotificationStep(CCEsockTestBase*& aEsockTest)
:   CTe_EsockStepBase(aEsockTest)
	{
	SetTestStepName(KCheckNegativeProgressNotificationStep);
	}

TInt CCheckNegativeProgressNotificationStep::ConfigureFromIni()
	{
	//clean parameters from previous runs
	iParams.Reset();

	//try reading the next event container's name
	if (GetStringFromConfig(iSection,KTe_ConnEventsName,iParams.iEventName)!=1
	    || iParams.iEventName.Length()==0)
		return KErrNotFound;

	TPtrC eventName;
	TInt event;
	if (GetIntFromConfig(iSection,KTe_SelectedProgress, event)==1)
	    {
        iParams.iEventMask = event;
	    }
	else if (GetStringFromConfig(iSection,KTe_SelectedProgress,eventName)==1)
	    {
        if (eventName.Compare(KTe_LinkLayerOpen)==0)
            { iParams.iEventMask = KLinkLayerOpen; }
        else if (eventName.Compare(KTe_LinkLayerClosed)==0)
            { iParams.iEventMask = KLinkLayerClosed; }
        else
            {
            INFO_PRINTF3(_L("%S: Event type (%S) not recognised."),&iParams.iEventName,&eventName);
            return KErrNotFound;
            }
        }
    else
        {
        INFO_PRINTF1(_L("Event type missing."));
        return KErrNotFound;
        }	

	if (!GetIntFromConfig(iSection, KExpectedError, iExpectedError))
		{
		iExpectedError = KExpectedErrorNotUsed;
		}

    // Get any timeout value and if we have one then we are not expecting any notification
    if(!GetIntFromConfig(iSection, KTimeoutInMilliSeconds, iParams.iTimeoutMiliSecs))
        {
        // Default to 5 second timeout
        iParams.iTimeoutMiliSecs = 5000;
        }

	if (iExpectedError != KExpectedErrorNotUsed)
		{
			INFO_PRINTF2(_L("Error to be expected: %d"),iExpectedError);
		}

	// All ok if we got this far
	return KErrNone;
	}


TVerdict CCheckNegativeProgressNotificationStep::doSingleTestStep()
	{
	TNifProgress* event = NULL;
	TInt err = iEsockTest->ReceiveProgressNotificationEvent(event, iParams.iEventName, iParams.iTimeoutMiliSecs);

	if (event == NULL && err == KErrNone)
		{
		INFO_PRINTF2(_L("%S: Did not receive any event."),&iParams.iEventName);
		return EPass;
		}
    else if (event == NULL || err != KErrNone)
        {
        INFO_PRINTF2(_L("%S: Did not receive any event but error condiction detected !"),&iParams.iEventName);
        INFO_PRINTF2(_L("The error code returned was %d."),err);
        return EFail;
        }


	TInt eventId = event->iStage;

	if (eventId == KLinkLayerOpen )
		{
		INFO_PRINTF1(_L("Received event: KLinkLayerOpen"));
		}
	else if (eventId == KLinkLayerClosed )
		{
		INFO_PRINTF1(_L("Received event: KLinkLayerClosed"));
		}
	else
		{
		INFO_PRINTF2(_L("Received event: Unknown (%d)"),eventId);
		}

	if (eventId == iParams.iEventMask)
		{
		INFO_PRINTF2(_L("Received the banned  event ( %S )."),&iParams.iEventName);
		return EFail;
		}


	if(iExpectedError != event->iError && iExpectedError < KExpectedErrorNotUsed)
		{
		INFO_PRINTF3(_L("Expected Error %d does not match returned Error %d"), iExpectedError, event->iError);
		return EFail;
		}

	else if (iExpectedError != KExpectedErrorNotUsed)
		{
		INFO_PRINTF3(_L("Expected Error %d matched returned Error %d"), iExpectedError, event->iError);
		}

	return EPass;
	}


// GetParameters_Int
//-------------------------------

CGetParameters_IntStep::CGetParameters_IntStep(CCEsockTestBase*& aEsockTest)
:   CTe_EsockStepBase(aEsockTest)
    {
    SetTestStepName(KGetParameters_IntStep);
    }

TInt CGetParameters_IntStep::ConfigureFromIni()
    {
    // Read in appropriate fields
    if((GetStringFromConfig(iSection, KTe_ConnectionName, iConnectionName) != 1)
        || (iConnectionName.Length() == 0))
        {
        INFO_PRINTF1(_L("Couldn't find appropriate field in config file"));
        return KErrNotFound;
        }

    if((GetStringFromConfig(iSection, KTe_ParameterType, iFieldName) != 1)
        || (iFieldName.Length() == 0))
        {
        INFO_PRINTF1(_L("Couldn't find appropriate field in config file"));
        return KErrNotFound;
        }
    _LIT(KIapTableIdDes, "IAP\\Id" );
    if (iFieldName.Compare(KIapTableIdDes) == 0)
        {
        iRecordTypeId = CommsDat::KCDTIdIAPRecord | CommsDat::KCDTIdRecordTag;
        }
    /*else
     * Don't get too angry, but so far, this generic looking test step only does IAP
     * There are nice arrays in CED that can be used to address any field. If you're
     * desperate, please pull them in and modify this test step to be able to retrieve
     * any field 
     */

    if(!GetIntFromConfig(iSection, KTe_ParameterExpectedValue, iExpectedValue))
        {
        INFO_PRINTF1(_L("Couldn't find appropriate field in config file"));
        return KErrNotFound;
        }
    
    return KErrNone;
    }


TVerdict CGetParameters_IntStep::doSingleTestStep()
    {
    TInt ret = KErrNone;
    ESock::XConnectionQuerySet* querySet = NULL;
    TRAP(ret, querySet = ESock::XConnectionQuerySet::NewL() );
   
    if ( ret == KErrNone )
        {
        TRAP(ret, ESock::XUintQuery::NewL(iRecordTypeId, *querySet) );
        }
    ESock::CConnectionQuerySet* connectionQuerySet = NULL;
    if ( ret == KErrNone )
        {
        TRAP(ret, connectionQuerySet = ESock::CConnectionQuerySet::NewL(querySet) );
        }    
    
    if( (ret = iEsockTest->GetParameters(iConnectionName, *connectionQuerySet)) != KErrNone)
        {
        INFO_PRINTF2(_L("RConnection::GetParameters returned %d"), ret);
        SetTestStepError(ret);
        return EFail;
        }

    ESock::XConnectionQuerySet& outputQuerySet = connectionQuerySet->DataObject();
    ESock::XUintQuery* iapTableIdQuery = static_cast<ESock::XUintQuery*>(outputQuerySet.FindQuery( iRecordTypeId ));
    __ASSERT_DEBUG(iapTableIdQuery, User::Panic(KSpecAssert_ESockTestCnctnsT, 2));
    if (iapTableIdQuery->Error() != KErrNone)
        {
        INFO_PRINTF2(_L("RConnection::GetParameters Int query returned (%d)"), iapTableIdQuery->Error());
        SetTestStepError(KErrCorrupt);
        return EFail;        
        }
    
    if (iapTableIdQuery->Data() != iExpectedValue)
        {
        INFO_PRINTF3(_L("RConnection::GetParameters Int query returned (%d) something else than expected (%d)"), iapTableIdQuery->Data(), iExpectedValue);
        SetTestStepError(KErrCorrupt);
        return EFail;        
        }
    
    /*Just out of justified paranoia, we're extracting the same field using RConnection::GetIntSetting*/
    TUint32 aValue;
    iEsockTest->GetIntSetting(iConnectionName, iFieldName, aValue);
    if (aValue != iExpectedValue)
        {
        INFO_PRINTF3(_L("RConnection::GetIntSetting returned (%d) something else than expected (%d)"), aValue, iExpectedValue);
        SetTestStepError(KErrCorrupt);
        return EFail;        
        }
           
    return EPass;
    }

// WaitStep
//-------------------------------

CWaitStep::CWaitStep(CCEsockTestBase*& aEsockTest)
:   CTe_EsockStepBase(aEsockTest)
    {
    SetTestStepName(KWaitStep);
    }

TInt CWaitStep::ConfigureFromIni()
    {
    if(!GetIntFromConfig(iSection, KTimeoutInMilliSeconds, iTimeOutMs))
        {
        INFO_PRINTF1(_L("Couldn't find appropriate field in config file"));
        return KErrNotFound;
        }
    
    return KErrNone;
    }

TVerdict CWaitStep::doSingleTestStep()
    {
    User::After(iTimeOutMs *1000);
    return EPass;
    }


