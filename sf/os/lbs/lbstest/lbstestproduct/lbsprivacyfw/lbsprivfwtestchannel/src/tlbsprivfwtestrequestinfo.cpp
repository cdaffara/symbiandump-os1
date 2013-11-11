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


#include <e32cmn.h>
#include <lbs/lbslocclasstypes.h>
#include <lbs/test/tlbsprivfwtestrequestinfo.h>



//------------------------------------------------------------------------------
// TLbsTestRequestInfo 
//------------------------------------------------------------------------------
/**
Constructor for TLbsTestRequestInfo.
@released
*/
EXPORT_C TLbsTestRequestInfo::TLbsTestRequestInfo()
	: TLbsExternalRequestInfo2()
	{
	iClassType |= EExternalRequestInfoFirstCustomClass; 
	iClassSize = sizeof(TLbsTestRequestInfo);
	
	iVerifyCancelReason = EPosCancelReasonNotAvailable;
	iRequestDecision = EPosDecisionNotAvailable;
	iTimeoutStrategy = EPosDecisionNotAvailable;
	iRequestSource = EPosRequestSourceNotAvailable;
	iNotificationReason = EPosNotificationReasonNotAvailable;
	iNotificationType = MLbsPrivacyObserver::ENotifyUnknown;
	}

/**  Get the verification cancel reason.
@return The cancel reason.
@released
*/
EXPORT_C TPosVerifyCancelReason TLbsTestRequestInfo::VerifyCancelReason() const
	{
	return iVerifyCancelReason;
	}

/**  Get the request decision.
@return The request decision.
@released
*/
EXPORT_C TPosRequestDecision TLbsTestRequestInfo::RequestDecision() const
	{
	return iRequestDecision;
	}

/**  Get the timeout strategy.
@return The timeout strategy.
@released
*/
EXPORT_C TPosRequestDecision TLbsTestRequestInfo::TimeoutStrategy() const
	{
	return iTimeoutStrategy;
	}

/**  Get the request source.
@return The request source.
@released
*/
EXPORT_C TPosRequestSource TLbsTestRequestInfo::RequestSource() const
	{
	return iRequestSource;
	}

/**  Get the notification reason.
@return The notification reason.
@released
*/
EXPORT_C TPosNotificationReason TLbsTestRequestInfo::NotificationReason() const
	{
	return iNotificationReason;
	}

/**  Get the notification yype.
@return The notification type.
@released
*/
EXPORT_C MLbsPrivacyObserver::TNotificationType TLbsTestRequestInfo::NotificationType() const
	{
	return iNotificationType;
	}


/**  Set the verification cancel reason.
@param aVerifyCancelReason The verification cancel reason.
@released
*/
EXPORT_C void TLbsTestRequestInfo::SetVerifyCancelReason(const TPosVerifyCancelReason aVerifyCancelReason)
	{
	iVerifyCancelReason = aVerifyCancelReason;
	}

/**  Set the request decision.
@param aRequestDecision The request decision.
@released
*/
EXPORT_C void TLbsTestRequestInfo::SetRequestDecision(const TPosRequestDecision aRequestDecision)
	{
	iRequestDecision = aRequestDecision;
	}

/**  Set the timeout strategy.
@param aTimeoutStrategy The timeout strategy.
@released
*/
EXPORT_C void TLbsTestRequestInfo::SetTimeoutStrategy(const TPosRequestDecision aTimeoutStrategy)
	{
	iTimeoutStrategy = aTimeoutStrategy;
	}

/**  Set the request source.
@param aRequestSource The request source.
@released
*/
EXPORT_C void TLbsTestRequestInfo::SetRequestSource(const TPosRequestSource aRequestSource)
	{
	iRequestSource = aRequestSource;
	}

/**  Set the notification reason.
@param aNotificationReason The notification reason.
@released
*/
EXPORT_C void TLbsTestRequestInfo::SetNotificationReason(const TPosNotificationReason aNotificationReason)
	{
	iNotificationReason = aNotificationReason;
	}

//void SetNotificationType(MLbsPrivacyObserver::TNotificationType aNotificationType);
/**  Set the notification type.
@param aNotificationType The notification type.
@released
*/
EXPORT_C void TLbsTestRequestInfo::SetNotificationType(const MLbsPrivacyObserver::TNotificationType aNotificationType)
	{
	iNotificationType = aNotificationType;
	}

