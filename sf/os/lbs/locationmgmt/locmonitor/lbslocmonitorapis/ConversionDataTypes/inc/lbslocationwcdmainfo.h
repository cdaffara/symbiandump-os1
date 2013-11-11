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
* cell information in a WCDMA PLMN. This class may also optionally contain
* neighbouring cell information
*
*/

/**
* @file
* @publishedPartner
* @prototype
*/


#ifndef C_LBSWCDMACELLINFO_H
#define C_LBSWCDMACELLINFO_H


#include <e32base.h>
#include <lbs/lbslocationcommon.h>
#include <lbs/lbslocationwcdmaneighbourinfo.h>


/**
 * This class contains information related to a cell in WCDMA PLMN. An instance
 * of this class may also contain information of one or more neighbouring 
 * WCDMA cells 
 */
NONSHARABLE_CLASS(CLbsWcdmaCellInfo) : public CLbsAreaInfoBase
    {
    
public:

    /**
     * Allocates and constructs a new WCDMA cell info object. In the returned
     * instance, the country code, network code and cell ID are set to zero.
     * 
     * @return Pointer to the new WCDMA cell info object.
     * @leave Standard symbian error codes, such as KErrNoMemory
     */
    IMPORT_C static CLbsWcdmaCellInfo* NewL();
     
    /**
     * Allocates and constructs a new WCDMA cell info object. 
     *
     * @param[in] aCountryCode Network country code.
     * @param[in] aNetworkCode Network code.
     * @param[in] aUniqueCellId Cell ID of a cell in WCDMA PLMN.
     * @return Pointer to the new WCDMA cell info object.
     * @leave KErrArgument in case of invalid input or other standard symbian 
     * error codes, such as KErrNoMemory.
     */    
    IMPORT_C static CLbsWcdmaCellInfo* NewL( const TInt aCountryCode,
                                             const TInt aNetworkCode,
                                             const TInt aUniqueCellId );     


	/**
     * Allocates and constructs a new WCDMA cell info object. 
     *
     * @param[in] aCountryCode Network country code.
     * @param[in] aNetworkCode Network code.
	 * @param[in] aLocalAreaCode Local Area Code (LAC)
     * @param[in] aUniqueCellId Cell ID of a cell in WCDMA PLMN.
     * @return Pointer to the new WCDMA cell info object.
     * @leave KErrArgument in case of invalid input or other standard symbian 
     * error codes, such as KErrNoMemory.
     */    

	IMPORT_C static CLbsWcdmaCellInfo* NewL( const TInt aCountryCode,
											 const TInt aNetworkCode,
											 const TInt aLocalAreaCode,
                                             const TInt aUniqueCellId );

    /**
      * Allocates and constructs a WCDMA cell info object, a copy of another one.
      * 
      * @param[in] aLocality Instance of CLbsWcdmaCellInfo. 
      */
    IMPORT_C static  CLbsWcdmaCellInfo* NewL( const CLbsWcdmaCellInfo& aPositionInfo);

    /**
     * Destructor
     */
    IMPORT_C ~CLbsWcdmaCellInfo();
    
    /**
     * Initializes the country code attribute.
     *
     * @param[in] aCountryCode Network country code, range 0..999
     */    
    IMPORT_C void SetMobileCountryCode( const TInt aCountryCode);
    
    /**
     * Retrieves the country code information.

     * @return Network country code, range 0..999
               KErrNotFound if the value is undefined.
     */        
    IMPORT_C TInt MobileCountryCode() const;
    
    /**
     * Initializes the network code attribute.
     *
     * @param[in] aNetworkCode Network code, range 0..999
     */    
    IMPORT_C void SetMobileNetworkCode( const TInt aNetworkCode );
    
    /**
     * Retrieves the network code information.
     *
     * @return Network code, range 0..999
     *        KErrNotFound if the value is undefined.
     */    
    IMPORT_C TInt MobileNetworkCode() const;


    /**
     * Sets the Local Area Code (LAC).
     *
     * @param[in] aLocalAreaCode Local Area Code (LAC), range 0..0xffff.
     */
    IMPORT_C void SetLocalAreaCode( const TInt aLocalAreaCode );
    
    /**
     * Retrieves the Local Area Code.
     *
     * @return Local Area Code (LAC), range 0..0xffff.
     *        KErrNotFound if the value is undefined.
     */    
    IMPORT_C TInt LocalAreaCode(void) const; 
    
    /**
     * Sets the Cell Identity.
     *
     * @param[in] aUniqueCellId Cell Identity, range 0..268435455.
     */
    IMPORT_C void SetUniqueCellId( const TInt aUniqueCellId );
    
    /**
     * Retrieves the Cell Identity.
     *
     * @return Cell Identity, range 0..268435455.
     *        KErrNotFound if the value is undefined.
     */    
    IMPORT_C TInt UniqueCellId() const; 
    
    /**
     * Sets the scrambling code 
     *
     * @param[in] aScramblingCode scrambling code , range 0..511.
     */    
    IMPORT_C void SetScramblingCode( const TInt aScramblingCode );
    
    /**
     * Retrieves the scrambling code .
     *
     * @return Base scrambling code, range 0..511.
     *        KErrNotFound if the value is undefined.
     */    
    IMPORT_C TInt ScramblingCode() const;
    
    /**
     * Sets the RSSI parameter 
     *
     * @param[in] aRssi Received Signal Strength Indicator, range 0..127.
     */    
    IMPORT_C void SetSignalStrength( const TInt aRssi );
    
    /**
     * Retrieves the RSSI information.
     *
     * @return RSSI, range 0..127.
     *        KErrNotFound if the value is undefined.
     */    
    IMPORT_C TInt SignalStrength() const;    

    /**
     * Adds a neighbouring cell information.
     *
     * @param[in] An instance of TLbsWcdmaNeighbourCellInfo class 
     * (memory ovnership is being transferred)
     * @leave KErrArgument if the argument is NULL or one of standard symbian 
     * error codes
     */
    IMPORT_C void AddNeighbouringCellInfoL( 
                TLbsWcdmaNeighbourCellInfo* aNeighbourCellInfo );

    /**
     * Retrieves a copy of neighbouring cell information information as an 
     * array. 
     *
     * @param[in] aNeighbourCellInfo WCDMA neighbouring cell information 
     * (memory ownership is not being transferred to this function therefore
	 * it is the responsibility of the caller to ensure Close() is called to 
	 * free resources)
     */
    IMPORT_C void GetNeighbouringCellInfoL( 
                RLbsWcdmaNeighbourCellInfoArray& aNeighbourCellInfo ) const;

    /** 
     * Removes all neighbouring cell information.
     */    
    IMPORT_C void ResetNeighbouringCellInfo();

public: // from CLbsAreaInfoBase

    /**
     * Returns the type of area information instance. 
     *
     * @return Area type information
     */
    virtual TLbsAreaInfoClassType Type();   
    
    /**
     * Validates WCDMA information.
     * This method checks for boundary conditions as well as the mandatory 
     * attributes. The attributes of this class become mandatory in a 
     * hierarchical manner. For instance, if scrambling code is specified then, 
     * all other attributes except the neighbouring cell information become 
     * mandatory.  However, the mobile country code attribute is always 
     * mandatory which means client applications must at least specify the MCC 
     * attribute when specifying WCDMA cell information. 
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
     * Internalize method to write WCDMA cell information to a stream.
     */
    virtual void DoInternaliseL( RReadStream& aStream );
  
    /**
     * Externalize method to read WCDMA cell information from a stream.
     */  
    virtual void DoExternaliseL( RWriteStream& aStream ) const;
    
private:

    /**
     * C++ default constructor
     */
    CLbsWcdmaCellInfo();

    /**
     * Copy constructor. Prohibited by default.
     */    
    CLbsWcdmaCellInfo( CLbsWcdmaCellInfo& );    
    
    /**
     * Overloaded assignment operator. Prohibited by default.
     */    
    CLbsWcdmaCellInfo& operator=( CLbsWcdmaCellInfo& );     
    
    /**
     * Symbian 2 phase constructor
     */    
    void ConstructL();
    
    /**
     * Symbian 2 phase constructor # 2
     */
    void ConstructL( const TInt aCountryCode, 
                     const TInt aNetworkCode,
					 const TInt aLocalAreaCode,
                     const TInt aUniqueCellId );
    
private: // data

    enum _TWcdmaDataValidationMask
        {
        EWcdmaDataNull = 0x00,
        EWcdmaMCC = 0x01,
        EWcdmaMNC = 0x02,
        EWcdmaUCid = 0x04,
        EWcdmaSC = 0x08,
        EWcdmaRSSI = 0x10,
		EWcdmaLAC = 0x20,
        };
    
    /**
     * Mobile country code
     */
    TInt16      iMCC;
    
    /**
     * Mobile network code
     */
    TInt16      iMNC;
    
    /**
     * Unique cell identifier
     */
    TInt32      iUCid;
    
    /**
     * Scrambling code
     */
    TInt16       iSCode;
    
    /**
     * RSSI
     */
    TInt16       iRssi;
    
    /**
     * Data mask to determine initialized data. Used internally.
     */
    TInt8       iDataValidationMask;
    
    /**
     * Neighbouring cell info
     */
    RLbsWcdmaNeighbourCellInfoArray  iNeighbourCellInfo;

    /**
     * Local Area Code (LAC)
     */
	TInt32		iLAC;
    
    };

#endif // C_POSWCDMACELLINFO_H
