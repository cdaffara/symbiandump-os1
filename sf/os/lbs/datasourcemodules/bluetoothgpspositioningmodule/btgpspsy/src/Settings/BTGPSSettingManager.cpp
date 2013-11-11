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
//



// INCLUDE FILES
#include <e32std.h>
#include <bt_sock.h>
#include "BtGpsPsyPrivatePSKeys.h"
#include "BtGpsPsyPrivateCRKeys.h"
#include "BTGPSSettingManager.h"
#include "BTGPSSettingsListener.h"
#include "BTGPSLogging.h"

// CONSTANTS

// BT GPS PSY specific setting partial Id 
const TUint32 KBTGPSPSYSpecificSettingsPartialId = 0x10000000;

// BT GPS PSY specific setting mask
const TUint32 KBTGPSPSYSpecificSettingsMask = 0xF0000000;


// MACROS


// ============================ MEMBER FUNCTIONS ===============================


// -----------------------------------------------------------------------------
// CBTGPSSettingManager::NewL
// -----------------------------------------------------------------------------
CBTGPSSettingManager* CBTGPSSettingManager::NewL()
    {
    CBTGPSSettingManager* self = new (ELeave) CBTGPSSettingManager();
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop();
    return self;
    }

// -----------------------------------------------------------------------------
// CBTGPSSettingManager::~CBTGPSSettingManager
// -----------------------------------------------------------------------------
CBTGPSSettingManager::~CBTGPSSettingManager()
    {
    //Set setting to unload status. All erros are ignored in destruction
    iEvents.Set(KPsUidBluetoothGpsPsy, KBluetoothGpsPsyState,           EPSYNotLoaded); 
	iEvents.Set(KPsUidBluetoothGpsPsy, KBluetoothGpsPsyBatteryLevel,    KErrNotFound); 
	iEvents.Set(KPsUidBluetoothGpsPsy, KBluetoothGpsPsyBatteryState,    EBatteryStateNA); 
	iEvents.Set(KPsUidBluetoothGpsPsy, KBluetoothGpsPsyExtAntennaState, EExtAntennaNA); 
	iEvents.Set(KPsUidBluetoothGpsPsy, KBluetoothGpsPsyExtPowerState,   EExtPowerNA); 
	iEvents.Set(KPsUidBluetoothGpsPsy, KBluetoothGpsPsyVersion,         KNullDesC); 
	iEvents.Set(KPsUidBluetoothGpsPsy, KBluetoothGpsPsyBtHwVersion,     KNullDesC); 
	iEvents.Set(KPsUidBluetoothGpsPsy, KBluetoothGpsPsyBtSwVersion,     KNullDesC); 
	iEvents.Set(KPsUidBluetoothGpsPsy, KBluetoothGpsPsyGpsHwVersion,    KNullDesC); 
	iEvents.Set(KPsUidBluetoothGpsPsy, KBluetoothGpsPsyGpsSwVersion,    KNullDesC);
	
	iEvents.Close();
	
	Cancel();
	
	delete iSettings;
	
	iListenerArray.Reset();
	iListenerArray.Close();
    }

