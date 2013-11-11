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
* Description:   Interface proxy for BT SBC Encoder configuration CI.
*
*/



// INCLUDE FILES
#include <CustomCommandUtility.h>
#include <CustomInterfaceUtility.h>
#include "SbcEncoderIntfcMsgs.h"
#include "SbcEncoderIntfcProxy.h"

// EXTERNAL DATA STRUCTURES

// EXTERNAL FUNCTION PROTOTYPES

// CONSTANTS

// MACROS

// LOCAL CONSTANTS AND MACROS

// MODULE DATA STRUCTURES

// LOCAL FUNCTION PROTOTYPES

// FORWARD DECLARATIONS

// ============================= LOCAL FUNCTIONS ===============================

// ============================= MEMBER FUNCTIONS ==============================

/**
 * CSbcEncoderIntfcProxy::CSbcEncoderIntfcProxy
 * C++ default constructor can NOT contain any code, that might leave.
 */
CSbcEncoderIntfcProxy::CSbcEncoderIntfcProxy(
                       TMMFMessageDestinationPckg aMessageHandler,
                       MCustomCommand& aCustomCommand,
                       CCustomInterfaceUtility* aCustomInterfaceUtility) :
	iCustomCommand(aCustomCommand),
	iMessageHandler(aMessageHandler),
	iCustomInterfaceUtility(aCustomInterfaceUtility)
    {
    }

/**
 * CSbcEncoderIntfcProxy::ConstructL
 * Symbian 2nd phase constructor can leave.
 */
void CSbcEncoderIntfcProxy::ConstructL()
    {
    iHasBeenApplied = EFalse;
    }

/**
 * CSbcEncoderIntfcProxy::NewL
 * Two-phased constructor.
 */
EXPORT_C CSbcEncoderIntfcProxy* CSbcEncoderIntfcProxy::NewL(
                               TMMFMessageDestinationPckg aMessageHandler,
                               MCustomCommand& aCustomCommand,
                               CCustomInterfaceUtility* aCustomInterfaceUtility)
    {
    CSbcEncoderIntfcProxy* self = new(ELeave) CSbcEncoderIntfcProxy(
                                              aMessageHandler,
                                              aCustomCommand,
                                              aCustomInterfaceUtility);
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }

/**
 * Destructor
 */
EXPORT_C CSbcEncoderIntfcProxy::~CSbcEncoderIntfcProxy()
    {
    iCustomInterfaceUtility->RemoveCustomInterface(iMessageHandler);
    delete iCustomInterfaceUtility;
    }

/**
 * CSbcEncoderIntfcProxy::GetSupportedSamplingFrequencies
 * Returns an array of supported sampling frequencies.
 * Calls a subfunction, which sends the appropriate custom command
 * to its message handler. A subfunction is used to contain multiple
 * leaving functions in a single trap.
 * (other items were commented in a header).
 */
EXPORT_C TInt CSbcEncoderIntfcProxy::GetSupportedSamplingFrequencies(
                                     RArray<TUint>&
                                     aSupportedSamplingFrequencies)
    {
    TRAPD(status,
          GetSupportedSamplingFrequenciesL(aSupportedSamplingFrequencies));
    return status;
    }

/**
 * CSbcEncoderIntfcProxy::GetSupportedSamplingFrequenciesL
 * Returns an array of supported sampling frequencies.
 * Sends the custom command for this function to its message handler. This
 * requires two commands. The first is a request for the number of supported
 * sampling frequencies. A buffer is allocated locally to hold this number of
 * frequencies that will be returned. A pointer to this buffer is sent with the
 * next command, which is a request for the frequencies. This buffer will be
 * filled with the frequency values. These values are then copied into the
 * array provided to this function and the local buffer is deleted.
 * (other items were commented in a header).
 */
