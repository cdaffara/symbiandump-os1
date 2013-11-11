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
* Description:  Used to configure the AAC decoder.
*
*/


#ifndef CAACDECODERCONFIG_H
#define CAACDECODERCONFIG_H

// INCLUDES
#include <e32base.h>
#include <AudioConfig.h>

// CONSTANTS
const TUid KUidAacDecoderConfig = {0x10207AAE};

// MACROS

// DATA TYPES

// FUNCTION PROTOTYPES

// FORWARD DECLARATIONS
class CMMFDevSound;
class CMdaAudioOutputStream;

// CLASS DECLARATION

/**
*  Interface for sending configuration info to AAC decoder.
*  This abstract class just provides the static NewL function for the creation
*  of the proxy, and also defines the custom interface to be implemented by the
*  proxy and the real custom interface implementation.
*
*  @lib AacDecoderConfig.lib
*  @since S60 3.0
*/
class CAacDecoderConfig : public CBase
	{
    public:  // Constructors and destructor
        
        /**
        * Creates the interface.
        */
		IMPORT_C static CAacDecoderConfig* 
		                NewL(CMMFDevSound& aDevSound);
        
        /**
        * Creates the interface.
        */
		IMPORT_C static CAacDecoderConfig* 
		                NewL(CMdaAudioOutputStream& aUtility);

    public: // New functions
        /**
        * Configures the decoder for decoding raw AAC data.
        * @since S60 3.0
        * @param aAudioConfig stucture defining configuration information.
        * @return status
        */
		IMPORT_C virtual TInt SetAudioConfig(TAudioConfig& aAudioConfig) = 0;
        
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

#endif      // CAACDECODERCONFIGCI_H
            
// End of File
