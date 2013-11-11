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
#include <bt_sock.h>
#include <lbserrors.h>

#include "BTGPSPanic.h"
#include "BTGPSRequestHandler.h"
#include "BTGPSConstantsManager.h"
#include "BTGPSSettingManager.h"
#include "BTGPSDeviceManager.h"
#include "BTGPSConnectManager.h"
#include "BTGPSMessageSender.h"
#include "BTGPSInitManager.h"
#include "BTGPSRequestManager.h"
#include "BTGPSPsyConnectionManager.h"
#include "BTGPSNokDeviceHandler.h"
#include "BTGPSEnvChangeHandler.h"
#include "BTGPSModuleStatusHandler.h"
#include "BTGPSPowerManager.h"
#include "BTGPSPositionerExt.h"
#include "BTGPSLogging.h"
#include "btgpsdevicelistmanager.h"

//  CONSTANTS
			
// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CBTGPSRequestHandler::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CBTGPSRequestHandler* CBTGPSRequestHandler::NewL()
    {
    CBTGPSRequestHandler* self = new( ELeave ) CBTGPSRequestHandler;
    
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop();

    return self;
    }

// -----------------------------------------------------------------------------
// CBTGPSRequestHandler::CBTGPSRequestHandler
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CBTGPSRequestHandler::CBTGPSRequestHandler()
    {
    }

    
// -----------------------------------------------------------------------------
// CBTGPSRequestHandler::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CBTGPSRequestHandler::ConstructL()
    {
    TRACESTRING("CBTGPSRequestHandler::ConstructL start...")
    //Construct Constants Mangaer
    iConstantsManager = CBTGPSConstantsManager::NewL();
    
    //Construct the Device List Manager
    iDeviceListManager = CBTGPSDeviceListManager::NewL();
    
    //Construct Setting Manager
    iSettingManager = CBTGPSSettingManager::NewL();
    
    //Construct BT Device Manager
    iDeviceManager = CBTGPSDeviceManager::NewL();
    iDeviceManager->AddListenerL(*this);
    
    //Construct connection manager
    iConnectManager = CBTGPSConnectManager::NewL(
        *iSettingManager,
        *iDeviceManager,
        *iConstantsManager,
        *iDeviceListManager);
    
    //Construct init manager
    iInitManager = CBTGPSInitManager::NewL(
        *iConnectManager,
        *iDeviceManager,
        *iSettingManager);

    //Construct request manager
    iRequestManager = CBTGPSRequestManager::NewL(
        *iConnectManager,
        *iConstantsManager);
    iRequestManager->AddListenerL(*this);
      
    //Construct PSY connection manager
    iPsyConnectionManager = CBTGPSPsyConnectionManager::NewL();
    
    //Construct Nok device handler
    iNokDeviceHandler = CBTGPSNokDeviceHandler::NewL(
        *iConnectManager,
        *iSettingManager,
        *iConstantsManager,
        *iDeviceManager,
        *iDeviceListManager);
    
    //Module status handler
    iModuleStatusHandler = CBTGPSModuleStatusHandler::NewL(
        *iPsyConnectionManager,
        *iDeviceManager,
        *iRequestManager);
        
    //Power manager
    iPowerManager = CBTGPSPowerManager::NewL(
        *iConnectManager,
        *iPsyConnectionManager,
        *iDeviceManager,
        *iConstantsManager);
        
    //Construct Environment change handler
    iEnvChangeHandler = CBTGPSEnvChangeHandler::NewL(*iRequestManager);
    TRACESTRING("CBTGPSRequestHandler::ConstructL end")
    }

// -----------------------------------------------------------------------------
// CBTGPSRequestHandler::~CBTGPSRequestHandler
// Destructor.
// -----------------------------------------------------------------------------
CBTGPSRequestHandler::~CBTGPSRequestHandler()
    {
    if(iDeviceManager != NULL)
        {
        iDeviceManager->RemoveListener(*this);
        }
        
    if(iRequestManager!=NULL)
        {
        iRequestManager->RemoveListener(*this);
        }

    delete iDeviceListManager;    
    delete iEnvChangeHandler;
    delete iPowerManager;
    delete iModuleStatusHandler;
    delete iNokDeviceHandler;
    delete iPsyConnectionManager;
    delete iRequestManager;
    delete iInitManager;
    delete iConnectManager;
    delete iDeviceManager;
    delete iSettingManager;
    delete iConstantsManager;
    Dll::SetTls(NULL);
    }

