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
* Description: 
*
*/



#include <lbs/lbslocclasstypes.h>
#include "lbsnrhngmsgs.h"

//
// Network Location Request Message
//

EXPORT_C TLbsNetLocationRequestMsg::TLbsNetLocationRequestMsg(
										const TLbsNetSessionIdInt& aSessionId, 
										TBool aEmergency,
										TLbsNetworkEnumInt::TLbsNetProtocolServiceInt aService, 
										const TLbsNetPosRequestQualityInt& aQuality,
										const TLbsNetPosRequestMethodInt& aMethod) :
	TLbsNetInternalMsgBase(ELocationRequest, 
						   sizeof(TLbsNetLocationRequestMsg), 
						   aSessionId)
	{
	// Must make sure that the data to be stored is not bigger than the P&S buffer.
	__ASSERT_COMPILE(sizeof(TLbsNetLocationRequestMsg) <= KMaximumMessageSizeBytes);

	iIsEmergency = aEmergency;
	iService = aService;
	iQuality = aQuality;
	iMethod = aMethod;
	}
	
EXPORT_C TBool TLbsNetLocationRequestMsg::IsEmergency() const
	{
	return iIsEmergency;
	}

EXPORT_C TLbsNetworkEnumInt::TLbsNetProtocolServiceInt TLbsNetLocationRequestMsg::Service() const
	{
	return iService;
	}
		
EXPORT_C const TLbsNetPosRequestQualityInt& TLbsNetLocationRequestMsg::Quality() const
	{
	return iQuality;
	}
	
EXPORT_C const TLbsNetPosRequestMethodInt& TLbsNetLocationRequestMsg::Method() const
	{
	return iMethod;
	}

//
// Network Location Response Message
//

/**
*/
EXPORT_C TLbsNetLocationResponseMsg::TLbsNetLocationResponseMsg(const TLbsNetSessionIdInt& aSessionId,
																TInt aReason,
																const TPositionInfoBase& aPosInfo,
																const TTime& aTimeStamp) :
	TLbsNetInternalMsgBase(ELocationResponse, 
						   sizeof(TLbsNetLocationResponseMsg), 
						   aSessionId)
	{
	// Must make sure that the data to be stored is not bigger than the buffer.
	__ASSERT_COMPILE(sizeof(TLbsNetLocationResponseMsg) <= KMaximumMessageSizeBytes);

	iReason = aReason;
	iTimeStamp = aTimeStamp;
	
	// Check that the type of aPosInfo is valid
	TUint32 type = aPosInfo.PositionClassType();
	if (((type & EPositionInfoUnknownClass) != EPositionInfoUnknownClass)
	    && ((type & EPositionInfoClass) != EPositionInfoClass)
	    && ((type & EPositionCourseInfoClass) != EPositionCourseInfoClass)
	    && ((type & EPositionSatelliteInfoClass) != EPositionSatelliteInfoClass)
	    && ((type & EPositionExtendedSatelliteInfoClass) != EPositionExtendedSatelliteInfoClass))

		{
		User::Panic(_L("LbsNetInternal"), 1);
		}
	
	// The TPositionInfoBase is just the base class, so we need to copy over 
	// the data from the actual concrete class type.
	__ASSERT_DEBUG(aPosInfo.PositionClassSize() <= sizeof(TPositionExtendedSatelliteInfo), User::Invariant());
	Mem::Copy(&iPosInfo, &aPosInfo, aPosInfo.PositionClassSize());
	}

EXPORT_C TInt TLbsNetLocationResponseMsg::Reason() const
	{
	return iReason;
	}

EXPORT_C const TPositionInfoBase& TLbsNetLocationResponseMsg::PositionInfo() const
	{
	return iPosInfo;
	}

EXPORT_C const TTime& TLbsNetLocationResponseMsg::TimeStamp() const
	{
	return iTimeStamp;
	}

//
// Location Transfer Request Message
//

EXPORT_C TLbsNetTransmitLocationRequestMsg::TLbsNetTransmitLocationRequestMsg(const TLbsNetSessionIdInt& aSessionId,
																			  const TDesC& aDestination,
									   										  TInt aPriority) :
	TLbsNetInternalMsgBase(ETransmitLocationRequest, 
						   sizeof(TLbsNetTransmitLocationRequestMsg), 
						   aSessionId)
	{
	// Must make sure that the data to be stored is not bigger than the buffer.
	__ASSERT_COMPILE(sizeof(TLbsNetTransmitLocationRequestMsg) <= KMaximumMessageSizeBytes);
	
	iPriority = aPriority;
	iDestination.Copy(aDestination);
	iOptions.SetPosMode(TPositionModuleInfo::ETechnologyUnknown);
	}

