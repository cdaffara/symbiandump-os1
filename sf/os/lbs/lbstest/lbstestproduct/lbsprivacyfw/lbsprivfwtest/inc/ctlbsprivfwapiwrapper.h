/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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
//
// ctlbsprivfwapiwrapper.h
//
// Class that abstracts from the privacy API so the tests can easily choose between 
// the NetworkPrivacyAPI and PrivacyRequestAPI
//
// @internalComponent

#ifndef __CT_LBS_PRIVFW_API_WRAPPER_H__
#define __CT_LBS_PRIVFW_API_WRAPPER_H__

#include <lbs/lbsprivacyrequest.h>
#include <epos_cposnetworkprivacy.h>
#include <epos_mposverificationobserver.h>
#include <epos_cposgsmprivacyrequestinfo.h>
#include <epos_cpossuplprivacyrequestinfo.h>
#include <lbs/test/lbsprivfwdefs.h>


enum TLbsCTApiUsed
	{
	ENetworkPrivacyAPI,
	EPrivacyRequestAPI
	};
	
class TPrivacyRequestObserverRequestIdPair
	{
	public:
		TPrivacyRequestObserverRequestIdPair(MLbsPrivacyRequestObserver& aObserver, 
												TInt aRequestId)
					: iObserver(aObserver), iRequestId(aRequestId)
			{}
	public:
		MLbsPrivacyRequestObserver& iObserver;
		TInt iRequestId;
	};

class TPosVerificationObserverRequestIdPair
	{
	public:
		TPosVerificationObserverRequestIdPair(MPosVerificationObserver& aObserver, 
												TInt aRequestId)
					: iObserver(aObserver), iRequestId(aRequestId)
			{}
	public:
		MPosVerificationObserver& iObserver;
		TInt iRequestId;
	};

