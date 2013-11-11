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
// The TEFUnit header file which tests the PacketService
// functional unit of the Common TSY.
// 
//

#ifndef CCTSYPACKETSERVICEFU_H
#define CCTSYPACKETSERVICEFU_H

#include <test/tefunit.h>

#include <etelmm.h>
#include <etelmmcs.h>

#include "pcktretrieve.h"

#include "cctsycomponenttestbase.h"

const TInt  KMaxSessionsInSet1 = 3;
const TInt  KMaxSessionsInSet2 = 5;
const TInt  KMaxSessionsInSet3 = 4;
const TInt  KMaxNumberMbmsContexts = 10;
const TInt  KZero = 0;

class CCTsyPacketServiceFU : public CCtsyComponentTestBase
	{
public:
	// Create a suite of all the tests
	static CTestSuite* CreateSuiteL(const TDesC& aName);

public:
	// Individual test steps

	void TestGetPreferredBearer0001L();
	void TestGetPreferredBearer0004L();	
	void TestNotifyContextAdded0001L();
	void TestNotifyContextAdded0002L();	
	void TestNotifyContextAdded0004L();	
	void TestNotifyStatusChange0001L();
	void TestNotifyStatusChange0002L();	
    void TestNotifyStatusChange0004L();
    void TestNotifyStatusChange0004aL();
	void TestNotifyStatusChange0001aL();
	void TestSetPreferredBearer0001L();
	void TestSetPreferredBearer0004L();
	void TestSetPreferredBearer0005L();
	void TestGetContextNameInNif0001L();
	void TestGetContextNameInNif0003L();
	void TestGetContextNameInNif0004L();
	void TestNotifyMSClassChange0001L();
	void TestNotifyMSClassChange0002L();
	void TestNotifyMSClassChange0004L();
	void TestGetCurrentReleaseMode0001L();
	void TestEnumerateContextsInNif0001L();
	void TestEnumerateContextsInNif0003L();
	void TestEnumerateContextsInNif0004L();
	void TestNotifyAttachModeChange0001L();
	void TestGetDefaultContextParams0001L();
	void TestGetDefaultContextParams0003L();
	void TestGetDefaultContextParams0004L();
	void TestNotifyDynamicCapsChange0001L();
	void TestNotifyDynamicCapsChange0001bL();
	void TestNotifyDynamicCapsChange0001cL();
	void TestNotifyDynamicCapsChange0002L();
	void TestNotifyDynamicCapsChange0004L();
	void TestNotifyReleaseModeChange0001L();
	void TestRejectActivationRequest0001L();
	void TestRejectActivationRequest0002L();
	void TestRejectActivationRequest0004L();
	void TestRejectActivationRequest0005L();
	void TestSetDefaultContextParams0001L();
	void TestSetDefaultContextParams0002L();
	void TestSetDefaultContextParams0003L();
	void TestSetDefaultContextParams0004L();
	void TestSetDefaultContextParams0005L();
	void TestNotifyChangeOfNtwkRegStatus0001L();
	void TestNotifyChangeOfNtwkRegStatus0002L();
	void TestNotifyChangeOfNtwkRegStatus0004L();
	void TestNotifyContextActivationRequested0001L();
	void TestNotifyContextActivationRequested0001bL();
	void TestNotifyContextActivationRequested0002L();
	void TestNotifyContextActivationRequested0003L();
	void TestNotifyContextActivationRequested0004L();
	void TestAttach0001L();
	void TestAttach0002L();
	void TestAttach0004L();
	void TestAttach0005L();
	void TestDetach0001L();
	void TestDetach0002L();
	void TestDetach0004L();
	void TestDetach0005L();
	void TestGetStatus0001L();
	void TestOpen0001L();
	void TestOpen0003L();
	void TestOpen0004L();
	void TestGetMSClass0001L();
	void TestGetMSClass0004L();
	void TestGetNifInfo0001L();
	void TestGetNifInfo0003L();
	void TestGetNifInfo0004L();
	void TestSetMSClass0001L();
	void TestDeactivateNIF0001L();
	void TestEnumerateNifs0001L();
	void TestEnumerateNifs0004L();
	void TestGetAttachMode0001L();
	void TestGetAttachMode0004L();
	void TestGetAttachMode0005L();
	void TestGetStaticCaps0001L();
	void TestGetStaticCaps0001bL();
	void TestGetStaticCaps0003L();
	void TestSetAttachMode0001L();
	void TestSetAttachMode0004L();
	void TestSetAttachMode0005L();
	void TestGetContextInfo0001L();
	void TestGetContextInfo0003L();
	void TestGetContextInfo0004L();
	void TestGetDynamicCaps0001L();
	void TestGetDynamicCaps0001bL();
	void TestGetNtwkRegStatus0001L();
	void TestGetNtwkRegStatus0004L();
	void TestGetNtwkRegStatus0005L();
	void TestEnumerateContexts0001L();
	void TestEnumerateContexts0004L();
	void TestNotifyContextAdded0002();	
	void TestNotifyContextAdded0004();	
	void TestNotifyStatusChange0002();	
	void TestNotifyDynamicCapsChange0001b();
	void TestNotifyContextActivationRequested0001b();
	
	// for testing internal CompleteNotifyEGprsInfoChange()
	void TestCompleteNotifyEGprsInfoChange0001L();
	// for testing internal CompleteSetAlwaysOn()
	void TestCompleteSetAlwaysOn0001L();
    void TestTsyMbmsSupportCaps0001L();
    void TestTsyMbmsSupportCaps0001bL();
    void TestNotifyMbmsNetworkServiceStatusChange0001L();
    void TestNotifyMbmsNetworkServiceStatusChange0002L();
    void TestNotifyMbmsNetworkServiceStatusChange0003L();
    void TestGetMbmsNetworkServiceStatus0001L();
    void TestGetMbmsNetworkServiceStatus0001bL();   
    void TestGetMbmsNetworkServiceStatus0001cL();     
    void TestGetMbmsNetworkServiceStatus0002L();
    void TestGetMbmsNetworkServiceStatus0003L();
    void TestGetMbmsNetworkServiceStatus0005L();
    void TestNotifyMbmsServiceAvailabilityChange0001L();
    void TestNotifyMbmsServiceAvailabilityChange0002L();
    void TestNotifyMbmsServiceAvailabilityChange0004L();
    void TestUpdateMbmsMonitorServiceList0001L();
    void TestUpdateMbmsMonitorServiceList0002L();
    void TestUpdateMbmsMonitorServiceList0003L();
    void TestUpdateMbmsMonitorServiceList0005L();
   	void TestStart0001L(); 
    void TestStart0002L(); 
    void TestEnumerateMbmsMonitorServiceList0001L();
    void TestEnumerateMbmsMonitorServiceList0002L();
    void TestEnumerateMbmsMonitorServiceList0004L();
    void TestEnumerateMbmsActiveServiceList0001L();
    void TestEnumerateMbmsActiveServiceList0002L();
    void TestUpdateMbmsSessionList0001L();
    void TestUpdateMbmsSessionList0002L();
    void TestUpdateMbmsSessionList0003L();
    void TestUpdateMbmsSessionList0005L();
    void TestRetrievePcktMbmsSessionListStart0001L();
    void TestRetrievePcktMbmsSessionListStart0002L();

private:
	
	TBool contains( TUint item, TUint* array);
	
    TBool contains( TUint item, const RArray<TUint> &array );
    
	}; // class CCTsyPacketServiceFU

#endif // CCTSYPACKETSERVICEFU_H

