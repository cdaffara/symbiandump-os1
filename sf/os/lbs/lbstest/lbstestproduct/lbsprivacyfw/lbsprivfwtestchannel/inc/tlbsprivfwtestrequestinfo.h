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
// tlbsprivfwtestrequestinfo.cpp
// 
//

/**
 @file
 @internalTechnology
 @test
*/


#ifndef LBS_PRIVFW_TEST_REQUESTINFO_H
#define LBS_PRIVFW_TEST_REQUESTINFO_H

#include <lbscommon.h>
#include <lbssatellite.h>
#include <lbs/lbsnetclasstypes.h>
#include <lbs/lbsnetcommon.h>
#include <lbs/lbsnetprotocolbase.h>
#include <lbs/lbslocdatasourcegpsbase.h>
#include <lbs/epos_privacy.h>
#include <lbs/epos_cposrequestor.h>
#include <lbs/lbsprivacycontroller.h>

#include <lbs/test/lbsprivfwdefs.h>


class TLbsTestRequestInfo : public TLbsExternalRequestInfo2
	{
	public:
		IMPORT_C TLbsTestRequestInfo();
		
		//Getters
		IMPORT_C TPosVerifyCancelReason VerifyCancelReason() const;
		IMPORT_C TPosRequestDecision RequestDecision() const;
		IMPORT_C TPosRequestDecision TimeoutStrategy() const;
		IMPORT_C TPosRequestSource RequestSource() const;
		IMPORT_C TPosNotificationReason NotificationReason() const;
		IMPORT_C MLbsPrivacyObserver::TNotificationType NotificationType() const;
	
		//Setters
		IMPORT_C void SetVerifyCancelReason(const TPosVerifyCancelReason aVerifyCancelReason);
		IMPORT_C void SetRequestDecision(const TPosRequestDecision aRequestDecision);
		IMPORT_C void SetTimeoutStrategy(const TPosRequestDecision	aTimeoutStrategy);
		IMPORT_C void SetRequestSource(const TPosRequestSource aRequestSource);
		IMPORT_C void SetNotificationReason(const TPosNotificationReason aNotificationReason);
		IMPORT_C void SetNotificationType(const MLbsPrivacyObserver::TNotificationType aNotificationType);
		
	private:
		/** QnN Notifier only.*/
		TPosVerifyCancelReason iVerifyCancelReason;
		TPosRequestDecision iRequestDecision;
		TPosRequestDecision	iTimeoutStrategy;
		TPosRequestSource iRequestSource;
		TPosNotificationReason iNotificationReason;
		
		/** Privacy Controller only. */
		MLbsPrivacyObserver::TNotificationType iNotificationType;
	};

#endif // LBS_PRIVFW_TEST_REQUESTINFO_H
