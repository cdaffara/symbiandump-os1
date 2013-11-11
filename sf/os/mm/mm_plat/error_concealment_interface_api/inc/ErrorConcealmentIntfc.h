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
* Description:  Used for error concealment.
*
*/


#ifndef CERRORCONCEALMENTINTFC_H
#define CERRORCONCEALMENTINTFC_H

// INCLUDES
#include <e32base.h>

// CONSTANTS
const TUid KUidErrorConcealmentIntfc = {0x10207AB6};

// MACROS

// DATA TYPES

// FUNCTION PROTOTYPES

// FORWARD DECLARATIONS
class CMMFDevSound;
class CMdaAudioOutputStream;

// CLASS DECLARATION

/**
*  Interface for error concealment.
*  Used for providing error concealment for lost data.
*  This abstract class just provides the static NewL function for the creation
*  of the proxy, and also defines the custom interface to be implemented by the
*  proxy and the real custom interface implementation.
*
*  @lib ErrorConcealmentIntfc.lib
*  @since S60 3.0
*/
class CErrorConcealmentIntfc : public CBase
	{
    public:  // Constructors and destructor
        
        /**
        * Creates the interface.
        */
		IMPORT_C static CErrorConcealmentIntfc* 
		                NewL(CMMFDevSound& aDevSound);

        /**
        * Creates the interface.
        */
		IMPORT_C static CErrorConcealmentIntfc*
		                NewL(CMdaAudioOutputStream& aUtility);
        
    public: // New functions
        /**
        * Indicates that an frame of encoded audio is lost and that error
        * concealment should be performed for a lost frame.
        * By definition, the next buffer sent to the decoder will contain no
        * valid audio data.
        * @since S60 3.0
        * @param na
        * @return status
        */
		IMPORT_C virtual TInt ConcealErrorForNextBuffer() = 0;

        /**
        * Puts the hwdevice into a frame-based interface.
        * The alternate mode of interface would be buffer-based.
        * @since S60 3.0
        * @param aFrameMode
        * @return status
        */
		IMPORT_C virtual TInt SetFrameMode(TBool aFrameMode) = 0;

        /**
        * Query to find out if frame-based interface is required by hwdevice
        * for supporting error concealment.
        * @since S60 3.0
        * @param aFrameModeRqrd indicates if frame mode is required.
        * @return status
        */
		IMPORT_C virtual TInt FrameModeRqrdForEC(TBool& aFrameModeRqrd) = 0;
        
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

#endif      // CERRORCONCEALMENTINTFC_H
            
// End of File
