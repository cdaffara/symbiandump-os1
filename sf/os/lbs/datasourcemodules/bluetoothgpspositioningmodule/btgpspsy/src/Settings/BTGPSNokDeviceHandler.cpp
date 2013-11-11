// Copyright (c) 2004-2009 Nokia Corporation and/or its subsidiary(-ies).
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
#include "BTGPSNokDeviceHandler.h"
#include "BtGpsPsyPrivateCRKeys.h"
#include "BTGPSConnectManager.h"
#include "BTGPSSettingManager.h"
#include "BTGPSDeviceManager.h"
#include "BTGPSNmeaParser.h"
#include "BTGPSMessageDef.h"
#include "BTGPSConstantsManager.h"
#include "BTGPSNokNightModeHandler.h"
#include "BTGPSPanic.h"
#include "BTGPSLogging.h"
#include "btgpsdevicelistmanager.h"

// ============================ MEMBER FUNCTIONS ===============================


// -----------------------------------------------------------------------------
// CBTGPSNokDeviceHandler::NewL
// -----------------------------------------------------------------------------
CBTGPSNokDeviceHandler* CBTGPSNokDeviceHandler::NewL(
        CBTGPSConnectManager& aConnectManager,
        CBTGPSSettingManager& aSettingManager,
        CBTGPSConstantsManager& aConstantsManager,
        CBTGPSDeviceManager& aDeviceManager,
        CBTGPSDeviceListManager& aDeviceListManager)
    {
    CBTGPSNokDeviceHandler* self = new (ELeave) CBTGPSNokDeviceHandler(
        aConnectManager,
        aSettingManager,
        aConstantsManager,
        aDeviceManager,
        aDeviceListManager);
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop();
    return self;
    }

// -----------------------------------------------------------------------------
// CBTGPSNokDeviceHandler::~CBTGPSNokDeviceHandler
// -----------------------------------------------------------------------------
CBTGPSNokDeviceHandler::~CBTGPSNokDeviceHandler()
    {
    iConnectManager.RemoveMessageListener(*this);
    iSettingManager.RemoveListener(*this);
    iDeviceManager.RemoveListener(*this);

    delete iNightModeHandler;
    }

// -----------------------------------------------------------------------------
// CBTGPSNokDeviceHandler::ConstructL
// -----------------------------------------------------------------------------
void CBTGPSNokDeviceHandler::ConstructL()
    {
    iConnectManager.AddMessageListenerL(*this);
    iSettingManager.AddListenerL(*this);
    iDeviceManager.AddListenerL(*this);
     
    iPreviousDeviceType = iDeviceManager.DeviceType();
    
    //Get Night Mode setting
    if(iSettingManager.GetNightModeSetting(
        (TBTGPSSettingsApi::TNightModeState &)iNightModeSetting) != KErrNone)
        {
        //In error case, turn night mode off
        iNightModeSetting = TBTGPSSettingsApi::ENightModeOff;
        }
    }

// -----------------------------------------------------------------------------
// CBTGPSNokDeviceHandler::CBTGPSNokDeviceHandler
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
CBTGPSNokDeviceHandler::CBTGPSNokDeviceHandler(
        CBTGPSConnectManager& aConnectManager,
        CBTGPSSettingManager& aSettingManager,
        CBTGPSConstantsManager& aConstantsManager,
        CBTGPSDeviceManager& aDeviceManager,
        CBTGPSDeviceListManager& aDeviceListManager)
    : iConnectManager(aConnectManager),
      iSettingManager(aSettingManager),
      iConstantsManager(aConstantsManager),
      iDeviceManager(aDeviceManager),
      iDeviceListManager(aDeviceListManager),
      iNightModeSetting(TBTGPSSettingsApi::ENightModeNA),
      iBattStatus(KErrNotFound),
      iExtPowerStatus(KErrNotFound),
      iExtAntennaStatus(KErrNotFound)
    {
    }

