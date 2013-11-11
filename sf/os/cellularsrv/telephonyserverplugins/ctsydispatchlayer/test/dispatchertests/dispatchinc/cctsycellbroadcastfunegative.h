// Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
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

/**
 @file
 @internalAll 
*/

#ifndef CTSYCELLBROADCASTUNEGATIVE_H_
#define CTSYCELLBROADCASTUNEGATIVE_H_

#include "cctsycomponenttestbase.h"

class CCTsyCellBroadcastFUNegative : public CCtsyComponentTestBase
	{
public:
	// Create a suite of all the tests
	static CTestSuite* CreateSuiteL(const TDesC& aName);

public:
	void TestMessagingReceiveMessageL();
	void TestMessagingReceiveMessageCancelL();
	void TestSetBroadcastFilterSettingL();
	void TestStartSimCbTopicBrowsingL();
	void TestDeleteSimCbTopicL();
	}; // class CCTsyCellBroadcastFUNegative


#endif /* CTSYCELLBROADCASTUNEGATIVE_H_ */
