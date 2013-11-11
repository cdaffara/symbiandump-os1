/*
* Copyright (c) 2006 Nokia Corporation and/or its subsidiary(-ies). 
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description:   Interface proxy for AddedDevSoundControl CI.
*
*/



// INCLUDE FILES
#include <CustomCommandUtility.h>
#include <CustomInterfaceUtility.h>
#include "AddedDevSoundControlMsgs.h"
#include <AddedDevSoundControlProxy.h>

// EXTERNAL DATA STRUCTURES

// EXTERNAL FUNCTION PROTOTYPES

// CONSTANTS

// MACROS

// LOCAL CONSTANTS AND MACROS

// MODULE DATA STRUCTURES

// LOCAL FUNCTION PROTOTYPES

// FORWARD DECLARATIONS

// ============================= LOCAL FUNCTIONS ===============================

// ============================= MEMBER FUNCTIONS ==============================

/**
 * CAddedDevSoundControlProxy::CAddedDevSoundControlProxy
 * C++ default constructor can NOT contain any code, that
 * might leave.
 */
CAddedDevSoundControlProxy::CAddedDevSoundControlProxy(
                            TMMFMessageDestinationPckg aMessageHandler,
                            MCustomCommand& aCustomCommand,
                            CCustomInterfaceUtility* aCustomInterfaceUtility) :
    iCustomCommand(aCustomCommand),
    iMessageHandler(aMessageHandler),
    iCustomInterfaceUtility(aCustomInterfaceUtility)
    {
    }

/**
 * CAddedDevSoundControlProxy::NewL
 * Two-phased constructor.
 */
EXPORT_C CAddedDevSoundControlProxy* CAddedDevSoundControlProxy::NewL(
                            TMMFMessageDestinationPckg aMessageHandler,
                            MCustomCommand& aCustomCommand,
                            CCustomInterfaceUtility* aCustomInterfaceUtility)
    {
    CAddedDevSoundControlProxy* self =
            new(ELeave) CAddedDevSoundControlProxy(aMessageHandler,
                                                   aCustomCommand,
                                                   aCustomInterfaceUtility);
    return self;                                                   
    }

/**
 * Destructor
 */
CAddedDevSoundControlProxy::~CAddedDevSoundControlProxy()
    {
    if(iCustomInterfaceUtility)
     { 
        iCustomInterfaceUtility->RemoveCustomInterface(iMessageHandler);
        delete iCustomInterfaceUtility;
        iCustomInterfaceUtility = NULL;
     }
    }


// From MAddedDevSoundControl

/**
 * Handles client request to alter DevSound's behavior for Pause.
 * (other items defined in the header)
 */
TInt CAddedDevSoundControlProxy::SetHwAwareness(TBool aHwAware)
    {
	TPckgBuf<TBool> hwAwarePckgBuf(aHwAware);
	return iCustomCommand.CustomCommandSync(iMessageHandler,
                                            EAddedDSControlSetHwAwareness,
                                            hwAwarePckgBuf,
                                            KNullDesC8);
    }

/**
 * Handles client request to pause the audio resources and flush.
 * (other items defined in the header)
 */
TInt CAddedDevSoundControlProxy::PauseAndFlush()
    {
	return iCustomCommand.CustomCommandSync(iMessageHandler,
                                            EAddedDSControlPauseAndFlush,
                                            KNullDesC8,
                                            KNullDesC8);
    }

// End of File
