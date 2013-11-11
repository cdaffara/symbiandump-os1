 //lbsprivacyrequestimpl.cpp

// Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#include    "lbsprivacyrequestimpl.h"
#include    "lbsprivacyrequestresponsehandler.h"
#include <lbs/lbsnetcommon.h>


/*
* Default constructor.
*/
CLbsPrivacyRequestImpl::CLbsPrivacyRequestImpl(MLbsPrivacyRequestObserver& aObserver)
   :iObserver(aObserver)
    {
        
    }

/**
 * Standard ConstructL
 */
void CLbsPrivacyRequestImpl::ConstructL()
    {
	User::LeaveIfError(iServer.Connect(ELbsProxyApiTypeLocal));
    }

/**
 * Standard Symbian OS two-phase constructor.
 * @return A new instance of this class.
 */
CLbsPrivacyRequestImpl* CLbsPrivacyRequestImpl::NewL(MLbsPrivacyRequestObserver& aObserver)
    {
    CLbsPrivacyRequestImpl* self = new (ELeave) CLbsPrivacyRequestImpl(aObserver);
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop(self);
    return self;
    }

/**
* Destructor
*/
CLbsPrivacyRequestImpl::~CLbsPrivacyRequestImpl()
    {
    iResponseHandler.ResetAndDestroy();
    iServer.Close();
	}

TInt CLbsPrivacyRequestImpl::NewPrivacyRequest(TLbsPrivacyRequestId& aRequestId, /*out*/
		 const TLbsPrivacyRequest& aPrivacy,/*in*/
		 const TLbsExternalRequestInfo& aRequestInfo/*in*/)
	{
	TInt validation = ValidateNewPrivReqArguments(aPrivacy, aRequestInfo);
	if(KErrNone != validation)
		{
		return validation;
		}

	TLbsNetPosRequestPrivacy netPosRequestPrivacy;
	netPosRequestPrivacy.SetRequestAction(static_cast<TLbsNetPosRequestPrivacy::TLbsRequestAction>(aPrivacy.RequestAction()));
	netPosRequestPrivacy.SetRequestAdvice(static_cast<TLbsNetPosRequestPrivacy::TLbsRequestAdvice>(aPrivacy.RequestAdvice()));

	CPrivacyRequestResponseHandler* newResponseHandler = NULL;
	TRAPD(err, newResponseHandler = CPrivacyRequestResponseHandler::NewLC(*this, iServer, iObserver);
                iResponseHandler.AppendL(newResponseHandler);
                CleanupStack::Pop(newResponseHandler);
          );
	newResponseHandler->NewPrivacyRequest(aRequestId, aRequestInfo, netPosRequestPrivacy);
	return err;
	}

TInt CLbsPrivacyRequestImpl::RepeatPrivacyRequest(const TLbsPrivacyRequestId aRequestId, /*in*/
		 const TLbsPrivacyRequest& aPrivacy,/*in*/
		 const TLbsExternalRequestInfo& aRequestInfo/*in*/)
	{
	TInt validation = ValidateRepeatPrivReqArguments(aRequestId, aPrivacy, aRequestInfo);
	if(KErrNone != validation)
		{
		return validation;
		}
	
	TLbsNetPosRequestPrivacy netPosRequestPrivacy;
	netPosRequestPrivacy.SetRequestAction(static_cast<TLbsNetPosRequestPrivacy::TLbsRequestAction>(aPrivacy.RequestAction()));
	netPosRequestPrivacy.SetRequestAdvice(static_cast<TLbsNetPosRequestPrivacy::TLbsRequestAdvice>(aPrivacy.RequestAdvice()));

	CPrivacyRequestResponseHandler* newResponseHandler = NULL;
	TRAPD(err, newResponseHandler = CPrivacyRequestResponseHandler::NewLC(*this, iServer, iObserver);
                iResponseHandler.AppendL(newResponseHandler);
                CleanupStack::Pop(newResponseHandler);
         );
	newResponseHandler->RepeatPrivacyRequest(aRequestId, aRequestInfo, netPosRequestPrivacy);

	return err;
	}

