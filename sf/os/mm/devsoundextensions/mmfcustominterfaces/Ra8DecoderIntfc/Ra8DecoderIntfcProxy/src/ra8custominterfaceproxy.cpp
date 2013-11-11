/*
* Copyright (c) 2002-2007 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:   Custom Interface proxy for RA8 decoder.
*
*/



// INCLUDE FILES
#include <CustomCommandUtility.h>
#include <CustomInterfaceUtility.h>
#include <sounddevice.h>
#include "Ra8CustomInterfaceProxy.h"
#include "Ra8CustomInterfaceMsgs.h"

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
// CRa8CustomInterfaceProxy::CRa8CustomInterfaceProxy
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CRa8CustomInterfaceProxy::CRa8CustomInterfaceProxy(
                          TMMFMessageDestinationPckg aMessageHandler, 
                          MCustomCommand& aCustomCommand,
                          CCustomInterfaceUtility* aCustomInterfaceUtility) :
	iCustomCommand(aCustomCommand),
	iMessageHandler(aMessageHandler),
	iCustomInterfaceUtility(aCustomInterfaceUtility)
    {
    }


// -----------------------------------------------------------------------------
// CRa8CustomInterfaceProxy::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CRa8CustomInterfaceProxy::ConstructL()
    {
    }

// -----------------------------------------------------------------------------
// CRa8CustomInterfaceProxy::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
EXPORT_C CRa8CustomInterfaceProxy* CRa8CustomInterfaceProxy::NewL(
                            TMMFMessageDestinationPckg aMessageHandler, 
                            MCustomCommand& aCustomCommand,
                            CCustomInterfaceUtility* aCustomInterfaceUtility)
    {
    CRa8CustomInterfaceProxy* self = new(ELeave) CRa8CustomInterfaceProxy(
                                                    aMessageHandler,
                                                    aCustomCommand,
                                                    aCustomInterfaceUtility);
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop(self);
    return self;
    }

// -----------------------------------------------------------------------------
// CRa8CustomInterfaceProxy::NewL
// Two-phased constructor (overloaded version).
// -----------------------------------------------------------------------------
//
EXPORT_C CRa8CustomInterfaceProxy* CRa8CustomInterfaceProxy::NewL(
                                   CMMFDevSound& aDevSound)
    {
    CRa8CustomInterfaceProxy* ra8CustomInterfaceProxy;
    ra8CustomInterfaceProxy = 
    (CRa8CustomInterfaceProxy*)aDevSound.CustomInterface(KUidRa8DecHwDeviceCI);
     
    if (!ra8CustomInterfaceProxy)
        {
        User::Leave(KErrNotFound);
        }
        
    return ra8CustomInterfaceProxy;
    }
    
// -----------------------------------------------------------------------------
// Destructor
// -----------------------------------------------------------------------------
//
EXPORT_C CRa8CustomInterfaceProxy::~CRa8CustomInterfaceProxy()
    {
    iCustomInterfaceUtility->RemoveCustomInterface(iMessageHandler);
    delete iCustomInterfaceUtility;
    }

// -----------------------------------------------------------------------------
// CRa8CustomInterfaceProxy::FrameNumber
// Sends the custom command for this function to its message handler.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C TInt CRa8CustomInterfaceProxy::FrameNumber()
    {
    TInt frame = 0;
	TPckgBuf<TInt> frameBuf(frame);
	TInt status = iCustomCommand.CustomCommandSync(iMessageHandler,
	                                               ERa8CiFrameNumber,
	                                               KNullDesC8,
	                                               KNullDesC8,
	                                               frameBuf);
    if (status == KErrNone)
        {        
        frame = frameBuf();
        }
    else
        {        
        frame = status;
        }
    
	return frame;
    }
    
// -----------------------------------------------------------------------------
// CRa8CustomInterfaceProxy::SetInitString
// Sends the custom command for this function to its message handler.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C TInt CRa8CustomInterfaceProxy::SetInitString(TDesC8& aInitString)
    {
	TInt status = iCustomCommand.CustomCommandSync(iMessageHandler,
	                                               ERa8CiSetInitString,
	                                               aInitString,
	                                               KNullDesC8);
	return status;
    }

// ========================== OTHER EXPORTED FUNCTIONS =========================

// End of File
