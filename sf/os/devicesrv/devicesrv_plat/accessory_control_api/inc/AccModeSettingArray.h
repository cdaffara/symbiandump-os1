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
* Description: ?Description
*
*/



#ifndef ACCMODESETTINGARRAY_H
#define ACCMODESETTINGARRAY_H

//  INCLUDES
#include <AccModeSetting.h>

// CONSTANTS
// Maximum number of settings TAccModeSettingArray can store.
const TInt KMaxAccModeSettings = 20;

// MACROS

// DATA TYPES

// FUNCTION PROTOTYPES

// FORWARD DECLARATIONS

// CLASS DECLARATION

/**
*  Stores number of TAccModeSetting objects.
*
*  @lib AccControl.lib
*  @since S60 3.1
*/
class TAccModeSettingArray
    {
    public:  // Constructors and destructor

        /**
        * C++ default constructor.
        */
        IMPORT_C TAccModeSettingArray();

    public: // New functions

        /**
        * Adds setting onto the end of the array.
        *
        * @since S60 3.1
        * @param aSetting setting
        * @return KErrAlreadyExists if setting with same id has already been
        * stored. KErrNotSupported if array is full.
        */
        IMPORT_C TInt AddSetting( const TAccModeSetting& aSetting );
		
        /**
        * Returns setting from array at the specified position.
        *
        * @since S60 3.1
        * @param aIndex specifies position (= an index of array)
        * @param aSetting setting
        * @return KErrNotSupported if aIndex is out of array's range
        */
        IMPORT_C TInt GetSetting( const TInt aIndex,
                      TAccModeSetting& aSetting ) const;
                      
        /**
        * Removes all items from array.
        *
        * @since S60 3.1
        * @return void
        */
        IMPORT_C void Reset();

        /**
        * Returns the size of an array.
        *
        * @since S60 3.1
        * @return size of an array
        */
        IMPORT_C TInt GetArraySize() const;

        /**
        * Returns maximum size of an array.
        *
        * @since S60 3.1
        * @return maximum size of an array
        */
        IMPORT_C TInt GetMaxArraySize() const;

    private:    // Data

        // points to next free index in iSettings array
        TInt iIndex;

        TAccModeSetting iSettings[KMaxAccModeSettings];

    };

#endif      // ACCMODESETTINGARRAY_H

// End of File
