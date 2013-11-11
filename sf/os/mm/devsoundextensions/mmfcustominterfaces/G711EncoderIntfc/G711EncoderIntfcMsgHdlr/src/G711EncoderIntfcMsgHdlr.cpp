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
* Description:   Message handler for G711 encoder interface
*
*/



// INCLUDE FILES
#include    "G711EncoderIntfcMsgHdlr.h"
#include    "G711EncoderIntfcMsgs.h"
#include    <G711EncoderIntfc.h>

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
// CG711EncoderIntfcMsgHdlr::CG711EncoderIntfcMsgHdlr
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CG711EncoderIntfcMsgHdlr::CG711EncoderIntfcMsgHdlr(
                             CG711EncoderIntfc* aG711EncoderIntfcCI) :
	CMMFObject(KUidG711EncoderIntfc)
    {
    iG711EncoderIntfcCI = aG711EncoderIntfcCI;
    }

// -----------------------------------------------------------------------------
// CG711EncoderIntfcMsgHdlr::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CG711EncoderIntfcMsgHdlr::ConstructL()
    {
    }

// -----------------------------------------------------------------------------
// CG711EncoderIntfcMsgHdlr::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
EXPORT_C CG711EncoderIntfcMsgHdlr* CG711EncoderIntfcMsgHdlr::NewL(
                                                TAny*  aG711EncoderIntfcCI)
    {
    CG711EncoderIntfc* errorConcealmentIntfcCI = 
                              (CG711EncoderIntfc*)aG711EncoderIntfcCI;
    CG711EncoderIntfcMsgHdlr* self = 
            new (ELeave) CG711EncoderIntfcMsgHdlr(errorConcealmentIntfcCI);
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );

    return self;
    }

// Destructor
EXPORT_C CG711EncoderIntfcMsgHdlr::~CG711EncoderIntfcMsgHdlr()
	{
	delete iG711EncoderIntfcCI;
	}

// ---------------------------------------------------------
// CG711EncoderIntfcMsgHdlr::HandleRequest
// Handles the messages from the proxy.
// Calls a subfunction which determines which custom interface to call.
// A subfunction is used to contain multiple leaving functions for a single
// trap.
// (other items were commented in a header).
// ---------------------------------------------------------
//
EXPORT_C void CG711EncoderIntfcMsgHdlr::HandleRequest(
                                                       TMMFMessage& aMessage)
	{
	ASSERT(aMessage.Destination().InterfaceId() == KUidG711EncoderIntfc);
	TRAPD(error,DoHandleRequestL(aMessage));
	if(error)
		{
		aMessage.Complete(error);
		}
	}

// ---------------------------------------------------------
// CG711EncoderIntfcMsgHdlr::DoHandleRequestL
// Determines which custom interface to call.
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CG711EncoderIntfcMsgHdlr::DoHandleRequestL(TMMFMessage& aMessage)
	{
	switch(aMessage.Function())
		{
		case EG711eimSetEncoderMode:
			{
    		DoSetEncoderModeL(aMessage);
			break;
			}
		case EG711eimSetVadMode:
			{
			DoSetVadModeL(aMessage);
			break;
			}
		case EG711eimGetVadMode:
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
// CG711EncoderIntfcMsgHdlr::DoSetEncoderModeL
// Handles the message from the proxy and calls the custom interface method.
// The data passed from the proxy is read from the message and passed to
// the custom interface.
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CG711EncoderIntfcMsgHdlr::DoSetEncoderModeL(TMMFMessage& aMessage)
    {
	TPckgBuf<CG711EncoderIntfc::TEncodeMode> pckgBuf;
	aMessage.ReadData1FromClientL(pckgBuf);
	TInt status = iG711EncoderIntfcCI->SetEncoderMode(pckgBuf());
    aMessage.Complete(status);
    }
    
// ---------------------------------------------------------
// CG711EncoderIntfcMsgHdlr::DoSetVadModeL
// Handles the message from the proxy and calls the custom interface method.
// The data passed from the proxy is read from the message and passed to
// the custom interface.
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CG711EncoderIntfcMsgHdlr::DoSetVadModeL(TMMFMessage& aMessage)
	{
	TPckgBuf<TBool> pckgBuf;
	aMessage.ReadData1FromClientL(pckgBuf);
	TInt status = iG711EncoderIntfcCI->SetVadMode(pckgBuf());
    aMessage.Complete(status);
	}

// ---------------------------------------------------------
// CG711EncoderIntfcMsgHdlr::DoGetVadModeL
// Handles the message from the proxy and calls the custom interface.
// The custom interface returns the data requested and this function
// writes it back to the proxy.
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CG711EncoderIntfcMsgHdlr::DoGetVadModeL(TMMFMessage& aMessage)
	{
	TBool vadMode;
	TInt status = iG711EncoderIntfcCI->GetVadMode(vadMode);
	if (status == KErrNone)
    	{
    	TPckgBuf<TBool> pckgBuf;
	    pckgBuf() = vadMode;
    	aMessage.WriteDataToClientL(pckgBuf);
    	}
    aMessage.Complete(status);
	}

    


// End of File
