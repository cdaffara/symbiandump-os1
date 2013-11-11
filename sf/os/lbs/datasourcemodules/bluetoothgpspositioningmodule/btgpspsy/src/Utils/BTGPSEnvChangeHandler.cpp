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
#include <bacntf.h>

#include "BTGPSEnvChangeHandler.h"
#include "BTGPSRequestManager.h"
#include "BTGPSLogging.h"

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
// CBTGPSEnvChangeHandler::NewL
// -----------------------------------------------------------------------------
CBTGPSEnvChangeHandler* CBTGPSEnvChangeHandler::NewL(
        CBTGPSRequestManager& aRequestManager)
    {
    CBTGPSEnvChangeHandler* self = new (ELeave) CBTGPSEnvChangeHandler(
        aRequestManager);
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop();
    return self;
    }

// -----------------------------------------------------------------------------
// CBTGPSEnvChangeHandler::~CBTGPSEnvChangeHandler
// -----------------------------------------------------------------------------
CBTGPSEnvChangeHandler::~CBTGPSEnvChangeHandler()
    {
    if(iEnvChangeNotifier!=NULL)
        {
        iEnvChangeNotifier->Cancel();
        }
    delete iEnvChangeNotifier;
    }

// -----------------------------------------------------------------------------
// CBTGPSEnvChangeHandler::ConstructL
// -----------------------------------------------------------------------------
void CBTGPSEnvChangeHandler::ConstructL()
    {
    iEnvChangeNotifier = 
        CEnvironmentChangeNotifier::NewL(
            CActive::EPriorityStandard, 
            TCallBack(NotifierCallBack, this));
    
    iEnvChangeNotifier->Start();
    }

// -----------------------------------------------------------------------------
// CBTGPSEnvChangeHandler::CBTGPSEnvChangeHandler
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
CBTGPSEnvChangeHandler::CBTGPSEnvChangeHandler(
        CBTGPSRequestManager& aRequestManager) : 
    iRequestManager(aRequestManager)
    {
    }

// -----------------------------------------------------------------------------
// CBTGPSEnvChangeHandler::NotifierCallBack
// -----------------------------------------------------------------------------
TInt CBTGPSEnvChangeHandler::NotifierCallBack(TAny* aPtr)
    {
    reinterpret_cast<CBTGPSEnvChangeHandler*>(aPtr)->HandleEnvironmentChange();
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CBTGPSEnvChangeHandler:HandleEnvironmentChange
// 
// Currently we are only interested of system time change. This function is also
// called once immediately after enviroment change notifier object is
// constructed.
//
// NOTE: This function can be used to detect changes in phone power supply
// in case this information is needed to change the functionality depending of
// whether external power supply is connected or not.
//
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CBTGPSEnvChangeHandler::HandleEnvironmentChange()
    {
    TRACESTRING("CBTGPSEnvChangeHandler::HandleEnvironmentChange start...")
    // This function is called once always after construction, in this
    // case we don't even need to check reason
    // --> "Filter out" this initialization
    if (!iEnvChangeNotifierInitialized)
        {
        iEnvChangeNotifierInitialized = ETrue;
        return;
        }

    // Get reason why this function was called
    TInt envChange = iEnvChangeNotifier->Change();

    // If reason was cancel, or system time change flag is not on, then we
    // can return.
     if (envChange == KErrCancel || !(envChange & EChangesSystemTime))
        {
        return;
        }

    // If system time really changed, then we set last fix time as 0
    iRequestManager.SystemTimeChange();
    TRACESTRING("CBTGPSEnvChangeHandler::HandleEnvironmentChange end")
    }

//  End of File



