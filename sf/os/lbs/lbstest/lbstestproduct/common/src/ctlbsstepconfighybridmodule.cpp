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
// This is the class implementation for the configuration of the AGPS Hybrid Module
// The main purpose of this test step is to allow flexible configurability of the AGps Hybrid Module
// from the Test Harness.
// Various configuration options are read from the ini files and sent to the AGps Hybrid Module through the TH channel.
// The AGps Hybrid Module always acknowledges each configuration message (see ProcessAGpsResponseMessage() method below).
// This test step has been written to understand '.ini' files containing sections such as [Config2] below:
// [Config2]
// agps_module_update_file = c:\testdata\configs\lbs_molr.ini
// agps_module_multi_updates_opt = on
// agps_module_assistance_data_opt= on
// $update,1,2,49.2,NAN,NAN,2,3*
// $update,1,2,49.2,NAN,NAN,2,3*
// $update,1,2,49.2,3.5,50,2,3*
// => Current module possible options are:
// agps_module_multi_updates_opt = [ on | off ]
// It sets the module to send multiple updates after Target Time.
// - When this mode is "off" only one update is sent just after 'Target time' (that is, the next update item available 
// from the module's update array).
// - When this mode is "on", the module sends N + 1 updates between 'Target time' and 'Max fix time'
// (currently hardcoded -> N = 2).
// The final update is the next update item available from the module's update array.
// The module calculates the first N updates from the final update, these updates contain incomplete
// positions (NAN longitude and latitude, ...). 
// Note:'Target time' specifies the time when the location update should be provided by the AGPS module.
// 'Max fix time' specifies the time maximum time the module should spend calculating the current location.
// agps_module_assistance_data_opt = [ on | off ]
// Only applicable when in Preferred Terminal Mode. 
// - When this mode is "on", for each new location request the AGps Module may Request new Assistance Data
// (if it has not been delivered already, sometimes assistance data is delivered to the module before it is requested).
// - When this mode is off, the default RequestAssistanceData(EAssistanceDataNone) call will always be made.		
// agps_module_dynamic_ass_data_opt = [ on | off ] (off by default)
// Switches between different assistance data delivery methods:
// -> OFF - On the first RequestAssistanceData requests the assistance data aDataMask is always set to
// the assistance data that it needs.  However for all subsequent assistance data requests the
// aDataMask will always be set to 0.  This is the method used in all the hybrid tests.
// -> ON - Here the module will always request any assistance data that it has not yet received. So on
// the first assistanceData request the aDataMask is set to the assistance data it requires.  On
// subsequent assistance data requests it will re-request the assistance data that has not yet
// been delivered.  This is the method that should be used in SUPL tests.
// agps_module_clear_all_opt = [ on | off ]
// Sets all the possible module options to on.
// agps_module_set_all_opt = [ on | off ]
// Sets all the possible module options to off.
// => $update command: 
// The $update entries in the ini files are used to populate the AGps Hybrid Module update array.
// Note: The Module's update array contains the update items used to generate the updates sent to the AGps Manager.
// The $update command syntax should be as follows:
// $update,num_of_updates,[num_of_measurements],[latitude],[longitude],[altitude],[horz_accuracy],[vert_accuracy],[update_err]*
// - where num_of_updates is the number of updates to add to the update array. For values greater than 1, each update
// will contain the same data position and measurement data as defined by the reminder of the sentence.
// - where num_of_measurements is the number of satellite measurements to define (see TPositionGpsMeasurementData). 
// If this value is 0 no measurement data is defined and only the position data is defined. 
// Values for the actual measurement data will be generated automatically.
// - where latitude, longitude, altitude, horz_accuracy, vert_accuracy are used to define the values for the 
// TPositionInfo part of the TPositionSatelliteInfo.
// - where update_err, is the error code to return to the manager for the update. If NOT defined the value will be KErrNone.
// => Possible Enhancements: This test step might be enhanced later on to also additional functionality such as:
// agps_module_update_error = -1
// agps_module_update_timeout = 5000000
// agps_module_multi_updates_opt = 2	// where 2 is the number of multi updates sent before  
// // the final update is sent prior to 'Max fix time'
// 
//
 
