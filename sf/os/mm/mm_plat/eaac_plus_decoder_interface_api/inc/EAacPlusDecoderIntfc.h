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
* Description:  Project specification for Enahanced AAC+ decoder 
*              : configuration Custom Interface.
*
*/


#ifndef EAACPLUSDECODERINTFC_H
#define EAACPLUSDECODERINTFC_H

// INCLUDES
#include <e32base.h>

// CONSTANTS
const TUid KUidEAacPlusDecoderIntfc = {0x10207B1F};

// MACROS

// DATA TYPES

// FUNCTION PROTOTYPES

// FORWARD DECLARATIONS
class CMMFDevSound;
class CMdaAudioOutputStream;

// CLASS DECLARATION

/**
*  Interface for sending configuration info to Enhanced AAC+ decoder.
*  This abstract class just provides the static NewL function for the 
*  creation of the proxy. Also, it defines the APIs to be implemented 
*  by the proxy and the real custom interface implementation.
*
*  @lib EAacPlusDecoderIntfc.lib
*  @since S60 3.0
*/
class CEAacPlusDecoderIntfc : public CBase
    {
    public: //data types

    enum TAudioObjectType
        {
        ENull    = 0,
        EAacLc   = 2,
        EAacLtp  = 4,
        };

    public:  // Constructors and destructor

        /**
        * Creates the interface.
        */
		IMPORT_C static CEAacPlusDecoderIntfc* 
		                NewL(CMMFDevSound& aDevSound);

        /**
        * Creates the interface.
        */
		IMPORT_C static CEAacPlusDecoderIntfc* 
		                NewL(CMdaAudioOutputStream& aUtility);

    public: // New functions

        /**
        * Configures decoder's profile defined by TAudioObjectType.
        * @since S60 3.0
        * @param TAudioObjectType - Supported AAC profile.
        * @return void
        */
		IMPORT_C virtual void SetAudioObjectType(
		                      TAudioObjectType aAudioObjectType) = 0;

        /**
        * Configures decoder's input sampling frequency.
        * @since S60 3.0
        * @param TUint - Decoder's input sampling frequency value.
        * @return void
        */
		IMPORT_C virtual void SetInputSamplingFrequency(
		                      TUint aInputSamplingFrequency) = 0;

        /**
        * Configures the number of channels to be used by the decoder.
        * @since S60 3.0
        * @param TUint - Number of channels (stereo/mono).
        * @return void
        */
		IMPORT_C virtual void SetNumOfChannels(TUint aNumOfChannels) = 0;

        /**
        * Configures decoder's spectral bandwidth replication (SBR).
        * @since S60 3.0
        * @param TBool - Toggle SBR.
        * @return void
        */
		IMPORT_C virtual void SetSbr(TBool aSbrEnabled) = 0;

        /**
        * Configures decoder's down-sampled mode (DSM).
        * @since S60 3.0
        * @param TBool - Toggle DSM.
        * @return void
        */
		IMPORT_C virtual void SetDownSampledMode(TBool aDsmEnabled) = 0;

        /**
        * Commits configuration settings to the decoder.
        * @since S60 3.0
        * @return TInt - Status
        */
		IMPORT_C virtual TInt ApplyConfig() = 0;

        /**
        * Returns decoder's AAC profile setting.
        * @since S60 3.0
        * @param TAudioObjectType& - Reference to AAC profile.
        * @return TInt - Status
        */
		IMPORT_C virtual TInt GetAudioObjectType(
		                      TAudioObjectType& aAudioObjectType) = 0;

        /**
        * Returns decoder's input sampling frequency setting. 
        * @since S60 3.0
        * @param TUint& - Reference to input sampling frequency.
        * @return TInt - Status
        */
		IMPORT_C virtual TInt GetInputSamplingFrequency(TUint& aInputSamplingFrequency) = 0;

        /**
        * Returns decoder's channel setting.
        * @since S60 3.0
        * @param TUint& - Reference to the number of channels.
        * @return TInt - Status
        */
		IMPORT_C virtual TInt GetNumOfChannels(TUint& aNumOfChannels) = 0;

        /**
        * Returns decoder's SBR setting.
        * @since S60 3.0
        * @param TBool& - Reference to the SBR setting.
        * @return TInt - Status
        */
		IMPORT_C virtual TInt GetSbr(TBool& aSbrEnabled) = 0;

        /**
        * Returns decoder's DSM setting.
        * @since S60 3.0
        * @param TBool& - Reference to the DSM setting.
        * @return TInt - Status
        */
		IMPORT_C virtual TInt GetDownSampledMode(TBool& aDsmEnabled) = 0;

	};

#endif      // EAACPLUSDECODERINTFC_H

// End of File
