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
* Description:   Message handler for speech encoder configuration interface
*
*/



// INCLUDE FILES
#include    "SpeechEncoderConfigMsgHdlr.h"
#include    "SpeechEncoderConfigMsgs.h"
#include    <SpeechEncoderConfig.h>

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
// CSpeechEncoderConfigMsgHdlr::CSpeechEncoderConfigMsgHdlr
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CSpeechEncoderConfigMsgHdlr::CSpeechEncoderConfigMsgHdlr(
                                 CSpeechEncoderConfig* aSpeechEncoderConfigCI) :
	CMMFObject(KUidSpeechEncoderConfig)
    {
    iSpeechEncoderConfigCI = aSpeechEncoderConfigCI;
    }

// -----------------------------------------------------------------------------
// CSpeechEncoderConfigMsgHdlr::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CSpeechEncoderConfigMsgHdlr::ConstructL()
    {
    }

// -----------------------------------------------------------------------------
// CSpeechEncoderConfigMsgHdlr::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
EXPORT_C CSpeechEncoderConfigMsgHdlr* CSpeechEncoderConfigMsgHdlr::NewL(
                                                  TAny*  aSpeechEncoderConfigCI)
    {
    CSpeechEncoderConfig* speechEncoderConfigCI = 
                                  (CSpeechEncoderConfig*)aSpeechEncoderConfigCI;
    CSpeechEncoderConfigMsgHdlr* self = 
                new (ELeave) CSpeechEncoderConfigMsgHdlr(speechEncoderConfigCI);
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }

// Destructor
EXPORT_C CSpeechEncoderConfigMsgHdlr::~CSpeechEncoderConfigMsgHdlr()
	{
	iSupportedBitrates.Close();
	delete iDataCopyBuffer;
	delete iSpeechEncoderConfigCI;
	}

// ---------------------------------------------------------
// CSpeechEncoderConfigMsgHdlr::HandleRequest
// Handles the messages from the proxy.
// Calls a subfunction which determines which custom interface to call.
// A subfunction is used to contain multiple leaving functions for a single
// trap.
// (other items were commented in a header).
// ---------------------------------------------------------
//
EXPORT_C void CSpeechEncoderConfigMsgHdlr::HandleRequest(TMMFMessage& aMessage)
	{
    // debug panic
	ASSERT(aMessage.Destination().InterfaceId() == KUidSpeechEncoderConfig);
	TRAPD(error,DoHandleRequestL(aMessage));
	if(error)
		{
		aMessage.Complete(error);
		}
	}

