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
* Description:  Used to map detected accessory HW Device IDs to Generic IDs
*                defined by Accessory Framework.
*
*/


#ifndef CACCCONFIGFILEPARSER_H
#define CACCCONFIGFILEPARSER_H

// INCLUDES
#include <e32base.h>
#include <AccPolGenericID.h>
#include <AccPolNameValueRecord.h>

// CONSTANTS

// MACROS

// DATA TYPES

// FUNCTION PROTOTYPES

// FORWARD DECLARATIONS
class CAccPolAccessoryPolicy;
class CAccConGenericID;

// CLASS DECLARATION

/**
*  Configuration file parser interface.
*
*  @lib AccSrvUtil.lib
*  @since S60 3.1
*/
class CAccConfigFileParser : public CBase
    {
    public:  // Constructors and destructor

        /**
        * Two-phased constructor.
        * 
        * @since S60 3.1
        * @param aConfFileName  The name of the configuration file to be used.
        *                       If KNullDesC, default configuration file is used.
        *                       Only KNullDesC is supported.
        */
        IMPORT_C static CAccConfigFileParser* NewL( const TDesC& aConfFileName );

        /**
        * Destructor.
        */
        IMPORT_C virtual ~CAccConfigFileParser();

    public: // New functions

		/**
        * Deprecated, will be removed.
        *
        * Find a matching Generic ID for Device ID. Returned Generic ID is 
        * unique among the all Generic IDs
        *
        * @since S60 3.1
        * @param aGenericID  On successful return, contains the  Generic ID.
        * @param aHWDeviceID Device ID
        * @param aHWModelID  Manufacturer specific model definition of the 
        *                    hardware (optional). Max size is KHWModelIDMaxLength,
        *                    define in AccPolGIDHeader.h (e.g. TBuf<KHWModelIDMaxLength>).
        * @return void
        */        
        IMPORT_C void FindL( TAccPolGenericID& aGenericID, 
                             TUint64 aHWDeviceID, 
                             const TDesC& aHWModelID )
        #ifndef __WINSCW__
        __attribute__ ((deprecated))
        #endif
         ;
       
        /**
        * Find a matching Generic ID for Device ID. Returned Generic ID is 
        * unique among the all Generic IDs
        *
        * @since S60 3.1
        * @param aGenericID  On successful return, contains the  Generic ID.
        * @param aHWDeviceID Device ID
        * @param aHWModelID  Manufacturer specific model definition of the 
        *                    hardware (optional). Max size is KHWModelIDMaxLength,
        *                    define in AccPolGIDHeader.h (e.g. TBuf<KHWModelIDMaxLength>).
        *                    If value is KNullDesC, model ID from 
        *                    Accessory Policy Database is used.
        * @return void
        */
        IMPORT_C void FindL( CAccConGenericID* const aGenericID,
                             TUint64 aHWDeviceID,
                             const TDesC& aHWModelID );
                             
        /**
        * Parse a Generic ID.
        *
        * @since S60 3.1
        * @param aGenericID       On successful return, contains the  Generic ID.
        * @param aGenericIDHeader Accessory capabilities, the static part of 
        *                         the accessory information.
        * @param aNameValueArray  Accessory capabilities, the dynamic part of 
        *                         the accessory information.
        * @return void
        */
        IMPORT_C void ParseGenericIDL( CAccConGenericID* const aGenericID,
                                       const TAccPolGIDHeader& aGenericIDHeader,
                                       const RArray<TAccPolNameValueRecord>& aNameValueArray );

    public:     // Functions from base classes

    protected:  // New functions

    protected:  // Functions from base classes

    private:

        /**
        * C++ default constructor.
        */
        CAccConfigFileParser();

        /**
        * By default Symbian 2nd phase constructor is private.
        */
        void ConstructL( const TDesC& aConfFileName );

        /**
        * Detect all subblock capabilities and append them to the given
        * array.
        */
        void ParseSubblocksL( const TUint64& aVal, RArray<TUint64>& aCaps );

        /**
        * Detect all subblock names and append them to the given array.
        */
        void ParseNamesL( const TUint32& aVal, RArray<TUint32>& aNames );
        
        /**
        * Find a bit number from bitmask.
        *
        * @since S60 3.1
        * @param aBitmask Bitmask where a bit is searched.
        * @return number of the bit.
        */
        TInt BitNumber( TUint64 aBitmask );

    public:     // Data

    protected:  // Data

    private:    // Data


    public:     // Friend classes

    protected:  // Friend classes

    private:    // Friend classes

    };

#endif // CACCCONFIGFILEPARSER_H

// End of File
