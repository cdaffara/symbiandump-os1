// Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// SettingsManagerFunction implementation
//



/**
 @file settingsmanagerfunctionteststep.cpp
 @internalTechnology
*/

#include "settingsmanagerfunctionteststep.h"
#include "te_settingsmanagerdefs.h"

CSettingsManagerFunctionTestStep::~CSettingsManagerFunctionTestStep()
/**
 * Destructor
 */
	{
	delete iPNSChecker;
	CActiveScheduler::Install(NULL);
	delete iScheduler;
	}

CSettingsManagerFunctionTestStep::CSettingsManagerFunctionTestStep()
/**
 * Constructor
 */
	{
	SetTestStepName(KSettingsManagerFunctionTestStep);
	}

CSettingsManagerFunctionTestStep* CSettingsManagerFunctionTestStep::NewLC()
	{
	CSettingsManagerFunctionTestStep* self = new (ELeave)CSettingsManagerFunctionTestStep();
	CleanupStack::PushL (self);
	self->ConstructL ();
	return self;
	}

CSettingsManagerFunctionTestStep* CSettingsManagerFunctionTestStep::NewL()
	{
	CSettingsManagerFunctionTestStep* self=CSettingsManagerFunctionTestStep::NewLC ();
	CleanupStack::Pop (self);
	return self;
	}

void CSettingsManagerFunctionTestStep::ConstructL()
	{
	iScheduler = new(ELeave) CActiveScheduler();
	CActiveScheduler::Install(iScheduler);
	iPNSChecker = CPNSChecker::NewL();
	}

TVerdict CSettingsManagerFunctionTestStep::doTestStepPreambleL()
/**
 * @return - TVerdict code
 * Override of base class virtual
 */
	{
	CTe_settingsmanagerStepBase::doTestStepPreambleL();
	return TestStepResult();
	}


