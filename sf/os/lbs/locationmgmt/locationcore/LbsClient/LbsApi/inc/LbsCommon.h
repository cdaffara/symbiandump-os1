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

#ifndef __LBSCOMMON_H__
#define __LBSCOMMON_H__

#include <e32std.h>
#include <e32math.h>
#include <lbsrequestor.h>
#include <lbsclasstypes.h>
#include <lbserrors.h>

/** 
@publishedAll
@released
// A compile time assert macro. The aExpr parameter must evaluate to an
// integer value at compile time.
// eg. sizeof(TTime) == KSizeOfTInt64
// If aExpr is false, the compiler generates an error.
// If aExpr is true, the compilation continues, and use of this macro will not
// result in any extra object code.
*/
#define POSITION_COMPILE_TIME_ASSERT(aExpr)   typedef char assert_type[aExpr ? 1 : -1]

/** 
@publishedAll
@released

Maximum characters in module name.
*/
const TInt KPositionMaxModuleName = 64;

/** 
@publishedAll
@released

Maximum size for TUInt
*/
const TUint KPositionMaxSizeQualityItem = sizeof(TInt64);

/** 
@publishedAll
@released

Maximum size for fields
*/
const TUint KPositionMaxSectionFields = 10;

/** 
@publishedAll
@released

Defines typedef for TPositionModuleId
*/
typedef TUid  TPositionModuleId;

/** 
@publishedAll
@released

Defines NULL TUid
*/
const TPositionModuleId KPositionNullModuleId = {KNullUidValue};


class TPositionClassTypeBase
/**
The base class for classes used to store position information

@publishedAll
@released
 */
	{
public:
	IMPORT_C TUint32 PositionClassType() const;
	IMPORT_C TUint PositionClassSize() const;

protected:
	TPositionClassTypeBase();

protected:
	/** The type of the derived class */
	TUint32 iPosClassType;
	/** The size of the derived class */
	TUint iPosClassSize;
	};

class TPositionQualityItem
/**
The class for classes used to store position quality

@publishedAll
@released
 */
	{
public:
	/** Quality result */
	enum TResult
		{
		/** Better */
		EIsBetter,
		/** Equal */
		EIsEqual,
		/** Worse */
		EIsWorse
		};

	/** Preference */
	enum TValuePreference
		{
		/** Smaller values */
		EPreferSmallerValues,
		/** Greater values */
		EPreferGreaterValues
		};

	/** Size of data type
	 */
	enum TDataType
		{
		/** Undefined */
		EUndefined,
		/** TInt8 */
		ETInt8,
		/** TInt16 */
		ETInt16,
		/** TInt32 */
		ETInt32,
		/** Tint64 */
		ETInt64,
		/** TUint8 */
		ETUint8,
		/** TUint16 */
		ETUint16,
		/** TUint 32 */
		ETUint32,
		/** TReal32 */
		ETReal32,
		/** TReal64 */
		ETReal64,
		/** ETime */
		ETTime,
		/** Time in microseconds */
		ETTimeIntervalMicroSeconds
		};
	/**
	Constructor for TPositionQualityItem
	 */
	TPositionQualityItem();
	
	/**
	Compare  method
	 */			
	TInt Compare(const TPositionQualityItem& aItem, TResult& aComparison) const;
	/**
	Sets Position Quality
	 */	
	void Set(TDataType aDataType, TValuePreference aValuePreference,
			 const TAny* aData, TUint aSizeOfData);
	/**
	Gets position quality
	 */	
	TInt Get(TDataType aDataType, TAny* aData, TUint aSizeOfData) const;
	
	/**
	Whether or not this element has a defined state
	 */
	TBool IsDefined() const;

private:
	/** Internal datatype */
	TDataType iDataType;
	/** Internel value preference */
	TValuePreference iScaleDirection;
	/** Unused variable for future expansion. */
	TUint8 iData[KPositionMaxSizeQualityItem];
	};

