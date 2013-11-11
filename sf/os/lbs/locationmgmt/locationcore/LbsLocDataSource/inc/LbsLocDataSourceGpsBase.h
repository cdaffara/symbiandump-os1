// Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// LbsLocSourceGpsBase.h
// 
//

#ifndef LBSLOCSOURCEGPSBASE_H
#define LBSLOCSOURCEGPSBASE_H

#include <lbs/lbsadmin.h>
#include <lbs/lbsloccommon.h>
#include <lbspositioninfo.h>
#include <lbs/lbsassistancedatabase.h>

class MLbsLocationSourceGpsObserver;
class RDataReaderRootBase;

/**
Maximum number of GPS mode options
@publishedPartner
@released
 */
const TUint KLbsMaxGpsOptions = 10;

/**
Assistance Data mask used when sending a 'dummy' event from lbs 
to report an error to the integration module
@publishedPartner
@released
 */
const TUint KLbsDummyAssistanceDataMaskWithError = 0xFFFFFFFF;


/** Specifies the desired quality of a location request
The TLbsLocRequestQuality is used by the location source observer to specify
the desired quality of a location request. An instance
of the class is passed to CLbsLocationSourceGpsBase::RequestLocationUpdate().
 
The parameters in TLbsLocRequestQuality include the desired minimum horizontal
and vertical accuracy for the requested location update. It also enables a
maximum time to be specified. This is a time limit indicating the maximum time
the data source should take to calculate the location.
 
It is possible to combine both the desired minimum accuracy and the maximum time.
In this situation the data source module will respond either when the desired
accuracy has been reached or when the time limit has expired.
@see CLbsLocationSourceBase::RequestLocationUpdate()
@see MLbsLocationSourceObserver
@publishedPartner
@released
 */
class TLbsLocRequestQuality : public TLbsLocClassTypeBase
	{
public:
	/** Constructor for the TLbsLocRequestQuality

	@publishedPartner
	@released
	@see CLbsLocationSourceGpsBase
	*/
	IMPORT_C TLbsLocRequestQuality();

	/** Maximum time a data source should spend calculating the current location.
	 
	 @publishedPartner
	 @released
	 @see SetMaxFixTime
	 @return The maximum time that a data source should spend calculating  the current location.
	 */
	IMPORT_C TTimeIntervalMicroSeconds MaxFixTime() const;

	/** Desired minimum horizontal accuracy for a location update.
	@publishedPartner
	@released
	@see SetMaxFixTime
	@return The desired minimum horizontal accuracy for a location update in metres.
	*/
	IMPORT_C TReal32 MinHorizontalAccuracy() const;
	
	/** Desired minimum vertical accuracy for a location update.
	@publishedPartner
	@released
	@see SetMaxFixTime
	@return The desired minimum vertical accuracy for a location update in metres.
	 */
	IMPORT_C TReal32 MinVerticalAccuracy()   const;
	
	/** Is the request, at least in part, coming from the client API? And is the client asking 
		that assistance data should be retrieved?
	 @publishedPartner
	 @prototype
	 @return ETrue if at least part of the request is coming from the client API and the client wants 
		the request to be assisted
	 */
	IMPORT_C TBool AssistedClientRequestPresent() const;

	/** Maximum time a data source should spend calculating the current location.

	When used in conjunction with SetMinHorizontalAccuracy and/or
	SetMinVerticalAccuracy the data source will produce an update either when the
	desired accuracy has be met or when the time limit is reached.

	@publishedPartner
	@released
	@see SetMaxFixTime
	@param aTime specifies the maximum time that the data source should spend
	calculating the current location.
	*/
	IMPORT_C void SetMaxFixTime(TTimeIntervalMicroSeconds aTime);

	/** Desired minimum horizontal accuracy for a location update. 

	When used in conjunction with SetMaxFixTime the data source will produce an
	update either when the desired accuracy is met or when the time limit is reached.
	@publishedPartner
	@released
	@see SetMaxFixTime
	@param aAccuracy specifies the desired minimum horizontal accuracy  (in metres)
	for a location update produced by the data source. 
	 */
	IMPORT_C void SetMinHorizontalAccuracy(TReal32 aAccuracy);
	
	/** Desired minimum vertical accuracy for a location update.
	When used in conjunction with SetMaxFixTime the data source will produce an
	update either when the desired accuracy is met or when the time limit is reached.
	@publishedPartner
	@released
	@see SetMaxFixTime
	@param aAccuracy specifies the desired minimum vertical accuracy (in metres) for
	a location update produced by the data source. 
	 */
	IMPORT_C void SetMinVerticalAccuracy(TReal32 aAccuracy);

	/** Sets the flag to true if there is an outstanding request from the client API that is 
	    part of the current request and if this request (coming from the client) is an assisted 
		request (non-autonomous)
    @publishedPartner
    @prototype
    @param aClientRequestPresent specifies if the request, or part of it, comes from the client API 
     */
	IMPORT_C void SetAssistedClientRequestPresent(TBool aAssistedClientRequestPresent);
protected:
	/**  Minimum horizonal accuracy */
	TReal32 iMinHorizontalAccuracy;
	/**  Minimum vertical accuracy */
	TReal32 iMinVerticalAccuracy;
	/**  Minimum time to wait for fix */
	TTimeIntervalMicroSeconds iMaxFixTime;
	/** Specifies if the client API has any outstanding request that requires assistance data
		that is part of the current request */
	TBool iAssistedClientRequestPresent;
	/** Reserved */
	TUint8  iReserved[12];
	};


