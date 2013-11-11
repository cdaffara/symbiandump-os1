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
#include "multipleprotocolmodules.h"
#include "te_lbsadminsuitedefs.h"
#include <lbs/lbsadmin.h>



CMultipleProtocolModules::~CMultipleProtocolModules()
	{
	if(iLbsAdmin)
		{
		iLbsAdmin->ResetToDefault();
		delete iLbsAdmin;
		}
	}

/**
 * Constructor
 */
CMultipleProtocolModules::CMultipleProtocolModules()
	{
	// **MUST** call SetTestStepName in the constructor as the controlling
	// framework uses the test step name immediately following construction to set
	// up the step's unique logging ID.
	SetTestStepName(KMultipleProtocolModules);
	}

/**
 * @return - TVerdict code
 * Override of base class virtual
 */
TVerdict CMultipleProtocolModules::doTestStepPreambleL()
	{
 
    if(TestStepResult() == EPass)
    {
    iLbsAdmin = CLbsAdmin::NewL();
    }    
	return TestStepResult();
	}

/**
 * @return - TVerdict code
 * Override of base class pure virtual
 * Our implementation only gets called if the base class doTestStepPreambleL() did
 * not leave. That being the case, the current test result value will be EPass.
 */
TVerdict CMultipleProtocolModules::doTestStepL()
	{
	  if (TestStepResult()==EPass)
		{
		/*
		 *  Multiple PM tests (basic Set and Get) for both home and roaming
		 *  Also checking the fuctionality between Array overload and Int method for
		 *  default are the same
		 *  
		 *  (01) Set 0 PM in array, get one PM from array, should be normal default PM.
		 *  (02) Set one PM in array, get this one PM from array.
		 *   ^-> also tests (07) Set PM UID of 00000000 - positive test on simplest type
		 *   
		 *     
		 *  - Boundary cases regarding KLbsAdminMaxProtocolModuleIds (10) for both home and roaming
		 *  
		 *  (03) Set 0,  get default - positive test
		 *  (04) Set 1,  get 1 - positive test
		 *  ^-> also tests (08) Set PM UID of FFFFFFFF - positive test on simplest type
		 *
		 *  (05-1) Set 10, get 10 - positive test
		 *  ^-> also tested (05-2) Get using Int should return element 0 *Checked in every test case*
		 *  (06) UNTESTABLE: Set 11 should fail, get default should still work. - negative test
		 *  
		 *  -'Fundamental type used' tests
		 *  
		 *  
		 */
		TInt testCase = 0;
		GetIntFromConfig(ConfigSection(),KTestCaseToRun,testCase);
		
		//variables common to all cases
		TLbsAdminProtocolModulesInfo info;
		TLbsAdminProtocolModulesInfo read;
		TLbsAdminProtocolModulesInfo::TLbsProtocolModuleIdArray uidArray;
		TLbsAdminProtocolModulesInfo::TLbsProtocolModuleIdArray readUidArray;
		TInt readUidCount;	
		TLbsProtocolModuleId readHomePmUid;
		TLbsProtocolModuleId readRoamPmUid;

		switch (testCase)
			{
			case 1:
				{
				// Nothing set, but we should get default PM (RRC)

				// note we are also checking that original default ProtocolModuleId functionality still works.
				User::LeaveIfError(iLbsAdmin->Get(KLbsSettingHomeProtocolModule, read));
				User::LeaveIfError(iLbsAdmin->Get(KLbsSettingHomeProtocolModule, readHomePmUid));
				read.GetModuleIds(readUidArray, readUidCount);
				if(readUidArray[0] != readHomePmUid ||
				   readUidArray[0] != KDefaultPMUid) 
					{
					INFO_PRINTF3(_L("HOME: read[0] was %d, readHomePmUid was %d"),
								readUidArray[0],readHomePmUid);
					SetTestStepResult(EFail);
					}
				User::LeaveIfError(iLbsAdmin->Get(KLbsSettingRoamingProtocolModule, read));
				User::LeaveIfError(iLbsAdmin->Get(KLbsSettingRoamingProtocolModule, readRoamPmUid));
				read.GetModuleIds(readUidArray, readUidCount);
				if(readUidArray[0] != readRoamPmUid ||
				   readUidArray[0] != TUid::Uid(0)) // 0 is the roaming module ie none
					{
					INFO_PRINTF3(_L("ROAMING: read[0] was %d, readRoamPmUid was %d"),
								readUidArray[0], readRoamPmUid);
					SetTestStepResult(EFail);
					}				
				break;
				}
				
			case 2:
				{
				// Set 1 and Get 1, this should overwrite the default value
				uidArray[0] = TUid::Uid(0x00000000);
				info.SetModuleIds(uidArray,1);

				User::LeaveIfError(iLbsAdmin->Set(KLbsSettingHomeProtocolModule, info));
				User::LeaveIfError(iLbsAdmin->Get(KLbsSettingHomeProtocolModule, read));
				User::LeaveIfError(iLbsAdmin->Get(KLbsSettingHomeProtocolModule, readHomePmUid));
				read.GetModuleIds(readUidArray, readUidCount);
				if(readUidArray[0] != readHomePmUid || 
				   readUidArray[0] != uidArray[0])
					{
					INFO_PRINTF3(_L("HOME: read[0] was %d, readHomePmUid was %d"),
								readUidArray[0],readHomePmUid);
					SetTestStepResult(EFail);
					}
				
				User::LeaveIfError(iLbsAdmin->Set(KLbsSettingRoamingProtocolModule, info));
				User::LeaveIfError(iLbsAdmin->Get(KLbsSettingRoamingProtocolModule, read));
				User::LeaveIfError(iLbsAdmin->Get(KLbsSettingRoamingProtocolModule, readRoamPmUid));
				read.GetModuleIds(readUidArray, readUidCount);
				if(readUidArray[0] != readRoamPmUid || 
				   readUidArray[0] != uidArray[0])
					{
					INFO_PRINTF3(_L("ROAMING: read[0] was %d, readRoamPmUid was %d"),
								readUidArray[0], readRoamPmUid);
					SetTestStepResult(EFail);
					}
				break;
				}
					
			case 3:
				{
				// (03) Set 0,  get default - should not overwrite default if nothing is passed in.
				// Note we dont set the first element to be a Uid
				info.SetModuleIds(uidArray,0);

				User::LeaveIfError(iLbsAdmin->Set(KLbsSettingHomeProtocolModule, info));
				User::LeaveIfError(iLbsAdmin->Get(KLbsSettingHomeProtocolModule, read));
				User::LeaveIfError(iLbsAdmin->Get(KLbsSettingHomeProtocolModule, readHomePmUid));
				read.GetModuleIds(readUidArray, readUidCount);
				if(readUidArray[0] != readHomePmUid ||
				   readUidArray[0] != TUid::Uid(0))
					{
					INFO_PRINTF4(_L("HOME: read[0] was %d, readHomePmUid was %d, KDefaultPMUid was %d"),
								readUidArray[0],readHomePmUid, KDefaultPMUid);
					SetTestStepResult(EFail);
					}
				
				User::LeaveIfError(iLbsAdmin->Set(KLbsSettingRoamingProtocolModule, info));
				User::LeaveIfError(iLbsAdmin->Get(KLbsSettingRoamingProtocolModule, read));
				User::LeaveIfError(iLbsAdmin->Get(KLbsSettingRoamingProtocolModule, readRoamPmUid));
				if(readUidArray[0] != readRoamPmUid ||
				   readUidArray[0] != TUid::Uid(0))
					{
					INFO_PRINTF3(_L("ROAMING: read[0] was %d, readRoamPmUid was %d, expected roaming PM uid here was 0"),
								readUidArray[0], readRoamPmUid);
					SetTestStepResult(EFail);
					}
				break;
				}				

			case 4:
				{
				// (04) Set 1,  get 1 - positive test
				// Also using the upper boundary UID value at the same time.
				uidArray[0] = TUid::Uid(0xFFFFFFFF);
				info.SetModuleIds(uidArray,1);

				User::LeaveIfError(iLbsAdmin->Set(KLbsSettingHomeProtocolModule, info));
				User::LeaveIfError(iLbsAdmin->Get(KLbsSettingHomeProtocolModule, read));
				User::LeaveIfError(iLbsAdmin->Get(KLbsSettingHomeProtocolModule, readHomePmUid));
				read.GetModuleIds(readUidArray, readUidCount);
				if(readUidArray[0] != readHomePmUid ||
				   readUidArray[0] != uidArray[0])
					{
					INFO_PRINTF3(_L("HOME: read[0] was %d, readHomePmUid was %d"),
								readUidArray[0],readHomePmUid);
					SetTestStepResult(EFail);
					}
				
				User::LeaveIfError(iLbsAdmin->Set(KLbsSettingRoamingProtocolModule, info));
				User::LeaveIfError(iLbsAdmin->Get(KLbsSettingRoamingProtocolModule, read));
				User::LeaveIfError(iLbsAdmin->Get(KLbsSettingRoamingProtocolModule, readRoamPmUid));
				read.GetModuleIds(readUidArray, readUidCount);
				if(readUidArray[0] != readRoamPmUid ||
				   readUidArray[0] != uidArray[0])
					{
					INFO_PRINTF3(_L("ROAMING: read[0] was %d, readRoamPmUid was %d"),
								readUidArray[0], readRoamPmUid);
					SetTestStepResult(EFail);
					}
				break;
				}		
				
				
			case 5:
					{
					//	(05-1) Set 10, get 10 - positive test
					// Also using the upper and lower boundary UID values at the same time.
					uidArray[9] = TUid::Uid(0xFFFFFFFF);
					uidArray[8] = TUid::Uid(0xFFFFFFFE);
					uidArray[7] = TUid::Uid(0xFFFFFFFD);
					uidArray[6] = TUid::Uid(0xFFFFFFFC);
					uidArray[5] = TUid::Uid(0xFFFFFFFB);
					uidArray[4] = TUid::Uid(0x00000004);
					uidArray[3] = TUid::Uid(0x00000003);
					uidArray[2] = TUid::Uid(0x00000002);
					uidArray[1] = TUid::Uid(0x00000001);
					uidArray[0] = TUid::Uid(0x00000000);
					info.SetModuleIds(uidArray,10);

					User::LeaveIfError(iLbsAdmin->Set(KLbsSettingHomeProtocolModule, info));
					User::LeaveIfError(iLbsAdmin->Get(KLbsSettingHomeProtocolModule, read));
					User::LeaveIfError(iLbsAdmin->Get(KLbsSettingHomeProtocolModule, readHomePmUid));
					read.GetModuleIds(readUidArray, readUidCount);
					if(readUidCount != KLbsAdminMaxProtocolModuleIds ||
					   readUidArray[0] != uidArray[0] || readUidArray[1] != uidArray[1] ||
					   readUidArray[2] != uidArray[2] || readUidArray[3] != uidArray[3] ||
					   readUidArray[4] != uidArray[4] || readUidArray[5] != uidArray[5] ||
					   readUidArray[6] != uidArray[6] || readUidArray[7] != uidArray[7] ||
					   readUidArray[8] != uidArray[8] || readUidArray[9] != uidArray[9] )
						{
						INFO_PRINTF4(_L("HOME: read[0] was %d, readHomePmUid was %d, readUidCount was %d"),
									readUidArray[0], readHomePmUid, readUidCount);
						SetTestStepResult(EFail);
						}
					
					User::LeaveIfError(iLbsAdmin->Set(KLbsSettingRoamingProtocolModule, info));
					User::LeaveIfError(iLbsAdmin->Get(KLbsSettingRoamingProtocolModule, read));
					User::LeaveIfError(iLbsAdmin->Get(KLbsSettingRoamingProtocolModule, readRoamPmUid));
					read.GetModuleIds(readUidArray, readUidCount);
					if(readUidCount != KLbsAdminMaxProtocolModuleIds ||
					   readUidArray[0] != uidArray[0] || readUidArray[1] != uidArray[1] ||
					   readUidArray[2] != uidArray[2] || readUidArray[3] != uidArray[3] ||
					   readUidArray[4] != uidArray[4] || readUidArray[5] != uidArray[5] ||
					   readUidArray[6] != uidArray[6] || readUidArray[7] != uidArray[7] ||
					   readUidArray[8] != uidArray[8] || readUidArray[9] != uidArray[9] )
						{
						INFO_PRINTF4(_L("ROAMING: read[0] was %d, readRoamPmUid was %d, readUidCount was %d"),
									readUidArray[0], readRoamPmUid, readUidCount);
						SetTestStepResult(EFail);
						}
					
					break;
					}		
			
				
			default:
				{
				SetTestStepResult(EFail);
				INFO_PRINTF2(_L("No such test step number exists in this instance: %d"), testCase);
				}
			}
			
		
		
		}
		
	  return TestStepResult();
	}

/**
 * @return - TVerdict code
 * Override of base class virtual
 */
TVerdict CMultipleProtocolModules::doTestStepPostambleL()
	{
	CTe_LbsAdminSuiteStepBase::doTestStepPostambleL();
	return TestStepResult();
	}