class TPositionQualityBase : public TPositionClassTypeBase
/**
The base class for classes used to store position quality information

@publishedAll
@released
 */
	{
public:
	IMPORT_C TInt Compare(const TPositionQualityBase& aPositionQuality,
						  TInt aElementToCompare,
						  TPositionQualityItem::TResult& aComparison) const;

	/**
 	Returns whether an element is defined
	@param aElementId The element Id of that we're interested in.
	@return TBool if defined
 	*/
	IMPORT_C TBool IsDefined(TInt aElementId) const;
	IMPORT_C TInt HighWaterMark() const;
protected:
	IMPORT_C TPositionQualityBase();

	IMPORT_C void ResetElement(TInt aElementId);


	/**
	Sets the data of an element from the array of elements.
	@param aElementId The element Id of that we're interested in.
	@param aDataType The datatype of the data to be fetched.
	@param aValuePreference Quality preferences
	@param aData A pointer to the data to be written into.
 	*/
	template <class T>
		inline void SetElement(TInt aElementId, TPositionQualityItem::TDataType aDataType,
							   TPositionQualityItem::TValuePreference aValuePreference, T aData)
		{DoSetElement(aElementId, aDataType, aValuePreference, &aData, sizeof(T));}

	/**
	Gets the data of an element from the array of elements.
	@param aElementId The element Id of that we're interested in.
	@param aDataType The datatype of the data to be fetched.
	@param aData A pointer to the data to be written into.
	@return Standard Symbian OS Error code
 	*/	
	template <class T>
		inline TInt GetElement(TInt aElementId, TPositionQualityItem::TDataType aDataType,
							    T& aData) const
		{return DoGetElement(aElementId, aDataType, &aData, sizeof(T));}

private:
	IMPORT_C void DoSetElement(TInt aElementId, TPositionQualityItem::TDataType aDataType,
							   TPositionQualityItem::TValuePreference aValuePreference,
							   const TAny* aData, TUint aSizeOfData);

	IMPORT_C TInt DoGetElement(TInt aElementId, TPositionQualityItem::TDataType aDataType,
								TAny* aData, TUint aSizeOfData) const;
private:
	/** Internal array */
	TFixedArray<TPositionQualityItem, KPositionMaxSectionFields> iPositionQualityData;
	/** Internal highwater mark */
	TInt iHighwaterMark;
	};

class TPositionQuality : public TPositionQualityBase
/**
The standard position quality class.

@publishedAll
@released
 */
	{
public:
	/** Cost Indicator for position request */
	enum TCostIndicator
		{
		/** Unknown cost
		This is the unassigned value and should not be returned */
		ECostUnknown,

		/** Zero cost
		No cost is expected to be incurred when obtaining a position fix */
		ECostZero,

		/** Possible cost
		The positioning module is uncertain if the user will incur a charge. */
		ECostPossible,

		/** Charged
		The positioning module expects a charge to be levied when obtaining
 		position information */
		ECostCharge
		};

	/** Power consumption */
	enum TPowerConsumption
		{
		/** Unknown power consumption
		The positioning module is unable to determine the likely power drain */
		EPowerUnknown,

		/** Zero power consumption
		No internal power will be used when obtaining a position fix */
		EPowerZero,

		/** Low power consumption
		The positioning module expects a minimum power drain when using the
		associated technology. This may be comparable to the power usage when
		the phone is in standby mode. */
		EPowerLow,

		/** Medium power consumption
		The positioning module expects a moderate power drain when using the
		associated technology. This may be comparable to the power usage when
		the phone is being actively used */
		EPowerMedium,

		/** High power consumption
		The positioning module expects a high power drain when using the
		associated technology. Use of this module will quickly consume the phone's batteries */
		EPowerHigh
		};

public:
	IMPORT_C TPositionQuality();

	IMPORT_C TTimeIntervalMicroSeconds TimeToFirstFix() const;
	IMPORT_C TTimeIntervalMicroSeconds TimeToNextFix()  const;  

	IMPORT_C TReal32 HorizontalAccuracy() const;
	IMPORT_C TReal32 VerticalAccuracy()   const;

	IMPORT_C TCostIndicator CostIndicator() const;
	IMPORT_C TPowerConsumption PowerConsumption() const;

	IMPORT_C void SetTimeToFirstFix(TTimeIntervalMicroSeconds aTimeToFirstFix);
	IMPORT_C void SetTimeToNextFix(TTimeIntervalMicroSeconds aTimeToNextFix);

	IMPORT_C void SetHorizontalAccuracy(TReal32 aHorizontalAccuracy);
	IMPORT_C void SetVerticalAccuracy(TReal32 aVerticalAccuracy);

	IMPORT_C void SetCostIndicator(TCostIndicator aCost);
	IMPORT_C void SetPowerConsumption(TPowerConsumption aPower);

private:
	/** Unused variable for future expansion. */
	TUint8 iReserved[16];
	};


