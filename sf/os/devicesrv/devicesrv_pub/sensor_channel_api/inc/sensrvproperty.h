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
* Description:  Sensor and channel property container
*
*/


#ifndef SENSRVPROPERTY_H
#define SENSRVPROPERTY_H

//  INCLUDES
#include <e32base.h>
#include <babitflags.h>

// CONSTANTS

/**
* Maximum size of the buffer when the property type is ESensrvBufferProperty
* @see TSensrvProperty
*/
const TInt KSensrvPropertyTextBufferSize = 20;


// DATA TYPES

/**
* Property types
* @see TSensrvProperty
*/
enum TSensrvPropertyType
    {
    /** Default property type */
    ESensrvUninitializedProperty = 0,
    /** Integer property type. TInt is used to store property value */
    ESensrvIntProperty,
    /** Real property type. TReal is used to store property value */
    ESensrvRealProperty,
    /**
    Descriptor property type. TBuf<KSensrvPropertyTextBufferSize> is used to store
    property value
    */
    ESensrvBufferProperty
    };

/**
* Special index types for iArrayIndex.  Any other value indicates that this property is a member of
* an array of properties
* @see TSensrvProperty
*/
enum TSensrvArrayIndex
    {
    /** This property is a single property and is not a member of an array of properties */
    ESensrvSingleProperty    = -1,
    /**
    This property is the information item for an array of properties. iIntValueMin and iIntValueMax
    values in a property of this type defines the range of the property array
    */
    ESensrvArrayPropertyInfo = -2
    };

/**
* Identifies the range of values that must be used for TSensrvPropertyId. These ranges are defined so that
* any customers wishing to add additional properties not yet supported by Symbian can do so without
* causing clashes with Symbian defined properties. 
* 1-4095      General channel properties owned and defined by Symbian
* 4096-8191   Specific Channel properties owned and defined by Symbian
* 8192-12287  A range for customers to define their own properties. Usage defined by customer
* @see TSensrvPropertyId
* @see TSensrvProperty
*/
enum TSensrvPropertyRangeUsage
    {
    // General properties
    ESensrvPropertyRangeNotDefined = 0, // 0x0000
    ESensrvGeneralPropertyRangeBase = 1, // 0x0001
    ESensrvGeneralPropertyRangeEnd = 4095, // 0x0FFF

    // Channel properties
    ESensrvChannelPropertyRangeBase = 4096, // 0x1000
    ESensrvChannelPropertyRangeEnd = 8191, // 0x1FFF

    // A range for licensees to define their own properties. Usage is defined by the licensee. 
    ESensrvLicenseePropertyRangeBase = 8192, // 0x2000
    ESensrvLicenseePropertyRangeEnd = 12287 // 0x2FFF
    };

/**
* Type definition for property identifier.
* @see TSensrvProperty
*/
typedef TUint32 TSensrvPropertyId;

// CLASS DECLARATION