TInt CLbsPrivacyRequestImpl::CompleteRequest(TLbsPrivacyRequestId aRequestId/*in*/,
		  TInt aReason/*in*/)
	{
	return iServer.CompleteRequest(aRequestId, aReason);
	}

TInt CLbsPrivacyRequestImpl::ValidateNewPrivReqArguments(const TLbsPrivacyRequest& aPrivacy, const TLbsExternalRequestInfo& aRequestInfo)
	{
	TLbsPrivacyRequest::TLbsRequestAdvice requestAdvice(aPrivacy.RequestAdvice());
	switch(requestAdvice)
		{
		case TLbsPrivacyRequest::ERequestAdviceNotify:
		case TLbsPrivacyRequest::ERequestAdviceVerify:
		case TLbsPrivacyRequest::ERequestAdviceSilent:
			break; // do nothing for good value(s)
		case TLbsPrivacyRequest::ERequestAdviceNotUsed:
			{
			return KErrArgument;
			}
		case TLbsPrivacyRequest::ERequestAdviceStealth:

			{
			return KErrNotSupported;
			}
		default:
			{
			return KErrArgument;
			}
		}

	TLbsPrivacyRequest::TLbsRequestAction requestAction(aPrivacy.RequestAction());
	switch(requestAction)
		{
		case TLbsPrivacyRequest::ERequestActionAllow:
		case TLbsPrivacyRequest::ERequestActionReject:
			break; // do nothing for good value(s)
		case TLbsPrivacyRequest::ERequestActionNotUsed:
			{
			return KErrArgument;
			}
		default:
			{
			return KErrArgument;
			}
		}

	TLbsExternalRequestInfo::TRequestSource requestSource(aRequestInfo.RequestSource());
	TLbsExternalRequestInfo::TRequestType requestType(aRequestInfo.RequestType());
	TLbsExternalRequestInfo::TNetworkType networkType(aRequestInfo.NetworkType());
	TLbsExternalRequestInfo::TFormatIndicator requesterIdFormatIndicator(aRequestInfo.RequesterIdFormat());
	TLbsExternalRequestInfo::TFormatIndicator clientFormatIndicator(aRequestInfo.ClientNameFormat());

	switch(requestSource)
		{
		case TLbsExternalRequestInfo::ERequestSourceLocal:
			{
			switch(requestType)
				{
				case TLbsExternalRequestInfo::ERequestSingleShot:
				case TLbsExternalRequestInfo::ERequestPeriodic:
				case TLbsExternalRequestInfo::ERequestSingleShotSilent:
				case TLbsExternalRequestInfo::ERequestPeriodicSilent:
					break; // do nothing for good value(s)
				case TLbsExternalRequestInfo::ERequestAreaEvent:
				case TLbsExternalRequestInfo::ERequestAreaEventSilent:
					{
					return KErrNotSupported;
					}
				case TLbsExternalRequestInfo::ERequestTypeUnknown:
					{
					return KErrArgument;
					}
				default:
					{
					return KErrArgument;
					}
				}

			switch(networkType)
				{
				case TLbsExternalRequestInfo::ENetworkTypeUnknown:
					break; // do nothing for good value(s)
				case TLbsExternalRequestInfo::ENetworkGSM:
				case TLbsExternalRequestInfo::ENetworkSUPL:
				case TLbsExternalRequestInfo::ENetworkWCDMA:
					{
					return KErrArgument;
					}
				default:
					{
					return KErrArgument;
					}
				}

			// Return KErrArgument if both formatIndicators are EFormatUnknown
			if((requesterIdFormatIndicator == TLbsExternalRequestInfo::EFormatUnknown)&&
					(clientFormatIndicator == TLbsExternalRequestInfo::EFormatUnknown))
				{
				return KErrArgument;
				}
			if(requesterIdFormatIndicator != TLbsExternalRequestInfo::EFormatUnknown)
				{
				switch(requesterIdFormatIndicator)
					{
					case TLbsExternalRequestInfo::EFormatLogicalName:
					case TLbsExternalRequestInfo::EFormatEmailAddress:
					case TLbsExternalRequestInfo::EFormatMSISDN:
					case TLbsExternalRequestInfo::EFormatURL:
					case TLbsExternalRequestInfo::EFormatSIPURL	:
					case TLbsExternalRequestInfo::EFormatIMSPublicIdentity:
					case TLbsExternalRequestInfo::EFormatMIN:
					case TLbsExternalRequestInfo::EFormatMDN:
					case TLbsExternalRequestInfo::EFormatAppUID:
						break; // do nothing for good value(s)
					default:
						{
						return KErrArgument;
						}
					}
				}
			if(clientFormatIndicator != TLbsExternalRequestInfo::EFormatUnknown)
				{
				switch(clientFormatIndicator)
					{
					case TLbsExternalRequestInfo::EFormatLogicalName:
					case TLbsExternalRequestInfo::EFormatEmailAddress:
					case TLbsExternalRequestInfo::EFormatMSISDN:
					case TLbsExternalRequestInfo::EFormatURL:
					case TLbsExternalRequestInfo::EFormatSIPURL	:
					case TLbsExternalRequestInfo::EFormatIMSPublicIdentity:
					case TLbsExternalRequestInfo::EFormatMIN:
					case TLbsExternalRequestInfo::EFormatMDN:
					case TLbsExternalRequestInfo::EFormatAppUID:
						break; // do nothing for good value(s)
					default:
						{
						return KErrArgument;
						}
					}
				}
			break;
			}
		
		case TLbsExternalRequestInfo::ERequestSourceNetwork:
			{
			switch(requestType)
				{
				case TLbsExternalRequestInfo::ERequestSingleShot:
				case TLbsExternalRequestInfo::ERequestPeriodic:
				case TLbsExternalRequestInfo::ERequestAreaEvent:
                case TLbsExternalRequestInfo::ERequestSingleShotSilent:
                case TLbsExternalRequestInfo::ERequestPeriodicSilent:
                case TLbsExternalRequestInfo::ERequestAreaEventSilent:
					break; // do nothing for good value(s)
				case TLbsExternalRequestInfo::ERequestTypeUnknown:
					{
					return KErrArgument;
					}
				default:
					{
					return KErrArgument;
					}
				}

			switch(networkType)
				{
				case TLbsExternalRequestInfo::ENetworkGSM:
				case TLbsExternalRequestInfo::ENetworkSUPL:
				case TLbsExternalRequestInfo::ENetworkWCDMA:
					break; // do nothing for good value(s)
				case TLbsExternalRequestInfo::ENetworkTypeUnknown:
					{
					return KErrArgument;
					}
				default:
					{
					return KErrArgument;
					}
				}
			// Return KErrArgument if both formatIndicators are EFormatUnknown
			if((requesterIdFormatIndicator == TLbsExternalRequestInfo::EFormatUnknown)&&
					(clientFormatIndicator == TLbsExternalRequestInfo::EFormatUnknown))
				{
				return KErrArgument;
				}
			if(requesterIdFormatIndicator != TLbsExternalRequestInfo::EFormatUnknown)
				{
				switch(requesterIdFormatIndicator)
					{
					case TLbsExternalRequestInfo::EFormatLogicalName:
					case TLbsExternalRequestInfo::EFormatEmailAddress:
					case TLbsExternalRequestInfo::EFormatMSISDN:
					case TLbsExternalRequestInfo::EFormatURL:
					case TLbsExternalRequestInfo::EFormatSIPURL	:
					case TLbsExternalRequestInfo::EFormatIMSPublicIdentity:
						break; // do nothing for good value(s)
					case TLbsExternalRequestInfo::EFormatMIN:
					case TLbsExternalRequestInfo::EFormatMDN:
						{
						if (networkType != TLbsExternalRequestInfo::ENetworkSUPL)
							{
							return KErrArgument;
							}
						}
						break;
					case TLbsExternalRequestInfo::EFormatAppUID:
						{
						return KErrArgument;
						}
					default:
						{
						return KErrArgument;
						}
					}
				}
			if(clientFormatIndicator != TLbsExternalRequestInfo::EFormatUnknown)
				{
				switch(clientFormatIndicator)
					{
					case TLbsExternalRequestInfo::EFormatLogicalName:
					case TLbsExternalRequestInfo::EFormatEmailAddress:
					case TLbsExternalRequestInfo::EFormatMSISDN:
					case TLbsExternalRequestInfo::EFormatURL:
					case TLbsExternalRequestInfo::EFormatSIPURL	:
					case TLbsExternalRequestInfo::EFormatIMSPublicIdentity:
						break; // do nothing for good value(s)
					case TLbsExternalRequestInfo::EFormatMIN:
					case TLbsExternalRequestInfo::EFormatMDN:
						{
						if (networkType != TLbsExternalRequestInfo::ENetworkSUPL)
							{
							return KErrArgument;
							}
						}
						break;
					case TLbsExternalRequestInfo::EFormatAppUID:
						{
						return KErrArgument;
						}
					default:
						{
						return KErrArgument;
						}
					}
				}
			break;
			}

		case TLbsExternalRequestInfo::ERequestSourceUnknown:
			{
			return KErrArgument;
			}
		default:
			{
			return KErrArgument;
			}
		}
	return KErrNone;
	}