// -----------------------------------------------------------------------------
// CBTGPSRequestHandler::GetInstanceL
// -----------------------------------------------------------------------------
//
CBTGPSRequestHandler * CBTGPSRequestHandler::GetInstanceL()
    {
    //Have we loaded the request handler already
    if ( !Dll::Tls() ) 
        {
        CBTGPSRequestHandler * theHandler = CBTGPSRequestHandler::NewL();
        Dll::SetTls(theHandler);
        }

    return reinterpret_cast<CBTGPSRequestHandler*>(Dll::Tls());
    }

// -----------------------------------------------------------------------------
// CBTGPSRequestHandler::Instance
// -----------------------------------------------------------------------------
CBTGPSRequestHandler& CBTGPSRequestHandler::Instance()
    {
    if(!Dll::Tls())
        {
        Panic(EPanicSingletonNotConstructed);
        }
    return *(reinterpret_cast<CBTGPSRequestHandler*>(Dll::Tls()));
    }

// -----------------------------------------------------------------------------
// CBTGPSRequestHandler::ConstantsManager
// -----------------------------------------------------------------------------
CBTGPSConstantsManager& CBTGPSRequestHandler::ConstantsManager()
    {
    return *(Instance().iConstantsManager);
    }

// -----------------------------------------------------------------------------
// CBTGPSRequestHandler::AcquireLocation
// -----------------------------------------------------------------------------
void CBTGPSRequestHandler::AcquireLocation(MBTGPSPositionerExt* aPSY)
    {
    TRACESTRING("CBTGPSRequestHandler::AcquireLocation start...")
    //tries to acquire locaiton
    TRAPD(err, AcquireLocationL(aPSY));
    if(err!=KErrNone)
        {
        //Convert error code
        if(err == KErrCouldNotConnect)
            {
            err = KPositionQualityLoss;
            }
            
        //if function leaves, then complete request with error code
        CompleteRequest(*aPSY, err);
        }
    TRACESTRING("CBTGPSRequestHandler::AcquireLocation end")
    }

// -----------------------------------------------------------------------------
// CBTGPSRequestHandler::AcquireLocation
// -----------------------------------------------------------------------------
void CBTGPSRequestHandler::AcquireLocationL(MBTGPSPositionerExt* aPSY)
    {
    //Make location request to connection manager
    iPsyConnectionManager->AcquireLocation(aPSY);

    //Connect to GPS
    User::LeaveIfError(iConnectManager->Connecting());

    //Retrieve Fix
    iRequestManager->RetrieveFixL();
    
    }

// -----------------------------------------------------------------------------
// CBTGPSRequestHandler::CancelAcquireLocation
// -----------------------------------------------------------------------------
void CBTGPSRequestHandler::CancelAcquireLocation(
                                    MBTGPSPositionerExt* aPSY)
{
    TRACESTRING("CBTGPSRequestHandler::CancelAcquireLocation start...")
    //Cancel location request from connection manager
    iPsyConnectionManager->CancelAcquireLocation(aPSY);

    if(iPsyConnectionManager->LocationRequestCount() == 0)
        {
        //When no location request, stop retrieving fix
        iRequestManager->StopRetrievingFix();
        }
    TRACESTRING("CBTGPSRequestHandler::CancelAcquireLocation end")
}

// -----------------------------------------------------------------------------
// CBTGPSRequestHandler::RegisterPSYL
// -----------------------------------------------------------------------------
void CBTGPSRequestHandler::RegisterPSYL(MBTGPSPositionerExt* aPSY)
    {
    TRACESTRING("CBTGPSRequestHandler::RegisterPSYL start...")
    //Register the psy
    iPsyConnectionManager->RegisterPSYL(aPSY);
    
    //Reconnecting to GPS and try to recover from error states
    iConnectManager->Reconnecting();
    
    //Set NMEA index
    aPSY->SetInitialNmeaBufferBottom(
        iRequestManager->GetPreviousNmeaBufferBottom());
        
    TRACESTRING2("Initial NMEA Buffer Bottom: %d", 
        iRequestManager->GetPreviousNmeaBufferBottom());
    TRACESTRING("CBTGPSRequestHandler::RegisterPSYL end")
    }

