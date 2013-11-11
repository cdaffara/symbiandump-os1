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
* Description:   Message handler for Ilbc decoder interface
*
*/



// INCLUDE FILES
#include    "IlbcDecoderIntfcMsgHdlr.h"
#include    "IlbcDecoderIntfcMsgs.h"
#include    <IlbcDecoderIntfc.h>

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
// CIlbcDecoderIntfcMsgHdlr::CIlbcDecoderIntfcMsgHdlr
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CIlbcDecoderIntfcMsgHdlr::CIlbcDecoderIntfcMsgHdlr(
                             CIlbcDecoderIntfc* aIlbcDecoderIntfcCI) :
	CMMFObject(KUidIlbcDecoderIntfc)
    {
    iIlbcDecoderIntfcCI = aIlbcDecoderIntfcCI;
    }

// -----------------------------------------------------------------------------
// CIlbcDecoderIntfcMsgHdlr::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CIlbcDecoderIntfcMsgHdlr::ConstructL()
    {
    }

// -----------------------------------------------------------------------------
// CIlbcDecoderIntfcMsgHdlr::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
EXPORT_C CIlbcDecoderIntfcMsgHdlr* CIlbcDecoderIntfcMsgHdlr::NewL(
                                                TAny*  aIlbcDecoderIntfcCI)
    {
    CIlbcDecoderIntfc* errorConcealmentIntfcCI = 
                              (CIlbcDecoderIntfc*)aIlbcDecoderIntfcCI;
    CIlbcDecoderIntfcMsgHdlr* self = 
            new (ELeave) CIlbcDecoderIntfcMsgHdlr(errorConcealmentIntfcCI);
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );

    return self;
    }

// Destructor
EXPORT_C CIlbcDecoderIntfcMsgHdlr::~CIlbcDecoderIntfcMsgHdlr()
	{
	delete iIlbcDecoderIntfcCI;
	}

// ---------------------------------------------------------
// CIlbcDecoderIntfcMsgHdlr::HandleRequest
// Handles the messages from the proxy.
// Calls a subfunction which determines which custom interface to call.
// A subfunction is used to contain multiple leaving functions for a single
// trap.
// (other items were commented in a header).
// ---------------------------------------------------------
//
EXPORT_C void CIlbcDecoderIntfcMsgHdlr::HandleRequest(
                                                       TMMFMessage& aMessage)
	{
	ASSERT(aMessage.Destination().InterfaceId() == KUidIlbcDecoderIntfc);
	TRAPD(error,DoHandleRequestL(aMessage));
	if(error)
		{
		aMessage.Complete(error);
		}
	}

// ---------------------------------------------------------
// CIlbcDecoderIntfcMsgHdlr::DoHandleRequestL
// Determines which custom interface to call.
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CIlbcDecoderIntfcMsgHdlr::DoHandleRequestL(TMMFMessage& aMessage)
	{
	switch(aMessage.Function())
		{
		case EIlbcdimSetDecoderMode:
			{
    		DoSetDecoderModeL(aMessage);
			break;
			}
		case EIlbcdimSetCng:
			{
    		DoSetCngL(aMessage);
			break;
			}
		case EIlbcdimGetCng:
			{
    		DoGetCngL(aMessage);
			break;
			}
		default:
			{
			aMessage.Complete(KErrNotSupported);
			}
		}
	}

// ---------------------------------------------------------
// CIlbcDecoderIntfcMsgHdlr::DoSetDecoderModeL
// Handles the message from the proxy and calls the custom interface method.
// The data passed from the proxy is read from the message and passed to
// the custom interface.
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CIlbcDecoderIntfcMsgHdlr::DoSetDecoderModeL(TMMFMessage& aMessage)
    {
	TPckgBuf<CIlbcDecoderIntfc::TDecodeMode> pckgBuf;
	aMessage.ReadData1FromClientL(pckgBuf);
	TInt status = iIlbcDecoderIntfcCI->SetDecoderMode(pckgBuf());
    aMessage.Complete(status);
    }

// ---------------------------------------------------------
// CIlbcDecoderIntfcMsgHdlr::DoSetCngL
// Handles the message from the proxy and calls the custom interface method.
// The data passed from the proxy is read from the message and passed to
// the custom interface.
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CIlbcDecoderIntfcMsgHdlr::DoSetCngL(TMMFMessage& aMessage)
    {
	TPckgBuf<TBool> pckgBuf;
	aMessage.ReadData1FromClientL(pckgBuf);
	TInt status = iIlbcDecoderIntfcCI->SetCng(pckgBuf());
    aMessage.Complete(status);
    }
    
// ---------------------------------------------------------
// CIlbcDecoderIntfcMsgHdlr::DoGetCngL
// Handles the message from the proxy and calls the custom interface method.
// The custom interface returns the data requested and this function
// writes it back to the proxy.
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CIlbcDecoderIntfcMsgHdlr::DoGetCngL(TMMFMessage& aMessage)
    {
	TBool cng;
	TInt status = iIlbcDecoderIntfcCI->GetCng(cng);
	if (status == KErrNone)
    	{
    	TPckgBuf<TBool> pckgBuf;
	    pckgBuf() = cng;
    	aMessage.WriteDataToClientL(pckgBuf);
    	}
    aMessage.Complete(status);
    }
    
    


// End of File
