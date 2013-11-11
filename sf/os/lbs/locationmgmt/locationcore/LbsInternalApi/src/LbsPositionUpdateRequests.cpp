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
//

#include <centralrepository.h>
#include "LbsInternalInterface.h"
#include "lbspropertykeydefs.h"
#include "lbsrootcenrepdefs.h"
#include "LbsInternalCommon.h"

/** Descriptor package of the position update request data */
typedef TPckg<TLbsPositionUpdateRequestBase> TUpdateRequestPckg;
/** Descriptor package of the position update request data */
typedef TPckgC<TLbsPositionUpdateRequestBase> TUpdateRequestPckgC;

/** The start key offset of position update request data */
const TUint KLbsPositionUpdateRequestsDataStartKeyOffset = KLbsPositionUpdateRequestsKeyBase;

/** Get the position update request key offset from position module ID and client ID

@param aChannel A reference to the TChannelIdentifer object.
@return key offset of the position update request
@released
@internalComponent
 */
 TUint PositionUpdateRequestKeyFromModuleIdAndClientIdL(const RLbsPositionUpdateRequests::TChannelIdentifer& aChannel)
	{
	//We support 3 channels at the moment.
	//Loc Server to the Network Loc Manager
	//Loc Server to the A-GPS Manager
	//NRH to the A-GPS Manager
	//If we need to support more, we just extend this code and the array in the InitializeL method
	
	//All that's needed is that each pair generates a unique key.
	//There's almost certainly better ways of doing this...
		
	if(aChannel.iClientId.iUid==KLbsLocServerUidValue)
		{
		if(aChannel.iModuleId.iUid==KLbsGpsLocManagerUidValue)
			{
			return KLbsPositionUpdateRequestsDataStartKeyOffset+0;
			}
		if(aChannel.iModuleId.iUid==KLbsNetLocManagerUidValue)
			{
			return KLbsPositionUpdateRequestsDataStartKeyOffset+1;
			}
		
		User::Leave(KErrNotSupported);
		}

	if(aChannel.iClientId.iUid==KLbsNetRequestHandlerUidValue)
		{
		if(aChannel.iModuleId.iUid==KLbsGpsLocManagerUidValue)
			{
			return KLbsPositionUpdateRequestsDataStartKeyOffset+2;	
			}
		}

	User::Leave(KErrNotSupported);	//Not a recognised channel.
	return 0;			//Have to return something.
	}

/** Initialize the position update request data. Check the security of the process, set the available keys according to the module array to 
RProperty object. The secure ID of the property is set to the initialization process secure ID.

@see TLbsPositionUpdateData
@released
@internalComponent
 */
 EXPORT_C void RLbsPositionUpdateRequests::InitializeL()
	{
	const TSecurityPolicy KReadPolicy(ECapability_None);
	const TSecurityPolicy KWritePolicy(ECapabilityWriteDeviceData);

	//First, set the data...
	for(TInt i=KChannelArrayCount-1; i>=0; i--)
		{
		const TUint key = PositionUpdateRequestKeyFromModuleIdAndClientIdL(KChannelArray[i]);
		TInt err = RProperty::Define(key, RProperty::EByteArray, KReadPolicy, KWritePolicy, sizeof(TLbsPositionUpdateRequestBase));
		if (err != KErrNone && err != KErrAlreadyExists)
			{
			User::Leave(err);
			}
		
		const TLbsPositionUpdateRequestBase dummyData;	//Will default the data the Unknown type.
		TUpdateRequestPckgC pckg(dummyData);
		User::LeaveIfError(RProperty::Set(RProcess().SecureId(), key, pckg));
		}

	// Save the Uid of the process which defined the properties
	// in the cenrep file owned by LbsRoot.
	LbsInternalCommon::SetCategoryUidL(KPositionUpdateRequestsCategoryKey, RProcess().SecureId());
	}
	
/**	Delete all keys defined in RProperty object.

@released
@internalComponent
 */
EXPORT_C void RLbsPositionUpdateRequests::ShutDownL()
	{
	for(TInt i=KChannelArrayCount-1; i>=0; i--)
		{
		const TUint key = PositionUpdateRequestKeyFromModuleIdAndClientIdL(KChannelArray[i]);
		User::LeaveIfError(RProperty::Delete(key));
		}
	}

