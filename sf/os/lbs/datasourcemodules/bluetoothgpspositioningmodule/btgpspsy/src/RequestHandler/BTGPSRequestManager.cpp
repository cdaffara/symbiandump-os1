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
#include "BTGPSRequestManager.h"
#include "BTGPSConnectManager.h"
#include "BTGPSConstantsManager.h"
#include "BTGPSRequestHandler.h"
#include "BTGPSFix.h"
#include "BTGPSNmeaBuffer.h"
#include "BTGPSNmeaParser.h"
#include "BTGPSLocationFixListener.h"
#include "BTGPSLogging.h"

// EXTERNAL DATA STRUCTURES

// EXTERNAL FUNCTION PROTOTYPES  

// CONSTANTS
//GSV wait time. If GSV sentence is not available, system
//wait wait this much time before the fix is delivered.
const TInt KBTGPSGsvWaitTime = 1000000; //1 second

//NMEA sentence terminator
_LIT8(KBTGPSNmeaTerminator, "\x0d\x0a");

// MACROS

// LOCAL CONSTANTS AND MACROS

// MODULE DATA STRUCTURES

// LOCAL FUNCTION PROTOTYPES

// FORWARD DECLARATIONS

// ============================= LOCAL FUNCTIONS ===============================

// ============================ MEMBER FUNCTIONS ===============================


// -----------------------------------------------------------------------------
// CBTGPSRequestManager::NewL
// -----------------------------------------------------------------------------
CBTGPSRequestManager* CBTGPSRequestManager::NewL(
    CBTGPSConnectManager& aConnectManager,
    CBTGPSConstantsManager& aConstantsManager)
    {
    CBTGPSRequestManager* self = new (ELeave) CBTGPSRequestManager(
        aConnectManager,
        aConstantsManager);
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop();
    return self;
    }

// -----------------------------------------------------------------------------
// CBTGPSRequestManager::~CBTGPSRequestManager
// -----------------------------------------------------------------------------
CBTGPSRequestManager::~CBTGPSRequestManager()
    {
    if(iTimer!=NULL)
        {
        iTimer->Cancel();
        delete iTimer;
        }
        
    delete iFix;
    delete iLastFix;
    iListenerArray.Close();
    delete iNmeaBuffer;
    }

// -----------------------------------------------------------------------------
// CBTGPSRequestManager::ConstructL
// -----------------------------------------------------------------------------
void CBTGPSRequestManager::ConstructL()
    {
    iNmeaBuffer = CBTGPSNmeaBuffer::NewL(
        iConstantsManager.iNmeaBufferSize);

    iFix = new (ELeave) CBTGPSFix();
    iFix->SetNmeaBuffer(iNmeaBuffer);
    
    iLastFix = new (ELeave) CBTGPSFix();
    iTimer = CPeriodic::NewL(CActive::EPriorityStandard);
    
    }

// -----------------------------------------------------------------------------
// CBTGPSRequestManager::CBTGPSRequestManager
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
CBTGPSRequestManager::CBTGPSRequestManager(
    CBTGPSConnectManager& aConnectManager,
    CBTGPSConstantsManager& aConstantsManager)
    : iConnectManager(aConnectManager),
      iConstantsManager(aConstantsManager)
    {
    iPrevBottom = 0;
    }

// -----------------------------------------------------------------------------
// CBTGPSRequestManager::RetrieveFixL
// -----------------------------------------------------------------------------
void CBTGPSRequestManager::RetrieveFixL()
    {
    if(!iReceivingMessage)
        {
        iConnectManager.AddMessageListenerL(*this);
        iReceivingMessage = ETrue;
        }
    }

// -----------------------------------------------------------------------------
// CBTGPSRequestManager::StopRetrievingFix
// -----------------------------------------------------------------------------
void CBTGPSRequestManager::StopRetrievingFix()
    {
    //For better performance, system handles received message
    //when device is connected.
    }


