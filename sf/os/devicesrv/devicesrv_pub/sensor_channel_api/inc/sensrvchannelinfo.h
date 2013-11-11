/*
* Copyright (c) 2008 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  The channel basic information
*
*/


#ifndef SENSRVCHANNELINFO_H
#define SENSRVCHANNELINFO_H

#include <e32base.h>
#include <sensrvtypes.h>
/**
 *  The TSensrvChannelInfo class is a data class that holds information about a sensor channel.
 *
 *  It also provides simple helper methods that allow users to determine if the class is complete
 *  and if one instance of this class matches another.
 *
 *  @lib sensrvutil.lib
 *  @since S60 5.0
 */
NONSHARABLE_CLASS( TSensrvChannelInfo )
    {
public:
    
    /**
    * Default Constructor
    *
    * @since S60 5.0
    */      
    IMPORT_C TSensrvChannelInfo();
      
    /**
    * Constructor
    * 
    * This constructor is for use with CSensrvChannelFinder::FindChannelsL(). It sets the ChannelId
    * and DataItemSize to zero.
    *
    * @since S60 5.0
    * @param  aContextType Channel context type. Set to a value from TSensrvContextType.
    * @param  aQuantity Channel quantity. Set to a value from TSensrvQuantity.
    * @param  aChannelType Channel type id. This is a uid.
    * @param  aLocation Channel physical location.
    * @param  aVendorId Channel vendor name.
    * @param  aChannelDataTypeId Channel data type Id. This is a uid.
    * @see TSensrvContextType
    * @see TSensrvQuantity
    */          
    IMPORT_C TSensrvChannelInfo( TInt aContextType, 
                                 TInt aQuantity, 
                                 TSensrvChannelTypeId aChannelType,
                                 const TDesC8& aLocation,
                                 const TDesC8& aVendorId,
                                 TSensrvChannelDataTypeId aChannelDataTypeId ); 

    /**
    * Constructor
    *
    * @since S60 5.0
    * @param aChannelId Channel identifier
    * @param  aContextType Channel context type. Set to a value from TSensrvContextType.
    * @param  aQuantity Channel quantity. Set to a value from TSensrvQuantity.
    * @param  aChannelType Channel type id. This is a uid.
    * @param  aLocation Channel physical location.
    * @param  aVendorId Channel vendor name.
    * @param  aDataItemSize Data item size for channel specific data
    * @param  aChannelDataTypeId Channel data type Id. This is a uid.
    * @see    TSensrvContextType
    * @see    TSensrvQuantity 
    */          
    IMPORT_C TSensrvChannelInfo( TSensrvChannelId aChannelId, 
                                 TInt aContextType, 
                                 TInt aQuantity, 
                                 TSensrvChannelTypeId aChannelType,
                                 const TDesC8& aLocation,
                                 const TDesC8& aVendorId, 
                                 TInt aDataItemSize,
                                 TSensrvChannelDataTypeId aChannelDataTypeId ); 
      
    /**
    * Copy constructor
    * 
    * @since S60 5.0
    * @param  aChannelInfo Instance to be copied
    */    
    IMPORT_C TSensrvChannelInfo( const TSensrvChannelInfo& aChannelInfo );
    
public:     
    
    /**
    * Checks if this instance matches supplied instance. Fields that are zero or empty are ignored.
    * Channel Id, Data Item Size and Reserved fields are ignored.
    * 
    * @since S60 5.0
    * @param  aInfo Instance to be matched against this instance.
    * @return ETrue if there is a match, otherwise EFalse
    */  
    IMPORT_C TBool IsMatch( const TSensrvChannelInfo &aInfo ) const;
          
    /**
    * Checks that channel information in this object is complete. Complete means no zero or empty fields.
    * Channel Id, Data Item Size and the Reserved fields are ignored.
    * 
    * @since S60 5.0
    * @return ETrue if channel information is complete.
    */
    IMPORT_C TBool IsComplete() const;

public:
    /**
    * Channel Identifier
    */
    TSensrvChannelId iChannelId;

    /**
    * Context type
    * @see TSensrvContextType
    */
    TInt iContextType;

    /**
    * Quantity
    * @see TSensrvQuantity
    */
    TInt iQuantity;    

    /**
    * Channel type identifier
    * 
    * Identifies the channel type and therefore the content of the channel. This value is a uid and is
    * unique among all channels. See sensor channel specific header files.
    */
    TSensrvChannelTypeId iChannelType;
    
    /**
    * Location of the of the channel. This is a free format string
    */
    TBuf8<KSensrvLocationLength> iLocation;

    /**
    * Vendor name of the channel provider. This is a free format string and is not to be confused with
    * Vendor Id unique Uid's.
    */
    TBuf8<KSensrvVendorIdLength> iVendorId;

    /**
    * Size of the data item delivered by the channel
    */
    TInt iDataItemSize;
    
private:
    TInt iReserved3;
    
public:
    /**
    * Identifies the channel data type. This value is a uid and is unique among all channels. See sensor
    * channel specific header files.
    */      
    TSensrvChannelDataTypeId iChannelDataTypeId;
    
private:
    TInt iReserved;
    TInt iReserved2;
    };



#endif // SENSRVCHANNELINFO_H
