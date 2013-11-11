/*
* Copyright (c) 2005 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Defined messages for SBC encoder interface.
*
*/



#ifndef SBCENCODERINTFCMSGS_H
#define SBCENCODERINTFCMSGS_H

// INCLUDES
#include <SbcEncoderIntfc.h>

// CONSTANTS

// MACROS

// DATA TYPES
class TSbcEncoderBitpoolRange
    {
    public:

    TUint iMinSupportedBitpoolSize;
    TUint iMaxSupportedBitpoolSize;
    };

/**
 * Structure defining SBC encoder configuration data
 */
class TSbcEncoderConfig
	{
	public:
	TUint iSamplingFrequency;
	CSbcEncoderIntfc::TSbcChannelMode iChannelMode;
	TUint iNumOfSubbands;
	TUint iNumOfBlocks;
	CSbcEncoderIntfc::TSbcAllocationMethod iAllocationMethod;
	TUint iBitpoolSize;
	};

/**
 * Supported interface messages
 */
enum TSbcEncoderIntfcMsgs
	{
	ESbceimGetNumOfSupportedSamplingFrequencies,
	ESbceimGetSupportedSamplingFrequencies,
	ESbceimGetNumOfSupportedChannelModes,
	ESbceimGetSupportedChannelModes,
    ESbceimGetNumOfSupportedBlocks,
    ESbceimGetSupportedBlocks,
	ESbceimGetNumOfSupportedNumOfSubbands,
	ESbceimGetSupportedNumOfSubbands,
	ESbceimGetNumOfSupportedAllocationMethods,
	ESbceimGetSupportedAllocationMethods,
	ESbceimGetSupportedBitpoolRange,
	ESbceimApplyConfig
	};

// FUNCTION PROTOTYPES

// FORWARD DECLARATIONS

// CLASS DECLARATION

#endif      // SBCENCODERINTFCMSGS_H

// End of File