class TPositionModuleInfoBase : public TPositionClassTypeBase
/**
The base class for classes storing information on position modules

@publishedAll
@released
 */
	{
protected:
	IMPORT_C TPositionModuleInfoBase();
	};

class TPositionModuleInfo : public TPositionModuleInfoBase
/**
The standard class for storing information on position modules

@publishedAll
@released
 */
	{
public:
	/**
	 Technology types
	 */
	typedef TUint32 TTechnologyType;

	/**
	 Technology Type
	 */ 
	enum _TTechnologyType
		{
		/** Indicates that the positioning module is unaware of the technology
		used to obtain position information */
		ETechnologyUnknown	= 0,
		/** The primary positioning technology is handset based.
		For example standard GPS */
		ETechnologyTerminal	= 0x01,
		/** The primary positioning technology is network based.
		For example, E-OTD */
		ETechnologyNetwork	= 0x02,
		/** The primary positioning mechanism receives assistance in some form.
		Generally to obtain a quicker or more accurate fix */
		ETechnologyAssisted	= 0x04
		};

	/**
	 Device locations
	 */
	typedef TUint32 TDeviceLocation;

	/**
	 Device location
	 */ 
	enum _TDeviceLocation
		{
		/** The positioning module is unaware of the hardware used to supply
		positioning information */
		EDeviceUnknown	= 0,
		/** The positioning hardware is integral to the terminal */
		EDeviceInternal	= 0x01,
		/** The positioning hardware is separate from the terminal */
		EDeviceExternal	= 0x02
		};

	/**
	 Position module capabilities
	 */
	typedef TUint32 TCapabilities;

	/**
	 Module Capabilities
	 */ 
	enum _TCapabilities
		{
		/** No Capabilities */
		ECapabilityNone	       = 0,

		/** Positioning modules with this capability will support the
		TPositionInfo class and are able to provide latitude and longitude
		related information
		 */
		ECapabilityHorizontal  = 0x0001,

		/** Positioning modules with this capability will support the
	 	TPositionInfo class and are able to provide height related information
		 */
		ECapabilityVertical    = 0x0002,

		/** Positioning modules with this capability will support the
		TPositionCourseInfo class and are able to provide information related
		to the current horizontal speed
		 */
		ECapabilitySpeed       = 0x0004,

		/** Positioning modules with this capability will support the
		TPositionCourseInfo class and are able to provide heading related information
		 */
		ECapabilityDirection   = 0x0008,

		/** Positioning modules with this capability will support the
		TPositionSatelliteInfo class. Such a module will be able to return at
		least some satellite data - but the extent of the information could vary.
		Applications must ensure that any value returned is valid before it is used 
		*/
		ECapabilitySatellite   = 0x0010,

		/** This value is reserved for future expansion.
		Positioning modules with this capability will be able to return
		information related to a magnetic compass. The API does not currently
		define an extended class that encapsulates this type of information
 		*/
		ECapabilityCompass     = 0x0020,

		/** This value is reserved for future expansion.
		Positioning modules with this capability will be able to return
		location information using NMEA formatted text strings.
		The API does not currently define an extended class that encapsulates
		this type of information
 		*/
		ECapabilityNmea        = 0x0040,

		/** This value is reserved for future expansion.
		Positioning modules with this capability will be able to return
		information related to the postal address of the current location
		 */
		ECapabilityAddress     = 0x0080,

		/** This value is reserved for future expansion.
		Positioning modules with this capability will be able to return the
		current position in terms of where within a building it is. For
		example, this may include the floor and the room name.	
		The API does not currently define an extended class that encapsulates
		this type of information
		 */
		ECapabilityBuilding    = 0x0100,

		/** This value is reserved for future expansion.
		Positioning modules with this capability will be able to return provide
		a link to further information about the location. The standard mechanism
		is via a URL.
		The API does not currently define an extended class that encapsulates
		this type of information
 		*/
		ECapabilityMedia       = 0x0200
		};

public:
	IMPORT_C TPositionModuleInfo();

	IMPORT_C TPositionModuleId ModuleId() const;
	IMPORT_C TBool IsAvailable() const;
	IMPORT_C void GetModuleName(TDes& aModuleName) const;
	IMPORT_C void GetPositionQuality(TPositionQuality& aPosQuality) const;
	IMPORT_C TTechnologyType TechnologyType() const;
	IMPORT_C TDeviceLocation DeviceLocation() const;
	IMPORT_C TCapabilities Capabilities() const ;
	IMPORT_C TUint32 ClassesSupported(TPositionClassFamily aClassType) const;
	IMPORT_C TVersion Version() const;

	IMPORT_C void SetModuleId(TPositionModuleId aModuleId);
	IMPORT_C void SetIsAvailable(TBool aIsAvailable);
	IMPORT_C void SetModuleName(const TDesC& aModuleName);
	IMPORT_C void SetPositionQuality(const TPositionQuality& aPosQuality);
	IMPORT_C void SetTechnologyType(TTechnologyType aTechnologyType);
	IMPORT_C void SetDeviceLocation(TDeviceLocation aDeviceLocation);
	IMPORT_C void SetCapabilities(TCapabilities aDeviceCapabilities);
	IMPORT_C void SetClassesSupported(TPositionClassFamily aClassType,
									  TUint32 aSupportedClasses);
	IMPORT_C void SetVersion(TVersion aVersion);

protected:
	/** The module ID */
	TPositionModuleId	iModuleId;
	/** Whether or not the module is available */
	TBool iIsAvailable;
	/** The module name */
	TBuf<KPositionMaxModuleName> iModuleName;
	/** The position quality */
	TPositionQuality    iPosQuality;
	/** The technology type */
	TTechnologyType     iTechnologyType;
	/** The location of the device associated with this module */
	TDeviceLocation     iDeviceLocation;
	/** The capabilities of this module */
	TCapabilities		iCapabilities;
	/** The array of supported classes for the different class types */
	TFixedArray<TUint32, EPositionLastFamily> iSupportedClassTypes;
	/** The version of this module */
	TVersion            iVersion;

private:
	/** Unused variable for future expansion. */
	TUint8 iReserved[16];
	};


