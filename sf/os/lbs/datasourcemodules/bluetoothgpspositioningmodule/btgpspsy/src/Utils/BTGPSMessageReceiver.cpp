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
#include "BTGPSMessageReceiver.h"
#include "BTGPSMessageListener.h"
#include "BTGPSDeviceManager.h"
#include "BTGPSMessageDef.h"
#include "BTGPSLogging.h"
#include "BTGPSNmeaParser.h"

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
// CBTGPSMessageReceiver::NewL
// -----------------------------------------------------------------------------
CBTGPSMessageReceiver* CBTGPSMessageReceiver::NewL(
    RSocket& aSocket,
    CBTGPSDeviceManager& aDeviceManager)
    {
    CBTGPSMessageReceiver* self = new (ELeave) CBTGPSMessageReceiver(
        aSocket,
        aDeviceManager);
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop();
    return self;
    }

// -----------------------------------------------------------------------------
// CBTGPSMessageReceiver::~CBTGPSMessageReceiver
// -----------------------------------------------------------------------------
CBTGPSMessageReceiver::~CBTGPSMessageReceiver()
    {
    Cancel();    
    iDeviceManager.RemoveListener(*this);
    iListenerArray.Close();
    }

// -----------------------------------------------------------------------------
// CBTGPSMessageReceiver::ConstructL
// -----------------------------------------------------------------------------
void CBTGPSMessageReceiver::ConstructL()
    {
    iDeviceManager.AddListenerL(*this);
    }

// -----------------------------------------------------------------------------
// CBTGPSMessageReceiver::CBTGPSMessageReceiver
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
CBTGPSMessageReceiver::CBTGPSMessageReceiver(
        RSocket& aSocket,
        CBTGPSDeviceManager& aDeviceManager)
    : CActive(EPriorityNormal),
      iSocket(aSocket),
      iDeviceManager(aDeviceManager)
    {
    CActiveScheduler::Add(this);
    }

// -----------------------------------------------------------------------------
// CBTGPSMessageReceiver::AddMessageListenerL
// -----------------------------------------------------------------------------
void CBTGPSMessageReceiver::AddMessageListenerL(MBTGPSMessageListener& aListener)
    {
    //Add listener to array
    User::LeaveIfError(iListenerArray.Append(&aListener));

    //Start listening if device is connected
    if(!IsActive() && iDeviceManager.DeviceConnectStatus() == EBTDeviceConnected)
        {
        Receive();
        }
    }

// -----------------------------------------------------------------------------
// CBTGPSMessageReceiver::AddMessageListenerL
// -----------------------------------------------------------------------------
void CBTGPSMessageReceiver::RemoveMessageListener(MBTGPSMessageListener& aListener)
    {
    //remove listener from array
    TInt index = iListenerArray.Find(&aListener);
    if(index!=KErrNotFound)
        {
        iListenerArray.Remove(index);
        }
        
    //If no listener anymore, then remove receiving
    if(iListenerArray.Count()==0)
        {
        Cancel();
        }
    }

// -----------------------------------------------------------------------------
// CBTGPSMessageReceiver::RunL
// -----------------------------------------------------------------------------
void CBTGPSMessageReceiver::RunL()
    {
    TRACESTRING2("CBTGPSMessageReceiver::RunL start...%d", iStatus.Int())
    if(iStatus == KErrNone)
        {
        HandleReceivedMessage();
        Receive();
        }
    else
        {
        //If connecting error then we report connecting status
        iDeviceManager.SetBTDeviceConnectStatus(
            EBTDeviceDisconnected,
            iStatus.Int());
        }
    TRACESTRING("CBTGPSMessageReceiver::RunL end")
    }
        
// -----------------------------------------------------------------------------
// CBTGPSMessageReceiver::DoCancel
// -----------------------------------------------------------------------------
void CBTGPSMessageReceiver::DoCancel()
    {
    iSocket.CancelRecv();
    }
        
// -----------------------------------------------------------------------------
// CBTGPSMessageReceiver::RunError
// -----------------------------------------------------------------------------
TInt CBTGPSMessageReceiver::RunError(TInt /*aError*/)
    {
    //should never be called
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CBTGPSMessageReceiver::BTDeviceStatusChanged
// -----------------------------------------------------------------------------
void CBTGPSMessageReceiver::BTDeviceStatusChanged(
            TInt aConnectStatus, 
            TInt /*aDeviceType*/,
            TInt /*aErr*/)
    {
    //Start listening if device is connected
    if( aConnectStatus == EBTDeviceConnected 
        && iListenerArray.Count()!=0)
        {
        Receive();
        }
    else
        {
        Cancel();
        }
    }

// -----------------------------------------------------------------------------
// CBTGPSMessageReceiver::Receive
// -----------------------------------------------------------------------------
void CBTGPSMessageReceiver::Receive()
    {
    if(!IsActive())
        {
        iSocket.RecvOneOrMore(iBuffer, 0, iStatus, iRecLen);
        SetActive();
        }
    }

// -----------------------------------------------------------------------------
// CBTGPSMessageReceiver::HandleReceivedMessage
// -----------------------------------------------------------------------------
void CBTGPSMessageReceiver::HandleReceivedMessage()
    {
    for(TInt i=0; i<iBuffer.Length(); i++)
        {
        TUint8 next = iBuffer[i];
        TInt nmeaBufLen = iNmeaBuf.Length();
        if((next==KNmeaSentenceTerminator1 || 
            next == KNmeaSentenceTerminator2))
            {
            if(nmeaBufLen>0)
                {
                //We have a valid nmea message. Inform listeners
                InformListeners();
                iNmeaBuf.Zero();
                }
            }
        else
            {
            if(nmeaBufLen>=KMaxNmeaMessageSize)
                {
                //Empty nmea message buffer if it's overflow
                iNmeaBuf.Zero();
                }
            iNmeaBuf.Append(next);
            }
        }
    //Empty receiving buffer
    iBuffer.Zero();
    }

// -----------------------------------------------------------------------------
// CBTGPSMessageReceiver::InformListeners
// -----------------------------------------------------------------------------
void CBTGPSMessageReceiver::InformListeners()
    {
    TRACESTRING("CBTGPSMessageReceiver::InformListeners start...")
    TRACESTRING8(iNmeaBuf)
    TBTGPSNmeaParser parser;
    parser.SetNmeaSentence(iNmeaBuf);
    if(parser.IsValid())
        {
        TInt count = iListenerArray.Count();
        for(TInt i=count-1; i>=0; i--)
            {
            iListenerArray[i]->HandleMessage(parser);
            }
        }
    TRACESTRING("CBTGPSMessageReceiver::InformListeners end")
    }

//  End of File



