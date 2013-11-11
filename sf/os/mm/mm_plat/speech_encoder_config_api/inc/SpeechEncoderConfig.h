/*
* Copyright (c) 2002-2006 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Used to configure the speech encoders.
*
*/


#ifndef CSPEECHENCODERCONFIG_H
#define CSPEECHENCODERCONFIG_H

// INCLUDES
#include <e32base.h>

// CONSTANTS
const TUid KUidSpeechEncoderConfig = {0x10207AB2};

// MACROS

// DATA TYPES

// FUNCTION PROTOTYPES

// FORWARD DECLARATIONS
class CMMFDevSound;
class CMdaAudioInputStream;

// CLASS DECLARATION

/**
*  Interface for configuring speech encoders.
*  Used to configure the speech encoders operting modes.
*  This abstract class just provides the static NewL function for the creation
*  of the proxy, and also defines the custom interface to be implemented by the
*  proxy and the real custom interface implementation.
*
*  @lib SpeechEncoderConfig.lib
*  @since S60 3.0
*/
class CSpeechEncoderConfig : public CBase
	{
    public:  // Constructors and destructor
        
        /**
        * Creates the interface.
        */
		IMPORT_C static CSpeechEncoderConfig* 
		                NewL(CMMFDevSound& aDevSound);

        /**
        * Creates the interface.
        */
		IMPORT_C static CSpeechEncoderConfig* 
		                NewL(CMdaAudioInputStream& aUtility);
        
    public: // New functions
        /**
        * Retrieves the bitrates supported by the encoder.
        * @since S60 3.0
        * @param aSupportedBitrates array returning the supported bitrates.
        * @return status
        */
		IMPORT_C virtual TInt GetSupportedBitrates(
		                      RArray<TUint>& aSupportedBitrates) = 0;

        /**
        * Sets the bitrate for encoding.
        * @since S60 3.0
        * @param aBitrate requested encoding bitrate.
        * @return status
        */
		IMPORT_C virtual TInt SetBitrate(TUint aBitrate) = 0;

        /**
        * Retrieves the current encode bitrate.
        * @since S60 3.0
        * @param aBitrate returns current encode bitrate.
        * @return status
        */
		IMPORT_C virtual TInt GetBitrate(TUint& aBitrate) = 0;

        /**
        * Enables and disables the voice activity (VAD) mode.
        * @since S60 3.0
        * @param aVadMode requested VAD mode.
        * @return status
        */
		IMPORT_C virtual TInt SetVadMode(TBool aVadMode) = 0;

        /**
        * Retrieves the current state of VAD mode.
        * @since S60 3.0
        * @param aVadMode returns current VAD mode.
        * @return status
        */
		IMPORT_C virtual TInt GetVadMode(TBool& aVadMode) = 0;
        
    public: // Functions from base classes
    protected:  // New functions
    protected:  // Functions from base classes
    private:

    public:     // Data
    protected:  // Data
    private:    // Data
    public:     // Friend classes
    protected:  // Friend classes
    private:    // Friend classes

	};

#endif      // CSPEECHENCODERCONFIG_H
            
// End of File
