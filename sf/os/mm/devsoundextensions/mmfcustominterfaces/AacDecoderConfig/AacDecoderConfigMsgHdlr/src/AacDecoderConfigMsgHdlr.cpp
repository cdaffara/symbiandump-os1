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
* Description:   Message handler for AAC decoder configuration interface
*
*/



// INCLUDE FILES
#include    "AacDecoderConfigMsgHdlr.h"
#include    "AacDecoderConfigMsgs.h"
#include    <AacDecoderConfig.h>

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
// CAacDecoderConfigMsgHdlr::CAacDecoderConfigMsgHdlr
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CAacDecoderConfigMsgHdlr::CAacDecoderConfigMsgHdlr(
                                       CAacDecoderConfig* aAacDecoderConfigCI) :
	CMMFObject(KUidAacDecoderConfig)
    {
    iAacDecoderConfigCI = aAacDecoderConfigCI;
    }

// -----------------------------------------------------------------------------
// CAacDecoderConfigMsgHdlr::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CAacDecoderConfigMsgHdlr::ConstructL()
    {
    }

// -----------------------------------------------------------------------------
// CAacDecoderConfigMsgHdlr::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
EXPORT_C CAacDecoderConfigMsgHdlr* CAacDecoderConfigMsgHdlr::NewL(
                                                     TAny*  aAacDecoderConfigCI)
    {
    CAacDecoderConfig* aacDecoderConfigCI = 
                                        (CAacDecoderConfig*)aAacDecoderConfigCI;
    CAacDecoderConfigMsgHdlr* self = 
                      new (ELeave) CAacDecoderConfigMsgHdlr(aacDecoderConfigCI);
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );

    return self;
    }

// Destructor
EXPORT_C CAacDecoderConfigMsgHdlr::~CAacDecoderConfigMsgHdlr()
	{
	delete iAacDecoderConfigCI;
	}

// ---------------------------------------------------------
// CAacDecoderConfigMsgHdlr::HandleRequest
// Handles the messages from the proxy.
// Calls a subfunction which determines which custom interface to call.
// A subfunction is used to contain multiple leaving functions for a single
// trap.
// (other items were commented in a header).
// ---------------------------------------------------------
//
EXPORT_C void CAacDecoderConfigMsgHdlr::HandleRequest(TMMFMessage& aMessage)
	{
	ASSERT(aMessage.Destination().InterfaceId() == KUidAacDecoderConfig);
	TRAPD(error,DoHandleRequestL(aMessage));
	if(error)
		{
		aMessage.Complete(error);
		}
	}

// ---------------------------------------------------------
// CAacDecoderConfigMsgHdlr::DoHandleRequestL
// Determines which custom interface to call.
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CAacDecoderConfigMsgHdlr::DoHandleRequestL(TMMFMessage& aMessage)
	{
	switch(aMessage.Function())
		{
		case EAdcmSetConfig:
			{
    		DoSetAudioConfigL(aMessage);
			break;
			}
		default:
			{
			aMessage.Complete(KErrNotSupported);
			}
		}
	}

// ---------------------------------------------------------
// CAacDecoderConfigMsgHdlr::DoSetAudioConfigL
// Handles the message from the proxy and calls the custom interface method.
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CAacDecoderConfigMsgHdlr::DoSetAudioConfigL(TMMFMessage& aMessage)
    {
	TPckgBuf<TAudioConfig> pckg;
	aMessage.ReadData1FromClientL(pckg);
	TInt status = iAacDecoderConfigCI->SetAudioConfig(pckg());
    aMessage.Complete(status);
    }


// End of File
