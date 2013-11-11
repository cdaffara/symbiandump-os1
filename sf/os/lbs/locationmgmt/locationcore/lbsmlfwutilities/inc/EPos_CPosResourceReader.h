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



#ifndef CPOSRESOURCEREADER_H
#define CPOSRESOURCEREADER_H

//  INCLUDES
#include <e32base.h>
#include <barsc.h>
#include <barsread.h>

// CONSTANTS

_LIT(KPosResourceFilePathFormat, "%c:%s");

// CLASS DECLARATION

/**
*  Simplified interface to resource files
*/
class CPosResourceReader : public CBase
    {
    private:    // Constructors and destructor

        /**
        * C++ default constructor.
        */
        CPosResourceReader();

    public:     // Constructors and destructor

        /**
        * Two-phased constructor.
        * @param aPath Path to the resource file (excluding drive)
        * @return
        */
        IMPORT_C static CPosResourceReader* NewLC(
            /* IN  */   const TDesC& aPath
            );

        /**
        * Destructor.
        */
        IMPORT_C virtual ~CPosResourceReader();

    private:

        /**
        * By default EPOC constructor is private.
        * @param aPath Path to the resource file (excluding drive)
        */
        void ConstructL(
            /* IN  */   const TDesC& aPath
            );

        // By default, prohibit copy constructor
        CPosResourceReader( const CPosResourceReader& );
        // Prohibit assigment operator
        CPosResourceReader& operator= ( const CPosResourceReader& );

    public:     // New functions

        /**
        * Reads a string from the currently loaded resource
        * @param aResourceId    Resource id to read from. The function leaves
        *                       if there is no matching resource or the offset
        *                       does not match.
        * @return a heap-allocated string
        */
        IMPORT_C HBufC* ReadHBufCL(
            /* IN  */      TInt aResourceId
            );

		/**
        * Reads an TInt/integer from the currently loaded resource
        * @param aResourceId    Resource id to read from. The function leaves
        *                       if there is no matching resource or the offset
        *                       does not match.
        * @return an integer
        */
        IMPORT_C TInt ReadInt8L(
            /* IN  */      TInt aResourceId
            );

		/**
        * Reads an TInt/integer from the currently loaded resource
        * @param aResourceId    Resource id to read from. The function leaves
        *                       if there is no matching resource or the offset
        *                       does not match.
        * @return an integer
        */
        IMPORT_C TInt ReadInt16L(
            /* IN  */      TInt aResourceId
            );

		/**
        * Reads an TInt/integer from the currently loaded resource
        * @param aResourceId    Resource id to read from. The function leaves
        *                       if there is no matching resource or the offset
        *                       does not match.
        * @return an integer
        */
        IMPORT_C TInt ReadInt32L(
            /* IN  */      TInt aResourceId
            );

    protected: // New functions

        /**
        * Loads a resource from the resource file into the buffer
        * @param aResourceId Id of the resource to be loaded
        */
        IMPORT_C void LoadResourceL(TInt aResourceId);


    private:    // Data
        RFs             iFileSession;
        RResourceFile   iResourceFile;
        HBufC8*         iResourceBuffer;
        TResourceReader iResourceReader;
    };

#endif      // CPOSRESOURCEREADER_H

// End of File
