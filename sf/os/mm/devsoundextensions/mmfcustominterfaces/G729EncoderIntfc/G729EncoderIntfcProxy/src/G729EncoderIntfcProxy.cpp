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
* Description:   Interface proxy for G729 encoder.
*
*/



// INCLUDE FILES
#include "G729EncoderIntfcProxy.h"
#include "G729EncoderIntfcMsgs.h"
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
// CG729EncoderIntfcProxy::CG729EncoderIntfcProxy
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CG729EncoderIntfcProxy::CG729EncoderIntfcProxy(
                             TMMFMessageDestinationPckg aMessageHandler, 
                             MCustomCommand& aCustomCommand,
                             CCustomInterfaceUtility* aCustomInterfaceUtility) :
	iCustomCommand(aCustomCommand),
	iMessageHandler(aMessageHandler),
	iCustomInterfaceUtility(aCustomInterfaceUtility)
    {
    }


// -----------------------------------------------------------------------------
// CG729EncoderIntfcProxy::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CG729EncoderIntfcProxy::ConstructL()
    {
    }

// -----------------------------------------------------------------------------
// CG729EncoderIntfcProxy::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
EXPORT_C CG729EncoderIntfcProxy* CG729EncoderIntfcProxy::NewL(
                              TMMFMessageDestinationPckg aMessageHandler, 
                              MCustomCommand& aCustomCommand,
                              CCustomInterfaceUtility* aCustomInterfaceUtility)
    {
    CG729EncoderIntfcProxy* self = new(ELeave) CG729EncoderIntfcProxy(
                                                        aMessageHandler,
                                                        aCustomCommand,
                                                        aCustomInterfaceUtility);
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }
    
// Destructor
EXPORT_C CG729EncoderIntfcProxy::~CG729EncoderIntfcProxy()
    {
    iCustomInterfaceUtility->RemoveCustomInterface(iMessageHandler);
    delete iCustomInterfaceUtility;
    }

// ---------------------------------------------------------
// CG729EncoderIntfcProxy::SetVadMode
// Sends the custom command for this function to its message handler.
// (other items were commented in a header).
// ---------------------------------------------------------
//
EXPORT_C TInt CG729EncoderIntfcProxy::SetVadMode(TBool aVadMode)
	{
	TPckgBuf<TBool> pckgBuf(aVadMode);
	TInt status = iCustomCommand.CustomCommandSync(iMessageHandler,
	                                               EG729eimSetVadMode,
	                                               pckgBuf,
	                                               KNullDesC8);
	return status;
	}

// ---------------------------------------------------------
// CG729EncoderIntfcProxy::GetVadMode
// Sends the custom command for this function to its message handler.
// (other items were commented in a header).
// ---------------------------------------------------------
//
EXPORT_C TInt CG729EncoderIntfcProxy::GetVadMode(TBool& aVadMode)
	{
	TInt vadMode = EFalse;
	TPckgBuf<TBool> pckgBuf(vadMode);
	TInt status = iCustomCommand.CustomCommandSync(iMessageHandler,
	                                               EG729eimGetVadMode,
	                                               KNullDesC8,
	                                               KNullDesC8,
	                                               pckgBuf);
	if (status == KErrNone)
	    aVadMode = pckgBuf();
	return status;
	}

    


// End of File