// -----------------------------------------------------------------------------
// CBTGPSSettingManager::ConstructL
// -----------------------------------------------------------------------------
void CBTGPSSettingManager::ConstructL()
    {
	TRACESTRING("CBTGPSSettingManager::ConstructL()")

    TInt err(KErrNone);

	//Define P&S events
	TSecurityPolicy readPolicy(ECapabilityReadDeviceData);
	TSecurityPolicy writePolicy(ECapabilityWriteDeviceData);	
	err |= iEvents.Define(KPsUidBluetoothGpsPsy, KBluetoothGpsPsyState,            RProperty::EInt, readPolicy, writePolicy); 
	err |= iEvents.Define(KPsUidBluetoothGpsPsy, KBluetoothGpsPsyBatteryLevel,     RProperty::EInt, readPolicy, writePolicy); 
	err |= iEvents.Define(KPsUidBluetoothGpsPsy, KBluetoothGpsPsyBatteryState,     RProperty::EInt, readPolicy, writePolicy); 
	err |= iEvents.Define(KPsUidBluetoothGpsPsy, KBluetoothGpsPsyExtAntennaState,  RProperty::EInt, readPolicy, writePolicy); 
	err |= iEvents.Define(KPsUidBluetoothGpsPsy, KBluetoothGpsPsyExtPowerState,    RProperty::EInt, readPolicy, writePolicy); 
	err |= iEvents.Define(KPsUidBluetoothGpsPsy, KBluetoothGpsPsyVersion,          RProperty::EByteArray, readPolicy, writePolicy); 
	err |= iEvents.Define(KPsUidBluetoothGpsPsy, KBluetoothGpsPsyBtHwVersion,      RProperty::EByteArray, readPolicy, writePolicy); 
	err |= iEvents.Define(KPsUidBluetoothGpsPsy, KBluetoothGpsPsyBtSwVersion,      RProperty::EByteArray, readPolicy, writePolicy); 
	err |= iEvents.Define(KPsUidBluetoothGpsPsy, KBluetoothGpsPsyGpsHwVersion,     RProperty::EByteArray, readPolicy, writePolicy); 
	err |= iEvents.Define(KPsUidBluetoothGpsPsy, KBluetoothGpsPsyGpsSwVersion,     RProperty::EByteArray, readPolicy, writePolicy); 

	if(err!=KErrAlreadyExists)
	    {
	    User::LeaveIfError(err);
	    }
	
    err = KErrNone;
    
    err |= iEvents.Set(KPsUidBluetoothGpsPsy, KBluetoothGpsPsyState,            EPSYLoaded); 
	err |= iEvents.Set(KPsUidBluetoothGpsPsy, KBluetoothGpsPsyBatteryLevel,     KErrNotFound); 
	err |= iEvents.Set(KPsUidBluetoothGpsPsy, KBluetoothGpsPsyBatteryState,     EBatteryStateNA); 
	err |= iEvents.Set(KPsUidBluetoothGpsPsy, KBluetoothGpsPsyExtAntennaState,  EExtAntennaNA); 
	err |= iEvents.Set(KPsUidBluetoothGpsPsy, KBluetoothGpsPsyExtPowerState,    EExtPowerNA); 
	err |= iEvents.Set(KPsUidBluetoothGpsPsy, KBluetoothGpsPsyVersion,          KNullDesC); 
	err |= iEvents.Set(KPsUidBluetoothGpsPsy, KBluetoothGpsPsyBtHwVersion,      KNullDesC); 
	err |= iEvents.Set(KPsUidBluetoothGpsPsy, KBluetoothGpsPsyBtSwVersion,      KNullDesC); 
	err |= iEvents.Set(KPsUidBluetoothGpsPsy, KBluetoothGpsPsyGpsHwVersion,     KNullDesC); 
	err |= iEvents.Set(KPsUidBluetoothGpsPsy, KBluetoothGpsPsyGpsSwVersion,     KNullDesC); 
	
	User::LeaveIfError(err);
	
    //Construct Central Repositary Setting
    iSettings = CRepository::NewL(KCRUidBtGpsPsy );
    iSettings->NotifyRequest(
        KBTGPSPSYSpecificSettingsPartialId, 
        KBTGPSPSYSpecificSettingsMask,
        iStatus);
    SetActive();
    
    }

// -----------------------------------------------------------------------------
// CBTGPSSettingManager::CBTGPSSettingManager
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
CBTGPSSettingManager::CBTGPSSettingManager() :
    CActive(EPriorityStandard)
    {
    CActiveScheduler::Add(this);
    }
    
// -----------------------------------------------------------------------------
// CBTGPSSettingManager::UpdatePsyState
// Updates the client for new BT GPS PSY values.
// -----------------------------------------------------------------------------
TInt CBTGPSSettingManager::UpdatePsyState(const TBTGPSPSYState aStatus)
    {
    TInt err = KErrNone;
    TRACESTRING28("CBTGPSSettingManager::UpdateBTGPSPSYStatus: %d", aStatus)
	err = iEvents.Set(KPsUidBluetoothGpsPsy, KBluetoothGpsPsyState, aStatus);
	if( aStatus != EPSYLoadedAndPNOKSupportedBTGPSUsed )
		{
		err |= iEvents.Set(KPsUidBluetoothGpsPsy, KBluetoothGpsPsyBatteryLevel,     KErrNotFound); 
	    err |= iEvents.Set(KPsUidBluetoothGpsPsy, KBluetoothGpsPsyBatteryState,     EBatteryStateNA); 
	    err |= iEvents.Set(KPsUidBluetoothGpsPsy, KBluetoothGpsPsyExtAntennaState,  EExtAntennaNA); 
	    err |= iEvents.Set(KPsUidBluetoothGpsPsy, KBluetoothGpsPsyExtPowerState,    EExtPowerNA); 
	    err |= iEvents.Set(KPsUidBluetoothGpsPsy, KBluetoothGpsPsyVersion,          KNullDesC); 
    	err |= iEvents.Set(KPsUidBluetoothGpsPsy, KBluetoothGpsPsyBtHwVersion,      KNullDesC); 
    	err |= iEvents.Set(KPsUidBluetoothGpsPsy, KBluetoothGpsPsyBtSwVersion,      KNullDesC); 
    	err |= iEvents.Set(KPsUidBluetoothGpsPsy, KBluetoothGpsPsyGpsHwVersion,     KNullDesC); 
    	err |= iEvents.Set(KPsUidBluetoothGpsPsy, KBluetoothGpsPsyGpsSwVersion,     KNullDesC); 
		}
    return err;
    }

