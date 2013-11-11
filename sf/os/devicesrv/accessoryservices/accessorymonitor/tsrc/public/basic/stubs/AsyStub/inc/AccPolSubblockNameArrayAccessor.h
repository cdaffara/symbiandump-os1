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
* Description:  A private accessor API for the Subblock Name Array for 
*                   Accessory Server and -Policy internal use only.
*
*/



#ifndef TACCPOLSUBBLOCKNAMEARRAYACCESSOR_H
#define TACCPOLSUBBLOCKNAMEARRAYACCESSOR_H

//  INCLUDES
#include "accpolsubblocknamearray.h"

// CONSTANTS

// MACROS

// DATA TYPES

// FUNCTION PROTOTYPES

// FORWARD DECLARATIONS

// CLASS DECLARATION

/**
*  A private accessor API for the Subblock Name Array.
*
*  @lib AccPolicy.lib
*  @since S60 3.1
*/
class TAccPolSubblockNameArrayAccessor
    {
    public:  // Constructors and destructor

    public: // New functions

        /**
        * Append name to subblock name array.
        *
        * @since S60 3.1
        * @param aArray Array for capabilities.
        * @param aName  Capability name which is stored to database.
        * @return void
        */
        IMPORT_C static void AppendNameL( CAccPolSubblockNameArray& aArray, 
                                          const TUint32& aName );
        
        /**
        * Reset (clear) the array.
        *
        * @since S60 3.1
        * @param aArray Array of capabilities.
        * @return void
        */
        IMPORT_C static void ResetArray( CAccPolSubblockNameArray& aArray );

        /**
        * Get count of name definitions in the array.
        *
        * @since S60 3.1
        * @param aArray Array of capabilities.
        * @return Count of capabilities.
        */
        IMPORT_C static TInt GetCount( const CAccPolSubblockNameArray& aArray );


        /**
        * Get a certain capability from the array.
        *
        * @since S60 3.1
        * @param aArray Array of capabilities.
        * @param aIndex Array index.
        * @param aName  On succesfull return, contains the capability.
        * @return Lenght/Size of capability.
        */
        IMPORT_C static void GetName( const CAccPolSubblockNameArray& aArray, 
                                      TInt aIndex, 
                                      TUint32& aName );

        /**
        * Get a pointer to the member array of a name array instance.
        *
        * @since S60 3.1
        * @param aArray Array of capabilities.
        * @return Pointer to capability array.
        */
        IMPORT_C static RArray<TUint32>* Array( CAccPolSubblockNameArray* aArray );

    protected:  // New functions

    protected:  // Functions from base classes

    private:

        /**
        * C++ default constructor.
        */
        TAccPolSubblockNameArrayAccessor();

    public:     // Data

    protected:  // Data

    private:    // Data

    public:     // Friend classes

    protected:  // Friend classes

    private:    // Friend classes

    };

#endif      // TACCPOLSUBBLOCKNAMEARRAYACCESSOR_H

// End of File
