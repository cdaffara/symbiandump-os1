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
// @file ctlbsclientstepmodinfo.cpp
// This is the class implementation for the Module Information Tests
// 
//
 
#include "ctlbsclientstepmodinfo.h"

#include <lbs.h>

#include <lbs/test/tlbsutils.h>

/** "num_of_mods" = the expected number (TUint) of modules returned from RPositionServer::GetNumModules().
*/
_LIT(KNumOfMods, "num_of_mods");

/** "default_mod_id" = the expected module id returned from RPositionServer::GetDefaultModuleId(),
	value is decimal.
*/
_LIT(KDefaultModId, "default_mod_id");



/**
 * Destructor
 */
CT_LbsClientStep_ModInfo::~CT_LbsClientStep_ModInfo()
	{
	}


/**
 * Constructor
 */
CT_LbsClientStep_ModInfo::CT_LbsClientStep_ModInfo(CT_LbsClientServer& aParent) : CT_LbsClientStep(aParent)
	{
	SetTestStepName(KLbsClientStep_ModInfo);
	}


/**
Static Constructor
*/
CT_LbsClientStep_ModInfo* CT_LbsClientStep_ModInfo::New(CT_LbsClientServer& aParent)
	{
	return new CT_LbsClientStep_ModInfo(aParent);
	// Note the lack of ELeave.
	// This means that having insufficient memory will return NULL;
	}


/**
 * @return - TVerdict code
 * Override of base class pure virtual
 * Our implementation only gets called if the base class doTestStepPreambleL() did
 * not leave. That being the case, the current test result value will be EPass.
 */
