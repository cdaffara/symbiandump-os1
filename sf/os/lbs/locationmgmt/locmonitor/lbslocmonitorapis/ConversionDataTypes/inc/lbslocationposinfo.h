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
* Description: Data class representing a geographical area in terms of 
* latitude,longitude,altitude,horizontal accuracy and vertical accuracy.
*
*/

/**
* @file
* @publishedPartner
* @prototype
*/


#ifndef LBS_COORDINATE_INFO_H
#define LBS_COORDINATE_INFO_H


#include <e32std.h>
#include <lbs.h>
#include <lbs/lbslocationcommon.h>


/**
 * This class contains the basic coordinates of a location (latitude, longitude 
 * and altitude) along with vertical and horizontal accuracies. This class 
 * is different from TPositionInfo class defined in LBS and cannot be used
 * interchangeably. @see TPositionInfo class for more details. 
 */
NONSHARABLE_CLASS(CLbsPositionInfo) : public CLbsAreaInfoBase
    {

public:

    /**
     * Allocates and constructs a new position info object. In the returned
     * instance, the longitude, latitude, altitude, horizontal accuracy and 
     * vertical accuracy are set to NaN.
     * 
     * @return Pointer to the new coordinate info object.
     * @leave Standard symbian error codes, such as KErrNoMemory
     */
    IMPORT_C static CLbsPositionInfo* NewL();
    
    /**
     * Allocates and constructs a new position info object, a copy of another one.
     * 
     * @param[in] aLocality Instance of CLbsPositionInfo. 
     */
    IMPORT_C static CLbsPositionInfo* NewL( const CLbsPositionInfo& aPositionInfo );
        
    /**
     * Allocates and constructs a new position info object
     * 
     * @param[in] aLocality Instance of TLocality class that contains 
     * coordinates along with the error estimates for the horizontal and 
     * vertical accuracy of the point. 
     */
       IMPORT_C static CLbsPositionInfo* NewL( const TLocality& aLocality );
      
    /**
     * Destructor
     */    
    IMPORT_C virtual ~CLbsPositionInfo();
    
    /**
     * Initializes the locality information. This includes
     *      latitude - should be between and including -90 and 90 degrees.
     *      longitude - should be between and including -180 and 180 degrees.
     *      altitude - Altitude information in meters.
     *      datum - datum information. Defaults to the constant for the 
     *      coordinate system for a specific positioning system.
     *      horizontal accuracy - horizontal accuracy in meters. 
     *      vertical accuracy - vertical accuracy in meters.
     *
     * @param[in] aLatitude Latitude information.
     */
    IMPORT_C void SetLocality( const TLocality& aLocality );
    
    /**
     * Retrieves the Locality information.
     *
     * @return NaN if nothing specified or latitude information.
     * network.
     */    
    IMPORT_C void GetLocality( TLocality& aLocality ) const;
    
public: // from CLbsAreaInfoBase

    /**
     * Returns the type of area information instance. 
     *
     * @return Area type information
     */
    virtual TLbsAreaInfoClassType Type();   
    
    /**
     * Validates position information.
     * This method checks for boundary conditions as well as the mandatory 
     * attributes. The latitude, longitude and horizontal accuracy attributes
     * are mandatory and must be specified in the TLocality instance being 
     * passed as an argument to instantiate/initialize an instance of this 
     * class.  
     * This method is used internally to determine the correctness of 
     * information provided. Optionally, this method, may also be used by 
     * client applications to validate their input.
     * 
     * @leave KErrArgument If the specified attributes dont meet the boundary
     * conditions or the mandatory arguments are not specified. 
     */
    virtual void ValidateDataL() const;
    
protected: // from CLbsAreaInfoBase

    /**
     * Internalize method to write GSM cell information to a stream.
     */
    virtual void DoInternaliseL( RReadStream& aStream );

    /**
     * Externalize method to read GSM cell information from a stream.
     */  
    virtual void DoExternaliseL( RWriteStream& aStream ) const;
    
private:

    /**
     * C++ default constructor
     */
    CLbsPositionInfo();

    /**
     * Copy constructor. Prohibited by default.
     */    
    CLbsPositionInfo( CLbsPositionInfo&  );    
    
    /**
     * Overloaded assignment operator. Prohibited by default.
     */    
    CLbsPositionInfo& operator=( CLbsPositionInfo& );     
    
    /**
     * Symbian 2 phase constructor
     */    
    void ConstructL();
    
    /**
     * Symbian 2 phase constuctor
     */
    void ConstructL( const TLocality& aLocality );
    
private: // attributes

    /**
     *  Locality information
     */
    TLocality iLocality;
    
    };

#endif //LBS_COORDINATE_INFO_H
