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
* Description:  Used for interface to G729 decoder.
*
*/


#ifndef CG729DECODERINTFC_H
#define CG729DECODERINTFC_H

// INCLUDES
#include <e32base.h>

// CONSTANTS
const TUid KUidG729DecoderIntfc = {0x10207ABE};

// MACROS

// DATA TYPES

// FUNCTION PROTOTYPES

// FORWARD DECLARATIONS
class CMMFDevSound;
class CMdaAudioOutputStream;

// CLASS DECLARATION

/**
*  Interface for G729 decoder.
*  This abstract class just provides the static NewL function for the creation
*  of the proxy, and also defines the custom interface to be implemented by the
*  proxy and the real custom interface implementation.
*
*  @lib G729DecoderIntfc.lib
*  @since S60 3.0
*/
class CG729DecoderIntfc : public CBase
	{
    public:  // Constructors and destructor
        
        /**
        * Creates the interface.
        */
		IMPORT_C static CG729DecoderIntfc* 
		                NewL(CMMFDevSound& aDevSound);

        /**
        * Creates the interface.
        */
		IMPORT_C static CG729DecoderIntfc* 
		                NewL(CMdaAudioOutputStream& aUtility);
        
    public: // New functions
		IMPORT_C virtual TInt BadLsfNextBuffer() = 0;

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

#endif      // CG729DECODERINTFC_H
            
// End of File
