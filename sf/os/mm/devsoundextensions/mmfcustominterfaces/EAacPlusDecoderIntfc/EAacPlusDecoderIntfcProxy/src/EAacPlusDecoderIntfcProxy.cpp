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
* Description:   Interface proxy for Enhanced AAC+ decoder configuration CI.
*
*/



// INCLUDE FILES
#include <CustomCommandUtility.h>
#include <CustomInterfaceUtility.h>
#include "EAacPlusDecoderIntfcMsgs.h"
#include "EAacPlusDecoderIntfcProxy.h"

// EXTERNAL DATA STRUCTURES

// EXTERNAL FUNCTION PROTOTYPES

// CONSTANTS
const CEAacPlusDecoderIntfc::TAudioObjectType
            KAacAudioObjectTypeINVALID = CEAacPlusDecoderIntfc::ENull;
const TInt  KAacInputSampleRateINVALID = 0;
const TInt  KAacNumOfChannelsINVALID   = 0;

// MACROS

// LOCAL CONSTANTS AND MACROS

// MODULE DATA STRUCTURES

// LOCAL FUNCTION PROTOTYPES

// FORWARD DECLARATIONS

// ============================= LOCAL FUNCTIONS ===============================

// ============================= MEMBER FUNCTIONS ==============================

/**
 * CEAacPlusDecoderIntfcProxy::CEAacPlusDecoderIntfcProxy
 * C++ default constructor can NOT contain any code, that
 * might leave.
 */
CEAacPlusDecoderIntfcProxy::CEAacPlusDecoderIntfcProxy(
                            TMMFMessageDestinationPckg aMessageHandler,
                            MCustomCommand& aCustomCommand,
                            CCustomInterfaceUtility* aCustomInterfaceUtility) :
    iCustomCommand(aCustomCommand),
    iMessageHandler(aMessageHandler),
    iCustomInterfaceUtility(aCustomInterfaceUtility)
    {
    }

/**
 * CEAacPlusDecoderIntfcProxy::ConstructL
 * Symbian 2nd phase constructor can leave.
 */
void CEAacPlusDecoderIntfcProxy::ConstructL()
    {
    iAudioObjectType        = KAacAudioObjectTypeINVALID;
    iInputSamplingFrequency = KAacInputSampleRateINVALID;
    iNumOfChannels          = KAacNumOfChannelsINVALID;
    iSbrEnabled             = EFalse;
    iDsmEnabled             = EFalse;
    iHasBeenApplied         = EFalse;
    }


/**
 * CEAacPlusDecoderIntfcProxy::NewL
 * Two-phased constructor.
 */
