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
//



// INCLUDE FILES
#include <e32std.h>
#include "BTGPSDeviceManager.h"
#include "BTGPSDeviceInfoHandler.h"
#include "BTGPSConnectManagerExt.h"
#include "BTGPSSettingManager.h"
#include "BTGPSHandlerFactory.h"
#include "BTGPSLogging.h"

// EXTERNAL DATA STRUCTURES

// EXTERNAL FUNCTION PROTOTYPES  

// CONSTANTS
// Device array granularity
const TInt KBTGPSDeviceArrayGranularity = 1;

// ============================= LOCAL FUNCTIONS ===============================

// ============================ MEMBER FUNCTIONS ===============================


// -----------------------------------------------------------------------------
// CBTGPSDeviceInfoHandler::NewL
// -----------------------------------------------------------------------------
CBTGPSDeviceInfoHandler* CBTGPSDeviceInfoHandler::NewL(
        MBTGPSConnectManagerExt& aManagerExt)
    {
    CBTGPSDeviceInfoHandler* self = new (ELeave) CBTGPSDeviceInfoHandler(
        aManagerExt);
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop();
    return self;
    }

// -----------------------------------------------------------------------------
// CBTGPSDeviceInfoHandler::~CBTGPSDeviceInfoHandler
// -----------------------------------------------------------------------------
CBTGPSDeviceInfoHandler::~CBTGPSDeviceInfoHandler()
    {
    
    if(iIdle)
        {
        iIdle->Cancel();
        delete iIdle;
        }
    if(iDeviceArray)
        {
        iDeviceArray->ResetAndDestroy();
        delete iDeviceArray;
        }
    delete iBtDevMan;
    }


// -----------------------------------------------------------------------------
// CBTGPSDeviceInfoHandler::ConstructL
// -----------------------------------------------------------------------------
void CBTGPSDeviceInfoHandler::ConstructL()
    {
    TRACESTRING("CBTGPSDeviceInfoHandler::ConstructL start...")
    
    //construct idle object
    iIdle = CIdle::NewL(CActive::EPriorityStandard);
    
    //construct BT device manager
    iBtDevMan = CBTGPSDevMan::NewL(this);
    iDeviceArray = new (ELeave) RBTDeviceArray( KBTGPSDeviceArrayGranularity );
    
    TInt err(KErrNone);

    err = iManagerExt.SettingManager().GetBTDeviceInfo(iValid, iBtSockAddr);

    if(err!=KErrNone || !iValid)
        {
        //device information is not found in setting
        iErrCode = KErrNotFound;
        iIdle->Start(TCallBack(
            HandlerCompleteCallback, 
            this));
        return;
        }

    //Get paired devices
    TBTRegistrySearch searchPattern;
    searchPattern.FindBonded();
    iBtDevMan->GetDevices(searchPattern, iDeviceArray);
    
    TRACESTRING("CBTGPSDeviceInfoHandler::ConstructL end")
    }

// -----------------------------------------------------------------------------
// CBTGPSDeviceInfoHandler::CBTGPSDeviceInfoHandler
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
CBTGPSDeviceInfoHandler::CBTGPSDeviceInfoHandler(
        MBTGPSConnectManagerExt& aManagerExt)
    : iManagerExt(aManagerExt)
    {
    }

// -----------------------------------------------------------------------------
// CBTGPSDeviceInfoHandler::HandleAddDeviceComplete
// -----------------------------------------------------------------------------
void CBTGPSDeviceInfoHandler::HandleAddDeviceComplete(
        TInt /*aErr*/ )
    {
    //nothing to do
    }

// -----------------------------------------------------------------------------
// CBTGPSDeviceInfoHandler::HandleDeleteDevicesComplete
// -----------------------------------------------------------------------------
void CBTGPSDeviceInfoHandler::HandleDeleteDevicesComplete(
        TInt /*aErr*/ )
    {
    //nothing to do
    }

// -----------------------------------------------------------------------------
// CBTGPSDeviceInfoHandler::HandleGetDevicesComplete
// -----------------------------------------------------------------------------
void CBTGPSDeviceInfoHandler::HandleGetDevicesComplete(
            TInt aErr, 
            RBTDeviceArray* aDeviceArray)
    {
    TRACESTRING2("CBTGPSDeviceInfoHandler::HandleGetDevicesComplete start...%d", aErr)
    iErrCode = aErr;

  	if(iErrCode==KErrNone && aDeviceArray)
  	    {
  	    iErrCode = KErrNotFound;
  	    
        TInt count = aDeviceArray->Count();
        for (TInt i = 0; i < count; i++)
            {
            const TBTDevAddr& dev = (*aDeviceArray)[i]->BDAddr();
            if(dev == iBtSockAddr.BTAddr())
                {
                //BT Address is paired
                //Set device info to device manager
                TBTDeviceType deviceType(EBTDeviceTypeUnknown);
                switch(iValid)
                    {
                    case TBTGPSSettingsApi::EBtDeviceInvalid:
                        break;
                    case TBTGPSSettingsApi::EBtDeviceNonNokGps:
                        deviceType = EBTDeviceTypeNonNokGps;
                        break;
                    case TBTGPSSettingsApi::EBtDeviceNokGps:
                    default:
                         deviceType = EBTDeviceTypeNokGps;
                    break;
                    }
                iManagerExt.DeviceManager().SetDeviceInfo(
                    iBtSockAddr,
                    deviceType);

              	//Notify device is paired
              	iErrCode = KErrNone;
              	break;
                }
            }
  	    }
    //Notify result
    iIdle->Start(TCallBack(
        HandlerCompleteCallback, 
        this));
    }

// -----------------------------------------------------------------------------
// CBTGPSDeviceInfoHandler::HandlerCompleteCallback
// -----------------------------------------------------------------------------
TInt CBTGPSDeviceInfoHandler::HandlerCompleteCallback(TAny* aAny)
    {
    reinterpret_cast<CBTGPSDeviceInfoHandler*>( aAny )->
        HandlerCompleteNotify();
        
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CBTGPSDeviceInfoHandler::HandlerCompleteNotify
// -----------------------------------------------------------------------------
void CBTGPSDeviceInfoHandler::HandlerCompleteNotify()
    {
    iManagerExt.HandlerComplete(EDeviceInfoHandler,iErrCode);
    }

//  End of File
