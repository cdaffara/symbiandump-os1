// Copyright (c) 2003-2009 Nokia Corporation and/or its subsidiary(-ies).
// All rights reserved.
// This component and the accompanying materials are made available
// under the terms of "Eclipse Public License v1.0"
// which accompanies this distribution, and is available
// at the URL "http://www.eclipse.org/legal/epl-v10.html".
//
// Initial Contributors:
// Nokia Corporation - initial contribution.
//
// Contributors:
//
// Description:
//


#ifndef __LBSCLASSTYPES_H__
#define __LBSCLASSTYPES_H__

/**
Position quality class

@publishedAll
@released
*/

enum TPositionClassFamily
	{
	/** EPositionInfoFamily
	All classes supported by the positioning module that are derived from
	TPositionInfoBase. For example, TPositionInfo and TPositionCourseInfo.

	When this values is passed to the ClassesSupported() method, it will
	return a bit mask of values corresponding to the enumeration
	TPositionInfoClassType.
 	*/
	EPositionInfoFamily,

	/** EPositionModuleInfoFamily
	All classes supported by the positioning module that are derived from
	TPositionModuleInfoBase. For example, TPositionModuleInfo.

	When this values is passed to the ClassesSupported() method, it will return
	a bit mask of values 	corresponding to the enumeration
	TPositionModuleInfoClassType
	 */
	EPositionModuleInfoFamily,

	/** EPositionModuleStatusFamily
	All classes supported by the positioning module that are derived from
	TPositionModuleStatusBase. For example, TPositionModuleStatus.

	When this values is passed to the ClassesSupported() method, it will return
	a bit mask of values corresponding to the enumeration TPositionModuleStatusClassType
	 */
	EPositionModuleStatusFamily,

	/** EPositionModuleStatusEventFamily
	All classes supported by the positioning module that are derived from
	TPositionModuleStatusEventBase. For example, TPositionModuleStatusEvent.

	When this values is passed to the ClassesSupported() method, it will return
	a bit mask of values corresponding to the enumeration PositionModuleStatusEventClassType
	 */
	EPositionModuleStatusEventFamily,

	/** EPositionModuleQualityFamily
	All classes supported by the positioning module that are derived from
	TPositionQualityBase. For example, TPositionQuality.

	When this values is passed to the ClassesSupported() method, it will return
	a bit mask of values corresponding to the enumeration TPositionQualityClassType.
	 */
	EPositionModuleQualityFamily,

	/** EPositionPositionCriteriaFamily
	All classes supported by the positioning module that are derived from
	TPositionCriteriaBase. For example, TPositionCriteria.

	When this values is passed to the ClassesSupported() method, it will return
	a bit mask of values corresponding to the enumeration TPositionCriteriaClassType
	 */
	EPositionPositionCriteriaFamily,

	/** EPositionUpdateOptionsFamily
	All classes supported by the positioning module that are derived from
	TPositionUpdateOptionsBase. For example, TPositionUpdateOptions.

	When this values is passed to the ClassesSupported() method, it will return
	a bit mask of values from the enumeration TPositionUpdateOptionsClassType.
	 */
	EPositionUpdateOptionsFamily,

	/** EPositionLastFamily */
	EPositionLastFamily = 20
	};

/**
Position quality class types

@publishedAll
@released
 */
enum _TPositionQualityClassType
	{
	/** EPositionQualityUnknownClass */
	EPositionQualityUnknownClass		= 0,
	/** EPositionQualityClass */
	EPositionQualityClass				= 0x01,
	/** EPositionQualityFirstCustomClass */
	EPositionQualityFirstCustomClass	= 0x01000000
	};

/**
Position module information class types

@publishedAll
@released
 */
enum _TPositionModuleInfoClassType
	{
	/** EPositionModuleInfoUnknownClass */
	EPositionModuleInfoUnknownClass		= 0,
	/** EPositionModuleInfoClass */
	EPositionModuleInfoClass			= 0x01,
	/** EPositionModuleInfoFirstCustomClass */
	EPositionModuleInfoFirstCustomClass	= 0x01000000
	};

