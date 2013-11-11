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
 @file LbsAdminModifierStep.cpp
*/
#include "lbsadminmodifierstep.h"
#include "te_lbsadminsuitedefs.h"
#include <lbs/lbsadmin.h>



CLbsAdminModifierStep::~CLbsAdminModifierStep()
	{
	if(iLbsAdmin)
		{
		iLbsAdmin->ResetToDefault();
		delete iLbsAdmin;
		}
	}

CLbsAdminModifierStep::CLbsAdminModifierStep()
/**
 * Constructor
 */
	{
	// **MUST** call SetTestStepName in the constructor as the controlling
	// framework uses the test step name immediately following construction to set
	// up the step's unique logging ID.
	SetTestStepName(KLbsAdminModifierStep);
	}

TVerdict CLbsAdminModifierStep::doTestStepPreambleL()
/**
 * @return - TVerdict code
 * Override of base class virtual
 */
	{
    CTe_LbsAdminSuiteStepBase::doTestStepPreambleL();
	if (TestStepResult()!=EPass)
	   return   TestStepResult();
	// Create an admin API 
	//
	iLbsAdmin = CLbsAdmin::NewL();
	SetTestStepResult(EPass);
	return TestStepResult();
	}


TVerdict CLbsAdminModifierStep::doTestStepL()
/**
 * @return - TVerdict code
 * Override of base class pure virtual
 * Our implementation only gets called if the base class doTestStepPreambleL() did
 * not leave. That being the case, the current test result value will be EPass.
 */
	{
	  if (TestStepResult()==EPass)
		{
		// Change all of the properties in each iteration.
		// Do this as many times as properties there are.
        for (TInt i = 0; i <= KNumberOfProperties; i++)
			{
		  	// Wait to be allowed to modify settings by the Observer step
			iModifierSem.Wait();
		    INFO_PRINTF1(_L("...Modifying settings"));
			// Reset settings when i is even. Set all settings to some value when
			// i is odd.
			if (!i%2)
				{
				User::LeaveIfError(iLbsAdmin->ResetToDefault());
				}
			else
				{
				User::LeaveIfError(iLbsAdmin->Set(KLbsSettingHomeSelfLocate, CLbsAdmin::ESelfLocateOn));
				User::LeaveIfError(iLbsAdmin->Set(KLbsSettingRoamingSelfLocate, CLbsAdmin::ESelfLocateOn));
				TLbsQualityProfileId profileId = 1;
				User::LeaveIfError(iLbsAdmin->Set(KLbsSettingQualityProfileSelfLocate, profileId));
				User::LeaveIfError(iLbsAdmin->Set(KLbsSettingHomeExternalLocate, CLbsAdmin::EExternalLocateOn));
				User::LeaveIfError(iLbsAdmin->Set(KLbsSettingRoamingExternalLocate, CLbsAdmin::EExternalLocateOn));
				User::LeaveIfError(iLbsAdmin->Set(KLbsSettingQualityProfileExternalLocate, profileId));
				User::LeaveIfError(iLbsAdmin->Set(KLbsSettingHomeEmergencyLocate, CLbsAdmin::EExternalLocateOn));
				User::LeaveIfError(iLbsAdmin->Set(KLbsSettingRoamingEmergencyLocate, CLbsAdmin::EExternalLocateOn));
				User::LeaveIfError(iLbsAdmin->Set(KLbsSettingHomeTransmitLocate, CLbsAdmin::ETransmitLocateOn));
				User::LeaveIfError(iLbsAdmin->Set(KLbsSettingRoamingTransmitLocate, CLbsAdmin::ETransmitLocateOn));
				User::LeaveIfError(iLbsAdmin->Set(KLbsSettingQualityProfileTransmitLocate, profileId));
				User::LeaveIfError(iLbsAdmin->Set(KLbsSettingHomeGpsMode, CLbsAdmin::EGpsPreferTerminalBased));
				User::LeaveIfError(iLbsAdmin->Set(KLbsSettingRoamingGpsMode, CLbsAdmin::EGpsPreferTerminalBased));
				TLbsProtocolModuleId protocolModule = TUid::Uid(1);
				User::LeaveIfError(iLbsAdmin->Set(KLbsSettingHomeProtocolModule, protocolModule));
				User::LeaveIfError(iLbsAdmin->Set(KLbsSettingRoamingProtocolModule, protocolModule));
				User::LeaveIfError(iLbsAdmin->Set(KLbsSettingLbsSystemStartMode, CLbsAdmin::ELbsSystemPersistent));
				User::LeaveIfError(iLbsAdmin->Set(KLbsSettingBehaviourMode, CLbsAdmin::ELbsBehaviourCustom1));
				User::LeaveIfError(iLbsAdmin->Set(KLbsSettingPrivacyAppTimeout, static_cast<TLbsTimeIntervalMilliSeconds>(4567)));				
				User::LeaveIfError(iLbsAdmin->Set(KLbsSettingPrivacyTimeoutAction, CLbsAdmin::EPrivacyTimeoutNetworkDefined));
				User::LeaveIfError(iLbsAdmin->Set(KLbsSettingPrivacyHandler, CLbsAdmin::EPrivacyHandleByController));
				User::LeaveIfError(iLbsAdmin->Set(KLbsSettingLogger, CLbsAdmin::ELoggerOff));
				TLbsSetClockModuleId clockModule = TUid::Uid(23);
				User::LeaveIfError(iLbsAdmin->Set(KLbsSettingSetClockModule, clockModule));
				User::LeaveIfError(iLbsAdmin->Set(KLbsSettingClockAdjustInterval, static_cast<TLbsTimeIntervalMilliSeconds>(987654321)));
				User::LeaveIfError(iLbsAdmin->Set(KLbsSettingClockAdjustThreshold, static_cast<TLbsTimeIntervalMilliSeconds>(1234)));
				User::LeaveIfError(iLbsAdmin->Set(KLbsSettingClockAdjust, CLbsAdmin::EClockAdjustOn));
				User::LeaveIfError(iLbsAdmin->Set(KLbsSettingAllowManualClockAdjust, CLbsAdmin::EClockAdjustOn));
				User::LeaveIfError(iLbsAdmin->Set(KLbsSettingHomeNetworkInducedLocate, CLbsAdmin::EExternalLocateOn));
				User::LeaveIfError(iLbsAdmin->Set(KLbsSettingRoamingNetworkInducedLocate, CLbsAdmin::EExternalLocateOn));
				}
				
				// Test once (when i=1) that the setting values have been set correctly 
				//  
				if (i==1)
				{
				CLbsAdmin::TSelfLocateService selfLocateService;
				User::LeaveIfError(iLbsAdmin->Get(KLbsSettingHomeSelfLocate, selfLocateService));
				TESTL(selfLocateService == CLbsAdmin::ESelfLocateOn);
				User::LeaveIfError(iLbsAdmin->Get(KLbsSettingHomeSelfLocate, selfLocateService));
				TESTL(selfLocateService == CLbsAdmin::ESelfLocateOn);
				
				TLbsQualityProfileId readProfileId;
				User::LeaveIfError(iLbsAdmin->Get(KLbsSettingQualityProfileSelfLocate, readProfileId));
				TESTL(readProfileId == 1);
				
				CLbsAdmin::TExternalLocateService externalLocateService;
				User::LeaveIfError(iLbsAdmin->Get(KLbsSettingHomeExternalLocate, externalLocateService));
				TESTL(externalLocateService == CLbsAdmin::EExternalLocateOn);
				
				User::LeaveIfError(iLbsAdmin->Get(KLbsSettingRoamingExternalLocate, externalLocateService));
				TESTL(externalLocateService == CLbsAdmin::EExternalLocateOn);
				
				User::LeaveIfError(iLbsAdmin->Get(KLbsSettingQualityProfileExternalLocate, readProfileId));
				TESTL(readProfileId == 1);
				
				User::LeaveIfError(iLbsAdmin->Get(KLbsSettingHomeEmergencyLocate, externalLocateService));
				TESTL(externalLocateService == CLbsAdmin::EExternalLocateOn);
				
				User::LeaveIfError(iLbsAdmin->Get(KLbsSettingRoamingEmergencyLocate, externalLocateService));
				TESTL(externalLocateService == CLbsAdmin::EExternalLocateOn);
				
				CLbsAdmin::TTransmitLocateService transmitLocateService;
				User::LeaveIfError(iLbsAdmin->Get(KLbsSettingHomeTransmitLocate, transmitLocateService));
				TESTL(transmitLocateService == CLbsAdmin::ETransmitLocateOn);
				
				User::LeaveIfError(iLbsAdmin->Get(KLbsSettingRoamingTransmitLocate, transmitLocateService));
				TESTL(transmitLocateService == CLbsAdmin::ETransmitLocateOn);
				
				User::LeaveIfError(iLbsAdmin->Get(KLbsSettingQualityProfileTransmitLocate, readProfileId));
				TESTL(readProfileId == 1);
				
				CLbsAdmin::TGpsMode gpsMode;
				User::LeaveIfError(iLbsAdmin->Get(KLbsSettingHomeGpsMode, gpsMode));
				TESTL(gpsMode == CLbsAdmin::EGpsPreferTerminalBased);
				
				User::LeaveIfError(iLbsAdmin->Get(KLbsSettingRoamingGpsMode, gpsMode));
				TESTL(gpsMode == CLbsAdmin::EGpsPreferTerminalBased);
				
				TLbsProtocolModuleId protocolModule;
				User::LeaveIfError(iLbsAdmin->Get(KLbsSettingHomeProtocolModule, protocolModule));
				TESTL(protocolModule.iUid == 1);
		
				User::LeaveIfError(iLbsAdmin->Get(KLbsSettingRoamingProtocolModule, protocolModule));
				TESTL(protocolModule.iUid == 1);				
				
				CLbsAdmin::TLbsSystemStartMode startupMode;
				User::LeaveIfError(iLbsAdmin->Get(KLbsSettingLbsSystemStartMode, startupMode));
				TESTL(startupMode == CLbsAdmin::ELbsSystemPersistent);
				
				CLbsAdmin::TLbsBehaviourMode behaviourMode;
				User::LeaveIfError(iLbsAdmin->Get(KLbsSettingBehaviourMode, behaviourMode));
				TESTL(behaviourMode == CLbsAdmin::ELbsBehaviourCustom1);

				TLbsTimeIntervalMilliSeconds timeout;
				User::LeaveIfError(iLbsAdmin->Get(KLbsSettingPrivacyAppTimeout, timeout));
				TESTL(timeout == 4567);
		
				CLbsAdmin::TPrivacyTimeoutAction privacyTimeoutAction;
				User::LeaveIfError(iLbsAdmin->Get(KLbsSettingPrivacyTimeoutAction, privacyTimeoutAction));
				TESTL(privacyTimeoutAction == CLbsAdmin::EPrivacyTimeoutNetworkDefined);
				
				CLbsAdmin::TPrivacyHandler privacyHandler;
				User::LeaveIfError(iLbsAdmin->Get(KLbsSettingPrivacyHandler, privacyHandler));
				TESTL(privacyHandler == CLbsAdmin::EPrivacyHandleByController);
			
				CLbsAdmin::TLogger logger;
				User::LeaveIfError(iLbsAdmin->Get(KLbsSettingLogger, logger));
				TESTL(logger == CLbsAdmin::ELoggerOff);
				
				TLbsSetClockModuleId clockModule;
				User::LeaveIfError(iLbsAdmin->Get(KLbsSettingSetClockModule, clockModule));
				TESTL(clockModule.iUid == 23);
				
				TLbsTimeIntervalMilliSeconds interval;
				User::LeaveIfError(iLbsAdmin->Get(KLbsSettingClockAdjustInterval, interval));
				TESTL(interval == 987654321);
				
				TLbsTimeIntervalMilliSeconds threshold;
				User::LeaveIfError(iLbsAdmin->Get(KLbsSettingClockAdjustThreshold, threshold));
				TESTL(threshold == 1234);

				CLbsAdmin::TClockAdjust clockAdjust;
				User::LeaveIfError(iLbsAdmin->Get(KLbsSettingClockAdjust, clockAdjust));
				TESTL(clockAdjust == CLbsAdmin::EClockAdjustOn);
				
				CLbsAdmin::TClockAdjust clockManAdjust;
				User::LeaveIfError(iLbsAdmin->Get(KLbsSettingAllowManualClockAdjust, clockManAdjust));
				TESTL(clockManAdjust == CLbsAdmin::EClockAdjustOn);		
				
				CLbsAdmin::TSpecialFeature ignoreAccuracy;
				User::LeaveIfError(iLbsAdmin->Get(KLbsSpecialFeatureIgnoreAccuracy, ignoreAccuracy));
				TESTL(ignoreAccuracy == CLbsAdmin::ESpecialFeatureOff);
				INFO_PRINTF2(_L("ignoreAccuracy = 0x%x"), ignoreAccuracy);

				User::LeaveIfError(iLbsAdmin->Get(KLbsSettingHomeNetworkInducedLocate, externalLocateService));
				TESTL(externalLocateService == CLbsAdmin::EExternalLocateOn);
				
				User::LeaveIfError(iLbsAdmin->Get(KLbsSettingRoamingNetworkInducedLocate, externalLocateService));
				TESTL(externalLocateService == CLbsAdmin::EExternalLocateOn);
				
				TLbsTimeIntervalMiliSeconds maximumRequestDelay;
				User::LeaveIfError(iLbsAdmin->Get(KLbsSpecialFeatureMaximumRequestDelay, maximumRequestDelay));
				TESTL(maximumRequestDelay == 0);
				}

			//Change done: allow observer step to run
			//
			INFO_PRINTF1(_L("SIGNAL Observer..."));
			iObserverSem.Signal();
			} 
		iModifierSem.Wait();
		
		
		INFO_PRINTF1(_L("Modifier run before leaving"));
		
		//test case for defect INC101055
		TInt err;
		TLbsQualityProfileId incorrectProfileId = 100;
		err = iLbsAdmin->Set(KLbsSettingQualityProfileSelfLocate, incorrectProfileId);
		if (KErrArgument != err)
			{
			INFO_PRINTF3(_L("Validate Profile Id Test Case FAILED, Expected %d, Received %d"), KErrArgument, err);
			SetTestStepResult(EFail);
			}
		err = iLbsAdmin->Set(KLbsSettingQualityProfileExternalLocate, incorrectProfileId);
		if (KErrArgument != err)
			{
			INFO_PRINTF3(_L("Validate Profile Id Test Case FAILED, Expected %d, Received %d"), KErrArgument, err);
			SetTestStepResult(EFail);
			}
		err = iLbsAdmin->Set(KLbsSettingQualityProfileTransmitLocate, incorrectProfileId);
		if (KErrArgument != err)
			{
			INFO_PRINTF3(_L("Validate Profile Id Test Case FAILED, Expected %d, Received %d"), KErrArgument, err);
			SetTestStepResult(EFail);
			}
		
		
		
		//LbsAdmin should not allow Set() larger than KMaxTInt for Int settings
		TLbsAdminSetting adminIntSettings[] = {KLbsSettingClockAdjustThreshold,
												KLbsSettingClockAdjustInterval,
												KLbsSettingMaximumExternalLocateRequests,
												KLbsSettingPrivacyShutdownDelay,
												KLbsSettingPrivacyMaxNetworkSessions,
												KLbsSettingPrivacyConnectionTimeout,
												KLbsSettingPrivacyAppTimeout};
		TInt adminIntSettingsLength = sizeof(adminIntSettings)/sizeof(adminIntSettings[0]);
	    for(TInt i = 0; i < adminIntSettingsLength;i++)
	    	{
	    	TUint testVal = 1;
   			err = iLbsAdmin->Set(adminIntSettings[i], testVal);
   			if (KErrNone != err)
   				{
   				INFO_PRINTF3(_L("Validate Int Size Test Case FAILED, Expected %d, Received %d"), KErrArgument, err);
   				SetTestStepResult(EFail);
   				}
   			
	    	testVal = (TUint)-1;
   			err = iLbsAdmin->Set(adminIntSettings[i], testVal);
   			if (KErrArgument != err)
   				{
   				INFO_PRINTF3(_L("Validate Int Size Test Case FAILED, Expected %d, Received %d"), KErrArgument, err);
   				SetTestStepResult(EFail);
   				}
   		
	    	testVal =  6417; // Normal number
   			err = iLbsAdmin->Set(adminIntSettings[i], testVal);
   			if (KErrNone != err)
   				{
   				INFO_PRINTF3(_L("Validate Int Size Test Case FAILED, Expected %d, Received %d"), KErrArgument, err);
   				SetTestStepResult(EFail);
   				}
   			
	    	testVal =  (TUint)KMaxTInt; //2147483647
   			err = iLbsAdmin->Set(adminIntSettings[i], testVal);
   			if (KErrNone != err)
   				{
   				INFO_PRINTF3(_L("Validate Int Size Test Case FAILED, Expected %d, Received %d"), KErrArgument, err);
   				SetTestStepResult(EFail);
   				}
   			
	    	testVal =  (TUint)KMaxTInt+1; //2147483648
   			err = iLbsAdmin->Set(adminIntSettings[i], testVal);
   			if (KErrArgument != err)
   				{
   				INFO_PRINTF3(_L("Validate Int Size Test Case FAILED, Expected %d, Received %d"), KErrArgument, err);
   				SetTestStepResult(EFail);
   				}
   		
	    	testVal = (TUint)KMaxTInt+3; //2147483651
   			err = iLbsAdmin->Set(adminIntSettings[i], testVal);
   			if (KErrArgument != err)
   				{
   				INFO_PRINTF3(_L("Validate Int Size Test Case FAILED, Expected %d, Received %d"), KErrArgument, err);
   				SetTestStepResult(EFail);
   				}
	    	}
		
		
		
		}
		
	  return TestStepResult();
	}



TVerdict CLbsAdminModifierStep::doTestStepPostambleL()
/**
 * @return - TVerdict code
 * Override of base class virtual
 */
	{
	CTe_LbsAdminSuiteStepBase::doTestStepPostambleL();
	return TestStepResult();
	}