void CSbcEncoderIntfcProxy::GetSupportedSamplingFrequenciesL(
                            RArray<TUint>& aSupportedSamplingFrequencies)
    {
	aSupportedSamplingFrequencies.Reset();

	TPckgBuf<TUint> pckgBuf;
	User::LeaveIfError(iCustomCommand.CustomCommandSync(
                       iMessageHandler,
                       ESbceimGetNumOfSupportedSamplingFrequencies,
                       KNullDesC8,
                       KNullDesC8,
                       pckgBuf));

    HBufC8* buf = HBufC8::NewLC(pckgBuf() * sizeof(TUint));
    TPtr8 ptr = buf->Des();

    User::LeaveIfError(iCustomCommand.CustomCommandSync(
                       iMessageHandler,
                       ESbceimGetSupportedSamplingFrequencies,
                       KNullDesC8,
                       KNullDesC8,
                       ptr));

    PopulateArrayL(aSupportedSamplingFrequencies, ptr, pckgBuf());
   	CleanupStack::PopAndDestroy(buf);
    }

/**
 * CSbcEncoderIntfcProxy::GetSupportedChannelModes
 * Returns an array of supported channel modes.
 * Calls a  subfunction which sends the appropriate custom command to its
 * message handler. A subfunction is used to contain multiple leaving
 * functions for a single trap.
 * (other items were commented in a header).
 */
EXPORT_C TInt CSbcEncoderIntfcProxy::GetSupportedChannelModes(
                                     RArray<TSbcChannelMode>&
                                     aSupportedChannelModes)
    {
    TRAPD(status, GetSupportedChannelModesL(aSupportedChannelModes));
    return status;
    }

/**
 * CSbcEncoderIntfcProxy::GetSupportedChannelModesL
 * Returns an array of supported channel modes.
 * Sends the custom command for this function to its message handler. This
 * requires two commands. The first is a request for the number of supported
 * channel modes. A buffer is allocated locally to hold this number of channel
 * modes that will be returned. A pointer to this buffer is sent with the next
 * command, which is a request for the channel modes. This buffer will be
 * filled with the frequency values. These values are then copied into the array
 * provided to this function and the local buffer is deleted.
 * (other items were commented in a header).
 */
void CSbcEncoderIntfcProxy::GetSupportedChannelModesL(
                            RArray<TSbcChannelMode>& aSupportedChannelModes)
    {
	aSupportedChannelModes.Reset();

	TPckgBuf<TSbcChannelMode> pckgBuf;
	User::LeaveIfError(iCustomCommand.CustomCommandSync(
                                      iMessageHandler,
                                      ESbceimGetNumOfSupportedChannelModes,
                                      KNullDesC8,
                                      KNullDesC8,
                                      pckgBuf));

    HBufC8* buf = HBufC8::NewLC(pckgBuf() * sizeof(TUint));
    TPtr8 ptr = buf->Des();
    User::LeaveIfError(iCustomCommand.CustomCommandSync(
                                      iMessageHandler,
                                      ESbceimGetSupportedChannelModes,
                                      KNullDesC8,
                                      KNullDesC8,
                                      ptr));

   	RDesReadStream stream(ptr);
    CleanupClosePushL(stream);

   	for (TInt i = 0; i < pckgBuf(); i++)
      	{
   		aSupportedChannelModes.AppendL(
   		    static_cast<TSbcChannelMode>(stream.ReadUint32L()));
   		}

   	CleanupStack::PopAndDestroy(&stream);
   	CleanupStack::PopAndDestroy(buf);
    }

/**
 * CSbcEncoderIntfcProxy::GetSupportedNumOfBlocks
 * Returns an array of supported blocks.
 * Calls a subfunction, which sends the appropriate custom command to its
 * message handler. A subfunction is used to contain multiple leaving functions
 * for a single trap.
 * (other items were commented in a header).
 */
EXPORT_C TInt CSbcEncoderIntfcProxy::GetSupportedNumOfBlocks(
		                             RArray<TUint>& aSupportedNumOfBlocks)
    {
    TRAPD(status, GetSupportedNumOfBlocksL(aSupportedNumOfBlocks));
    return status;
    }

/**
 * CSbcEncoderIntfcProxy::GetSupportedNumOfBlocks
 * Returns an array of supported blocks.
 * Sends the custom command for this function to its message handler. This
 * requires two commands. The first is a request for the number of supported
 * blocks. A buffer is allocated locally to hold this number of blocks that
 * will be returned. A pointer to this buffer is then sent with the next
 * command, which is a request for the blocks. This buffer will be filled
 * with the block values. These values are then copied into the array provided
 * to this function and the local buffer is deleted.
 * (other items were commented in a header).
 */
