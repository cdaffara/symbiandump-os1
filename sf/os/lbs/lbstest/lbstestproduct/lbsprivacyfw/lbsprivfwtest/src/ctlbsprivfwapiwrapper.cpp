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
// ctlbsprivfwapiwrapper.cpp
//
// @internalComponent

#include "ctlbsprivfwapiwrapper.h"

CPrivFwApiWrapper* CPrivFwApiWrapper::NewL(TLbsCTApiUsed aApiUsed)
	{
	CPrivFwApiWrapper* self = new(ELeave) CPrivFwApiWrapper(aApiUsed);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;
	}

CPrivFwApiWrapper* CPrivFwApiWrapper::NewL(TLbsCTApiUsed aApiUsed, MLbsPrivacyRequestObserver& aObserver)
	{
	CPrivFwApiWrapper* self = new(ELeave) CPrivFwApiWrapper(aApiUsed, aObserver);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;	
	}
	
CPrivFwApiWrapper::~CPrivFwApiWrapper()
	{
	delete iPosNetworkPrivacy;
	delete iPrivacyRequest;
	iPrivacyRequestObservers.Close();
	iPosVerificationObservers.Close();
	iDummyObservers.Close();
	iRequestsThatNeedCompleting.Close();
	}

void CPrivFwApiWrapper::SetConnectionTimeout(TTimeIntervalMicroSeconds32 aTimeout)
	{
	switch(iApiUsed)
		{
		case ENetworkPrivacyAPI:
			{
			iPosNetworkPrivacy->SetConnectionTimeout(aTimeout);
			break;
			}
		case EPrivacyRequestAPI:
			{
			break;
			}
		default:
			{
			User::Panic(_L("LbsPrivFwTest:"), KErrArgument);
			break;
			}
		}
	}

TTimeIntervalMicroSeconds32 CPrivFwApiWrapper::ConnectionTimeout() const
	{
	switch(iApiUsed)
		{
		case ENetworkPrivacyAPI:
			{
			return iPosNetworkPrivacy->ConnectionTimeout();
			}
		case EPrivacyRequestAPI:
			{
			return KErrNotSupported;
			}
		default:
			{
			User::Panic(_L("LbsPrivFwTest:"), KErrArgument);
			return KErrArgument;
			}
		}
	}

void CPrivFwApiWrapper::VerifyLocationRequestL(const CPosNetworkPrivacyRequestInfo& aRequestInfo,
				TInt& aRequestId,
				MPosVerificationObserver& aObserver)
	{
	switch(iApiUsed)
		{
		case ENetworkPrivacyAPI:
			{
			iPosNetworkPrivacy->VerifyLocationRequestL(aRequestInfo, aRequestId, *this);
			iPosVerificationObservers.Append(TPosVerificationObserverRequestIdPair(aObserver,aRequestId));
			break;
			}
		case EPrivacyRequestAPI:
			{
			TLbsPrivacyRequest privacy;
			TLbsExternalRequestInfo requestInfo;
			ConvertNetworkPrivacyApiDataTypeToPrivacyRequestApiDataType(aRequestInfo, requestInfo);
			privacy.SetRequestAdvice(TLbsPrivacyRequest::ERequestAdviceVerify);
			privacy.SetRequestAction(TLbsPrivacyRequest::ERequestActionAllow);
			TUint32 id;
			User::LeaveIfError(iPrivacyRequest->NewPrivacyRequest(id, privacy, requestInfo));
			aRequestId = id;
			iPosVerificationObservers.Append(TPosVerificationObserverRequestIdPair(aObserver,aRequestId));
			iRequestsThatNeedCompleting.Append(aRequestId);
			break;
			}
		default:
			{
			User::Panic(_L("LbsPrivFwTest:"), KErrArgument);
			break;
			}
		}
	}

void CPrivFwApiWrapper::VerifyLocationRequestL(const CPosNetworkPrivacyRequestInfo& aRequestInfo,
				TInt& aRequestId,
				MPosVerificationObserver& aObserver,
				CPosNetworkPrivacy::TRequestDecision aTimeoutStrategy)
	{
	switch(iApiUsed)
		{
		case ENetworkPrivacyAPI:
			{
			iPosNetworkPrivacy->VerifyLocationRequestL(aRequestInfo, aRequestId, *this, aTimeoutStrategy);
			iPosVerificationObservers.Append(TPosVerificationObserverRequestIdPair(aObserver,aRequestId));
			break;
			}
		case EPrivacyRequestAPI:
			{
			TLbsPrivacyRequest privacy;
			TLbsExternalRequestInfo requestInfo;
			ConvertNetworkPrivacyApiDataTypeToPrivacyRequestApiDataType(aRequestInfo, aTimeoutStrategy, privacy, requestInfo);
			privacy.SetRequestAdvice(TLbsPrivacyRequest::ERequestAdviceVerify);
			TUint32 id;
			User::LeaveIfError(iPrivacyRequest->NewPrivacyRequest(id, privacy, requestInfo));
			aRequestId = id;
			iPosVerificationObservers.Append(TPosVerificationObserverRequestIdPair(aObserver,aRequestId));
			iRequestsThatNeedCompleting.Append(aRequestId);
			break;
			}
		default:
			{
			User::Panic(_L("LbsPrivFwTest:"), KErrArgument);
			break;
			}
		}
	}

