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


#ifndef LBSNRHNGMSGS_H
#define LBSNRHNGMSGS_H

#include <e32std.h>
#include "lbscommoninternaldatatypes.h"
#include <lbs/lbsextendedsatellite.h>
#include <lbs/lbsloccommon.h>
#include "lbsnetinternalmsgtypes.h"

/**
@file
@internalTechnology
@released
*/

/** Location request originating from the network.
*/
NONSHARABLE_CLASS(TLbsNetLocationRequestMsg) : public TLbsNetInternalMsgBase
	{
public:
	IMPORT_C TLbsNetLocationRequestMsg(const TLbsNetSessionIdInt& aSessionId, 
									   TBool aEmergency,
									   TLbsNetworkEnumInt::TLbsNetProtocolServiceInt aService, 
									   const TLbsNetPosRequestQualityInt& aQuality,
									   const TLbsNetPosRequestMethodInt& aMethod);

	IMPORT_C TBool IsEmergency() const;
	IMPORT_C TLbsNetworkEnumInt::TLbsNetProtocolServiceInt Service() const;
	IMPORT_C const TLbsNetPosRequestQualityInt& Quality() const;
	IMPORT_C const TLbsNetPosRequestMethodInt& Method() const;
		
private:
	TLbsNetLocationRequestMsg();

private:
	TBool iIsEmergency;
	TLbsNetworkEnumInt::TLbsNetProtocolServiceInt iService;
	TLbsNetPosRequestQualityInt iQuality;
	TLbsNetPosRequestMethodInt iMethod;
	};
	
/** Response to a location request.
*/
NONSHARABLE_CLASS(TLbsNetLocationResponseMsg) : public TLbsNetInternalMsgBase
	{
public:
	IMPORT_C TLbsNetLocationResponseMsg(const TLbsNetSessionIdInt& aSessionId,
										TInt aReason,
										const TPositionInfoBase& aPosInfo,
										const TTime& aTimeStamp);
	
	IMPORT_C TInt Reason() const;
	IMPORT_C const TPositionInfoBase& PositionInfo() const;
	IMPORT_C const TTime& TimeStamp() const;
	
private:
	TLbsNetLocationResponseMsg();
	
private:
	TInt iReason;
	TTime iTimeStamp;
	// This class is the biggest one in the TPositionInfoBase
	// class hirearchy. If in future another class is created
	// which is bigger then it will have to be put here instead.
	TPositionExtendedSatelliteInfo iPosInfo;
	};
		
/** Request to send the current location to a third party.
*/
NONSHARABLE_CLASS(TLbsNetTransmitLocationRequestMsg) : public TLbsNetInternalMsgBase
	{
public:
	IMPORT_C TLbsNetTransmitLocationRequestMsg(const TLbsNetSessionIdInt& aSessionId,
											   const TDesC& aDestination,
												 TInt aPriority);
											   
	IMPORT_C TLbsNetTransmitLocationRequestMsg(const TLbsNetSessionIdInt& aSessionId,
											   const TDesC& aDestination,
											   TInt aPriority,
											   const TLbsNetPosRequestOptionsTechnologyInt& aOptions);
	
	IMPORT_C const TDesC& Destination() const;
	IMPORT_C TInt Priority() const;
	IMPORT_C const TLbsNetPosRequestOptionsTechnologyInt& Options() const;

private:
	TInt iPriority;
	TBuf<64> iDestination;
	TLbsNetPosRequestOptionsTechnologyInt iOptions;
	};
		
/** Cancel a previous transmit location to a third party request.
*/
NONSHARABLE_CLASS(TLbsNetTransmitLocationCancelMsg) : public TLbsNetInternalMsgBase
	{
public:
	IMPORT_C TLbsNetTransmitLocationCancelMsg(const TLbsNetSessionIdInt& aSessionId,
											   TInt aReason);
	IMPORT_C TInt Reason() const;

private:
	TLbsNetTransmitLocationCancelMsg();
	
private:
	TInt iReason;
	};
		
