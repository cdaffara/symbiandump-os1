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
* Description:   Interface proxy for G711 decoder.
*
*/



// INCLUDE FILES
#include "G711DecoderIntfcProxy.h"
#include "G711DecoderIntfcMsgs.h"
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
// CG711DecoderIntfcProxy::CG711DecoderIntfcProxy
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CG711DecoderIntfcProxy::CG711DecoderIntfcProxy(
                             TMMFMessageDestinationPckg aMessageHandler, 
                             MCustomCommand& aCustomCommand,
                             CCustomInterfaceUtility* aCustomInterfaceUtility) :
	iCustomCommand(aCustomCommand),
	iMessageHandler(aMessageHandler),
	iCustomInterfaceUtility(aCustomInterfaceUtility)
    {
    }


// -----------------------------------------------------------------------------
// CG711DecoderIntfcProxy::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CG711DecoderIntfcProxy::ConstructL()
    {
    }

// -----------------------------------------------------------------------------
// CG711DecoderIntfcProxy::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
EXPORT_C CG711DecoderIntfcProxy* CG711DecoderIntfcProxy::NewL(
                              TMMFMessageDestinationPckg aMessageHandler, 
                              MCustomCommand& aCustomCommand,
                              CCustomInterfaceUtility* aCustomInterfaceUtility)
    {
    CG711DecoderIntfcProxy* self = new(ELeave) CG711DecoderIntfcProxy(
                                                        aMessageHandler,
                                                        aCustomCommand,
                                                        aCustomInterfaceUtility);
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }
    
// Destructor
EXPORT_C CG711DecoderIntfcProxy::~CG711DecoderIntfcProxy()
    {
    iCustomInterfaceUtility->RemoveCustomInterface(iMessageHandler);
    delete iCustomInterfaceUtility;
    }

// ---------------------------------------------------------
// CG711DecoderIntfcProxy::SetDecoderMode
// Sends the custom command for this function to its message handler.
// (other items were commented in a header).
// ---------------------------------------------------------
//
EXPORT_C TInt CG711DecoderIntfcProxy::SetDecoderMode(TDecodeMode aDecodeMode)
    {
	TPckgBuf<TDecodeMode> pckgBuf(aDecodeMode);
	TInt status = iCustomCommand.CustomCommandSync(iMessageHandler,
	                                               EG711dimSetDecoderMode,
	                                               pckgBuf,
	                                               KNullDesC8);
	return status;
    
    }
    
// ---------------------------------------------------------
// CG711DecoderIntfcProxy::SetCng
// Sends the custom command for this function to its message handler.
// (other items were commented in a header).
// ---------------------------------------------------------
//
EXPORT_C TInt CG711DecoderIntfcProxy::SetCng(TBool aCng)
    {
	TPckgBuf<TBool> pckgBuf(aCng);
	TInt status = iCustomCommand.CustomCommandSync(iMessageHandler,
	                                               EG711dimSetCng,
	                                               pckgBuf,
	                                               KNullDesC8);
	return status;
    
    }
    
// ---------------------------------------------------------
// CG711DecoderIntfcProxy::GetCng
// Sends the custom command for this function to its message handler.
// (other items were commented in a header).
// ---------------------------------------------------------
//
EXPORT_C TInt CG711DecoderIntfcProxy::GetCng(TBool& aCng)
    {
	TBool cng = EFalse;
	TPckgBuf<TBool> pckgBuf(cng);
	TInt status = iCustomCommand.CustomCommandSync(iMessageHandler,
	                                               EG711dimGetCng,
	                                               KNullDesC8,
	                                               KNullDesC8,
	                                               pckgBuf);
	aCng = pckgBuf();
	return status;
    }

// ---------------------------------------------------------
// CG711DecoderIntfcProxy::SetPlc
// Sends the custom command for this function to its message handler.
// (other items were commented in a header).
// ---------------------------------------------------------
//
EXPORT_C TInt CG711DecoderIntfcProxy::SetPlc(TBool aPlc)
    {
	TPckgBuf<TBool> pckgBuf(aPlc);
	TInt status = iCustomCommand.CustomCommandSync(iMessageHandler,
	                                               EG711dimSetPlc,
	                                               pckgBuf,
	                                               KNullDesC8);
	return status;
    
    }
    

// End of File