EXPORT_C CEAacPlusDecoderIntfcProxy* CEAacPlusDecoderIntfcProxy::NewL(
                              TMMFMessageDestinationPckg aMessageHandler,
                              MCustomCommand& aCustomCommand,
                              CCustomInterfaceUtility* aCustomInterfaceUtility)
    {
    CEAacPlusDecoderIntfcProxy* self = new(ELeave) CEAacPlusDecoderIntfcProxy(
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
EXPORT_C CEAacPlusDecoderIntfcProxy::~CEAacPlusDecoderIntfcProxy()
    {
    iCustomInterfaceUtility->RemoveCustomInterface(iMessageHandler);
    delete iCustomInterfaceUtility;
    }


// From CEAacPlusDecoderIntfc

/**
 * Configures decoder's profile defined by TAudioObjectType.
 * (other items defined in the header)
 */
EXPORT_C void CEAacPlusDecoderIntfcProxy::SetAudioObjectType(
                                          TAudioObjectType aAudioObjectType)
	{
	iAudioObjectType = aAudioObjectType;
	}

/**
 * Configures decoder's input sampling frequency.
 * (other items defined in the header)
 */
EXPORT_C void CEAacPlusDecoderIntfcProxy::SetInputSamplingFrequency(
                                          TUint aInputSamplingFrequency)
    {
    iInputSamplingFrequency = aInputSamplingFrequency;
    }

/**
 * Configures decoder's channel settings.
 * (other items defined in the header)
 */
EXPORT_C void CEAacPlusDecoderIntfcProxy::SetNumOfChannels(
                                          TUint aNumOfChannels)
    {
    iNumOfChannels = aNumOfChannels;
    }

/**
 * Configures decoder's SBR settings.
 * (other items defined in the header)
 */
EXPORT_C void CEAacPlusDecoderIntfcProxy::SetSbr(TBool aSbrEnabled)
    {
    iSbrEnabled = aSbrEnabled;
    }

/**
 * Configures decoder's DSM settings.
 * (other items defined in the header)
 */
EXPORT_C void CEAacPlusDecoderIntfcProxy::SetDownSampledMode(TBool aDsmEnabled)
    {
    iDsmEnabled = aDsmEnabled;
    }

/**
 * Returns decoder's AAC profile settings.
 * (other items defined in the header)
 */
EXPORT_C TInt CEAacPlusDecoderIntfcProxy::GetAudioObjectType(
                                          TAudioObjectType& aAudioObjectType)
	{
    TInt status = KErrNone;

    if (iHasBeenApplied)
        {
    	aAudioObjectType = iCurrentAudioObjectType;
        }
    else
        {
        status = KErrArgument;
        }

    return status;
	}

/**
 * Returns decoder's input sampling frequency settings.
 * (other items defined in the header)
 */
EXPORT_C TInt CEAacPlusDecoderIntfcProxy::GetInputSamplingFrequency(
                                          TUint& aInputSamplingFrequency)
    {
    TInt status = KErrNone;

    if (iHasBeenApplied)
        {
    	aInputSamplingFrequency = iCurrentInputSamplingFrequency;
        }
    else
        {
        status = KErrArgument;
        }

    return status;
    }

/**
 * Returns decoder's channel settings.
 * (other items defined in the header)
 */
EXPORT_C TInt CEAacPlusDecoderIntfcProxy::GetNumOfChannels(
                                          TUint& aNumOfChannels)
    {
    TInt status = KErrNone;

    if (iHasBeenApplied)
        {
    	aNumOfChannels = iCurrentNumOfChannels;
        }
    else
        {
        status = KErrArgument;
        }

    return status;
    }

/**
 * Returns decoder's SBR settings.
 * (other items defined in the header)
 */
EXPORT_C TInt CEAacPlusDecoderIntfcProxy::GetSbr(TBool& aSbrEnabled)
    {
    TInt status = KErrNone;

    if (iHasBeenApplied)
        {
    	aSbrEnabled = iCurrentSbrEnabled;
        }
    else
        {
        status = KErrArgument;
        }

    return status;
    }

/**
 * Returns decoder's DSM settings.
 * (other items defined in the header)
 */
EXPORT_C TInt CEAacPlusDecoderIntfcProxy::GetDownSampledMode(TBool& aDsmEnabled)
    {
    TInt status = KErrNone;

    if (iHasBeenApplied)
        {
    	aDsmEnabled = iCurrentDsmEnabled;
        }
    else
        {
        status = KErrArgument;
        }

    return status;
    }

/**
 * Applies configuration settings to the decoder.
 * (other items defined in the header)
 */
EXPORT_C TInt CEAacPlusDecoderIntfcProxy::ApplyConfig()
    {
    TInt status = KErrNone;

    if (!iHasBeenApplied)
        {
        // Make sure Set() methods for these params were called at least once
        if (iAudioObjectType == KAacAudioObjectTypeINVALID ||
            iInputSamplingFrequency == KAacInputSampleRateINVALID ||
            iNumOfChannels == KAacNumOfChannelsINVALID)
            {
            status = KErrArgument;
            }
        }

    if (status == KErrNone)
        {
        TEAacPlusDecoderConfig configParams;

    	configParams.iAudioObjectType        = iAudioObjectType;
    	configParams.iInputSamplingFrequency = iInputSamplingFrequency;
    	configParams.iNumOfChannels          = iNumOfChannels;
    	configParams.iSbrEnabled             = iSbrEnabled;
    	configParams.iDsmEnabled             = iDsmEnabled;

    	TPckgBuf<TEAacPlusDecoderConfig> pckgBuf(configParams);
    	TInt status = iCustomCommand.CustomCommandSync(iMessageHandler,
    	                                               EEaacpdimApplyConfig,
    	                                               pckgBuf,
    	                                               KNullDesC8);
    	if (status == KErrNone)
    		{
            iCurrentAudioObjectType        = iAudioObjectType;
            iCurrentInputSamplingFrequency = iInputSamplingFrequency;
            iCurrentNumOfChannels          = iNumOfChannels;
            iCurrentSbrEnabled             = iSbrEnabled;
            iCurrentDsmEnabled             = iDsmEnabled;
    		iHasBeenApplied = ETrue;
    		}
        }

	return status;
    }

// End of File