/** Privacy notification request arriving from the network.
*/
NONSHARABLE_CLASS(TLbsNetMtLrRequestMsg) : public TLbsNetInternalMsgBase
	{		
public:
	IMPORT_C TLbsNetMtLrRequestMsg(const TLbsNetSessionIdInt& aSessionId,
								   TBool aEmergency,
								   const TLbsNetPosRequestPrivacyInt& aPrivacy,
								   const TLbsExternalRequestInfo& aRequestInfo);
								   
	IMPORT_C TBool IsEmergency() const;
	IMPORT_C const TLbsNetPosRequestPrivacyInt& Privacy() const;
	IMPORT_C const TLbsExternalRequestInfo& RequestInfo() const;

private:
	TLbsNetMtLrRequestMsg();
	
private:
	TBool iIsEmergency;
	TLbsNetPosRequestPrivacyInt iPrivacy;
	TLbsExternalRequestInfo2 iInfo;
	};
	
/** Response to a privacy notification request.
*/
NONSHARABLE_CLASS(TLbsNetMtLrReponseMsg) : public TLbsNetInternalMsgBase
	{		
public:
	IMPORT_C TLbsNetMtLrReponseMsg(const TLbsNetSessionIdInt& aSessionId,
								   const TLbsNetworkEnumInt::TLbsPrivacyResponseInt& aResponse,
								   TInt aReason);
								   
	IMPORT_C const TLbsNetworkEnumInt::TLbsPrivacyResponseInt& Response() const;
	IMPORT_C TInt Reason() const;
private:
	TLbsNetMtLrReponseMsg();
	
private:
	TLbsNetworkEnumInt::TLbsPrivacyResponseInt iResponse;
	TInt iReason;
	};
		
/** Signals the end of a request. 

This is a generic 'complete' that is used to end all of the different types 
of request that are represented by the other message types.

The reason for completing the request will usually be KErrNone but it may
also signal an error. The particular error is defined by the type of request
being completed and the reason for the error.
*/	
NONSHARABLE_CLASS(TLbsNetSessionCompleteMsg) : public TLbsNetInternalMsgBase
	{
public:
	IMPORT_C TLbsNetSessionCompleteMsg(const TLbsNetSessionIdInt& aSessionId,
									   TInt aReason);
	
	IMPORT_C TInt Reason() const;

private:
	TLbsNetSessionCompleteMsg();
	
private:
	TInt iReason;
	};
		
/** Location update from the network.

This could be either a reference position or a 'final network position'.
You can tell which from the technology type of iPosInfo.
*/
NONSHARABLE_CLASS(TLbsNetLocationUpdateMsg) : public TLbsNetInternalMsgBase
	{
public:
	IMPORT_C TLbsNetLocationUpdateMsg(const TLbsNetSessionIdInt& aSessionId,
									  TInt aReason,
									  const TPositionInfoBase& aPosInfo,
									  const TTime& aTimeStamp);

	IMPORT_C TInt Reason() const;
	IMPORT_C const TPositionInfoBase& PositionInfo() const;
	IMPORT_C const TTime& TimeStamp() const;
	
private:
	TLbsNetLocationUpdateMsg();
	
private:
	TInt iReason;
	TTime iTimeStamp;
	// This class is the biggest one in the TPositionInfoBase
	// class hirearchy. If in future another class is created
	// which is bigger then it will have to be put here instead.
	TPositionExtendedSatelliteInfo iPosInfo;
	};

/** A Cancel from the PrivacyController which is sent by the NRH to the the NetworkGateway
*/
NONSHARABLE_CLASS(TLbsNetCancelFromPrivacyControllerMsg) : public TLbsNetInternalMsgBase
	{
public:
	IMPORT_C TLbsNetCancelFromPrivacyControllerMsg(const TLbsNetSessionIdInt& aSessionId,
											   TInt aReason);
	IMPORT_C TInt Reason() const;

private:
	TLbsNetCancelFromPrivacyControllerMsg();
	
private:
	TInt iReason;
	};

#endif // LBSNRHNGMSGS_H