// -----------------------------------------------------------------------------
// CBTGPSRequestManager::HandleMessage
// -----------------------------------------------------------------------------
void CBTGPSRequestManager::HandleMessage(const TBTGPSNmeaParser& aParser)
    {
    //Add the received message to buffer
    iNmeaBuffer->AddSentences(aParser.NmeaSentence());
    //Add 0x0d 0x0a
    iNmeaBuffer->AddSentences(KBTGPSNmeaTerminator);
    
    //Parse the message
    CBTGPSFix::TParsingStatus err = iFix->ParseMessage(aParser);

    //Check if we have received a valid sentence
    if(err == CBTGPSFix::EInfoUpdated)
        {
        if(iFix->IfFullNmeaPatternReceived())
            {
            TTime gsvTime = iFix->GsvTime();
            TTime now;
            now.UniversalTime();
            TRACESTRING2("CBTGPSRequestManager:: Now time = %d", now.Int64())
            TRACESTRING2("CBTGPSRequestManager:: GSV time = %d", gsvTime.Int64())
            if(gsvTime!=0 && now.MicroSecondsFrom(gsvTime)< 
                CBTGPSRequestHandler::ConstantsManager().iSatelliteInfoLifeTime)
                {
                //GSV information is still valid, set fix as valid
                TRACESTRING("CBTGPSRequestManager:: GSV information is still valid")
                iTimer->Cancel();
                InformListeners();
                }
            else
                {
                if(!iTimer->IsActive())
                    {
                    //GSV information is not valid, start timer
                    iTimer->Start(
                        KBTGPSGsvWaitTime,
                        KBTGPSGsvWaitTime,
                        TCallBack(TimerCallback, this));
                    }
                }
            }
        }
    }

// -----------------------------------------------------------------------------
// CBTGPSRequestManager::LastLocation
// -----------------------------------------------------------------------------
TInt CBTGPSRequestManager::LastLocation(
            const CBTGPSFix*& aFix,
            const TTime& aMaxAge,
            const TBool /*aAcceptPartial*/)
    {
    if(aMaxAge.Int64() == 0)
        {
        return KErrNotFound;
        }
        
    if(iLastFix->IsFixValid())
        {
        if(iLastFix->FixTime()>=aMaxAge)
            {
            aFix = iLastFix;
            return KErrNone;
            }
        }
    return KErrNotFound;
    }

// -----------------------------------------------------------------------------
// CBTGPSRequestManager::SystemTimeChange
// -----------------------------------------------------------------------------
void CBTGPSRequestManager::SystemTimeChange()
    {
    iLastFix->Reset();
    
    //Reset GSV time
    iFix->ResetGSVTime();
    }

// -----------------------------------------------------------------------------
// CBTGPSRequestManager::AddListenerL
// -----------------------------------------------------------------------------
void CBTGPSRequestManager::AddListenerL(MBTGPSLocationFixListener& aListener)
    {
    User::LeaveIfError(iListenerArray.Append(&aListener));
    }
        
// -----------------------------------------------------------------------------
// CBTGPSRequestManager::RemoveListener
// -----------------------------------------------------------------------------
void CBTGPSRequestManager::RemoveListener(MBTGPSLocationFixListener& aListener)
    {
    TInt index = iListenerArray.Find(&aListener);
    if(index!=KErrNotFound)
        {
        iListenerArray.Remove(index);
        }
    }

// -----------------------------------------------------------------------------
// CBTGPSRequestManager::InformListeners
// -----------------------------------------------------------------------------
void CBTGPSRequestManager::InformListeners()
    {
    TRACESTRING("CBTGPSRequestManager::InformListeners start...")

    //Inform listeners
    TInt count = iListenerArray.Count();
    for(TInt i=count-1; i>=0; i--)
        {
        iListenerArray[i]->HandleLocationFixUpdate(*iFix);
        }
        
    //If fix is valid then save it to last fix
    if(iFix->IsFixValid())
        {
        *iLastFix = *iFix;
        }
        
    //Reset fix after listeners are informed.
    iFix->ResetFix();
    
    //Set previous bottom of NMEA buffer
    iPrevBottom = iNmeaBuffer->CurrentIndex();

    TRACESTRING("CBTGPSRequestManager::InformListeners end")
    }

// -----------------------------------------------------------------------------
// CBTGPSRequestManager::GetPreviousNmeaBufferBottom
// -----------------------------------------------------------------------------
TInt CBTGPSRequestManager::GetPreviousNmeaBufferBottom() const
    {
    return iPrevBottom;
    }


// -----------------------------------------------------------------------------
// CBTGPSRequestManager::TimerTick
// -----------------------------------------------------------------------------
void CBTGPSRequestManager::TimerTick()
    {
    //Deliver fix even the satellite information is not valid
    TRACESTRING("CBTGPSRequestManager::TimerTick GSV information not valid")

    InformListeners();
    iTimer->Cancel();
    }
       
// -----------------------------------------------------------------------------
// CBTGPSRequestManager::TimerCallback
// -----------------------------------------------------------------------------
TInt CBTGPSRequestManager::TimerCallback(TAny* aAny)
    {
    reinterpret_cast<CBTGPSRequestManager*>(aAny)->TimerTick();
    return KErrNone; //Stop periodic timer
    }

//  End of File



