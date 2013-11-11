// Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// Location Based Services Internal Interfaces API
// 
//

/**
 @file
 @internalTechnology
 @released
*/

#ifndef LBSINTERNALINTERFACE_H
#define LBSINTERNALINTERFACE_H

#include <e32def.h>
#include <e32cmn.h>
#include <e32std.h>
#include <e32property.h>
#include <lbs/lbslocdatasourcegpsbase.h>
#include <lbscommon.h>
#include "lbscommoninternaldatatypes.h"
#include <lbs/lbsnetcommon.h>
#include "lbsprocessuiddefs.h"
#include "LbsExtendModuleInfo.h"

#include <lbsadmindatatypes.h>

class TPositionModuleInfoBase;
class TPositionInfoBase;
class TLbsPositionUpdateRequestBase;
class TPositionModuleInfoExtended;
class TPositionExtendedSatelliteInfo;
class TQualityProfile;

/**
Class presents a static interface - there's no need to have a connect as we don't support
//notifications (all info is const).

@internalcomponent
@released
@see TPositionModuleInfoBase
@see TPositionInfoBase
@see TLbsPositionUpdateRequestBase
 */
 
NONSHARABLE_CLASS(LbsModuleInfo)
	{
public:
	//All module infos MUST be TPositionModuleInfo - will leave with KErrArgument otherwise.
	//Should only be called once, by the 'defining' process.
	//Which process does this is yet undecided.
	IMPORT_C static void InitializeL(const TPositionModuleInfoExtended* aModuleInfoPtrArray[], TInt aModuleInfoCount);
	IMPORT_C static void ShutDownL();	
	
	IMPORT_C static TInt GetNumModules(TUint& aNumModules);
	
	//Module info MUST be TPositionModuleInfo - no other type is acceptable.
	IMPORT_C static TInt GetModuleInfoByIndex(TInt aModuleIndex, TAny* aBlob,TInt aBlobSize);
							
	
	//Module infos MUST be TPositionModuleInfo - no other type is acceptable.
	IMPORT_C static TInt GetModuleInfoById(TPositionModuleId aModuleId,TAny* aBlob,TInt aBlobSize);
	
						
    IMPORT_C static TInt GetDataSourceId(TPositionModuleId aModuleId, 
    						TUid& aDataSourceId);
	
	IMPORT_C static TInt GetDeviceCapabilities(TPositionModuleId aModuleId, 
    						TPositionModuleInfoExtended::TDeviceGpsModeCapabilities& aDeviceCapabilities);
						
	};
	
	

/**
This class is for transferring the module status about. It will allow notifications and track
changes to Device Status and Data Quality Status. It will not however allow the tracking of the
'System Module' status events - for this you must use the RLbsSystemModuleEvent class.
To implement the RPositionServer::NotifyModuleStatusEvent the client API will need to make use of
n instances of RLbsModuleStatus (1 per module) and 1 RLbsSystemModuleEvent object.
Also, RPositionServer should use the static GetModuleStatus function to implement
RPositionServer::GetModuleStatus - it should NOT use the same instance of RLbsModuleStatus
that is used for the notifications - doing so will mess up the way occurred events are tracked.

@internalcomponent
@released
@see TPositionModuleInfoBase
@see TPositionInfoBase
@see TLbsPositionUpdateRequestBase
*/
NONSHARABLE_CLASS(RLbsModuleStatus)
	{
public:
	//IMPORT_C static void InitializeL();
	IMPORT_C static void InitializeL(const TPositionModuleStatus& aStatusData);

  	IMPORT_C static void ShutDownL();	
	IMPORT_C static TInt GetModuleStatus(TAny* aBlob,TInt aBlobSize, TPositionModuleId aModuleId);
	IMPORT_C RLbsModuleStatus();
	IMPORT_C void OpenL(TUid aModuleId);
	IMPORT_C void Close();
	IMPORT_C TInt SetModuleStatus(const TAny* aBlob, TInt aBlobSize,
				const TPositionModuleStatusEventBase::TModuleEvent& aOccurredEventsSinceLastSet);
	IMPORT_C void NotifyModuleStatusChange(TRequestStatus& aStatus);
	IMPORT_C void CancelNotifyModuleStatusChange();
	IMPORT_C TInt GetModuleStatus(TAny* aBlob, TInt aBlobSize,
				TPositionModuleStatusEventBase::TModuleEvent& aOccurredEventsSinceLastGet) const;

private:
	RLbsModuleStatus(const RLbsModuleStatus&);
	RLbsModuleStatus& operator=(const RLbsModuleStatus&);
private:
	/** the RProperty object used for publish&subscribe purpose */
	RProperty iProperty;
	mutable TInt iEventDeviceStatusCount;
	mutable TInt iEventDataQualityStatusCount;
	};


