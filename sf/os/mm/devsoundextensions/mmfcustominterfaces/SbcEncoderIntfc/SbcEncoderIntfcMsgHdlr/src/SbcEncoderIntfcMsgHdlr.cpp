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
* Description:   Message handler for SBC encoder interface.
*
*/



// INCLUDE FILES
#include <SbcEncoderIntfc.h>
#include "SbcEncoderIntfcMsgHdlr.h"
#include "SbcEncoderIntfcMsgs.h"

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
 * CSbcEncoderIntfcMsgHdlr::CSbcEncoderIntfcMsgHdlr
 * C++ default constructor can NOT contain any code, that might leave.
 */
CSbcEncoderIntfcMsgHdlr::CSbcEncoderIntfcMsgHdlr(
                         CSbcEncoderIntfc* aSbcEncoderIntfcCI) :
	CMMFObject(KUidSbcEncoderIntfc)
    {
    iSbcEncoderIntfcCI = aSbcEncoderIntfcCI;
    }

/**
 * CSbcEncoderIntfcMsgHdlr::ConstructL
 * Symbian 2nd phase constructor can leave.
 */
void CSbcEncoderIntfcMsgHdlr::ConstructL()
    {
    }

/**
 * CSbcEncoderIntfcMsgHdlr::NewL
 * Two-phased constructor.
 */
EXPORT_C CSbcEncoderIntfcMsgHdlr* CSbcEncoderIntfcMsgHdlr::NewL(
                                  TAny* aSbcEncoderIntfcCI)
    {
    CSbcEncoderIntfc* SbcEncoderIntfcCI =
                      (CSbcEncoderIntfc*)aSbcEncoderIntfcCI;
    CSbcEncoderIntfcMsgHdlr* self =
        new (ELeave) CSbcEncoderIntfcMsgHdlr(SbcEncoderIntfcCI);
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }

/**
 * Destructor
 */
EXPORT_C CSbcEncoderIntfcMsgHdlr::~CSbcEncoderIntfcMsgHdlr()
	{
	delete iDataCopyBuffer;
	delete iSbcEncoderIntfcCI;
	}

/**
 * CSbcEncoderIntfcMsgHdlr::HandleRequest
 * Handles the messages from the proxy.
 * Calls a subfunction which determines which custom interface to call.
 * A subfunction is used to contain multiple leaving functions for a
 * single trap.
 * (other items were commented in a header).
 */
EXPORT_C void CSbcEncoderIntfcMsgHdlr::HandleRequest(TMMFMessage& aMessage)
	{
    // debug panic
	ASSERT(aMessage.Destination().InterfaceId() == KUidSbcEncoderIntfc);
	TRAPD(error, DoHandleRequestL(aMessage));
	if(error)
		{
		aMessage.Complete(error);
		}
	}

/**
 * CSbcEncoderIntfcMsgHdlr::DoHandleRequestL
 * Determines which custom interface to call.
 * (other items were commented in a header).
 */
