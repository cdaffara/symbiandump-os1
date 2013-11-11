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
* Description:   Message handler for error concealment interface
*
*/



// INCLUDE FILES
#include    "ErrorConcealmentIntfcMsgHdlr.h"
#include    "ErrorConcealmentIntfcMsgs.h"
#include    <ErrorConcealmentIntfc.h>

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
// CErrorConcealmentIntfcMsgHdlr::CErrorConcealmentIntfcMsgHdlr
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CErrorConcealmentIntfcMsgHdlr::CErrorConcealmentIntfcMsgHdlr(
                             CErrorConcealmentIntfc* aErrorConcealmentIntfcCI) :
	CMMFObject(KUidErrorConcealmentIntfc)
    {
    iErrorConcealmentIntfcCI = aErrorConcealmentIntfcCI;
    }

// -----------------------------------------------------------------------------
// CErrorConcealmentIntfcMsgHdlr::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CErrorConcealmentIntfcMsgHdlr::ConstructL()
    {
    }

// -----------------------------------------------------------------------------
// CErrorConcealmentIntfcMsgHdlr::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
EXPORT_C CErrorConcealmentIntfcMsgHdlr* CErrorConcealmentIntfcMsgHdlr::NewL(
                                                TAny*  aErrorConcealmentIntfcCI)
    {
    CErrorConcealmentIntfc* errorConcealmentIntfcCI = 
                              (CErrorConcealmentIntfc*)aErrorConcealmentIntfcCI;
    CErrorConcealmentIntfcMsgHdlr* self = 
            new (ELeave) CErrorConcealmentIntfcMsgHdlr(errorConcealmentIntfcCI);
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );

    return self;
    }

// Destructor
EXPORT_C CErrorConcealmentIntfcMsgHdlr::~CErrorConcealmentIntfcMsgHdlr()
	{
	delete iErrorConcealmentIntfcCI;
	}

// ---------------------------------------------------------
// CErrorConcealmentIntfcMsgHdlr::HandleRequest
// Handles the messages from the proxy.
// Calls a subfunction which determines which custom interface to call.
// A subfunction is used to contain multiple leaving functions for a single
// trap.
// (other items were commented in a header).
// ---------------------------------------------------------
//
EXPORT_C void CErrorConcealmentIntfcMsgHdlr::HandleRequest(
                                                       TMMFMessage& aMessage)
	{
	ASSERT(aMessage.Destination().InterfaceId() == KUidErrorConcealmentIntfc);
	TRAPD(error,DoHandleRequestL(aMessage));
	if(error)
		{
		aMessage.Complete(error);
		}
	}

// ---------------------------------------------------------
// CErrorConcealmentIntfcMsgHdlr::DoHandleRequestL
// Determines which custom interface to call.
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CErrorConcealmentIntfcMsgHdlr::DoHandleRequestL(TMMFMessage& aMessage)
	{
	switch(aMessage.Function())
		{
		case EEcimConcealError:
			{
    		DoConcealErrorForNextBufferL(aMessage);
			break;
			}
		case EEcimSetFrameMode:
			{
    		DoSetFrameModeL(aMessage);
			break;
			}
		case EEcimFrameModeRqrd:
			{
    		DoFrameModeRqrdForEcL(aMessage);
			break;
			}
		default:
			{
			aMessage.Complete(KErrNotSupported);
			}
		}
	}

// ---------------------------------------------------------
// CErrorConcealmentIntfcMsgHdlr::DoConcealErrorForNextBufferL
// Handles the message from the proxy and calls the custom interface method.
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CErrorConcealmentIntfcMsgHdlr::DoConcealErrorForNextBufferL(
                                                         TMMFMessage& aMessage)
    {
	TInt status = iErrorConcealmentIntfcCI->ConcealErrorForNextBuffer();
    aMessage.Complete(status);
    }
    
// ---------------------------------------------------------
// CErrorConcealmentIntfcMsgHdlr::DoSetFrameModeL
// Handles the message from the proxy and calls the custom interface method.
// The data passed from the proxy is read from the message and passed to
// the custom interface.
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CErrorConcealmentIntfcMsgHdlr::DoSetFrameModeL(TMMFMessage& aMessage)
    {
	TPckgBuf<TBool> pckg;
	aMessage.ReadData1FromClientL(pckg);
	TInt status = iErrorConcealmentIntfcCI->SetFrameMode(pckg());
    aMessage.Complete(status);
    }
    
// ---------------------------------------------------------
// CErrorConcealmentIntfcMsgHdlr::DoFrameModeRqrdForEcL
// Handles the message from the proxy and calls the custom interface method.
// The custom interface returns the data requested and this function
// writes it back to the proxy.
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CErrorConcealmentIntfcMsgHdlr::DoFrameModeRqrdForEcL(TMMFMessage& aMessage)
    {
	TBool frameModeRqrd;
	TInt status = iErrorConcealmentIntfcCI->FrameModeRqrdForEC(frameModeRqrd);
	if (status == KErrNone)
    	{
    	TPckgBuf<TBool> pckg;
	    pckg() = frameModeRqrd;
    	aMessage.WriteDataToClientL(pckg);
    	}
    aMessage.Complete(status);
    }

// End of File