/** Encapsulates settings for the GPS integration module. 
TLbsGpsOptions is the default class used to hold various global settings for
the GPS integration module. An instance of the class is passed from the LBS subsystem
to the integration module via the method CLbsLocationSourceGpsBase::SetGpsOptions().
*
@see CLbsLocationSourceGpsBase::SetGpsOptions()
@see CLbsAdmin::TGpsMode
@publishedPartner
@released
 */
class TLbsGpsOptions : public TLbsLocClassTypeBase
	{
public:

	/** 
	@publishedPartner
	@released
	Default constructor 
	*/
	IMPORT_C TLbsGpsOptions();
	
	/** Sets the GPS mode that the positioning module should use, such as prefer terminal based or terminal assisted

	@publishedPartner
	@released
	@param aGpsMode the GPS mode that the module should use
	@see CLbsAdmin::TGpsMode
	*/
	IMPORT_C void SetGpsMode(CLbsAdmin::TGpsMode aGpsMode);
	
	/** Sets the GPS Time Relation Requested flag

	@publishedPartner
	@prototype
	@param aGpsTimeRelationReq the GPS Time Relation Requested flag
	*/
	IMPORT_C void SetGpsTimeRelationReq(TBool aGpsTimeRelationReq);

	/** Gets GPS Mode

	@publishedPartner
	@released
	@return Returns the GPS mode that the module is currently using
	@see CLbsAdmin::TGpsMode
	*/	
	IMPORT_C CLbsAdmin::TGpsMode GpsMode() const;

	/** Gets GPS Time Relation Requested flag

	@publishedPartner
	@prototype
	@return Returns the GPS Time Relation Requested flag
	*/	
	IMPORT_C TBool GpsTimeRelationReq() const;

protected:
	/** GPSMode admin setting */
	CLbsAdmin::TGpsMode iGpsMode;
	/** Flag for the time relation request */
	TBool iGpsTimeRelationReq;
	/** Reserved */
	TUint8 iReserved[12];
	};

/** Encapsulates an item from an array of GPS options.
@publishedPartner
@released
@see TLbsGpsOptionsArray
 */
class TLbsGpsOptionsItem
	{
public:
	enum TPosUpdateType
		{
		EPosUpdateUnknown,
		EPosUpdateCalculation,
		EPosUpdateMeasurement		
		};
 
public:
	IMPORT_C TLbsGpsOptionsItem();
	
	IMPORT_C TPosUpdateType PosUpdateType() const;

	IMPORT_C void SetLocUpdateType(TPosUpdateType aPosUpdateType);

protected:
	/** Type of Location Update Data Required */
	 TPosUpdateType iPosUpdateType;
	/** Reserved */
	TUint8 iReserved[16];
	};


