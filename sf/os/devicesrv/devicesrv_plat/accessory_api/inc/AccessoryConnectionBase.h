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
* Description:  Base class for Accessory API
*
*/



#ifndef RACCESSORYCONNECTIONBASE_H
#define RACCESSORYCONNECTIONBASE_H

//  INCLUDES
#include <AccessorySubsessionBase.h>
#include <AccPolNameRecord.h>
#include <AccessoryTypes.h>

// FORWARD DECLARATIONS
class CAccPolSubblockNameArray;
class CAccPolObjectCon;

// CLASS DECLARATION

/**
*  Base class for Accessory Server RAccessoryConnection and
*  RAccessorySingleConnection sub-sessions.
*
*  This class is not intended for user derivation.
*
*  @lib AccClient.lib
*  @since S60 3.1
*/
class RAccessoryConnectionBase : public RAccessorySubsessionBase
    {

    public:

        /**
        * Get supported capabilities for the connected accessory.
        *
        * @since S60 3.1
        * @param aGenericID Generic ID of the connected accessory.
        * @param aNameArray On return, contains array of supported
        *                   capabilities.
        * @return void
        * @exception system-wide error code if failed.
        *            Specifically:
        *            KErrArgument if an argument is out of range
        * @see GetValueL
        * @see AccPolCommonNameValuePairs.h
        * @see AccPolProprietaryNameValuePairs.h
        */
          IMPORT_C void GetSubblockNameArrayL( const TAccPolGenericID& aGenericID,
                                               CAccPolSubblockNameArray& aNameArray ) const;

        /**
        * Get the value for the specific accessory capability.
        *
        * Accessory capability name constants and value types are defined in
        * AccPolCommonNameValuePairs.h and AccPolProprietaryNameValuePairs.h.
        *
        * @since S60 3.1
        * @param aGenericID  Generic ID of the connected accessory.
        * @param aNameRecord Accessory capability.
        * @param aValue      On return, contains the current value for
        *                    the specific accessory capability.
        * @return KErrNone if successful.
        *         KErrTimedOut if an operation has timed out.
        * @exception system-wide error code if failed.
        *            Specifically:
        *            KErrArgument if an argument is out of range.
        * @see GetSubblockNameArrayL()
        * @see AccPolCommonNameValuePairs.h
        * @see AccPolProprietaryNameValuePairs.h
        * @see AccPolGenericid.h
        */
        IMPORT_C TInt GetValueL( const TAccPolGenericID& aGenericID,
                                 const TAccPolNameRecord& aNameRecord,
                                 TAccValueTypeTBool& aValue ) const;
        /**
        * Get the value for the specific accessory capability.
        *
        * Accessory capability name constants and value types are defined in
        * AccPolCommonNameValuePairs.h and AccPolProprietaryNameValuePairs.h.
        *
        * @since S60 3.1
        * @param aGenericID  Generic ID of the connected accessory.
        * @param aNameRecord Accessory capability.
        * @param aValue      On return, contains the current value for
        *                    the specific accessory capability.
        * @return KErrNone if successful.
        *         KErrTimedOut if an operation has timed out.
        * @exception system-wide error code if failed.
        *            Specifically:
        *            KErrArgument if an argument is out of range.
        * @see GetSubblockNameArrayL()
        * @see AccPolCommonNameValuePairs.h
        * @see AccPolProprietaryNameValuePairs.h
        * @see AccPolGenericid.h
        */

        IMPORT_C TInt GetValueL( const TAccPolGenericID& aGenericID,
                                 const TAccPolNameRecord& aNameRecord,
                                 TAccValueTypeTInt& aValue ) const;

        /**
        * Get the value for the specific accessory capability.
        *
        * Accessory capability name constants and value types are defined in
        * AccPolCommonNameValuePairs.h and AccPolProprietaryNameValuePairs.h.
        *
        * @since S60 3.1
        * @param aGenericID  Generic ID of the connected accessory.
        * @param aNameRecord Accessory capability.
        * @param aValue      On return, contains the current value for
        *                    the specific accessory capability.
        * @return KErrNone if successful.
        *         KErrTimedOut if an operation has timed out.
        * @exception system-wide error code if failed.
        *            Specifically:
        *            KErrArgument if an argument is out of range.
        * @see GetSubblockNameArrayL()
        * @see AccPolCommonNameValuePairs.h
        * @see AccPolProprietaryNameValuePairs.h
        * @see AccPolGenericid.h
        */
        IMPORT_C TInt GetValueL( const TAccPolGenericID& aGenericID,
                                 const TAccPolNameRecord& aNameRecord,
                                 TDes8& aValue ) const;

        /**
        * Get the value for the specific accessory capability.
        *
        * Accessory capability name constants and value types are defined in
        * AccPolCommonNameValuePairs.h and AccPolProprietaryNameValuePairs.h.
        *        
        * @param aGenericID  Generic ID of the connected accessory.
        * @param aNameRecord Accessory capability.
        * @param aValue      On return, contains the current value for
        *                    the specific accessory capability.
        * @return KErrNone if successful.
        *         KErrTimedOut if an operation has timed out.
        * @exception system-wide error code if failed.
        *            Specifically:
        *            KErrArgument if an argument is out of range.        
        * @see accpolobjectbase.h
        * @see accpolobjectcon.h
        * @see AccPolCommonNameValuePairs.h
        */
        IMPORT_C TInt GetValueL( const TAccPolGenericID& aGenericID,
                                 const TAccPolNameRecord& aNameRecord,
                                 CAccPolObjectCon& aValue ) const;

    protected:

        /**
        * C++ default constructor.
        */
        IMPORT_C RAccessoryConnectionBase();

    };

#endif      // RACCESSORYCONNECTIONBASE_H

// End of File

