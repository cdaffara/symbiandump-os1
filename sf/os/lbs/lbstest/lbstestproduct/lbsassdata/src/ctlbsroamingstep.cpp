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
// @file ctlbsroamingstep.cpp
// This is the class implementation for the Roaming Tests
// 
//

//Etel includes
#include <etelmm.h>
#include <simtsy.h>

// LBS includes. 
#include <lbs.h>

// LBS test includes.
#include "ctlbsroamingstep.h"
#include <lbs/test/tlbsutils.h>

const TInt KAssistanceDataProviderPluginUidValue = 0x10281D77;


//Network mode enum in ini file
_LIT(KLbsNetworkModeEnum, "networkmode_enum");

//Network mode enum switch over to some other new value in ini file
_LIT(KLbsNetworkModeEnumSwitchOver, "networkmode_enumswitchoverto");
/**
Static Constructor
*/
CT_LbsRoamingStep* CT_LbsRoamingStep::New(CT_LbsAssDataServer& aParent)
	{
	// Note the lack of ELeave.
	// This means that having insufficient memory will return NULL;
	CT_LbsRoamingStep* testStep = new CT_LbsRoamingStep(aParent);
	if (testStep)
		{
		TInt err = KErrNone;

		TRAP(err, testStep->ConstructL());
		if (err)
			{
			delete testStep;
			testStep = NULL;
			}
		}
	return testStep;
	}


/**
 * Constructor
 */
CT_LbsRoamingStep::CT_LbsRoamingStep(CT_LbsAssDataServer& aParent) :CT_LbsNetSimStep(),iParent(aParent)
	{
	SetTestStepName(KLbsRoamingStep);
	}

void CT_LbsRoamingStep::ConstructL()
	{
	// Create the base class objects.
	CT_LbsNetSimStep::ConstructL();
	// Create and start the module data bus monitor, to listen for incoming data.	
	iModuleDataBusMonitor = CT_LbsAssDataModuleDataBusMonitor::NewL(this);
	
	// and the active object wrapper for the notify position update.
	iDoPosUpdate = CT_LbsDoPosUpdate::NewL(this);

	// create the admin
	iLbsAdmin = CLbsAdmin::NewL();
	
	}

/**
 * Destructor
 */
CT_LbsRoamingStep::~CT_LbsRoamingStep()
	{
	iModuleDataBusMonitor->Cancel();
	delete iModuleDataBusMonitor;
	
	iDoPosUpdate->Cancel();
	delete iDoPosUpdate;
	
	delete iLbsAdmin;
	}

/**
 * @return - TVerdict code
 * Override of base class pure virtual
 * Our implementation only gets called if the base class doTestStepPreambleL() did
 * not leave. That being the case, the current test result value will be EPass.
 */