/** Encapsulates an array of settings for the GPS integration module.
An instance of the class may be passed by the LBS subsystem to the
integration module via the method CLbsLocationSourceGpsBase::SetGpsOptions().
An instance of the class is passed from the LBS subsystem
to the integration module via the method CLbsLocationSourceGpsBase::SetGpsOptions()
if the network has indicated that it wants to use hybrid mode - a combination of 
terminal based and terminal assisted positioning.
@see CLbsLocationSourceGpsBase::SetGpsOptions()
@see CLbsAdmin::TGpsMode
@publishedPartner
@released
@see TLbsGpsOptionsItem
 */
class TLbsGpsOptionsArray : public TLbsGpsOptions
	{
public:
	IMPORT_C TLbsGpsOptionsArray();

	IMPORT_C TInt GetOptionItem(TUint aIndex, TLbsGpsOptionsItem& aOptionItem) const;
	IMPORT_C TInt AppendOptionItem(const TLbsGpsOptionsItem& aOptionItem);

	IMPORT_C TInt NumOptionItems() const;
	IMPORT_C void ClearOptionItems();
	
protected:
	/** The number of items in the array */
	TUint iNumItems;
	/** The array of GPS options */
	TFixedArray<TLbsGpsOptionsItem, KLbsMaxGpsOptions> iOptions;

	/** Reserved */
	TUint8 iReserved[16];
	};

/** Passes all commands and information to the GPS Integration module 
The CLbsLocationSourceGpsBase is the abstract interface to a GPS integration
module and is used by the location observer to pass all commands and information
to the module. The GPS integration module provides responses to the location
observer via the MLbsLocationSourceGpsObserver class. This observer class is
also used by the integration module to make requests to the observer.
 
An actual implementation of a GPS integration module will derive from
CLbsLocationSourceGpsBase to provide the specific implementation of the
interface methods.

Although all the interface methods in the class are synchronous they must
complete quickly. When the location observer makes a request via
CLbsLocationSourceGpsBase the integration module must queue the request and
return control to the caller immediately. Afterwards, when the integration
module provides a response, it uses the corresponding notification method in
the MLbsLocationSourceGpsObserver class.
@see MLbsLocationSourceGpsObserver
@see CLbsLocationSourceBase
@publishedPartner
@released
 */
