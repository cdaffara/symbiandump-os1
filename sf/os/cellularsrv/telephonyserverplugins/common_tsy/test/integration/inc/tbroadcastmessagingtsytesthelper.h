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

#ifndef TBROADCASTMESSAGINGTSYTESTHELPER_H
#define TBROADCASTMESSAGINGTSYTESTHELPER_H


#include "ttsytesthelperbase.h"
	
class TBroadcastMessagingTsyTestHelper : public TTsyTestHelperBase
	{
public:
	TBroadcastMessagingTsyTestHelper(CCTSYIntegrationTestSuiteStepBase& aTestStep);

	void WaitForMobileBroadcastMessagingNotifyBroadcastIdListChange( 
				TEtelRequestBase& aRequestStatus,
				TInt aWantedStatus );

	void WaitForMobileBroadcastMessagingNotifyFilterSettingChange( 
				RMobileBroadcastMessaging& aMobileBroadcastMessaging,
				TEtelRequestBase& aRequestStatus,
				RMobileBroadcastMessaging::TMobilePhoneBroadcastFilter& aSetting, 
				RMobileBroadcastMessaging::TMobilePhoneBroadcastFilter aExpectedSetting,
				TInt aWantedStatus );

	void WaitForMobileBroadcastMessagingNotifyLanguageFilterChange( 
				RMobileBroadcastMessaging& aMobileBroadcastMessaging,
				TEtelRequestBase& aRequestStatus,
				TDes16& aLangFilter, 
				TDes16 aExpectedLangFilter,
				TInt aWantedStatus );

	}; // class TBroadcastMessagingTsyTestHelper 

#endif
