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
* Description:  This class defines the Accessory Policy DB interface. It is
*                used by the Accessory Policy to access the database.
*
*/


#ifndef ACCPOLICYDB_H
#define ACCPOLICYDB_H

// INCLUDES
#include "AccClientServerMessages.h"
#include "AccPolCapabilityStorage.h"
#include "AccPolicyDBNames.h"
#include <AccPolValueRecord.h> //TAccPolValueType & TAccPolValueLocation 
#include <e32std.h>
#include <d32dbms.h> // RDbStoreDatabase
#include <f32file.h> // RFs
#include <s32file.h> // CFileStore

// CONSTANTS
const TInt KMaxBufLength = 256;

// MACROS

// DATA TYPES

// FUNCTION PROTOTYPES

// FORWARD DECLARATIONS

// CLASS DECLARATION
/**
*  Policy DB interface.
*
*  @lib AccPolicyDB.lib
*  @since S60 3.1
*/
class CAccPolicyDB : public CBase
    {
    public:  // Constructors and destructor

        /**
        * Two-phased constructor.
        */
        IMPORT_C static CAccPolicyDB* NewL();

        /**
        * Destructor.
        */
        IMPORT_C virtual ~CAccPolicyDB();

    public: // New functions

        /**
        * OpenDBL Open DB connection.
        *
        * @since S60 3.1.
        * @return void.
        */
        IMPORT_C void OpenDBL();

        /**
        * CloseDB Close DB connection.
        *
        * @since S60 3.1.
        * @return void.
        */
        IMPORT_C void CloseDB();

        /**
        * Search if DeviceID has a mapped GIDInteger defined in DB.
        *
        * @since S60 3.1.
        * @param aDeviceID Device ID.
        * @param aGIDInt On succesfull return contains the GID integer value.
        * @return void.
        */
        IMPORT_C void FindGIDIntL( const TUint64& aDeviceID, TDes8& aGIDInt );
          
        
        /**
        * Search if DeviceID has a mapped GIDInteger defined in DB.
        *
        * @since S60 3.1.
        * @param aDeviceID Device ID.
        * @param aGIDInt On succesfull return contains the GID integer value.
        * @return void.
        */
        IMPORT_C void FindGIDIntL( const TUint64& aDeviceID, TDes8& aGIDInt, TAccGIDtype aType );  
           
        /**
        * Read all critical capabilities from DB.
        *
        * @since S60 3.1
        * @return void
        */       
        IMPORT_C TInt ReadAllCriticalCapabilitiesL();
        
        /**
        * Get the next critical capability read with ReadAllCriticalCapabilitiesL().
        *
        * @since S60 3.1
        * @param aName  On succesfull return, contains the next critical capability.
        * @param aValue On succesfull return, contains the value of next critical capability.
        * @return void
        */  
        IMPORT_C void GetNextCriticalCapabilityL( TUint32& aName, TInt32& aValue );
                
        /**
        * 
        *
        * @since S60 3.1
        * @param 
        * @param 
        * @return void
        */        
        IMPORT_C TInt FilterGIDByTypeL( TAccGIDtype aType );
        
        /**
        * 
        *
        * @since S60 3.1
        * @param 
        * @param 
        * @return void
        */        
        IMPORT_C void GetNextGIDL( TUint64& aHWDeviceID, TDes8& aGIDInt );
        
        
        /**
        * 
        *
        * @since S60 3.1
        * @param 
        * @param 
        * @param aGetAllDeviceTypes If Ture, all configured device types are 
        *                           returned (i.e. all selection indexes).
        * @return void
        */        
        IMPORT_C void GetSelectionsL( TUint32 ruleIndex, 
                                      TUint32& aSelectionListBitMask,
                                      TBool aGetAllDeviceTypes = EFalse );
                       
                        
    public:     // Functions from base classes

    protected:  // New functions

    protected:  // Functions from base classes

    private:

        /**
        * C++ default constructor.
        */
        CAccPolicyDB();

        /**
        * By default Symbian 2nd phase constructor is private.
        */
        void ConstructL();

    public:     // Data
    
    protected:  // Data

    private:    // Data

        // For use of iFileStore
        RFs iFsSession;

        // For database operations
        RDbNamedDatabase iDb;
    
        // Buffer for SQL clauses
        TBuf<KMaxBufLength> iSQL;

        // View instance for rowsets from an SQL query
        RDbView iView;

    public:     // Friend classes

    protected:  // Friend classes

    private:    // Friend classes

    };

#endif// ACCPOLICYDB_H

// End of File
