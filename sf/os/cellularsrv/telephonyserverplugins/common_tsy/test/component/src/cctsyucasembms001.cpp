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
 @file The TEFUnit integration test suite for MBMS Context support in the Common TSY.
*/

#include "cctsyucasembms001.h"
#include "cctsyactiveretriever.h"
#include <etel.h>
#include <etelmm.h>
#include <et_clsvr.h>
#include <ctsy/mmtsy_names.h>
#include <ctsy/serviceapi/mmtsy_ipcdefs.h>
#include "tmockltsydata.h"
#include <ctsy/serviceapi/gsmerror.h>

#include <pcktcs.h>
#include <pcktlist.h>
#include "pcktretrieve.h"
#include "CMmCommonStaticUtility.h"
#include <ctsy/serviceapi/mmtsy_defaults.h>

CTestSuite* CCTsyUCaseMbms001::CreateSuiteL(const TDesC& aName) 
	{
	SUB_SUITE;
    ADD_TEST_STEP_ISO_CPP(CCTsyUCaseMbms001, CCtsyIntegrationTestMbms0001L );
	END_SUITE;
	}    

//
// Actual test cases
//

/**
 * @SYMTestCaseID BA-CTSY-INT-MB-0001
 * @SYMFssID BA/CTSY/PKTS-0001
 * @SYMTestCaseDesc MBMS Phone Support Requirements.
 * @SYMTestPriority High
 * @SYMTestActions RPacketService::GetStaticCaps
 * @SYMTestExpectedResults Pass - MBMS phone capability is supported.
 * @SYMTestType CIT
 * @SYMTestCaseDependencies live/manual
 *
 * Reason for test: to determinate whether phone supports MBMS or not.
 *
 * @return - none
 */
void CCTsyUCaseMbms001::CCtsyIntegrationTestMbms0001L()
    {
        
	//
	// SET UP
	//
	        
    OpenEtelServerL( EUseExtendedError );
	CleanupStack::PushL( TCleanupItem( Cleanup, this ) );
	OpenPhoneL();	
	
	RPacketService packetService;
    OpenPacketServiceL( packetService );
    CleanupClosePushL( packetService );
              
	TInfoName contextId;
    RPacketMbmsContext packetMbmsContext;
    packetMbmsContext.OpenNewContext( packetService, contextId );
    CleanupClosePushL( packetMbmsContext );
    
	//
	// SET UP END
	//    
            
	//
	// TEST START
	//

  	TRequestStatus mockLtsyStatus;	
	RBuf8 data;
	CleanupClosePushL(data);

	RBuf8 data2;
	CleanupClosePushL(data2);

	// Ensure RPacketService::GetStaticCaps returns caps KCapsMBMSSupported.			  	
    TUint staticCaps = 0;
   	TUint expectedCaps = 	RPacketService::KCapsSuspendSupported |
							RPacketService::KCapsNetworkAvailabilitySupported | 
							RPacketService::KCapsSetDefaultContextSupported | 
							RPacketService::KCapsChangeAttachModeSupported | 
							RPacketService::KCapsGetDataTransferredSupported | 
							RPacketService::KCapsPreferredBearerSupported | 
							RPacketService::KCapsPdpDataCompSupported | 
							RPacketService::KCapsPdpHeaderCompSupported | 
						    RPacketService::KCapsMSClassSupported |
							RPacketService::KCapsNotifyMSClassSupported |
							RPacketService::KCapsMBMSSupported;	
									
	TMockLtsyData1<TUint> ltsyData2(expectedCaps);
	data2.Close();
	ltsyData2.SerialiseL(data2);

	iMockLTSY.NotifyTerminated(mockLtsyStatus);	
	iMockLTSY.CompleteL( EPacketGetStaticCaps, KErrNone, data2 );
	User::WaitForRequest(mockLtsyStatus);	
	ASSERT_EQUALS(KErrNone, mockLtsyStatus.Int());
			   		
   	TInt ret = packetService.GetStaticCaps(staticCaps, RPacketContext::EPdpTypeIPv4);
 	AssertMockLtsyStatusL();
	ASSERT_EQUALS(KErrNone, ret);	
				
	ASSERT_EQUALS(expectedCaps, staticCaps);
		
	//
	// TEST END
	//	
	
	AssertMockLtsyStatusL();
	CleanupStack::PopAndDestroy( 5, this ); // packetMbmsContext, packetService, data, data2, this 
    }    
    