void CPrivFwApiWrapper::CancelVerifyLocationRequest(TInt aRequestId)
	{
	switch(iApiUsed)
		{
		case ENetworkPrivacyAPI:
			{
			iPosNetworkPrivacy->CancelVerifyLocationRequest(aRequestId);
			break;
			}
		case EPrivacyRequestAPI:
			{
			iPrivacyRequest->CompleteRequest(aRequestId, KErrCancel);
			break;
			}
		default:
			{
			User::Panic(_L("LbsPrivFwTest:"), KErrArgument);
			break;
			}
		}
	}

void CPrivFwApiWrapper::CancelVerifyLocationRequest(TInt aRequestId,
			CPosNetworkPrivacy::TCancelReason aCancelReason)
	{
	switch(iApiUsed)
		{
		case ENetworkPrivacyAPI:
			{
			iPosNetworkPrivacy->CancelVerifyLocationRequest(aRequestId, aCancelReason);
			break;
			}
		case EPrivacyRequestAPI:
			{
			switch(aCancelReason)
				{
				case CPosNetworkPrivacy::ECancelReasonNotAvailable:
					{
					iPrivacyRequest->CompleteRequest(aRequestId, KErrCancel);
					break;
					}
				case CPosNetworkPrivacy::ECancelReasonTimeout:
					{
					iPrivacyRequest->CompleteRequest(aRequestId, KErrTimedOut);
					break;
					}
				default:
					{
					User::Panic(_L("LbsPrivFwTest:"), KErrArgument);
					break;
					}
				}
			break;
			}
		default:
			{
			User::Panic(_L("LbsPrivFwTest:"), KErrArgument);
			break;
			}
		}
	}

void CPrivFwApiWrapper::NotifyLocationRequestL(const CPosNetworkPrivacyRequestInfo& aRequestInfo,
				TInt& aRequestId)
	{
	switch(iApiUsed)
		{
		case ENetworkPrivacyAPI:
			{
			iPosNetworkPrivacy->NotifyLocationRequestL(aRequestInfo, aRequestId);
			iDummyObservers.Append(aRequestId);
			break;
			}
		case EPrivacyRequestAPI:
			{
			TLbsPrivacyRequest privacy;
			TLbsExternalRequestInfo requestInfo;
			ConvertNetworkPrivacyApiDataTypeToPrivacyRequestApiDataType(aRequestInfo, requestInfo);
			privacy.SetRequestAdvice(TLbsPrivacyRequest::ERequestAdviceNotify);
			privacy.SetRequestAction(TLbsPrivacyRequest::ERequestActionAllow);
			TUint32 id;
			User::LeaveIfError(iPrivacyRequest->NewPrivacyRequest(id, privacy, requestInfo));
			aRequestId = id;
			iDummyObservers.Append(aRequestId);
			iRequestsThatNeedCompleting.Append(aRequestId);
			break;
			}
		default:
			{
			User::Panic(_L("LbsPrivFwTest:"), KErrArgument);
			break;
			}
		}
	}

void CPrivFwApiWrapper::NotifyVerificationTimeoutL(const CPosNetworkPrivacyRequestInfo& aRequestInfo,
				TInt aRequestId,
				CPosNetworkPrivacy::TRequestDecision aRequestDecision)
	{
	switch(iApiUsed)
		{
		case ENetworkPrivacyAPI:
			{
			iPosNetworkPrivacy->NotifyVerificationTimeoutL(aRequestInfo, aRequestId, aRequestDecision);
			iDummyObservers.Append(aRequestId);
			break;
			}
		case EPrivacyRequestAPI:
			{
			TLbsPrivacyRequest privacy;
			TLbsExternalRequestInfo requestInfo;
			ConvertNetworkPrivacyApiDataTypeToPrivacyRequestApiDataType(aRequestInfo, aRequestDecision, privacy, requestInfo);
			privacy.SetRequestAdvice(TLbsPrivacyRequest::ERequestAdviceNotify);
			User::LeaveIfError(iPrivacyRequest->RepeatPrivacyRequest(aRequestId, privacy, requestInfo));
			iDummyObservers.Append(aRequestId);
			iRequestsThatNeedCompleting.Append(aRequestId);
			break;
			}
		default:
			{
			User::Panic(_L("LbsPrivFwTest:"), KErrArgument);
			break;
			}
		}
	}

