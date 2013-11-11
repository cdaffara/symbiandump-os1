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
// The TEFUnit header file which tests the PacketContext
// functional unit of the Common TSY.
// 
//

#ifndef CCTSYPACKETCONTEXTFU_H
#define CCTSYPACKETCONTEXTFU_H

#include <test/tefunit.h>

#include <etelmm.h>
#include <etelmmcs.h>

#include "cctsycomponenttestbase.h"


class CCTsyPacketContextFU : public CCtsyComponentTestBase
	{
public:
	// Create a suite of all the tests
	static CTestSuite* CreateSuiteL(const TDesC& aName);

public:
	// Individual test steps

	void TestNotifyStatusChange0001L();
	void TestNotifyStatusChange0002L();
	void TestNotifyStatusChange0003L();
	void TestNotifyStatusChange0004L();
	void TestNotifyStatusChange0005L();
	void TestDeactivate0001L();
	void TestDeactivate0002L();
	void TestDeactivate0003L();
	void TestDeactivate0004L();
	void TestDeactivate0005L();
	void TestGetDnsInfo0001L();
	void TestGetDnsInfo0002L();
	void TestGetDnsInfo0003L();
	void TestGetDnsInfo0004L();
	void TestGetDnsInfo0005L();
	void TestLoanCommPort0001L();
	void TestLoanCommPort0002L();
	void TestLoanCommPort0003L();
	void TestLoanCommPort0004L();
	void TestLoanCommPort0005L();
	void TestGetProfileName0001L();
	void TestGetProfileName0002L();
	void TestGetProfileName0003L();
	void TestGetProfileName0004L();
	void TestGetProfileName0005L();
	void TestRecoverCommPort0001L();
	void TestRecoverCommPort0002L();
	void TestRecoverCommPort0003L();
	void TestRecoverCommPort0004L();
	void TestRecoverCommPort0005L();
	void TestGetLastErrorCause0001L();
	void TestGetLastErrorCause0002L();
	void TestGetLastErrorCause0003L();
	void TestGetLastErrorCause0004L();
	void TestGetLastErrorCause0005L();
	void TestInitialiseContext0001L();
	void TestInitialiseContext0002L();
	void TestInitialiseContext0003L();
	void TestInitialiseContext0004L();
	void TestInitialiseContext0005L();
	void TestGetConnectionSpeed0001L();
	void TestGetConnectionSpeed0002L();
	void TestGetConnectionSpeed0003L();
	void TestGetConnectionSpeed0004L();
	void TestGetConnectionSpeed0005L();
	void TestModifyActiveContext0001L();
	void TestModifyActiveContext0002L();
	void TestModifyActiveContext0003L();
	void TestModifyActiveContext0004L();
	void TestModifyActiveContext0005L();
	void TestNotifyConfigChanged0001L();
	void TestNotifyConfigChanged0001bL();
	void TestNotifyConfigChanged0001cL();
	void TestNotifyConfigChanged0002L();
	void TestNotifyConfigChanged0003L();
	void TestNotifyConfigChanged0004L();
	void TestNotifyConfigChanged0005L();
	void TestNotifyDataTransferred0001L();
	void TestNotifyDataTransferred0002L();
	void TestNotifyDataTransferred0003L();
	void TestNotifyDataTransferred0004L();
	void TestNotifyDataTransferred0005L();
	void TestEnumeratePacketFilters0001L();
	void TestEnumeratePacketFilters0002L();
	void TestEnumeratePacketFilters0003L();
	void TestEnumeratePacketFilters0004L();
	void TestEnumeratePacketFilters0005L();
	void TestGetDataVolumeTransferred0001L();
	void TestGetDataVolumeTransferred0002L();
	void TestGetDataVolumeTransferred0003L();
	void TestGetDataVolumeTransferred0004L();
	void TestGetDataVolumeTransferred0005L();
	void TestNotifyConnectionSpeedChange0001L();
	void TestNotifyConnectionSpeedChange0002L();
//	void TestNotifyConnectionSpeedChange0003L();
	void TestNotifyConnectionSpeedChange0004L();
//	void TestNotifyConnectionSpeedChange0005L();
	void TestDelete0001L();
	void TestDelete0002L();
	void TestDelete0003L();
	void TestDelete0004L();
	void TestDelete0005L();
	void TestActivate0001L();
	void TestActivate0001bL();
	void TestActivate0002L();
	void TestActivate0003L();
	void TestActivate0004L();
	void TestActivate0005L();
	void TestGetConfig0001L();
//	void TestGetConfig0002L();
	void TestGetConfig0003L();
	void TestGetConfig0004L();
//	void TestGetConfig0005L();
	void TestGetStatus0001L();
	//void TestGetStatus0002L();
	//void TestGetStatus0003L();
	//void TestGetStatus0004L();
	//void TestGetStatus0005L();
	void TestSetConfig0001L();
	void TestSetConfig0002L();
	void TestSetConfig0003L();
	void TestSetConfig0004L();
	void TestSetConfig0005L();
//	void TestNotifyConnectionSpeedChange0003();
//	void TestNotifyConnectionSpeedChange0005();
//	void TestGetConfig0002();
//	void TestGetConfig0005();
	//void TestGetStatus0002();
	//void TestGetStatus0003();
	//void TestGetStatus0004();
	//void TestGetStatus0005();	
#ifdef SYMBIAN_ADAPTIVE_TCP_RECEIVE_WINDOW
	void TestGetConnectionInfo0001L();
	void TestGetConnectionInfo0002L();
	void TestGetConnectionInfo0004L();
	void TestNotifyConnectionInfoChange0001L();
	void TestNotifyConnectionInfoChange0001_workerL(TBool aChangeValid,TBool aChangeHSDPACategory,TBool aChangeHSUPACategory,TBool aResendFirst=EFalse);
	void TestNotifyConnectionInfoChange0002L();
	void TestNotifyConnectionInfoChange0004L();		
#endif // SYMBIAN_ADAPTIVE_TCP_RECEIVE_WINDOW
	void TestOpenNewSecondaryContext0001L(); 
	void TestOpenNewSecondaryContext0001bL();
    void TestOpenNewSecondaryContext0003L();     
    void TestOpenNewSecondaryContext0003bL();
    void TestOpenNewSecondaryContext0003cL();
   
    void TestOpenNewContext0001L();   
    void TestOpenNewContext0003L();     
    void TestOpenExistingContext0001L();    
    void TestOpenExistingContext0003L();
   
	
	//void TestAddPacketFilter0001L();


private:
	static void CleanupArrayPtrFlat(TAny* self);

	}; // class CCTsyPacketContextFU

#endif // CCTSYPACKETCONTEXTFU_H

