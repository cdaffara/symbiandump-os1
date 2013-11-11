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
// Example CTestStep derived implementation
// 
//

/**
 @file lbsnrhprivacystartnrhStep.cpp
*/

#include <lbs/lbsadmin.h>
#include "lbsnrhstartnrhstep.h"
#include "te_lbsnrhsuite2defs.h"
#include "lbsnetinternalapi.h"
#include "lbsinternalinterface.h"
#include "lbsqualityprofile.h"
#include "cserverlaunch.h"
#include "tserverstartparams.h"
#include "lbsnetregstatusint.h"


CLbsNrhStartNrhStep::~CLbsNrhStartNrhStep()
/**
 * Destructor
 */
	{
	}

CLbsNrhStartNrhStep::CLbsNrhStartNrhStep()
/**
 * Constructor
 */
	{
	// **MUST** call SetTestStepName in the constructor as the controlling
	// framework uses the test step name immediately following construction to set
	// up the step's unique logging ID.
	SetTestStepName(KLbsNrhStartNrhStep);
	}

TVerdict CLbsNrhStartNrhStep::doTestStepPreambleL()
/**
 * @return - TVerdict code
 * Override of base class virtual
 */
	{
	CTe_LbsNrhSuiteStepBase::doTestStepPreambleL();
	if (TestStepResult()!=EPass)
	   return   TestStepResult();

	SetTestStepResult(EPass);
	return TestStepResult();
	}


TVerdict CLbsNrhStartNrhStep::doTestStepL()
/**
 * @return - TVerdict code
 * Override of base class pure virtual
 * Our implementation only gets called if the base class doTestStepPreambleL() did
 * not leave. That being the case, the current test result value will be EPass.
 */
	{
	  if (TestStepResult()==EPass)
		{
		//
		// Special test step to start-up the LBS system
		// as needed for this suite of tests.
		//
		
		// Read any admin settings from .ini file
		ParseAdminSettingsFromIniL();
		
		
		// Initialise NRH-NG interface
		RLbsNetChannel::InitializeL(RLbsNetChannel::EChannelNG2NRH);
		RLbsNetChannel::InitializeL(RLbsNetChannel::EChannelNRH2NG);
		
		// Initialise the NRH-AGPS interface
		RLbsPositionUpdateRequests::InitializeL();
		TPositionExtendedSatelliteInfo satInfo;
		RLbsPositionUpdates::InitializeL(satInfo);
		RLbsGpsMeasurementUpdates::InitializeL();
		
		// Initialise Network Position updates interface
		RLbsNetworkPositionUpdates::InitializeL();
		
		// Initialize SystemController interface
		RLbsProcessSupervisor::InitializeL();
		
		// Initialize the network registration status interface
		RLbsNetworkRegistrationStatus::InitializeL();
		RLbsNetworkRegistrationStatus netRegStatus;
		netRegStatus.OpenL();
		netRegStatus.SetNetworkRegistrationStatus(RLbsNetworkRegistrationStatus::ERegisteredHomeNetwork);
		netRegStatus.Close();
		
		// Initialise quality profile data
		RArray<TQualityProfile> profileArray;
		CleanupClosePushL(profileArray);
		TQualityProfile profile;
		profile.SetQualityProfileId(0);
		profile.SetMaxFixTime(20000000);
		profile.SetMinHorizontalAccuracy(TReal32(100.0));
		profile.SetMinVerticalAccuracy(TReal32(100.0));
		profileArray.AppendL(profile);
		LbsQualityProfile::InitializeL(profileArray);
		CleanupStack::PopAndDestroy(&profileArray);
		
		// set up Module Info.  NRH only uses the DeviceGpsModeCapabilities
		TPositionModuleInfoExtended::TDeviceGpsModeCapabilities integrationModuleCaps;
		GetDeviceGpsModeCapabilitiesFromIniL(integrationModuleCaps);

		
		TPositionModuleInfoExtended moduleInfoArray;
		moduleInfoArray.iModuleId = KLbsGpsLocManagerUid;
		moduleInfoArray.iDeviceGpsModeCapabilities =  integrationModuleCaps;
		const TPositionModuleInfoExtended* constmoduleInfoArray[] = 
			{
			&moduleInfoArray,
			};
		
		LbsModuleInfo::InitializeL(constmoduleInfoArray, 1);

		// Start NRH
		User::LeaveIfError(StartNrh());		
		
		// All tests passed ok.
		SetTestStepResult(EPass);
		}
	  return TestStepResult();
	}



TVerdict CLbsNrhStartNrhStep::doTestStepPostambleL()
/**
 * @return - TVerdict code
 * Override of base class virtual
 */
	{
	CTe_LbsNrhSuiteStepBase::doTestStepPostambleL();
	
	SetTestStepResult(EPass);
	return TestStepResult();
	}

/* Starts the LbsNetGateway executable that is being tested.
 */
 TInt CLbsNrhStartNrhStep::StartNrh()
 	{
 	_LIT(KTestNrhFilename, "t_lbsnetworkrequesthandler.exe");
 	_LIT(KNrhServerName, "lbsnetworkrequesthandler.exe");
 	TServerStartParams params;
 	params.SetServerFileName(KTestNrhFilename);
 	params.SetServerName(KNrhServerName);
 	params.SetNumberOfServerSlots(KServerFrameworkDefaultMessageSlots);
 	params.SetServerUids(KNullUid, KNullUid, TUid::Uid(0x1028227B));
 	TInt r = CServerLaunch::ServerLaunch(params);
 	return r;
 	}
 
