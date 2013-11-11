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
#include <lbscommon.h>
#include "BTGPSModuleStatusHandler.h"
#include "BTGPSPsyConnectionManager.h"
#include "BTGPSDeviceManager.h"
#include "BTGPSRequestManager.h"
#include "BTGPSFix.h"

// EXTERNAL DATA STRUCTURES

// EXTERNAL FUNCTION PROTOTYPES  

// CONSTANTS

// MACROS

// LOCAL CONSTANTS AND MACROS

// MODULE DATA STRUCTURES

// LOCAL FUNCTION PROTOTYPES

// FORWARD DECLARATIONS

// ============================= LOCAL FUNCTIONS ===============================

// ============================ MEMBER FUNCTIONS ===============================


// -----------------------------------------------------------------------------
// CBTGPSModuleStatusHandler::NewL
// -----------------------------------------------------------------------------
CBTGPSModuleStatusHandler* CBTGPSModuleStatusHandler::NewL(
            CBTGPSPsyConnectionManager& aPsyConnectionManager,
            CBTGPSDeviceManager& aDeviceManager,
            CBTGPSRequestManager& aRequestManager)
    {
    CBTGPSModuleStatusHandler* self = new (ELeave) CBTGPSModuleStatusHandler(
        aPsyConnectionManager,
        aDeviceManager,
        aRequestManager);
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop();
    return self;
    }

// -----------------------------------------------------------------------------
// CBTGPSModuleStatusHandler::~CBTGPSModuleStatusHandler
// -----------------------------------------------------------------------------
CBTGPSModuleStatusHandler::~CBTGPSModuleStatusHandler()
    {
    iPsyConnectionManager.RemoveListener(*this);
    iRequestManager.RemoveListener(*this);
    iDeviceManager.RemoveListener(*this);
    }

// -----------------------------------------------------------------------------
// CBTGPSModuleStatusHandler::ConstructL
// -----------------------------------------------------------------------------
void CBTGPSModuleStatusHandler::ConstructL()
    {
    iDeviceManager.AddListenerL(*this);
    iRequestManager.AddListenerL(*this);
    iPsyConnectionManager.AddListenerL(*this);
    
    //Set moudle status to not found
    iModuleStatus.SetDataQualityStatus(TPositionModuleStatus::EDataQualityLoss);
    iModuleStatus.SetDeviceStatus(TPositionModuleStatus::EDeviceUnknown);
    }

// -----------------------------------------------------------------------------
// CBTGPSModuleStatusHandler::CBTGPSModuleStatusHandler
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
CBTGPSModuleStatusHandler::CBTGPSModuleStatusHandler(
            CBTGPSPsyConnectionManager& aPsyConnectionManager,
            CBTGPSDeviceManager& aDeviceManager,
            CBTGPSRequestManager& aRequestManager)
    : iPsyConnectionManager(aPsyConnectionManager),
      iDeviceManager(aDeviceManager),
      iRequestManager(aRequestManager)
    {
    }

// -----------------------------------------------------------------------------
// CBTGPSModuleStatusHandler::HandlePsyConnectionChange
// -----------------------------------------------------------------------------
void CBTGPSModuleStatusHandler::HandlePsyConnectionChange()
    {
    TPositionModuleStatus::TDeviceStatus deviceStatus = GetDeviceStatus();
    if(deviceStatus!=iModuleStatus.DeviceStatus())
        {
        iModuleStatus.SetDeviceStatus(deviceStatus);
        iPsyConnectionManager.ReportStatus(iModuleStatus);
        }
    }