class CLbsLocationSourceGpsBase : public CBase
	{
public:
	/**
	Power mode advice for GPS
	@see AdvisePowerMode()
	 */
	enum TPowerMode 
		{
		/** Power Mode Unknown */
		EPowerModeUnknown,
		/** Advise the module to turn on GPS hardware power */
		EPowerModeOn,
		/** Advise the module to set the GPS hardware power to standby */
		EPowerModeStandby,
		/** Advise the module to turn off GPS hardware power */
		EPowerModeOff,
		/** Advise the module that the LBS subsystem is shutting down */
		EPowerModeClose
		};

public: 
	/** Returns a pointer to an instance of the interface class for a GPS integration module.
	@see MLbsLocationSourceGpsObserver
	@param aObserver is a pointer to the integration module's observer. 
	The observer is used by the integration module to respond to requests made via
	methods in CLbsLocationSourceGpsBase.
	The MLbsLocationSourceGpsObserver observer class is also used by the module to
	provide unsolicited information and obtain and request information from the
	observer.
	@param aEcomModuleId is the UID of the ECOM module to load.
	@return A pointer to an instance of the interface class implemented by the GPS
	integration module. NULL is returned if the specified ECOM module could not be
	loaded.
	 */
  	IMPORT_C static CLbsLocationSourceGpsBase* NewL(MLbsLocationSourceGpsObserver& aObserver, TUid aEcomModuleId);

	/** Destructor for CLbsLocationSourceGpsBase.
	*/
	IMPORT_C ~CLbsLocationSourceGpsBase();
   	
  	/** Used by the location observer to request a position update from the data source. 
	The parameters indicate when the position update is needed and the desired
	accuracy.

	This method must return almost immediately. When the integration module is ready
	to provide the requested location information it responds by calling MLbsLocationSourceGpsObserver::UpdateLocation() on its observer.

	The aTargetTime parameter specifies when the location update is required. The
	integration module should provide a response to the location request at this
	time or shortly afterwards. When the data source provides a response to the
	request, it includes the target time to indicate which request it is dealing with.

	The data source module should only hold and process a single location request at
	any time. If another RequestLocationUpdate is received before the location module
	has responded to an outstanding request, the outstanding request is cancelled. 

	Generally, a second location request will specify a different target time -
	indicating that a location update is needed either before or after the current
	request. However, a subsequent request may also indicate that a different
	accuracy is required. This could have the same target time of the outstanding
	request.

	The current location request may also be cancelled via the
	CancelLocationRequest() method.

	More normally, the integration module will receive its the next location request
	after it has responded to the current request.

	Integration modules are responsible for performing power management on any
	associated hardware. One factor they should take into account is when the next
	update is required. It may be possible for them to put the hardware into power
	saving mode in the intervening period. Integration modules are also advised
	which power mode to select via the method AdvisePowerMode().
	
	Everytime this call is made to the module, the module should make a call to the
	RequestAssistanceData() function.  This will contain the request of any assistanca
	data it requires and has still not received.  Even after all assistance data
	has been received by the module this call will still be made after this location
	request, but the assistance data mask will always be 0.

	@see MLbsLocationSourceGpsObserver::UpdateLocation()
	@see TLbsLocRequestQuality
	@see CancelLocationRequest()
	@see AdvisePowerMode() 
	@param aTargetTime Specifies the time when the location update should be
	provided by the integration module. A value of 0 indicates that the update is
	required immediately.
	@param aQuality Contains the desired quality of the location update. This
	includes both the desired minimum accuracy and the maximum time that should be
	taken to produce the location update.
	@param aQuality Contains the desired accuracy of the location information and
	also the maximum time before the module produces an update. The module should
	provide an update either when the desired accuracy has been reach OR the maximum
	time has expired.
	 */
  	virtual void RequestLocationUpdate(const TTime& aTargetTime, const TLbsLocRequestQuality& aQuality) = 0;
  	
  	/** Cancels an outstanding location request
	Used by the location observer to cancel an outstanding location request. It is
	normally called when it is not known if and when the next location update is
	required. When the observer wishes to replace the current location request with
	another, it makes a single call to RequestLocationUpdate(). In this case
	CancelLocationRequest() is not used.

	The integration module does not need to respond or acknowledge to the
	cancellation. It may even continue to provide the location update at specified
	times.

	This method must return almost immediately. Any internal processing must be
	done asynchronously by the integration module.
	@see RequestUpdateLocation()
	 */	   
  	virtual void CancelLocationRequest() = 0;

	/** Helps the GPS integration module select the appropriate power mode.
	AdvisePowerMode is used by the location observer to help the GPS integration
	module select the appropriate power mode. However, this method only advises the
	integration module of what to do. The module is ultimately responsible for
	selecting the most appropriate power mode for its associated hardware.

	The integration module should report changes to the GPS hardware's operating
	mode by calling the observer method MLbsLocationSourceGpsObserver::UpdateDeviceStatus().

	The AdvisePowerMode method must return almost immediately. Any internal rocessing
	must be done asynchronously by the integration module.

	@see TPowerMode
	@see MLbsLocationSourceGpsObserver::UpdateDeviceStatus()
	@param aMode Indicates the power mode that module may wish to use. Modes include
	On, Off, Standby and Close. A value of Close indicates that integration module
	should shutdown. Note, This value should be ignored where GPS device is used in
	emergency call situations.
	 */
	virtual void AdvisePowerMode(TPowerMode aMode) = 0;
   	
	/** Sets global parameters for the GPS data
	This includes the mode of operation for the GPS device. For example, it is used
	to specify if the GPS device should operate in autonomous or terminal-based modes.

	The standard parameter passed to the method is an instance of the TLbsGpsOptions class.
	This indicates to the GPS module what mode it is required to operate in and that it should
	only produce updates of type TPositionSatelliteInfo.

	However, if LBS subsystem requires the module to provide updates containing more than one
	type of data or indeed a different class type it will pass a parameter of the type
	TLbsGpsOptionsArray. The GPS module can determine the full type of the supplied parameter
	by calling the method ClassType() on aGpsOptions. See the Note below for more information.

	TLbsGpsOptionsArray contains an array of TLbsGpsOptionsItem with the elements specifying
	the information the module is required to provide when it produces a location update.

	When the GPS module calls MLbsLocationSourceGpsObserver::UpdateLocation() it supplies
	an array of position information via the parameter aPosInfoArray. Each element of aPosInfoArray
	array should correspond to the requested class types from the elements of TLbsGpsOptionsArray.

	For example, the data provided by the GPS module in element aPosInfoArray[0] should be
	that requested data type as specified in the first element TLbsGpsOptionsArray.

	The GPS module can determine the required data types by calling the function
	TLbsGpsOptionsItem::PosUpdateType() on each element of the TLbsGpsOptionsArray.

	A value of "EPosUpdateCalculation" specifies that the GPS module should
	provide an item of type TPositionSatelliteInfo in the corresponding array entry of
	the parameter aPosInfoArray[] when it calls MLbsLocationSourceGpsObserver::UpdateLocation().

	A value of "EPosUpdateMeasurement" specifies that the GPS module should
	provide an item of type TPositionGpsMeasurementInfo in the corresponding array entry of
	aPosInfoArray[].

	Note: As mentioned above, the GPS module can determine the full type of the aGpsOptions parameter
	by calling aGpsOptions.ClassType(). To check if the LBS subsystem has passed an instance
	TLbsGpsOptionsArray the returned value should be bitwise compared with ELbsGpsOptionsArrayClass.
	It is best to perform a bitwise "AND" rather than testing if the ClassType() is equal to
	ELbsGpsOptionsArrayClass. This allows future expandability while maintaining binary compatibility.

	@see TLbsGpsOptions
	@param aGpsOptions This specifies global operational parameters for the GPS
	module and its associated hardware.
	 */
	virtual void SetGpsOptions(const TLbsGpsOptions& aGpsOptions) = 0;

	/** Informs the GPS integration module that fresh assistance data is available
	  	or, in the case of 'Prefer Terminal Assisted' gps mode, that there has been an error such that assistance data is unavailable.
	  	 
	This method is used by the LBS subsystem to inform the GPS
	integration module that fresh assistance data is available. This may have been
	delivered unsolicited or due to a request by the integration module. 
	The aDataMask parameter indicates which subsets of assistance data have just
	arrived.
	
	To request fresh assistance data, the GPS module uses the observer method MLbsLocationSourceGpsObserver::RequestAssistanceData().

	To obtain the assistance data, the GPS module uses the observer method MLbsLocationSourceGpsObserver::GetAssistanceDataItem(). 
	
	This method is also used by the LBS subsystem to ask the GPS integration module to reset its 
	assistance data and to inform it that the LBS subsystem assistance data cache has been cleared. 
	The parameter aDataMask specifies which assistance data subset must be reset by the integration module.
	The error code parameter aError is set to KPositionAssistanceDataReset by the LBS subsystem to tell the integration module to reset its assistance data.
	
	When the assistance data has been reset, calling 
	MLbsLocationSourceGpsObserver::GetAssistanceDataItem() for any item in the reset 
	subset will return KErrNotFound.

	Note: Assistance data may be delivered by the observer in batches. For example
	AssistanceDataEvent() may be called multiple times as a result of a single
	download request.
	
	Note: If the module has been put into 'Prefer Terminal Assisted' gps mode, and a subsequent network error causes the self-locate session to be closed,
	then LBS will send a 'dummy' assistance data event to the module with that network error.
	In this case the assistance data mask will be set to KLbsDummyAssistanceDataMaskWithError to indicate a 'dummy' event.
	At this point, the integration module should either switch to 'Autonomous' mode or, if unable to calculate positions autonomously, fail with appropriate error.
	If the module switches to Autonomous mode, it should supply a position update immediately (with NANs, say) indicating that it has done so,  
	otherwise it should supply a measurement update with error and stop. 

	@see TLbsAsistanceDataGroup
	@see TLbsAsistanceDataItem
	@see MLbsLocationSourceGpsObserver::GetAssistanceDataItem()
	@see MLbsLocationSourceGpsObserver::RequestAssistanceData()
	@param aError error code associated with this  assistance data event, or KPositionAssistanceDataReset 
	if the LBS subsystem is asking the GPS integration module to reset its assistance data.
	@param aDataMask Contains a bit mask of values from TLbsAsistanceDataItem
	indicating the subsets of assistance data that have newly arrived from the network.
	 */
	virtual void AssistanceDataEvent(TInt aError, TLbsAsistanceDataGroup aDataMask) = 0;
	
protected:
	/** Constructor for the CLbsLocationSourceGpsBase
	@see MLbsLocationSourceGpsObserver
	@param aObserver is a pointer to the integration module's observer. 
	The observer is used by the integration module to respond to requests and also
	by the module to obtain information.
	*/
	IMPORT_C CLbsLocationSourceGpsBase(MLbsLocationSourceGpsObserver& aObserver);
	
	/** Leaving constructor for CLbsLocationSourceGpsBase. In case required.
	*/
	IMPORT_C void ConstructL();

private:
	/**
	This method is used to allow polymorphic extensions to the API without breaking
	BC. See documentation for explanation.

	Reserved for future expansion - derived classes should see documentation
	on how this is to be used.
 
	@param aFunctionNumber contains the Id of the function to be invoked.
	@param aPtr1 a pointer to any data
	@param aPtr2 a pointer to any data.
	@return TAny* a pointer to any function
 	 */
	IMPORT_C virtual TAny* ExtendedInterface(TInt aFunctionNumber, TAny* aPtr1, TAny* aPtr2);
	
protected:
	/** observer */
	MLbsLocationSourceGpsObserver& iLocSourceGpsObserver;
	
private:
	/** reserved */
	TAny* iReserved;
	TUid  iDtor_ID_Key;
	};