class CPrivFwApiWrapper : public CBase, 
							public MLbsPrivacyRequestObserver, 
							public MPosVerificationObserver
	{
public:
	static CPrivFwApiWrapper* NewL(TLbsCTApiUsed aApiUsed);
	static CPrivFwApiWrapper* NewL(TLbsCTApiUsed aApiUsed, MLbsPrivacyRequestObserver& aObserver);
	~CPrivFwApiWrapper();
	
public: // interface from CPosNetworkPrivacy
	void SetConnectionTimeout(TTimeIntervalMicroSeconds32 aTimeout);
	
	TTimeIntervalMicroSeconds32 ConnectionTimeout() const;
	
	void VerifyLocationRequestL(
					const CPosNetworkPrivacyRequestInfo& aRequestInfo,
					TInt& aRequestId,
					MPosVerificationObserver& aObserver);
	
	void VerifyLocationRequestL(
					const CPosNetworkPrivacyRequestInfo& aRequestInfo,
					TInt& aRequestId,
					MPosVerificationObserver& aObserver,
					CPosNetworkPrivacy::TRequestDecision aTimeoutStrategy);
	
	void CancelVerifyLocationRequest(TInt aRequestId);
	
	void CancelVerifyLocationRequest(
					TInt aRequestId,
					CPosNetworkPrivacy::TCancelReason aCancelReason);
	
	void NotifyLocationRequestL(
					const CPosNetworkPrivacyRequestInfo& aRequestInfo,
					TInt& aRequestId);
	
	void NotifyVerificationTimeoutL(
					const CPosNetworkPrivacyRequestInfo& aRequestInfo,
					TInt aRequestId,
					CPosNetworkPrivacy::TRequestDecision aRequestDecision);
	
	void PositionUpdateL( 
			TInt aRequestId,const TPosition& aPosition);
	
	void SetRequestInfoType(TPosNetworkPrivacyRequestInfoType aRequestInfoType);

public: // interface from CLbsPrivacyRequest
	TInt NewPrivacyRequest(TLbsPrivacyRequestId& aRequestId,
								 const TLbsPrivacyRequest& aPrivacy,
								 const TLbsExternalRequestInfo& aRequestInfo);
	
	TInt RepeatPrivacyRequest(const TLbsPrivacyRequestId aRequestId, 
								 const TLbsPrivacyRequest& aPrivacy,
								 const TLbsExternalRequestInfo& aRequestInfo);
	
	TInt CompleteRequest(TLbsPrivacyRequestId aRequestId,
								  TInt aReason);

public: // from MLbsPrivcyRequestObserver
	void OnPrivacyResponse(TLbsPrivacyRequestId aRequestId, 
				const TLbsPrivacyResponse& aResponse, TInt aError);

public: // from MPosVerificationObserver
	void HandleVerifyComplete(TInt aRequestId, TInt aCompletionCode);
	

	void SetBadRequestId();
	
private:
	CPrivFwApiWrapper(TLbsCTApiUsed aApiUsed, MLbsPrivacyRequestObserver& aObserver);
	CPrivFwApiWrapper(TLbsCTApiUsed aApiUsed);
	
	void ConstructL();
	
private:
	void RemoveObserverForRequest(TInt aRequestId);
	void ConvertNetworkPrivacyApiDataTypeToPrivacyRequestApiDataType(const CPosNetworkPrivacyRequestInfo& aRequestInfo, 
						TLbsExternalRequestInfo& aExternalRequestInfo);
	void ConvertNetworkPrivacyApiDataTypeToPrivacyRequestApiDataType(const CPosNetworkPrivacyRequestInfo& aRequestInfo, 
				const CPosNetworkPrivacy::TRequestDecision& aDecision, 
				TLbsPrivacyRequest& aPrivacy, TLbsExternalRequestInfo& aExternalRequestInfo);
	void ConvertPrivacyRequestApiDataTypeToNetworkPrivacyApiDataType(const TLbsPrivacyRequest& aPrivacy, 
			const TLbsExternalRequestInfo& aExternalRequestInfo, 
			CPosNetworkPrivacyRequestInfo*& aRequestInfo, 
			CPosNetworkPrivacy::TRequestDecision& aDecision);
	void ConvertPrivacyRequestApiDataTypeToNetworkPrivacyApiDataType(const TLbsExternalRequestInfo& aExternalRequestInfo, 
			CPosNetworkPrivacyRequestInfo*& aRequestInfo);
	
	TLbsExternalRequestInfo::TFormatIndicator ConvertGSMIdTypeToFormatIndicator(CPosGSMPrivacyRequestInfo::TIdType aIdType);
	TLbsExternalRequestInfo::TFormatIndicator ConvertSUPLIdTypeToFormatIndicator(CPosSUPLPrivacyRequestInfo::TIdType aIdType);
	CPosGSMPrivacyRequestInfo::TIdType ConvertFormatIndicatorToGSMIdType(TLbsExternalRequestInfo::TFormatIndicator aIdType);
	CPosSUPLPrivacyRequestInfo::TIdType ConvertFormatIndicatorToSUPLIdType(TLbsExternalRequestInfo::TFormatIndicator aIdType);
	
private:
	// To decide which API to use
	TLbsCTApiUsed iApiUsed;
	// Pointer to the Network Privacy API
	CPosNetworkPrivacy* iPosNetworkPrivacy;
	// Pointer to the Privacy Request API
	CLbsPrivacyRequest* iPrivacyRequest;
	TPosNetworkPrivacyRequestInfoType iRequestInfoType;

	//Flag to idicate that an invalid (<1) requestId value has been set
	TBool iBadRequestId;

	// Flag to tell if the observer for the privacy request has been set
	TBool iObserverForPrivacyRequestPresent;
	// The observer for a privacy request. This is needed permanently by the Privacy Request API
	MLbsPrivacyRequestObserver& iPrivacyRequestObserver;

	RArray<TPrivacyRequestObserverRequestIdPair> iPrivacyRequestObservers;
	RArray<TPosVerificationObserverRequestIdPair> iPosVerificationObservers;
	RArray<TInt> iDummyObservers;
	RArray<TInt> iRequestsThatNeedCompleting;
	};

#endif // __CT_LBS_PRIVFW_API_WRAPPER_H__
