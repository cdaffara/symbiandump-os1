// Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// Example CTestStep derived implementation
// 
//

/**
 @file lbsnetgatewaystartstep.cpp
*/
#include "lbsnetgatewaystartmultiplepmstep.h"
#include "Te_lbsnetgatewaySuiteDefs.h"
#include "lbsnetinternalapi.h"
#include "lbsnetregstatusint.h"
#include <lbs/lbsadmin.h>


Clbsnetgatewaystartmultiplepmstep::~Clbsnetgatewaystartmultiplepmstep()
/**
 * Destructor
 */
	{
	}

Clbsnetgatewaystartmultiplepmstep::Clbsnetgatewaystartmultiplepmstep()
/**
 * Constructor
 */
	{
	// **MUST** call SetTestStepName in the constructor as the controlling
	// framework uses the test step name immediately following construction to set
	// up the step's unique logging ID.
	SetTestStepName(Klbsnetgatewaystartstep2);
	}

TVerdict Clbsnetgatewaystartmultiplepmstep::doTestStepPreambleL()
/**
 * @return - TVerdict code
 * Override of base class virtual
 */
	{
	//INFO_PRINTF1(_L("Please delete this line or modify it. I am in Test Step Preamble in Class Clbsnetgatewaystartmultiplepmstep"));
	// uncomment the following 3 lines if you have common pre setting to all the test steps in there
	CTe_lbsnetgatewaySuiteStepBase::doTestStepPreambleL();
	// if (TestStepResult()!=EPass)
	//    return   TestStepResult();
	// process some pre setting to this test step then set SetTestStepResult to EFail or Epass.
	
	SetTestStepResult(EPass);
	return TestStepResult();
	}


TVerdict Clbsnetgatewaystartmultiplepmstep::doTestStepL()
/**
 * @return - TVerdict code
 * Override of base class pure virtual
 * Our implementation only gets called if the base class doTestStepPreambleL() did
 * not leave. That being the case, the current test result value will be EPass.
 */
	{
	INFO_PRINTF1(_L("*** Clbsnetgatewaystartmultiplepmstep::doTestStepL() >> </b>"));
	  if (TestStepResult()==EPass)
		{
		
		//
		// Special test step to start-up the LBS system
		// as needed for this suite of tests.
		//
		
		// Setup aray of 10 valid PMs
		TLbsAdminProtocolModulesInfo infoHome;
		TLbsAdminProtocolModulesInfo infoRoaming;

        InitialiseUidArray();
        
        // Assign array to TClass for passing to admin
		infoHome.SetModuleIds(iUidHomeArray,10);
		infoRoaming.SetModuleIds(iUidRoamingArray,10);
		
		
		// Pass to admin and watch for errors....
		CLbsAdmin* admin = CLbsAdmin::NewL();
		CleanupStack::PushL(admin);
		INFO_PRINTF1(_L("*** Setting Home PM Uids</b>"));
		User::LeaveIfError(admin->Set(KLbsSettingHomeProtocolModule, infoHome));
		INFO_PRINTF1(_L("*** Set Home PM Uids</b>"));

		INFO_PRINTF1(_L("*** Setting Roaming PM Uids</b>"));
		User::LeaveIfError(admin->Set(KLbsSettingRoamingProtocolModule, infoRoaming));
		INFO_PRINTF1(_L("*** Set Roaming PM Uids</b>"));
		
		//Cleanup admin...
		CleanupStack::PopAndDestroy(admin);

		//Continue to startgateway as normal...
		INFO_PRINTF1(_L("*** Setting Home & Roaming PM Uids successfully set...</b>"));
		TLbsAdminProtocolModuleLoadingInfo loadInfo;
		TInt loadingStrategy = TLbsAdminProtocolModuleLoadingInfo::EProtocolModuleLoadingUnknown;
		if(!GetIntFromConfig(ConfigSection(), _L("ModuleLoading"), loadingStrategy))
			{
			ERR_PRINTF1(_L("Failed to get the ModuleLoading setting from the test ini file"));
			}
		loadInfo.SetLoadingStrategy(loadingStrategy);
		User::LeaveIfError(iAdmin->Set(KLbsProtocolModuleLoading, loadInfo));
		
		// Initialise NRH-NG interface
		RLbsNetChannel::InitializeL(RLbsNetChannel::EChannelNG2NRH);
		RLbsNetChannel::InitializeL(RLbsNetChannel::EChannelNRH2NG);
		
		// Intialise A-GPS interface
		RLbsNetChannel::InitializeL(RLbsNetChannel::EChannelNG2AGPS);
		RLbsNetChannel::InitializeL(RLbsNetChannel::EChannelAGPS2NG);
		RAssistanceDataCache::InitializeL();
		
        // Initialise the SuplPSY to NG interface 
        RLbsNetChannel::InitializeL(RLbsNetChannel::EChannelNG2SUPLPSY);
        RLbsNetChannel::InitializeL(RLbsNetChannel::EChannelSUPLPSY2NG);
        
		// Initialise Network Location Manager interface
		RLbsNetworkLocationRequests::InitializeL();
		RLbsNetworkLocationResponses::InitializeL();
		RLbsNetworkPositionUpdates::InitializeL();
		
		// Initialize SystemController interface
		RLbsProcessSupervisor::InitializeL();
		
		// Initialize the network registration status interface
		RLbsNetworkRegistrationStatus::InitializeL();
		
		// Start NetGateway
		User::LeaveIfError(StartNetGateway());		
		
		// All tests passed ok.
		SetTestStepResult(EPass);
		}
	  INFO_PRINTF1(_L("*** Clbsnetgatewaystartmultiplepmstep::doTestStepL() << </b>"));
	  return TestStepResult();
	}



TVerdict Clbsnetgatewaystartmultiplepmstep::doTestStepPostambleL()
/**
 * @return - TVerdict code
 * Override of base class virtual
 */
	{
	//INFO_PRINTF1(_L("Please delete this line or modify it. I am in Test Step Postamble in Class Clbsnetgatewaystartmultiplepmstep"));
	// process something post setting to the test step
	
	// uncomment the following line if you have common post setting to all the test steps in there
	CTe_lbsnetgatewaySuiteStepBase::doTestStepPostambleL();
	// uncomment the following line if you have post process or remove the following line if no post process
	SetTestStepResult(EPass);		// or EFail
	return TestStepResult();
	}