// -----------------------------------------------------------------------------
// CBTGPSNokDeviceHandler::HandleMessage
// -----------------------------------------------------------------------------
void CBTGPSNokDeviceHandler::HandleMessage(const TBTGPSNmeaParser& aParser)
    {
    TRACESTRING("CBTGPSNokDeviceHandler::HandleMessage start...")
    TInt msgId = aParser.MessageId();
    TInt status(0);

    switch(msgId)
        {
        case ENmeaPNokPeriodicEvents:
            //Handler PNok STAT message
            HandleStatMessage(aParser);
            break;
        case ENmeaPNokNightMode:
            if(aParser.GetFieldData(ENmeaFieldPNOKNightModeStatus, status)==KErrNone)
                {
                CheckNightModeStatus(status);
                }
            break;
        case ENmeaPNokVersionInfo:
            UpdateDeviceVersion(aParser);
            break;
        default:
            //do nothing
            break;
        }
    TRACESTRING("CBTGPSNokDeviceHandler::HandleMessage end")
    }

// -----------------------------------------------------------------------------
// CBTGPSNokDeviceHandler::HandleNightModeChange
// -----------------------------------------------------------------------------
void CBTGPSNokDeviceHandler::HandleNightModeChange( 
	        const TBTGPSSettingsApi::TNightModeState aSetting )
    {
    TRACESTRING("CBTGPSNokDeviceHandler::HandleNightModeChange start...")
    //Set night mode
    UpdateNightMode(aSetting);
    TRACESTRING("CBTGPSNokDeviceHandler::HandleNightModeChange end")
    }

// -----------------------------------------------------------------------------
// CBTGPSNokDeviceHandler::HandleStatMessage
// -----------------------------------------------------------------------------
void CBTGPSNokDeviceHandler::HandleStatMessage(const TBTGPSNmeaParser& aParser)
    {
    TRACESTRING("CBTGPSNokDeviceHandler::HandleStatMessage start...")
    TInt battStatus(KErrNotFound);
    TInt battLevel(KErrNotFound);
    TInt extPowStatus(KErrNotFound);
    TInt extAntStatus(KErrNotFound);
    
    //Check battery status
    if(aParser.GetFieldData(ENmeaFieldPNOKBatteryStatus, battStatus)==KErrNone)
        {
        //Tuning is needed after testing
        if(battStatus != iBattStatus)
            {
            iBattStatus = battStatus;
            }
        }
        
    //Check ext power status
    if(aParser.GetFieldData(ENmeaFieldPNOKExtPowerStatus, extPowStatus)==KErrNone)
        {
        iExtPowerStatus = extPowStatus;
        }
    
    //Check ext antenna status
    if(aParser.GetFieldData(ENmeaFieldPNOKExtAntennaStatus, extAntStatus)==KErrNone)
        {
        iExtAntennaStatus = extAntStatus;
        }
    
    //Get battery level. Error ignored
    aParser.GetFieldData(ENmeaFieldPNOKBatteryLevel, battLevel);
    
    // Update device status to setting manager
    UpdateDeviceStatus(battLevel,battStatus, extPowStatus, extAntStatus);
    TRACESTRING("CBTGPSNokDeviceHandler::HandleStatMessage end")
    }
        
// -----------------------------------------------------------------------------
// CBTGPSNokDeviceHandler::UpdateDeviceVersion
// -----------------------------------------------------------------------------
void CBTGPSNokDeviceHandler::UpdateDeviceVersion(const TBTGPSNmeaParser& aParser)
    {
    TPtrC8 id;
    TPtrC8 btHwVersion;
    TPtrC8 btSwVersion;
    TPtrC8 gpsHwVersion;
    TPtrC8 gpsSwVersion;
    
    //all errors are ignored
    aParser.GetFieldBlock(ENmeaFieldPNOKProductID, id);
    aParser.GetFieldBlock(ENmeaFieldPNOKBTHWVersionNumber, btHwVersion);
    aParser.GetFieldBlock(ENmeaFieldPNOKBTSWVersionNumber, btSwVersion);
    aParser.GetFieldBlock(ENmeaFieldPNOKGPSHWVersionNumber, gpsHwVersion);
    aParser.GetFieldBlock(ENmeaFieldPNOKGPSSWVersionNumber, gpsSwVersion);
    
    //Update settings in setting manager. Error ignored
    iSettingManager.UpdateVersions(
        id,
        btHwVersion,
        btSwVersion,
        gpsHwVersion,
        gpsSwVersion);
    }
        

