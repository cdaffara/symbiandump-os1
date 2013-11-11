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
#include "BTGPSNokGpsInitialiser.h"
#include "BTGPSMessageDef.h"
#include "BTGPSConnectManager.h"
#include "BTGPSSettingManager.h"
#include "BTGPSSettingsApi.h"
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
// CBTGPSNokGpsInitialiser::NewL
// -----------------------------------------------------------------------------
CBTGPSNokGpsInitialiser* CBTGPSNokGpsInitialiser::NewL(
        CBTGPSConnectManager& aConnectManager,
        CBTGPSSettingManager& aSettingManager,
        CBTGPSNokGpsInitialiser*& aOwner)
    {
    CBTGPSNokGpsInitialiser* self = new (ELeave) CBTGPSNokGpsInitialiser(
        aConnectManager,
        aSettingManager,
        aOwner);
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop();
    return self;
    }

// -----------------------------------------------------------------------------
// CBTGPSNokGpsInitialiser::New
// -----------------------------------------------------------------------------
CBTGPSNokGpsInitialiser* CBTGPSNokGpsInitialiser::New(
        CBTGPSConnectManager& aConnectManager,
        CBTGPSSettingManager& aSettingManager,
        CBTGPSNokGpsInitialiser*& aOwner)
    {
    CBTGPSNokGpsInitialiser* self = NULL;
    TInt err(KErrNone);
    TRAP(err, self = CBTGPSNokGpsInitialiser::NewL(
        aConnectManager,
        aSettingManager,
        aOwner));
    return self;
    }

// -----------------------------------------------------------------------------
// CBTGPSNokGpsInitialiser::~CBTGPSNokGpsInitialiser
// -----------------------------------------------------------------------------
CBTGPSNokGpsInitialiser::~CBTGPSNokGpsInitialiser()
    {
    iConnectManager.RemoveMessageListener(*this);
    //reset owner
    iOwner = NULL;
    }


// -----------------------------------------------------------------------------
// CBTGPSNokGpsInitialiser::ConstructL
// -----------------------------------------------------------------------------
void CBTGPSNokGpsInitialiser::ConstructL()
    {
    //Add listener
    iConnectManager.AddMessageListenerL(*this);
    
    //Set wanted message
    iConnectManager.SendMessageL(KNMEAPNOKSelectMessage);
    }

// -----------------------------------------------------------------------------
// CBTGPSNokGpsInitialiser::CBTGPSNokGpsInitialiser
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
CBTGPSNokGpsInitialiser::CBTGPSNokGpsInitialiser(
        CBTGPSConnectManager& aConnectManager,
        CBTGPSSettingManager& aSettingManager,
        CBTGPSNokGpsInitialiser*& aOwner)
    : iConnectManager(aConnectManager),
      iSettingManager(aSettingManager),
      iOwner(aOwner)
    {
    }

// -----------------------------------------------------------------------------
// CBTGPSNokGpsInitialiser::HandleMessage
// -----------------------------------------------------------------------------
void CBTGPSNokGpsInitialiser::HandleMessage(const TBTGPSNmeaParser& aParser)
    {
    if(aParser.MessageId() == ENmeaPNokPeriodicMsgs )
        {
        iConnectManager.SendMessage(KNMEAPNOKToPowerSaving1s);
        }
    else if(aParser.MessageId() == ENmeaPNokLowPwrMode)
        {
        //initialisation complete
        //Night mode setting is done by CBTGPSNokDeviceHandler
        delete this;
        }
    else 
        {
        //If other PNOK messages are received
        if(aParser.MessageId()> ENmeaProprietaryNok)
            {
            iConnectManager.SendMessage(KNMEAPNOKSelectMessage);
            }
        }
    }

//  End of File



