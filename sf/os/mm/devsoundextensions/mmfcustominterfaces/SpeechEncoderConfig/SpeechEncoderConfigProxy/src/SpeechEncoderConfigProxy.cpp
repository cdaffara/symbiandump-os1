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
* Description:   Interface proxy for speech encoder configuration.
*
*/



// INCLUDE FILES
#include "SpeechEncoderConfigProxy.h"
#include "SpeechEncoderConfigMsgs.h"
#include <CustomCommandUtility.h>
#include <CustomInterfaceUtility.h>

// EXTERNAL DATA STRUCTURES

// EXTERNAL FUNCTION PROTOTYPES  

// CONSTANTS

// MACROS

// LOCAL CONSTANTS AND MACROS

// MODULE DATA STRUCTURES

// LOCAL FUNCTION PROTOTYPES

// FORWARD DECLARATIONS

// ============================= LOCAL FUNCTIONS ===============================

// ================= MEMBER FUNCTIONS =======================

// -----------------------------------------------------------------------------
// CSpeechEncoderConfigProxy::CSpeechEncoderConfigProxy
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CSpeechEncoderConfigProxy::CSpeechEncoderConfigProxy(
                            TMMFMessageDestinationPckg aMessageHandler, 
                            MCustomCommand& aCustomCommand,
                            CCustomInterfaceUtility* aCustomInterfaceUtility) :
	iCustomCommand(aCustomCommand),
	iMessageHandler(aMessageHandler),
	iCustomInterfaceUtility(aCustomInterfaceUtility)
    {
    }


// -----------------------------------------------------------------------------
// CSpeechEncoderConfigProxy::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CSpeechEncoderConfigProxy::ConstructL()
    {
    }

// -----------------------------------------------------------------------------
// CSpeechEncoderConfigProxy::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
EXPORT_C CSpeechEncoderConfigProxy* CSpeechEncoderConfigProxy::NewL(
                               TMMFMessageDestinationPckg aMessageHandler, 
                               MCustomCommand& aCustomCommand,
                               CCustomInterfaceUtility* aCustomInterfaceUtility)
    {
    CSpeechEncoderConfigProxy* self = new(ELeave) CSpeechEncoderConfigProxy(
                                                      aMessageHandler,
                                                      aCustomCommand,
                                                      aCustomInterfaceUtility);
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }
    
// Destructor
EXPORT_C CSpeechEncoderConfigProxy::~CSpeechEncoderConfigProxy()
    {
    iCustomInterfaceUtility->RemoveCustomInterface(iMessageHandler);
    delete iCustomInterfaceUtility;
    }

// ---------------------------------------------------------
// CSpeechEncoderConfigProxy::GetSupportedBitrates
// Calls a  subfunction which sends the appropriate custom command for this
// function to its message handler. A subfunction is used
// to contain multiple leaving functions for a single trap.
// (other items were commented in a header).
// ---------------------------------------------------------
//
EXPORT_C TInt CSpeechEncoderConfigProxy::GetSupportedBitrates(
                                           RArray<TUint>& aSupportedBitrates)
    {
    TInt status;
    TRAP(status, GetSupportedBitratesL(aSupportedBitrates));
    return status;
    }

// ---------------------------------------------------------
// CSpeechEncoderConfigProxy::GetSupportedBitratesL
// Sends the custom command for this function to its message handler.
// This requires two commands. The first is a request for the number of
// supported bitrates. A buffer is allocated locally to hold this number of
// bitrates that will be returned. A pointer to this buffer is sent with the
// next command which is a request for the bitrates. This buffer will be filled
// with the bitrate values. These bitrates are then copied into the array
// provided to this function and the local buffer is deleted.
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CSpeechEncoderConfigProxy::GetSupportedBitratesL(
                                            RArray<TUint>& aSupportedBitrates)
    {
	aSupportedBitrates.Reset();

	TPckgBuf<TUint> numberOfBitratesPckg;
	User::LeaveIfError(iCustomCommand.CustomCommandSync(
	                                           iMessageHandler, 
                                               ESecmGetNumOfSupportedBitrates, 
                                               KNullDesC8,
                                               KNullDesC8,
                                               numberOfBitratesPckg));
    HBufC8* buf = HBufC8::NewLC(numberOfBitratesPckg()*sizeof(TUint));
    // buf is left on cleanup 
    TPtr8 ptr = buf->Des();

    User::LeaveIfError(iCustomCommand.CustomCommandSync(
                                               iMessageHandler, 
                                               ESecmGetSupportedBitrates, 
                                               KNullDesC8,
                                               KNullDesC8,
                                               ptr));
   	RDesReadStream stream(ptr);
    CleanupClosePushL(stream); // stream on cleanup
   	for (TInt i=0; i<numberOfBitratesPckg(); i++)
      	{
   		aSupportedBitrates.AppendL(stream.ReadUint32L());
   		}
   	CleanupStack::PopAndDestroy(&stream);
   	CleanupStack::PopAndDestroy(buf);
    }

// ---------------------------------------------------------
// CSpeechEncoderConfigProxy::SetBitrate
// Sends the custom command for this function to its message handler.
// (other items were commented in a header).
// ---------------------------------------------------------
//
EXPORT_C TInt CSpeechEncoderConfigProxy::SetBitrate(TUint aBitrate)
	{
	TPckgBuf<TInt> bitratePckg(aBitrate);
	TInt status = iCustomCommand.CustomCommandSync(iMessageHandler,
	                                               ESecmSetBitrate,
	                                               bitratePckg,
	                                               KNullDesC8);
	return status;
	}

// ---------------------------------------------------------
// CSpeechEncoderConfigProxy::GetBitrate
// Sends the custom command for this function to its message handler.
// (other items were commented in a header).
// ---------------------------------------------------------
//
EXPORT_C TInt CSpeechEncoderConfigProxy::GetBitrate(TUint& aBitrate)
	{
	TInt bitrate = 0;
	TPckgBuf<TInt> bitratePckg(bitrate);
	TInt status = iCustomCommand.CustomCommandSync(iMessageHandler,
	                                               ESecmGetBitrate,
	                                               KNullDesC8,
	                                               KNullDesC8,
	                                               bitratePckg);
	if (status == KErrNone)
	    aBitrate = bitratePckg();
	return status;
	}

// ---------------------------------------------------------
// CSpeechEncoderConfigProxy::SetVadMode
// Sends the custom command for this function to its message handler.
// (other items were commented in a header).
// ---------------------------------------------------------
//
EXPORT_C TInt CSpeechEncoderConfigProxy::SetVadMode(TBool aVadMode)
	{
	TPckgBuf<TBool> pckgBuf(aVadMode);
	TInt status = iCustomCommand.CustomCommandSync(iMessageHandler,
	                                               ESecmSetVadMode,
	                                               pckgBuf,
	                                               KNullDesC8);
	return status;
	}

// ---------------------------------------------------------
// CSpeechEncoderConfigProxy::GetVadMode
// Sends the custom command for this function to its message handler.
// (other items were commented in a header).
// ---------------------------------------------------------
//
EXPORT_C TInt CSpeechEncoderConfigProxy::GetVadMode(TBool& aVadMode)
	{
	TInt vadMode = EFalse;
	TPckgBuf<TBool> pckgBuf(vadMode);
	TInt status = iCustomCommand.CustomCommandSync(iMessageHandler,
	                                               ESecmGetVadMode,
	                                               KNullDesC8,
	                                               KNullDesC8,
	                                               pckgBuf);
	if (status == KErrNone)
	    aVadMode = pckgBuf();
	return status;
	}



// End of File
