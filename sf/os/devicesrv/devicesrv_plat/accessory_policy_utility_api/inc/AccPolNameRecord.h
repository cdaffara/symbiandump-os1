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
* Description:  Name record definition.
*
*/



#ifndef TACCPOLNAMERECORD_H
#define TACCPOLNAMERECORD_H

//  INCLUDES
#include <e32base.h>
#include <AccPolCommonNameValuePairs.h>

// CONSTANTS

// MACROS

// DATA TYPES

// FUNCTION PROTOTYPES

// FORWARD DECLARATIONS

// CLASS DECLARATION

/**
* TAccPolNameRecord is a class representation that holds a Accessory Capability.   
*
*  @lib AccPolicy.lib
*  @since S60 3.1
*/
class TAccPolNameRecord
	{
    public:  // Constructors and destructor
        
        /**
        * C++ default constructor.
        */
        IMPORT_C TAccPolNameRecord();

    public: // New functions

        /**
        * Returns the name of Accessory Capability.
        *
        * Accessory Capability name constants and value types are defined in
        * AccPolCommonNameValuePairs.h and AccPolProprietaryNameValuePairs.h.
        *
        * @since S60 3.1
        * @param aName On succesfull return, contains the capability name.
        * @return void
        */
        IMPORT_C void GetName( TUint32& aName ) const;

        /**
        * Sets the name of Accessory Capability.
        *
        * Accessory Capability name constants and value types are defined in
        * AccPolCommonNameValuePairs.h and AccPolProprietaryNameValuePairs.h.
        *
        * @since S60 3.1
        * @param aName Accessory Capability name which is set.
        * @return void
        */
		IMPORT_C void SetNameL( const TUint32& aName );


        /**
        * Sets the name of Accessory Capability.
        *
        * Accessory Capability name constants and value types are defined in
        * AccPolCommonNameValuePairs.h and AccPolProprietaryNameValuePairs.h.
        *
        * @since S60 3.1
        * @param aName Accessory Capability name which is set.
        * @return void
        */
		IMPORT_C void SetName( const TUint32& aName );
	private:
    // Name of Accessory Capability. 
    TUint32 iName;
	};

#endif // ACCNAMEVALUERECORD_H   

// End of File