TVerdict CT_LbsRoamingStep::doTestStepL()
	{
	// Generic test step used to test the LBS Roaming.
	INFO_PRINTF1(_L("&gt;&gt;CT_LbsRoamingStep::doTestStepL()"));

	if (TestStepResult() == EPass)
		{		
		// Expected callbacks flags, all tests require this
		iFlagsToHaltOn =  KLbsCallback_Got_NotifyUpdateAnswer |	KLbsCallback_Got_ModuleGpsModeAnswer;// Test module returned an answer.	
	  	
	  	//Module response status
		INFO_PRINTF1(_L("iModuleResponse ===> EModuleResponseWaiting"));
		iModuleResponse = EModuleResponseWaiting;
		
		TModuleDataIn modDataIn; // Used to send test information to the test module.
        
        //Request type is enabling reports of changes to the GPS options
		modDataIn.iRequestType = TModuleDataIn::EModuleRequestOptionsChangeReportMode;
		modDataIn.iOptionsChangeReportMode = TModuleDataIn::EGpsModuleDoReportOptionsChange;
		
	    // Carry out common test actions 
		T_LbsUtils utils;
		// Inform the test module of the required options - will block.
		utils.NotifyModuleOfConfigChangeL(modDataIn);
		// Carry out unique test actions.
		iModuleDataBusMonitor->StartMonitorL();
		//Enable RoamingSelflocate
		CLbsAdmin::TSelfLocateService selflocate;
		User::LeaveIfError(iLbsAdmin->Set(KLbsSettingRoamingSelfLocate, CLbsAdmin::ESelfLocateOn));
		User::LeaveIfError(iLbsAdmin->Get(KLbsSettingRoamingSelfLocate, selflocate));
		if(selflocate != CLbsAdmin::ESelfLocateOn)
		{
		SetTestStepResult(EFail);
		}
	    // Allow the setting to be propagated
	    User::After(2000000);		
		
		if (GetIntFromConfig(ConfigSection(), KTestCaseId, iTestCaseId))
			{
			switch (iTestCaseId)
				{
				// Test case LBS-Roaming-0001 - Switch over from Not registered to Home
				case 1:
				// Test case LBS-Roaming-0002 - Roaming GPS mode test
				case 2:
				// Test case LBS-Roaming-0003 - Switch over from Home to Roaming
				case 3:
				// Test case LBS-Roaming-0004 - Not registered on any network
				case 4: 
				// Test case LBS-Roaming-0005 - Registration status is unknown
				case 5:
				// Test case LBS-Roaming-0006 - Not registered make emergency calls only 
				case 6:
			    // Test case LBS-Roaming-0007 - Not registered, but searching	
				case 7:
				// Test case LBS-Roaming-0008 - Registered, network busy
				case 8:
				// Test case LBS-Roaming-0009 - Registration denied 
				case 9:
				// Test case LBS-Roaming-0009 - switch back to home network
				case 10:
					{
				    //Current Network Mode Enum 
                    TInt networkModeEnum;
	                //Network Mode Enum exist in ini file
	                TBool networkModeEnumExists;
				    //Get Network Registration status from ini file
				    networkModeEnumExists = GetIntFromConfig(ConfigSection(), KLbsNetworkModeEnum, networkModeEnum);
		            
		            //Verify Network mode Enum read succesful from ini file 
		            if(!networkModeEnumExists)
						{
						INFO_PRINTF1(_L("Could not get Network Mode Enum from ini file"));
			    		SetTestStepResult(EFail);
			    		}
				    					
					//Set Simtsy property registration status  
					User::LeaveIfError(RProperty::Set(KUidPSSimTsyCategory,KPSSimTsyRegStatChange,RMobilePhone::TMobilePhoneRegistrationStatus(networkModeEnum)));
					
	                if((iTestCaseId==3 || iTestCaseId==10)&& iModuleResponse==EModuleResponseWaiting )
		                {
		                // Connect to net sim.
	                	iNetSim.ConnectL(NULL);

                    	// Set the reference position, which also be used for any required verification.
	                    iRefPos.SetCoordinate(REFPOS_LAT, REFPOS_LONG, REFPOS_ALT);
	                    iRefPos.SetAccuracy(REFPOS_HORZ_ACCURACY, REFPOS_VERT_ACCURACY);
	                    iRefPos.SetCurrentTime();

	                    if (!iNetSim.SetReferenceLocation(iRefPos))
		                 {
                         INFO_PRINTF1(_L("Failed test, can't set NetSim's reference location."));
                         SetTestStepResult(EFail);
                         iNetSim.Close();
                         return TestStepResult();
                         }	
                    	// Set plugin to use.
		                TUid pluginUid = TUid::Uid(KAssistanceDataProviderPluginUidValue);
		                if (!iNetSim.SetAssistanceDataProvider(pluginUid))
				            {
				            INFO_PRINTF1(_L("Failed test, can't set NetSim's assistance data plugin uid."));
				            SetTestStepResult(EFail);
				            iNetSim.Close();
				            return TestStepResult();
				            }	
		                        		                
		                 }
		              
                      // Kick off pos update - this will invoke setgpsoptions 
					  // within the test module. The result of this will be reported back to
					  // the test step via the module data bus monitor.
					  iDoPosUpdate->StartL(iPositionHolder);		// lrm - passing a stack variable which goes out of scope!
					  //Move to the waiting state after kick off first pos update
		    	      
		    	      if((iTestCaseId==3 || iTestCaseId==1)  && iModuleResponse==EModuleResponseWaiting )
			    	      {
			    	      INFO_PRINTF1(_L("iState ===> EFirstPosUpdate"));
			    	      iState = EFirstPosUpdate;
			    	      }
		    	      else
			    	      {
			    	      INFO_PRINTF1(_L("iState ===> EWaiting"));
			    	      iState = EWaiting;
			    	      }
					  }
					 break;
			
			   default:
					{
					User::Panic(KLbsRoamingStep, KErrUnknown);
					}					
				}
			}
		
				
		// Kick off the keep alive timer.
		TTimeIntervalMicroSeconds32 interval(KLbsKeepAlivePeriod);

		iKeepAliveTimer->SetTimer(interval);

		// Kick off test.
		CActiveScheduler::Start();
		
		//disable reports of changes to the GPS options after completion of test
	    modDataIn.iOptionsChangeReportMode = TModuleDataIn::EGpsModuleDoNotReportOptionsChange;
	        
		iNetSim.Close();
		}

	INFO_PRINTF1(_L("&lt;&lt;CT_LbsRoamingStep::doTestStepL()"));

	return TestStepResult();
	}
    
   /**Notify position update callback.

	The notify position update has completed. We can mark as done in the callback flags.
   */