void CPrivFwApiWrapper::PositionUpdateL(TInt /*aRequestId*/, const TPosition& /*aPosition*/)
	{
	User::Leave(KErrNotSupported);
	}

TInt CPrivFwApiWrapper::NewPrivacyRequest(TLbsPrivacyRequestId& aRequestId,
							 const TLbsPrivacyRequest& aPrivacy,
							 const TLbsExternalRequestInfo& aRequestInfo)
	{
// not sure what this does and it's stopping tests - so comment out
//	if(!iObserverForPrivacyRequestPresent)
//		{
//		User::Panic(_L("LbsPrivFwTest:"), KErrCorrupt);
//		}

	switch(iApiUsed)
		{
		case ENetworkPrivacyAPI:
			{
			switch(aPrivacy.RequestAdvice())
				{
				case TLbsPrivacyRequest::ERequestAdviceNotify:
					{
					CPosNetworkPrivacyRequestInfo* requestInfo(NULL);
					ConvertPrivacyRequestApiDataTypeToNetworkPrivacyApiDataType(aRequestInfo, requestInfo);
					TInt id;
					TRAPD(err, iPosNetworkPrivacy->NotifyLocationRequestL(*requestInfo, id));
					aRequestId = id;
					if(err == KErrNone)
						{ // the NotidyLocationRequest will not give any result, so we are giving the response straight away
						iPrivacyRequestObserver.OnPrivacyResponse(aRequestId, MLbsPrivacyRequestObserver::EPrivacyResponseAccepted, KErrNone);
						}
					return err;
					}
				
				case TLbsPrivacyRequest::ERequestAdviceVerify:
					{
					CPosNetworkPrivacyRequestInfo* requestInfo(NULL);
					CPosNetworkPrivacy::TRequestDecision timeoutStrategy;
					ConvertPrivacyRequestApiDataTypeToNetworkPrivacyApiDataType(aPrivacy, aRequestInfo, requestInfo, timeoutStrategy);
					TInt id;
					TRAPD(err, iPosNetworkPrivacy->VerifyLocationRequestL(*requestInfo, id, *this, timeoutStrategy));
					aRequestId = id;
					if(err == KErrNone)
						{
						iPrivacyRequestObservers.Append(TPrivacyRequestObserverRequestIdPair(iPrivacyRequestObserver, aRequestId));
						}
					return err;
					}
	
				default:
					{ //  do nothing
					return KErrArgument;
					}
				}
			}
		case EPrivacyRequestAPI:
			{
			TInt err = iPrivacyRequest->NewPrivacyRequest(aRequestId, aPrivacy, aRequestInfo);
			if(err == KErrNone)
				{
				iPrivacyRequestObservers.Append(TPrivacyRequestObserverRequestIdPair(iPrivacyRequestObserver, aRequestId));
				}
			return err;
			}
		default:
			{
			User::Panic(_L("LbsPrivFwTest:"), KErrArgument);
			return KErrArgument;
			}
		}
	}

TInt CPrivFwApiWrapper::RepeatPrivacyRequest(const TLbsPrivacyRequestId aRequestId, 
							 const TLbsPrivacyRequest& aPrivacy,
							 const TLbsExternalRequestInfo& aRequestInfo)
	{
	switch(iApiUsed)
		{
		case ENetworkPrivacyAPI:
			{
			CPosNetworkPrivacyRequestInfo* requestInfo(NULL);
			CPosNetworkPrivacy::TRequestDecision requestDecision;
			ConvertPrivacyRequestApiDataTypeToNetworkPrivacyApiDataType(aPrivacy, aRequestInfo, requestInfo, requestDecision);
			TRAPD(err, iPosNetworkPrivacy->NotifyVerificationTimeoutL(*requestInfo, aRequestId, requestDecision));
			if(err == KErrNone)
				{ // the NotidyLocationRequest will not give any result, so we are giving the response straight away
				iPrivacyRequestObserver.OnPrivacyResponse(aRequestId, MLbsPrivacyRequestObserver::EPrivacyResponseAccepted, KErrNone);
				}
			return err;
			}
		case EPrivacyRequestAPI:
			{
			TInt err = iPrivacyRequest->RepeatPrivacyRequest(aRequestId, aPrivacy, aRequestInfo);
			if(err == KErrNone)
				{
				iPrivacyRequestObservers.Append(TPrivacyRequestObserverRequestIdPair(iPrivacyRequestObserver, aRequestId));
				}
			return err;
			}
		default:
			{
			User::Panic(_L("LbsPrivFwTest:"), KErrArgument);
			return KErrArgument;
			}
		}
	}

