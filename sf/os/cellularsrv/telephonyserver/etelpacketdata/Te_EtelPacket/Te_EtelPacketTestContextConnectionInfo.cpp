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
// This is the cpp file for Packet Data Context test Connection Information
// 
//

// Symbian OS includes
#include <e32base.h>
#include <etelpckt.h>
#include <pcktcs.h>

// Test includes
#include "Te_EtelPacketTestStepBase.h"
#include "testdef.h"
#include "Te_EtelPacketTestContextConnectionInfo.h"
#include "Te_etelpckt_defs.h"

// Constants
const TInt KThousand = 1000;

//Constructor
CEtelPacketTestContextConnectionInfo::CEtelPacketTestContextConnectionInfo()
    {
	//Store the name of this test case
	SetTestStepName(KContextConnectionInfo);
    }

//Destructor
CEtelPacketTestContextConnectionInfo::~CEtelPacketTestContextConnectionInfo()
    {
    //None
    }


enum TVerdict CEtelPacketTestContextConnectionInfo::doTestStepL( void )
/**
 * Test step tests getting context connection information
 */
    {
	RPhone phone;
	TInt ret=phone.Open(iTelServer,DPCKTTSY_PHONE_NAME);
	CHECKPOINT(ret,KErrNone,CHP_OPEN_PHONE);
	CleanupClosePushL(phone);

	RPacketService packetService;
	ret=packetService.Open(phone);
	CHECKPOINT(ret,KErrNone,_L("Failed to open packet service"));
	CleanupClosePushL(packetService);

	RPacketContext packetContext;
	TName contextName;
	ret=packetContext.OpenNewContext(packetService, contextName);
	CHECKPOINT(ret,KErrNone,_L("Failed to open packet context"));
	CleanupClosePushL(packetContext);

	TRequestStatus reqStatus; 
	TUint32 validData = RPacketContext::KHSDPACategory | RPacketContext::KHSUPACategory;
    RPacketContext::TConnectionInfoV1 connectionInfoV1;
    
    //Initialize connectionInfoV1
    connectionInfoV1.iValid = 0;
    connectionInfoV1.iHSDPACategory = 0;
    connectionInfoV1.iHSUPACategory = 0;
    
    TPckg<RPacketContext::TConnectionInfoV1> pckgConnectionInfoV1(connectionInfoV1); 

	//Test GetConnectionInfo
	packetContext.GetConnectionInfo(reqStatus, pckgConnectionInfoV1);
	User::WaitForRequest(reqStatus);
	TEST(reqStatus.Int()                        ==KErrNone);
	TEST(TConnectionInfoBase::KConnectionInfoV1 ==connectionInfoV1.ExtensionId());
	TEST(validData                              ==connectionInfoV1.iValid);	
	TEST(DPCKTTSY_HSDPA_CATEGORY                ==connectionInfoV1.iHSDPACategory);
	TEST(DPCKTTSY_HSUPA_CATEGORY                ==connectionInfoV1.iHSUPACategory);
	INFO_PRINTF1(_L("GetConnectionInfo Test Passed"));

    //Test cancel GetConnectionInfo
	packetContext.GetConnectionInfo(reqStatus, pckgConnectionInfoV1);
	User::After(KThousand);
	packetContext.CancelAsyncRequest(EPacketContextGetConnectionInfo);	
	User::WaitForRequest(reqStatus);
	TEST(reqStatus.Int()==KErrCancel);
	INFO_PRINTF1(_L("GetConnectionInfo Cancel Test Passed"));

    //Reset connectionInfoV1 data before continue testing
    connectionInfoV1.iValid = 0;
    connectionInfoV1.iHSDPACategory = 0;
    connectionInfoV1.iHSUPACategory = 0;

	//Test NotifyConnectionInfoChange
	packetContext.NotifyConnectionInfoChange(reqStatus, pckgConnectionInfoV1);
	User::WaitForRequest(reqStatus);
	TEST(reqStatus.Int()                        ==KErrNone);
	TEST(TConnectionInfoBase::KConnectionInfoV1 ==connectionInfoV1.ExtensionId());
	TEST(validData                              ==connectionInfoV1.iValid);	
	TEST(DPCKTTSY_HSDPA_CATEGORY                ==connectionInfoV1.iHSDPACategory);
	TEST(DPCKTTSY_HSUPA_CATEGORY                ==connectionInfoV1.iHSUPACategory);
	INFO_PRINTF1(_L("NotifyConnectionInfoChange Test Passed"));

    //Test cancel NotifyConnectionInfoChange
	packetContext.NotifyConnectionInfoChange(reqStatus, pckgConnectionInfoV1);
	User::After(KThousand);
	packetContext.CancelAsyncRequest(EPacketContextNotifyConnectionInfoChange);	
	User::WaitForRequest(reqStatus);
	TEST(reqStatus.Int()==KErrCancel);
	INFO_PRINTF1(_L("NotifyConnectionInfoChange Cancel Test Passed"));

    //Close handles    
	CleanupStack::PopAndDestroy(3); //packetContext, packetService, phone

	return TestStepResult();
    }
    