// -----------------------------------------------------------------------------
// CBTGPSRequestHandler::UnregisterPSY
// -----------------------------------------------------------------------------
void CBTGPSRequestHandler::UnregisterPSY(MBTGPSPositionerExt* aPSY)
    {
    TRACESTRING("CBTGPSRequestHandler::UnregisterPSY start...")
    //Unregister PSY
    iPsyConnectionManager->UnregisterPSY(aPSY);

    //If no PSY is connected, then delete this instance
    if ( iPsyConnectionManager->Count() == 0 )
        {
        //Report device status before destruction
        TPositionModuleStatus moduleStatus;
        moduleStatus.SetDataQualityStatus(TPositionModuleStatus::EDataQualityUnknown);
        moduleStatus.SetDeviceStatus(TPositionModuleStatus::EDeviceInactive);
        aPSY->ReportStatus(moduleStatus);
        
        delete this;
        }
    TRACESTRING("CBTGPSRequestHandler::UnregisterPSY end")
    }

// -----------------------------------------------------------------------------
// CBTGPSRequestHandler::TrackingSessionStart
// -----------------------------------------------------------------------------
void CBTGPSRequestHandler::TrackingSessionStartL(
    MBTGPSPositionerExt* aPSY,
    const TTimeIntervalMicroSeconds& aInterval)
    {
    TRACESTRING("CBTGPSRequestHandler::TrackingSessionStartL start...")
    iPsyConnectionManager->TrackingSessionStartL(aPSY, aInterval);
    TRACESTRING("CBTGPSRequestHandler::TrackingSessionStartL end")
    }

// -----------------------------------------------------------------------------
// CBTGPSRequestHandler::TrackingSessionStop
// -----------------------------------------------------------------------------
void CBTGPSRequestHandler::TrackingSessionStop(
                                    MBTGPSPositionerExt* aPSY)
    {
    TRACESTRING("CBTGPSRequestHandler::TrackingSessionStop start...")
    iPsyConnectionManager->TrackingSessionStop(aPSY);
    TRACESTRING("CBTGPSRequestHandler::TrackingSessionStop end")
    }
            
// -----------------------------------------------------------------------------
// CBTGPSRequestHandler::LastLocation
// -----------------------------------------------------------------------------
TInt CBTGPSRequestHandler::LastLocation(
                                         const CBTGPSFix*& aFix,
                                         const TTime& aMaxAge,
                                         const TBool aAcceptPartial)
    {
    return iRequestManager->LastLocation(aFix, aMaxAge, aAcceptPartial);
    }

// -----------------------------------------------------------------------------
// CBTGPSRequestHandler::CompleteRequest
// -----------------------------------------------------------------------------
void CBTGPSRequestHandler::CompleteRequest(
            MBTGPSPositionerExt& aPSY, 
            TInt aErr)
    {
    iPsyConnectionManager->CompleteRequest(aPSY, aErr);
    }

// -----------------------------------------------------------------------------
// CBTGPSRequestHandler::CompleteAllRequests
// -----------------------------------------------------------------------------
void CBTGPSRequestHandler::CompleteAllRequests(TInt aErr)
    {
    iPsyConnectionManager->CompleteAllRequests(aErr);
    }

// -----------------------------------------------------------------------------
// CBTGPSRequestHandler::HandleLocationFixAvailable
// -----------------------------------------------------------------------------
//
void CBTGPSRequestHandler::HandleLocationFixUpdate(const CBTGPSFix& aFix)
    {
    iPsyConnectionManager->LocationFixUpdate(aFix);

    if(iPsyConnectionManager->LocationRequestCount() == 0)
        {
        //If no location request, then stop retrieving fix
        iRequestManager->StopRetrievingFix();
        }
    }

// -----------------------------------------------------------------------------
// CBTGPSRequestHandler::BTDeviceStatusChanged
// -----------------------------------------------------------------------------
void CBTGPSRequestHandler::BTDeviceStatusChanged(
            TInt aConnectStatus, 
            TInt /*aDeviceType*/,
            TInt aErr)
    {
    if(aConnectStatus==EBTDeviceConnectError || 
        aConnectStatus == EBTDeviceDisconnected )
        {
        //When connect error or disconnected, then we complete all 
        //request with error KErrQualityLoss
        if(aErr!=KErrNoMemory)
            {
            aErr = KPositionQualityLoss;
            }
        CompleteAllRequests(aErr);
        }
    }


//  End of File

