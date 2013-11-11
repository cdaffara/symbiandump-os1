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
* Description:   Interface proxy for error concealment.
*
*/



// INCLUDE FILES
#include "ErrorConcealmentIntfcProxy.h"
#include "ErrorConcealmentIntfcMsgs.h"
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
// CErrorConcealmentIntfcProxy::CErrorConcealmentIntfcProxy
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CErrorConcealmentIntfcProxy::CErrorConcealmentIntfcProxy(
                             TMMFMessageDestinationPckg aMessageHandler, 
                             MCustomCommand& aCustomCommand,
                             CCustomInterfaceUtility* aCustomInterfaceUtility) :
	iCustomCommand(aCustomCommand),
	iMessageHandler(aMessageHandler),
	iCustomInterfaceUtility(aCustomInterfaceUtility)
    {
    }


// -----------------------------------------------------------------------------
// CErrorConcealmentIntfcProxy::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CErrorConcealmentIntfcProxy::ConstructL()
    {
    }

// -----------------------------------------------------------------------------
// CErrorConcealmentIntfcProxy::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
EXPORT_C CErrorConcealmentIntfcProxy* CErrorConcealmentIntfcProxy::NewL(
                              TMMFMessageDestinationPckg aMessageHandler, 
                              MCustomCommand& aCustomCommand,
                              CCustomInterfaceUtility* aCustomInterfaceUtility)
    {
    CErrorConcealmentIntfcProxy* self = new(ELeave) CErrorConcealmentIntfcProxy(
                                                        aMessageHandler,
                                                        aCustomCommand,
                                                        aCustomInterfaceUtility);
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }
    
// Destructor
EXPORT_C CErrorConcealmentIntfcProxy::~CErrorConcealmentIntfcProxy()
    {
    iCustomInterfaceUtility->RemoveCustomInterface(iMessageHandler);
    delete iCustomInterfaceUtility;
    }

// ---------------------------------------------------------
// CErrorConcealmentIntfcProxy::ConcealErrorForNextBuffer
// Sends the custom command for this function to its message handler.
// (other items were commented in a header).
// ---------------------------------------------------------
//
EXPORT_C TInt CErrorConcealmentIntfcProxy::ConcealErrorForNextBuffer()
    {
	TInt status = iCustomCommand.CustomCommandSync(iMessageHandler,
	                                               EEcimConcealError,
	                                               KNullDesC8,
	                                               KNullDesC8);
	return status;
    
    }
    
// ---------------------------------------------------------
// CErrorConcealmentIntfcProxy::SetFrameMode
// Sends the custom command for this function to its message handler.
// (other items were commented in a header).
// ---------------------------------------------------------
//
EXPORT_C TInt CErrorConcealmentIntfcProxy::SetFrameMode(TBool aFrameMode)
    {
	TPckgBuf<TBool> frameModePckgBuf(aFrameMode);
	TInt status = iCustomCommand.CustomCommandSync(iMessageHandler,
	                                               EEcimSetFrameMode,
	                                               frameModePckgBuf,
	                                               KNullDesC8);
	return status;
    
    }
    
// ---------------------------------------------------------
// CErrorConcealmentIntfcProxy::FrameModeRqrdForEC
// Sends the custom command for this function to its message handler.
// (other items were commented in a header).
// ---------------------------------------------------------
//
EXPORT_C TInt CErrorConcealmentIntfcProxy::FrameModeRqrdForEC(
                                                       TBool& aFrameModeRqrd)
    {
	TBool frameModeRqrd = EFalse;
	TPckgBuf<TBool> frameModeRqrdPckg(frameModeRqrd);
	TInt status = iCustomCommand.CustomCommandSync(iMessageHandler,
	                                               EEcimFrameModeRqrd,
	                                               KNullDesC8,
	                                               KNullDesC8,
	                                               frameModeRqrdPckg);
	aFrameModeRqrd = frameModeRqrdPckg();
	return status;
    }



// End of File
