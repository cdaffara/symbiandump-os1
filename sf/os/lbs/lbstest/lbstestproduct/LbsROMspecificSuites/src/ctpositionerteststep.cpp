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
 @file 
 @internalTechnology
 @test
*/
#include "ctpositionerteststep.h"
#include "ctlbstestsuitedefs.h"
#include <lbsareainfo.h>
#include <lbspositioninfo.h>

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
	
	CTe_TestSuiteStepBase::doTestStepPreambleL();
	SetTestStepResult(EPass);
	return TestStepResult();
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
		if(!GetIntFromConfig(ConfigSection(),KTestCaseIdInt,testIndex))
			{
            INFO_PRINTF1(_L("Error: could not get testindex from ini file"));
			User::Leave(KErrNotFound);
			}
		_LIT(KTestName, "Call test by index");
		INFO_PRINTF1(KTestName);
		TestByIndexL(testIndex);
		}
	return TestStepResult();
	}

void CPositionerTestStep::TestByIndexL(TInt aIndex)
    {
    switch (aIndex)
        {
        case 1:
            _LIT(KTestName1, "LBS Full ROM Variant2 - Location Monitor startup/closedown ");
            INFO_PRINTF1(KTestName1);
        	TdVariant2_FullRom_Check_LocMonitor_PersistentL();
        	break;
        case 2:
            _LIT(KTestName2, "LBS StandAlonePrivacy ROM Variant2 - Location Monitor startup/closedown");
            INFO_PRINTF1(KTestName2);
        	TdVariant2_StandAlonePrivacyRom_Check_LocMonitor_TransientL();
        	break;
        	
        default: 
            _LIT(KTestName10, "This test step doesn't exist");
            INFO_PRINTF1(KTestName10);
            SetTestStepResult(EFail);
            break;
            
        }
    }


TVerdict CPositionerTestStep::TdVariant2_StandAlonePrivacyRom_Check_LocMonitor_TransientL()
	{
	
	// Check if the location monitor process is transient in 
	// the standalone privacy mode
	//_LIT(KLocMonitorExeName, "\\sys\\bin\\locationmonitor.exe");
	
	// Validate that the location monitor is not started by the root
	// in this configuration
	_LIT(KLocMonitorServerName, "locationmonitor.exe");
	TFindServer findServer(KLocMonitorServerName);
	TInt err;
	TFullName name;
	err = findServer.Next(name);

    if ((err==KErrNone)||(err==KErrAlreadyExists))
        {
        _LIT(KFailLog, "Location monitor process started by root - so test case failed");
        ERR_PRINTF1(KFailLog);
        SetTestStepResult(EFail);
        }
    else
        {
    
        _LIT(KPassInfoLog, "Location Monitor NOT started by the root process");
        INFO_PRINTF1(KPassInfoLog);
        
        RPositionServer server;
        TInt retval = server.Connect();
        INFO_PRINTF2(_L("RPositionServer.Connect() returned with value %d, should return 0"),retval);
        CleanupClosePushL(server);

        User::After(100*1000);

        _LIT(KTest, "Session with epos location server opened successfully");
        INFO_PRINTF1(KTest);
        
		RPositioner positioner;
		TInt err = positioner.Open(server);

        if(KErrNone != err)
            {
            INFO_PRINTF2(_L("positioner.Open(server) returned value %d, should be 0"), err);
            ERR_PRINTF1(KFailedOpenPositioner);
            SetTestStepResult(EFail);
            CleanupStack::PopAndDestroy(&server);
            return TestStepResult();
            }
        
        CleanupClosePushL(positioner);
        _LIT(KTest2, "SubSession with epos location server opened successfully");
        INFO_PRINTF1(KTest2);
		
		// Validate that the location monitor gets started up when
		// a client opens a session with the  EPos location server.
		_LIT(KLocMonitorServerName, "locationmonitor.exe");
		TFindServer findServer2(KLocMonitorServerName);
		TFullName name2;
		err = -1; // assign the default value of -1 [KErrNotFound]
		err = findServer2.Next(name2);
		if ((err!=KErrNone)&&(err!=KErrAlreadyExists))
			{
			_LIT(KFailInfoLog, "Location Monitor NOT started even after a client connects to the epos location server");
			ERR_PRINTF1(KFailInfoLog);
			SetTestStepResult(EFail);
			return TestStepResult();
			}
		
		if ((err==KErrNone)||(err==KErrAlreadyExists))
			{
			_LIT(KPassInfoLog, "Location Monitor started after a client connects to the epos location server");
			INFO_PRINTF1(KPassInfoLog);
			}

		CleanupStack::PopAndDestroy(2, &server);
		User::After(15000000);

		//Validate that the location monitor no longer exists after the client
		//closes the handle with the location server
		TFindServer findServer3(KLocMonitorServerName);
		TFullName name3;
		err = 0; // assign the default value of 0 [KErrNone]
		err = findServer3.Next(name3);
		if ((err==KErrNone)||(err==KErrAlreadyExists))
			{
			_LIT(KFailInfoLog, "Location Monitor NOT shut down even after all the clients disconnect from the epos location server");
			ERR_PRINTF1(KFailInfoLog);
			SetTestStepResult(EFail);
			return TestStepResult();
			}
		
		}

	return TestStepResult();

	}