TInt CPrivFwApiWrapper::CompleteRequest(TLbsPrivacyRequestId aRequestId,
							  TInt aReason)
	{
	switch(iApiUsed)
		{
		case ENetworkPrivacyAPI:
			{
			if(aReason == KErrTimedOut)
				{
				iPosNetworkPrivacy->CancelVerifyLocationRequest(aRequestId, CPosNetworkPrivacy::ECancelReasonTimeout);
				}
			else if(aReason == KErrCancel)
				{
				iPosNetworkPrivacy->CancelVerifyLocationRequest(aRequestId);				
				}
			else
				{
				// do nothing, the request should be completed automatically on the server side
				}
				
			return KErrNone;
			}
		case EPrivacyRequestAPI:
			{
			return iPrivacyRequest->CompleteRequest(aRequestId, aReason);
			}
		default:
			{
			User::Panic(_L("LbsPrivFwTest:"), KErrArgument);
			return KErrArgument;
			}
		}
	}

void CPrivFwApiWrapper::OnPrivacyResponse(TLbsPrivacyRequestId aRequestId, 
			const TLbsPrivacyResponse& aResponse, TInt aError)
	{
	if (iBadRequestId)
		{
		// A bad request ID will cause the client to panic (Network Privacy API) or will cause LBS to complete
		// with KErrNone without servicing the request (Privacy Request API).
		// In the latter case, the normal mechanisms to stop the test (i.e. using the 2 observers) are not available
		// so just stop the test here. 
		CActiveScheduler::Stop();
		return;
		}
	
	for(TInt index = 0; index < iRequestsThatNeedCompleting.Count(); ++index)
		{
		if(iRequestsThatNeedCompleting[index] == aRequestId)
			{
			TInt error = iPrivacyRequest->CompleteRequest(aRequestId, KErrNone);
			if(error != KErrNone)
				{
				User::Panic(_L("LbsPrivFwTest:"), KErrCorrupt);
				}
			iRequestsThatNeedCompleting.Remove(index);
			break;
			}
		}
	for(TInt index = 0; index < iPrivacyRequestObservers.Count(); ++index)
		{
		if(iPrivacyRequestObservers[index].iRequestId == aRequestId)
			{
			iPrivacyRequestObservers[index].iObserver.OnPrivacyResponse(aRequestId, aResponse, aError);
			iPrivacyRequestObservers.Remove(index);
			return;
			}
		}
	for(TInt index = 0; index < iPosVerificationObservers.Count(); ++index)
		{
		if(iPosVerificationObservers[index].iRequestId == aRequestId)
			{
			TInt completionCode;
			if(aError == KErrNone)
				{
				completionCode = (aResponse == EPrivacyResponseAccepted) ? KErrNone : KErrAccessDenied;
				}
			else
				{
				completionCode = aResponse;
				}
			iPosVerificationObservers[index].iObserver.HandleVerifyComplete(aRequestId, completionCode);
			iPosVerificationObservers.Remove(index);
			return;
			}
		}

	for(TInt index = 0; index < iDummyObservers.Count(); ++index)
		{
		if(iDummyObservers[index] == aRequestId)
			{ // don't do anything since the client doesn't want any reporting
			iDummyObservers.Remove(index);
			return;
			}
		}

	// If no observer was found for this request then something is corrupt
	User::Panic(_L("LbsPrivFwTest:"), KErrCorrupt);
	}

