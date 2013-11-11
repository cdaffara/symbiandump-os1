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
* Description:   Message handler for G729 decoder interface
*
*/



// INCLUDE FILES
#include    "G729DecoderIntfcMsgHdlr.h"
#include    "G729DecoderIntfcMsgs.h"
#include    <G729DecoderIntfc.h>

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
// CG729DecoderIntfcMsgHdlr::CG729DecoderIntfcMsgHdlr
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CG729DecoderIntfcMsgHdlr::CG729DecoderIntfcMsgHdlr(
                             CG729DecoderIntfc* aG729DecoderIntfcCI) :
	CMMFObject(KUidG729DecoderIntfc)
    {
    iG729DecoderIntfcCI = aG729DecoderIntfcCI;
    }

// -----------------------------------------------------------------------------
// CG729DecoderIntfcMsgHdlr::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CG729DecoderIntfcMsgHdlr::ConstructL()
    {
    }

// -----------------------------------------------------------------------------
// CG729DecoderIntfcMsgHdlr::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
EXPORT_C CG729DecoderIntfcMsgHdlr* CG729DecoderIntfcMsgHdlr::NewL(
                                                TAny*  aG729DecoderIntfcCI)
    {
    CG729DecoderIntfc* errorConcealmentIntfcCI = 
                              (CG729DecoderIntfc*)aG729DecoderIntfcCI;
    CG729DecoderIntfcMsgHdlr* self = 
            new (ELeave) CG729DecoderIntfcMsgHdlr(errorConcealmentIntfcCI);
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );

    return self;
    }

// Destructor
EXPORT_C CG729DecoderIntfcMsgHdlr::~CG729DecoderIntfcMsgHdlr()
	{
	delete iG729DecoderIntfcCI;
	}

// ---------------------------------------------------------
// CG729DecoderIntfcMsgHdlr::HandleRequest
// Handles the messages from the proxy.
// Calls a subfunction which determines which custom interface to call.
// A subfunction is used to contain multiple leaving functions for a single
// trap.
// (other items were commented in a header).
// ---------------------------------------------------------
//
EXPORT_C void CG729DecoderIntfcMsgHdlr::HandleRequest(
                                                       TMMFMessage& aMessage)
	{
	ASSERT(aMessage.Destination().InterfaceId() == KUidG729DecoderIntfc);
	TRAPD(error,DoHandleRequestL(aMessage));
	if(error)
		{
		aMessage.Complete(error);
		}
	}

// ---------------------------------------------------------
// CG729DecoderIntfcMsgHdlr::DoHandleRequestL
// Determines which custom interface to call.
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CG729DecoderIntfcMsgHdlr::DoHandleRequestL(TMMFMessage& aMessage)
	{
	switch(aMessage.Function())
		{
		case EG729dimBadLsfNextBuffer:
			{
    		DoBadLsfNextBufferL(aMessage);
			break;
			}
		default:
			{
			aMessage.Complete(KErrNotSupported);
			}
		}
	}

// ---------------------------------------------------------
// CG729DecoderIntfcMsgHdlr::DoBadLsfNextBufferL
// Handles the message from the proxy and calls the custom interface method.
// The data passed from the proxy is read from the message and passed to
// the custom interface.
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CG729DecoderIntfcMsgHdlr::DoBadLsfNextBufferL(TMMFMessage& aMessage)
    {
	TInt status = iG729DecoderIntfcCI->BadLsfNextBuffer();
    aMessage.Complete(status);
    }
    


// End of File
