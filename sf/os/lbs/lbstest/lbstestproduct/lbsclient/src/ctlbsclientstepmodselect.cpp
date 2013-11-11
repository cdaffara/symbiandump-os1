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
// @file ctlbsclientstepmodselect.cpp
// This is the class implementation for the Module Selection Tests
// 
//
 
#include "ctlbsclientstepmodselect.h"

#include <lbs/test/tlbsutils.h>

/**
 * Destructor
 */
CT_LbsClientStep_ModSelect::~CT_LbsClientStep_ModSelect()
	{
	}


/**
 * Constructor
 */
CT_LbsClientStep_ModSelect::CT_LbsClientStep_ModSelect(CT_LbsClientServer& aParent) : CT_LbsClientStep(aParent)
	{
	iModuleId=TUid::Null();
	SetTestStepName(KLbsClientStep_ModSelect);
	}

/**
Static Constructor
*/
CT_LbsClientStep_ModSelect* CT_LbsClientStep_ModSelect::New(CT_LbsClientServer& aParent)
	{
	return new CT_LbsClientStep_ModSelect(aParent);
	// Note the lack of ELeave.
	// This means that having insufficient memory will return NULL;
	}


/**
 * Verifies that the last position retrieved was from the a-gps module
 */
TBool CT_LbsClientStep_ModSelect::VerifyLastModuleIdWasAGpsL()
	{
	TPositionInfo* posInfo = static_cast<TPositionInfo*>(iParent.iSharedData->iCurrentPosInfoArr[iParent.iSharedData->iCurrentPosInfoArr.Count()-1]);
	TPositionModuleId checkModId = posInfo->ModuleId();
	TPositionModuleId verifyModId;
	
	T_LbsUtils utils;
	
	verifyModId = utils.GetAGpsModuleIdL(iServer);
	
	if(checkModId == verifyModId)
		{
		INFO_PRINTF2(_L("Module IDs Match: Get AGPS Module Id %d"), checkModId.iUid);
		return ETrue;
		}
	else
		{
		INFO_PRINTF3(_L("Module IDs Do Not Match: Expected %d, Receive %d"), verifyModId.iUid, checkModId.iUid);
		return EFalse;
		}	
	}

/**
 * Verifies that the last position retrieved was from the network module
 */
TBool CT_LbsClientStep_ModSelect::VerifyLastModuleIdWasNetworkL()
	{
	TPositionInfo* posInfo = static_cast<TPositionInfo*>(iParent.iSharedData->iCurrentPosInfoArr[iParent.iSharedData->iCurrentPosInfoArr.Count()-1]);
	TPositionModuleId checkModId = posInfo->ModuleId();
	TPositionModuleId verifyModId;
	
	T_LbsUtils utils;
	verifyModId = utils.GetNetworkModuleIdL(iServer);
	
	if(checkModId == verifyModId)
		{
		INFO_PRINTF2(_L("Module IDs Match: Get Network Module Id %d"), checkModId.iUid);
		return ETrue;
		}
	else
		{
		INFO_PRINTF3(_L("Module IDs Do Not Match: Expected %d, Receive %d"), verifyModId, checkModId);
		return EFalse;
		}	
	
	}

TInt CT_LbsClientStep_ModSelect::Blocking_NotifyUpdateL()
	{
	
	TInt err;
	
	User::LeaveIfError(iPositioner.Open(iServer));
	INFO_PRINTF1(_L("Opened RPositioner successfully with RPositionServer"));
	CleanupClosePushL(iPositioner);

	err = Blocking_NotifyUpdateCommonL();
	
	CleanupStack::PopAndDestroy(&iPositioner);
	
	return err;
	}
	
TInt CT_LbsClientStep_ModSelect::Blocking_NotifyUpdateWithModuleIdL(TPositionModuleId aModuleId)
	{	
	TInt err;
	
	User::LeaveIfError(iPositioner.Open(iServer, aModuleId));
	INFO_PRINTF1(_L("Opened RPositioner successfully with RPositionServer and TPositionModuleId"));
	CleanupClosePushL(iPositioner);

	err = Blocking_NotifyUpdateCommonL();
	
	CleanupStack::PopAndDestroy(&iPositioner);
	
	return err;
	}