void CT_LbsRoamingStep::MT_LbsDoPosUpdateCallback(TRequestStatus& aStatus)
	{
	INFO_PRINTF1(_L("Got - Notify Update - Callback Event."));

	if(iTestCaseId!=3  && iTestCaseId!=1 )	
		{
		SetCallbackFlag(KLbsCallback_Got_NotifyUpdateAnswer);
		}
		   
    if((iTestCaseId==3 || iTestCaseId==1 ) && iState != EFirstPosUpdate) 
		{
		SetCallbackFlag(KLbsCallback_Got_NotifyUpdateAnswer);	
		}
	
	if (KErrNone != aStatus.Int())
		{
		INFO_PRINTF2(_L("Failed test, pos info request err = %d."), aStatus.Int());
		SetTestStepResult(EFail);
    	}
      
	if (iState == EFirstPosUpdate)
		{
		//Current Network Mode Enum 
		 TInt networkModeEnum;
		//Network Mode Enum exist in ini file
		TBool networkModeEnumExists;
		//Get Network Registration status(ERegisteredRoaming)from ini file
		networkModeEnumExists = GetIntFromConfig(ConfigSection(), KLbsNetworkModeEnumSwitchOver, networkModeEnum);
		//Verify Network mode Enum read succesful from ini file 
		if(!networkModeEnumExists)
			{
			INFO_PRINTF1(_L("Could not get Network Mode Enum from ini file"));
			SetTestStepResult(EFail);
			}
			
		//Set property registration status as Registered, roaming
		User::LeaveIfError(RProperty::Set(KUidPSSimTsyCategory,KPSSimTsyRegStatChange,RMobilePhone::TMobilePhoneRegistrationStatus(networkModeEnum)));
		
		
		if(iTestCaseId == 3)
			{
			// Going from Home to Roaming...the connections go down,
			// the previous MOLR session must terminate
	    	iNetSim.SetResponseError(KErrCancel, EFalse);
	    	User::After(1*1000*1000);
			}
		
		//jcm
		// wait until LBS has time to adjyst to the new registration status
		//User::After(1*1000*1000);
						    	
		if (iTestCaseId==1)	// Second NPUD uses PTB
			{
	        // Connect to net sim.
	    	iNetSim.ConnectL(NULL);

	    	// Set the reference position, which also be used for any required verification.
	        iRefPos.SetCoordinate(REFPOS_LAT, REFPOS_LONG, REFPOS_ALT);
	        iRefPos.SetAccuracy(REFPOS_HORZ_ACCURACY, REFPOS_VERT_ACCURACY);
	        iRefPos.SetCurrentTime();

	        if (!iNetSim.SetReferenceLocation(iRefPos))
		         {
		         INFO_PRINTF1(_L("Failed test, can't set NetSim's reference location."));
		         SetTestStepResult(EFail);
		         iNetSim.Close();
		         }	   
	        
	    	// Set plugin to use.
	        TUid pluginUid = TUid::Uid(KAssistanceDataProviderPluginUidValue);
	        if (!iNetSim.SetAssistanceDataProvider(pluginUid))
	            {
	            INFO_PRINTF1(_L("Failed test, can't set NetSim's assistance data plugin uid."));
	            SetTestStepResult(EFail);
	            iNetSim.Close();
	            }		        
	         }
		
		// Kick off pos update - this will invoke setgpsoptions 
		// within the test module. The result of this will be reported back to
		// the test step via the module data bus monitor.
		iDoPosUpdate->StartL(iPositionHolder);
		//Move to the waiting state after kick off first pos update
		INFO_PRINTF1(_L("iState ===> EWaiting"));
		iState = EWaiting;
		}
	}	
		
