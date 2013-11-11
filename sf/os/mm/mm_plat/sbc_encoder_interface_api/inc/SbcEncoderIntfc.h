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
* Description:  Interface definition for BT SBC Encoder configuration.
*
*/


#ifndef CSBCENCODERINTFC_H
#define CSBCENCODERINTFC_H

// INCLUDES
#include <e32base.h>

// CONSTANTS
const TUid KUidSbcEncoderIntfc = {0x10207B24};

// MACROS

// DATA TYPES

// FUNCTION PROTOTYPES

// FORWARD DECLARATIONS
class CMMFDevSound;
class CMdaAudioInputStream;

// CLASS DECLARATION

/**
*  Interface for configuring the SBC encoder.
*  Used to configure the SBC encoder operating modes.
*  This abstract class just provides the static NewL function for the creation
*  of the proxy, and also defines the custom interface to be implemented by the
*  proxy and the real custom interface implementation.
*
*  @lib SbcEncoderIntfc.lib
*  @since S60 3.0
*/
class CSbcEncoderIntfc : public CBase
	{
    public: // Data

    // Supported channel modes
    enum TSbcChannelMode
        {
        ESbcChannelMono,
        ESbcChannelDual,
        ESbcChannelStereo,
        ESbcChannelJointStereo
        };

    // Supported allocation methods
    enum TSbcAllocationMethod
        {
        ESbcAllocationSNR,
        ESbcAllocationLoudness
        };

    public:  // Constructors and destructor

        /**
        * Creates the interface.
        */
		IMPORT_C static CSbcEncoderIntfc* NewL(CMMFDevSound& aDevSound);

        /**
        * Creates the interface.
        */
		IMPORT_C static CSbcEncoderIntfc* NewL(CMdaAudioInputStream& aUtility);

    public: // New functions
        /**
        * Retrieves the sampling frequencies supported by the encoder.
        * @since S60 3.0
        * @param aSupportedSamplingFrequencies array returning the supported
        *        frequencies.
        * @return TInt status
        */
		IMPORT_C virtual TInt GetSupportedSamplingFrequencies(
		                      RArray<TUint>& aSupportedSamplingFrequencies) = 0;

        /**
        * Retrieves the channel configurations supported by the encoder.
        * @since S60 3.0
        * @param aSupportedChannelModes array returning the supported
        *        frequencies.
        * @return TInt status
        */
		IMPORT_C virtual TInt GetSupportedChannelModes(
		                      RArray<TSbcChannelMode>&
		                      aSupportedChannelModes) = 0;

        /**
        * Retrieves array of supported block numbers by the encoder.
        * @since S60 3.0
        * @param aNumBlocks array returning block numbers.
        * @return TInt status
        */
		IMPORT_C virtual TInt GetSupportedNumOfBlocks(
		                      RArray<TUint>& aSupportedNumOfBlocks) = 0;

        /**
        * Retrieves the number of subbands supported by the encoder.
        * @since S60 3.0
        * @param aSupportedNumOfSubbands array returning the supported number
        *        of subbands.
        * @return TInt status
        */
		IMPORT_C virtual TInt GetSupportedNumOfSubbands(
		                      RArray<TUint>& aSupportedNumOfSubbands) = 0;

        /**
        * Retrieves the allocation methods supported by the encoder.
        * @since S60 3.0
        * @param aSupportedAllocationMethods array returning the supported
        *        allocation methods.
        * @return TInt status
        */
		IMPORT_C virtual TInt GetSupportedAllocationMethods(
		                      RArray<TSbcAllocationMethod>&
		                      aSupportedAllocationMethods) = 0;

        /**
        * Retrieves the supported bitpool range by the encoder.
        * @since S60 3.0
        * @param aMinSupportedBitpoolSize returning the min bitpool size.
        * @param aMaxSupportedBitpoolSize returning the max bitpool size.
        * @return TInt status
        */
		IMPORT_C virtual TInt GetSupportedBitpoolRange(
		                      TUint& aMinSupportedBitpoolSize,
		                      TUint& aMaxSupportedBitpoolSize) = 0;

        /**
        * Sets the sampling frequency for encoding.
        * @since S60 3.0
        * @param aSamplingFrequency requested encoding sampling frequency.
        * @return void
        */
		IMPORT_C virtual void SetSamplingFrequency(
		                      TUint aSamplingFrequency) = 0;

        /**
        * Retrieves the current encode sampling frequency.
        * @since S60 3.0
        * @param aSamplingFrequency returns current encode sampling frequency.
        * @return TInt status
        */
		IMPORT_C virtual TInt GetSamplingFrequency(
		                      TUint& aSamplingFrequency) = 0;

        /**
        * Sets the channel mode for encoding.
        * @since S60 3.0
        * @param aChannelMode requested encoding channel mode.
        * @return void
        */
		IMPORT_C virtual void SetChannelMode(
		                      TSbcChannelMode aChannelMode) = 0;

        /**
        * Retrieves the current encode channel mode.
        * @since S60 3.0
        * @param aChannelMode returns current encode channel mode.
        * @return TInt status
        */
		IMPORT_C virtual TInt GetChannelMode(
		                      TSbcChannelMode& aChannelMode) = 0;

        /**
        * Sets the number of subbands for encoding.
        * @since S60 3.0
        * @param aNumofSubbands requested encoding number of subbands.
        * @return void
        */
		IMPORT_C virtual void SetNumOfSubbands(TUint aNumOfSubbands) = 0;

        /**
        * Retrieves the current encode number of subbands.
        * @since S60 3.0
        * @param aNumOfSubbands returns current encode number of subbands.
        * @return TInt status
        */
		IMPORT_C virtual TInt GetNumOfSubbands(TUint& aNumOfSubbands) = 0;

        /**
        * Sets the number of blocks for encoding.
        * @since S60 3.0
        * @param aNumOfBlocks requested encoding number of blocks.
        * @return void
        */
		IMPORT_C virtual void SetNumOfBlocks(TUint aNumOfBlocks) = 0;

        /**
        * Retrieves the current encode number of blocks.
        * @since S60 3.0
        * @param aNumOfBlocks returns current encode number of blocks.
        * @return TInt status
        */
		IMPORT_C virtual TInt GetNumOfBlocks(TUint& aNumOfBlocks) = 0;

        /**
        * Sets the allocation method for encoding.
        * @since S60 3.0
        * @param aAllocationMethod requested encoding allocation method.
        * @return void
        */
		IMPORT_C virtual void SetAllocationMethod(
		                      TSbcAllocationMethod aAllocationMethod) = 0;

        /**
        * Retrieves the current encode allocation method.
        * @since S60 3.0
        * @param aAllocationMethod returns current encode allocation method.
        * @return TInt status
        */
		IMPORT_C virtual TInt GetAllocationMethod(
		                      TSbcAllocationMethod& aAllocationMethod) = 0;

        /**
        * Sets the bitpool size for encoding.
        * @since S60 3.0
        * @param aBitpoolSize requested encoding number of blocks.
        * @return void
        */
		IMPORT_C virtual void SetBitpoolSize(TUint aBitpoolSize) = 0;

        /**
        * Retrieves the current encode bitpool size.
        * @since S60 3.0
        * @param aBitpoolSize returns current encode bitpool size.
        * @return TInt status
        */
		IMPORT_C virtual TInt GetBitpoolSize(TUint& aBitpoolSize) = 0;

        /**
        * Applies configuration to the encoder.
        * @since S60 3.0
        * @return TInt - status
        */
		IMPORT_C virtual TInt ApplyConfig() = 0;

	};

#endif      // CSBCENCODERINTFC_H

// End of File
