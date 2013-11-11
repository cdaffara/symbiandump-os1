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
// The TEFUnit header file which tests the NetworkControl
// functional unit of the Common TSY.
// 
//

#ifndef CCTSYNETWORKCONTROLFU_H
#define CCTSYNETWORKCONTROLFU_H

#include <test/tefunit.h>

#include <etelmm.h>
#include <etelmmcs.h>

#include "cctsycomponenttestbase.h"

class CCTsyNetworkControlFU : public CCtsyComponentTestBase
	{
public:
	// Create a suite of all the tests
	static CTestSuite* CreateSuiteL(const TDesC& aName);

public:
	// Individual test steps

	void TestStartV20001L();
	void TestStartV20002L();
	void TestStartV20004L();
	void TestStartV20005L();
	void TestStart0001L();
	void TestGetNITZInfo0001L();
	void TestGetNITZInfo0003L();
	void TestSelectNetwork0001L();
	void TestSelectNetwork0002L();
	void TestSelectNetwork0004L();
	void TestSelectNetwork0005L();
	void TestGetCurrentMode0001L();
	void TestGetHomeNetwork0001L();
	void TestGetHomeNetwork0001aL();
	void TestGetHomeNetwork0001bL();
	void TestGetHomeNetwork0002L();
	void TestGetHomeNetwork0003L();
	void TestGetHomeNetwork0004L();
	void TestGetHomeNetwork0004bL();
	void TestGetNetworkCaps0001L();
	void TestNotifyModeChange0001L();
	void TestNotifyModeChange0002L();
	void TestNotifyModeChange0004L();
	void TestGetCurrentNetwork0001L();
	void TestGetCurrentNetwork0001bL();
	void TestGetCurrentNetwork0002L();
	void TestGetCurrentNetwork0003L();
	void TestGetCurrentNetwork0004L();
	void TestGetCurrentNetwork0004bL();
	void TestNotifyNITZInfoChange0001L();
	void TestNotifyNITZInfoChange0002L();
	void TestNotifyNITZInfoChange0004L();
	void TestGetCurrentNetworkName0001L();
	void TestGetCurrentNetworkName0003L();
	void TestGetCurrentNetworkName0004L();
	void TestGetServiceProviderName0001L();
	void TestGetServiceProviderName0001aL();
	void TestGetServiceProviderName0002L();
	void TestGetServiceProviderName0002bL();
	void TestGetServiceProviderName0003L();
	void TestGetServiceProviderName0004L();
	void TestGetServiceProviderName0005L();
	void TestGetNetworkInvScanSetting0001L();
	void TestNotifyNetworkInvScanEvent0001L();
	void TestGetHomeNetworkSearchPeriod0001L();
	void TestGetNetworkSelectionSetting0001L();
	void TestGetNetworkSelectionSetting0003L();
	void TestNotifyCurrentNetworkChange0001L();
	void TestNotifyCurrentNetworkChange0001bL();
	void TestNotifyCurrentNetworkChange0002L();
	void TestNotifyCurrentNetworkChange0002bL();
	void TestNotifyCurrentNetworkChange0003L();
	void TestNotifyCurrentNetworkChange0004L();
	void TestNotifyCurrentNetworkChange0004bL();
	void TestGetCellInfo0001L();
	void TestGetCellInfo0002L();
	void TestGetCellInfo0003L();
	void TestGetCellInfo0004L();
	void TestNotifyCellInfoChange0001L();
	void TestNotifyCellInfoChange0002L();
	void TestNotifyCellInfoChange0003L();
	void TestNotifyCellInfoChange0004L();
	void TestNotifyNetworkInvScanChange0001L();
	void TestSetNetworkSelectionSetting0001L();
	void TestSetNetworkSelectionSetting0002L();
	void TestSetNetworkSelectionSetting0003L();
	void TestSetNetworkSelectionSetting0004L();
	void TestSetNetworkSelectionSetting0005L();
	void TestGetLastUsedAccessTechnology0001L();
	void TestStorePreferredNetworksListL0001L();
	void TestGetNetworkRegistrationStatus0001L();
	void TestGetNetworkRegistrationStatus0002L();
	void TestGetNetworkRegistrationStatus0004L();
	void TestGetNetworkRegistrationStatus0005L();
	void TestNotifyPreferredNetworksListChange0001L();
	void TestNotifyNetworkSelectionSettingChange0001L();
	void TestNotifyNetworkSelectionSettingChange0002L();
	void TestNotifyNetworkSelectionSettingChange0004L();
	void TestNotifyNetworkRegistrationStatusChange0001L();
	void TestNotifyNetworkRegistrationStatusChange0002L();
	void TestNotifyNetworkRegistrationStatusChange0004L();
	void TestStartV50001L();


private:


	}; // class CCTsyNetworkControlFU

#endif // CCTSYNETWORKCONTROLFU_H

