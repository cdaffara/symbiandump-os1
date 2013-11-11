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
* Description:  Used for interface to G711 encoder.
*
*/


#ifndef CG711ENCODERINTFC_H
#define CG711ENCODERINTFC_H

// INCLUDES
#include <e32base.h>

// CONSTANTS
const TUid KUidG711EncoderIntfc = {0x10207AFF};

// MACROS

// DATA TYPES

// FUNCTION PROTOTYPES

// FORWARD DECLARATIONS
class CMMFDevSound;
class CMdaAudioInputStream;

// CLASS DECLARATION

/**
*  Interface for G711 encoder.
*  This abstract class just provides the static NewL function for the creation
*  of the proxy, and also defines the custom interface to be implemented by the
*  proxy and the real custom interface implementation.
*
*  @lib G711EncoderIntfc.lib
*  @since S60 3.0
*/
class CG711EncoderIntfc : public CBase
	{
    // DATA TYPES
    public:
        enum TEncodeMode
            {
            EEncALaw,
            EEncULaw
            };

    public:  // Constructors and destructor
        
        /**
        * Creates the interface.
        */
		IMPORT_C static CG711EncoderIntfc* NewL(CMMFDevSound& aDevSound);

        /**
        * Creates the interface.
        */
		IMPORT_C static CG711EncoderIntfc* NewL(CMdaAudioInputStream& aUtility);
        
    public: // New functions
    
        /**
        * Used to change the encoding mode.
        * @since S60 3.0
        * @param aEncodeMode requested encode mode.
        * @return status
        */
		IMPORT_C virtual TInt SetEncoderMode(TEncodeMode aEncodeMode) = 0;
		
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

#endif      // CG711ENCODERINTFC_H
            
// End of File