TVerdict CT_LbsClientStep_ModInfo::doTestStepL()
	{
	// Generic test step used to test the LBS Client Notify position update API.
	INFO_PRINTF1(_L("&gt;&gt;CT_LbsClientStep_ModInfo::doTestStepL()"));

	if (TestStepResult() == EPass)
		{
		TInt err = KErrNone;
		TUint 					numOfMods;
		TInt 					modIndex;
		TPositionModuleId		modId;
		TPositionModuleInfo&	currentModInfo = iParent.iSharedData->iCurrentModuleInfo;
							
		// Connect to self locate server.
		User::LeaveIfError(iServer.Connect());
		CleanupClosePushL(iServer);
		
		// Carryout unique test actions.
		TInt testCaseId;
		if (GetIntFromConfig(ConfigSection(), KTestCaseId, testCaseId))
			{
			switch (testCaseId)
				{
				// Test case LBS-ModCnt-0001
				case 1:
					{
					// Find the number of modules installed.
					err = iServer.GetNumModules(numOfMods);
					if ((KErrNone != err) || (numOfMods <=0))
                        {
                        INFO_PRINTF3(_L("Incorrect err = %d, or num of modules %d shouldn't be equal to zero "), err, numOfMods);
                        SetTestStepResult(EFail);	
                        }
					break;
					}
				// Test case LBS-ModCnt-0002
				case 2:
					{
				
					// Call GetNumModules with no connection to server.
					RPositionServer server;	// Don't use iServer as it's open for every test.

					server.GetNumModules(numOfMods);	// No need to check err, call will panic.
					break;
					}

				// Test case LBS-ModCnt-0003
				// Test case LBS-ModCnt-0004
				case 3:
				case 4:
					{
					// Find the number of modules installed.
					TUint verifyNumOfMods;

					if (GetIntFromConfig(ConfigSection(), KNumOfMods, reinterpret_cast<TInt&>(verifyNumOfMods)))
						{
						err = iServer.GetNumModules(numOfMods);
						if ((KErrNone != err) || (verifyNumOfMods != numOfMods))
							{
							INFO_PRINTF4(_L("Incorrect err = %d, or num of modules %d should be %d"), err, numOfMods, verifyNumOfMods);
							SetTestStepResult(EFail);	
							}
						}
						else
						{
							INFO_PRINTF1(_L("No num_of_mods variable set in .ini file"));
							SetTestStepResult(EFail);
						}

					// Setup verification data
					const TInt KNumModules = 3;
					_LIT(KLitModName1, "OtherModule");
					_LIT(KLitModName2, "SIRF_Loc_AGPS_Module");
					_LIT(KLitModName2_C, "C_Drive_SIRF_Module");
					_LIT(KLitModName3, "TEST_Loc_AGPS_Module");
					TPositionModuleId modId;
					TPositionModuleInfo modInfArray[KNumModules];
					modId.iUid = 271064389;			// othermodule.ini
					modInfArray[0].SetModuleId(modId);
					modInfArray[0].SetModuleName(KLitModName1);
					modInfArray[0].SetIsAvailable(ETrue);
					modId.iUid = 271064388;			// sirfdatasource.ini
					modInfArray[1].SetModuleId(modId);
					if (testCaseId == 3)
						modInfArray[1].SetModuleName(KLitModName2);
					else
						modInfArray[1].SetModuleName(KLitModName2_C);
					modInfArray[1].SetIsAvailable(ETrue);
					modId.iUid = 271064388;			// xtestmodule.ini
					modInfArray[2].SetModuleId(modId);
					modInfArray[2].SetModuleName(KLitModName3);
					modInfArray[2].SetIsAvailable(ETrue);
						
					if (err == KErrNone)
						{
						// Verify the expected modules were returned
						for (TInt i=0; i < numOfMods; i++)
							{
							TPositionModuleInfo modInfo;
							err = iServer.GetModuleInfoByIndex(i, modInfo);
							if (err == KErrNone)
								{
								// Check module info is as expected
								for (TInt j=0; j < KNumModules; j++)
									{
									// Check module info
									if (!modInfArray[j].IsAvailable())
										continue;
									// Check ID
									if (modInfo.ModuleId() != modInfArray[j].ModuleId())
										continue;
									// Check name
									TBuf<32> modName, verifyName;
									modInfo.GetModuleName(modName);
									modInfArray[j].GetModuleName(verifyName);
									if (modName.Compare(verifyName) == 0)
										{
										// Use the available to flag module info already 
										// seen in modInfArray.
										modInfArray[j].SetIsAvailable(EFalse);
										break;
										}
									}
								}
							else
								{
								INFO_PRINTF3(_L("Error getting module info %d (%d)"), i, err);
								SetTestStepResult(EFail);
								}
							}
							
						// Check that all modules were seen
						for (TInt k=0; k < KNumModules; k++)
							{
							if (modInfArray[k].IsAvailable())
								{
								INFO_PRINTF2(_L("Expected module %d not found"), k);
								SetTestStepResult(EFail);
								}
							}
						}
					break;
					}


				// TODO:
				// how do we compare the mod info items, will have to read the ini file ourselfs
				// no point in having our own ini file type like we have for posinfo
				// have a test step does the reading of the standard modinfo ini file
				// have a test step to install the modinfo ini file


				// Test case LBS-ModInfo-0001
				case 11:
					{
					// Get mod info, normal functional test.
					//iServer.GetNumModules(numOfMods);
				    
					//err = iServer.GetModuleInfoByIndex(numOfMods-1, currentModInfo);
					err = iServer.GetModuleInfoByIndex(0, currentModInfo);
					if (KErrNone != err)
						{
						INFO_PRINTF2(_L("Incorrect err = %d"), err);
						SetTestStepResult(EFail);	
						}
					break;
					}
				
				// Test case LBS-ModInfo-0002
				case 12:
					{
					// Get mod info using a invalid index.
					modIndex = -1;
					
					err = iServer.GetModuleInfoByIndex(modIndex, currentModInfo);
					if (KErrNotFound != err)
						{
						INFO_PRINTF3(_L("Incorrect err = %d, should be %d"), err, KErrNotFound);
						SetTestStepResult(EFail);	
						}
					break;
					}
				
				// Test case LBS-ModInfo-0003
				case 13:
					{
					// Get mod info using a invalid index.
					User::LeaveIfError(iServer.GetNumModules(numOfMods));
					modIndex = numOfMods + 1; // This will give us a invalid index.
					
					err = iServer.GetModuleInfoByIndex(modIndex, currentModInfo);
					if (KErrNotFound != err)
						{
						INFO_PRINTF3(_L("Incorrect err = %d, should be %d"), err, KErrNotFound);
						SetTestStepResult(EFail);	
						}
					break;
					}

				// Test case LBS-ModInfo-0004
				case 14:
					{
					// Get mod info with no connection to the server.
					RPositionServer server;	// Don't use iServer as it's open for every test.
					
					server.GetModuleInfoByIndex(0, currentModInfo);	// No need to check err, call will panic.
					break;
					}

				// Test case LBS-ModInfo-0005
				case 15:
					{
					// Get mod info using a valid id.
					TPositionModuleInfo	modInfo;
					
					// Determine a valid module id, use get mod by index
					User::LeaveIfError(iServer.GetModuleInfoByIndex(0, modInfo));
					modId = modInfo.ModuleId();

					// Carry out test of a valid id					
					err = iServer.GetModuleInfoById(modId, currentModInfo);
					if (KErrNone != err)
						{
						INFO_PRINTF2(_L("Incorrect err = %d"), err);
						SetTestStepResult(EFail);	
						}
					break;
					}
				
				// Test case LBS-ModInfo-0006
				case 16:
					{
					// Get mod info with invalid id.
					modId = KPositionNullModuleId;
					
					err = iServer.GetModuleInfoById(modId, currentModInfo);
					if (KErrNotFound != err)
						{
						INFO_PRINTF3(_L("Incorrect err = %d, should be %d"), err, KErrNotFound);
						SetTestStepResult(EFail);	
						}
					break;
					}
				
				// Test case LBS-ModInfo-0007
				case 17:
					{
					// Get mod info with no connection to the server.
					RPositionServer server;	// Don't use iServer as it's open for every test.
					
					server.GetModuleInfoById(KPositionNullModuleId, currentModInfo);	// No need to check err, call will panic.
					break;
					}

				// Test case LBS-ModInfo-0008
				case 18:
					{
					// Get default mod id.
					TPositionModuleId verifyModId;
					if (GetIntFromConfig(ConfigSection(), KDefaultModId, reinterpret_cast<TInt&>(verifyModId)))
						{
						err = iServer.GetDefaultModuleId(modId);
						if ((KErrNone != err) || (verifyModId != modId))
							{
							INFO_PRINTF4(_L("Incorrect err = %d, or default module id %d should be %d"), err, modId, verifyModId);
							SetTestStepResult(EFail);	
							}
						}
						
					else
						{
						INFO_PRINTF1(_L("No default_mod_id variable set in .ini file"));
						SetTestStepResult(EFail);
						}
					break;		
					}

				// Test case LBS-ModInfo-0009
				case 19:
					{
					// Get default mod id with no connection to server.
					RPositionServer server;	// Don't use iServer as it's open for every test.
					
					server.GetDefaultModuleId(modId);	// No need to check err, call will panic.
					break;
					}
				}
			}
			
		// All done, clean up.
		CleanupStack::PopAndDestroy(&iServer);		
		}

	INFO_PRINTF1(_L("&lt;&lt;CT_LbsClientStep_ModInfo::doTestStepL()"));

	return TestStepResult();
	}