/**
Position module status class types

@publishedAll
@released
 */
enum _TPositionModuleStatusClassType
	{
	/** EPositionModuleStatusUnknownClass */
	EPositionModuleStatusUnknownClass		= 0,
	/** EPositionModuleStatusClass */
	EPositionModuleStatusClass				= 0x01,
	/** EPositionModuleStatusFirstCustomClass */
	EPositionModuleStatusFirstCustomClass	= 0x01000000
	};

/**
Position module status event class types

@publishedAll
@released
 */
enum _TPositionModuleStatusEventClassType
	{
	/** EPositionModuleStatusEventUnknownClass */
	EPositionModuleStatusEventUnknownClass		= 0,
	/** EPositionModuleStatusEventClass */
	EPositionModuleStatusEventClass				= 0x01,
	/** EPositionModuleStatusEventFirstCustomClass */
	EPositionModuleStatusEventFirstCustomClass	= 0x01000000
	};

/**
Position update options class types

@publishedAll
@released
 */
enum _TPositionUpdateOptionsClassType
	{
	/** EPositionUpdateOptionsUnknownClass */
	EPositionUpdateOptionsUnknownClass		= 0,
	/** EPositionUpdateOptionsClass */
	EPositionUpdateOptionsClass				= 0x01,
	/** EPositionUpdateOptionsFirstCustomClass */
	EPositionUpdateOptionsFirstCustomClass	= 0x01000000
	};

/**
TPositionInfoBase derived classes class types

@publishedAll
@released
 */
enum _TPositionInfoClassType
	{
	/** EPositionInfoUnknownClass */
	EPositionInfoUnknownClass		= 0,
	/** EPositionInfoClass */
	EPositionInfoClass				= 0x01,
	/** EPositionGenericInfoClass */
	EPositionGenericInfoClass		= 0x02,
	/** EPositionCourseInfoClass */
	EPositionCourseInfoClass		= 0x04,
	/** EPositionSatelliteInfoClass */
	EPositionSatelliteInfoClass		= 0x08,
	/** EPositionGpsMeasurementInfoClass */
	EPositionGpsMeasurementInfoClass = 0x10,
	/**EPositionExtendedSatelliteInfoClass */
	EPositionExtendedSatelliteInfoClass =0x20,
	/** EPositionInfoFirstCustomClass */
	EPositionInfoFirstCustomClass	= 0x01000000
	};

/**
TPositionCriteriaBase derived classes class types

@publishedAll
@released
 */
enum _TPositionCriteriaClassType
	{
	/** EPositionCriteriaUnknownClass */	
	EPositionCriteriaUnknownClass		= 0,
	/** EPositionCriteriaClass */
	EPositionCriteriaClass				= 0x01,
	/** EPositionCriteriaFirstCustomClass */
	EPositionCriteriaFirstCustomClass	= 0x01000000
	};

/**
Position Area class types. 
Used for casting form a base class to
a derived class.

@publishedAll
@prototype
 */
enum _TPositionAreaInfoClassType
	{
	/** EPositionAreaInfoUnknownClass
	@see TPositionAreaInfoBase */
	EPositionAreaInfoUnknownClass			= 0,
	
	/** EPositionAreaInfoClass 
	@see TPositionAreaInfo*/
	EPositionAreaInfoClass					= 0x01,
	
	/** EPositionAreaExtendedInfoClass
	@see TPositionAreaExtendedInfo */
	EPositionAreaExtendedInfoClass		= 0x02,
	
	/** EPositionAreaFirstCustomInfoClass */
	EPositionAreaFirstCustomInfoClass		= 0x01000000
	};

/**
Define type for TPositionInfoClassType as TUint32
@publishedAll
@released
 */
typedef TUint32 TPositionInfoClassType;


#endif //__LBSCLASSTYPES_H__