void CSbcEncoderIntfcProxy::GetSupportedNumOfBlocksL(
		                    RArray<TUint>& aSupportedNumOfBlocks)
    {
	aSupportedNumOfBlocks.Reset();

	TPckgBuf<TUint> pckgBuf;
	User::LeaveIfError(iCustomCommand.CustomCommandSync(
                                      iMessageHandler,
                                      ESbceimGetNumOfSupportedBlocks,
                                      KNullDesC8,
                                      KNullDesC8,
                                      pckgBuf));

    HBufC8* buf = HBufC8::NewLC(pckgBuf() * sizeof(TUint));
    TPtr8 ptr = buf->Des();
    User::LeaveIfError(iCustomCommand.CustomCommandSync(
                                      iMessageHandler,
                                      ESbceimGetSupportedBlocks,
                                      KNullDesC8,
                                      KNullDesC8,
                                      ptr));

    PopulateArrayL(aSupportedNumOfBlocks, ptr, pckgBuf());
   	CleanupStack::PopAndDestroy(buf);
    }

/**
 * CSbcEncoderIntfcProxy::GetSupportedNumOfSubbands
 * Returns an array of supported subbands.
 * Calls a  subfunction which sends the appropriate custom command to its
 * message handler. A subfunction is used to contain multiple leaving functions
 * for a single trap.
 * (other items were commented in a header).
 */
EXPORT_C TInt CSbcEncoderIntfcProxy::GetSupportedNumOfSubbands(
                                     RArray<TUint>& aSupportedNumOfSubbands)
    {
    TRAPD(status, GetSupportedNumOfSubbandsL(aSupportedNumOfSubbands));
    return status;
    }

/**
 * CSbcEncoderIntfcProxy::GetSupportedNumOfSubbandsL
 * Returns an array of supported subbands.
 * Sends the custom command for this function to its message handler. This
 * requires two commands. The first is a request for the number of supported
 * subbands. A buffer is allocated locally to hold this number of subbands that
 * will be returned. A pointer to this buffer is sent with the next command,
 * which is a request for the subbands. This buffer will be filled with the
 * subband values. These values are then copied into the array provided to this
 * function and the local buffer is deleted.
 * (other items were commented in a header).
 */
void CSbcEncoderIntfcProxy::GetSupportedNumOfSubbandsL(
                            RArray<TUint>& aSupportedNumOfSubbands)
    {
	aSupportedNumOfSubbands.Reset();

	TPckgBuf<TUint> pckgBuf;
	User::LeaveIfError(iCustomCommand.CustomCommandSync(
                                      iMessageHandler,
                                      ESbceimGetNumOfSupportedNumOfSubbands,
                                      KNullDesC8,
                                      KNullDesC8,
                                      pckgBuf));

    HBufC8* buf = HBufC8::NewLC(pckgBuf() * sizeof(TUint));
    TPtr8 ptr = buf->Des();
    User::LeaveIfError(iCustomCommand.CustomCommandSync(
                                      iMessageHandler,
                                      ESbceimGetSupportedNumOfSubbands,
                                      KNullDesC8,
                                      KNullDesC8,
                                      ptr));

    PopulateArrayL(aSupportedNumOfSubbands, ptr, pckgBuf());
   	CleanupStack::PopAndDestroy(buf);
    }

/**
 * CSbcEncoderIntfcProxy::GetSupportedAllocationMethods
 * Returns an array of supported allocation methods.
 * Calls a  subfunction which sends the appropriate custom command to its
 * message handler. A subfunction is used to contain multiple leaving
 * functions for a single trap.
 * (other items were commented in a header).
 */
EXPORT_C TInt CSbcEncoderIntfcProxy::GetSupportedAllocationMethods(
                                     RArray<TSbcAllocationMethod>&
                                     aSupportedAllocationMethods)
    {
    TRAPD(status, GetSupportedAllocationMethodsL(aSupportedAllocationMethods));
    return status;
    }

/**
 * CSbcEncoderIntfcProxy::GetSupportedAllocationMethodsL
 * Returns an array of supported allocation methods.
 * Sends the custom command for this function to its message handler. This
 * requires two commands. The first is a request for the number of supported
 * allocation methods. A buffer is allocated locally to hold this number of
 * allocation methods that will be returned. A pointer to this buffer is sent
 * with the next command which is a request for the allocation methods. This
 * buffer will be filled with the allocation method values. These values are
 * then copied into the array provided to this function and the local buffer
 * is deleted.
 * (other items were commented in a header).
 */