/** The module's output data bus has been updated.
	This function is called, for example, when the module sends back answers when gps mode updates */ 
void CT_LbsRoamingStep::HandleDataBusUpdateL(const TModuleDataOut& aModuleDataOut)
	{
	INFO_PRINTF4(_L("Got - Module Response - RequestType %x, Response %x, GpsMode %x."), aModuleDataOut.iRequestType, aModuleDataOut.iResponse, aModuleDataOut.iGpsMode);
    
    if(aModuleDataOut.iRequestType == TModuleDataOut::EModuleGpsMode)
		{
		switch (iTestCaseId)
			{
			case 1:
				switch (iModuleResponse)
					{
					case EModuleResponseWaiting:
						{
						//Update module response status
						INFO_PRINTF1(_L("iModuleResponse ===> EModuleResponseGotFirst"));
						iModuleResponse = EModuleResponseGotFirst; 	
						if((aModuleDataOut.iGpsMode != CLbsAdmin::EGpsAutonomous))	
							{
							INFO_PRINTF1(_L("Failed test, Gps mode not set correctly."));	
							SetTestStepResult(EFail);	
							}						
						}						
						break;
					case EModuleResponseGotFirst:
						{
				   	   	INFO_PRINTF1(_L("iModuleResponse ===> EModuleResponseGotSecond"));
						//Update module response status  
						iModuleResponse =EModuleResponseGotSecond;	
						SetCallbackFlag(KLbsCallback_Got_ModuleGpsModeAnswer);
						if((aModuleDataOut.iGpsMode != CLbsAdmin::EGpsPreferTerminalBased))	
							{
							INFO_PRINTF1(_L("Failed test, Gps mode not set correctly."));	
							SetTestStepResult(EFail);	
							}							
						}
						break;
					case EModuleResponseGotSecond:
						ASSERT(EFalse);
						break;
					}
				break;
				
			case 3:
				switch (iModuleResponse)
					{
					case EModuleResponseWaiting:
						{
						INFO_PRINTF1(_L("iModuleResponse ===> EModuleResponseGotFirst"));
						iModuleResponse = EModuleResponseGotFirst; 	
						SetCallbackFlag(KLbsCallback_Got_ModuleGpsModeAnswer);
					    if((aModuleDataOut.iGpsMode != CLbsAdmin::EGpsPreferTerminalBased))	
							{
							INFO_PRINTF1(_L("Failed test, Gps mode not set correctly."));	
							SetTestStepResult(EFail);	
						    }						
						}						
						break;
					case EModuleResponseGotFirst:
						{
				   	   	INFO_PRINTF1(_L("iModuleResponse ===> EModuleResponseGotFirst"));
						//Update module response status  
						iModuleResponse =EModuleResponseGotSecond;	
						SetCallbackFlag(KLbsCallback_Got_ModuleGpsModeAnswer);
						if((aModuleDataOut.iGpsMode != CLbsAdmin::EGpsAutonomous))	
							{
							INFO_PRINTF1(_L("Failed test, Gps mode not set correctly."));	
							SetTestStepResult(EFail);	
							}							
						}
						break;						
					case EModuleResponseGotSecond:
						ASSERT(EFalse);
						break;
					}				
				break;
				
			case 10:
				SetCallbackFlag(KLbsCallback_Got_ModuleGpsModeAnswer);
				if((aModuleDataOut.iGpsMode != CLbsAdmin::EGpsPreferTerminalBased))	
					{
					INFO_PRINTF1(_L("Failed test, Gps mode not set correctly."));	
					SetTestStepResult(EFail);	
					}				
				break;
				
			default:
				SetCallbackFlag(KLbsCallback_Got_ModuleGpsModeAnswer);
				if((aModuleDataOut.iGpsMode != CLbsAdmin::EGpsAutonomous))	
					{
					INFO_PRINTF1(_L("Failed test, Gps mode not set correctly."));	
					SetTestStepResult(EFail);	
					}				
				break;
			}
		}
		else if(aModuleDataOut.iResponse != TModuleDataOut::EModuleAssDataRequestedOk && aModuleDataOut.iResponse != TModuleDataOut::EModuleAssDataOk)	
			 {
			 INFO_PRINTF1(_L("Failed test,unexpected response from module "));
			 SetTestStepResult(EFail);
	       	 }  
  	}

