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
* Description:   Interface proxy for G711 encoder.
*
*/



// INCLUDE FILES
#include "G711EncoderIntfcProxy.h"
#include "G711EncoderIntfcMsgs.h"
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
// CG711EncoderIntfcProxy::CG711EncoderIntfcProxy
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CG711EncoderIntfcProxy::CG711EncoderIntfcProxy(
                             TMMFMessageDestinationPckg aMessageHandler, 
                             MCustomCommand& aCustomCommand,
                             CCustomInterfaceUtility* aCustomInterfaceUtility) :
	iCustomCommand(aCustomCommand),
	iMessageHandler(aMessageHandler),
	iCustomInterfaceUtility(aCustomInterfaceUtility)
    {
    }


// -----------------------------------------------------------------------------
// CG711EncoderIntfcProxy::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CG711EncoderIntfcProxy::ConstructL()
    {
    }

// -----------------------------------------------------------------------------
// CG711EncoderIntfcProxy::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
EXPORT_C CG711EncoderIntfcProxy* CG711EncoderIntfcProxy::NewL(
                              TMMFMessageDestinationPckg aMessageHandler, 
                              MCustomCommand& aCustomCommand,
                              CCustomInterfaceUtility* aCustomInterfaceUtility)
    {
    CG711EncoderIntfcProxy* self = new(ELeave) CG711EncoderIntfcProxy(
                                                        aMessageHandler,
                                                        aCustomCommand,
                                                        aCustomInterfaceUtility);
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }
    
// Destructor
EXPORT_C CG711EncoderIntfcProxy::~CG711EncoderIntfcProxy()
    {
    iCustomInterfaceUtility->RemoveCustomInterface(iMessageHandler);
    delete iCustomInterfaceUtility;
    }

// ---------------------------------------------------------
// CG711EncoderIntfcProxy::SetEncoderMode
// Sends the custom command for this function to its message handler.
// (other items were commented in a header).
// ---------------------------------------------------------
//
EXPORT_C TInt CG711EncoderIntfcProxy::SetEncoderMode(TEncodeMode aEncodeMode)
    {
	TPckgBuf<TEncodeMode> pckgBuf(aEncodeMode);
	TInt status = iCustomCommand.CustomCommandSync(iMessageHandler,
	                                               EG711eimSetEncoderMode,
	                                               pckgBuf,
	                                               KNullDesC8);
	return status;
    
    }
    
// ---------------------------------------------------------
// CG711EncoderIntfcProxy::SetVadMode
// Sends the custom command for this function to its message handler.
// (other items were commented in a header).
// ---------------------------------------------------------
//
EXPORT_C TInt CG711EncoderIntfcProxy::SetVadMode(TBool aVadMode)
	{
	TPckgBuf<TBool> pckgBuf(aVadMode);
	TInt status = iCustomCommand.CustomCommandSync(iMessageHandler,
	                                               EG711eimSetVadMode,
	                                               pckgBuf,
	                                               KNullDesC8);
	return status;
	}

// ---------------------------------------------------------
// CG711EncoderIntfcProxy::GetVadMode
// Sends the custom command for this function to its message handler.
// (other items were commented in a header).
// ---------------------------------------------------------
//
EXPORT_C TInt CG711EncoderIntfcProxy::GetVadMode(TBool& aVadMode)
	{
	TInt vadMode = EFalse;
	TPckgBuf<TBool> pckgBuf(vadMode);
	TInt status = iCustomCommand.CustomCommandSync(iMessageHandler,
	                                               EG711eimGetVadMode,
	                                               KNullDesC8,
	                                               KNullDesC8,
	                                               pckgBuf);
	if (status == KErrNone)
	    aVadMode = pckgBuf();
	return status;
	}

    
    


// End of File
