// Copyright (c) 2002-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// EtelPacketTest3G_1.cpp
// JetStream Packet data tests.
// This file tests the 3G (WCDMA) Packet Service functionality - TestCase 3G.1 
// 
//

// Symbian OS includes
#include <e32base.h>

#include <etelpckt.h>
#include <pcktcs.h>

#include "Te_EtelPacketTestStepBase.h"

#include "testdef.h"

#include "Te_EtelPacketTest3g_1.h"
#include "Te_etelpckt_defs.h"


CEtelPacketTest3G_1::CEtelPacketTest3G_1()
/** Constructor
 *  Stores the name of this test case.
 */
{
	SetTestStepName(_L("3GPacketService"));
}


CEtelPacketTest3G_1::~CEtelPacketTest3G_1()
/**
 * Destructor
 */
{
}


enum TVerdict CEtelPacketTest3G_1::doTestStepL( void )
/**
 * Test step 3G.1 calls the 3G Packet service (RPacketService) methods
 */
{
	RPhone pcktPhone;
	TInt ret=pcktPhone.Open(iTelServer,DPCKTTSY_PHONE_NAME);
	CHECKPOINT_EX(ret==KErrNone,CHP_OPEN_PHONE);

	RPacketService wcdmaService;
	ret=wcdmaService.Open(pcktPhone);
    CHECKPOINT(ret,KErrNone,CHP_SRVS_CASE("A.3G1"));

	TRequestStatus reqStatus;
	
// Enumerate NIFs
	TInt nifCount;
	wcdmaService.EnumerateNifs(reqStatus, nifCount);
	User::WaitForRequest(reqStatus);

	CHECKPOINT(reqStatus.Int(),KErrNone,CHP_SRVS_CASE("A.3G2"));
	CHECKPOINT(nifCount,DPCKTTSY_NUMBER_OF_NIFS,CHP_SRVS_CASE("A.3G2"));

// Cancel Request
	wcdmaService.EnumerateNifs(reqStatus, nifCount);
	wcdmaService.CancelAsyncRequest(EPacketEnumerateNifs);
	User::WaitForRequest(reqStatus);
	CHECKPOINT(reqStatus.Int(), KErrCancel, CHP_SRVS_CASE("A.3G2"))

// Get NIF Information
	TInt nifIndex=1;		// 2 NIFs are defined in the testdef.h file
	RPacketService::TNifInfoV2 nifInfoV2;
	RPacketService::TNifInfoV2Pckg nifInfoV2Pckg(nifInfoV2);

	wcdmaService.GetNifInfo(reqStatus, nifIndex, nifInfoV2Pckg);
	User::WaitForRequest(reqStatus);

	CHECKPOINT(reqStatus.Int(),KErrNone,CHP_SRVS_CASE("A.3G3"));
	CHECKPOINT_COMP_STR(nifInfoV2.iContextName,DPCKTTSY_NIF_CONTEXT1,CHP_SRVS_CASE("A.3G3"));
	CHECKPOINT(nifInfoV2.iNumberOfContexts,DPCKTTSY_NUMBER_OF_CONTEXTS_IN_NIF1,CHP_SRVS_CASE("A.3G3"));
	CHECKPOINT(nifInfoV2.iNifStatus,DPCKTTSY_CONTEXT_INFO_STATUS0,CHP_SRVS_CASE("A.3G3"));
	CHECKPOINT_COMP_STR(nifInfoV2.iPdpAddress,DPCKTTSY_PDP_ADDRESS1,CHP_SRVS_CASE("A.3G3"));
	CHECKPOINT(nifInfoV2.iContextType,DPCKTTSY_INTERNAL_CONTEXT,CHP_SRVS_CASE("A.3G3"));

// Cancel Request
	wcdmaService.GetNifInfo(reqStatus, nifIndex, nifInfoV2Pckg);
	wcdmaService.CancelAsyncRequest(EPacketGetNifInfo);
	User::WaitForRequest(reqStatus);
	CHECKPOINT(reqStatus.Int(), KErrCancel, CHP_SRVS_CASE("A.3G3"))
	
// Enumerate Contexts in NIF
	TInt nifContextCount;
	TBufC<DPCKTTSY_NIF_NAME_LENGTH> existingContextName=DPCKTTSY_NIF_CONTEXT2;

	wcdmaService.EnumerateContextsInNif(reqStatus, existingContextName, nifContextCount);
	User::WaitForRequest(reqStatus);

	CHECKPOINT(reqStatus.Int(),KErrNone,CHP_SRVS_CASE("A.3G4"));
	CHECKPOINT(nifContextCount,DPCKTTSY_NUMBER_OF_CONTEXTS_IN_NIF2,CHP_SRVS_CASE("A.3G4"));

// Cancel Request
	wcdmaService.EnumerateContextsInNif(reqStatus, existingContextName, nifContextCount);
	wcdmaService.CancelAsyncRequest(EPacketEnumerateContextsInNif);
	User::WaitForRequest(reqStatus);
	CHECKPOINT(reqStatus.Int(), KErrCancel, CHP_SRVS_CASE("A.3G4"))

// Get Context name in NIF
	TInt contextIndex=1;
	TName contextName;

	wcdmaService.GetContextNameInNif(reqStatus, existingContextName, contextIndex, contextName);	
	User::WaitForRequest(reqStatus);

	CHECKPOINT(reqStatus.Int(),KErrNone,CHP_SRVS_CASE("A.3G5"));
	CHECKPOINT_COMP_STR(contextName,DPCKTTSY_CONTEXT2_IN_NIF2,CHP_SRVS_CASE("A.3G5"));

									
// Cancel Request
	wcdmaService.GetContextNameInNif(reqStatus, existingContextName, contextIndex, contextName);	
	wcdmaService.CancelAsyncRequest(EPacketGetContextNameInNif);
	User::WaitForRequest(reqStatus);
	CHECKPOINT(reqStatus.Int(), KErrCancel, CHP_SRVS_CASE("A.3G5"))

// Change Request: NDOE-57BHUF.
// Get Current Packet Release mode
	RPacketService::TPacketReleaseMode releaseMode;
	wcdmaService.GetCurrentReleaseMode(reqStatus, releaseMode);
	User::WaitForRequest(reqStatus);
	CHECKPOINT(reqStatus.Int(),KErrNone,CHP_SRVS_CASE("A.3G6"));
	CHECKPOINT(releaseMode,DPCKTTSY_NETWORK_MODE1,CHP_SRVS_CASE("A.3G6"));

// Cancel Get Current Packet Release mode
	wcdmaService.GetCurrentReleaseMode(reqStatus, releaseMode);
	wcdmaService.CancelAsyncRequest(EPacketGetCurrentReleaseMode);
	User::WaitForRequest(reqStatus);
	CHECKPOINT(reqStatus.Int(),KErrCancel,CHP_SRVS_CASE("A.3G6"));

// Roaming notification - NotifyReleaseModeChange() method
	wcdmaService.NotifyReleaseModeChange(reqStatus, releaseMode);
	User::WaitForRequest(reqStatus);
	CHECKPOINT(reqStatus.Int(),KErrNone,CHP_SRVS_CASE("A.3G7"));
	CHECKPOINT(releaseMode,DPCKTTSY_NETWORK_MODE2,CHP_SRVS_CASE("A.3G7"));

// Cancel Roaming notification - NotifyReleaseModeChange() method
	wcdmaService.NotifyReleaseModeChange(reqStatus, releaseMode);
	wcdmaService.CancelAsyncRequest(EPacketNotifyReleaseModeChange);
	User::WaitForRequest(reqStatus);
	CHECKPOINT(reqStatus.Int(),KErrCancel,CHP_SRVS_CASE("A.3G7"));
	
//DeactivateNIF
	wcdmaService.DeactivateNIF(reqStatus, DPCKTTSY_NIF_CONTEXT1);
	User::WaitForRequest(reqStatus);
	CHECKPOINT(reqStatus.Int(),KErrNone,CHP_SRVS_CASE("A.3G8"));
	
//Cancel DeactivateNIF
	wcdmaService.DeactivateNIF(reqStatus, DPCKTTSY_NIF_CONTEXT1);
	wcdmaService.CancelAsyncRequest(EPacketDeactivateNIF);
	User::WaitForRequest(reqStatus);
	CHECKPOINT(reqStatus.Int(),KErrCancel,CHP_SRVS_CASE("A.3G8"));	

	wcdmaService.Close();
	pcktPhone.Close();

	return TestStepResult();
}
