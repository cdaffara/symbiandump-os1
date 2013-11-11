/*
* Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:
*
*/



#ifndef CPOSFILEHANDLER_H
#define CPOSFILEHANDLER_H

//  INCLUDES
#include <s32file.h>
#include <badesca.h>
#include "EPos_SimPsyConstants.h"

/**
 *  A helper class to access files and read from files.
 */
class CPosFileHandler : public CBase
    {
    public: // Constructors and destructor

        /**
        * Two-phased constructor.
        *
        * @param aFileName The name of the file to read from.
        */
        static CPosFileHandler* NewL(
        /* IN  */       const TDesC& aFileName
        );

        /**
        * Destructor.
        */
        virtual ~CPosFileHandler();

    public: // New functions

        /**
        * Reads one line from file (asynchronously)
        *
        * @param aDes A buffer to store the incoming data.
        *
        * @return System wide error code.
        */
        TInt Read(
        /* OUT */   TDes8&              aDes
        );

        /**
        * Populates the incoming array with simulation data.
        *
        * @param aArray An array to store the simulation data.
        */
        void ReadL(
        /* OUT */   CDesC8Array& aArray
        );

    private:

        /**
        * C++ default constructor.
        */
        CPosFileHandler();

        /**
        * EPOC constructor.
        *
        * @param aFileName The name of the file to read from.
        */
        void ConstructL(
        /* IN  */       const TDesC& aFileName
        );

        /**
        * Resets the file handler to read from the start of the file.
        */
        TInt ReadFromStart();

        // Prohibit copy constructor
        CPosFileHandler( const CPosFileHandler& );
        // Prohibit assigment operator
        CPosFileHandler& operator= ( const CPosFileHandler& );

    private:    // Data
        //Handle to file server session
        RFs iFsSession;

        //Instance of RFile. Creating, opening and operating on files.
        RFile iFile;

        //Instance of RFileReadStream. Reading of a stream from file.
        RFileReadStream iStream;

        HBufC* iFileName;
    };

#endif      // CPOSFILEHANDLER_H

// End of File
