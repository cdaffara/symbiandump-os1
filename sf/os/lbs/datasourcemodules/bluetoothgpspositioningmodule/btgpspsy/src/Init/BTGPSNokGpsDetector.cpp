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
#include "BTGPSNokGpsDetector.h"
#include "BTGPSDeviceManager.h"
#include "BTGPSMessageDef.h"
#include "BTGPSConnectManager.h"
#include "BTGPSConstantsManager.h"
#include "BTGPSRequestHandler.h"
#include "BTGPSLogging.h"
#include "BTGPSNmeaParser.h"

// EXTERNAL DATA STRUCTURES

// EXTERNAL FUNCTION PROTOTYPES  

// CONSTANTS

// Timer for sending switch to NMEA mode
const TInt KBTGPSSwitchToNmeaModeTimer = 300000; //300ms

// Limit of Switch to NMEA mode message count. If not enough valid
// NMEA message was received, then the device is not a GPS device
const TInt KBTGPSSwitchToNmeaModeLimit = 10; 

// Number of PNOK message that will be sent
const TInt KBTGPSNumberOfPnokDetectMsg = 3;



// MACROS

// LOCAL CONSTANTS AND MACROS

// MODULE DATA STRUCTURES

// LOCAL FUNCTION PROTOTYPES

// FORWARD DECLARATIONS

// ============================= LOCAL FUNCTIONS ===============================

// ============================ MEMBER FUNCTIONS ===============================


// -----------------------------------------------------------------------------
// CBTGPSNokGpsDetector::NewL
// -----------------------------------------------------------------------------
CBTGPSNokGpsDetector* CBTGPSNokGpsDetector::NewL(
        CBTGPSConnectManager& aConnectManager,
        CBTGPSDeviceManager& aDeviceManager)
    {
    CBTGPSNokGpsDetector* self = new (ELeave) CBTGPSNokGpsDetector(
        aConnectManager,
        aDeviceManager);
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop();
    return self;
    }

// -----------------------------------------------------------------------------
// CBTGPSNokGpsDetector::~CBTGPSNokGpsDetector
// -----------------------------------------------------------------------------
CBTGPSNokGpsDetector::~CBTGPSNokGpsDetector()
    {
    Cancel();
    iConnectManager.RemoveMessageListener(*this);
    delete iTimer;
    }


// -----------------------------------------------------------------------------
// CBTGPSNokGpsDetector::ConstructL
// -----------------------------------------------------------------------------
void CBTGPSNokGpsDetector::ConstructL()
    {
    iTimer = CPeriodic::NewL(CActive::EPriorityStandard);

    //Add message listener
    iConnectManager.AddMessageListenerL(*this);

    }

// -----------------------------------------------------------------------------
// CBTGPSNokGpsDetector::CBTGPSNokGpsDetector
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
CBTGPSNokGpsDetector::CBTGPSNokGpsDetector(
        CBTGPSConnectManager& aConnectManager,
        CBTGPSDeviceManager& aDeviceManager)
    : iConnectManager(aConnectManager),
      iDeviceManager(aDeviceManager)
    {
    }

// -----------------------------------------------------------------------------
// CBTGPSNokGpsDetector::Detect
// -----------------------------------------------------------------------------
TInt CBTGPSNokGpsDetector::Detect()
    {
    TRAPD(err, DetectL());
    return err;
    }

// -----------------------------------------------------------------------------
// CBTGPSNokGpsDetector::DetectL
// -----------------------------------------------------------------------------
void CBTGPSNokGpsDetector::DetectL()
    {
    TRACESTRING("CBTGPSNokGpsDetector::DetectL start...")
    //Start Nok GPS detector if it's not started

    if(iTimer->IsActive())
        {
        return;
        }

    //Start timer
    iTimer->Start(
        CBTGPSRequestHandler::ConstantsManager().iDelayForPNokGpsDetect, 
        CBTGPSRequestHandler::ConstantsManager().iDelayForPNokGpsDetect, 
        TCallBack(TimerCallBack, this));
    
    //Send PNok polling message. 
    iConnectManager.SendMessageL(KNMEAPNOK);
    iNumPnokPolling = 1;
    
    TRACESTRING("CBTGPSNokGpsDetector::DetectL end")
    }