/**
Class for ditribution of the system module status. There is one notional system module in the system.
It could be argued that system module events need to be queued, but for the first cut we're not doing that.

@internalcomponent
@released
@see TPositionModuleInfoBase
@see TPositionInfoBase
@see TLbsPositionUpdateRequestBase
*/
NONSHARABLE_CLASS(RLbsSystemModuleEvent)
	{
public:
	IMPORT_C static void InitializeL();
	IMPORT_C static void ShutDownL();	
		
	IMPORT_C RLbsSystemModuleEvent();
	IMPORT_C void OpenL();
	IMPORT_C void Close();
	IMPORT_C TInt SetSystemModuleEvent(const TPositionModuleStatusEventBase::TSystemModuleEvent& aEvent);
	IMPORT_C TInt GetSystemModuleEvent(TPositionModuleStatusEventBase::TSystemModuleEvent& aEvent) const;
	
	IMPORT_C void NotifySystemModuleEvent(TRequestStatus& aStatus);
	IMPORT_C void CancelNotifySystemModuleEvent();
private:
	RLbsSystemModuleEvent(const RLbsSystemModuleEvent&);
	RLbsSystemModuleEvent& operator=(const RLbsSystemModuleEvent&);
private:
	/** the RProperty object used for publish&subscribe purpose */
	RProperty iProperty;
	};




/**
Class used to publish location updates to the Location Server, Network Request Handler
and the Lbs Client API from the managers.

@internalcomponent
@released
@see TPositionInfoBase
@see TLbsPositionUpdateRequestBase
*/

NONSHARABLE_CLASS(RLbsPositionUpdates)
	{
public:
	/** Extra attributes for a position update.
	
	These attributes are used to send extra information about 
	position updates internally between LBS components.
	*/
	enum TPositionInfoAttributes
		{
		EPositionInfoAttributeNone								= 0x0000,
		/* Signals that the AGPS manager has an open SelfLocate 
		   session when this position update was generated. */
		ESelfLocateSessionInProgress							= 0x0001,
		/* Signals that the AGPS manager does not have an open
		   SelfLocate session when this position update was generated. */
		ESelfLocateSessionClosed								= 0x0002
		};
		
	/**
	Information needed by the Location
	Server about the mode the GPS module is in.
	**/
	enum TGpsMode
		{
		EGpsModeNone											=0x0000,
		EGpsModeAutonomous										=0x0001,
		EGpsModeTerminalBased									=0x0002,
		EGpsModeTerminalAssisted								=0x0004
		};

public:
	IMPORT_C static void InitializeL(const TPositionExtendedSatelliteInfo& aSatData);
	IMPORT_C static void ShutDownL();	

	IMPORT_C RLbsPositionUpdates();
	//We support both the A-GPS manager and the Network request handler.
	IMPORT_C void OpenL(TUid aModuleId);
	IMPORT_C void Close();


	IMPORT_C TInt GetPositionInfo(TBool& aConflictControl, TAny* aBlob, TUint aBlobSize, TTime& aTargetTime, TTime& aActualTime) const;
	IMPORT_C TInt GetPositionInfo(TBool& aConflictControl, TAny* aBlob, TUint aBlobSize, TTime& aTargetTime, TTime& aActualTime, TUint& aAttributes, TUint& aGpsMode) const;
	IMPORT_C TInt GetAsMuchAsCanPositionInfo(TBool& aConflictControl, TAny* aBlob, TUint aBlobSize, TTime& aTargetTime, TTime& aActualTime) const;
	// old IMPORT_C TInt SetPositionInfo(const TInt& aError, const TPositionInfoBase& aPositionInfo, const TTime& aTargetTime, const TTime& aActualTime);
	IMPORT_C TInt SetPositionInfo(const TInt& aError, const TBool aConflictControl, const TAny* aBlob,TUint aBlobSize, const TTime& aTargetTime, const TTime& aActualTime);
	IMPORT_C TInt SetPositionInfo(const TInt aError, const TBool aConflictControl, const TAny* aBlob, TUint aBlobSize, const TTime& aTargetTime, const TTime& aActualTime,const TUint& aAttributes,const TUint& aGpsMode);

	IMPORT_C void NotifyPositionUpdate(TRequestStatus& aStatus);
	IMPORT_C void CancelNotifyPositionUpdate();
private:
	RLbsPositionUpdates(const RLbsPositionUpdates&);
	RLbsPositionUpdates& operator=(const RLbsPositionUpdates&);
private:
	/** the RProperty object used for publish&subscribe purpose */
	RProperty iProperty;
	};