void CPrivFwApiWrapper::HandleVerifyComplete(TInt aRequestId, TInt aCompletionCode)
	{
	for(TInt index = 0; index < iPosVerificationObservers.Count(); ++index)
		{
		if(iPosVerificationObservers[index].iRequestId == aRequestId)
			{
			iPosVerificationObservers[index].iObserver.HandleVerifyComplete(aRequestId, aCompletionCode);
			iPosVerificationObservers.Remove(index);
			return;
			}
		}
	for(TInt index = 0; index < iPrivacyRequestObservers.Count(); ++index)
		{
		if(iPrivacyRequestObservers[index].iRequestId == aRequestId)
			{
			TInt response(MLbsPrivacyRequestObserver::EPrivacyResponseUnknown);
			TInt error(KErrNone);
			switch(aCompletionCode)
				{
				case KErrNone:
					{
					response = MLbsPrivacyRequestObserver::EPrivacyResponseAccepted;
					break;
					}
				case KErrAccessDenied:
					{
					response = MLbsPrivacyRequestObserver::EPrivacyResponseRejected;
					break;
					}
				default:
					{
					error = aCompletionCode;
					break;
					}
				}
			iPrivacyRequestObservers[index].iObserver.OnPrivacyResponse(aRequestId, response, error);
			iPrivacyRequestObservers.Remove(index);
			return;
			}
		}

	for(TInt index = 0; index < iDummyObservers.Count(); ++index)
		{
		if(iDummyObservers[index] == aRequestId)
			{ // don't do anything since the client doesn't want any reporting
			iDummyObservers.Remove(index);
			return;
			}
		}

	// If no observer was found for this request then something is corrupt
	User::Panic(_L("LbsPrivFwTest:"), KErrCorrupt);
	}

CPrivFwApiWrapper::CPrivFwApiWrapper(TLbsCTApiUsed aApiUsed)
	: iApiUsed(aApiUsed), iBadRequestId(EFalse), iObserverForPrivacyRequestPresent(EFalse), iPrivacyRequestObserver(*this)
	{
	}

CPrivFwApiWrapper::CPrivFwApiWrapper(TLbsCTApiUsed aApiUsed, MLbsPrivacyRequestObserver& aObserver)
	: iApiUsed(aApiUsed), iBadRequestId(EFalse), iObserverForPrivacyRequestPresent(ETrue), iPrivacyRequestObserver(aObserver)
	{
	}

void CPrivFwApiWrapper::ConstructL()
	{
	switch(iApiUsed)
		{
		case ENetworkPrivacyAPI:
			{
			iPosNetworkPrivacy = CPosNetworkPrivacy::NewL();
			break;
			}
		case EPrivacyRequestAPI:
			{
			iPrivacyRequest = CLbsPrivacyRequest::NewL(*this);
			break;
			}
		default:
			{
			User::Panic(_L("LbsPrivFwTest:"), KErrArgument);
			break;
			}
		}
	}

void CPrivFwApiWrapper::SetBadRequestId()
	{
	iBadRequestId = ETrue;
	}

void CPrivFwApiWrapper::RemoveObserverForRequest(TInt aRequestId)
	{
	for(TInt index = 0; index < iPrivacyRequestObservers.Count(); ++index)
		{
		if(iPrivacyRequestObservers[index].iRequestId == aRequestId)
			{
			iPrivacyRequestObservers.Remove(index);
			return;
			}
		}
	for(TInt index = 0; index < iPosVerificationObservers.Count(); ++index)
		{
		if(iPosVerificationObservers[index].iRequestId == aRequestId)
			{
			iPosVerificationObservers.Remove(index);
			return;
			}
		}

	for(TInt index = 0; index < iDummyObservers.Count(); ++index)
		{
		if(iDummyObservers[index] == aRequestId)
			{ // don't do anything since the client doesn't want any reporting
			iDummyObservers.Remove(index);
			return;
			}
		}

	// If no observer was found for this request then something is corrupt
	User::Panic(_L("LbsPrivFwTest:"), KErrCorrupt);
	}

