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
* Description:   Message handler for G729 encoder interface
*
*/



// INCLUDE FILES
#include    "G729EncoderIntfcMsgHdlr.h"
#include    "G729EncoderIntfcMsgs.h"
#include    <G729EncoderIntfc.h>

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
// CG729EncoderIntfcMsgHdlr::CG729EncoderIntfcMsgHdlr
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CG729EncoderIntfcMsgHdlr::CG729EncoderIntfcMsgHdlr(
                             CG729EncoderIntfc* aG729EncoderIntfcCI) :
	CMMFObject(KUidG729EncoderIntfc)
    {
    iG729EncoderIntfcCI = aG729EncoderIntfcCI;
    }

// -----------------------------------------------------------------------------
// CG729EncoderIntfcMsgHdlr::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CG729EncoderIntfcMsgHdlr::ConstructL()
    {
    }

// -----------------------------------------------------------------------------
// CG729EncoderIntfcMsgHdlr::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
EXPORT_C CG729EncoderIntfcMsgHdlr* CG729EncoderIntfcMsgHdlr::NewL(
                                                TAny*  aG729EncoderIntfcCI)
    {
    CG729EncoderIntfc* errorConcealmentIntfcCI = 
                              (CG729EncoderIntfc*)aG729EncoderIntfcCI;
    CG729EncoderIntfcMsgHdlr* self = 
            new (ELeave) CG729EncoderIntfcMsgHdlr(errorConcealmentIntfcCI);
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );

    return self;
    }

// Destructor
EXPORT_C CG729EncoderIntfcMsgHdlr::~CG729EncoderIntfcMsgHdlr()
	{
	delete iG729EncoderIntfcCI;
	}

// ---------------------------------------------------------
// CG729EncoderIntfcMsgHdlr::HandleRequest
// Handles the messages from the proxy.
// Calls a subfunction which determines which custom interface to call.
// A subfunction is used to contain multiple leaving functions for a single
// trap.
// (other items were commented in a header).
// ---------------------------------------------------------
//
EXPORT_C void CG729EncoderIntfcMsgHdlr::HandleRequest(
                                                       TMMFMessage& aMessage)
	{
	ASSERT(aMessage.Destination().InterfaceId() == KUidG729EncoderIntfc);
	TRAPD(error,DoHandleRequestL(aMessage));
	if(error)
		{
		aMessage.Complete(error);
		}
	}

// ---------------------------------------------------------
// CG729EncoderIntfcMsgHdlr::DoHandleRequestL
// Determines which custom interface to call.
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CG729EncoderIntfcMsgHdlr::DoHandleRequestL(TMMFMessage& aMessage)
	{
	switch(aMessage.Function())
		{
		case EG729eimSetVadMode:
			{
			DoSetVadModeL(aMessage);
			break;
			}
		case EG729eimGetVadMode:
			{
			DoGetVadModeL(aMessage);
			break;
			}
		default:
			{
			aMessage.Complete(KErrNotSupported);
			}
		}
	}

// ---------------------------------------------------------
// CG729EncoderIntfcMsgHdlr::DoSetVadModeL
// Handles the message from the proxy and calls the custom interface method.
// The data passed from the proxy is read from the message and passed to
// the custom interface.
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CG729EncoderIntfcMsgHdlr::DoSetVadModeL(TMMFMessage& aMessage)
	{
	TPckgBuf<TBool> pckgBuf;
	aMessage.ReadData1FromClientL(pckgBuf);
	TInt status = iG729EncoderIntfcCI->SetVadMode(pckgBuf());
    aMessage.Complete(status);
	}

// ---------------------------------------------------------
// CG729EncoderIntfcMsgHdlr::DoGetVadModeL
// Handles the message from the proxy and calls the custom interface.
// The custom interface returns the data requested and this function
// writes it back to the proxy.
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CG729EncoderIntfcMsgHdlr::DoGetVadModeL(TMMFMessage& aMessage)
	{
	TBool vadMode;
	TInt status = iG729EncoderIntfcCI->GetVadMode(vadMode);
	if (status == KErrNone)
    	{
    	TPckgBuf<TBool> pckgBuf;
	    pckgBuf() = vadMode;
    	aMessage.WriteDataToClientL(pckgBuf);
    	}
    aMessage.Complete(status);
	}

    


// End of File