#include "ctlbsstepconfighybridmodule.h"

#include <lbs/test/tlbsutils.h>


_LIT(KUpdateFile, "agps_module_update_file");
_LIT(KClearAllOpt, "agps_module_clear_all_opt");
_LIT(KAssistanceDataOpt, "agps_module_assistance_data_opt");
_LIT(KMultiUpdateOpt, "agps_module_multi_updates_opt");
_LIT(KReqAssDatReqOpt, "agps_module_req_ass_data_req_opt");
_LIT(KSetAllOpt, "agps_module_set_all_opt");
_LIT(KSuplAssDataOpt, "agps_module_dynamic_ass_data_opt");
_LIT(KUpdateTimeout, "agps_module_update_timeout");
_LIT(KImmediateMeasurements, "agps_module_immediate_measurements");
_LIT(KOption_OFF, "off");
_LIT(KOption_ON, "on");
_LIT(KNoFileName, "");


/**
 * Constructor
 */
CT_LbsStep_ConfigHybridModule::CT_LbsStep_ConfigHybridModule(CT_LbsServer& aParent) : CT_LbsStep(aParent)
	{
	SetTestStepName(KLbsStep_ConfigHybridModule);
	}


/**
Static Constructor
*/
CT_LbsStep_ConfigHybridModule* CT_LbsStep_ConfigHybridModule::New(CT_LbsServer& aParent)
	{
	return new CT_LbsStep_ConfigHybridModule(aParent);
	// Note the lack of ELeave.
	// This means that having insufficient memory will return NULL;
	}

/**
 * @return - TVerdict code
 * Override of base class virtual
 */
TVerdict CT_LbsStep_ConfigHybridModule::doTestStepPreambleL()
	{
	INFO_PRINTF1(_L("Test Preamble. CT_LbsStep_ConfigHybridModule"));

	// create the test channel handler
	iAGpsHandler = CT_LbsAGpsHandler::NewL(this);
	
	SetTestStepResult(EPass);
	return TestStepResult();
	}

/**
 * @return - TVerdict code
 * Override of base class virtual
 */
TVerdict CT_LbsStep_ConfigHybridModule::doTestStepPostambleL()
	{
	INFO_PRINTF1(_L("Test Postamble. CT_LbsStep_ConfigHybridModule"));

	delete iAGpsHandler;
	iAGpsHandler = NULL;
		
	return TestStepResult();
	}
	
/**
 * @return - TVerdict code
 * Override of base class pure virtual
 * Our implementation only gets called if the base class doTestStepPreambleL() did
 * not leave. That being the case, the current test result value will be EPass.
 */
