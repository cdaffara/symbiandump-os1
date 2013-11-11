/*
* Copyright (c) 2006 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Accessory information is encapsulated to this class.
*
*/



#ifndef ACCMONITORINFO_H
#define ACCMONITORINFO_H

// INCLUDES
#include <e32base.h>
#include <AccMonitorCapabilities.h>

// FORWARD DECLARATIONS
class CAccMonitorInfo;

// EXTERNAL DATA TYPES
/**
* Type definition RConnectedAccessories consist of connected accessories
* definitions which are defined in CAccMonitorInfo class.
* Note: If Pushed onto Cleanup stack, make sure to use ResetAndDestroy Array API just before PopAndDestroy Cleanup stack API, else it causes array elements to leak its memory.
*/
typedef RPointerArray<CAccMonitorInfo>  RConnectedAccessories;

/**
* Type definition RAccMonCapabilityArray consist of an array of accessory
* capabilities.
*/
typedef RArray<TAccMonCapability>   RAccMonCapabilityArray;

// CLASSES
/**
* CAccMonitorInfo is a container for single accessory information.
*
* CAccMonitorInfo holds accessory capabilities and other accessory related
* information. The data members of this class can be accessed with methods in
* in this class.
*
* @see CAccMonitor for examples.
* @lib AccMonitor
* @since S60 5.0
*/
class CAccMonitorInfo : public CBase
    {
    
public:
    
    /**
    * Symbian two phased constructor.
    */
    static IMPORT_C CAccMonitorInfo* NewL();
    
    /**
    * Symbian two phased constructor.
    */
    static IMPORT_C CAccMonitorInfo* NewLC();
    
    /**
    * Symbian two phased constructor.
    */
    static IMPORT_C CAccMonitorInfo* NewL( const CAccMonitorInfo* aAccMonInfo );
    
    /**
    * Symbian two phased constructor.
    */
    static IMPORT_C CAccMonitorInfo* NewLC( const CAccMonitorInfo* aAccMonInfo );
    
    /**
    * Reset the instance values.
    */
    inline void Reset();
    
    /**
    * Gets the accessory device type defined for this connected accessory.
    *
    * @return The accessory device type.
    */
    inline TAccMonCapability AccDeviceType() const;
    
    /**
    * Gets the accessory physical connection defined for this connected
    * accessory.
    *
    * @return The accessory physical connection.
    */
    inline TAccMonCapability AccPhysicalConnection() const;
    
    /**
    * Gets the accessory device address defined for this connected
    * accessory.
    *
    * @return This accessory's device address.
    */
    inline TUint64 AccDeviceAddress() const;
    
    /**
    * Gets a capability that is defined in the array for this connected
    * accessory. If the array of this class is empty and this function is
    * used this function will panic.
    *
    * @param aIndex is the index where some capability needs to be getted.
    * @return TAccMonCapability the capability in that index.
    */
    inline TAccMonCapability AccCapabilityAtIndex( TInt aIndex ) const;
    
    /**
    * Gets the count of objects in the array.
    *
    * @return TInt the size of array.
    */
    inline TInt Count() const;
    
    /**
    * Checks if some capability is defined in this connected accessory.
    *
    * @param    aCapability is the capability definition that needs to be
    *           checked if it is defined for this accessory.
    * @return   ETrue if this capability is defined.
    */
    inline TBool Exists( TAccMonCapability aCapability ) const;
    
    /**
    * Checks if a group of capabilities are defined for this accessory.
    * aCapabilityArray is the array of capabilities that need to be checked.
    *
    * @param    aCapabilityArray is the array of capability definitionion that need to be
    *           checked if they are defined for this accessory.
    * @return   ETrue if these capabilities is defined.
    */
    IMPORT_C TBool Exists( RAccMonCapabilityArray& aCapabilityArray ) const;
    
    /**
    * Copy function for this class.
    *
    * @param    aAccMonInfo is the instance from what the copy needs to be made of.
    * @return   CAccMonitorInfo the copy of this class.
    */
    IMPORT_C void CopyL( const CAccMonitorInfo* aAccMonInfo );
    
    /**
    * Comparison function for this class.
    *
    * @param    aAccMonInfo is the instance what needs to be compared.
    * @return   ETrue if comparison matches.
    */
    IMPORT_C TBool CompareL( const CAccMonitorInfo* aAccMonInfo ) const;
    
    /**
    * Destructor
    */
    virtual ~CAccMonitorInfo();
      
private:  
    
    friend class CAccMonitorContainer;
    
    /**
    * Default Constructor 
    */
    CAccMonitorInfo();
    
    /**
    * Symbian OS 2nd phase constructor 
    */
    void ConstructL();
    
    /**
    * Symbian OS 2nd phase constructor 
    */
    void ConstructL( const CAccMonitorInfo* aAccMonInfo );
    
    /**
    * Assignment operator is hidden so that clients cannot use the
    * assignement to this class. Instead the CopyL method should be used.
    */
    void operator=( CAccMonitorInfo aAccInfo );
    
private:
    
    // An array to hold the capabilities for this instance
    RAccMonCapabilityArray iAccCapabilityArray;
    
    // Defines device type for this accessory
    TAccMonCapability iAccessoryDeviceType;
    
    // Defines physical connection for this accessory
    TAccMonCapability iAccessoryPhysicalConnection;
    
    // Defines device address for this accessory, not defined allways
    TUint64 iDeviceAddress;
    
    // A numerical definition to point the correct accessory
    TInt iAccessoryID;
    
    // A reserved member variable for future use.
    TInt iReserved1;
    
    // A reserved member variable for future use.
    TInt iReserved2;
    };
    
#include <AccMonitorInfo.inl>    

#endif // ACCMONITORINFO_H

// End of File