EXPORT_C TLbsNetTransmitLocationRequestMsg::TLbsNetTransmitLocationRequestMsg(const TLbsNetSessionIdInt& aSessionId,
																			  const TDesC& aDestination,
																			  TInt aPriority,
																			  const TLbsNetPosRequestOptionsTechnologyInt& aOptions) :
	TLbsNetInternalMsgBase(ETransmitLocationRequest,sizeof(TLbsNetTransmitLocationRequestMsg), aSessionId)
	{
	// Must make sure that the data to be stored is not bigger than the buffer.
	__ASSERT_COMPILE(sizeof(TLbsNetTransmitLocationRequestMsg) <= KMaximumMessageSizeBytes);
	
	
	// Set the data member values	
	iPriority = aPriority;
	iDestination.Copy(aDestination);
	iOptions = aOptions;
	}
	
EXPORT_C const TDesC& TLbsNetTransmitLocationRequestMsg::Destination() const
	{
	return iDestination;
	}
	
EXPORT_C TInt TLbsNetTransmitLocationRequestMsg::Priority() const
	{
	return iPriority;
	}

EXPORT_C const TLbsNetPosRequestOptionsTechnologyInt& TLbsNetTransmitLocationRequestMsg::Options() const
	{
	return iOptions;
	}

//
// Location Transfer Cancel Message
//

EXPORT_C TLbsNetTransmitLocationCancelMsg::TLbsNetTransmitLocationCancelMsg(const TLbsNetSessionIdInt& aSessionId,
																			TInt aReason) :
	TLbsNetInternalMsgBase(ETransmitLocationCancel, 
						   sizeof(TLbsNetTransmitLocationCancelMsg), 
						   aSessionId)
	{
	// Must make sure that the data to be stored is not bigger than the buffer.
	__ASSERT_COMPILE(sizeof(TLbsNetTransmitLocationCancelMsg) <= KMaximumMessageSizeBytes);
	
	iReason = aReason;
	}
									   

EXPORT_C TInt TLbsNetTransmitLocationCancelMsg::Reason() const
	{
	return iReason;
	}
		
//
// Location Transfer Cancel Message
//

EXPORT_C TLbsNetCancelFromPrivacyControllerMsg::TLbsNetCancelFromPrivacyControllerMsg(const TLbsNetSessionIdInt& aSessionId,
																			TInt aReason) :
	TLbsNetInternalMsgBase(ECancelFromPrivacyController, 
						   sizeof(TLbsNetCancelFromPrivacyControllerMsg), 
						   aSessionId)
	{
	// Must make sure that the data to be stored is not bigger than the buffer.
	__ASSERT_COMPILE(sizeof(TLbsNetCancelFromPrivacyControllerMsg) <= KMaximumMessageSizeBytes);
	
	iReason = aReason;
	}
									   

EXPORT_C TInt TLbsNetCancelFromPrivacyControllerMsg::Reason() const
	{
	return iReason;
	}
		
//
// MT-LR Request Message
//

EXPORT_C TLbsNetMtLrRequestMsg::TLbsNetMtLrRequestMsg(
								const TLbsNetSessionIdInt& aSessionId,
								TBool aEmergency,
								const TLbsNetPosRequestPrivacyInt& aPrivacy,
								const TLbsExternalRequestInfo& aRequestInfo) :
	TLbsNetInternalMsgBase(EPrivacyRequest,
						   sizeof(TLbsNetMtLrRequestMsg), 
						   aSessionId)
	{
	// Must make sure that the data to be stored is not bigger than the buffer.
	__ASSERT_COMPILE(sizeof(TLbsNetMtLrRequestMsg) <= KMaximumMessageSizeBytes);

	iIsEmergency = aEmergency;
	iPrivacy = aPrivacy;

	if (aRequestInfo.ClassType() == EExternalRequestInfoClass)
		{
		__ASSERT_DEBUG(aRequestInfo.ClassSize() == sizeof(TLbsExternalRequestInfo), 
					   User::Panic(_L("LbsNetInternal"), 4));
		
		Mem::Copy(&iInfo, &aRequestInfo, sizeof(TLbsExternalRequestInfo));
	}
	else 
		{
		if (aRequestInfo.ClassType() == (EExternalRequestInfoClass | EExternalRequestInfoClass2))
			{
			__ASSERT_DEBUG(aRequestInfo.ClassSize() == sizeof(TLbsExternalRequestInfo2), 
						   User::Panic(_L("LbsNetInternal"), 4));
								   
			Mem::Copy(&iInfo, &aRequestInfo, sizeof(TLbsExternalRequestInfo2));
			}
		else
	{
			User::Panic(_L("LbsNetInternal"), 5);
			}
	}
	}
	