// ---------------------------------------------------------
// CSpeechEncoderConfigMsgHdlr::DoHandleRequestL
// Determines which custom interface to call.
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CSpeechEncoderConfigMsgHdlr::DoHandleRequestL(TMMFMessage& aMessage)
	{
	switch(aMessage.Function())
		{
		case ESecmGetNumOfSupportedBitrates:
			{
			DoGetNumOfSupportedBitratesL(aMessage);
			break;
			}
		case ESecmGetSupportedBitrates:
			{
			DoGetSupportedBitratesL(aMessage);
			break;
			}
		case ESecmSetBitrate:
			{
			DoSetBitrateL(aMessage);
			break;
			}
		case ESecmGetBitrate:
			{
			DoGetBitrateL(aMessage);
			break;
			}
		case ESecmSetVadMode:
			{
			DoSetVadModeL(aMessage);
			break;
			}
		case ESecmGetVadMode:
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
// CSpeechEncoderConfigMsgHdlr::DoGetNumOfSupportedBitratesL
// Handles the message from the proxy and calls the custom interface.
// The custom interface returns the data requested and this function
// writes it back to the proxy. It also creates a buffer and fills
// it with the bitrate data to be returned in the subsequent call
// of DoGetSupportedBitratesL().
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CSpeechEncoderConfigMsgHdlr::DoGetNumOfSupportedBitratesL(
                                                       TMMFMessage& aMessage)
	{
	iSupportedBitrates.Reset();
	TInt status = iSpeechEncoderConfigCI->GetSupportedBitrates(
	                                                     iSupportedBitrates);
	CreateBufFromUintArrayL(iSupportedBitrates);
	if (status == KErrNone)
    	{
	    TPckgBuf<TUint> pckg;
    	pckg() = iSupportedBitrates.Count();
	    aMessage.WriteDataToClientL(pckg);
	    }
    aMessage.Complete(status);
	}

// ---------------------------------------------------------
// CSpeechEncoderConfigMsgHdlr::CreateBufFromUintArrayL
// Utility function used to create a buffer a fill it with data from the array
// passed in.
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CSpeechEncoderConfigMsgHdlr::CreateBufFromUintArrayL(RArray<TUint>& aArray)
	{
	delete iDataCopyBuffer;
	iDataCopyBuffer = NULL;

	iDataCopyBuffer = CBufFlat::NewL(8);
	RBufWriteStream stream;
	stream.Open(*iDataCopyBuffer);
	CleanupClosePushL(stream);
	for (TInt i=0;i<aArray.Count();i++)
		stream.WriteUint32L(aArray[i]);
	CleanupStack::PopAndDestroy(&stream);//stream
	}
	
// ---------------------------------------------------------
// CSpeechEncoderConfigMsgHdlr::DoGetSupportedBitratesL
// Handles the message from the proxy and calls the custom interface.
// The custom interface returns the data requested and this function
// writes it back to the proxy.
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CSpeechEncoderConfigMsgHdlr::DoGetSupportedBitratesL(TMMFMessage& aMessage)
	{
	if (!iDataCopyBuffer)
		User::Leave(KErrNotReady);
	aMessage.WriteDataToClientL(iDataCopyBuffer->Ptr(0));
    aMessage.Complete(KErrNone);
	}
	
// ---------------------------------------------------------
// CSpeechEncoderConfigMsgHdlr::DoSetBitrateL
// Handles the message from the proxy and calls the custom interface method.
// The data passed from the proxy is read from the message and passed to
// the custom interface.
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CSpeechEncoderConfigMsgHdlr::DoSetBitrateL(TMMFMessage& aMessage)
	{
	TPckgBuf<TUint> pckg;
	aMessage.ReadData1FromClientL(pckg);
	TInt status = iSpeechEncoderConfigCI->SetBitrate(pckg());
    aMessage.Complete(status);
	}

// ---------------------------------------------------------
// CSpeechEncoderConfigMsgHdlr::DoGetBitrateL
// Handles the message from the proxy and calls the custom interface.
// The custom interface returns the data requested and this function
// writes it back to the proxy.
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CSpeechEncoderConfigMsgHdlr::DoGetBitrateL(TMMFMessage& aMessage)
	{
	TUint bitrate;
	TInt status = iSpeechEncoderConfigCI->GetBitrate(bitrate);
	if (status == KErrNone)
    	{
    	TPckgBuf<TUint> pckg;
    	pckg() = bitrate;
	    aMessage.WriteDataToClientL(pckg);
    	}
    aMessage.Complete(status);
	}

// ---------------------------------------------------------
// CSpeechEncoderConfigMsgHdlr::DoSetVadModeL
// Handles the message from the proxy and calls the custom interface method.
// The data passed from the proxy is read from the message and passed to
// the custom interface.
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CSpeechEncoderConfigMsgHdlr::DoSetVadModeL(TMMFMessage& aMessage)
	{
	TPckgBuf<TBool> pckg;
	aMessage.ReadData1FromClientL(pckg);
	TInt status = iSpeechEncoderConfigCI->SetVadMode(pckg());
    aMessage.Complete(status);
	}

// ---------------------------------------------------------
// CSpeechEncoderConfigMsgHdlr::DoGetVadModeL
// Handles the message from the proxy and calls the custom interface.
// The custom interface returns the data requested and this function
// writes it back to the proxy.
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CSpeechEncoderConfigMsgHdlr::DoGetVadModeL(TMMFMessage& aMessage)
	{
	TBool vadMode;
	TInt status = iSpeechEncoderConfigCI->GetVadMode(vadMode);
	if (status == KErrNone)
    	{
    	TPckgBuf<TBool> pckg;
	    pckg() = vadMode;
    	aMessage.WriteDataToClientL(pckg);
    	}
    aMessage.Complete(status);
	}



// End of File