void CSbcEncoderIntfcProxy::GetSupportedAllocationMethodsL(
                            RArray<TSbcAllocationMethod>&
                            aSupportedAllocationMethods)
    {
	aSupportedAllocationMethods.Reset();

	TPckgBuf<TSbcAllocationMethod> pckgBuf;
	User::LeaveIfError(iCustomCommand.CustomCommandSync(
                                      iMessageHandler,
                                      ESbceimGetNumOfSupportedAllocationMethods,
                                      KNullDesC8,
                                      KNullDesC8,
                                      pckgBuf));

    HBufC8* buf = HBufC8::NewLC(pckgBuf() * sizeof(TUint));
    TPtr8 ptr = buf->Des();
    User::LeaveIfError(iCustomCommand.CustomCommandSync(
                                      iMessageHandler,
                                      ESbceimGetSupportedAllocationMethods,
                                      KNullDesC8,
                                      KNullDesC8,
                                      ptr));

   	RDesReadStream stream(ptr);
    CleanupClosePushL(stream);

   	for (TInt i = 0; i < pckgBuf(); i++)
      	{
   		aSupportedAllocationMethods.AppendL(
   		            static_cast<TSbcAllocationMethod>(stream.ReadUint32L()));
   		}

   	CleanupStack::PopAndDestroy(&stream);
   	CleanupStack::PopAndDestroy(buf);
    }

/**
 * CSbcEncoderIntfcProxy::GetSupportedBitpoolRange
 * Returns supported bitpool range.
 * Calls a subfunction which sends the appropriate custom command to its
 * message handler. A subfunction is used to contain multiple leaving
 * functions for a single trap.
 * (other items were commented in a header).
 */
EXPORT_C TInt CSbcEncoderIntfcProxy::GetSupportedBitpoolRange(
        		                     TUint& aMinSupportedBitpoolSize,
        		                     TUint& aMaxSupportedBitpoolSize)
    {
    TRAPD(status, GetSupportedBitpoolRangeL(aMinSupportedBitpoolSize,
                                            aMaxSupportedBitpoolSize));
    return status;
    }

/**
 * CSbcEncoderIntfcProxy::GetSupportedBitpoolRangeL
 * Returns an array of supported bitpool range.
 * Sends the custom command for this function to its message handler with TUint
 * type arguments that will be filled with the bitpool min and max values.
 * (other items were commented in a header).
 */
void CSbcEncoderIntfcProxy::GetSupportedBitpoolRangeL(
                            TUint& aMinSupportedBitpoolSize,
		                    TUint& aMaxSupportedBitpoolSize)
    {
    TSbcEncoderBitpoolRange bitPoolRange;
    bitPoolRange.iMinSupportedBitpoolSize = 0;
    bitPoolRange.iMaxSupportedBitpoolSize = 0;

	TPckgBuf<TSbcEncoderBitpoolRange> pckgBuf(bitPoolRange);
    User::LeaveIfError(iCustomCommand.CustomCommandSync(
                                      iMessageHandler,
                                      ESbceimGetSupportedBitpoolRange,
                                      KNullDesC8,
                                      KNullDesC8,
                                      pckgBuf));

    aMinSupportedBitpoolSize = pckgBuf().iMinSupportedBitpoolSize;
    aMaxSupportedBitpoolSize = pckgBuf().iMaxSupportedBitpoolSize;
    }

/*
 * CSbcEncoderIntfcProxy::ApplyConfig
 * Commits encoder configuration settings configured by callas to Set() APIs.
 * Sends the custom command for this function to its message handler.
 * New settings will not take effect until ApplyConfig() is called.
 * (other items were commented in a header).
 */
