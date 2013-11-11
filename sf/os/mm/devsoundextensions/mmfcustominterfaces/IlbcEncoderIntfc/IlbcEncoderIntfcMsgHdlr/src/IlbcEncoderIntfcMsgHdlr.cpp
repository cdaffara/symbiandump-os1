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
* Description:   Message handler for Ilbc encoder interface
*
*/



// INCLUDE FILES
#include    "IlbcEncoderIntfcMsgHdlr.h"
#include    "IlbcEncoderIntfcMsgs.h"
#include    <IlbcEncoderIntfc.h>

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
// CIlbcEncoderIntfcMsgHdlr::CIlbcEncoderIntfcMsgHdlr
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CIlbcEncoderIntfcMsgHdlr::CIlbcEncoderIntfcMsgHdlr(
                             CIlbcEncoderIntfc* aIlbcEncoderIntfcCI) :
	CMMFObject(KUidIlbcEncoderIntfc)
    {
    iIlbcEncoderIntfcCI = aIlbcEncoderIntfcCI;
    }

// -----------------------------------------------------------------------------
// CIlbcEncoderIntfcMsgHdlr::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CIlbcEncoderIntfcMsgHdlr::ConstructL()
    {
    }

// -----------------------------------------------------------------------------
// CIlbcEncoderIntfcMsgHdlr::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
EXPORT_C CIlbcEncoderIntfcMsgHdlr* CIlbcEncoderIntfcMsgHdlr::NewL(
                                                TAny*  aIlbcEncoderIntfcCI)
    {
    CIlbcEncoderIntfc* errorConcealmentIntfcCI = 
                              (CIlbcEncoderIntfc*)aIlbcEncoderIntfcCI;
    CIlbcEncoderIntfcMsgHdlr* self = 
            new (ELeave) CIlbcEncoderIntfcMsgHdlr(errorConcealmentIntfcCI);
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );

    return self;
    }

// Destructor
EXPORT_C CIlbcEncoderIntfcMsgHdlr::~CIlbcEncoderIntfcMsgHdlr()
	{
	delete iIlbcEncoderIntfcCI;
	}

// ---------------------------------------------------------
// CIlbcEncoderIntfcMsgHdlr::HandleRequest
// Handles the messages from the proxy.
// Calls a subfunction which determines which custom interface to call.
// A subfunction is used to contain multiple leaving functions for a single
// trap.
// (other items were commented in a header).
// ---------------------------------------------------------
//
EXPORT_C void CIlbcEncoderIntfcMsgHdlr::HandleRequest(
                                                       TMMFMessage& aMessage)
	{
	ASSERT(aMessage.Destination().InterfaceId() == KUidIlbcEncoderIntfc);
	TRAPD(error,DoHandleRequestL(aMessage));
	if(error)
		{
		aMessage.Complete(error);
		}
	}

// ---------------------------------------------------------
// CIlbcEncoderIntfcMsgHdlr::DoHandleRequestL
// Determines which custom interface to call.
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CIlbcEncoderIntfcMsgHdlr::DoHandleRequestL(TMMFMessage& aMessage)
	{
	switch(aMessage.Function())
		{
		case EIlbceimSetEncoderMode:
			{
    		DoSetEncoderModeL(aMessage);
			break;
			}
		case EIlbceimSetVadMode:
			{
			DoSetVadModeL(aMessage);
			break;
			}
		case EIlbceimGetVadMode:
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
// CIlbcEncoderIntfcMsgHdlr::DoSetEncoderModeL
// Handles the message from the proxy and calls the custom interface method.
// The data passed from the proxy is read from the message and passed to
// the custom interface.
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CIlbcEncoderIntfcMsgHdlr::DoSetEncoderModeL(TMMFMessage& aMessage)
    {
	TPckgBuf<CIlbcEncoderIntfc::TEncodeMode> pckgBuf;
	aMessage.ReadData1FromClientL(pckgBuf);
	TInt status = iIlbcEncoderIntfcCI->SetEncoderMode(pckgBuf());
    aMessage.Complete(status);
    }

// ---------------------------------------------------------
// CIlbcEncoderIntfcMsgHdlr::DoSetVadModeL
// Handles the message from the proxy and calls the custom interface method.
// The data passed from the proxy is read from the message and passed to
// the custom interface.
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CIlbcEncoderIntfcMsgHdlr::DoSetVadModeL(TMMFMessage& aMessage)
	{
	TPckgBuf<TBool> pckgBuf;
	aMessage.ReadData1FromClientL(pckgBuf);
	TInt status = iIlbcEncoderIntfcCI->SetVadMode(pckgBuf());
    aMessage.Complete(status);
	}

// ---------------------------------------------------------
// CIlbcEncoderIntfcMsgHdlr::DoGetVadModeL
// Handles the message from the proxy and calls the custom interface.
// The custom interface returns the data requested and this function
// writes it back to the proxy.
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CIlbcEncoderIntfcMsgHdlr::DoGetVadModeL(TMMFMessage& aMessage)
	{
	TBool vadMode;
	TInt status = iIlbcEncoderIntfcCI->GetVadMode(vadMode);
	if (status == KErrNone)
    	{
    	TPckgBuf<TBool> pckgBuf;
	    pckgBuf() = vadMode;
    	aMessage.WriteDataToClientL(pckgBuf);
    	}
    aMessage.Complete(status);
	}

    


// End of File