/**
 *	Opens a subsession with the requested criteria and retrieves a location update 
 *  which it appends to the current array of positions
 */
TInt CT_LbsClientStep_ModSelect::Blocking_NofifyUpdateWithCriteriaL(TPositionCriteria aCriteria)
	{
	TInt err;
	
	User::LeaveIfError(iPositioner.Open(iServer, aCriteria));
	INFO_PRINTF1(_L("Opened RPositioner successfully with RPositionServer and TPositionCriteria"));
	CleanupClosePushL(iPositioner);

	err = Blocking_NotifyUpdateCommonL();
	
	CleanupStack::PopAndDestroy(&iPositioner);
	
	return err;
	}


TInt CT_LbsClientStep_ModSelect::Blocking_NotifyUpdateCommonL()
	{
	TInt err = KErrNone;
	
	// Create a posinfo and store in our shared array for later verification.
	RPointerArray<TAny>& posInfoArr = iParent.iSharedData->iCurrentPosInfoArr;
	TPositionInfo* posInfo = new(ELeave) TPositionInfo();
	
	T_LbsUtils utils;
	utils.ResetAndDestroy_PosInfoArr(posInfoArr);	// Clear previous entries before new entry is appended.

	posInfoArr.Append(posInfo);
	
	err = DoNotifyUpdateL(*posInfo);
	
	return err;	
	}

/**
 * Connect to netsim and set up reference position
 * return KErrNone if successful
 */
TInt CT_LbsClientStep_ModSelect::SetupNetSimL()
	{
	TInt err = iNetSim.ConnectL(NULL); //connect to netsim but ingnore the callbacks
	if (err != KErrNone)
		{
		INFO_PRINTF2(_L("Cannot connect to netsim, return with error %d"), err);	
		return err;
		}
	else
		{
		//set up reference position
		RPointerArray<TAny>& srcPosInfoArr = iParent.iSharedData->iVerifyPosInfoArr;
		TPositionInfo* srcPosInfo = reinterpret_cast<TPositionInfo*>(srcPosInfoArr[0]);
		TPosition srcPos;
		srcPosInfo->GetPosition(srcPos);
		
		if (!iNetSim.SetReferenceLocation(srcPos))
			{
			INFO_PRINTF1(_L("Test Failed: can't set NetSim's reference location."));
			iNetSim.Close();
			return KErrGeneral;
			}
		// Set plugin to use for getting assistance data:
		TUid pluginUid;
		if(iParent.iSharedData->iTestModuleInUse)
			{
			pluginUid = TUid::Uid(KSimpleAssistanceDataProviderPluginUidValue);
			}
		else
			{
			pluginUid = TUid::Uid(KSuplAssistanceDataProviderPluginUidValue);
			}
		if (!iNetSim.SetAssistanceDataProvider(pluginUid))
			{
			INFO_PRINTF1(_L("Test Failed: Can't set NetSim's assistance data plugin uid."));
			SetTestStepResult(EFail);
			iNetSim.Close();
			return KErrGeneral;
			}
		}
	return KErrNone;
	}

/**
 * do notify position update with required postion criteria and verify the position 
 * received is from expected module
 */	
void CT_LbsClientStep_ModSelect::DoModuleSelectionL(TOpenSesstionMethod aMethod, TModuleType aModuleType)
	{
	TInt err = KErrNone;

	//do netsim connection and configure reference position
	err = SetupNetSimL();
	if (err !=KErrNone)
		{
		SetTestStepResult(EFail);
		return;
		}

	switch (aMethod)
		{
		case EWithCriteria:
			err = Blocking_NofifyUpdateWithCriteriaL(iCriteria);
			break;
			
		case EWithModuleId:
			err = Blocking_NotifyUpdateWithModuleIdL(iModuleId);
			break;
			
		case ENone:
			err = Blocking_NotifyUpdateL();
			break;
			
		default:
			break;
		}
	
	
	if(KErrNone == err)
		{
		// Verify that the last position was 
		// retrieved by the expected module: module ids not defined yet
		switch (aModuleType)
			{
			case EAGPSModule:
				if(!VerifyLastModuleIdWasAGpsL())
					{
					SetTestStepResult(EFail);
					}
			break;
			
			case ENetworkModule:
				if (!VerifyLastModuleIdWasNetworkL())
					{
					SetTestStepResult(EFail);
					break;
					}
			break;
			
			case ERandomModule:
				if (!VerifyLastModuleIdWasNetworkL() && !VerifyLastModuleIdWasAGpsL())
					{
					SetTestStepResult(EFail);
					}
			break;
			
			default:
			break;
			}
		
		}
	else
		{
		SetTestStepResult(EFail);
		}

	iNetSim.Close();
	}

