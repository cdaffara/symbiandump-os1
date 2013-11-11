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
* Description:   Interface proxy for Ilbc decoder.
*
*/



// INCLUDE FILES
#include "IlbcDecoderIntfcProxy.h"
#include "IlbcDecoderIntfcMsgs.h"
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
// CIlbcDecoderIntfcProxy::CIlbcDecoderIntfcProxy
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CIlbcDecoderIntfcProxy::CIlbcDecoderIntfcProxy(
                             TMMFMessageDestinationPckg aMessageHandler, 
                             MCustomCommand& aCustomCommand,
                             CCustomInterfaceUtility* aCustomInterfaceUtility) :
	iCustomCommand(aCustomCommand),
	iMessageHandler(aMessageHandler),
	iCustomInterfaceUtility(aCustomInterfaceUtility)
    {
    }


// -----------------------------------------------------------------------------
// CIlbcDecoderIntfcProxy::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CIlbcDecoderIntfcProxy::ConstructL()
    {
    }

// -----------------------------------------------------------------------------
// CIlbcDecoderIntfcProxy::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
EXPORT_C CIlbcDecoderIntfcProxy* CIlbcDecoderIntfcProxy::NewL(
                              TMMFMessageDestinationPckg aMessageHandler, 
                              MCustomCommand& aCustomCommand,
                              CCustomInterfaceUtility* aCustomInterfaceUtility)
    {
    CIlbcDecoderIntfcProxy* self = new(ELeave) CIlbcDecoderIntfcProxy(
                                                        aMessageHandler,
                                                        aCustomCommand,
                                                        aCustomInterfaceUtility);
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }
    
// Destructor
EXPORT_C CIlbcDecoderIntfcProxy::~CIlbcDecoderIntfcProxy()
    {
    iCustomInterfaceUtility->RemoveCustomInterface(iMessageHandler);
    delete iCustomInterfaceUtility;
    }

// ---------------------------------------------------------
// CIlbcDecoderIntfcProxy::SetDecoderMode
// Sends the custom command for this function to its message handler.
// (other items were commented in a header).
// ---------------------------------------------------------
//
EXPORT_C TInt CIlbcDecoderIntfcProxy::SetDecoderMode(TDecodeMode aDecodeMode)
    {
	TPckgBuf<TDecodeMode> pckgBuf(aDecodeMode);
	TInt status = iCustomCommand.CustomCommandSync(iMessageHandler,
	                                               EIlbcdimSetDecoderMode,
	                                               pckgBuf,
	                                               KNullDesC8);
	return status;
    
    }
    
// ---------------------------------------------------------
// CIlbcDecoderIntfcProxy::SetCng
// Sends the custom command for this function to its message handler.
// (other items were commented in a header).
// ---------------------------------------------------------
//
EXPORT_C TInt CIlbcDecoderIntfcProxy::SetCng(TBool aCng)
    {
	TPckgBuf<TBool> pckgBuf(aCng);
	TInt status = iCustomCommand.CustomCommandSync(iMessageHandler,
	                                               EIlbcdimSetCng,
	                                               pckgBuf,
	                                               KNullDesC8);
	return status;
    
    }
    
// ---------------------------------------------------------
// CIlbcDecoderIntfcProxy::GetCng
// Sends the custom command for this function to its message handler.
// (other items were commented in a header).
// ---------------------------------------------------------
//
EXPORT_C TInt CIlbcDecoderIntfcProxy::GetCng(TBool& aCng)
    {
	TBool cng = EFalse;
	TPckgBuf<TBool> pckgBuf(cng);
	TInt status = iCustomCommand.CustomCommandSync(iMessageHandler,
	                                               EIlbcdimGetCng,
	                                               KNullDesC8,
	                                               KNullDesC8,
	                                               pckgBuf);
	aCng = pckgBuf();
	return status;
    }
    


// End of File
