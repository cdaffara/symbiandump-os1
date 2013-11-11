/*
* Copyright (c) 2006 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:   Message handler for eAAC+ decoder configuration interface.
*
*/



// INCLUDE FILES
#include    "EAacPlusDecoderIntfcMsgs.h"
#include    "EAacPlusDecoderIntfcMsgHdlr.h"

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

/**
 * CEAacPlusDecoderIntfcMsgHdlr::CEAacPlusDecoderIntfcMsgHdlr
 * C++ default constructor can NOT contain any code, that might leave.
 */
CEAacPlusDecoderIntfcMsgHdlr::CEAacPlusDecoderIntfcMsgHdlr(
                              CEAacPlusDecoderIntfc* aAacDecoderConfigCI) :
	CMMFObject(KUidEAacPlusDecoderIntfc)
    {
    iAacDecoderConfigCI = aAacDecoderConfigCI;
    }

/**
 * CEAacPlusDecoderIntfcMsgHdlr::ConstructL
 * Symbian 2nd phase constructor can leave.
 */
void CEAacPlusDecoderIntfcMsgHdlr::ConstructL()
    {
    }

/**
 * CEAacPlusDecoderIntfcMsgHdlr::NewL
 * Two-phased constructor.
 */
EXPORT_C CEAacPlusDecoderIntfcMsgHdlr* CEAacPlusDecoderIntfcMsgHdlr::NewL(
                                       TAny* aAacDecoderConfigCI)
    {
    CEAacPlusDecoderIntfc* aacDecoderConfigCI =
                (CEAacPlusDecoderIntfc*)aAacDecoderConfigCI;
    CEAacPlusDecoderIntfcMsgHdlr* self =
                new (ELeave) CEAacPlusDecoderIntfcMsgHdlr(aacDecoderConfigCI);
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );

    return self;
    }

/**
 * Destructor
 */
EXPORT_C CEAacPlusDecoderIntfcMsgHdlr::~CEAacPlusDecoderIntfcMsgHdlr()
	{
	delete iAacDecoderConfigCI;
	}

/**
 * CEAacPlusDecoderIntfcMsgHdlr::HandleRequest
 * Handles the messages from the proxy.
 * Calls a subfunction which determines what custom interface to call.
 * A subfunction is used to contain multiple leaving functions for a single
 * trap.
 * (other items were commented in a header).
 */
EXPORT_C void CEAacPlusDecoderIntfcMsgHdlr::HandleRequest(
                                            TMMFMessage& aMessage)
	{
	ASSERT(aMessage.Destination().InterfaceId() == KUidEAacPlusDecoderIntfc);

	TRAPD(error, DoHandleRequestL(aMessage));
	if(error)
		{
		aMessage.Complete(error);
		}
	}

/**
 * CEAacPlusDecoderIntfcMsgHdlr::DoHandleRequestL
 * Determines which custom interface to call.
 * (other items were commented in a header).
 */
void CEAacPlusDecoderIntfcMsgHdlr::DoHandleRequestL(TMMFMessage& aMessage)
	{
	switch(aMessage.Function())
		{
		case EEaacpdimApplyConfig:
			{
    		DoApplyConfigL(aMessage);
			break;
			}
		default:
			{
			aMessage.Complete(KErrNotSupported);
			}
		}
	}

/**
 * CEAacPlusDecoderIntfcMsgHdlr::DoApplyConfigL
 * Handles the message from the proxy and calls the custom interface method
 * to commit configuration settings to the decoder.
 * (other items were commented in a header).
 */
void CEAacPlusDecoderIntfcMsgHdlr::DoApplyConfigL(TMMFMessage& aMessage)
    {
	TPckgBuf<TEAacPlusDecoderConfig> pckgBuf;
	aMessage.ReadData1FromClientL(pckgBuf);

	iAacDecoderConfigCI->SetAudioObjectType(pckgBuf().iAudioObjectType);
	iAacDecoderConfigCI->SetInputSamplingFrequency(
	                     pckgBuf().iInputSamplingFrequency);
	iAacDecoderConfigCI->SetNumOfChannels(pckgBuf().iNumOfChannels);
	iAacDecoderConfigCI->SetSbr(pckgBuf().iSbrEnabled);
	iAacDecoderConfigCI->SetDownSampledMode(pckgBuf().iDsmEnabled);

	TInt status = iAacDecoderConfigCI->ApplyConfig();
    aMessage.Complete(status);
    }

// End of File