EXPORT_C TBool TLbsNetMtLrRequestMsg::IsEmergency() const
	{
	return iIsEmergency;
	}

EXPORT_C const TLbsNetPosRequestPrivacyInt& TLbsNetMtLrRequestMsg::Privacy() const
	{
	return iPrivacy;
	}
	
EXPORT_C const TLbsExternalRequestInfo& TLbsNetMtLrRequestMsg::RequestInfo() const
	{
	return iInfo;
	}
	
//
// MT-LR Response Message
//

EXPORT_C TLbsNetMtLrReponseMsg::TLbsNetMtLrReponseMsg(const TLbsNetSessionIdInt& aSessionId,
													  const TLbsNetworkEnumInt::TLbsPrivacyResponseInt& aResponse,
													  TInt aReason) :
	TLbsNetInternalMsgBase(EPrivacyResponse,
						   sizeof(TLbsNetMtLrReponseMsg), 
						   aSessionId)
	{
	// Must make sure that the data to be stored is not bigger than the buffer.
	__ASSERT_COMPILE(sizeof(TLbsNetMtLrReponseMsg) <= KMaximumMessageSizeBytes);

	iResponse = aResponse;
	iReason = aReason;
	}
								   
	
EXPORT_C const TLbsNetworkEnumInt::TLbsPrivacyResponseInt& TLbsNetMtLrReponseMsg::Response() const
	{
	return iResponse;
	}
EXPORT_C TInt TLbsNetMtLrReponseMsg::Reason() const
	{
	return iReason;
	}

//
// Session Complete Message
//

EXPORT_C TLbsNetSessionCompleteMsg::TLbsNetSessionCompleteMsg(const TLbsNetSessionIdInt& aSessionId,
															  TInt aReason) :
	TLbsNetInternalMsgBase(ESessionComplete,
						   sizeof(TLbsNetSessionCompleteMsg), 
						   aSessionId)
	{
	// Must make sure that the data to be stored is not bigger than the buffer.
	__ASSERT_COMPILE(sizeof(TLbsNetSessionCompleteMsg) <= KMaximumMessageSizeBytes);

	iReason = aReason;
	}
	
EXPORT_C TInt TLbsNetSessionCompleteMsg::Reason() const
	{
	return iReason;
	}
	
//
// Network Location Update Message
//

EXPORT_C TLbsNetLocationUpdateMsg::TLbsNetLocationUpdateMsg(
		const TLbsNetSessionIdInt& aSessionId,
		TInt aReason,
		const TPositionInfoBase& aPosInfo,
		const TTime& aTimeStamp):
	TLbsNetInternalMsgBase(ENetworkLocationUpdate,
						   sizeof(TLbsNetLocationUpdateMsg),
						   aSessionId)
	{
	// Must make sure that the data to be stored is not bigger than the buffer.
	__ASSERT_COMPILE(sizeof(TLbsNetLocationUpdateMsg) <= KMaximumMessageSizeBytes);
	
	iReason = aReason;
	iTimeStamp = aTimeStamp;
	
	// Check that the type of aPosInfo is valid
	TUint32 type = aPosInfo.PositionClassType();
	if (((type & EPositionInfoUnknownClass) != EPositionInfoUnknownClass)
	    && ((type & EPositionInfoClass) != EPositionInfoClass)
	    && ((type & EPositionCourseInfoClass) != EPositionCourseInfoClass)
	    && ((type & EPositionSatelliteInfoClass) != EPositionSatelliteInfoClass)
	    && ((type & EPositionExtendedSatelliteInfoClass) != EPositionExtendedSatelliteInfoClass))
		{
		User::Panic(_L("LbsNetInternal"), 1);
		}
	
	// The TPositionInfoBase is just the base class, so we need to copy over 
	// the data from the actual concrete class type.
	__ASSERT_DEBUG(aPosInfo.PositionClassSize() <= sizeof(TPositionExtendedSatelliteInfo), User::Invariant());
	Mem::Copy(&iPosInfo, &aPosInfo, aPosInfo.PositionClassSize());
	}
	
EXPORT_C TInt TLbsNetLocationUpdateMsg::Reason() const
	{
	return iReason;
	}

EXPORT_C const TPositionInfoBase& TLbsNetLocationUpdateMsg::PositionInfo() const
	{
	return iPosInfo;
	}

EXPORT_C const TTime& TLbsNetLocationUpdateMsg::TimeStamp() const
	{
	return iTimeStamp;
	}