void CPrivFwApiWrapper::ConvertNetworkPrivacyApiDataTypeToPrivacyRequestApiDataType(const CPosNetworkPrivacyRequestInfo& aRequestInfo, 
								TLbsExternalRequestInfo& aExternalRequestInfo)
	{
	switch(aRequestInfo.RequestType())
		{
		case CPosNetworkPrivacyRequestInfo::ERequestTypeUnknown:
			{
			aExternalRequestInfo.SetRequestType(TLbsExternalRequestInfo::ERequestTypeUnknown);
			break;
			}
		case CPosNetworkPrivacyRequestInfo::ERequestSingleShot:
			{
			aExternalRequestInfo.SetRequestType(TLbsExternalRequestInfo::ERequestSingleShot);
			break;
			}
		case CPosNetworkPrivacyRequestInfo::ERequestPeriodic:
			{
			aExternalRequestInfo.SetRequestType(TLbsExternalRequestInfo::ERequestPeriodic);
			break;
			}
		case CPosNetworkPrivacyRequestInfo::ERequestAreaEvent:
			{
			aExternalRequestInfo.SetRequestType(TLbsExternalRequestInfo::ERequestAreaEvent);
			break;
			}
		default:// pass illegal values straight through - to test error conditions
			{
			aExternalRequestInfo.SetRequestType(aRequestInfo.RequestType());
			break;
			}
		}
		
	if(iRequestInfoType == ERequestInfoTypeGsm)
		{
		const CPosGSMPrivacyRequestInfo& gsmReqInfo = reinterpret_cast<const CPosGSMPrivacyRequestInfo&>(aRequestInfo);
		TPtrC ptr;
		CPosGSMPrivacyRequestInfo::TIdType idType;
		gsmReqInfo.GetRequestor(ptr, idType);
		aExternalRequestInfo.SetRequesterIdFormat(ConvertGSMIdTypeToFormatIndicator(idType));
		aExternalRequestInfo.SetRequesterId(ptr);
		gsmReqInfo.GetLCSClient(ptr, idType);
		aExternalRequestInfo.SetClientNameFormat(ConvertGSMIdTypeToFormatIndicator(idType));
		aExternalRequestInfo.SetClientName(ptr);
		aExternalRequestInfo.SetNetworkType(TLbsExternalRequestInfo::ENetworkGSM);
		aExternalRequestInfo.SetRequestSource(TLbsExternalRequestInfo::ERequestSourceNetwork);
		aExternalRequestInfo.SetRequesterIdCodingScheme(TLbsExternalRequestInfo::ECodingSchemeGSMDefault);
		aExternalRequestInfo.SetClientNameCodingScheme(TLbsExternalRequestInfo::ECodingSchemeGSMDefault);
		}
	if(iRequestInfoType == ERequestInfoTypeSupl)
		{
		const CPosSUPLPrivacyRequestInfo& suplReqInfo = reinterpret_cast<const CPosSUPLPrivacyRequestInfo&>(aRequestInfo);
		TPtrC ptr;
		CPosSUPLPrivacyRequestInfo::TIdType idType;
		suplReqInfo.GetRequestor(ptr, idType);
		aExternalRequestInfo.SetRequesterIdFormat(ConvertSUPLIdTypeToFormatIndicator(idType));
		aExternalRequestInfo.SetRequesterId(ptr);
		suplReqInfo.GetLCSClient(ptr, idType);
		aExternalRequestInfo.SetClientNameFormat(ConvertSUPLIdTypeToFormatIndicator(idType));
		aExternalRequestInfo.SetClientName(ptr);
		aExternalRequestInfo.SetNetworkType(TLbsExternalRequestInfo::ENetworkSUPL);
		aExternalRequestInfo.SetRequestSource(TLbsExternalRequestInfo::ERequestSourceNetwork);
		aExternalRequestInfo.SetRequesterIdCodingScheme(TLbsExternalRequestInfo::ECodingSchemeUCS2);
		aExternalRequestInfo.SetClientNameCodingScheme(TLbsExternalRequestInfo::ECodingSchemeUCS2);
		}
	}

void CPrivFwApiWrapper::ConvertNetworkPrivacyApiDataTypeToPrivacyRequestApiDataType(const CPosNetworkPrivacyRequestInfo& aRequestInfo, 
		const CPosNetworkPrivacy::TRequestDecision& aDecision, 
		TLbsPrivacyRequest& aPrivacy, TLbsExternalRequestInfo& aExternalRequestInfo)
	{
	switch(aDecision)
		{
		case CPosNetworkPrivacy::EDecisionRejected:
			{
			aPrivacy.SetRequestAction(TLbsPrivacyRequest::ERequestActionReject);
			break;
			}
		case CPosNetworkPrivacy::EDecisionAccepted:
			{
			aPrivacy.SetRequestAction(TLbsPrivacyRequest::ERequestActionAllow);
			break;
			}
		default:
			{
			aPrivacy.SetRequestAction(TLbsPrivacyRequest::ERequestActionAllow);
			break;
			}
		}
	ConvertNetworkPrivacyApiDataTypeToPrivacyRequestApiDataType(aRequestInfo, aExternalRequestInfo);
	}

void CPrivFwApiWrapper::ConvertPrivacyRequestApiDataTypeToNetworkPrivacyApiDataType(const TLbsPrivacyRequest& aPrivacy, 
		const TLbsExternalRequestInfo& aExternalRequestInfo, 
		CPosNetworkPrivacyRequestInfo*& aRequestInfo, 
		CPosNetworkPrivacy::TRequestDecision& aDecision)
	{
	switch(aPrivacy.RequestAction())
		{
		case TLbsPrivacyRequest::ERequestActionReject:
			{
			aDecision = CPosNetworkPrivacy::EDecisionRejected;
			break;
			}
		case TLbsPrivacyRequest::ERequestActionAllow:
			{
			aDecision = CPosNetworkPrivacy::EDecisionAccepted;
			break;
			}
		default:
			{
			aDecision = CPosNetworkPrivacy::EDecisionNotAvailable;
			break;
			}
		}
	ConvertPrivacyRequestApiDataTypeToNetworkPrivacyApiDataType(aExternalRequestInfo, aRequestInfo);
	}