EXPORT_C TInt CSbcEncoderIntfcProxy::ApplyConfig()
    {
    TInt status = KErrNone;

    if (!iHasBeenApplied)
        {
        if (!iSbcEncConf.iSamplingFrequencySet ||
        	!iSbcEncConf.iChannelModeSet ||
        	!iSbcEncConf.iNumOfSubbandsSet ||
        	!iSbcEncConf.iNumOfBlocksSet ||
        	!iSbcEncConf.iAllocationMethodSet ||
        	!iSbcEncConf.iBitpoolSizeSet)
            {
            status = KErrArgument;
            }
        }

    if (status == KErrNone)
        {
        TSbcEncoderConfig sbcEncoderConfig;

    	sbcEncoderConfig.iSamplingFrequency = iSbcEncConf.iSamplingFrequency;
    	sbcEncoderConfig.iChannelMode       = iSbcEncConf.iChannelMode;
    	sbcEncoderConfig.iNumOfSubbands     = iSbcEncConf.iNumOfSubbands;
    	sbcEncoderConfig.iNumOfBlocks       = iSbcEncConf.iNumOfBlocks;
    	sbcEncoderConfig.iAllocationMethod  = iSbcEncConf.iAllocationMethod;
    	sbcEncoderConfig.iBitpoolSize       = iSbcEncConf.iBitpoolSize;

    	TPckgBuf<TSbcEncoderConfig> pckgBuf(sbcEncoderConfig);
    	status = iCustomCommand.CustomCommandSync(iMessageHandler,
    	                                          ESbceimApplyConfig,
    	                                          pckgBuf,
    	                                          KNullDesC8);
    	if (status == KErrNone)
    		{
    		iSbcEncConfCurrent = iSbcEncConf;
    		iHasBeenApplied = ETrue;
    		}
        }

	return status;
    }

/**
 * CSbcEncoderIntfcProxy::SetSamplingFrequency
 * Saves locally requested sampling frequency.
 * Change does not apply to the encoder until ApplyConfig() is called.
 * (other items were commented in a header).
 */
EXPORT_C void CSbcEncoderIntfcProxy::SetSamplingFrequency(
                                     TUint aSamplingFrequency)
    {
    iSbcEncConf.iSamplingFrequency = aSamplingFrequency;
    iSbcEncConf.iSamplingFrequencySet = ETrue;
    }

/**
 * CSbcEncoderIntfcProxy::GetSamplingFrequency
 * Returns current sampling frequency commited by call to ApplyConfig().
 * (other items were commented in a header).
 */
EXPORT_C TInt CSbcEncoderIntfcProxy::GetSamplingFrequency(
                                     TUint& aSamplingFrequency)
    {
    TInt status = KErrNone;

    if (iHasBeenApplied)
        {
        aSamplingFrequency = iSbcEncConfCurrent.iSamplingFrequency;
        }
    else
        {
        status = KErrArgument;
        }

    return status;
    }

/**
 * CSbcEncoderIntfcProxy::SetChannelMode
 * Saves locally requested channel mode.
 * Change does not apply to the encoder until ApplyConfig() is called.
 * (other items were commented in a header).
 */
EXPORT_C void CSbcEncoderIntfcProxy::SetChannelMode(
                                     TSbcChannelMode aChannelMode)
    {
    iSbcEncConf.iChannelMode = aChannelMode;
    iSbcEncConf.iChannelModeSet = ETrue;
    }

/**
 * CSbcEncoderIntfcProxy::GetChannelMode
 * Returns current channel mode commited by call to ApplyConfig().
 * (other items were commented in a header).
 */
EXPORT_C TInt CSbcEncoderIntfcProxy::GetChannelMode(
                                     TSbcChannelMode& aChannelMode)
    {
    TInt status = KErrNone;

    if (iHasBeenApplied)
        {
        aChannelMode = iSbcEncConfCurrent.iChannelMode;
        }
    else
        {
        status = KErrArgument;
        }

    return status;
    }

/**
 * CSbcEncoderIntfcProxy::SetNumOfSubbands
 * Saves locally requested number of subbands.
 * Change does not apply to the encoder until ApplyConfig() is called.
 * (other items were commented in a header).
 */
EXPORT_C void CSbcEncoderIntfcProxy::SetNumOfSubbands(TUint aNumOfSubbands)
    {
    iSbcEncConf.iNumOfSubbands = aNumOfSubbands;
    iSbcEncConf.iNumOfSubbandsSet = ETrue;
    }

/**
 * CSbcEncoderIntfcProxy::GetNumOfSubbands
 * Returns current number of subbands commited by call to ApplyConfig().
 * (other items were commented in a header).
 */