// -----------------------------------------------------------------------------
// CBTGPSNokDeviceHandler::UpdateDeviceStatus
// -----------------------------------------------------------------------------
void CBTGPSNokDeviceHandler::UpdateDeviceStatus(
            TInt aBattLevel,
            TInt aBattStatus,
            TInt aExtPowStatus,
            TInt aExtAntStatus)
    {
    //Set batt level
    TInt battLevel(0);
    if(aBattLevel!=KErrNotFound)
        {
        battLevel = aBattLevel;
        }
    
    //Set batt status
    TBTGPSBatteryState battState(EBatteryStateNA);
    switch(aBattStatus)
        {
        case KPNOKStatBattNormal:
            battState = EBatteryStateNormal;
            break;
        case KPNOKStatBattFull:
            battState = EBatteryStateFull;
            break;
        case KPNOKStatBattLow:
            battState = EBatteryStateLow;
            break;
        default:
            //do nothing
            break;
        }
    
    //Set ext power state
    TBTGPSExtPowerState extPowState(EExtPowerNA);
    switch (aExtPowStatus)
        {
        case KPNOKStatExtPowerConnected:
            extPowState = EExtPowerOn;
            break;
        case KPNOKStatExtPowerNotConnected:
            extPowState = EExtPowerOff;
            break;
        default: 
            //do nothing
            break;
        }
        
    //Set ext antenna state
    TBTGPSExtAntennaState extAntState(EExtAntennaNA);
    switch(aExtAntStatus)
        {
        case KPNOKStatExtAntennaNotConnected:
            extAntState = EExtAntennaOff;
            break;
        case KPNOKStatExtAntennaConnected:
            extAntState = EExtAntennaOn;
            break;
        default:
            //do nothing
            break;
        }
        
    //Set device status to setting manager. Error is ignored
    iSettingManager.UpdateDeviceStatus(
        battLevel,
        extPowState,
        extAntState,
        battState);
    }

// -----------------------------------------------------------------------------
// CBTGPSNokDeviceHandler::CheckNightModeStatus
// -----------------------------------------------------------------------------
void CBTGPSNokDeviceHandler::CheckNightModeStatus(TInt aStatus)
    {
    if( iNightModeSetting == TBTGPSSettingsApi::ENightModeOn && 
            aStatus == KPNOKNightNightModeOn ||
        iNightModeSetting == TBTGPSSettingsApi::ENightModeOff && 
            aStatus == KPNOKNightNightModeOff)
        {
        //Correctly set
        delete iNightModeHandler;
        iNightModeHandler = NULL;
        }
    }

// -----------------------------------------------------------------------------
// CBTGPSNokDeviceHandler::UpdateNightMode
// -----------------------------------------------------------------------------
void CBTGPSNokDeviceHandler::UpdateNightMode(
        TBTGPSSettingsApi::TNightModeState aSetting)
    {
    if(iDeviceManager.DeviceType()!=EBTDeviceTypeNokGps)
        {
        return;
        }
        
    delete iNightModeHandler;
    iNightModeHandler = NULL;

    if(aSetting != TBTGPSSettingsApi::ENightModeNA)
        {
        //ignore error if night mode can't be set
        TInt ignore(KErrNone);
        TRAP(ignore,
            iNightModeHandler = CBTGPSNokNightModeHandler::NewL(
                iConnectManager,
                aSetting));
        }
    }