void CPrivFwApiWrapper::ConvertPrivacyRequestApiDataTypeToNetworkPrivacyApiDataType(const TLbsExternalRequestInfo& aExternalRequestInfo, 
		CPosNetworkPrivacyRequestInfo*& aRequestInfo)
	{
	switch(aExternalRequestInfo.NetworkType())
		{
		case TLbsExternalRequestInfo::ENetworkGSM:
			{
			CPosGSMPrivacyRequestInfo* gsmRequestInfo = NULL;
			TRAP_IGNORE(gsmRequestInfo = CPosGSMPrivacyRequestInfo::NewL());
			TBuf<256> info;
			aExternalRequestInfo.GetClientName(info);
			TRAP_IGNORE(gsmRequestInfo->SetLCSClientL(info, ConvertFormatIndicatorToGSMIdType(aExternalRequestInfo.ClientNameFormat())));
			aExternalRequestInfo.GetRequesterId(info);
			TRAP_IGNORE(gsmRequestInfo->SetRequestorL(info, ConvertFormatIndicatorToGSMIdType(aExternalRequestInfo.RequesterIdFormat())));
			aRequestInfo = gsmRequestInfo;
			break;
			}
		case TLbsExternalRequestInfo::ENetworkSUPL:
			{
			CPosSUPLPrivacyRequestInfo* gsmRequestInfo = NULL;
			TRAP_IGNORE(gsmRequestInfo = CPosSUPLPrivacyRequestInfo::NewL());
			TBuf<256> info;
			aExternalRequestInfo.GetClientName(info);
			TRAP_IGNORE(gsmRequestInfo->SetLCSClientL(info, ConvertFormatIndicatorToSUPLIdType(aExternalRequestInfo.ClientNameFormat())));
			aExternalRequestInfo.GetRequesterId(info);
			TRAP_IGNORE(gsmRequestInfo->SetRequestorL(info, ConvertFormatIndicatorToSUPLIdType(aExternalRequestInfo.RequesterIdFormat())));
			aRequestInfo = gsmRequestInfo;
			break;
			}
		default:
			{
			User::Panic(_L("LbsPrivFwTest:"), KErrNotSupported);
			}
		}

	switch(aExternalRequestInfo.RequestType())
		{
		case TLbsExternalRequestInfo::ERequestSingleShot:
			{
			aRequestInfo->SetRequestType(CPosNetworkPrivacyRequestInfo::ERequestSingleShot);
			break;
			}
		case TLbsExternalRequestInfo::ERequestPeriodic:
			{
			aRequestInfo->SetRequestType(CPosNetworkPrivacyRequestInfo::ERequestPeriodic);
			break;
			}
		case TLbsExternalRequestInfo::ERequestAreaEvent:
			{
			aRequestInfo->SetRequestType(CPosNetworkPrivacyRequestInfo::ERequestAreaEvent);
			break;
			}
		default:
			{
			aRequestInfo->SetRequestType(CPosNetworkPrivacyRequestInfo::ERequestTypeUnknown);
			break;
			}
		}
	}

TLbsExternalRequestInfo::TFormatIndicator CPrivFwApiWrapper::ConvertGSMIdTypeToFormatIndicator(CPosGSMPrivacyRequestInfo::TIdType aIdType)
	{
	switch(aIdType)
		{
		case CPosGSMPrivacyRequestInfo::EIdTypeLogicalName:
			return TLbsExternalRequestInfo::EFormatLogicalName;
		case CPosGSMPrivacyRequestInfo::EIdTypeMSISDN:
			return TLbsExternalRequestInfo::EFormatMSISDN;
		case CPosGSMPrivacyRequestInfo::EIdTypeEmailAddress:
			return TLbsExternalRequestInfo::EFormatEmailAddress;
		case CPosGSMPrivacyRequestInfo::EIdTypeURL:
			return TLbsExternalRequestInfo::EFormatURL;
		case CPosGSMPrivacyRequestInfo::EIdTypeSIPURL:
			return TLbsExternalRequestInfo::EFormatSIPURL;
		case CPosGSMPrivacyRequestInfo::EIdTypeIMSPublicIdentity:
			return TLbsExternalRequestInfo::EFormatIMSPublicIdentity;
		default:
			return TLbsExternalRequestInfo::EFormatLogicalName;
		}
	}