EXPORT_C TInt CSbcEncoderIntfcProxy::GetNumOfSubbands(TUint& aNumOfSubbands)
    {
    TInt status = KErrNone;

    if (iHasBeenApplied)
        {
        aNumOfSubbands = iSbcEncConfCurrent.iNumOfSubbands;
        }
    else
        {
        status = KErrArgument;
        }

    return status;
    }

/**
 * CSbcEncoderIntfcProxy::SetNumOfBlocks
 * Saves locally requested number of blocks.
 * Change does not apply to the encoder until ApplyConfig() is called.
 * (other items were commented in a header).
 */
EXPORT_C void CSbcEncoderIntfcProxy::SetNumOfBlocks(TUint aNumOfBlocks)
    {
    iSbcEncConf.iNumOfBlocks = aNumOfBlocks;
    iSbcEncConf.iNumOfBlocksSet = ETrue;
    }

/**
 * CSbcEncoderIntfcProxy::GetNumOfBlocks
 * Returns current number of blocks commited by call to ApplyConfig().
 * (other items were commented in a header).
 */
EXPORT_C TInt CSbcEncoderIntfcProxy::GetNumOfBlocks(TUint& aNumOfBlocks)
    {
    TInt status = KErrNone;

    if (iHasBeenApplied)
        {
        aNumOfBlocks = iSbcEncConfCurrent.iNumOfBlocks;
        }
    else
        {
        status = KErrArgument;
        }

    return status;
    }

/**
 * CSbcEncoderIntfcProxy::SetAllocationMethod
 * Saves locally requested allocation method.
 * Change does not apply to the encoder until ApplyConfig() is called.
 * (other items were commented in a header).
 */
EXPORT_C void CSbcEncoderIntfcProxy::SetAllocationMethod(
                                     TSbcAllocationMethod aAllocationMethod)
    {
    iSbcEncConf.iAllocationMethod = aAllocationMethod;
    iSbcEncConf.iAllocationMethodSet = ETrue;
    }

/**
 * CSbcEncoderIntfcProxy::GetAllocationMethod
 * Returns current allocation method commited by call to ApplyConfig().
 * (other items were commented in a header).
 */
EXPORT_C TInt CSbcEncoderIntfcProxy::GetAllocationMethod(
                                     TSbcAllocationMethod& aAllocationMethod)
    {
    TInt status = KErrNone;

    if (iHasBeenApplied)
        {
        aAllocationMethod = iSbcEncConfCurrent.iAllocationMethod;
        }
    else
        {
        status = KErrArgument;
        }

    return status;
    }

/**
 * CSbcEncoderIntfcProxy::SetBitpoolSize
 * Saves locally requested bitpool range.
 * Change does not apply to the encoder until ApplyConfig() is called.
 * (other items were commented in a header).
 */
EXPORT_C void CSbcEncoderIntfcProxy::SetBitpoolSize(TUint aBitpoolSize)
    {
    iSbcEncConf.iBitpoolSize = aBitpoolSize;
    iSbcEncConf.iBitpoolSizeSet = ETrue;
    }

/**
 * CSbcEncoderIntfcProxy::GetBitpoolSize
 * Returns current bitpool range commited by call to ApplyConfig().
 * (other items were commented in a header).
 */
EXPORT_C TInt CSbcEncoderIntfcProxy::GetBitpoolSize(TUint& aBitpoolSize)
    {
    TInt status = KErrNone;

    if (iHasBeenApplied)
        {
        aBitpoolSize = iSbcEncConfCurrent.iBitpoolSize;
        }
    else
        {
        status = KErrArgument;
        }

    return status;
    }

/**
 * CSbcEncoderIntfcProxy::PopulateArrayL
 * Utility method that reads stream from 8-bit descriptor, converts it
 * to TUint data items and then copies them to the aArray.
 * (other items were commented in a header).
 */
void CSbcEncoderIntfcProxy::PopulateArrayL(RArray<TUint>& aArray,
                                           TPtr8 aPtr,
                                           TUint aCount)
    {
   	RDesReadStream stream(aPtr);
    CleanupClosePushL(stream);

   	for (TInt i = 0; i < aCount; i++)
      	{
   		aArray.AppendL(stream.ReadUint32L());
   		}

   	CleanupStack::PopAndDestroy(&stream);
    }

// ========================== OTHER EXPORTED FUNCTIONS =========================

// End of File