class TPositionModuleStatusBase : public TPositionClassTypeBase
/**
The base class for classes storing a position module's status

@publishedAll
@released
 */
	{
protected:
	IMPORT_C TPositionModuleStatusBase();
	};

class TPositionModuleStatus : public TPositionModuleStatusBase
/**
The normal class for storing a position module's status

@publishedAll
@released
 */
	{
public:
	/** defined type for TDeviceStatus */
	typedef TInt TDeviceStatus;

	/**
	 * The device status
	 */
	enum _TDeviceStatus 
		{
		/** Device unknown
		This is not a valid state and should never be reported */
		EDeviceUnknown,

		/** Device error
		There are problems using the device. For example, there may be
		hardware errors. It should not be confused with complete loss of data
		quality which indicates that the device is functioning correctly but
		is currently unable to obtain position information.
		The error state will be reported if the device can not be successfully
		brought on line. For example, the positioning module may have been unable
		to communicate with the device or it is not responding as expected.
 		*/
		EDeviceError,

		/** Device disabled
		Although the device may be working properly, it has been taken off line
		and is regarded as being unavailable to obtain position information.
		This will generally have been done by the user via the control panel.
		In this state, positioning framework will not use the device.
		 */
		EDeviceDisabled,

		/** Device inactive
		The device is not being used by the positioning framework. This is
		typically because there are no clients currently obtaining the position
		from it. This is the normal status that is returned for a module that
		is not currently loaded by the system
	 	*/
		EDeviceInactive,

		/** Device initialising
		This is a transient state. The device is being brought out of the
		"inactive" state but has not 	reached either the "ready" or "stand by"
		modes. The initialising state will occur when the positioning module
		is first selected to provide a client application with location information	
		 */
		EDeviceInitialising,

		/** Device standby
		This state indicates the device has entered "sleep" or "power save"
		mode. This signifies that the device is online, but is not actively
		retrieving position information. A device will generally enter this mode
		when the next position update is not required for some time and it is
		more 	efficient to partially power down.
		Note: Not all positioning modules will support this state - particularly
		when there is external hardware
		 */
		EDeviceStandBy,

		/** Device ready
		The positioning device is online and is ready to retrieve
		position information
		 */
		EDeviceReady,

		/** Device active
		The positioning device is actively in the process of retrieving
		position information.
		Note: Not all positioning modules will support this state - particularly
		when there is external hardware
		 */
		EDeviceActive
		};

	/** defined type for TDataQualityStatus */
	typedef TInt TDataQualityStatus;

	/**
	 * The data quality status
	 */
	enum _TDataQualityStatus
		{
		/** Data Quality Unknown
		This is the unassigned valued. This state should only be reported
		during an event indicating that a positioning module has been removed 
		or is not yet loaded.
 		*/
		EDataQualityUnknown,

		/** Data Quality Loss
		The accuracy and contents of the position information has been
		completely compromised. It is no longer possible to return any
		coherent data.
		This situation will occur if the device has lost track of all the
		transmitters (for example, satellites or base stations).
		It should be noted although it is currently not possible to obtain
		position information, the device may still be functioning correctly.
		This state should not be confused with a device error.
 		*/
		EDataQualityLoss,

		/** Data Quality Partial
		There has been a partial degradation in the available position
		information. In particular, it is not possible to provide the required
		(or expected) quality of information.
		This situation could occur if the device has lost track of one of the
		transmitters (for example, satellites or base stations)
		 */
		EDataQualityPartial,

		/** Data Quality Normal
		The positioning device is functioning as expected
 		*/
		EDataQualityNormal
		};

	IMPORT_C TPositionModuleStatus();

	IMPORT_C TDeviceStatus DeviceStatus() const;
	IMPORT_C TDataQualityStatus DataQualityStatus() const;


	IMPORT_C void SetDeviceStatus(TDeviceStatus aStatus);
	IMPORT_C void SetDataQualityStatus(TDataQualityStatus aStatus);
	
protected:
	/** The device status */
	TDeviceStatus      iDeviceStatus;
	/** The data quality status */
	TDataQualityStatus iDataQualityStatus;

private:
	/** Unused variable for future expansion. */
	TUint8 iReserved[8];
	};