TVerdict CSettingsManagerFunctionTestStep::doTestStepL()
/**
 * @return - TVerdict code
 * 
 * This test step check all Publish and Subscribe keys used by the CBTGPSSettingManager.
 * 
 * 1) It check that all keys are created at CBTGPSSettingManager's construction time.
 * If the keys are created alreday, we delete them all, and then call CBTGPSSettingManager::NewL().
 * If the keys are not created, we  call CBTGPSSettingManager::NewL() directly.
 * 
 * 2) It call CBTGPSSettingManager::UpdatePsyState().
 * This method update the KBluetoothGpsPsyState to a EPSYLoadedAndPNOKNotSupportedBTGPSUsed and set up other keys to some default value. 
 * The test check the value of KBluetoothGpsPsyState after this operation.
 * 
 * 3) It call CBTGPSSettingManager::UpdateDeviceStatus().
 * This method update the following keys to the following value:
 * KBluetoothGpsPsyBatteryLevel => 55
 * KBluetoothGpsPsyBatteryState => EBatteryStateNormal
 * KBluetoothGpsPsyExtAntennaState => EExtAntennaOn
 * KBluetoothGpsPsyExtPowerState => EExtPowerOff
 * The test check the value of the keys after this operation.
 * 
 * 4) It call CBTGPSSettingManager::UpdateVersions().
 * This method update the following keys:
 * KBluetoothGpsPsyVersion
 * KBluetoothGpsPsyBtHwVersion
 * KBluetoothGpsPsyBtSwVersion
 * KBluetoothGpsPsyGpsHwVersion
 * KBluetoothGpsPsyGpsSwVersion
 * The test check the value of the keys after this operation.
 * 
 * 5) finally, the test call CBTGPSSettingManager::UpdateBTDeviceInfo() and CBTGPSSettingManager::GetBTDeviceInfo().
 * The test check the value of the centrep entries after CBTGPSSettingManager::GetBTDeviceInfo().
 * 
 */
	{
	_LIT(KKeysNeedsToBeDeleted, "The P&S keys are available, we need to delete them.");
	_LIT(KKeysNotToBeDeleted, "The P&S keys are not available, we don't need to delete them.");
	_LIT(KKeysToBeGenerated, "The Settings Manager should generate the P&S keys at construction time.");
	_LIT(KKeysCorrectlyGenerated, "The P&S keys have been correctly generated");
	_LIT(KKeysIncorrectlyGenerated, "The Settings Manager failed to generate valid P&S keys at construction time.");
	_LIT(KStateUpdateFailed, "The Settings Manager failed to update the KBluetoothGpsPsyState key: %d");
	_LIT(KStateUpdateSucceed, "The Settings Manager updated the KBluetoothGpsPsyState key successfully.");
	_LIT(KDeviceUpdateFailed, "The Settings Manager failed to update the device status: %d");
	_LIT(KDeviceUpdateSucceed, "The Settings Manager updated the device status successfully.");
	_LIT(KDeviceVersionUpdateFailed, "The Settings Manager failed to update the device version values: %d");
	_LIT(KDeviceVersionUpdateSucceed, "The Settings Manager updated the device version values successfully.");
	_LIT(KDeviceInfoFailed, "The Settings Manager failed to update the BT device info: %d");
	_LIT(KDeviceInfoSucceed, "The Settings Manager updated the BT device info successfully.");
	
	if (TestStepResult()==EPass)
		{
		TBool areAvailableKeys = ETrue;
		TBool isBTPSYLoaded = ETrue;
		TBool areValidKeys = ETrue;
		iPNSChecker->CheckPNSKeys(areAvailableKeys, isBTPSYLoaded, areValidKeys);
		if(areAvailableKeys)
			{
			INFO_PRINTF1(KKeysNeedsToBeDeleted);
			iPNSChecker->DeleteAllKeys();
			}
		else
			{
			INFO_PRINTF1(KKeysNotToBeDeleted);
			}
		
		INFO_PRINTF1(KKeysToBeGenerated);
		CBTGPSSettingManager* settingManager = CBTGPSSettingManager::NewL();
		iPNSChecker->CheckPNSKeys(areAvailableKeys, isBTPSYLoaded, areValidKeys);
		if(areValidKeys)
			{
			INFO_PRINTF1(KKeysCorrectlyGenerated);
			}
		else
			{
			INFO_PRINTF1(KKeysIncorrectlyGenerated);
			SetTestStepResult(EFail);
			return TestStepResult();
			}
		
		TInt err = KErrNone;
		
		err |= settingManager->UpdatePsyState(EPSYLoadedAndPNOKNotSupportedBTGPSUsed);
		err |= iPNSChecker->CheckKey(KBluetoothGpsPsyState, EPSYLoadedAndPNOKNotSupportedBTGPSUsed);
		
		if(KErrNone != err)
			{
			SetTestStepResult(EFail);
			INFO_PRINTF2(KStateUpdateFailed, err);
			err = KErrNone;
			}
		else
			{
			INFO_PRINTF1(KStateUpdateSucceed);
			}
		
		err |= settingManager->UpdateDeviceStatus(55, EExtPowerOff, EExtAntennaOn, EBatteryStateNormal);
		err |= iPNSChecker->CheckKey(KBluetoothGpsPsyBatteryLevel, 55);
		err |= iPNSChecker->CheckKey(KBluetoothGpsPsyBatteryState, EBatteryStateNormal);
		err |= iPNSChecker->CheckKey(KBluetoothGpsPsyExtAntennaState, EExtAntennaOn);
		err |= iPNSChecker->CheckKey(KBluetoothGpsPsyExtPowerState, EExtPowerOff);
		if(KErrNone != err)
			{
			SetTestStepResult(EFail);
			INFO_PRINTF2(KDeviceUpdateFailed, err);
			err = KErrNone;
			}
		else
			{
			INFO_PRINTF1(KDeviceUpdateSucceed);
			}
		
		_LIT8(KId,"anid");
		_LIT8(KBtHwVersion,"abtHwVersion");
		_LIT8(KBtSwVersion,"abtSwVersion");
		_LIT8(KGpsHwVersion,"agpsHwVersion");
		_LIT8(KGpsSwVersion,"agpsSwVersion");
		TBufC8<4> id(KId);
		TBufC8<12> btHwVersion(KBtHwVersion);
		TBufC8<12> btSwVersion(KBtSwVersion);
		TBufC8<13> gpsHwVersion(KGpsHwVersion);
		TBufC8<13> gpsSwVersion(KGpsSwVersion);
		
		err |= settingManager->UpdateVersions(id, btHwVersion, btSwVersion, gpsHwVersion, KGpsSwVersion);
		err |= iPNSChecker->CheckKey(KBluetoothGpsPsyVersion, id);
		err |= iPNSChecker->CheckKey(KBluetoothGpsPsyBtHwVersion, btHwVersion);
		err |= iPNSChecker->CheckKey(KBluetoothGpsPsyBtSwVersion, btSwVersion);
		err |= iPNSChecker->CheckKey(KBluetoothGpsPsyGpsHwVersion, gpsHwVersion);
		err |= iPNSChecker->CheckKey(KBluetoothGpsPsyGpsSwVersion, KGpsSwVersion);
		if(KErrNone != err)
			{
			SetTestStepResult(EFail);
			INFO_PRINTF2(KDeviceVersionUpdateFailed, err);
			err = KErrNone;
			}
		else
			{
			INFO_PRINTF1(KDeviceVersionUpdateSucceed);
			}
		
		TBTSockAddr sockAddr;
		TBTSockAddr sockAddrResult;
		TInt validity = KErrNotFound;
		_LIT8(KAddr,"5F9131");
		sockAddr.SetBTAddr(TBTDevAddr(KAddr));
		err |= settingManager->UpdateBTDeviceInfo(2, sockAddr);
		//err |= iPNSChecker->CheckRepository(KBluetoothGpsPsyDeviceAddressValidity, 2);
		err |= settingManager->GetBTDeviceInfo(validity,sockAddrResult);
		
		if(KErrNone == err)
			{
			if(2 != validity)
				{
				err = KErrCorrupt;
				}
			else
				{
				if(sockAddrResult.BTAddr() != sockAddr.BTAddr())
					{
					err = KErrCorrupt;
					}
				}
			}
		
		if(KErrNone != err)
			{
			SetTestStepResult(EFail);
			INFO_PRINTF2(KDeviceInfoFailed, err);
			err = KErrNone;
			}
		else
			{
			INFO_PRINTF1(KDeviceInfoSucceed);
			}
		
		delete settingManager;
		}
	return TestStepResult();
	}



TVerdict CSettingsManagerFunctionTestStep::doTestStepPostambleL()
/**
 * @return - TVerdict code
 * Override of base class virtual
 */
	{
	CTe_settingsmanagerStepBase::doTestStepPostambleL();
	return TestStepResult();
	}
