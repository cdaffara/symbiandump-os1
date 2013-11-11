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
* Description:  This is a public API class that defines the Generic ID Array 
*                   -class. It is used as a container for a set of Generic ID 
*                   instances.
*
*/



#ifndef ACCPOLGENERICIDARRAY_H
#define ACCPOLGENERICIDARRAY_H

//  INCLUDES
#include <AccPolGenericID.h>

// CONSTANTS
// Maximum amount of Generic ID -instances that an array can contain.
const TInt KMaxGenericIDInstances( 10 );

// MACROS

// DATA TYPES

// FUNCTION PROTOTYPES

// FORWARD DECLARATIONS

// CLASS DECLARATION

/**
*  This class is used as a container for a set of Generic ID instances.
*
*  @lib AccPolicy.lib
*  @since S60 3.1
*/
class TAccPolGenericIDArray
    {
    public:  // Constructors and destructor

        /**
        * C++ default constructor.
        */
        IMPORT_C TAccPolGenericIDArray();

    public: // New functions

        /**
        * Get Generic ID from a defined index.
        *
        * @since S60 3.1
        * @param aIndex Index of the returned Generic ID instance
        * @return TAccPolGenericID the Generic ID instance at the defined index,
        * leaves with KErrGeneral if index is out of array size range.
        */
        IMPORT_C const TAccPolGenericID GetGenericIDL( const TInt aIndex ) const;

        /**
        * Gets the number of objects in the array.
        *
        * @since S60 3.1
        * @return The number of objects in the array. 
        */
        IMPORT_C TInt Count() const;

    private:    // Data
        
        // The actual array of Generic ID -instances.
        TAccPolGenericID iGenericIDArray[KMaxGenericIDInstances];

    private:    // Friend classes
        // An accessor API for the class
        friend class TAccPolGenericIDArrayAccessor;

    };

#endif      // ACCPOLGENERICIDARRAY_H

// End of File
