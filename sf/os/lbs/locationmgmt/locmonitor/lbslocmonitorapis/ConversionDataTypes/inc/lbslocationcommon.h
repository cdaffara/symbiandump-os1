/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Description: Base class for all the class representing
* location informations.
*
*/


/**
@file
@publishedPartner
@prototype
*/


#ifndef LBS_LOCATION_COMMON_H
#define LBS_LOCATION_COMMON_H


#include <e32base.h>
#include <s32strm.h>
#include <lbs/lbslocationclasstypes.h>


// Constants 

/**
 * Defines the maximum allowable batch size supported by the system
 */
const TUint KPositionMaxBatchSize = 500;


/**
 * Abstract base class for data class representing location area 
 * information. 
 */
NONSHARABLE_CLASS(CLbsLocationInfoBase) : public CBase
    {

public:
  
    /**
     * This method is implemented by each of the derived classes that
     * extend this class.  
     */
    IMPORT_C virtual TLbsLocationInfoClassType Type();
  
    /**
     * Destructor
     */
    IMPORT_C virtual ~CLbsLocationInfoBase();

    /**
     * Internalizes the geographical area information from a stream.
     *
     * @param[in] aStream Stream from which the object is internalized.
     */    
    IMPORT_C void InternaliseL( RReadStream& aStream );
  
    /**
     * Externalizes the geographical area information to a stream.
     *
     * @param[in] aStream Stream to which the object is to be 
     * externalized.
     */    
    IMPORT_C void ExternaliseL( RWriteStream& aStream ) const;
    
protected:

    /**
     * C++ default constructor
     */
    CLbsLocationInfoBase();
        
    /**
     * Internalize method that subclass must implement.
     * 
     * @param[in] aStream Stream from which the object is internalized.
     */    
    virtual void DoInternaliseL( RReadStream& aStream ) = 0;
  
    /**
     * Externalize method that subclass must implement.
     *
     * @param[in] aStream Stream to which the object is to be 
     * externalized.
     */    
    virtual void DoExternaliseL( RWriteStream& aStream ) const = 0;
 
private:
   
    /**
     * Copy constructor. Prohibited by default.
     */
    CLbsLocationInfoBase( CLbsLocationInfoBase& );
   
    /**
     * Overloaded assignment operator. Prohibited by default.
     */
    CLbsLocationInfoBase& operator=( CLbsLocationInfoBase& );

private:
    
    /*
     * Reserved for future use.
     */
    TAny* iReserved;
    
    };    
    
typedef RPointerArray<CLbsLocationInfoBase> RLbsLocationInfoBaseArray;

/**
 * Abstract base class for data types representing geogrphical area 
 * information. 
 *
 * Geographical information may be a collection of GSM or WCDMA cells
 * or WLAN information. 
 */
NONSHARABLE_CLASS(CLbsAreaInfoBase) : public CBase
    {

public:
  
    /**
     * This method is implemented by each of the geographical area type classes
     * that extend this class. The method returns the type of geographical area
     * being represented.
     */
    IMPORT_C virtual TLbsAreaInfoClassType Type();
  
    /**
     * Destructor
     */
    IMPORT_C virtual ~CLbsAreaInfoBase();

    /**
     * Internalizes the geographical area information from a stream.
     *
     * @param[in] aStream Stream from which the object is internalized.
     */    
    IMPORT_C void InternaliseL( RReadStream& aStream );
  
    /**
     * Externalizes the geographical area information to a stream.
     *
     * @param[in] aStream Stream to which the object is to be 
     * externalized.
     */    
    IMPORT_C void ExternaliseL( RWriteStream& aStream );
    
    /**
     * Validates area information.
     */
    virtual void ValidateDataL() const = 0;

protected:

    /**
     * C++ default constructor
     */
    CLbsAreaInfoBase();
		
    /**
     * Internalize method that subclass must implement.
     * 
     * @param[in] aStream Stream from which the object is internalized.
     */    
    virtual void DoInternaliseL( RReadStream& aStream ) = 0;
  
    /**
     * Externalize method that subclass must implement.
     *
     * @param[in] aStream Stream to which the object is to be 
     * externalized.
     */    
    virtual void DoExternaliseL( RWriteStream& aStream ) const = 0;
 
private:
   
    /**
     * Copy constructor. Prohibited by default.
     */
    CLbsAreaInfoBase( CLbsAreaInfoBase& );
   
    /**
     * Overloaded assignment operator. Prohibited by default.
     */
    CLbsAreaInfoBase& operator=( CLbsAreaInfoBase& );

private:
    
    /**
     * Reserved for future use.
     */
    TAny* iReserved;
    
    };

#endif // LBS_LOCATION_COMMON_H
