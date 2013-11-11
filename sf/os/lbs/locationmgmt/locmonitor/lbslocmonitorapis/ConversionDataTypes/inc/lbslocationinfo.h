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
* Description: Container class representing geographical area information as a
* collection of GSM/WCDMA cell information and WLAN MAC IDs.
*
*/


/**
* @file
* @publishedPartner
* @prototype
*/


#ifndef C_LBSLOCATIONINFO_H
#define C_LBSLOCATIONINFO_H


#include <e32std.h>
#include <lbs/lbslocationcommon.h>
#include <lbs/lbslocationwcdmainfo.h>
#include <lbs/lbslocationgsminfo.h>
#include <lbs/lbslocationwlaninfo.h>
#include <lbs/lbslocationposinfo.h>


/**
 * Type definitions
 */
 
/**
 * Structure used to carry location information to be converted
 */
typedef RPointerArray<CLbsAreaInfoBase> RLbsAreaInfoBaseArray;

/**
 * This class contains information related to a collection of geographical area
 * information. Client applications create an instance of this class to 
 * represent a geographical area for which a conversion is required. The 
 * corresponding coordinate values determined by the conversion plugins would 
 * be updated in the same instance of CLbsLocationInfo. Client applications
 * may retrieve the position estimate(s) using GetPositionInfo method.
 *
 */
NONSHARABLE_CLASS(CLbsLocationInfo) : public CLbsLocationInfoBase
    {

public: 
    
	typedef TUint32 TAreaInfoMask;
    
	enum _TAreaInfoMask
        {
        /**
         * Bit mask used to retrieve position information.
         */
        ELbsPosInfo = 0x01,
        
        /**
         * Bit mask used to retrieve GSM cell information. 
         */
        ELbsGsmInfo = 0x02,
        
        /**
         * Bit mask used to retrieve WCDMA cell information.
         */
        ELbsWcdmaInfo = 0x04,
        
        /**
         * Bit mask used to retrieve WLAN AP information.
         */
        ELbsWlanInfo = 0x08,
        
        /**
         * Bit mask used to retrieve all area information.
         */
        ELbsAreaInfoAll = 0xFFFFFFFF
        };

    /**
     * Allocates and constructs a new location info object. 
     * 
     * @return Pointer to the new area info object.
     * @leave Standard symbian error codes, such as KErrNoMemory
     */
    IMPORT_C static CLbsLocationInfo* NewL();
  
    /**
     * Allocates and constructs a new location info object. This method will
     * push the resultant object onto the cleanup stack. 
     * 
     * @return Pointer to the new area info object.
     * @leave Standard symbian error codes, such as KErrNoMemory
     */    
    IMPORT_C static CLbsLocationInfo* NewLC();
   
    /**
     * Destructor
     */
    IMPORT_C ~CLbsLocationInfo();
     
    /**
     * Appends the specified area info entry to the data structure. This
     * method takes a pointer to CLbsAreaInfoBase type instances.  
     * 
     * @param[in] aAreaInfo Pointer to a CLbsAreaInfoBase type instance.
     * @leave KErrArgument if the pointer to CLbsAreaInfoBase type is NULL
     */
    IMPORT_C void AddAreaInfoL( CLbsAreaInfoBase* aAreaInfo );
    
    /**
     * Retrieves the area information that is stored in the internal 
     * data structure. Client applications can specify the type of information
     * to be retrieved using the TAreaInfoMask. The result is updated in 
     * the instance of RLbsAreaInfoBaseArray. If there is no information stored
     * for the requested type the instance of RLbsAreaInfoBaseArray will not be
     * updated. Client applications would need to check the contents of the 
     * array to determine the information retrieved.
     * 
     * @param[out] aAreaInfoArray Array that is filled with pointers to the 
	 * area information retrieved. Note it is the callers responsibility to call
	 * Close() to free the pointers. However ownership of the objects pointed to
	 * is not transferred to the caller.
     * @param[in] aLocInfoMask Specifies the type of information to be 
     * retrieved. 
     */
    IMPORT_C void GetAreaInfoL( RLbsAreaInfoBaseArray& aAreaInfoArray, 
                               TAreaInfoMask aAreaInfoMask = ELbsAreaInfoAll );
    
    /**
     * Removes all entries of the specified type from the data structure. If 
     * the mask is not specified all entries would be removed. If there are no
     * entries found for a specific type, no changes will be done.
     * 
     * @param[in] aLocInfoMask Specifies the type of information to be 
     * removed. 
     */    
    IMPORT_C void ResetAreaInfo( TAreaInfoMask aAreaInfoMask 
                                     = ELbsAreaInfoAll );
             
protected: // from CLbsLocationInfoBase
    
    /**
     * Type of location info class
     */
    virtual TLbsLocationInfoClassType Type();

    /**
     * Internalize method to write location information to a stream.
     */
    virtual void DoInternaliseL( RReadStream& aStream );
  
    /**
     * Externalize method to read location information from a stream.
     */  
    virtual void DoExternaliseL( RWriteStream& aStream ) const;
    
private:
   
    /**
     * C++ default constructor
     */   
    CLbsLocationInfo();
   
    /**
     * Copy constructor. Prohibited by default.
     */    
    CLbsLocationInfo( CLbsLocationInfo& );
   
    /**
     * Overloaded assignment operator. Prohibited by default.
     */    
    CLbsLocationInfo& operator = ( CLbsLocationInfo& );
   
    /**
     * Symbian 2 phase constructor
     */    
    void ConstructL();
   
private:

    /**
     * A collection of geographical area information.
     */
    RPointerArray<CLbsAreaInfoBase>  iAreaInfoArray;
    
    };

typedef RPointerArray<CLbsLocationInfo> RLbsLocationInfoArray;

   
#endif //C_LBSLOCATIONINFO_H
