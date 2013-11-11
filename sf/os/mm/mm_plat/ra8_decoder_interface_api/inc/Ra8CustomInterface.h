/*
* Copyright (c) 2002-2007 Nokia Corporation and/or its subsidiary(-ies). 
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


#ifndef RA8CUSTOMINTERFACE_H
#define RA8CUSTOMINTERFACE_H

// INCLUDES
#include <e32base.h>

// CONSTANTS
const TUid KUidRa8DecHwDeviceCI = {0x101FAF42};

// MACROS

// DATA TYPES
enum TRa8DecHwDeviceCommand
	{
	ERa8DecHwdFrameNumber
	};

// FUNCTION PROTOTYPES

// FORWARD DECLARATIONS
class CMMFDevSound;

// CLASS DECLARATION

/**
*  Interface for ra8 and sipro.
*  Used for providing initialization information for dsp decoder and getting.
*  information about played frames.
*  This abstract class just provides the static NewL function for the creation
*  of the proxy, and also defines the custom interface to be implemented by the
*  proxy and the real custom interface implementation.
*
*/
class CRa8CustomInterface : public CBase
	{
    public:  // Constructors and destructor

        /**
        * Creates the interface.
        */
		IMPORT_C static CRa8CustomInterface* NewL(CMMFDevSound& aDevSound);

    public: // New functions
		/**
		* CRa8CustomInterface::FrameNumber
		* is used get the current number of audio frames played.
		* @return status
		*/
		virtual TInt FrameNumber() = 0;
		/**
		* CRa8CustomInterface::SetInitString
		* is used to configure the initialization parameters for task creation.
		* @param aInitString contains the initialization information for the task.
		* @return status
		*/
		virtual TInt SetInitString(TDesC8& aInitString) = 0;

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

#endif      // RA8CUSTOMINTERFACE_H

// End of File
