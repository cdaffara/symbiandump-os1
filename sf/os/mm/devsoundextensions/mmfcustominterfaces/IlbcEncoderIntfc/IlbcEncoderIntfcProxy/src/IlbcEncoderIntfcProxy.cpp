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
* Description:   Interface proxy for Ilbc encoder.
*
*/



// INCLUDE FILES
#include "IlbcEncoderIntfcProxy.h"
#include "IlbcEncoderIntfcMsgs.h"
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
// CIlbcEncoderIntfcProxy::CIlbcEncoderIntfcProxy
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CIlbcEncoderIntfcProxy::CIlbcEncoderIntfcProxy(
                             TMMFMessageDestinationPckg aMessageHandler, 
                             MCustomCommand& aCustomCommand,
                             CCustomInterfaceUtility* aCustomInterfaceUtility) :
	iCustomCommand(aCustomCommand),
	iMessageHandler(aMessageHandler),
	iCustomInterfaceUtility(aCustomInterfaceUtility)
    {
    }


// -----------------------------------------------------------------------------
// CIlbcEncoderIntfcProxy::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CIlbcEncoderIntfcProxy::ConstructL()
    {
    }

// -----------------------------------------------------------------------------
// CIlbcEncoderIntfcProxy::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
EXPORT_C CIlbcEncoderIntfcProxy* CIlbcEncoderIntfcProxy::NewL(
                              TMMFMessageDestinationPckg aMessageHandler, 
                              MCustomCommand& aCustomCommand,
                              CCustomInterfaceUtility* aCustomInterfaceUtility)
    {
    CIlbcEncoderIntfcProxy* self = new(ELeave) CIlbcEncoderIntfcProxy(
                                                        aMessageHandler,
                                                        aCustomCommand,
                                                        aCustomInterfaceUtility);
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }
    
// Destructor
EXPORT_C CIlbcEncoderIntfcProxy::~CIlbcEncoderIntfcProxy()
    {
    iCustomInterfaceUtility->RemoveCustomInterface(iMessageHandler);
    delete iCustomInterfaceUtility;
    }

// ---------------------------------------------------------
// CIlbcEncoderIntfcProxy::SetEncoderMode
// Sends the custom command for this function to its message handler.
// (other items were commented in a header).
// ---------------------------------------------------------
//
EXPORT_C TInt CIlbcEncoderIntfcProxy::SetEncoderMode(TEncodeMode aEncodeMode)
    {
	TPckgBuf<TEncodeMode> pckgBuf(aEncodeMode);
	TInt status = iCustomCommand.CustomCommandSync(iMessageHandler,
	                                               EIlbceimSetEncoderMode,
	                                               pckgBuf,
	                                               KNullDesC8);
	return status;
    
    }

// ---------------------------------------------------------
// CIlbcEncoderIntfcProxy::SetVadMode
// Sends the custom command for this function to its message handler.
// (other items were commented in a header).
// ---------------------------------------------------------
//
EXPORT_C TInt CIlbcEncoderIntfcProxy::SetVadMode(TBool aVadMode)
	{
	TPckgBuf<TBool> pckgBuf(aVadMode);
	TInt status = iCustomCommand.CustomCommandSync(iMessageHandler,
	                                               EIlbceimSetVadMode,
	                                               pckgBuf,
	                                               KNullDesC8);
	return status;
	}

// ---------------------------------------------------------
// CIlbcEncoderIntfcProxy::GetVadMode
// Sends the custom command for this function to its message handler.
// (other items were commented in a header).
// ---------------------------------------------------------
//
EXPORT_C TInt CIlbcEncoderIntfcProxy::GetVadMode(TBool& aVadMode)
	{
	TInt vadMode = EFalse;
	TPckgBuf<TBool> pckgBuf(vadMode);
	TInt status = iCustomCommand.CustomCommandSync(iMessageHandler,
	                                               EIlbceimGetVadMode,
	                                               KNullDesC8,
	                                               KNullDesC8,
	                                               pckgBuf);
	if (status == KErrNone)
	    aVadMode = pckgBuf();
	return status;
	}
    


// End of File
