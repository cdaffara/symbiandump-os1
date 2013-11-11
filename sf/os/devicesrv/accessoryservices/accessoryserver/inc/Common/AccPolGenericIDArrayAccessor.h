/*
* Copyright (c) 2002-2004 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  A private accessor API for the Generic ID Array. 
*                  The Accessory Server and the -Policy use it internally.
*
*/



#ifndef TACCPOLGENERICIDARRAYACCESSOR_H
#define TACCPOLGENERICIDARRAYACCESSOR_H

//  INCLUDES
#include <AccPolGenericIDArray.h>

// CONSTANTS

// MACROS

// DATA TYPES

// FUNCTION PROTOTYPES

// FORWARD DECLARATIONS

// CLASS DECLARATION

/**
*  A private accessor API for the Generic ID Array
*
*  @lib AccPolicy.lib
*  @since S60 3.1
*/
class TAccPolGenericIDArrayAccessor
    {
    public:  // Constructors and destructor
        
    public: // New functions

        /**
        * Replace the member array with the given one.
        *
        * @since S60 3.1
        * @param aGenericIDArray Destination Generic ID Array.
        * @param aNewArray       Source Generic ID Array.
        * @return void
        */
        IMPORT_C static void ReplaceArrayL(
                    TAccPolGenericIDArray& aGenericIDArray,
                    const RArray<TAccPolGenericID>& aNewArray );

        /**
        * Copy Generic ID array instance to another.
        *
        * @since S60 3.1
        * @param aSource Source Generic ID array.
        * @param aTarget Destination Generic ID Array.
        * @return ?description
        */
        IMPORT_C static void CopyGenericIDArray(
                    const TAccPolGenericIDArray& aSource,
                    TAccPolGenericIDArray& aTarget );
        
       /** 
        * Remove an index entry from Generic ID array. 
        * 
        * @since S60 3.1 
        * @param aGenericIDArray Source Generic ID array. 
        * @param aIndex Index entry to be removed from aGenericIDArray. 
        * @return ?description 
        */ 
        IMPORT_C static void RemoveIndexFromGenericIDArray( 
                    TAccPolGenericIDArray& aGenericIDArray, 
                    TInt  aIndex );

        /**
        * Find Generic ID from array based on Device ID.
        *
        * @since S60 3.1
        * @param aGenericIDArray Array where to find.
        * @param aHWDeviceID     Searched HW Device ID.
        * @return If found the index of the Generic ID, else KErrNotFound.
        */
        IMPORT_C static TInt FindWithHWDeviceIDL(
                    const TAccPolGenericIDArray& aGenericIDArray,
                    const TUint64& aHWDeviceID );

        /**
        * Find Generic ID from array based on Device Address.
        *
        * @since S60 3.1
        * @param aGenericIDArray Array where to find.
        * @param aHWDeviceID     Searched Device Address.
        * @return If found the index of the Generic ID, else KErrNotFound.
        */
        IMPORT_C static TInt FindWithDeviceAddressL(
                    const TAccPolGenericIDArray& aGenericIDArray,
                    const TUint64& aDeviceAddress );
                                                    
        /**
        * Find Generic ID from array based on Unique DBID.
        *
        * @since S60 3.1
        * @param aGenericIDArray Array where to find.
        * @param aUnigueID       Searched unique ID.
        * @return If found the index of the Generic ID, else KErrNotFound.
        */
        IMPORT_C static TInt FindWithUniqueIDL(
                    const TAccPolGenericIDArray& aGenericIDArray,
                    TInt aUnigueID );
        
    public: // Functions from base classes
        
    protected:  // New functions

    protected:  // Functions from base classes

    private:

        /**
        * C++ default constructor.
        */
        TAccPolGenericIDArrayAccessor();
        
    public:     // Data
    
    protected:  // Data

    private:    // Data

    public:     // Friend classes

    protected:  // Friend classes

    private:    // Friend classes

    };

#endif      // TACCPOLGENERICIDARRAYACCESSOR_H   

// End of File
