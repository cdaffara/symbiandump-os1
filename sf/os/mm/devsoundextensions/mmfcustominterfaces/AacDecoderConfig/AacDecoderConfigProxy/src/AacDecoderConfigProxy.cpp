/*
* Copyright (c) 2002-2004 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:   Interface proxy for AAC decoder configuration.
*
*/



// INCLUDE FILES
#include "AacDecoderConfigProxy.h"
#include "AacDecoderConfigMsgs.h"
#include <CustomCommandUtility.h>
#include <CustomInterfaceUtility.h>

// EXTERNAL DATA STRUCTURES

// EXTERNAL FUNCTION PROTOTYPES  

// CONSTANTS

// MACROS

// LOCAL CONSTANTS AND MACROS

// MODULE DATA STRUCTURES

// LOCAL FUNCTION PROTOTYPES

// FORWARD DECLARATIONS

// ============================= LOCAL FUNCTIONS ===============================

// ================= MEMBER FUNCTIONS =======================

// -----------------------------------------------------------------------------
// CAacDecoderConfigProxy::CAacDecoderConfigProxy
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CAacDecoderConfigProxy::CAacDecoderConfigProxy(
                           TMMFMessageDestinationPckg aMessageHandler, 
                           MCustomCommand& aCustomCommand,
                           CCustomInterfaceUtility* aCustomInterfaceUtility) :
        iCustomCommand(aCustomCommand),
        iMessageHandler(aMessageHandler),
        iCustomInterfaceUtility(aCustomInterfaceUtility)
    {
    }


// -----------------------------------------------------------------------------
// CAacDecoderConfigProxy::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CAacDecoderConfigProxy::ConstructL()
    {
    }

// -----------------------------------------------------------------------------
// CAacDecoderConfigProxy::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
EXPORT_C CAacDecoderConfigProxy* CAacDecoderConfigProxy::NewL(
                              TMMFMessageDestinationPckg aMessageHandler, 
                              MCustomCommand& aCustomCommand,
                              CCustomInterfaceUtility* aCustomInterfaceUtility)
    {
    CAacDecoderConfigProxy* self = new(ELeave) CAacDecoderConfigProxy(
                                                      aMessageHandler,
                                                      aCustomCommand,
                                                      aCustomInterfaceUtility);
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }
    
// Destructor
EXPORT_C CAacDecoderConfigProxy::~CAacDecoderConfigProxy()
    {
    iCustomInterfaceUtility->RemoveCustomInterface(iMessageHandler);
    delete iCustomInterfaceUtility;
    }

// ---------------------------------------------------------
// CAacDecoderConfigProxy::SetAudioConfig
// Sends the custom command for this function to its message handler.
// (other items were commented in a header).
// ---------------------------------------------------------
//
EXPORT_C TInt CAacDecoderConfigProxy::SetAudioConfig(TAudioConfig& aAudioConfig)
    {
	TPckgBuf<TAudioConfig> audioConfigPckgBuf(aAudioConfig);
	TInt status = iCustomCommand.CustomCommandSync(iMessageHandler,
	                                               EAdcmSetConfig,
	                                               audioConfigPckgBuf,
	                                               KNullDesC8);
	return status;
    }


// End of File
