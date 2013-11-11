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
#include <es_sock.h>
#include "BTGPSMessageSender.h"
#include "BTGPSDeviceManager.h"
#include "BTGPSConstantsManager.h"
#include "BTGPSRequestHandler.h"
#include "BTGPSLogging.h"

// EXTERNAL DATA STRUCTURES

// EXTERNAL FUNCTION PROTOTYPES  

// CONSTANTS

// Granularity
const TInt KBTGPSArrayGranularity = 3;

// MACROS

// LOCAL CONSTANTS AND MACROS

// MODULE DATA STRUCTURES

// LOCAL FUNCTION PROTOTYPES

// FORWARD DECLARATIONS

// ============================= LOCAL FUNCTIONS ===============================

// ============================ MEMBER FUNCTIONS ===============================


// -----------------------------------------------------------------------------
// CBTGPSMessageSender::NewL
// -----------------------------------------------------------------------------
CBTGPSMessageSender* CBTGPSMessageSender::NewL(
    RSocket& aSocket,
    CBTGPSDeviceManager& aDeviceManager)
    {
    CBTGPSMessageSender* self = new (ELeave) CBTGPSMessageSender(
        aSocket,
        aDeviceManager);
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop();
    return self;
    }

// -----------------------------------------------------------------------------
// CBTGPSMessageSender::~CBTGPSMessageSender
// -----------------------------------------------------------------------------
CBTGPSMessageSender::~CBTGPSMessageSender()
    {
    Cancel();
    if(iTimer!=NULL)
        {
        iTimer->Cancel();
        delete iTimer;
        }
    delete iBuffer;
    }

// -----------------------------------------------------------------------------
// CBTGPSMessageSender::ConstructL
// -----------------------------------------------------------------------------
void CBTGPSMessageSender::ConstructL()
    {
    iBuffer = new (ELeave) CDesC8ArrayFlat(KBTGPSArrayGranularity);
    iTimer = CPeriodic::NewL(CActive::EPriorityStandard);
    }

// -----------------------------------------------------------------------------
// CBTGPSMessageSender::CBTGPSMessageSender
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
CBTGPSMessageSender::CBTGPSMessageSender(
        RSocket& aSocket,
        CBTGPSDeviceManager& aDeviceManager)
    : CActive(EPriorityNormal),
      iSocket(aSocket),
      iDeviceManager(aDeviceManager)
    {
    CActiveScheduler::Add(this);
    }

// -----------------------------------------------------------------------------
// CBTGPSMessageSender::SendL
// -----------------------------------------------------------------------------
void CBTGPSMessageSender::SendL(const TDesC8& aMsg)
    {
    //if device is not connected then fail the sending
    if(iDeviceManager.DeviceConnectStatus() != EBTDeviceConnected)
        {
        User::Leave(KErrDisconnected);
        }
    
    if(IsActive() || iTimer->IsActive())
        {
        //Add this message to buffer
        iBuffer->AppendL(aMsg);
        }
    else
        {
        //Send this message
        TRACESTRING("CBTGPSMessageSender::SendL sending")
        TRACESTRING8(aMsg)
        iSocket.Write(aMsg, iStatus);
        SetActive();
        StartTimer();
        }
    }
    
// -----------------------------------------------------------------------------
// CBTGPSMessageSender::StartTimer
// -----------------------------------------------------------------------------
void CBTGPSMessageSender::StartTimer()
    {
    iTimer->Cancel();
    iTimer->Start(
        CBTGPSRequestHandler::ConstantsManager().iSendDelay,
        CBTGPSRequestHandler::ConstantsManager().iSendDelay,
        TCallBack(TimerCallback, this));
    }
    
// -----------------------------------------------------------------------------
// CBTGPSMessageSender::TimerTick
// -----------------------------------------------------------------------------
void CBTGPSMessageSender::TimerTick()
    {
    if(iBuffer->MdcaCount()!=0)
        {
        //check device connectivity
        if(iDeviceManager.DeviceConnectStatus() != EBTDeviceConnected)
            {
            Cancel();
            }
        else
            {
            //Buffer is not empty
            if(!IsActive())
                {
                TRACESTRING("CBTGPSMessageSender::TimerTick sending")
                TRACESTRING8(iBuffer->MdcaPoint(0))
                iSocket.Write(iBuffer->MdcaPoint(0), iStatus); //Send the first one
                SetActive();
                iBuffer->Delete(0); //Delete it from buffer
                }
            }
        }
    else
        {
        //Stop timer
        iTimer->Cancel();
        }
    }
        
// -----------------------------------------------------------------------------
// CBTGPSMessageSender::TimerCallback
// -----------------------------------------------------------------------------
TInt CBTGPSMessageSender::TimerCallback(TAny* aAny)
    {
    reinterpret_cast<CBTGPSMessageSender*>(aAny)->TimerTick();
    return 1; //continue periodic timer
    }

// -----------------------------------------------------------------------------
// CBTGPSMessageSender::RunL
// -----------------------------------------------------------------------------
void CBTGPSMessageSender::RunL()
    {
    //error is ignored. do nothing
    }
        
// -----------------------------------------------------------------------------
// CBTGPSMessageSender::DoCancel
// -----------------------------------------------------------------------------
void CBTGPSMessageSender::DoCancel()
    {
    iSocket.CancelWrite();
    if(iBuffer!=NULL)
        {
        iBuffer->Reset();
        }
    }
        
// -----------------------------------------------------------------------------
// CBTGPSMessageSender::RunError
// -----------------------------------------------------------------------------
TInt CBTGPSMessageSender::RunError(TInt /*aError*/)
    {
    //should never be called
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CBTGPSMessageSender::BTDeviceStatusChanged
// -----------------------------------------------------------------------------
void CBTGPSMessageSender::BTDeviceStatusChanged(
            TInt aConnectStatus, 
            TInt /*aDeviceType*/,
            TInt /*aErr*/)
    {
    if(aConnectStatus!=EBTDeviceConnected)
        {
        //reset buffer is device is not connected
        Cancel();
        }
    }

//  End of File