// -----------------------------------------------------------------------------
// CBTGPSNokDeviceHandler::BTDeviceStatusChanged
// -----------------------------------------------------------------------------
void CBTGPSNokDeviceHandler::BTDeviceStatusChanged(
            TInt aConnectStatus, 
            TInt aDeviceType,
            TInt /*aErr*/)
    {
    //Update night mode setting
    if(aConnectStatus==EBTDeviceConnected && aDeviceType ==EBTDeviceTypeNokGps)
        {
        //Set night mode
        TBTGPSSettingsApi::TNightModeState setting;
        if(iSettingManager.GetNightModeSetting(setting) == KErrNone)
            {
            UpdateNightMode(setting);
            }
        }
    else
        {
        //Cancel night mode setting
        delete iNightModeHandler;
        iNightModeHandler = NULL;
        }
        
    //Update PSY state in setting
    UpdatePsyState(aConnectStatus, aDeviceType);
    
    //Update connected device info in setting
    UpdateBtDeviceInfo(aConnectStatus, aDeviceType);
    
    //Update previous bt device type
    iPreviousDeviceType = aDeviceType;
    }

// -----------------------------------------------------------------------------
// CBTGPSNokDeviceHandler::UpdatePsyState
// -----------------------------------------------------------------------------
void CBTGPSNokDeviceHandler::UpdatePsyState(
            TInt aConnectStatus, 
            TInt aDeviceType)
    {
    TBTGPSPSYState psyState(EPSYLoaded);
    
    switch(aConnectStatus)
        {
        case EBTDeviceDisconnected:
            if(aDeviceType==EBTDeviceTypeNonNokGps || 
                aDeviceType==EBTDeviceTypeNokGps)
                {
                psyState = EPSYInStandBy;
                }
            break;
        case EBTDeviceConnected:
            if(aDeviceType == EBTDeviceTypeNonNokGps)
                {
                psyState = EPSYLoadedAndPNOKNotSupportedBTGPSUsed;
                }
            else if(aDeviceType == EBTDeviceTypeNokGps)
                {
                psyState = EPSYLoadedAndPNOKSupportedBTGPSUsed;
                }
            else
                {
                //do nothing
                }
            break;
        case EBTDeviceConnecting:
        case EBTDeviceConnectError:
        default:
            //do nothing
            break;
        }
            
    //Update to setting manager
    iSettingManager.UpdatePsyState(psyState);
    }

// -----------------------------------------------------------------------------
// CBTGPSNokDeviceHandler::UpdateBtDeviceInfo
// -----------------------------------------------------------------------------
void CBTGPSNokDeviceHandler::UpdateBtDeviceInfo(
            TInt aConnectStatus, 
            TInt aDeviceType)
    {
    //Update bt device info when device type is changed from unkown or not GPS to 
    //NokGps or NonNokGps
    if(aConnectStatus == EBTDeviceConnected &&
        aDeviceType != iPreviousDeviceType && 
        (aDeviceType == EBTDeviceTypeNonNokGps ||
         aDeviceType == EBTDeviceTypeNokGps))
        {
        //Get bt sock address from device manager
        TBTSockAddr btSockAddr;
        iDeviceManager.BtSockAddr(btSockAddr);
        
#ifdef SYMBIAN_LOCATION_BTGPSCONFIG
        iDeviceListManager.SetDeviceInfo(aDeviceType, btSockAddr);
#else
        //Determine if the device is valid
        TInt valid = (aDeviceType==EBTDeviceTypeNonNokGps)?
            TBTGPSSettingsApi::EBtDeviceNonNokGps : TBTGPSSettingsApi::EBtDeviceNokGps;

        //Update device info in setting manager
        iSettingManager.UpdateBTDeviceInfo(valid, btSockAddr);
#endif
        }
#ifdef SYMBIAN_LOCATION_BTGPSCONFIG
	else if(aConnectStatus == EBTDeviceConnected)
		{
		//Device address and type has not changed,, but it needs to be moved to the top of the list
		TBTSockAddr btSockAddr;
    	iDeviceManager.BtSockAddr(btSockAddr);
    	iDeviceListManager.SetDeviceInfo(aDeviceType, btSockAddr);
		}
#endif		
    }
        

//  End of File