class TPositionModuleStatusEventBase : public TPositionClassTypeBase
/**
The base class for classes storing position module status events

@publishedAll
@released
 */
	{
public:
	/** defined type for TSystemModuleEvent */
	typedef TUint32 TSystemModuleEvent;

	/**
	 * Module events
	 */
	enum _TSystemModuleEvent
		{
		/** System Unknown
		This is not a valid state and should never be reported */
		ESystemUnknown				= 0,

		/** System Error
		There are problems using the module. For example, the module may have
		terminated abnormally. It should not be confused with the module
		reporting the error EDeviceError via TPositionModuleStatus::DeviceStatus().
		That signifies the module itself is up and running - but it may be
		unable to successful communicate with the hardware
 		*/
		ESystemError,
	
		/** System Module Installed
		A new positioning module has been dynamically added to the system. To
		receive this event, the client application must have expressed interest
		in status notifications from any positioning module. The Id of the newly
		installed module can be found by calling TPositionModuleStatusEvent::ModuleId().
		 */
		ESystemModuleInstalled,

		/** System Module Removed
		A positioning module is uninstalled. The Id of the removed module can
		be found by calling TPositionModuleStatusEvent::ModuleId()
		 */
		ESystemModuleRemoved
		};

	/** defined type for TModuleEvent */
	typedef TUint32 TModuleEvent;

	/**
	 Module events
	 */
	enum _TModuleEvent
		{
		/** No module event
		This is the unassigned value and should not be reported or used
 		*/
		EEventNone				= 0,

		/** Module event device status
		Events about the general status of the device. When this type of event
		occurs, client applications should inspect the value returned by the
 		TPositionModuleInfo::DeviceStatus() method for more information
 		*/
		EEventDeviceStatus      = 0x01,
		
		/** Module Data Quality status
		Events about the quality of the data a module is able to return. When
		this type of event occurs, client applications should inspect the
		value returned by the TPositionModuleInfo::DataQualityStatus() method
		for more information.
 		*/
		EEventDataQualityStatus = 0x02,

		/** Module System Event
		System level events about the status of modules. Events of this type
		indicate when modules have been added or removed from the system. When
		this event type occurs, client applications should inspect the value
		returned by TPositionModuleInfo::SystemModuleEvent() to determine which
		particular event was responsible
 		*/
		EEventSystemModuleEvent = 0x04,

		/** All events */
		EEventAll				= EEventDeviceStatus |
								  EEventDataQualityStatus |
								  EEventSystemModuleEvent
		};

protected:
	IMPORT_C TPositionModuleStatusEventBase();
	IMPORT_C TPositionModuleStatusEventBase(TModuleEvent aRequestedEventMask);

public:
	IMPORT_C void SetRequestedEvents(TModuleEvent aRequestedEventMask);
	IMPORT_C TModuleEvent RequestedEvents() const;

	IMPORT_C void SetOccurredEvents(TModuleEvent aOccurredEventMask);
	IMPORT_C TModuleEvent OccurredEvents() const;

	IMPORT_C TPositionModuleId ModuleId() const;
	IMPORT_C void SetModuleId(TPositionModuleId aModuleId);

	IMPORT_C void SetSystemModuleEvent(TSystemModuleEvent aSystemModuleEvent);
	IMPORT_C TSystemModuleEvent SystemModuleEvent() const;

protected:
	IMPORT_C virtual void DoReset(TInt aSizeOfClass);

private:
	friend class RPositionServer;
	void Reset();

protected:
	/** A bitmask of the requested events */
	TModuleEvent iRequestedEvents;
	/** A bitmask of the events which occurred */
	TModuleEvent iOccurredEvents;
	/** System level module status events*/
	TSystemModuleEvent iSystemModuleEvent;
	/** The module ID causing the event*/
	TPositionModuleId  iModuleId;
	};