TInt CLbsPrivacyRequestImpl::ValidateRepeatPrivReqArguments(const TLbsPrivacyRequestId aRequestId, const TLbsPrivacyRequest& aPrivacy, const TLbsExternalRequestInfo& aRequestInfo)
	{
	if((aRequestId == 0) || (aRequestId > KMaxTInt))
		{
		return KErrArgument;
		}
	
	TLbsPrivacyRequest::TLbsRequestAdvice requestAdvice(aPrivacy.RequestAdvice());
	switch(requestAdvice)
		{
		case TLbsPrivacyRequest::ERequestAdviceNotify:
			break; // do nothing for good value(s)
		case TLbsPrivacyRequest::ERequestAdviceVerify:
		case TLbsPrivacyRequest::ERequestAdviceSilent:
		case TLbsPrivacyRequest::ERequestAdviceNotUsed:
			{
			return KErrArgument;
			}
		case TLbsPrivacyRequest::ERequestAdviceStealth:
			{
			return KErrNotSupported;
			}
		default:
			{
			return KErrArgument;
			}
		}

	TLbsPrivacyRequest::TLbsRequestAction requestAction(aPrivacy.RequestAction());
	switch(requestAction)
		{
		case TLbsPrivacyRequest::ERequestActionAllow:
		case TLbsPrivacyRequest::ERequestActionReject:
			break; // do nothing for good value(s)
		case TLbsPrivacyRequest::ERequestActionNotUsed:
			{
			return KErrArgument;
			}
		default:
			{
			return KErrArgument;
			}
		}

	TLbsExternalRequestInfo::TRequestSource requestSource(aRequestInfo.RequestSource());
	TLbsExternalRequestInfo::TRequestType requestType(aRequestInfo.RequestType());
	TLbsExternalRequestInfo::TNetworkType networkType(aRequestInfo.NetworkType());
	TLbsExternalRequestInfo::TFormatIndicator requesterIdFormatIndicator(aRequestInfo.RequesterIdFormat());
	TLbsExternalRequestInfo::TFormatIndicator clientFormatIndicator(aRequestInfo.ClientNameFormat());

	switch(requestSource)
		{
		case TLbsExternalRequestInfo::ERequestSourceNetwork:
			{
			switch(requestType)
				{
				case TLbsExternalRequestInfo::ERequestSingleShot:
				case TLbsExternalRequestInfo::ERequestPeriodic:
				case TLbsExternalRequestInfo::ERequestAreaEvent:
                case TLbsExternalRequestInfo::ERequestSingleShotSilent:
                case TLbsExternalRequestInfo::ERequestPeriodicSilent:
                case TLbsExternalRequestInfo::ERequestAreaEventSilent:
					break; // do nothing for good value(s)
				case TLbsExternalRequestInfo::ERequestTypeUnknown:
					{
					return KErrArgument;
					}
				default:
					{
					return KErrArgument;
					}
				}

			switch(networkType)
				{
				case TLbsExternalRequestInfo::ENetworkGSM:
				case TLbsExternalRequestInfo::ENetworkSUPL:
				case TLbsExternalRequestInfo::ENetworkWCDMA:
					break; // do nothing for good value(s)
				case TLbsExternalRequestInfo::ENetworkTypeUnknown:
					{
					return KErrArgument;
					}
				default:
					{
					return KErrArgument;
					}
				}
			// Return KErrArgument if both formatIndicators are EFormatUnknown
			if((requesterIdFormatIndicator == TLbsExternalRequestInfo::EFormatUnknown)&&
					(clientFormatIndicator == TLbsExternalRequestInfo::EFormatUnknown))
				{
				return KErrArgument;
				}
			if(requesterIdFormatIndicator != TLbsExternalRequestInfo::EFormatUnknown)
				{
				switch(requesterIdFormatIndicator)
					{
					case TLbsExternalRequestInfo::EFormatLogicalName:
					case TLbsExternalRequestInfo::EFormatEmailAddress:
					case TLbsExternalRequestInfo::EFormatMSISDN:
					case TLbsExternalRequestInfo::EFormatURL:
					case TLbsExternalRequestInfo::EFormatSIPURL	:
					case TLbsExternalRequestInfo::EFormatIMSPublicIdentity:
						break; // do nothing for good value(s)
					case TLbsExternalRequestInfo::EFormatMIN:
					case TLbsExternalRequestInfo::EFormatMDN:
						{
						if (networkType != TLbsExternalRequestInfo::ENetworkSUPL)
							{
							return KErrArgument;
							}
						}
						break;
					case TLbsExternalRequestInfo::EFormatAppUID:
						{
						return KErrArgument;
						}
					default:
						{
						return KErrArgument;
						}
					}
				}
			if(clientFormatIndicator != TLbsExternalRequestInfo::EFormatUnknown)
				{
				switch(clientFormatIndicator)
					{
					case TLbsExternalRequestInfo::EFormatLogicalName:
					case TLbsExternalRequestInfo::EFormatEmailAddress:
					case TLbsExternalRequestInfo::EFormatMSISDN:
					case TLbsExternalRequestInfo::EFormatURL:
					case TLbsExternalRequestInfo::EFormatSIPURL	:
					case TLbsExternalRequestInfo::EFormatIMSPublicIdentity:
						break; // do nothing for good value(s)
					case TLbsExternalRequestInfo::EFormatMIN:
					case TLbsExternalRequestInfo::EFormatMDN:
						{
						if (networkType != TLbsExternalRequestInfo::ENetworkSUPL)
							{
							return KErrArgument;
							}
						}
						break;
					case TLbsExternalRequestInfo::EFormatAppUID:
						{
						return KErrArgument;
						}
					default:
						{
						return KErrArgument;
						}
					}
				}
			break;
			}

		case TLbsExternalRequestInfo::ERequestSourceLocal:
		case TLbsExternalRequestInfo::ERequestSourceUnknown:
			{
			return KErrArgument;
			}
		default:
			{
			return KErrArgument;
			}
		}
	return KErrNone;
	}

void CLbsPrivacyRequestImpl::RemoveHandler(CPrivacyRequestResponseHandler* aHandler)
	{
	for(TUint index = 0; index < iResponseHandler.Count();  ++index)
		{
		if(iResponseHandler[index] == aHandler)
			{
			iResponseHandler.Remove(index);
			break;
			}
		}
	}