TVerdict CT_LbsStep_ConfigHybridModule::doTestStepL()
	{
	INFO_PRINTF1(_L("&gt;&gt;CT_LbsStep_ConfigHybridModule::doTestStepL()")); // ??

	if (TestStepResult() == EPass)
		{
		TPtrC configFileName;
		GetStringFromConfig(ConfigSection(), KUpdateFile, configFileName);
		if (configFileName != KNoFileName)
			{
			iConfigCtr++;
			iAGpsHandler->SendRequestUpdateInitMsg(configFileName, ConfigSection());		
			}
						
		TPtrC clearAllOpt;
		GetStringFromConfig(ConfigSection(), KClearAllOpt, clearAllOpt);
		if (clearAllOpt == KOption_ON)
			{
			iConfigCtr++;
			iAGpsHandler->SendRequestClearAllModuleOptions();
			}

		TPtrC updateOpt;
		GetStringFromConfig(ConfigSection(), KMultiUpdateOpt, updateOpt);
		if (updateOpt == KOption_OFF)
			{
			iConfigCtr++;
			iAGpsHandler->SendRequestModuleOption(ELbsHybridModuleOptions_MultiUpdatesOn, EFalse);
			}
		else if (updateOpt == KOption_ON)
			{
			iConfigCtr++;
			iAGpsHandler->SendRequestModuleOption(ELbsHybridModuleOptions_MultiUpdatesOn, ETrue);
			}
			
		TPtrC assDataOpt;
		GetStringFromConfig(ConfigSection(), KAssistanceDataOpt, assDataOpt);
		if (assDataOpt == KOption_OFF)
			{
			iConfigCtr++;
			iAGpsHandler->SendRequestModuleOption(ELbsHybridModuleOptions_AssistanceDataOn, EFalse);
			}
		else if (assDataOpt == KOption_ON)
			{
			iConfigCtr++;
			iAGpsHandler->SendRequestModuleOption(ELbsHybridModuleOptions_AssistanceDataOn, ETrue);
			}


		TPtrC reqAssDataOpt;
		GetStringFromConfig(ConfigSection(), KReqAssDatReqOpt, reqAssDataOpt);
		if (reqAssDataOpt == KOption_OFF)
			{
			iConfigCtr++;
			iAGpsHandler->SendRequestModuleOption(ELbsHybridModuleOptions_DisableReqAssistData, EFalse);
			}
		else if (reqAssDataOpt == KOption_ON)
			{
			iConfigCtr++;
			iAGpsHandler->SendRequestModuleOption(ELbsHybridModuleOptions_DisableReqAssistData, ETrue);
			}
		
		
		TPtrC suplAssDataOpt;
		GetStringFromConfig(ConfigSection(), KSuplAssDataOpt, suplAssDataOpt);
		if (suplAssDataOpt == KOption_OFF)
			{
			iConfigCtr++;
			iAGpsHandler->SendRequestModuleOption(ELbsHybridModuleOptions_DynamicAssistanceData, EFalse);
			}
		else if (suplAssDataOpt == KOption_ON)
			{
			iConfigCtr++;
			iAGpsHandler->SendRequestModuleOption(ELbsHybridModuleOptions_DynamicAssistanceData, ETrue);
			}



		TPtrC setAllOpt;
		GetStringFromConfig(ConfigSection(), KSetAllOpt, setAllOpt);
		if (setAllOpt == KOption_ON)
			{
			iConfigCtr++;
			iAGpsHandler->SendRequestSetAllModuleOptions();
			}
		TInt updateTimeout;
		if (GetIntFromConfig(ConfigSection(), KUpdateTimeout, updateTimeout))
			{
			TTimeIntervalMicroSeconds timeoutVal(updateTimeout);
			
			iConfigCtr++;
			iAGpsHandler->SendRequestTimeOutMsg(timeoutVal); 
			}		
		
		TInt immediateMeasurements;
		if (GetIntFromConfig(ConfigSection(), KImmediateMeasurements, immediateMeasurements))
			{
			iConfigCtr++;
			iAGpsHandler->SendImmediateMeasurementsMsg(immediateMeasurements); 
			}	
		// All the configuration requests sent, now start the active scheduler
		if (iConfigCtr)
			{
			CActiveScheduler::Start();
			}
		}
	
	
	INFO_PRINTF1(_L("&lt;&lt;CT_LbsStep_ConfigHybridModule::doTestStepL()"));	//??

	return TestStepResult();
	}


void CT_LbsStep_ConfigHybridModule::ProcessAGpsResponseMessage(const TT_LbsAGpsResponseMsg::TModuleResponseType aResponse)
	{
	iConfigCtr--;

	if (TT_LbsAGpsResponseMsg::EModuleResponseOk == aResponse)
		{
		// If any of the previous responses has failed do not override the verdict
		if (iVerdict != EFail)
			{
			iVerdict = EPass;
			}
		}
	else
		{
		iVerdict = EFail;
		}
	
	if (!iConfigCtr)
		{
		SetTestStepResult(iVerdict);
		CActiveScheduler::Stop();
		}
	}
