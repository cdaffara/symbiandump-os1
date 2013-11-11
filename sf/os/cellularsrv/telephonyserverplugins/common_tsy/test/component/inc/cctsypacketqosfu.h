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
// The TEFUnit header file which tests the PacketQoS
// functional unit of the Common TSY.
// 
//

#ifndef CCTSYPACKETQOSFU_H
#define CCTSYPACKETQOSFU_H

#include <test/tefunit.h>

#include <etelmm.h>
#include <etelmmcs.h>

#include "cctsycomponenttestbase.h"

class CCTsyPacketQoSFU : public CCtsyComponentTestBase
	{
public:
	// Create a suite of all the tests
	static CTestSuite* CreateSuiteL(const TDesC& aName);

public:
	// Individual test steps

	void TestAddMediaAuthorizationL0001L();
	void TestAddMediaAuthorizationL0002L();
	void TestAddMediaAuthorizationL0004L();
	void TestAddMediaAuthorizationL0005L();
	void TestRemoveMediaAuthorization0001L();
	void TestRemoveMediaAuthorization0002L();
	void TestRemoveMediaAuthorization0003L();
	void TestRemoveMediaAuthorization0004L();
	void TestRemoveMediaAuthorization0005L();
	void TestGetProfileParameters0001L();
	void TestGetProfileParameters0003L();
	void TestGetProfileParameters0004L();
	void TestNotifyProfileChanged0001L();
	void TestNotifyProfileChanged0002L();
	void TestNotifyProfileChanged0003L();
	void TestNotifyProfileChanged0004L();
	void TestSetProfileParameters0001L();
	void TestSetProfileParameters0002L();
	void TestSetProfileParameters0003L();
	void TestSetProfileParameters0004L();
	void TestSetProfileParameters0005L();
	void TestGetProfileCapabilities0001L();
	void TestGetProfileCapabilities0003L();
	void TestGetProfileCapabilities0004L();
	void TestGetProfileCapabilities0005L();
	void TestCreateNewTFT0001L();
	void TestAddPacketFilter0001L();
	void TestAddPacketFilter0002L();
	void TestAddPacketFilter0003L();
	void TestAddPacketFilter0004L();
	void TestRemovePacketFilter0001L();
	void TestRemovePacketFilter0002L();
	void TestRemovePacketFilter0003L();
	void TestRemovePacketFilter0004L();
	void TestRemovePacketFilter0005L();
	void TestGetPacketFilterInfo0001L();
	void TestGetPacketFilterInfo0002L();
	void TestGetPacketFilterInfo0003L();
	void TestGetPacketFilterInfo0004L();
	void TestGetPacketFilterInfo0005L();
	void TestDeleteTFT0001L();
	void TestOpenNewQoS0001L();
	void TestOpenNewQoS0003L();
	void TestOpenExistingQoS0001L();
	void TestOpenExistingQoS0003L();
	void TestOpenExistingQoS0004L();

private:

	template< class T >
	void ChangeProfileDataL(RPacketContext& aPacketContext, RPacketQoS& aPacketQoS,
	                       TInfoName& aContextName, RBuf8& aData);

	}; // class CCTsyPacketQoSFU

#endif // CCTSYPACKETQOSFU_H