// -----------------------------------------------------------------------------
// CBTGPSSettingManager::UpdateDeviceStatus
// Updates the client information file for the new status event values 
// (batterylevel ext. antenna and ext. power state).
// -----------------------------------------------------------------------------     
TInt CBTGPSSettingManager::UpdateDeviceStatus(
        const TInt aBatteryLevel, 
        const TBTGPSExtPowerState aExtPowerState, 
        const TBTGPSExtAntennaState aExtAntennaState,
        const TBTGPSBatteryState aBatteryState )
    {
    TRACESTRING("CBTGPSSettingManager::UpdateStatus")
    
    TInt err = KErrNone;
    err |= iEvents.Set(KPsUidBluetoothGpsPsy, KBluetoothGpsPsyBatteryLevel,     aBatteryLevel); 
	err |= iEvents.Set(KPsUidBluetoothGpsPsy, KBluetoothGpsPsyBatteryState,     aBatteryState); 
	err |= iEvents.Set(KPsUidBluetoothGpsPsy, KBluetoothGpsPsyExtAntennaState,  aExtAntennaState); 
	err |= iEvents.Set(KPsUidBluetoothGpsPsy, KBluetoothGpsPsyExtPowerState,    aExtPowerState); 
	
	return err;
    }
    
// -----------------------------------------------------------------------------
// CBTGPSSettingManager::UpdateVersions
// Updates the client information file for the new version (device, BT HW, BT SW,
// GPS HW and GPS SW) information.
// ----------------------------------------------------------------------------- 
TInt CBTGPSSettingManager::UpdateVersions(
        const TDesC8& aID, 
        const TDesC8& aBTHW, 
        const TDesC8& aBTSW, 
        const TDesC8& aGPSHW, 
        const TDesC8& aGPSSW)
    {
    TInt err = KErrNone;
    
    TRACESTRING("CBTGPSSettingManager::UpdateVersions")  
	TRACETEXT(aID) 
	TRACETEXT(aBTHW)
	TRACETEXT(aBTSW)
	TRACETEXT(aGPSHW)
	TRACETEXT(aGPSSW)
	
	err |= iEvents.Set(KPsUidBluetoothGpsPsy, KBluetoothGpsPsyVersion,          aID); 
	err |= iEvents.Set(KPsUidBluetoothGpsPsy, KBluetoothGpsPsyBtHwVersion,      aBTHW);
	err |= iEvents.Set(KPsUidBluetoothGpsPsy, KBluetoothGpsPsyBtSwVersion,      aBTSW);
	err |= iEvents.Set(KPsUidBluetoothGpsPsy, KBluetoothGpsPsyGpsHwVersion,     aGPSHW);
	err |= iEvents.Set(KPsUidBluetoothGpsPsy, KBluetoothGpsPsyGpsSwVersion,     aGPSSW);
	
	return err;
    }
    
// -----------------------------------------------------------------------------
// CBTGPSSettingManager::GetNightModeSetting
// ----------------------------------------------------------------------------- 
TInt CBTGPSSettingManager::GetNightModeSetting(TBTGPSSettingsApi::TNightModeState& aStatus)
    {
    TInt err = KErrNone;
    TInt nightMode = KErrNotFound;
    err |= iSettings->Get(KBluetoothGpsPsyNightModeState, nightMode);
    switch(nightMode)
        {
        case TBTGPSSettingsApi::ENightModeOn:
        case TBTGPSSettingsApi::ENightModeOff:
            {
            aStatus = (TBTGPSSettingsApi::TNightModeState) nightMode;
            break;
            }
        default:
            return KErrCorrupt;
        }
    return err;
    }

// -----------------------------------------------------------------------------
// CBTGPSSettingManager::GetBTDeviceInfo
// ----------------------------------------------------------------------------- 
TInt CBTGPSSettingManager::GetBTDeviceInfo(TInt& aValid, TBTSockAddr& aAddress )
    {
    TRACESTRING("CBTGPSSettingManager::GetBTDeviceInfo")

    TInt err = KErrNone;
    err |=  iSettings->Get( KBluetoothGpsPsyDeviceAddressValidity, aValid );
    TBuf<KMaxSockAddrSize> addressBuffer;
    err |= iSettings->Get( KBluetoothGpsPsyDeviceAddress, addressBuffer );
    aAddress.Copy(addressBuffer);
    
    TRACESTRING2("err=%d", err)
    TRACESTRING2("validity=%d", aValid)
    TRACESTRING28("Address=%S", &aAddress)

    return err;
    }