/** 
The MLbsLocationSourceGpsObserver class is used in partnership with
CLbsLocationSourceGpsBase. These interfaces allow a GPS integration module to
pass location data and status information to the observer. They are also used by
the GPS integration module to retrieve GPS assistance data via the observer. 

The location observer makes a request to the GPS integration module via the class
CLbsLocationSourceGpsBase. As these requests will typically take a long time to
complete, the module provides a response via this observer class.

When the GPS module wishes to make a request, it uses MLbsLocationSourceGpsObserver
to contact the observer. In this case, the observer provides its response via
CLbsLocationSourceGpsBase.

Calls from the integration module to the observer may be unsolicited and/or in
response to requests made by the observer. For example, if the observer requests
a location update via CLbsLocationSourceGpsBase::RequestLocationUpdate() the
GPS module will respond some time later via 
MLbsLocationSourceGpsObserver::UpdateLocation(). However, the integration module
is also allowed to provide location updates at any time and not just in response
to the requests from the observer.

When the GPS integration module makes a request via the observer API that takes
a long time to complete, the location observer provides a response via the
corresponding notification method in the CLbsLocationSourceGpsBase class.

For example, when an integration module needs fresh GPS assistance data it will
invoke MLbsLocationSourceGpsObserver::GetAssistanceData(). When this information
eventually arrives from the network, the observer informs the integration
module by calling CLbsLocationSourceGpsBase::NotifyAssistanceDataUpdate().
@see CLbsLocationSourceGpsBase
@publishedPartner
@released
 */
