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
* cell information in a GSM PLMN. This class may also optionally contain
* neighbouring cell information
*
*/


/**
* @file
* @publishedPartner
* @prototype
*/


#ifndef LBS_GSM_CELL_INFO_H
#define LBS_GSM_CELL_INFO_H


#include <e32std.h>
#include <lbs/lbslocationcommon.h>
#include <lbs/lbslocationgsmneighbourinfo.h>


/**
 * This class contains information related to a cell in GSM PLMN. An instance
 * of this class may also contain information of one or more neighbouring 
 * GSM cells 
 */
NONSHARABLE_CLASS(CLbsGsmCellInfo) : public CLbsAreaInfoBase
    {

public:

    /**
     * Allocates and constructs a new GSM cell info object. In the returned
     * instance, the country code, network code, location area code and cell ID
     * are marked as undefined.
     * 
     * @return Pointer to the new GSM cell info object.
     * @leave Standard symbian error codes, such as KErrNoMemory
     */
    IMPORT_C static CLbsGsmCellInfo* NewL();
    
    /**
     * Allocates and constructs a new GSM cell info object. 
     *
     * @param[in] aCountryCode Network country code.
     * @param[in] aNetworkCode Network code.
     * @param[in] aLocationAreaCode Location area code.
     * @param[in] aCellId Cell ID of a cell in GSM PLMN.
     * @return Pointer to the new GSM cell info object.
     * @leave standard symbian error codes, such as KErrNoMemory.
     */
    IMPORT_C static CLbsGsmCellInfo* NewL( const TInt aCountryCode,
                                           const TInt aNetworkCode,
                                           const TInt aLocationAreaCode,
                                           const TInt aCellId );
        
  
    /**
      * Allocates and constructs new GSM cell info objec, a copy of another one.
      * 
      * @param[in] aLocality Instance of CLbsGsmCellInfo. 
      */
     IMPORT_C static CLbsGsmCellInfo* NewL( const CLbsGsmCellInfo& aPositionInfo );
         
    
    /**
     * Destructor
     */    
    IMPORT_C ~CLbsGsmCellInfo();
    
    /**
     * Sets the Mobile Country Code information.
     * 
     * @param[in] aCountryCode Mobile Country Code, range 0..999. 
     */
    IMPORT_C void SetMobileCountryCode(const TInt aCountryCode);
    
    /**
     * Retrieves the Mobile Country Code information.
     * 
     * @return Mobile Country Code, range 0..999;
     *        KErrNotFound if the value is undefined.
     */    
    IMPORT_C TInt MobileCountryCode() const;
    
    /**
     * Sets the Mobile Network Code information.
     *
     * @param[in] aNetworkCode Mobilre Network Code, range 0..999. 
     */    
    IMPORT_C void SetMobileNetworkCode( const TInt aNetworkCode );
    
    /**
     * Retrieves the Mobile Network Code information.
     *
     * @return Mobile Network Code, range 0..999. 
     *        KErrNotFound if the value is undefined.
     */    
    IMPORT_C TInt MobileNetworkCode() const;
    
    /**
     * Sets the Location Area Code information.
     *
     * @param[in] aLocationAreaCode Location Area Code, range 0..65535. 
     */    
    IMPORT_C void SetLocationAreaCode( const TInt aLocationAreaCode );  
    
    /**
     * Retrieves the Location Area Code information.
     *
     * @return Location Area Code, range 0..65535. 
     *        KErrNotFound if the value is undefined.
     */    
    IMPORT_C TInt LocationAreaCode() const;
    
    /**
     * Sets the Cell Identity information.
     * 
     * @param[in] aCellId Cell Identity, range 0..65535. 
     */
    IMPORT_C void SetCellId( const TInt aCellId ); 
    
    /**
     * Retrieves the Cell Identity information.
     * 
     * @return Cell Identity, range 0..65535.
     *        KErrNotFound if the value is undefined.
     */
    IMPORT_C TInt CellId() const;
    
    /**
     * Initializes the signal strength parameter.
     * 
     * @param[in] aRxLev Rx Level, range 0..63. 
     */
    IMPORT_C void SetRxLevel( const TInt aRxLev );
    
    /**
     * Retrieves the Rx level.
     *
     * @return Rx Level, range 0..63. 
     *        KErrNotFound if the value is undefined.
     */    
    IMPORT_C TInt RxLevel() const;    
    
    /**
     * Sets the Timing Advance information.
     *
     * @param[in] aTimingAdvance Timing Advance, range 0..255.
     */    
    IMPORT_C void SetTimingAdvance( const TInt aTimingAdvance );
    
    /**
     * Retrieves the Timing Advance information.
     *
     * @return Timing Advance, range 0..255.
     *        KErrNotFound if the value is undefined.
     */
    IMPORT_C TInt TimingAdvance() const;
    
    /**
     * Adds a neighbouring cell information.
     *
     * @param[in] An instance of TLbsGsmNeighbourCellInfo class 
     * (memory ovnership is being transferred)
     * @leave KErrArgument if the argument is NULL or one of standard symbian 
     * error codes
     */
    IMPORT_C void AddNeighbouringCellInfoL( 
                    TLbsGsmNeighbourCellInfo* aNeighbourCellInfo );

    /**
     * Retrieves a copy of neighbouring cell information information as an 
     * array. 
     *
     * @param[in] aNeighbourCellInfo GSM neighbouring cell information 
     * (memory ownership is not being transferred to this function therefore
	 * it is the responsibility of the caller to ensure Close() is called to 
	 * free resources)
     */
    IMPORT_C void GetNeighbouringCellInfoL( 
                     RLbsGsmNeighbourCellInfoArray& aNeighbourCellInfo ) const;

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
     * Validates GSM cell information. 
     * This method checks for boundary conditions as well as the mandatory 
     * attributes. The attributes of this class become mandatory in a 
     * hierarchical manner. For instance, if location area code is 
     * specified then the mobile network code attribute is mandatory. If cell 
     * id is specified then, both LAC and MNC become mandatory attributes and 
     * so on. However, the mobile country code attribute is always mandatory 
     * which means client applications must at least specify the MCC attribute 
     * when specifying GSM cell information. 
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
    CLbsGsmCellInfo();
   
    /**
     * Copy constructor. Prohibited by default.
     */    
    CLbsGsmCellInfo( CLbsGsmCellInfo& ); 
    
    /**
     * Overloaded assignment operator. Prohibited by default.
     */    
    CLbsGsmCellInfo& operator=( CLbsGsmCellInfo& );     
    
    /**
     * Symbian 2 phase constructor
     */    
    void ConstructL();
    
    /**
     * Symbian 2 phase constructor
     */
    void ConstructL( const TInt aCountryCode, const TInt aNetworkCode,
                     const TInt aLocationAreaCode, const TInt aCellId );
    
private: // attributes
    
    enum _TGsmDataValidationMask
        {
        EGsmDataNull = 0x00,
        EGsmMCC = 0x01,
        EGsmMNC = 0x02,
        EGsmLAC = 0x04,
        EGsmCI = 0x08,
        EGsmRx = 0x10,
        EGsmTA = 0x20
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
     * Location area code
     */
    TInt32      iLAC;
    
    /**
     * Cell id
     */
    TInt32      iCid;

    /**
     * Signal strength
     */
    TInt32      iRxLev;
    
    /**
     * Timing advance
     */
    TInt16      iTA;
    
    /**
     * Data mask to determine initialized data. Used internally.
     */
    TInt8       iDataValidationMask;
    
    /**
     * Neighbouring cell info
     */
    RLbsGsmNeighbourCellInfoArray iNeighbourCellInfo;
    };

#endif //LBS_GSM_CELL_INFO_H
