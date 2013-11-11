/*
* Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:
* Name        : LbsFieldIds.h
* Part of     : Location Framework/Location FW
* Interface   : Location Acquisition API
* Position data Fields
* Version     : %version: 4 %
*
*/



#ifndef __LBSFIELDIDS_H__
#define __LBSFIELDIDS_H__

#include <e32std.h>

/* This file contains the enumerations for the different positioning
 * related fields that a positioning technology can return.
 */
 
/**
 * Standard position field identifiers.
 * It defines the enumerated values that indicates the different types of
 * positioning related information that a positioning module can return. 
 * Each of these fields are categorised into different capabilities.
 * Even though the client can request for any of these fields, 
 * the positioning module may or may not return this information based 
 * on its capability.
 * The data type for each field is also specified here. This data type
 * must be used when retrieving the value for this field using
 * HPositionGenericInfo::GetValue()
 * @publishedAll
 * @released
 */
enum _TPositionFieldId
	{
	/**  
    * Reserved field ID. Used to terminate an array of requested fields. 
     */
	EPositionFieldNone,
	/**
	 * A free field that can be used for a comment.
     * This field can be returned by any type of positioning technology.
	 * The value of this field uses data type TDes16.
     */
	EPositionFieldComment,
    
    /**
     * Fields related to Speed capability are part of this section.
     */
	EPositionFieldSpeedCapabilitiesBegin		= 100,
	/**
	 * Horizontal speed at which the terminal is travelling. 
	 * The value of this field is specified in metres per second and 
	 * it uses data type TReal32.
	 */
	EPositionFieldHorizontalSpeed,
	/**
	 * Accuracy of the horizontal speed defined by the field 
	 * #EPositionFieldHorizontalSpeed.
	 * The value of this field is specified in metres per second and 
	 * it uses data type TReal32.
	 */
	EPositionFieldHorizontalSpeedError,
	/**
	 * Vertical speed at which the terminal is travelling. 
	 * The value of this field is specified in metres per second and 
	 * it uses data type TReal32.
	 */
	EPositionFieldVerticalSpeed,
	/**
	 * Accuracy of the vertical speed defined by the field 
	 * #EPositionFieldVerticalSpeed.
	 * The value of this field is specified in metres per second and 
	 * it uses data type TReal32.
	 */
	EPositionFieldVerticalSpeedError,

    /**
     * Fields related to Direction capability are part of this section.
     */
	EPositionFieldDirectionCapabilitiesBegin	= 200,
	/**
	 * Current direction measured with respect to the true north.
	 * The value of this field is specified in degrees and it
	 * uses data type TReal32.
	 */
	EPositionFieldTrueCourse,
	/**
	 * Accuracy of the current direction defined by the field 
	 * #EPositionFieldTrueCourse. This field is measured with respect
	 * to true north.
	 * The value of this field is specified in degrees and it
	 * uses data type TReal32.
	 */
	EPositionFieldTrueCourseError,
	/**
	 * Current direction measured with respect to the magnetic north.
	 * The value of this field is specified in degrees and it
	 * uses data type TReal32.
	 */
	EPositionFieldMagneticCourse,
	/**
	 * Accuracy of the current direction defined by the field 
	 * #EPositionFieldMagneticCourse. This field is measured with respect 
	 * to magnetic north.
	 * The value of this field is specified in degrees and it
	 * uses data type TReal32.
	 */
	EPositionFieldMagneticCourseError,

    /**
     * Fields related to Compass capability are part of this section.
     */
	EPositionFieldCompassCapabilitiesBegin		= 300,
	/**
	 * Current instantaneous direction of traveling measured with respect 
	 * to the true north.
	 * The value of this field is specified in degrees and it
	 * uses data type TReal32.
	 */
	EPositionFieldHeading, 
	/**
	 * Accuracy of current instantaneous direction of traveling defined
     * by the field #EPositionFieldHeading. This field is measured with
	 * respect to the true north.
	 * The value of this field is specified in degrees and it
	 * uses data type TReal32.
	 */
	EPositionFieldHeadingError,
	/**
	 * Current instantaneous direction of traveling measured with respect 
	 * to the magnetic north.
	 * The value of this field is specified in degrees and it
	 * uses data type TReal32.
	 */
	EPositionFieldMagneticHeading, 
	/**
	 * Accuracy of current instantaneous direction of traveling defined
     * by the field #EPositionFieldMagneticHeading. This field is measured
	 * with respect to the magnetic north.
	 * The value of this field is specified in degrees and it
	 * uses data type TReal32.
	 */
	EPositionFieldMagneticHeadingError,

    /**
     * Fields related to Address capability are part of this section.
     */
	EPositionFieldAddressCapabilitiesBegin		= 400,
	/**
	 * Name of the country.
	 * The value of this field uses data type TDes16.
	 */
	EPositionFieldCountry,
	/**
	 * Country as specified by the two letter ISO 3166-1 code.
	 * The value of this field uses data type TDes16.
	 */
	EPositionFieldCountryCode,
	/**
	 * Name of the state within the country specfied by the field 
	 * #EPositionFieldCountry.
	 * The value of this field uses data type TDes16.
	 */
	EPositionFieldState,
	/**
	 * Name of the city within the state specfied by the field 
	 * #EPositionFieldState.
	 * The value of this field uses data type TDes16.
	 */
	EPositionFieldCity,
	/**
	 * Name of the municipal district within the city specified by the field
     * #EPositionFieldCity.
	 * The value of this field uses data type TDes16.
	 */
	EPositionFieldDistrict,
	/**
	 * Street name and building number.
	 * The value of this field uses data type TDes16.
	 */
	EPositionFieldStreet,
	/**
	 * Additional details about the location within a building. For example,
	 * flat number.
	 * The value of this field uses data type TDes16.
	 */
	EPositionFieldStreetExtension,
	/**
	 * Name of the company, organization or building at the address.
	 * The value of this field uses data type TDes16.
	 */
	EPositionFieldLocationName,
	/**
	 * Post code or Zip code of the address.
	 * The value of this field uses data type TDes16.
	 */
	EPositionFieldPostalCode,
	/**
	 * Name of locality. Locality denotes a small geographical area. 
	 * Locality is usually not a part of the official address.
	 * The value of this field uses data type TDes16.
	 */
	EPositionFieldLocality,
	/**
	 * Information about the cross within the street field, 
	 * #EPositionFieldStreet.
	 * The value of this field uses data type TDes16.
	 */
	EPositionFieldCrossing1,
	/**
	 * Extension for the cross field defined by #EPositionFieldCrossing1.
	 * The value of this field uses data type TDes16.
	 */
	EPositionFieldCrossing2,
	/**
	 * Name of the county, region or province.
	 * The value of this field uses data type TDes16.
	 */
	EPositionFieldCounty,

    /**
     * Fields related to Building capability are part of this section.
     */
	EPositionFieldBuildingCapabilitiesBegin		= 500,
	/**
	 * Name of the building.
	 * The value of this field uses data type TDes16.
	 */
	EPositionFieldBuildingName,
	/**
	 * Floor or level within the building.
	 * The value of this field uses data type TDes16.
	 */
	EPositionFieldBuildingFloor,
	/**
	 * Room name or number within the building.
	 * The value of this field uses data type TDes16.
	 */
	EPositionFieldBuildingRoom,
	/**
	 * Section of a building.
	 * The value of this field uses data type TDes16.
	 */
	EPositionFieldBuildingZone,
	/**
	 * Telephone number associated with the building.
	 * The value of this field uses data type TDes16.
	 */
	EPositionFieldBuildingTelephone,

    /**
     * Fields related to NMEA capability are part of this section.
     */
	EPositionFieldNMEACapabilitiesBegin			= 600,
	/**
	 * Raw NMEA data. If the client requests this field then on completion,
	 * it contains the number of NMEA sentences provided by the positioning
	 * module.
	 * The value of this field uses data type TUint8.
	 */
	EPositionFieldNMEASentences,
	/**
	 * The first NMEA sentence. The rest of the sentences follow this field
	 * The value of this field uses data type TDes8.
	 */
	EPositionFieldNMEASentencesStart,

    /**
     * Fields related to Satellite capability are part of this section.
     */
	EPositionFieldSatelliteCapabilitiesBegin	= 700,
	/**
	 * The number of satellites currently in view. 
	 * The value of this field uses data type TInt8.
	 */
	EPositionFieldSatelliteNumInView,
	/**
	 * The number of satellites being used to provide position information.
	 * The value of this field uses data type TInt8.
	 */
	EPositionFieldSatelliteNumUsed,
	/**
	 * The time as obtained from satellites. 
	 * The value of this field uses data type TTime.
	 */
	EPositionFieldSatelliteTime,
	/**
	 * The Horizontal dilution of precision. Dilution of precision is 
	 * associated with the accuracy of the field.
	 * The value of this field uses data type TReal32.
	 */
	EPositionFieldSatelliteHorizontalDoP,
	/**
	 * The Vertical dilution of precision.
	 * The value of this field uses data type TReal32.
	 */
	EPositionFieldSatelliteVerticalDoP,
	/**
	 * The dilution of precision in time.
	 * The value of this field uses data type TReal32.
	 */
	EPositionFieldSatelliteTimeDoP,
	/**
	 * The position dilution of precision.
	 * The value of this field uses data type TReal32.
	 */
	EPositionFieldSatellitePositionDoP,
	/**
	 * Altitude above the mean sea level.
	 * The value of this field uses data type TReal32.
	 */
	EPositionFieldSatelliteSeaLevelAltitude,
	/**
	 * The difference between the WGS-84 earth ellipsoid and the mean sea 
	 * level. A negative value indicates that the geoid is below the 
	 * WGS84 ellipsoid.
	 * The value of this field uses data type TReal32.
	 */
	EPositionFieldSatelliteGeoidalSeparation,

	/**
     * Fields related to Media capability are part of this section.
	 */
	EPositionFieldMediaCapabilitiesBegin		= 800,
	/**
	 * Media link data field. If the client requests this field then on completion,
	 * it contains the number of media links provided by the positioning module.
	 * The value of this field uses data type TUint8.
	 */
	EPositionFieldMediaLinks,
	/**
	 * The first media link field relevant to this location. The rest of the links
	 * follow this field.
	 * The media link is of the format type/format/URI.
	 * Type and Format are the standard major and minor MIME types of the
	 * media. URI provides the location of the media.
	 * For example, text/html/http://www.s60.com
	 * The value of this field uses data type TDes8.
	 */
	EPositionFieldMediaLinksStart,
	/**
	 * The field id of the last media field. All media fields will be defined between
	 * #EPositionFieldMediaLinksStart and this field.
	 */
	EPositionFieldMedaiLinksEnd					= 899,
    
    /** 
     * Other propritery fields 
     */
	EPositionFieldProprietaryFieldsBegin		= 0x8000,
    /** 
     * the maximum field id value 
     */
	EPositionFieldIdLast = KMaxTUint16
	};

#endif	//__LBSFIELDIDS_H__
