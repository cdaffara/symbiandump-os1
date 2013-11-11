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
// EtelPacketMbmsSupport.cpp
// This contains EtelPacket TestCase 
// 
//

// Symbian OS includes
#include <e32base.h>

#include <etelpckt.h>
#include <pcktlist.h>
#include <pcktcs.h>
#include "Te_EtelPacketTestStepBase.h"
#include "testdef.h"
#include "Te_EtelPacketMbmsSupport.h"
#include "Te_etelpckt_defs.h"

// constructor
CTestMbmsSupport::CTestMbmsSupport()
{
	// store the name of this test case
	SetTestStepName(_L("MBMS_SUPPORT"));
}

// destructor
CTestMbmsSupport::~CTestMbmsSupport()
{
}

enum TVerdict CTestMbmsSupport::doTestStepL( void )
/**
 * Test step to check the MBMS Support and Availability in a cell
 */
{
	RPhone phone;
	TInt ret=phone.Open(iTelServer,DPCKTTSY_PHONE_NAME);

	RPacketService pcktService;
	ret=pcktService.Open(phone);
	
	/**
	 * Verify the extension identification of MBMS functionality
	 */
	RPacketService::TMbmsServiceAvailabilityV1 aMbmsSAV1;
	TESTL(aMbmsSAV1.ExtensionId() == KETelExtPcktV1);
	INFO_PRINTF1(_L("MBMS Extension Identifier correct"));
	
	/**
	 * Verify whether the phone supports MBMS
	 */
	TUint aCaps;
	TInt mbmsStaticCap = pcktService.GetStaticCaps(aCaps, RPacketContext::EPdpTypeIPv4);
	TESTL(mbmsStaticCap == KErrNone);
	TESTL(aCaps == DPCKTTSY_MISC_CAPS);
	INFO_PRINTF1(_L("Test for MBMS support in the device - Completed"));
	
	/**
	 * Check whether MBMS is being used by the device. [Dynamic capability]
	 */
	TRequestStatus reqStatusNotifyDynCaps;
	RPacketService::TDynamicCapsFlags dynamicCapsFlags;
	pcktService.NotifyDynamicCapsChange(reqStatusNotifyDynCaps, dynamicCapsFlags);
	User::WaitForRequest(reqStatusNotifyDynCaps);
	TESTL(reqStatusNotifyDynCaps == KErrNone);
	TESTL(dynamicCapsFlags == DPCKTTSY_DYNAMIC_CAPS2);
	INFO_PRINTF1(_L("Notify Dynamic Caps - Completed"));
	
	/**
	 * Cancel Notify dynamic caps API
	 */
	TRequestStatus reqStatusNotifyDynCapsCancel;
	pcktService.NotifyDynamicCapsChange(reqStatusNotifyDynCapsCancel, dynamicCapsFlags);
	pcktService.CancelAsyncRequest(EPacketNotifyDynamicCapsChange);
	User::WaitForRequest(reqStatusNotifyDynCapsCancel);
	TESTL(reqStatusNotifyDynCapsCancel == KErrCancel);
	INFO_PRINTF1(_L("Cancel Notify Dynamic Caps - Completed"));
	
	/**
	 * Get dynamic caps of MBMS
	 */
	TRequestStatus reqStatusGetDynCaps;
	TInt retDynCaps = pcktService.GetDynamicCaps(dynamicCapsFlags);
	TESTL(retDynCaps == KErrNone);
	TESTL(dynamicCapsFlags == DPCKTTSY_DYNAMIC_CAPS1);
	INFO_PRINTF1(_L("Get Dynamic Caps - Completed"));
	
	/**
	 * Receive notification about status change on MBMS support in the current network.
	 */
	TRequestStatus aGetNtwkServiceStatus;
	TMbmsNetworkServiceStatus aMbmsNtwkServiceStatus;
	pcktService.NotifyMbmsNetworkServiceStatusChange(aGetNtwkServiceStatus, aMbmsNtwkServiceStatus);
	
	User::WaitForRequest(aGetNtwkServiceStatus);
	TESTL(aMbmsNtwkServiceStatus == DPCKTTSY_MBMS_NETWORKSERVICE_SUPPORTED);
	TESTL(aGetNtwkServiceStatus == KErrNone);
	INFO_PRINTF1(_L("Test for notification on MBMS support in the device - Completed"));
	
	/**
	 * Cancel the notification posted to know about status change on MBMS support in the current network.
	 */
	pcktService.NotifyMbmsNetworkServiceStatusChange(aGetNtwkServiceStatus, aMbmsNtwkServiceStatus);
	pcktService.CancelAsyncRequest(EPacketNotifyMbmsNetworkServiceStatusChange);
	User::WaitForRequest(aGetNtwkServiceStatus);
	
	TESTL(aGetNtwkServiceStatus == KErrCancel);
	INFO_PRINTF1(_L("Test for notification cancellation on MBMS support in the device - CANCELLED"));
	
	/**
	 * Get status on the support of MBMS in the current network. 
	 * Request to attempt GMM Attach if not done earlier.
	 */
	TBool gmmAttach =ETrue;
	pcktService.GetMbmsNetworkServiceStatus(aGetNtwkServiceStatus, gmmAttach, aMbmsNtwkServiceStatus);
	User::WaitForRequest(aGetNtwkServiceStatus);
	TESTL(aMbmsNtwkServiceStatus == DPCKTTSY_MBMS_NETWORKSERVICE_SUPPORTED);
	TESTL(aGetNtwkServiceStatus == KErrNone);
	INFO_PRINTF1(_L("Test for MBMS support, without GMM Attach request, in the network - Completed"));


	/**
	 * Get status on the support of MBMS in the current network. 
	 * Do not perform GMM Attach.
	 */
	gmmAttach = EFalse;
	pcktService.GetMbmsNetworkServiceStatus(aGetNtwkServiceStatus, gmmAttach, aMbmsNtwkServiceStatus);
	User::WaitForRequest(aGetNtwkServiceStatus);
	TESTL(aMbmsNtwkServiceStatus == DPCKTTSY_MBMS_NETWORKSERVICE_UNKNOWN);
	TESTL(aGetNtwkServiceStatus == KErrNone);
	INFO_PRINTF1(_L("Test for MBMS support, with GMM Attach request, in the network - Completed"));
	
	/**
	 * Cancel the Get status on the support of MBMS in the current network
	 */
	gmmAttach = ETrue;
	pcktService.GetMbmsNetworkServiceStatus(aGetNtwkServiceStatus,gmmAttach, aMbmsNtwkServiceStatus);
	pcktService.CancelAsyncRequest(EPacketGetMbmsNetworkServiceStatus);
	User::WaitForRequest(aGetNtwkServiceStatus);
	
	TESTL(aGetNtwkServiceStatus == KErrCancel);
	INFO_PRINTF1(_L("Test for GetMbmsNetworkServiceStatus cancellation on MBMS support in the device - CANCELLED"));
		
	pcktService.Close();
	phone.Close();
	return TestStepResult();
}
