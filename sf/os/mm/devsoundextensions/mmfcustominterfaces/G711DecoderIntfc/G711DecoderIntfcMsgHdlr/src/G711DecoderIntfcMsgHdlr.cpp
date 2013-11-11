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
* Description:   Message handler for G711 decoder interface
*
*/



// INCLUDE FILES
#include    "G711DecoderIntfcMsgHdlr.h"
#include    "G711DecoderIntfcMsgs.h"
#include    <G711DecoderIntfc.h>

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
// CG711DecoderIntfcMsgHdlr::CG711DecoderIntfcMsgHdlr
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CG711DecoderIntfcMsgHdlr::CG711DecoderIntfcMsgHdlr(
                             CG711DecoderIntfc* aG711DecoderIntfcCI) :
	CMMFObject(KUidG711DecoderIntfc)
    {
    iG711DecoderIntfcCI = aG711DecoderIntfcCI;
    }

// -----------------------------------------------------------------------------
// CG711DecoderIntfcMsgHdlr::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CG711DecoderIntfcMsgHdlr::ConstructL()
    {
    }

// -----------------------------------------------------------------------------
// CG711DecoderIntfcMsgHdlr::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
EXPORT_C CG711DecoderIntfcMsgHdlr* CG711DecoderIntfcMsgHdlr::NewL(
                                                TAny*  aG711DecoderIntfcCI)
    {
    CG711DecoderIntfc* errorConcealmentIntfcCI = 
                              (CG711DecoderIntfc*)aG711DecoderIntfcCI;
    CG711DecoderIntfcMsgHdlr* self = 
            new (ELeave) CG711DecoderIntfcMsgHdlr(errorConcealmentIntfcCI);
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );

    return self;
    }

// Destructor
EXPORT_C CG711DecoderIntfcMsgHdlr::~CG711DecoderIntfcMsgHdlr()
	{
	delete iG711DecoderIntfcCI;
	}

// ---------------------------------------------------------
// CG711DecoderIntfcMsgHdlr::HandleRequest
// Handles the messages from the proxy.
// Calls a subfunction which determines which custom interface to call.
// A subfunction is used to contain multiple leaving functions for a single
// trap.
// (other items were commented in a header).
// ---------------------------------------------------------
//
EXPORT_C void CG711DecoderIntfcMsgHdlr::HandleRequest(
                                                       TMMFMessage& aMessage)
	{
	ASSERT(aMessage.Destination().InterfaceId() == KUidG711DecoderIntfc);
	TRAPD(error,DoHandleRequestL(aMessage));
	if(error)
		{
		aMessage.Complete(error);
		}
	}

// ---------------------------------------------------------
// CG711DecoderIntfcMsgHdlr::DoHandleRequestL
// Determines which custom interface to call.
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CG711DecoderIntfcMsgHdlr::DoHandleRequestL(TMMFMessage& aMessage)
	{
	switch(aMessage.Function())
		{
		case EG711dimSetDecoderMode:
			{
    		DoSetDecoderModeL(aMessage);
			break;
			}
		case EG711dimSetCng:
			{
    		DoSetCngL(aMessage);
			break;
			}
		case EG711dimGetCng:
			{
    		DoGetCngL(aMessage);
			break;
			}
		case EG711dimSetPlc:
			{
    		DoSetPlcL(aMessage);
			break;
			}
		default:
			{
			aMessage.Complete(KErrNotSupported);
			}
		}
	}

// ---------------------------------------------------------
// CG711DecoderIntfcMsgHdlr::DoSetDecoderModeL
// Handles the message from the proxy and calls the custom interface method.
// The data passed from the proxy is read from the message and passed to
// the custom interface.
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CG711DecoderIntfcMsgHdlr::DoSetDecoderModeL(TMMFMessage& aMessage)
    {
	TPckgBuf<CG711DecoderIntfc::TDecodeMode> pckgBuf;
	aMessage.ReadData1FromClientL(pckgBuf);
	TInt status = iG711DecoderIntfcCI->SetDecoderMode(pckgBuf());
    aMessage.Complete(status);
    }
    
// ---------------------------------------------------------
// CG711DecoderIntfcMsgHdlr::DoSetCngL
// Handles the message from the proxy and calls the custom interface method.
// The data passed from the proxy is read from the message and passed to
// the custom interface.
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CG711DecoderIntfcMsgHdlr::DoSetCngL(TMMFMessage& aMessage)
    {
	TPckgBuf<TBool> pckgBuf;
	aMessage.ReadData1FromClientL(pckgBuf);
	TInt status = iG711DecoderIntfcCI->SetCng(pckgBuf());
    aMessage.Complete(status);
    }
    
// ---------------------------------------------------------
// CG711DecoderIntfcMsgHdlr::DoGetCngL
// Handles the message from the proxy and calls the custom interface method.
// The custom interface returns the data requested and this function
// writes it back to the proxy.
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CG711DecoderIntfcMsgHdlr::DoGetCngL(TMMFMessage& aMessage)
    {
	TBool cng;
	TInt status = iG711DecoderIntfcCI->GetCng(cng);
	if (status == KErrNone)
    	{
    	TPckgBuf<TBool> pckgBuf;
	    pckgBuf() = cng;
    	aMessage.WriteDataToClientL(pckgBuf);
    	}
    aMessage.Complete(status);
    }
    
// ---------------------------------------------------------
// CG711DecoderIntfcMsgHdlr::DoSetPlcL
// Handles the message from the proxy and calls the custom interface method.
// The data passed from the proxy is read from the message and passed to
// the custom interface.
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CG711DecoderIntfcMsgHdlr::DoSetPlcL(TMMFMessage& aMessage)
    {
	TPckgBuf<TBool> pckgBuf;
	aMessage.ReadData1FromClientL(pckgBuf);
	TInt status = iG711DecoderIntfcCI->SetPlc(pckgBuf());
    aMessage.Complete(status);
    }
    


// End of File