TVerdict CPositionerTestStep::TdVariant2_FullRom_Check_LocMonitor_PersistentL()
	{

	// The root process should have already been started before entering
	// this function [by start lbs test step]
	//_LIT(KLocMonitorExeName, "\\sys\\bin\\locationmonitor.exe");
	_LIT(KLocMonitorServerName, "locationmonitor.exe");
	
	// Check if the location monitor process is running 
	TFindServer findServer(KLocMonitorServerName);
	TInt err;
	TFullName name;
	err = findServer.Next(name);
	
	if ((err!=KErrNone)&&(err!=KErrAlreadyExists))
		{
		_LIT(KFailLog, "Location monitor process not started by root - so test case failed");
		ERR_PRINTF1(KFailLog);
		SetTestStepResult(EFail);
		}
	else
		{
		
		_LIT(KPassInfoLog, "Location Monitor started by the root process");
		INFO_PRINTF1(KPassInfoLog);
		
			RPositionServer server;
			TInt retval = server.Connect();
			if(err != KErrNone)
			    {
                SetTestStepResult(EFail);
                INFO_PRINTF2(_L("RPositionServer.Connect() returned with value %d, should return 0"),retval);
                CleanupStack::PopAndDestroy(&server);
                return TestStepResult();
			    }
			CleanupClosePushL(server);

            _LIT(KTest, "Session with epos location server opened successfully");
            INFO_PRINTF1(KTest);
			
			RPositioner positioner;
			TInt err = positioner.Open(server);
			
			if(KErrNone != err)
			{
			ERR_PRINTF1(KFailedOpenPositioner);
			SetTestStepResult(EFail);
			CleanupStack::PopAndDestroy(&server);
			return TestStepResult();
			}
            
			_LIT(KTest2, "SubSession with epos location server opened successfully");
            INFO_PRINTF1(KTest2);
            
			//CleanupClosePushL(positioner);
			positioner.Close();
			CleanupStack::PopAndDestroy(&server);
			User::After(5000);

			// Check if the process exists even after the client
			// closes the handle with the location server

			_LIT(KTest3, "Epos location server closed down as the clients are disconnected");
            INFO_PRINTF1(KTest3);
            
			_LIT(KLocMonitorServerName, "locationmonitor.exe");
			TFindServer findServer2(KLocMonitorServerName);
			TFullName name2;
			err = -1; // assign the default value of -1 [KErrNotFound]
			err = findServer2.Next(name2);
			if ((err!=KErrNone)&&(err!=KErrAlreadyExists))
				{
				_LIT(KPassInfoLog2, "Location Monitor dies when the epos location server is closed down");
				ERR_PRINTF1(KPassInfoLog2);
				SetTestStepResult(EFail);
				return TestStepResult();
				}
			_LIT(KPassInfoLog2, "Location Monitor exists even after the location server is closed down");
			INFO_PRINTF1(KPassInfoLog2);
		}
	return TestStepResult();
	
	}


TVerdict CPositionerTestStep::doTestStepPostambleL()
/**
 * @return - TVerdict code
 * Override of base class virtual
 */
	{
	return TestStepResult();
	}