/**	Nothing here.

@released
@internalComponent
 */
 EXPORT_C RLbsPositionUpdateRequests::RLbsPositionUpdateRequests()
	{
	}
	
/**	Open the RProperty object by module ID, client ID and initialization process UID

@param aChannel The channel identifier, include Module ID and Client ID.
@param aInitializationProcessUid The initialization process UID.
@released
@internalComponent
 */
 EXPORT_C void RLbsPositionUpdateRequests::OpenL(const TChannelIdentifer& aChannel)
	{
	__ASSERT_DEBUG(iProperty.Handle()==NULL, User::Invariant());		//Handle already open.
	
	TUint key = PositionUpdateRequestKeyFromModuleIdAndClientIdL(aChannel);
	
	// Get the CategoryUid from the cenrep file owned by LbsRoot.
	TUid categoryUid;
	LbsInternalCommon::GetCategoryUidL(KPositionUpdateRequestsCategoryKey, categoryUid);
		
	User::LeaveIfError(iProperty.Attach(categoryUid, key));
	}
	
/** Close the RProperty object of the module 

@released
@internalComponent
 */
EXPORT_C void RLbsPositionUpdateRequests::Close()
	{
	iProperty.Close();
	}
	
/** Set position update request to RProperty, should be call by modules

@param aRequest A reference to the TLbsPositionUpdateRequestBase object, the published position update request.
@see TLbsPositionUpdateRequestBase
@return Symbian Location service Error code
@released
@internalComponent
 */
 EXPORT_C TInt RLbsPositionUpdateRequests::SetPositionUpdateRequest(const TLbsPositionUpdateRequestBase& aRequest)
	{
	__ASSERT_DEBUG(iProperty.Handle()!=NULL, User::Invariant());		//Handle not open.
	
	return iProperty.Set(TUpdateRequestPckgC(aRequest));
	}
	
/** Get published position update request from RProperty 

@param aRequest A reference to the TLbsPositionUpdateRequestBase object, the published position update request.
@see TLbsPositionUpdateRequestBase
@return Symbian Location service Error code
@released
@internalComponent
 */
 EXPORT_C TInt RLbsPositionUpdateRequests::GetPositionUpdateRequest(TLbsPositionUpdateRequestBase& aRequest) const
	{
	__ASSERT_DEBUG(iProperty.Handle()!=NULL, User::Invariant());		//Handle not open.
	
	TUpdateRequestPckg pckg(aRequest);
	return const_cast<RLbsPositionUpdateRequests*>(this)->iProperty.Get(pckg);
	}
	
/** Set request for position update request notification by subscribe to the RProperty object

@param aStatus A reference to the TRequestStatus object.
@see TRequestStatus
@released
@internalComponent
 */
 EXPORT_C void RLbsPositionUpdateRequests::NotifyPositionUpdateRequestChange(TRequestStatus& aStatus)
	{
	__ASSERT_DEBUG(iProperty.Handle()!=NULL, User::Invariant());		//Handle not open.
	
	iProperty.Subscribe(aStatus);
	}

/** Cancel the position update change notification request. Stop the notification publish from the RProperty object.

@released
@internalComponent
 */
 EXPORT_C void RLbsPositionUpdateRequests::CancelNotifyPositionUpdateRequestChange()
	{
	__ASSERT_DEBUG(iProperty.Handle()!=NULL, User::Invariant());		//Handle not open.
	
	iProperty.Cancel();
	}


//
//Rather than clog the header, comments are here.
//We had to use an array of TUint64s rather than the usual array of TUint8s for alignment
//reasons - using an array of TUint64s forces the compiler to align the buffer to an 8-byte boundary
EXPORT_C TLbsPositionUpdateRequestBase::TType TLbsPositionUpdateRequestBase::Type()
	{
	return iType;
	}
	
EXPORT_C TLbsPositionUpdateRequestBase::TLbsPositionUpdateRequestBase()
:	iType(EUnknown),
	iTracking(EFalse),
	iPowerAdvice(ENoPowerAdvice)
	{
	memclr(&iData, sizeof(iData));
	}

TLbsPositionUpdateRequestBase::TLbsPositionUpdateRequestBase(TType aType)
:	iType(aType),
	iTracking(EFalse),
	iPowerAdvice(ENoPowerAdvice)
	{
	memclr(&iData, sizeof(iData));
	}