class TPositionModuleStatusEvent : public TPositionModuleStatusEventBase
/**
The normal class for storing position module status events

@publishedAll
@released
 */
	{
public:
	IMPORT_C TPositionModuleStatusEvent();
	IMPORT_C TPositionModuleStatusEvent(TModuleEvent aRequestedEventMask);

	IMPORT_C void SetModuleStatus(const TPositionModuleStatus& aModuleStatus);
	IMPORT_C void GetModuleStatus(TPositionModuleStatus& aModuleStatus) const;

protected:
	IMPORT_C virtual void DoReset(TInt aSizeOfClass);

protected:
	/** The module status */
	TPositionModuleStatus iModuleStatus;
	};


class TPositionUpdateOptionsBase : public TPositionClassTypeBase
/**
The base class for classes storing position update options

@publishedAll
@released
 */
	{
protected:
	IMPORT_C TPositionUpdateOptionsBase();
	IMPORT_C TPositionUpdateOptionsBase(TTimeIntervalMicroSeconds aInterval,
	                                    TTimeIntervalMicroSeconds aTimeOut = TTimeIntervalMicroSeconds(0),
	                                    TTimeIntervalMicroSeconds aMaxAge = TTimeIntervalMicroSeconds(0));

public:
	IMPORT_C void SetUpdateInterval(TTimeIntervalMicroSeconds aInterval);
	IMPORT_C void SetUpdateTimeOut(TTimeIntervalMicroSeconds aTimeOut);
	IMPORT_C void SetMaxUpdateAge(TTimeIntervalMicroSeconds aMaxAge);

	IMPORT_C TTimeIntervalMicroSeconds UpdateInterval() const;
	IMPORT_C TTimeIntervalMicroSeconds UpdateTimeOut() const;
	IMPORT_C TTimeIntervalMicroSeconds MaxUpdateAge() const;

protected:
	/** The update interval */
	TTimeIntervalMicroSeconds iUpdateInterval;
	/** The update timeout */
	TTimeIntervalMicroSeconds iUpdateTimeOut;
	/** The maximum update age */
	TTimeIntervalMicroSeconds iMaxUpdateAge;

private:
	/** Unused variable for future expansion. */
	TUint8 iReserved[8];
	};

class TPositionUpdateOptions : public TPositionUpdateOptionsBase
/**
The normal class for storing position update options

@publishedAll
@released
 */
	{
public:
	IMPORT_C TPositionUpdateOptions();
	IMPORT_C TPositionUpdateOptions(TTimeIntervalMicroSeconds aInterval,
	                                TTimeIntervalMicroSeconds aTimeOut = TTimeIntervalMicroSeconds(0),
	                                TTimeIntervalMicroSeconds aMaxAge = TTimeIntervalMicroSeconds(0),
	                                TBool aAcceptPartialUpdates = EFalse);

	IMPORT_C void SetAcceptPartialUpdates(TBool aAcceptPartialUpdates);
	IMPORT_C TBool AcceptPartialUpdates() const;

protected:
	/** Whether partial updates are accepted */
	TBool iAcceptPartialUpdates;

private:
	/** Unused variable for future expansion. */
	TUint8 iReserved[16];
	};


#endif //__LBSCOMMON_H__
