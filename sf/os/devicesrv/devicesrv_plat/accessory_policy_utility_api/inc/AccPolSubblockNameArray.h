/*
* Copyright (c) 2002-2005 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  This is a public API class that defines the Subblock 
*                   Array -class. It is used as a container for a set of 
*                   subblock name definitions.
*
*/



#ifndef CACCPOLSUBBLOCKNAMEARRAY_H
#define CACCPOLSUBBLOCKNAMEARRAY_H

//  INCLUDES
#include <e32std.h>
#include <e32base.h>

// CONSTANTS

// MACROS

// DATA TYPES

// FUNCTION PROTOTYPES

// FORWARD DECLARATIONS

// CLASS DECLARATION

/**
*  Used as a container for a set of subblock name definitions.
*
*  @lib AccPolicy.lib
*  @since S60 3.1
*/
class CAccPolSubblockNameArray : public CBase
    {
    public:  // Constructors and destructor
        
        /**
        * Two-phased constructor.
        */
        IMPORT_C static CAccPolSubblockNameArray* NewL();
        
        /**
        * Destructor.
        */
        IMPORT_C virtual ~CAccPolSubblockNameArray();

    public: // New functions
        
        /**
        * Checks if the array contains the specified name.
        *
        * @since S60 3.1
        * @param aName Name definition which is searched from the array.
        * @return ETrue if the array contains the name, otherwise EFalse.
        */
        IMPORT_C TBool HasName( const TUint32& aName ) const;

    public:     // Functions from base classes
        
    protected:  // New functions
        
    protected:  // Functions from base classes

    private:

        /**
        * C++ default constructor.
        */
        CAccPolSubblockNameArray();

        /**
        * By default Symbian 2nd phase constructor is private.
        */
        void ConstructL();

    public:     // Data
    
    protected:  // Data

    private:    // Data
        
        //Name Array
        RArray<TUint32> iNameArray;

    public:     // Friend classes
        
    protected:  // Friend classes
        
    private:    // Friend classes
        friend class TAccPolSubblockNameArrayAccessor;
    };

#endif      // CACCPOLSUBBLOCKNAMEARRAY_H   
            
// End of File
