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
* WLAN MAC ID. 
*
*/

/**
* @file
* @publishedPartner
* @prototype
*/


#ifndef LBSWLANINFO_H
#define LBSWLANINFO_H


#include <e32std.h>
#include <lbs/lbslocationcommon.h>


/** 
 * Length of a WLAN MAC address.
 */
const TInt KLbsWlanMacAddressLength = 18;


/** 
 * WLAN MAC address consists of six hex numbers and is typically represented as
 * a string with 6, colon separated hexadecimal numbers. (e.g. 11:22:33:44:55:66)
 *
 * In order so save memory the LBS subsystem does not use a string. A 6 byte long
 * buffer is used instead.
 *
 * The leftmost number of the MAC address ("11" in the example) is stored in the first 
 * byte of the buffer. The rightmost number of the MAC address ("66" in the example) 
 * is stored in the last byte of the buffer.
 */
typedef TBuf8<KLbsWlanMacAddressLength> TLbsWlanMacAddress;


/**
 *  This class contains information related to a WLAN radio map object. 
 */
NONSHARABLE_CLASS(CLbsWlanInfo) : public CLbsAreaInfoBase
    {
      
public:

    /**
     * Allocates and constructs a new WLAN info object. In the returned
     * instance, the MAC ID and signal strength values are set to zero.
     * 
     * @return Pointer to the new WLAN info object.
     * @leave Standard symbian error codes, such as KErrNoMemory
     */
    IMPORT_C static CLbsWlanInfo* NewL();
    
    /**
     * Allocates and constructs a new WLAN info object. 
     *
     * The validation for the arguments is hierarchical. For instance, if 
     * signal strength is given, then specifying MAC ID is mandatory. 
     * 
     * @param[in] aMacAddress MAC address of the WLAN AP.
     * @param[in] aRxStrength AP signal strength received at the SET.

     * @return Pointer to the new WLAN info object.
     * @leave KErrArgument in case of invalid input or other standard symbian 
     * error codes, such as KErrNoMemory.
     */
    IMPORT_C static CLbsWlanInfo* NewL( const TLbsWlanMacAddress& MacAddress, 
    									const TInt aRxStrength );

    /**
     * Allocates and constructs a Wlan info object, a copy of another one.
     * 
     * @param[in] aLocality Instance of CLbsWlanInfo. 
     */

    IMPORT_C static CLbsWlanInfo* NewL( const CLbsWlanInfo& aPositionInfo);
 
    /**
     * Destructor
     */
    IMPORT_C ~CLbsWlanInfo();
    
    /**
     * Initializes the MAC ID attribute.
     *
     * @param[in] aMacAddress MAC address of the WLAN AP
     */    
    IMPORT_C void SetMacAddress( const TDesC8& aMacAddress );
    
    /**
     * Retrieves the MAC ID information.
     */       
    IMPORT_C void GetMacAddress(TDes8& aMacAddress ) const;
    
    /**
     * Initializes the AP signal strength received at the SET.
     *
     * @param[in] aRxStrength AP signal strength received at the SET, range 0..255
     */    
    IMPORT_C void SetSignalStrength( const TInt aRxStrength );
    
    /**
     * Retrieves the AP signal strength received at the SET.
     *
     * @return AP signal strength received at the SET, range 0..255
     *            KErrNotFound if the value is undefined.
     */           
    IMPORT_C TInt SignalStrength() const;
    
public: // from CLbsAreaInfoBase

    /**
     * Returns the type of area information instance. 
     *
     * @return Area type information
     */
    virtual TLbsAreaInfoClassType Type();  
    
    /**
     * Validates WLAN information.
     * This method checks for boundary conditions as well as the mandatory 
     * attributes. All attributes of this class are mandatory. 
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
     * Internalize method to write WLAN information to a stream.
     */
    virtual void DoInternaliseL( RReadStream& aStream );
  
    /**
     * Externalize method to read WLAN information from a stream.
     */      
    virtual void DoExternaliseL( RWriteStream& aStream ) const;
    
private:

    /**
     * C++ default constructor
     */
    CLbsWlanInfo();
    
    /**
     * Copy constructor. Prohibited by default.
     */    
    CLbsWlanInfo( CLbsWlanInfo& );  
    
    /**
     * Overloaded assignment operator. Prohibited by default.
     */    
    CLbsWlanInfo& operator=( CLbsWlanInfo& );     
    
    /**
     * Symbian 2 phase constructor
     */        
    void ConstructL();
    
    /**
     * Symbian 2 phase constructor 
     */
    void ConstructL( const TLbsWlanMacAddress& MacAddress,
                     const TInt aPowRx );
    
private: // data

    /**
     * MAC code
     */
    TLbsWlanMacAddress iMacAddress;
    
    /** 
     * Received signal strength
     */
    TInt16    iPowRx;
    
    };

#endif // LBSWLANINFO_H