// -----------------------------------------------------------------------------
// CBTGPSModuleStatusHandler::BTDeviceStatusChanged
// -----------------------------------------------------------------------------
void CBTGPSModuleStatusHandler::BTDeviceStatusChanged(
            TInt aConnectStatus, 
            TInt aDeviceType,
            TInt /*aErr*/)
    {
    //Check if device initialising procedure is completed
    if(!iInitialized && 
        (aDeviceType == EBTDeviceTypeNonNokGps || aDeviceType == EBTDeviceTypeNokGps))
        {
        iInitialized = ETrue;
        }
        
    //Check device status
    TPositionModuleStatus::TDeviceStatus deviceStatus = GetDeviceStatus();

    //Check data quality
    TPositionModuleStatus::TDataQualityStatus dataQualityStatus = 
        iModuleStatus.DataQualityStatus();

    if(!(aConnectStatus == EBTDeviceConnected &&
        aDeviceType == EBTDeviceTypeNonNokGps || aDeviceType == EBTDeviceTypeNokGps))
        {
        dataQualityStatus =TPositionModuleStatus::EDataQualityLoss;
        }

    if(dataQualityStatus != iModuleStatus.DataQualityStatus() || 
        deviceStatus != iModuleStatus.DeviceStatus())
        {
        iModuleStatus.SetDataQualityStatus(dataQualityStatus);
        iModuleStatus.SetDeviceStatus(deviceStatus);
        iPsyConnectionManager.ReportStatus(iModuleStatus);
        }
    }

// -----------------------------------------------------------------------------
// CBTGPSModuleStatusHandler::HandleLocationFixUpdate
// -----------------------------------------------------------------------------
void CBTGPSModuleStatusHandler::HandleLocationFixUpdate(
            const CBTGPSFix& aFix)
    {
    if(!aFix.IfFullNmeaPatternReceived())
        {
        //if not full nmea pattern received, then not update data quality change
        return;
        }

    TPositionModuleStatus::TDataQualityStatus dataQualityStatus
        = TPositionModuleStatus::EDataQualityLoss;
    switch(aFix.FixState())
        {
        case CBTGPSFix::EFixValid2D:
            dataQualityStatus = TPositionModuleStatus::EDataQualityPartial;
            break;
        case CBTGPSFix::EFixValid3D:
            dataQualityStatus = TPositionModuleStatus::EDataQualityNormal;
            break;
        case CBTGPSFix::EFixNotValid:
            dataQualityStatus = TPositionModuleStatus::EDataQualityLoss;
            break;
        case CBTGPSFix::EFixValidityUnkown:
        default:
            //do nithing
            break;
        }
    if(dataQualityStatus != iModuleStatus.DataQualityStatus())
        {
        iModuleStatus.SetDataQualityStatus(dataQualityStatus);
        iPsyConnectionManager.ReportStatus(iModuleStatus);
        }
    }

// -----------------------------------------------------------------------------
// CBTGPSModuleStatusHandler::GetDeviceStatus
// -----------------------------------------------------------------------------
TPositionModuleStatus::TDeviceStatus CBTGPSModuleStatusHandler::GetDeviceStatus()
    {
    TPositionModuleStatus::TDeviceStatus deviceStatus = 
        TPositionModuleStatus::EDeviceUnknown;
    TInt connectStatus = iDeviceManager.DeviceConnectStatus();
    TInt deviceType = iDeviceManager.DeviceType();
    
    if(!iInitialized)
        {
        if(connectStatus == EBTDeviceConnectError)
            {
            //in error case, this will not change 
            //device status.
            deviceStatus = TPositionModuleStatus::EDeviceError;
            }
        else
            {
            deviceStatus = TPositionModuleStatus::EDeviceInitialising;
            }
        }
    else
        {
        switch (connectStatus)
            {
            case EBTDeviceDisconnected:
            case EBTDeviceConnecting:
            case EBTDeviceConnectError:
                deviceStatus = TPositionModuleStatus::EDeviceStandBy;
                break;
            case EBTDeviceConnected:
                if(iPsyConnectionManager.LocationRequestCount()>0 || 
                    iPsyConnectionManager.TrackingSessionCount()>0)
                    {
                    deviceStatus = TPositionModuleStatus::EDeviceActive;
                    }
                else
                    {
                    deviceStatus = TPositionModuleStatus::EDeviceReady;
                    }
                break;
            default:
                break;
            }
        }
    return deviceStatus;
    }

//  End of File