/**
Class used to make requests to the Managers from either the Location Server or the
Network Request Handler.

@internalcomponent
@released
@see TPositionInfoBase
@see TLbsPositionUpdateRequestBase
*/

NONSHARABLE_CLASS(RLbsPositionUpdateRequests)
	{
public:
	/**
	Channel identifier for the key of the RProperty.
	We currently support 3 request channels - the module-client pairs below...
	KLbsGpsLocManagerUidValue,KLbsLocServerUidValue
	KLbsNetLocManagerUidValue,KLbsLocServerUidValue
	KLbsGpsLocManagerUidValue,KLbsNetRequestHandlerUidValue
	If we want to support others, see comments in code for how to update.
	
	@internalcomponent
	@released
	*/
	NONSHARABLE_STRUCT(TChannelIdentifer)
		{
		/** position modules Module ID */
		TUid iModuleId;
		/** NOT SURE */
		TUid iClientId;
		};
	
	IMPORT_C static void InitializeL();
	IMPORT_C static void ShutDownL();	
	
	IMPORT_C RLbsPositionUpdateRequests();
	

	IMPORT_C void OpenL(const TChannelIdentifer& aChannel);
	IMPORT_C void Close();
	IMPORT_C TInt SetPositionUpdateRequest(const TLbsPositionUpdateRequestBase& aRequest);
	IMPORT_C TInt GetPositionUpdateRequest(TLbsPositionUpdateRequestBase& aRequest) const;
	IMPORT_C void NotifyPositionUpdateRequestChange(TRequestStatus& aStatus);
	IMPORT_C void CancelNotifyPositionUpdateRequestChange();
private:
	RLbsPositionUpdateRequests(const RLbsPositionUpdateRequests&);
	RLbsPositionUpdateRequests& operator=(const RLbsPositionUpdateRequests&);
private:
	/** the RProperty object used for publish&subscribe purpose */
	RProperty iProperty;
	};

/**
Base class for update request info.
Not abstract - should be instantiated in the managers for reading the data out of
RLbsPositionUpdateRequests::GetPositionUpdateRequest
They then check the Type() of the object and cast to the appropriate derived class.

@internalcomponent
@released
@see TLbsPositionUpdateRequestBase
*/

NONSHARABLE_CLASS(TLbsPositionUpdateRequestBase)
	{
public:
	/** Request type */
	enum TType
		{
		EUnknown 		= 0,
		EStatus  		= 1,
		ECancel  		= 2,
		EUpdateRequest 	= 3
		};
	enum TPowerAdvice
		{
 		ENoPowerAdvice	= 0,
 		EPowerAdviceOn,
 		EPowerAdviceStandby,
 		EPowerAdviceOff,
 		EPowerAdviceClosed
		};

	IMPORT_C TType Type();
	IMPORT_C TLbsPositionUpdateRequestBase();
	IMPORT_C TBool operator== (const TLbsPositionUpdateRequestBase& aVal);
protected:
	TLbsPositionUpdateRequestBase(TType aType);
	/** Request data buffer */
	typedef TUint64 KUpdateRequestBuffer[0x30];		//384 bytes should be enough...
protected:
	/** Request type */
	TType iType;
	/** Statutory information */
	TBool iTracking;
	/** Power Mode Advice */
	TPowerAdvice iPowerAdvice;

	/** Request data buffer */
	KUpdateRequestBuffer iData;
	};

/**
A simple class that contains only statutory information.

@internalcomponent
@released
@see TLbsPositionUpdateRequestBase
*/

NONSHARABLE_CLASS(TLbsPositionUpdateRequestStatus) : public TLbsPositionUpdateRequestBase
	{
public:
	IMPORT_C TLbsPositionUpdateRequestStatus();
	
	IMPORT_C TBool& Tracking();
	IMPORT_C const TBool& Tracking() const;
	IMPORT_C TPowerAdvice PowerAdvice();
	IMPORT_C void SetPowerAdvice(TPowerAdvice aAdvice);

	
protected:
	TLbsPositionUpdateRequestStatus(TType aType);
	};

/**
Instantiated in the loc server or NRH to cancel an update request.
Should be passed into RLbsPositionUpdateRequests::SetPositionUpdateRequest.

@internalcomponent
@released
@see TLbsPositionUpdateRequestBase
*/
NONSHARABLE_CLASS(TLbsPositionUpdateRequestCancel) : public TLbsPositionUpdateRequestStatus
	{
public:
	IMPORT_C TLbsPositionUpdateRequestCancel();
	};

