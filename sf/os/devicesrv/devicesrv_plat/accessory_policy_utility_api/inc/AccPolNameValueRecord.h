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
* Description:  A struct definition that realizes the Name/Value record 
*                concept. An accessory can have a variable amount of 
*                Name/Value records that define some specific features for 
*                that accessory. The existence of a Name/Value record is 
*                defined (and can be queried from) the Generic ID.
*
*/



#ifndef ACCNAMEVALUERECORD_H
#define ACCNAMEVALUERECORD_H

//  INCLUDES
#include <e32base.h>
#include <AccPolNameRecord.h>
#include <AccPolValueRecord.h>

// CONSTANTS

// MACROS

// DATA TYPES

// FUNCTION PROTOTYPES

// FORWARD DECLARATIONS
class CAccPolObjectCon;

// CLASS DECLARATION

class TAccPolNameValueRecord
	{
    public:  // Constructors and destructor
        
    /**
     * C++ default constructor.
     */
   	IMPORT_C TAccPolNameValueRecord();

    /**
     * Constructor with name and value record.
     *
     * @since S60 3.1
     * @param aNameRecord  Name record.
     * @param aValueRecord Value record.
     */        
		IMPORT_C TAccPolNameValueRecord( const TAccPolNameRecord aNameRecord, 
		                                 const TAccPolValueRecord aValueRecord );
		
		/**
     * Constructor with capability name and value.
     *
     * Accessory capability name constants and value types are defined in
     * AccPolCommonNameValuePairs.h and AccPolProprietaryNameValuePairs.h.
     *
     * @since S60 3.1
     * @param aName     Capability name.
     * @param aValue    Capability value.
     * @param aType     Capability type.
     * @param aLocation Capability location.
     */
		IMPORT_C TAccPolNameValueRecord( const TUint32& aName, 
		                                 TInt aValue, 
		                                 TAccPolValueType aType, 
		                                 TAccPolValueLocation aLocation );
		                                
		/**
     * Constructor with capability name and value.
     *
     * Accessory capability name constants and value types are defined in
     * AccPolCommonNameValuePairs.h and AccPolProprietaryNameValuePairs.h.
     *
     * @since S60 3.1
     * @param aName     Capability name.
     * @param aValue    Capability value.
     *                  Notice! only reference to capability value is stored.
     * @param aLocation Capability location.
     */ 
		IMPORT_C TAccPolNameValueRecord( const TUint32& aName, 
		                                 TDesC8& aValue,
		                                 TAccPolValueLocation aLocation );
		                                 
		/**
     * Constructor with capability name without value.
     *
     * Accessory capability name constants and value types are defined in
     * AccPolCommonNameValuePairs.h and AccPolProprietaryNameValuePairs.h.
     *
     * @since S60 3.1
     * @param aName Capability name.
     */ 
		IMPORT_C TAccPolNameValueRecord( const TUint32& aName );
    
    /**
     * Constructor with capability name and value.
     *
     * Accessory capability name constants and value types are defined in
     * AccPolCommonNameValuePairs.h and AccPolProprietaryNameValuePairs.h.
     *     
     * @param aName     Capability name.     
     * @param aLocation Capability location.
     * @param aValue    Capability value.        
     */ 
    IMPORT_C TAccPolNameValueRecord( const TUint32& aName,
                                     TAccPolValueLocation aLocation,
                                     CAccPolObjectCon* aValue = NULL );

    public: // New functions

    /**
     * Returns the name record of name/value record.
     *
     * @since S60 3.1
     * @return TAccPolNameRecord instance.
     */
    IMPORT_C TAccPolNameRecord NameRecord() const;

    /**
     * Returns the value record of name/value record.
     *
     * @since S60 3.1
     * @return TAccPolValueRecord instance.
     */
    IMPORT_C TAccPolValueRecord ValueRecord() const;

	private:
	
	    // Name record of this name/value record
	    TAccPolNameRecord iNameRecord;

	    // Value record of this name/value record
	    TAccPolValueRecord iValueRecord;
	};

#endif // ACCNAMEVALUERECORD_H   

// End of File