void CSbcEncoderIntfcMsgHdlr::DoHandleRequestL(TMMFMessage& aMessage)
	{
	switch(aMessage.Function())
		{
		case ESbceimGetNumOfSupportedSamplingFrequencies:
			{
			DoGetNumOfSupportedSamplingFrequenciesL(aMessage);
			break;
			}
		case ESbceimGetSupportedSamplingFrequencies:
			{
			DoGetSupportedSamplingFrequenciesL(aMessage);
			break;
			}
		case ESbceimGetNumOfSupportedChannelModes:
			{
			DoGetNumOfSupportedChannelModesL(aMessage);
			break;
			}
		case ESbceimGetSupportedChannelModes:
			{
			DoGetSupportedChannelModesL(aMessage);
			break;
			}
		case ESbceimGetNumOfSupportedNumOfSubbands:
			{
			DoGetNumOfSupportedNumOfSubbandsL(aMessage);
			break;
			}
		case ESbceimGetSupportedNumOfSubbands:
			{
			DoGetSupportedNumOfSubbandsL(aMessage);
			break;
			}
		case ESbceimGetNumOfSupportedBlocks:
			{
			DoGetNumOfSupportedBlocksL(aMessage);
			break;
			}
		case ESbceimGetSupportedBlocks:
			{
			DoGetSupportedBlocksL(aMessage);
			break;
			}
		case ESbceimGetNumOfSupportedAllocationMethods:
			{
			DoGetNumOfSupportedAllocationMethodsL(aMessage);
			break;
			}
		case ESbceimGetSupportedAllocationMethods:
			{
			DoGetSupportedAllocationMethodsL(aMessage);
			break;
			}
		case ESbceimGetSupportedBitpoolRange:
			{
			DoGetSupportedBitpoolRangeL(aMessage);
			break;
			}
		case ESbceimApplyConfig:
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
 * CSbcEncoderIntfcMsgHdlr::DoGetNumOfSupportedSamplingFrequenciesL
 * Handles the message from the proxy and calls the custom interface.
 * The custom interface returns the data requested and this function writes
 * it back to the proxy. It also creates a buffer and fills the sampling
 * frequencies data to be returned in the subsequent call to
 * DoGetSupportedSamplingFrequenciesL().
 * (other items were commented in a header).
 */
void CSbcEncoderIntfcMsgHdlr::DoGetNumOfSupportedSamplingFrequenciesL(
                              TMMFMessage& aMessage)
	{
	RArray<TUint> supSamplingFrequencies;
    CleanupClosePushL(supSamplingFrequencies);

	TInt status = iSbcEncoderIntfcCI->GetSupportedSamplingFrequencies(
	                                  supSamplingFrequencies);

    // store array in iDataCopyBuffer
	CreateBufFromUintArrayL(supSamplingFrequencies);

	if (status == KErrNone)
    	{
	    TPckgBuf<TUint> pckg;
    	pckg() = supSamplingFrequencies.Count();
	    aMessage.WriteDataToClientL(pckg);
	    }

    CleanupStack::PopAndDestroy(&supSamplingFrequencies);
    aMessage.Complete(status);
	}

/**
 * CSbcEncoderIntfcMsgHdlr::DoGetSupportedSamplingFrequenciesL
 * Sends the sampling frequency data returned from the custom interface
 * implementation to the client.
 * (other items were commented in a header).
 */
void CSbcEncoderIntfcMsgHdlr::DoGetSupportedSamplingFrequenciesL(
                              TMMFMessage& aMessage)
	{
    SendDataBufferToClientL(aMessage);
	}

/**
 * CSbcEncoderIntfcMsgHdlr::DoGetNumOfSupportedChannelModesL
 * Handles the message from the proxy and calls the custom interface.
 * The custom interface returns the data requested and this function writes
 * it back to the proxy. It also creates a buffer and fills the sampling
 * frequencies data to be returned in the subsequent call to
 * DoGetSupportedChannelModesL().
 * (other items were commented in a header).
 */
void CSbcEncoderIntfcMsgHdlr::DoGetNumOfSupportedChannelModesL(
                              TMMFMessage& aMessage)
    {
    RArray<CSbcEncoderIntfc::TSbcChannelMode> supChannelModes;
    CleanupClosePushL(supChannelModes);
	TInt status =
	     iSbcEncoderIntfcCI->GetSupportedChannelModes(supChannelModes);

    // store array in iDataCopyBuffer
	CreateBufFromUintArrayL(reinterpret_cast<RArray<TUint>&>(supChannelModes));

	if (status == KErrNone)
    	{
	    TPckgBuf<TUint> pckg;
    	pckg() = supChannelModes.Count();
	    aMessage.WriteDataToClientL(pckg);
	    }

    CleanupStack::PopAndDestroy(&supChannelModes);
    aMessage.Complete(status);
    }

/**
 * CSbcEncoderIntfcMsgHdlr::DoGetSupportedChannelModesL
 * Sends the channel modes data returned from the custom interface
 * implementation to the client.
 * (other items were commented in a header).
 */
void CSbcEncoderIntfcMsgHdlr::DoGetSupportedChannelModesL(
                              TMMFMessage& aMessage)
    {
    SendDataBufferToClientL(aMessage);
    }

/**
 * CSbcEncoderIntfcMsgHdlr::DoGetNumOfSupportedBlocksL
 * Handles the message from the proxy and calls the custom interface.
 * The custom interface returns the data requested and this function writes
 * it back to the proxy. It also creates a buffer and fills the supported
 * blocks data to be returned in the subsequent call to
 * DoGetSupportedBlocksL().
 * (other items were commented in a header).
 */
void CSbcEncoderIntfcMsgHdlr::DoGetNumOfSupportedBlocksL(
                              TMMFMessage& aMessage)
    {
	RArray<TUint> supNumBlocks;
    CleanupClosePushL(supNumBlocks);
    
	TInt status = iSbcEncoderIntfcCI->GetSupportedNumOfBlocks(supNumBlocks);

	CreateBufFromUintArrayL(supNumBlocks);

	if (status == KErrNone)
    	{
	    TPckgBuf<TUint> pckg;
    	pckg() = supNumBlocks.Count();
	    aMessage.WriteDataToClientL(pckg);
	    }

    CleanupStack::PopAndDestroy(&supNumBlocks);
    aMessage.Complete(status);
    }

/**
 * CSbcEncoderIntfcMsgHdlr::DoGetSupportedBlocksL
 * Sends the supported blocks data returned from the custom interface
 * implementation to the client.
 * (other items were commented in a header).
 */
void CSbcEncoderIntfcMsgHdlr::DoGetSupportedBlocksL(TMMFMessage& aMessage)
    {
    SendDataBufferToClientL(aMessage);
    }

/**
 * CSbcEncoderIntfcMsgHdlr::DoGetNumOfSupportedNumOfSubbandsL
 * Handles the message from the proxy and calls the custom interface.
 * The custom interface returns the data requested and this function writes
 * it back to the proxy. It also creates a buffer and fills the supported
 * subbands data to be returned in the subsequent call to
 * DoGetSupportedNumOfSubbandsL().
 * (other items were commented in a header).
 */
void CSbcEncoderIntfcMsgHdlr::DoGetNumOfSupportedNumOfSubbandsL(
                              TMMFMessage& aMessage)
    {
	RArray<TUint> supNumSubbands;
	CleanupClosePushL(supNumSubbands);

	TInt status = iSbcEncoderIntfcCI->GetSupportedNumOfSubbands(supNumSubbands);

	CreateBufFromUintArrayL(supNumSubbands);

	if (status == KErrNone)
    	{
	    TPckgBuf<TUint> pckg;
    	pckg() = supNumSubbands.Count();
	    aMessage.WriteDataToClientL(pckg);
	    }

    CleanupStack::PopAndDestroy(&supNumSubbands);
    aMessage.Complete(status);
    }

/**
 * CSbcEncoderIntfcMsgHdlr::DoGetSupportedNumOfSubbandsL
 * Sends the supported subbands data returned from the custom interface
 * implementation to the client.
 * (other items were commented in a header).
 */
void CSbcEncoderIntfcMsgHdlr::DoGetSupportedNumOfSubbandsL(
                              TMMFMessage& aMessage)
    {
    SendDataBufferToClientL(aMessage);
    }

/**
 * CSbcEncoderIntfcMsgHdlr::DoGetNumOfSupportedAllocationMethodsL
 * Handles the message from the proxy and calls the custom interface.
 * The custom interface returns the data requested and this function writes
 * it back to the proxy. It also creates a buffer and fills the supported
 * allocation methods data to be returned in the subsequent call to
 * DoGetSupportedAllocationMethodsL().
 * (other items were commented in a header).
 */
void CSbcEncoderIntfcMsgHdlr::DoGetNumOfSupportedAllocationMethodsL(
                              TMMFMessage& aMessage)
    {
    RArray<CSbcEncoderIntfc::TSbcAllocationMethod> supAllocMethods;
	CleanupClosePushL(supAllocMethods);

	TInt status =
	     iSbcEncoderIntfcCI->GetSupportedAllocationMethods(supAllocMethods);

    // store array in iDataCopyBuffer
	CreateBufFromUintArrayL(reinterpret_cast<RArray<TUint>&>(supAllocMethods));

	if (status == KErrNone)
    	{
	    TPckgBuf<TUint> pckg;
    	pckg() = supAllocMethods.Count();
	    aMessage.WriteDataToClientL(pckg);
	    }

    CleanupStack::PopAndDestroy(&supAllocMethods);
    aMessage.Complete(status);
    }

/**
 * CSbcEncoderIntfcMsgHdlr::DoGetSupportedAllocationMethodsL
 * Sends the supported allocation methods data returned from the custom
 * interface implementation to the client.
 * (other items were commented in a header).
 */
void CSbcEncoderIntfcMsgHdlr::DoGetSupportedAllocationMethodsL(
                              TMMFMessage& aMessage)
    {
    SendDataBufferToClientL(aMessage);
    }

/**
 * CSbcEncoderIntfcMsgHdlr::DoGetSupportedBitpoolRangeL
 * Handles the message from the proxy and calls the custom interface.
 * The custom interface returns the requested bitpool range and this function
 * writes it back to the proxy.
 * (other items were commented in a header).
 */
void CSbcEncoderIntfcMsgHdlr::DoGetSupportedBitpoolRangeL(
                              TMMFMessage& aMessage)
    {
    TSbcEncoderBitpoolRange bitPoolRange;
    bitPoolRange.iMinSupportedBitpoolSize = 0;
    bitPoolRange.iMaxSupportedBitpoolSize = 0;

	TInt status = iSbcEncoderIntfcCI->GetSupportedBitpoolRange(
	                                  bitPoolRange.iMinSupportedBitpoolSize,
	                                  bitPoolRange.iMaxSupportedBitpoolSize);

	if (status == KErrNone)
    	{
	    TPckgBuf<TSbcEncoderBitpoolRange> pckg;
    	pckg() = bitPoolRange;
	    aMessage.WriteDataToClientL(pckg);
	    }

    aMessage.Complete(status);
    }

/**
 * CSbcEncoderIntfcMsgHdlr::DoApplyConfigL
 * Handles the message from the proxy to commit configuration settings and
 * calls the custom interface method. The data passed from the proxy is read
 * from the message and passed to the custom interface.
 * (other items were commented in a header).
 */
void CSbcEncoderIntfcMsgHdlr::DoApplyConfigL(TMMFMessage& aMessage)
	{
	TPckgBuf<TSbcEncoderConfig> pckgBuf;
	aMessage.ReadData1FromClientL(pckgBuf);

	iSbcEncoderIntfcCI->SetSamplingFrequency(pckgBuf().iSamplingFrequency);
	iSbcEncoderIntfcCI->SetChannelMode(pckgBuf().iChannelMode);
	iSbcEncoderIntfcCI->SetNumOfSubbands(pckgBuf().iNumOfSubbands);
	iSbcEncoderIntfcCI->SetNumOfBlocks(pckgBuf().iNumOfBlocks);
	iSbcEncoderIntfcCI->SetAllocationMethod(pckgBuf().iAllocationMethod);
	iSbcEncoderIntfcCI->SetBitpoolSize(pckgBuf().iBitpoolSize);

	TInt status = iSbcEncoderIntfcCI->ApplyConfig();
    aMessage.Complete(status);
	}

/**
 * CSbcEncoderIntfcMsgHdlr::CreateBufFromUintArrayL
 * Utility function used to create a buffer and fill it with data from the
 * array passed in.
 * (other items were commented in a header).
 */
void CSbcEncoderIntfcMsgHdlr::CreateBufFromUintArrayL(RArray<TUint>& aArray)
	{
	delete iDataCopyBuffer;
	iDataCopyBuffer = NULL;
	iDataCopyBuffer = CBufFlat::NewL(8);

	RBufWriteStream stream;
	stream.Open(*iDataCopyBuffer);
	CleanupClosePushL(stream);

	for (TInt i = 0; i < aArray.Count(); i++)
	    {	    	    
		stream.WriteUint32L(aArray[i]);
	    }
	    
	CleanupStack::PopAndDestroy(&stream);
	}	

/**
 * CSbcEncoderIntfcMsgHdlr::SendDataBufferToClientL
 * Sends message with requested configuration data back to the client.
 * (other items were commented in a header).
 */
void CSbcEncoderIntfcMsgHdlr::SendDataBufferToClientL(TMMFMessage& aMessage)
    {
	if (!iDataCopyBuffer)
	    {
		User::Leave(KErrNotReady);
	    }

	aMessage.WriteDataToClientL(iDataCopyBuffer->Ptr(0));
    aMessage.Complete(KErrNone);
    }


// ========================== OTHER EXPORTED FUNCTIONS =========================

// End of File