EXPORT_C TBool TLbsPositionUpdateRequestBase::operator== (const TLbsPositionUpdateRequestBase& aVal)
	{
	return (iType == aVal.iType) && (iTracking == aVal.iTracking) && (iData == aVal.iData);
	}
	
//
EXPORT_C TLbsPositionUpdateRequestStatus::TLbsPositionUpdateRequestStatus()
:	TLbsPositionUpdateRequestBase(EStatus)
	{
	}
	
EXPORT_C TLbsPositionUpdateRequestStatus::TLbsPositionUpdateRequestStatus(TType aType)
:	TLbsPositionUpdateRequestBase(aType)
	{
	}

EXPORT_C TBool& TLbsPositionUpdateRequestStatus::Tracking()
	{
	return iTracking;
	}
	
EXPORT_C const TBool& TLbsPositionUpdateRequestStatus::Tracking() const
	{
	return iTracking;
	}
	
	EXPORT_C TLbsPositionUpdateRequestBase::TPowerAdvice TLbsPositionUpdateRequestStatus::PowerAdvice()
	{
	return iPowerAdvice;
	}
	
	EXPORT_C void TLbsPositionUpdateRequestStatus::SetPowerAdvice(TPowerAdvice aAdvice)
	{
	iPowerAdvice = aAdvice;
	}

//
EXPORT_C TLbsPositionUpdateRequestCancel::TLbsPositionUpdateRequestCancel()
:	TLbsPositionUpdateRequestStatus(ECancel)
	{
	}

//

EXPORT_C TLbsPositionUpdateRequest::TLbsPositionUpdateRequest()
:	TLbsPositionUpdateRequestStatus(EUpdateRequest)
	{
	// Check that the update request data fits into the pre-defined buffer.
	__ASSERT_COMPILE(sizeof(TPositionUpdateRequestBody) <= sizeof(KUpdateRequestBuffer));
	
	new (&RequestBody()) TPositionUpdateRequestBody;
	}

EXPORT_C TTime& TLbsPositionUpdateRequest::TargetTime()
	{
	return RequestBody().iTargetTime;
	}
	
EXPORT_C const TTime& TLbsPositionUpdateRequest::TargetTime() const
	{
	return RequestBody().iTargetTime;
	}
		
EXPORT_C TLbsLocRequestQualityInt& TLbsPositionUpdateRequest::RequestQuality()
	{
	return RequestBody().iRequestQuality;
	}

EXPORT_C const TLbsLocRequestQualityInt& TLbsPositionUpdateRequest::RequestQuality() const
	{
	return RequestBody().iRequestQuality;
	}

EXPORT_C TBool& TLbsPositionUpdateRequest::NewClient()
	{
	return RequestBody().iNewClient;
	}
	
EXPORT_C const TBool& TLbsPositionUpdateRequest::NewClient() const
	{
	return RequestBody().iNewClient;
	}

/**
@internalcomponent
@released
*/
EXPORT_C TLbsNetPosRequestMethodInt& TLbsPositionUpdateRequest::RequestMethod()
	{
	return RequestBody().iRequestMethod;
	}

/**
@internalcomponent
@released
*/
EXPORT_C const TLbsNetPosRequestMethodInt& TLbsPositionUpdateRequest::RequestMethod() const
	{
	return RequestBody().iRequestMethod;
	}

EXPORT_C TBool& TLbsPositionUpdateRequest::HybridRequest()
	{
	return RequestBody().iHybridRequest;
	}
	
EXPORT_C const TBool& TLbsPositionUpdateRequest::HybridRequest() const
	{
	return RequestBody().iHybridRequest;
	}

inline TLbsPositionUpdateRequest::TPositionUpdateRequestBody& TLbsPositionUpdateRequest::RequestBody()
	{
	return reinterpret_cast<TLbsPositionUpdateRequest::TPositionUpdateRequestBody&>(iData);
	}
	
inline const TLbsPositionUpdateRequest::TPositionUpdateRequestBody& TLbsPositionUpdateRequest::RequestBody() const
	{
	return reinterpret_cast<const TLbsPositionUpdateRequest::TPositionUpdateRequestBody&>(iData);
	}