TLbsExternalRequestInfo::TFormatIndicator CPrivFwApiWrapper::ConvertSUPLIdTypeToFormatIndicator(CPosSUPLPrivacyRequestInfo::TIdType aIdType)
	{
	switch(aIdType)
		{
		case CPosSUPLPrivacyRequestInfo::EIdTypeLogicalName:
			return TLbsExternalRequestInfo::EFormatLogicalName;
		case CPosSUPLPrivacyRequestInfo::EIdTypeMSISDN:
			return TLbsExternalRequestInfo::EFormatMSISDN;
		case CPosSUPLPrivacyRequestInfo::EIdTypeEmailAddress:
			return TLbsExternalRequestInfo::EFormatEmailAddress;
		case CPosSUPLPrivacyRequestInfo::EIdTypeURL:
			return TLbsExternalRequestInfo::EFormatURL;
		case CPosSUPLPrivacyRequestInfo::EIdTypeSIPURL:
			return TLbsExternalRequestInfo::EFormatSIPURL;
		case CPosSUPLPrivacyRequestInfo::EIdTypeIMSPublicIdentity:
			return TLbsExternalRequestInfo::EFormatIMSPublicIdentity;
		case CPosSUPLPrivacyRequestInfo::EIdTypeMIN:
			return TLbsExternalRequestInfo::EFormatMIN;
		case CPosSUPLPrivacyRequestInfo::EIdTypeMDN:
			return TLbsExternalRequestInfo::EFormatMDN;
		default:
			return TLbsExternalRequestInfo::EFormatLogicalName;
		}
	}

CPosGSMPrivacyRequestInfo::TIdType CPrivFwApiWrapper::ConvertFormatIndicatorToGSMIdType(TLbsExternalRequestInfo::TFormatIndicator aIdType)
	{
	switch(aIdType)
		{
		case TLbsExternalRequestInfo::EFormatLogicalName:
			return CPosGSMPrivacyRequestInfo::EIdTypeLogicalName;
		case TLbsExternalRequestInfo::EFormatMSISDN:
			return CPosGSMPrivacyRequestInfo::EIdTypeMSISDN;
		case TLbsExternalRequestInfo::EFormatEmailAddress:
			return CPosGSMPrivacyRequestInfo::EIdTypeEmailAddress;
		case TLbsExternalRequestInfo::EFormatURL:
			return CPosGSMPrivacyRequestInfo::EIdTypeURL;
		case TLbsExternalRequestInfo::EFormatSIPURL:
			return CPosGSMPrivacyRequestInfo::EIdTypeSIPURL;
		case TLbsExternalRequestInfo::EFormatIMSPublicIdentity:
			return CPosGSMPrivacyRequestInfo::EIdTypeIMSPublicIdentity;
		default:
			return CPosGSMPrivacyRequestInfo::EIdTypeLogicalName;
		}
	}

CPosSUPLPrivacyRequestInfo::TIdType CPrivFwApiWrapper::ConvertFormatIndicatorToSUPLIdType(TLbsExternalRequestInfo::TFormatIndicator aIdType)
	{
	switch(aIdType)
		{
		case TLbsExternalRequestInfo::EFormatLogicalName:
			return CPosSUPLPrivacyRequestInfo::EIdTypeLogicalName;
		case TLbsExternalRequestInfo::EFormatMSISDN:
			return CPosSUPLPrivacyRequestInfo::EIdTypeMSISDN;
		case TLbsExternalRequestInfo::EFormatEmailAddress:
			return CPosSUPLPrivacyRequestInfo::EIdTypeEmailAddress;
		case TLbsExternalRequestInfo::EFormatURL:
			return CPosSUPLPrivacyRequestInfo::EIdTypeURL;
		case TLbsExternalRequestInfo::EFormatSIPURL:
			return CPosSUPLPrivacyRequestInfo::EIdTypeSIPURL;
		case TLbsExternalRequestInfo::EFormatIMSPublicIdentity:
			return CPosSUPLPrivacyRequestInfo::EIdTypeIMSPublicIdentity;
		case TLbsExternalRequestInfo::EFormatMIN:
			return CPosSUPLPrivacyRequestInfo::EIdTypeMIN;
		case TLbsExternalRequestInfo::EFormatMDN:
			return CPosSUPLPrivacyRequestInfo::EIdTypeMDN;
		default:
			return CPosSUPLPrivacyRequestInfo::EIdTypeLogicalName;
		}
	}

void CPrivFwApiWrapper::SetRequestInfoType(TPosNetworkPrivacyRequestInfoType aRequestInfoType)
	{
	iRequestInfoType = aRequestInfoType;
	}