/**
Instantiated in the loc server or NRH to request a location update.
Should be passed into RLbsPositionUpdateRequests::SetPositionUpdateRequest.

@internalcomponent
@released
@see TLbsPositionUpdateRequestBase
*/
NONSHARABLE_CLASS(TLbsPositionUpdateRequest) : public TLbsPositionUpdateRequestStatus
	{
public:
	IMPORT_C TLbsPositionUpdateRequest();
	
	IMPORT_C TTime& TargetTime();
	IMPORT_C const TTime& TargetTime() const;
	
	IMPORT_C TLbsLocRequestQualityInt& RequestQuality();
	IMPORT_C const TLbsLocRequestQualityInt& RequestQuality() const;
	
	IMPORT_C TBool& NewClient();
	IMPORT_C const TBool& NewClient() const;

	IMPORT_C TLbsNetPosRequestMethodInt& RequestMethod();
	IMPORT_C const TLbsNetPosRequestMethodInt& RequestMethod() const;

	IMPORT_C TBool& HybridRequest();
	IMPORT_C const TBool& HybridRequest() const;

private:
	/**
	Target time and quality of the position update request.
	
	@internalcomponent
	@released
	@see TLbsLocRequestQuality
	@see TLbsNetPosRequestMethod
	*/
	class TPositionUpdateRequestBody
		{
	public:
		/** target time of the request */
		TTime iTargetTime;
		/** requested quality */
		TLbsLocRequestQualityInt iRequestQuality;
		/** Indicates  the first request from a new client */
		TBool iNewClient;
		/** Indicates the positioning method(s) that the network has requested. */
		TLbsNetPosRequestMethodInt iRequestMethod;
		/** Indicates this request contains a 'hybrid' location request. */
		TBool iHybridRequest;
		};
	inline TPositionUpdateRequestBody& RequestBody();
	inline const TPositionUpdateRequestBody& RequestBody() const;
	};

/** 
channel array 
@internalcomponent
@released
*/

const RLbsPositionUpdateRequests::TChannelIdentifer KChannelArray[] = 
	{
		{KLbsGpsLocManagerUidValue,KLbsLocServerUidValue},
		{KLbsNetLocManagerUidValue,KLbsLocServerUidValue},
		{KLbsGpsLocManagerUidValue,KLbsNetRequestHandlerUidValue}
	};

const TUint KAGpsManagerToLocServerChannelIndex = 0;
const TUint KNetworkManagerToLocServerChannelIndex = 1;
const TUint KAGpsManagerToNRHChannelIndex = 2;

/** 
the count of channels in the channel array

@internalcomponent
@released
*/

const TInt KChannelArrayCount = sizeof(KChannelArray)/sizeof(RLbsPositionUpdateRequests::TChannelIdentifer);


/** Internal interface for accessing quality profile data.

Class presents a static interface - there's no need to have a connect as we don't support
notifications (all info is const).

@internalcomponent
@released
@see TQualityProfile
 */
 
NONSHARABLE_CLASS(LbsQualityProfile)
	{
public:
	IMPORT_C static void InitializeL(RArray<TQualityProfile>& aQualityProfileArray);
	IMPORT_C static void ShutDownL();	
	
	IMPORT_C static TInt GetNumQualityProfiles(TUint& aNumQualityProfiles);
	
	IMPORT_C static TInt GetQualityProfileByIndex(TInt aIndex, 
												  TQualityProfile& aQualityProfile);
	
	IMPORT_C static TInt GetQualityProfileById(TLbsQualityProfileId aId,
											   TQualityProfile& aQualityProfile);
							
	};

/** Class used to publish GPS measurement updates.

This class is used by the GPS manager to publish GPS measurement data,
which is read by the network request handler when needed.

@internalcomponent
@released
@see TPositionGpsMeasurementInfo
*/
NONSHARABLE_CLASS(RLbsGpsMeasurementUpdates)
	{
public:
	IMPORT_C static void InitializeL();
	IMPORT_C static void ShutDownL();

	IMPORT_C RLbsGpsMeasurementUpdates();

	IMPORT_C void OpenL();
	IMPORT_C void Close();

	IMPORT_C TInt GetGpsMeasurementInfo(TAny* aBlob, TUint aBlobSize, TTime& aActualTime) const;
	IMPORT_C TInt SetGpsMeasurementInfo(const TInt& aError, const TAny* aBlob, TUint aBlobSize, const TTime& aActualTime);

	IMPORT_C void NotifyGpsMeasurementUpdate(TRequestStatus& aStatus);
	IMPORT_C void CancelNotifyGpsMeasurementUpdate();

private:
	RLbsGpsMeasurementUpdates(const RLbsGpsMeasurementUpdates&);
	RLbsGpsMeasurementUpdates& operator = (const RLbsGpsMeasurementUpdates&);
private:
	RProperty iProperty;
	};

#endif //LBSINTERNALINTERFACE_H