class MLbsLocationSourceGpsObserver
	{
public:
	/** Returns the version of the MLbsLocationSourceGpsObserver interface
	implemented by the observer
	@return The version of this implementation
	 */
	IMPORT_C virtual TVersion Version();
	
	/** Provides a location update
	UpdateLocation is used by the GPS integration module to provide a location
	update. This is normally in response to having received a location request via
	CLbsLocationSourceGpsBase::RequestLocationUpdate().

	Location requests received by the integration module contain a target time 
	(start time) of when the location update is wanted and its desired quality.
	The integration module should provide the response at the start time or shortly
	afterwards. If the quality information specifies a maximum time period this
	indicates the stop time. In this case the module must produce its best effort
	before that time expires.

	To associate a response with a location request, the module must include the
	target time as received in the request.

	The default positioning provided by the GPS module is of type TPositionSatelliteInfo.
	A pointer to an instance of this class is normally passed in the first element
	of aPosInfoArray. However, the LBS subsystem may require the GPS module to
	provide either different or additional positioning information via this array.
	The data types required by the LBS subsystem are specified via the method
	CLbsLocationSourceGpsBase::SetGpsOptions(). The GPS module should provide positioning
	information according to the current GPS options.

	Note that a GPS module must not return a position with a horizontal accuracy or vertical accuracy = 0 
	because the position update will not be returned to the client by the LBS subsystem.

	Although integration modules normally provide a location update as a direct
	response to a request, they are allowed to provide unsolicited location updates
	at any time. This is done by indicating they are "additional" updates to a previous
	request. The module continues to include the target time of the previous request
	in the response until 
	(a) it decides to stop producing unsolicited updates or
	(b) it is time to provide a location update in response to the next request.

	Only when the integration module starts to produce a location update for a
	subsequent request should it call UpdateLocation() with the new target time.

	When the integration module wishes to produce location updates without having
	received a request, the target time in the location update "response" should be zero.

	Note: It is not recommended that integration modules provide unsolicited location
	updates. Careful consideration must be given to both the power drain and the
	overall performance impact.

	In certain circumstances, the GPS module may determine that it will not be able
	to satisfy the required accuracy of the request. In this situation, it should 
	provide an update but set the aStatus parameter to KPositionCalculationFutile.
	All required elements of the the position information array (aPosInfoArray) must
	be supplied as normal. The GPS module should assign any information available
	to the data members.
	@param aStatus is the return/error code for the location update.
	see RPositioner::NotifyPositionUpdate for more information.
	@param aPosInfoArray This is an array of pointers of position information. For
	most GPS positioning modules this will typically contain a single pointer to an
	instance of type TPositionSatelliteInfo. This should contain the current location 
	of the handset and details about the satellites that are in view.
	@param aNumItems Contains a count of the number of items in aPosInfoArray
	@param aTargetTime Contains the time when the location update was requested by
	the observer.
	@see CLbsLocationSourceGpsBase::RequestLocationUpdate()
	@see TLbsLocRequestQuality
	@see RPositioner::NotifyPositionUpdate()
	*/
	virtual void UpdateLocation(TInt aStatus, const TPositionInfoBase* aPosInfoArray[], TInt aNumItems, const TTime& aTargetTime) = 0;
   	
	/** Shows the current state of associated hardware
	UpdateDeviceStatus is used by GPS integration module to provide an indication
	of the current state of its associated hardware. It is the responsibility of
	the integration module to call this method whenever it is required. The module
	does not receive any form of request from the location observer asking it for a
	status update.

	Device status updates reflect the current power mode of the hardware. In the
	case of GPS it will also indicate how long the GPS device takes to produce a
	location update. For example, a status of "Ready" indicates that the hardware is
	powered on and is in a "warm" state. It is capable of producing an accurate
	location update quickly.

	An "Inactive" state indicates the hardware is powered off and will need a "cold"
	start. The GPS module may need to download assistance data resulting in a delay
	in producing an accurate fix. 

	The "StandBy" state could indicate that although the hardware is not powered on
	it is still capable of quickly producing a location update. This may be as a
	result of having valid assistance data cached.

	Note: The GPS integration module may need to report a status change even though
	the hardware itself has not changed state. For example, when the GPS assistance
	data becomes invalid, the module may not be able to produce a location update
	quickly. The GPS integration module should actively monitor for these types of
	situation and update its state accordingly - for example, from "StandBy"
	to "Inactive".
	@see TPositionModuleStatus::TDeviceStatus
	@param aDeviceStatus Contains the current device status of the data source.
	 */
  	virtual void UpdateDeviceStatus(TPositionModuleStatus::TDeviceStatus aDeviceStatus) = 0;
   	
	/** Informs the observer about the accuracy of data it expects to produce
	UpdateDataQualityStatus is used by the GPS integration module to inform the
	observer about the accuracy of data it expects to produce. It is the
	responsibility of the integration module to call this method whenever it is
	required. The module does not receive any form of request from the location
	observer asking it for a status update.

	A module may inform its observer that the quality of its location data will be
	"Normal", "Partial" or "Loss".

	A status of "Normal" indicates that the data source is able to produce updates
	that are to its normal accuracy. When a module is still able to produce location
	information but to a less accurate degree than it would normally expect it
	signals this with a data quality of "Partial." A status of "Loss" is used when
	it is not possible to provide any location estimate.

	Note: Data quality updates reflect the current ability for the data source
	module to produce accurate location information. The module is still required
	to produce updates of its expected data quality whenever it is powered on and
	not just when it is processing a location request. 

	Note: The values of "Normal" and "Partial" correspond to the integration module's
	normal accuracy. They do not reflect its ability to satisfy the accuracy desired
	by any current location request.
	@see PositionModuleStatus::TDataQualityStatus

	@param aDataQuality Indicates the expected data quality of location information
	that could be produced by the GPS integration module.
	 */
  	virtual void UpdateDataQualityStatus(TPositionModuleStatus::TDataQualityStatus aDataQuality) = 0;
  		
	/** Gets cached assistance data
	GetAssistanceDataItem is called by the GPS integration module to obtain cached
	assistance data. The method always returns immediately the requested assistance
	data if it is available. 

	If the desired assistance data is not available from cache or it has become
	invalid, the GPS integration module uses the RequestAssistanceData method to
	request that fresh information is downloaded from the network.
	@see RDataReaderRootBase
	@see RequestAssistanceData
	@param aItem Specifies a single item of assistance.
	@param aDataRoot Returns the requested assistance data if available.
	@param aTimeStamp Contains the time the assistance data was originally
	downloaded from the network.
	@return The method returns KErrNotFound is the specified assistance data is not
	cached. KErrNone is returned if the assistance data is available.
	 */
	virtual TInt GetAssistanceDataItem(TLbsAssistanceDataItem aItem, RDataReaderRootBase& aDataRoot, TTime& aTimeStamp) = 0;
	
	/** Gets cached assistance data time stamp
	GetAssistanceDataItemTimeStamp is called by the GPS integration module to obtain 
	cached assistance data time stamp. 
	The method always returns immediately the requested assistance data time stamp
	if it is available. 

	@see RDataReaderRootBase
	@see GetAssistanceDataItem
	@param aItem Specifies a single item of assistance.
	@param aTimeStamp Contains the time the assistance data was originally
	downloaded from the network.
	@return The method returns KErrNotFound is the specified assistance data is not
	cached. KErrNone is returned if the assistance data is available and the timestamp 
	is valid.
	 */
	virtual TInt GetAssistanceDataItemTimeStamp(TLbsAssistanceDataItem aItem, TTime& aTimeStamp) = 0;
	
	/** Requests fresh assistance data is downloaded from the network
	RequestAssistanceData is called by the GPS integration module to request fresh
	assistance data is downloaded from the network. This method returns quickly
	after having instigated the download.

	When the assistance data arrives at the handset, the GPS integration module is
	informed by the observer via the method CLbsLocationSourceGpsBase::AssistanceDataEvent(). 
	The GPS module accesses the assistance data by calling GetAssistanceDataItem().

	Note: Although the GPS integration module is able to make a single call
	requesting various subsets of assistance data are downloaded, these may arrive
	in batches. That is, a single call to RequestAssistanceData() may result in
	multiple deliveries and CLbsLocationSourceGpsBase::AssistanceDataEvent() being
	called more than once.
	
	The Module should always make this RequestAssistanceData call after a location update
	request has been received (from the Lbs SubSystem via RequestLocationUpdate()).
	This request should be made to request any assistance data it has not yet received (so 
	aDataItemMask should change to only request the outstanding assistance data elements).  
	Even when all of the required assistance data has been received this call should still
	be made, but with the aDataItemMask set to 0.
	
	@see TLbsAsistanceDataGroup
	@see TLbsAsistanceDataItem
	@see CLbsLocationSourceGpsBase::AssistanceDataEvent()
	@see GetAssistanceDataItem()
	@param aDataItemMask Contains a bit mask of values from TLbsAsistanceDataItem
	indicating the subsets of assistance data that are required.
	*/
	virtual void RequestAssistanceData(TLbsAsistanceDataGroup aDataItemMask) = 0;

	/** Shuts down the data source module
	Shutdown is called by the data source module when it wishes to close down. 
	Note: the integration module should NOT start shutting down immediately after
	this call returns. When the observer is ready to shutdown it "deletes" the GPS
	integration module. This results in the module's destructor being called.

	 */	
  	virtual void Shutdown() = 0;

private:
	/**
	This method is used to allow polymorphic extensions to the API without breaking
	BC. See documentation for explanation.

	Reserved for future expansion - derived classes should see documentation
	on how this is to be used.
	 
	@param aFunctionNumber contains the Id of the function to be invoked.
	@param aPtr1 a pointer to any data
	@param aPtr2 a pointer to any data.
	@return TAny* a pointer to any function
	 */
	IMPORT_C virtual TAny* ExtendedInterface(TInt aFunctionNumber, TAny* aPtr1, TAny* aPtr2);
	};

#endif //LBSLOCSOURCEGPSBASE_H