// -----------------------------------------------------------------------------
// CBTGPSNokGpsDetector::Cancel
// -----------------------------------------------------------------------------
void CBTGPSNokGpsDetector::Cancel()
    {
    if(iTimer!=NULL)
        {
        iTimer->Cancel();
        }
    }


// -----------------------------------------------------------------------------
// CBTGPSNokGpsDetector::HandleMessage
// -----------------------------------------------------------------------------
void CBTGPSNokGpsDetector::HandleMessage(const TBTGPSNmeaParser& aParser)
    {
    if(!iTimer->IsActive())
        {
        return;
        }
    TInt deviceType = iDeviceManager.DeviceType();
    if(deviceType==EBTDeviceTypeUnknown && aParser.IsValid())
        {
        iValidNmeaMsg++;
        TInt sentences = CBTGPSRequestHandler::ConstantsManager().iNumberOfTestNmeaSentences;
        if(iValidNmeaMsg==sentences)
            {
            //Non-Nok GPS
            TRACESTRING("CBTGPSNokGpsDetector::HandleMsg Non Nok Gps")
            iDeviceManager.SetBTDeviceType(EBTDeviceTypeNonNokGps);
            }
        }
        
    
    if(aParser.MessageId() == ENmeaPNokOk)
        {
        //The BT device is Nok GPS
        TRACESTRING("CBTGPSNokGpsDetector::HandleMessage: Nok GPS");
        iDeviceManager.SetBTDeviceType(EBTDeviceTypeNokGps);
        Cancel();
        }
    }

// -----------------------------------------------------------------------------
// CBTGPSNokGpsDetector::TimerTick
// -----------------------------------------------------------------------------
void CBTGPSNokGpsDetector::TimerTick()
    {
    //Cancel periodic timer
    iTimer->Cancel();
    
    if(iNumPnokPolling < KBTGPSNumberOfPnokDetectMsg)
        {
        iConnectManager.SendMessage(KNMEAPNOK);

        iNumPnokPolling++;
        //Start timer
        iTimer->Start(
            CBTGPSRequestHandler::ConstantsManager().iDelayForPNokGpsDetect, 
            CBTGPSRequestHandler::ConstantsManager().iDelayForPNokGpsDetect, 
            TCallBack(TimerCallBack, this));
        return;
        }
    
    if(iDeviceManager.DeviceType()!=EBTDeviceTypeUnknown)
        {
        //not send switch to NMEA mode message if device
        //type is known
        return;
        }

    if(iSwitchCount>=KBTGPSSwitchToNmeaModeLimit)
        {
        //Not a GPS device
        TRACESTRING("CBTGPSNokGpsDetector::TimerTick: Not GPS")
        iDeviceManager.SetBTDeviceType(EBTDeviceTypeNonGPS);
        }
    else
        {
        //Send switch to NMEA mode message and send set to FullPower mode message
        iConnectManager.SendMessage(KSirfToNMEA);
        iSwitchCount++;
        iTimer->Start(
            KBTGPSSwitchToNmeaModeTimer,
            KBTGPSSwitchToNmeaModeTimer,
            TCallBack(TimerCallBack,this));
        }
    }
        
// -----------------------------------------------------------------------------
// CBTGPSNokGpsDetector::TimerCallBack
// -----------------------------------------------------------------------------
TInt CBTGPSNokGpsDetector::TimerCallBack(TAny* aAny)
    {
    reinterpret_cast<CBTGPSNokGpsDetector*>(aAny)->TimerTick();
    return KErrNone;
    }



//  End of File