/**
 * @return - TVerdict code
 * Override of base class pure virtual
 * Our implementation only gets called if the base class doTestStepPreambleL() did
 * not leave. That being the case, the current test result value will be EPass.
 */
TVerdict CT_LbsClientStep_ModSelect::doTestStepL()
	{
	// Generic test step used to test the LBS Client Notify position update API.
	INFO_PRINTF1(_L("&gt;&gt;CT_LbsClientStep_ModSelect::doTestStepL()"));

	if (TestStepResult()==EPass)
		{

		TInt err = KErrNone;
		
		// Connect to self locate server.
		User::LeaveIfError(iServer.Connect());
		CleanupClosePushL(iServer);
		
		//reset the default value
		iCriteria.ClearCriteria();
		iQuality = TPositionQuality();
		iSelectOrder.ClearSelectionOrder(); 
		
		TInt testCaseId;
		if (GetIntFromConfig(ConfigSection(), KTestCaseId, testCaseId))
			{
			switch (testCaseId)
				{
				case 01:	// test module selection by capabilities 
							// LBS-Capability-0001
					{
					iCriteria.AddRequiredCapabilities(TPositionModuleInfo::ECapabilitySatellite);
					DoModuleSelectionL(EWithCriteria, EAGPSModule);			
					} 
					break;

				case 03:	// test module selection by unsupported capabilities 
							// LBS-Capability-0003
					{
					// Set capabilities not supported by either (gps/network) module:
					iCriteria.AddRequiredCapabilities(TPositionModuleInfo::ECapabilityMedia);
					err = iPositioner.Open(iServer, iCriteria);
					if(KErrNotFound != err)
						{
						INFO_PRINTF2(_L("Test failed: wrong error returned %d"), err);
						SetTestStepResult(EFail);
						if(KErrNone == err)
							{
							iPositioner.Close();
							}						
						}
					
					iCriteria.ClearCriteria();
					User::LeaveIfError(iPositioner.Open(iServer, iCriteria));
					iPositioner.Close();
					}  
					break;
					
				case 04: //required more than one capability
						 //LBS-Capability-0004	
					{
					iCriteria.AddRequiredCapabilities(TPositionModuleInfo::ECapabilityHorizontal);
					iCriteria.AddRequiredCapabilities(TPositionModuleInfo::ECapabilitySatellite);
					DoModuleSelectionL(EWithCriteria, EAGPSModule);	
					} 
					break;
					
				case 11:	// test module selection by horizontal/vertical accuracy 
							// LBS-Quality-0001
					{
					// set up the required accuracy
					iQuality.SetHorizontalAccuracy(AGPS_MODINFO_HORIZONTAL_ACCURACY);
					iQuality.SetVerticalAccuracy(AGPS_MODINFO_VERTICAL_ACCURACY);
					
					iCriteria.SetRequiredQuality(iQuality);
					
					DoModuleSelectionL(EWithCriteria, EAGPSModule);		
					}
					break;
					
				case 12:	// test module selection by time to first fix 
							// LBS-Quality-0002
					{
					TTimeIntervalMicroSeconds timetofirstfix(NETWORK_MODINFO_TIME_TO_FIRST_FIX);
					 
					// set up the required TTFF
					iQuality.SetTimeToFirstFix(timetofirstfix);
					
					iCriteria.SetRequiredQuality(iQuality);
					
					DoModuleSelectionL(EWithCriteria, ENetworkModule);		
					}
					break;
					
				case 13:	// test module selection by power consumption 
							// LBS-Quality-0003
					// this test case can only be run in test module
					// in xtestmodule.ini powerconsumption need to be changed to 1
					{
					if (!iParent.iSharedData->iTestModuleInUse)
						{
						INFO_PRINTF1(_L("This test case can ONLY be run with Test A-GPS Module"));
						User::Panic(_L("Client Module Selection"), 1);
						}
					iQuality.SetPowerConsumption(TPositionQuality::EPowerLow);
					iCriteria.SetRequiredQuality(iQuality);	
					
					//because both modules have the same power consumption value
					//agps module will be selected by default
					DoModuleSelectionL(EWithCriteria, ENetworkModule);
					}			
					break;
								
				case 14:	// test module selection by cost limit 
							// LBS-Quality-0004
					{
					iQuality.SetCostIndicator(AGPS_MODINFO_COST_INDICATOR);
					iCriteria.SetRequiredQuality(iQuality);
					
					DoModuleSelectionL(EWithCriteria, EAGPSModule);	
					}					
					break;
				
				case 15: 	// setting a higher accuracy than all existing modules for module selection
							// LBS-Quality-0005
					{
					//set an required horizontal accuracy as 1 meter
					iQuality.SetHorizontalAccuracy(1); 	
					iCriteria.SetRequiredQuality(iQuality);
					err = iPositioner.Open(iServer, iCriteria);
					if(KErrNotFound != err)
						{
						INFO_PRINTF2(_L("Test failed: wrong error returned %d"), err);
						SetTestStepResult(EFail);
						if(KErrNone == err)
							{
							iPositioner.Close();
							}						
						}
					}
					break;
						
 				case 16:
 							// LBS-Quality-0006 - TTFF lower than module's and H-Acc lower (more accurate) than module's. Specified V-Acc is met by module.
 					{
 					//set required TTFF=20s, TTNF=1, H-Acc=10m, V-Acc=200m
 					iQuality.SetTimeToFirstFix(10000000);
 					iQuality.SetTimeToNextFix(1000000);
 					iQuality.SetHorizontalAccuracy(10); 
 					iQuality.SetVerticalAccuracy(200);
 										
 					iCriteria.SetRequiredQuality(iQuality);
 					err = iPositioner.Open(iServer, iCriteria);
 					if(KErrNotFound != err)
 						{
 						INFO_PRINTF2(_L("Test failed: wrong error returned %d"), err);
 						SetTestStepResult(EFail);
 						if(KErrNone == err)
 							{
 							iPositioner.Close();
 							}						
 						}
 					}
 					break;
 					
 				case 17:
 							// LBS-Quality-0007 - TTFF lower than module's and H-Acc/V-Acc lower than module's. Power and Cost match module's
 					{
 					//set required TTFF=20s, TTNF=1, H-Acc=10m, V-Acc=10m
 					iQuality.SetTimeToFirstFix(10000000);
 					iQuality.SetTimeToNextFix(1000000);
 					iQuality.SetHorizontalAccuracy(10); 
 					iQuality.SetVerticalAccuracy(10);
 					
 					iQuality.SetCostIndicator(TPositionQuality::ECostCharge);	
 					iQuality.SetPowerConsumption(TPositionQuality::EPowerMedium);
 					
 					iCriteria.SetRequiredQuality(iQuality);
 					err = iPositioner.Open(iServer, iCriteria);
 					if(KErrNotFound != err)
 						{
 						INFO_PRINTF2(_L("Test failed: wrong error returned %d"), err);
 						SetTestStepResult(EFail);
 						if(KErrNone == err)
 							{
 							iPositioner.Close();
 							}						
 						}
 					}
 					break;
						
				case 21:	// Request Location update with default request ordering 
							// LBS-SelectionOrder-0001 
					{
					DoModuleSelectionL(ENone,EAGPSModule);					
					}
					break;
						
				case 22:	// Request location update with cost/time/accuracy/power mgmt in order of importance
					{		// LBS-SelectionOrder-0002
					User::LeaveIfError(iSelectOrder.SetOrderCostIndicator(TPositionSelectionOrder::EOrderVeryHigh));
					User::LeaveIfError(iSelectOrder.SetOrderTimeToFirstFix(TPositionSelectionOrder::EOrderHigh));
					User::LeaveIfError(iSelectOrder.SetOrderHorizontalAccuracy(TPositionSelectionOrder::EOrderFairlyHigh));
					User::LeaveIfError(iSelectOrder.SetOrderPowerConsumption(TPositionSelectionOrder::EOrderMedium));
					
					iCriteria.SetSelectionOrder(iSelectOrder);
					
					DoModuleSelectionL(EWithCriteria, EAGPSModule);
					}
					break;
							
							// Test requires test A-GPS module to have accuracy of <100m. This must be better than Network Module.
				case 32:	// Check that we can fall back on the selection order process (stage 3) when capabilities and qualities of both modules are equal.
					{		// LBS-ModuleSelection-0002 
					iQuality.SetHorizontalAccuracy(500);
					iCriteria.SetRequiredQuality(iQuality);
					
					DoModuleSelectionL(EWithCriteria, EAGPSModule);
					}
					break;
						
							// Test requires test A-GPS module and Network Module to have equal horizontal accuracy
				case 33:	// Check that we can ack on the Rpositioner Framework when both modules appear identical.
					{		// LBS-ModuleSelection-0003
					if (!iParent.iSharedData->iTestModuleInUse)
						{
						INFO_PRINTF1(_L("This test case can ONLY be run with Test A-GPS Module"));
						User::Panic(_L("Client Module Selection"), 1);
						}
					iQuality.SetHorizontalAccuracy(100);
					iCriteria.SetRequiredQuality(iQuality);
					
					User::LeaveIfError(iSelectOrder.SetOrderHorizontalAccuracy(TPositionSelectionOrder::EOrderVeryHigh));
					iCriteria.SetSelectionOrder(iSelectOrder);
					
					DoModuleSelectionL(EWithCriteria, ERandomModule);			
					}
					break;
						
				case 34:	// Select default module on connecting to the RPositioner
					{		// LBS-ModuleSelection-0004 
					DoModuleSelectionL(EWithCriteria, EAGPSModule);
					}
					break;	
					
					
				case 35:	// Select A-GPS module by its ID
					{		// LBS-ModuleSelection-0005					
					iModuleId.iUid = APGS_MODINFO_MODULE_ID;
					DoModuleSelectionL(EWithModuleId, EAGPSModule);				
					}
					break;	
					
					
				case 36: 	// Select Network module by its ID
					{		// LBS-ModuleSelection-0006		
					iModuleId.iUid = NETWORK_MODINFO_MODULE_ID;
					DoModuleSelectionL(EWithModuleId, ENetworkModule);				
					}
					break;	
					
				case 37: // Test requires test A-GPS module and Network Module to have equal power consumption
						 // when both modules match the iCriteria, check default module is used for position update
						 // LBS-ModuleSelection-0007
					{
					iQuality.SetPowerConsumption(TPositionQuality::EPowerLow);
					iCriteria.SetRequiredQuality(iQuality);
					
					User::LeaveIfError(iSelectOrder.SetOrderPowerConsumption(TPositionSelectionOrder::EOrderVeryHigh));
					iCriteria.SetSelectionOrder(iSelectOrder);
					
					DoModuleSelectionL(EWithCriteria, ERandomModule);
					}
					break;
					
				case 38:
						//LBS-ModuleSelection-0008
					{
					if (!iParent.iSharedData->iTestModuleInUse)
						{
						INFO_PRINTF1(_L("This test case can ONLY be run with Test A-GPS Module"));
						User::Panic(_L("Client Module Selection"), 1);
						}
					iQuality.SetHorizontalAccuracy(100);
					iCriteria.SetRequiredQuality(iQuality);
					
					User::LeaveIfError(iSelectOrder.SetOrderHorizontalAccuracy(TPositionSelectionOrder::EOrderVeryHigh));
					User::LeaveIfError(iSelectOrder.SetOrderPowerConsumption(TPositionSelectionOrder::EOrderVeryHigh));
					iCriteria.SetSelectionOrder(iSelectOrder);
					
					// Verify that the last position was retrieved by a-gps module, which has lower power consumption.
					DoModuleSelectionL(EWithCriteria, ENetworkModule);
					}
					break;
							
				default:
					{
					INFO_PRINTF1(_L("Invalid Test Case"));
					User::Leave(KErrArgument);
					}	
				}	
			}	
		CleanupStack::PopAndDestroy(&iServer);	
		} // end if TestStepResult
	INFO_PRINTF1(_L("&lt;&lt;CT_LbsClientStep_ModSelect::doTestStepL()"));

	return TestStepResult();
	}