/**
* TSensrvProperty is a simple data class that contains sensor channel property information. The class has
* little behaviour and only does sanity checks where it is possible. It is therefore assumed that users of
* this class are aware of and conform to a properties definition for a given property identifier.
* 
* There are 2 groups of properties, General Channel Properties and Specific Channel Properties.
* 
* General Channel Properties are properties that are supported by all channels. These are defined in 
* sensrvgeneralproperties.h. Specific Channel Properties are properties that are supported by specific 
* channel types as required. These are defined in the relevant headers files.
* 
* The property identifier uniquely identifies the property within a channel. These property ids must fall
* into the relevant range specified by TSensrvPropertyRangeUsage. Each property will have a constant 
* defined for its identifier. e.g. KSensrvPropIdDataRate.
* 
* @see TSensrvProperty::SetPropertyId()
* @see TSensrvProperty::GetPropertyId()
* @see TSensrvPropertyRangeUsage
* @see KSensrvPropIdDataRate
* 
* The property type is used to determine whether the property is TInt, TReal or Buffer. The property type
* is automatically set when the value of the object is set. 
* 
* @see TSensrvPropertyType
* @see TSensrvProperty::PropertyType()
* 
* The value fields are used to indicate the value of a property when the property contains a single int,
* real or buffer value. The min and max value fields are used when a property has a range of int or real
* values. A property cannot have a range of buffer values. The value and min/max values of a property
* will never be set at the same time.
* 
* @see TSensrvProperty::GetValue()
* @see TSensrvProperty::SetValue()
* @see TSensrvProperty::GetValueRef()
* @see TSensrvProperty::GetMinValue()
* @see TSensrvProperty::SetMinValue()
* @see TSensrvProperty::GetMaxValue()
* @see TSensrvProperty::SetMaxValue()
* 
* Properties can be read only which means it cannot be set by CSensrvChannel::SetPropertyL().
* 
* @see TSensrvProperty::ReadOnly()
* @see TSensrvProperty::SetReadOnly()
* @see CSensrvChannel
* 
* Properties can either be single properties, or can be a member of an array of properties. The array
* index field is set to ESensrvSingleProperty(-1) if the property is single. If a property is part of an
* array then the array index field will be set to ESensrvArrayPropertyInfo(-2) or the index for the array.
* When the array index is ESensrvArrayPropertyInfo the property uses the iIntValueMin and iIntValueMax fields
* to indicate the range of the array. When the array index is an actual index (neither ESensrvArrayPropertyInfo
* nor ESensrvArrayPropertyInfo) the property contains data.
* 
* Example:
* 
* A Sensor channel which supports data rates within a given range can use one Data Rate
* property. The range is defined using the property's maximum and minimum value. E.g. A
* sensor channel supports a data range from 10Hz to 100Hz and all values within this range
* are feasible. To implement this use one Data Rate property with a minimum value 10 and a
* maximum value of 100.
* 
* A Sensor channel which supports discrete data rates can use the Data Rate property as
* an array. E.g. A sensor channel supports the following data rates 10Hz, 40Hz and 50Hz.
* To implement this four different Data Rate properties are needed. The following table
* shows the content of the four properties, only mandatory attributes are shown.
*  
* @code
* property ID     Array index     Value           Min Value       Max Value       Read only
* -----------     -----------     -----           ---------       ---------       ---------
* 0x00000002      -2              1               0               2               EFalse
* 0x00000002      0               10              n/a             n/a             ETrue
* 0x00000002      1               40              n/a             n/a             ETrue
* 0x00000002      2               50              n/a             n/a             ETrue
* @endcode
* 
* The first property (first row in table above) is the header for the property array. It
* defines that this property is an array property. The attributes of this property are:
* 
* Array index:
* -2 means that the property is an array property
* Value:
* 1 means that current value of the property is defined by the property with an array index of 
* 1. The value is 40Hz in this example.
* Min value 
* 0 is the start index of the property array 
* Max value:
* 2 is the last index of the property array
* Read only:
* EFalse means that the properties current value can be changed,
* 
* In this example the possible values of the property are 0, 1, and 2 which corresponds to data
* rates of 10Hz, 40Hz and 50Hz.
* 
* @see TSensrvArrayIndex
* @see TSensrvProperty::SetArrayIndex()
* @see TSensrvProperty::GetArraIndex()
* 
* The property item index is used to identify a specific item in the channel data structure that the property
* refers to. For example the Accelerometer XYZ-axis data structure defines data for the x, y and Z axis.
* Individual properties could have been defined for each of these axis. Instead a single property is defined
* and the Item Index is used to identify the relevant axis in the channel data structure that the property
* applies to. The indexes for these specific items and the channel data structures themselves are defined in
* the sensor specific headers files found in epoc32\include\sensors\channels.
* 
* Example:
* 
* Accelerometer data definition. TSensrvAccelerometerAxisDataIndexes is used to provide an index that
* relates to each of the class fields.
* 
* @code
* // ACCELEROMETER RELATED DATATYPES
* 
* // Accelerometer axis data type
* 
* class TSensrvAccelerometerAxisData 
*     {
* public:
*     //
*     // Channel data type Id number
*     //      
*     static const TSensrvChannelDataTypeId KDataTypeId = 0x1020507E;
* 
*     //
*     // Channel data type index numbers
*     //
* 	enum TSensrvAccelerometerAxisDataIndexes
*           {
*            ETimeStamp = 0,
*            EAxisX,
*            EAxisY,
*            EAxisZ
*           }; 
* public:
*     //
*     // - Item name:   Sampling time.
*     // - Item Index:  0
*     // - Conditions:  None
*     // - Description: Timestamp for a sample.
*     //   
*     TTime iTimeStamp;    
* 
*     //
*     // - Item name:   Accelerometer x-axis 
*     // - Item Index:  1
*     // - Conditions:  Single limit and range 
*     // - Description: Accelerometer values from x-axis 
*     //
*     TInt iAxisX;  
* 
*     //
*     // - Item name:   Accelerometer y-axis 
*     // - Item Index:  2
*     // - Conditions:  Single limit and range 
*     // - Description: Accelerometer values from y-axis 
*     // 
*     TInt iAxisY;  
* 
*     //
*     // - Item name:   Accelerometer z-axis 
*     // - Item Index:  3
*     // - Conditions:  Single limit and range 
*     // - Description: Accelerometer values from z-axis 
*     //
*     TInt iAxisZ;  
*     };
* @endcode
* 
* @see TSensrvProperty::SetItemIndex()
* @see TSensrvProperty::PropertyItemIndex()
* @lib sensrvutil.lib
* @since S60 5.0
*/
NONSHARABLE_CLASS( TSensrvProperty )
    {
public:

    /**
    Default Constructor
    */
    IMPORT_C TSensrvProperty();

    /**
    * Constructor to create complete integer type property.
    * 
    * @since S60 5.0
    * @param  aPropertyId Id of the property
    * @param  aItemIndex Item index of the property. Mapped to a specific item in channel data structure.
    * @param  aValue Value of the property.
    * @param  aMaxValue Maximum value of the property
    * @param  aMinValue Minimum value of the property
    * @param  aReadOnly Readonly-flag of the property
    * @param  aPropertyType Type of the property
    */
    IMPORT_C TSensrvProperty( const TSensrvPropertyId aPropertyId,
                              const TInt aItemIndex,
                              const TInt aValue,
                              const TInt aMaxValue,
                              const TInt aMinValue,
                              const TBool aReadOnly,
                              const TSensrvPropertyType aPropertyType );

    /**
    * Constructor to create property object for setting integer property value.
    * 
    * @since S60 5.0
    * @param  aPropertyId Id of the property
    * @param  aItemIndex Item index of the property. Mapped to a specific item in channel data structure.
    * @param  aValue Value of the property.
    */
    IMPORT_C TSensrvProperty( const TSensrvPropertyId aPropertyId,
                              const TInt aItemIndex,
                              const TInt aValue );

    /**
    * Constructor to create complete TReal type of property.
    *
    * @since S60 5.0
    * @param  aPropertyId Id of the property
    * @param  aItemIndex Item index of the property. Mapped to a specific item in channel data structure.
    * @param  aValue Value of the property.
    * @param  aMaxValue Maximum value of the property
    * @param  aMinValue Minimum value of the property
    * @param  aReadOnly Readonly-flag of the property
    * @param  aPropertyType Type of the property
    */
    IMPORT_C TSensrvProperty( const TSensrvPropertyId aPropertyId,
                              const TInt aItemIndex,
                              const TReal aValue,
                              const TReal aMaxValue,
                              const TReal aMinValue,
                              const TBool aReadOnly,
                              const TSensrvPropertyType aPropertyType );

    /**
    * Constructor to create property object for setting real property value.
    * 
    * @since S60 5.0
    * @param  aPropertyId Id of the property
    * @param  aItemIndex Item index of the property. Mapped to a specific item in channel data structure.
    * @param  aValue Value of the property.
    */
    IMPORT_C TSensrvProperty( const TSensrvPropertyId aPropertyId,
                              const TInt aItemIndex,
                              const TReal aValue );

    /**
    * Constructor to create complete buffer property.
    * 
    * @since S60 5.0
    * @param  aPropertyId Id of the property
    * @param  aItemIndex Item index of the property. Mapped to a specific item in channel data structure.
    * @param  aValue Value of the property. Max length is defined in KSensrvPropertyTextBufferSize.
    * @param  aReadOnly Readonly-flag of the property
    * @param  aPropertyType Type of the property
    */
    IMPORT_C TSensrvProperty( const TSensrvPropertyId aPropertyId,
                              const TInt aItemIndex,
                              const TDesC8& aValue,
                              const TBool aReadOnly,
                              const TSensrvPropertyType aPropertyType );

   /**
    * Constructor to create property object for setting buffer property value.
    * 
    * @since S60 5.0
    * @param  aPropertyId Id of the property
    * @param  aItemIndex Item index of the property. Mapped to a specific item in channel data structure.
    * @param  aValue Value of the property. Max length is defined in KSensrvPropertyTextBufferSize.
    */
    IMPORT_C TSensrvProperty( const TSensrvPropertyId aPropertyId,
                              const TInt aItemIndex,
                              const TDesC8& aValue );

    /**
    * Set the property Id for the property.
    * 
    * @since S60 5.0
    * @param  aPropertyId Property Id to be set.
    */
    IMPORT_C void SetPropertyId( TSensrvPropertyId aPropertyId );

    /**
    * Return Property Id
    * 
    * @since S60 5.0
    * @return Property Id
    */
    IMPORT_C TSensrvPropertyId GetPropertyId() const;

    /**
    * Set the property item index for the property.
    * 
    * @since S60 5.0
    * @param  aItemIndex property item index to be set.
    */
    IMPORT_C void SetItemIndex( TInt aItemIndex );

    /**
    * Get the property item index number
    * 
    * @since S60 5.0
    * @return Item index number.
    */
    IMPORT_C TInt PropertyItemIndex() const;

    /**
    * Return property type
    * 
    * @since S60 5.0
    * @return Property type's Id
    */
    IMPORT_C TSensrvPropertyType PropertyType() const;

    /**
    * Checks if the property is readonly
    *
    * @since S60 5.0
    * @return ETrue value indicates that property is a read only property.
    */
    IMPORT_C TBool ReadOnly() const;

    /**
    * Set the readonly information for the property.
    * 
    * @since S60 5.0
    * @param  aReadOnly readonly information to be set. EFalse means that the property is not readonly.
    */
    IMPORT_C void SetReadOnly( TBool aReadOnly );

    /**
    * Set integer property value
    * 
    * @since S60 5.0
    * @param  aValue Value to be set.
    * @panic  KSensrvPanicCategory ESensrvPanicInvalidPropertyType Panic if the property object is not
    * a correct type
    */
    IMPORT_C void SetValue( const TInt aValue );

    /**
    * Set real property value
    * 
    * @since S60 5.0
    * @param  aValue Value to be set.
    * @panic  KSensrvPanicCategory ESensrvPanicInvalidPropertyType Panic if the property object is not
    * a correct type
    */
    IMPORT_C void SetValue( const TReal aValue );

    /**
    * Set buffer property value
    * 
    * @since S60 5.0
    * @param  aValue Value to be set.
    * @panic  KSensrvPanicCategory ESensrvPanicInvalidPropertyType Panic if the property object is not
    * a correct type
    */
    IMPORT_C void SetValue( const TDesC8& aValue );

    /**
    * Get integer property value
    * 
    * @since S60 5.0
    * @param  aValue Value to get
    * @panic  KSensrvPanicCategory ESensrvPanicInvalidPropertyType Panic if the property object is not
    * a correct type
    */
    IMPORT_C void GetValue( TInt& aValue ) const;

    /**
    * Get real property value
    * 
    * @since S60 5.0
    * @param  aValue Value to get
    * @panic  KSensrvPanicCategory ESensrvPanicInvalidPropertyType Panic if the property object is not
    * a correct type
    */
    IMPORT_C void GetValue( TReal& aValue ) const;

    /**
    * Get buffer property value
    * 
    * @since S60 5.0
    * @param  aValue Destination where a property value is to be copied
    * @panic  KSensrvPanicCategory ESensrvPanicInvalidPropertyType Panic if the property object is not
    * a correct type
    */
    IMPORT_C void GetValue( TDes8& aValue ) const;

    /**
    * Get reference to buffer property value
    * 
    * @since S60 5.0
    * @return Reference to a property value
    * @panic  KSensrvPanicCategory ESensrvPanicInvalidPropertyType Panic if the property object is not
    * a correct type
    */
    IMPORT_C TDes8& GetValueRef();

    /**
    * Get property maximum value of integer property
    * 
    * @since S60 5.0
    * @param  aMaxValue Maximum value to get
    * @panic  KSensrvPanicCategory ESensrvPanicInvalidPropertyType Panic if the property object is not
    * a correct type
    */
    IMPORT_C void GetMaxValue( TInt& aMaxValue ) const;

    /**
    * Get property minimum value of integer property
    * 
    * @since S60 5.0
    * @param  aMinValue Minimum value to get
    * @panic  KSensrvPanicCategory ESensrvPanicInvalidPropertyType Panic if the property object is not
    * a correct type
    */
    IMPORT_C void GetMinValue( TInt& aMinValue ) const;

    /**
    * Set property maximum value of integer property
    * 
    * @since S60 5.0
    * @param  aMaxValue Maximum value to be set
    * @panic  KSensrvPanicCategory ESensrvPanicInvalidPropertyType Panic if the property object is not
    * a correct type
    */
    IMPORT_C void SetMaxValue( TInt aMaxValue );

    /**
    * Set property minimum value of integer property
    * 
    * @since S60 5.0
    * @param  aMinValue Minimum value to be set
    * @panic  KSensrvPanicCategory ESensrvPanicInvalidPropertyType Panic if the property object is not
    * a correct type
    */
    IMPORT_C void SetMinValue( TInt aMinValue );

    /**
    * Get property maximum value of real property
    * 
    * @since S60 5.0
    * @param  aMaxValue Maximum value to get
    * @panic  KSensrvPanicCategory ESensrvPanicInvalidPropertyType Panic if the property object is not
    * a correct type
    */
    IMPORT_C void GetMaxValue( TReal& aMaxValue ) const;

    /**
    * Get property minimum value of real property
    * 
    * @since S60 5.0
    * @param aMinValue Minimum value to get
    * @panic  KSensrvPanicCategory ESensrvPanicInvalidPropertyType Panic if the property object is not
    * a correct type
    */
    IMPORT_C void GetMinValue( TReal& aMinValue ) const;

    /**
    * Set property maximum value of real property
    * 
    * @since S60 5.0
    * @param  aMaxValue Maximum value to be set.
    * @panic  KSensrvPanicCategory ESensrvPanicInvalidPropertyType Panic if the property object is not
    * a correct type
    */
    IMPORT_C void SetMaxValue( const TReal& aMaxValue );

    /**
    * Set property minimum value of real property
    * 
    * @since S60 5.0
    * @param  aMinValue Minimum value to be set.
    * @panic  KSensrvPanicCategory ESensrvPanicInvalidPropertyType Panic if the property object is not
    * a correct type
    */
    IMPORT_C void SetMinValue( const TReal& aMinValue );

    /**
    * Set security info of the property. This should be set to the Security info for the client process
    * 
    * @since S60 5.0
    * @param aSecurityInfo Security info to be set
    */
    IMPORT_C void SetSecurityInfo( const TSecurityInfo& aSecurityInfo );

    /**
    * Get security info of the property
    * 
    * @since S60 5.0
    * @return Security info of the property
    */
    IMPORT_C TSecurityInfo GetSecurityInfo() const;

    /**
    * Set array index of the property.
    * 
    * @since S60 5.0
    * @param aArrayIndex Array index to be set
    * @see TSensrvArrayIndex
    * @see KSensrvPropIdDataRate for an example
    */
    IMPORT_C void SetArrayIndex( const TInt aArrayIndex );

    /**
    * Get array index of the property
    * 
    * @since S60 5.0
    * @return Array index of the property.
    * @see TSensrvArrayIndex
    * @see KSensrvPropIdDataRate for an example
    */
    IMPORT_C TInt GetArrayIndex() const;

private:
    // property identifier
    TSensrvPropertyId iPropertyId;

    // property item index
    TInt iItemIndex;

    //Array index
    TInt16 iArrayIndex;


    // property value
    union
        {
        TInt iIntValue;
        TReal iRealValue;
        };
    TBuf8<KSensrvPropertyTextBufferSize> iBufValue;

    // read only flag
    TBitFlags32 iFlags;

    // maximum value
    union
        {
        TInt iIntValueMax;
        TReal iRealValueMax;
        };

    // minimum value
    union
        {
        TInt iIntValueMin;
        TReal iRealValueMin;
        };

    // property type
    TSensrvPropertyType iPropertyType;

    // property security infomation
    TSecurityInfo iSecurityInfo;

    // not used
    TInt iReserved;
    };

#endif //SENSRVPROPERTY_H

// End of File