// -----------------------------------------------------------------------------
// CBTGPSSettingManager::UpdateBTDeviceInfo
// ----------------------------------------------------------------------------- 
TInt CBTGPSSettingManager::UpdateBTDeviceInfo(TInt aValid, const TBTSockAddr& aAddress)
    {
    TInt err = KErrNone;
    err |= iSettings->Set(KBluetoothGpsPsyDeviceAddressValidity, aValid);
    
    TBuf<KMaxSockAddrSize> addressBuffer;
    addressBuffer.Copy(aAddress);
    err |= iSettings->Set(KBluetoothGpsPsyDeviceAddress , addressBuffer);
    
    return err;
    }
    
// -----------------------------------------------------------------------------
// CBTGPSSettingManager::ResetEventStatuses
// ----------------------------------------------------------------------------- 
void CBTGPSSettingManager::ResetEventStatuses()
    {
    //Reset status events in P&S. All errors are ignored.
    iEvents.Set(KPsUidBluetoothGpsPsy, KBluetoothGpsPsyState,            EPSYLoaded); 
    iEvents.Set(KPsUidBluetoothGpsPsy, KBluetoothGpsPsyBatteryLevel,     KErrNotFound); 
	iEvents.Set(KPsUidBluetoothGpsPsy, KBluetoothGpsPsyBatteryState,     EBatteryStateNA); 
	iEvents.Set(KPsUidBluetoothGpsPsy, KBluetoothGpsPsyExtAntennaState,  EExtAntennaNA); 
	iEvents.Set(KPsUidBluetoothGpsPsy, KBluetoothGpsPsyExtPowerState,    EExtPowerNA); 
	iEvents.Set(KPsUidBluetoothGpsPsy, KBluetoothGpsPsyVersion,          KNullDesC); 
    iEvents.Set(KPsUidBluetoothGpsPsy, KBluetoothGpsPsyBtHwVersion,      KNullDesC); 
    iEvents.Set(KPsUidBluetoothGpsPsy, KBluetoothGpsPsyBtSwVersion,      KNullDesC); 
    iEvents.Set(KPsUidBluetoothGpsPsy, KBluetoothGpsPsyGpsHwVersion,     KNullDesC); 
    iEvents.Set(KPsUidBluetoothGpsPsy, KBluetoothGpsPsyGpsSwVersion,     KNullDesC); 
    }	
	
// -----------------------------------------------------------------------------
// CBTGPSSettingManager::HandleSettingsChangeL
// Callback function from MPosSettingsObserver. The Positioning Plug-in API
// uses this to inform changes in the observable file.
// ----------------------------------------------------------------------------- 
void CBTGPSSettingManager::HandleSettingsChange()
	{
	TRACESTRING("CBTGPSSettingManager::HandleSettingsChange start ...")
	
	//Get night mode status
	TInt nightMode;
	iSettings->Get( KBluetoothGpsPsyNightModeState, nightMode );
	
	//Inform listeners on night mode setting change.
    TInt count = iListenerArray.Count();
    for(TInt i=count-1; i>=0; i--)
        {
        iListenerArray[i]->HandleNightModeChange(
            (TBTGPSSettingsApi::TNightModeState)nightMode);
        }
	
	//no change event notifies for dialog status setting change.
    TRACESTRING("CBTGPSSettingManager::HandleSettingsChange end")
	}
	
// -----------------------------------------------------------------------------
// CBTGPSSettingManager::RunL
// ----------------------------------------------------------------------------- 
void CBTGPSSettingManager::RunL()
    {
	TRACESTRING2("CBTGPSSettingManager::RunL start ....%d", iStatus.Int())
    //Issue request right after. Otherwise we may miss some change notification
    iSettings->NotifyRequest(
        KBTGPSPSYSpecificSettingsPartialId, 
        KBTGPSPSYSpecificSettingsMask,
        iStatus);
    SetActive();
    
    HandleSettingsChange();
	TRACESTRING("CBTGPSSettingManager::RunL end")
    }

// -----------------------------------------------------------------------------
// CBTGPSSettingManager::DoCancel
// ----------------------------------------------------------------------------- 
void CBTGPSSettingManager::DoCancel()
    {
    if(iSettings!=NULL)
        {
        iSettings->NotifyCancelAll();
        }
    }

// -----------------------------------------------------------------------------
// CBTGPSSettingManager::AddListenerL
// ----------------------------------------------------------------------------- 
void CBTGPSSettingManager::AddListenerL(MBTGPSSettingsListener& aListener)
    {
    User::LeaveIfError(iListenerArray.Append(&aListener));
    }
        
// -----------------------------------------------------------------------------
// CBTGPSSettingManager::RemoveListener
// ----------------------------------------------------------------------------- 
void CBTGPSSettingManager::RemoveListener(MBTGPSSettingsListener& aListener)
    {
    TInt index = iListenerArray.Find(&aListener);
    if(index!=KErrNone)
        {
        iListenerArray.Remove(index);
        }
    }



//  End of File
