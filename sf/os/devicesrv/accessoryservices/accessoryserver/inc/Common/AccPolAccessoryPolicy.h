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
* Description:  This is the main Accessory Policy API class, and the 
*                   Accessory Server uses it. It provides all kinds of accessor 
*                   functionality for the server, and most operations can be 
*                   mapped straight to the ones defined in the Accessory Server 
*                   client API.
*
*/



#ifndef CACCPOLACCESSORYPOLICY_H
#define CACCPOLACCESSORYPOLICY_H

//  INCLUDES
#include "AccPolCapsSubblockNameArraySerial.h"
#include "AccClientServerMessages.h"
#include "AccPolicyDBNames.h"

#include <AccConGenericID.h>
#include <AccPolGenericID.h>
#include <AccPolGenericIDArray.h>
#include <AccPolAccessoryMode.h>
#include <AccessoryTypes.h>
#include <s32strm.h>
#include <AccPolValueRecord.h> //TAccPolValueType & TAccPolValueLocation

// CONSTANTS

// MACROS

// DATA TYPES

// FUNCTION PROTOTYPES

// FORWARD DECLARATIONS

class CCapabilityStorage;

// CLASS DECLARATION

/**
*  Accessory Policy
*
*  @lib AccPolicy.lib
*  @since S60 3.1
*/
class CAccPolAccessoryPolicy : public CBase
    {
    public:  // Constructors and destructor

        /**
        * Two-phased constructor.
        */
        IMPORT_C static CAccPolAccessoryPolicy* NewL( CCapabilityStorage * aCapabilityStorage);

        /**
        * Destructor.
        */
        IMPORT_C virtual ~CAccPolAccessoryPolicy();

    public: // New functions

        /**
        * Get integer value from database.
        *
        * @since S60 3.1
        * @param aGenericId     Generic ID in question.
        * @param aNameRecord    Capability name which value is queried.
        * @return value of the capability
        */
        IMPORT_C TAccValueTypeTInt GetValueTIntL( const TAccPolGenericID& aGenericId,
                                                  TAccPolNameRecord& aNameRecord ) const;

        /**
        * Get boolen value from database.
        *
        * @since S60 3.1
        * @param aGenericId     Generic ID in question.
        * @param aNameRecord    Capability name which value is queried.
        * @return value of the capability
        */
        IMPORT_C TAccValueTypeTBool GetValueTBoolL( const TAccPolGenericID& aGenericId,
                                                    TAccPolNameRecord& aNameRecord ) const;
        /**
        * Check if the connection of this accessory is allowed/valid.
        *
        * @since S60 3.1
        * @param aGenericID      Generic ID in question.
        * @param aGenericIDArray Generic ID array of accessories.
        * @param aError          In failure contais the Symbian OS error code.
        * @return Connection type.
        * @see AccClientServerMessages.h
        */
        IMPORT_C TAccSrvConnection IsValidConnectionL( 
                                   const TAccPolGenericID& aGenericID,
                                   TInt& aError );

        /**
        * Resolve current accessory mode based on set of Generic IDs.
        *
        * @since S60 3.1
        * @param aGenericIDArray     Source array where to resolve accessory mode.
        * @param aDbId               Unique identifier of a Generic ID. If not exist, 
        *                            accessory mode is updated according to the current,
        *                            combined accessory connection Status.
        * @param aAudioOutputStatus  Current audio routing status, 
        *                            used when aDbId exists.                           
        * @param aAccessoryMode      Current accessory mode.
        * @return Updated accessory mode is returned.
        */
        IMPORT_C TAccPolAccessoryMode ResolveAccessoryModeL( 
                                    const TAccPolGenericIDArray& aGenericIDArray, 
                                    const TInt aDbId, 
                                    const TBool aAudioOutputStatus, 
                                    const TAccPolAccessoryMode aAccessoryMode,
                                    const TInt UniqueID );

        /**
        * Get all critical capabilities defined in Generic ID.
        *
        * @since S60 3.1
        * @param aGenericID            Generic ID instance to be checked
        * @param aCriticalCapabilities On return, contains the list of critical
        *                              capabilities defined in Generic ID(aGenericID).
        * @return void
        */
       IMPORT_C void ResolveCriticalCapabilitiesL( const TAccPolGenericID& aGenericID,
                    CAccPolSubblockNameArray& aCriticalCapabilities );

        /**
        * Get all critical capabilities. 
        * These capabilities are stored to CapabilityStorage.
        *
        * @since S60 3.1        .
        * @return void
        */
        IMPORT_C void GetAllCriticalCapabilitiesL() const;
                    
        /**
        * Compare generig id against connection rules, fill selection index (as a parameter) list
        *
        * @since S60 3.1
        * @param aGenericID Generic ID to be evaluated
        * @param aSelectionList Selections which are possible for generic ID  
        * @return void
        */                    
        IMPORT_C void EvaluateConnectionRulesL( const TAccPolGenericID& aGenericID, 
                                                TUint32& aSelectionListBitMask );
        
        
        /**
        * Update generic ID according to selection index
        *
        * @since S60 3.1
        * @param aGenericID Generic ID to be updated, this reference to generic id will be also updated
        * @param aSelectionIndex Selected index
        * @return void
        */                    
        IMPORT_C void UpdateGenericIDL( TAccPolGenericID& aGenericID, 
                                        const TInt& aSelectionIndex );
        
        
        /**
        * 
        *
        * @since 
        * @param 
        * @param 
        * @return
        */                    
   		IMPORT_C void ParseGenericIDContentL( CAccConGenericID* const aGenericID,
	                                          TUint64 aHWDeviceID,
                                              const TDesC& aHWModelID,
                                              const TDes8& aGidInt,
                                              TAccGIDtype aType );   
                                              
        /**
        * Get all accessory types supported by device.
        *
        * @since S60 3.1
        * @return Supported devices bitmask.
        * @see AccessoryTypes.h for supported HW device types.
        */        
        IMPORT_C TUint32 SupportedHWDevicesL();           
        
        /**
        * Check to see if the requested capability is defined for the genericId
        *
        * @since S60 3.1
        * @param aGenericID Generic ID of the list to be used
        * @param aName Name of the capability
        * @return True if the requested capability is defined for the genericId. Else False. 
        */
        IMPORT_C TBool IsCapabilityDefinedL(const TAccPolGenericID& aGenericId,
                                            const TUint32& aName );
                         
    public: // Functions from base classes
        
    protected:  // New functions

    protected:  // Functions from base classes

    private:

        /**
        * C++ default constructor.
        */
        CAccPolAccessoryPolicy(  CCapabilityStorage * aCapabilityStorage);

        /**
        * By default Symbian 2nd phase constructor is private.
        */
        void ConstructL();

        /**
        * Accessory mode is decided according to mode resolving table.
        */
        void GetAccessoryModeFromRulesTableL(
                const TAccPolGenericIDArray& aGenericIDArray,
                TAccPolAccessoryMode& aAccMode );

        /**
        * Accessory mode is revolved from Generic ID.
        */  
        void SetAccessoryModeL( const TAccPolGenericID& gID,
                                TAccPolAccessoryMode& aAccMode ) const;
                               
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
        TBool BitNumber( TUint64 aBitmask );

        /**
        * 
        *
        * @since S60 3.1
        * @param 
        * @return 
        */
        TBool EvaluateGidHeader( const TAccPolGenericID& aGenericID, 
	                               const TAccPolGenericID& aGIDRule );        
        
    public:     // Data

    protected:  // Data

    private:    // Data
    	
    	 CCapabilityStorage* iCapabilityStorage;
    	 
    	 //When critical capabilities are read from AccessoryPolicy DC this is set to ETrue.
    	 TBool iCriticalCaps;

    public:     // Friend classes

    protected:  // Friend classes

    private:    // Friend classes

    };

#endif      // ACCPOLACCESSORYPOLICY_H

// End of File
