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
// The test server for the Common TSY.
// 
//

/**
 @file 
*/

#include <test/ctefunitserver.h>
#include <test/tefunit.h>

#include "cctsylbsfu.h"
#include "cctsysmsstorefu.h"
#include "cctsydatacallcontrolfu.h"
#include "cctsyphonecapabilityfu.h"
#include "cctsyphonestorefu.h"
#include "cctsysubscriberinfofu.h"
#include "cctsymailboxnumbersfu.h"
#include "cctsyalternatingcallfu.h"
#include "cctsysmsmessagingfu.h"
#include "cctsyusimapplicationfu.h"
#include "cctsyphoneidentityfu.h"
#include "cctsycallcompletionfu.h"
#include "cctsylinecontrolfu.h"
#include "cctsyeapauthenticationfu.h"
#include "cctsyphonepowerfu.h"
#include "cctsycallbarringfu.h"
#include "cctsymultimediacallcontrolfu.h"
#include "cctsyphonebookstorefu.h"
#include "cctsycallcostfu.h"
#include "cctsyimsfu.h"
#include "cctsyfixeddiallingfu.h"
#include "cctsyenstorefu.h"
#include "cctsymulticallcallcontrolfu.h"
#include "cctsyphonesecurityfu.h"
#include "cctsyusertousersignallingfu.h"
#include "cctsydtmffu.h"
#include "cctsypacketqosfu.h"
#include "cctsyonstorefu.h"
#include "cctsysmspstorefu.h"
#include "cctsycallownershipfu.h"
#include "cctsymmsfu.h"
#include "cctsycallemergencyfu.h"
#include "cctsycallwaitingfu.h"
#include "cctsycallinformationfu.h"
#include "cctsycallcontrolfu.h"
#include "cctsytrafficchannelfu.h"
#include "cctsyairtimedurationfu.h"
#include "cctsybroadcastmessagingfu.h"
#include "cctsyconferencecallcontrolfu.h"
#include "cctsysupplementaryservicecapsfu.h"
#include "cctsyussdmessagingfu.h"
#include "cctsymessagewaitingfu.h"
#include "cctsysessionmanagementfu.h"
#include "cctsyindicatorfu.h"
#include "cctsysmartcardapplicationfu.h"
#include "cctsyusernetworkaccessfu.h"
#include "cctsyalternatelineservicefu.h"
#include "cctsyfaxfu.h"
#include "cctsyauthorisationandauthenticationfu.h"
#include "cctsynetworkcontrolfu.h"
#include "cctsysignalstrengthfu.h"
#include "cctsynetworksecurityfu.h"
#include "cctsypacketcontextfu.h"
#include "cctsycallforwardingfu.h"
#include "cctsyprivacyfu.h"
#include "cctsypacketservicefu.h"
#include "cctsypersonalisationfu.h"
#include "cctsyapncontrollistfu.h"
#include "cctsyphonecontrolfu.h"
#include "cctsycustomipcfu.h"
#include "cctsyfeatmgr.h"
#include "cctsysystemstate.h"
#include "cctsytelephonyaudiocontrolfu.h"

#include "cctsymbmscontextfu.h"
#include "cctsyucasembms001.h"
#include "cctsyucasembms002.h"
#include "cctsyucasembms003.h"
#include "cctsyucasembms004.h"
#include "cctsyucasembms005.h"
#include "cctsyucasembms006.h"
#include "cctsyucasembms007.h"
#include "cctsyucasembms008.h"
#include "cctsyucasembms009.h"
#include "cctsyucasembms010.h"
#include "cctsyucasembms011.h"
#include "cctsyucasembms012.h"
#include "cctsyucasembms013.h"
#include "cctsyucasembms014.h"
#include "cctsyucasembms015.h"
#include "cctsyucasembms016.h"
#include "cctsyucasembms017.h"
#include "cctsyucasembms018.h"
#include "cctsyucasembms019.h"
#include "cctsyucasembms020.h"
#include "cctsyucasembms021.h"
#include "cctsyucasembms022.h"
#include "cctsyucasembms023.h"
#include "cctsyucasembms024.h"
#include "cctsyucasembms025.h"
#include "cctsyucasembms026.h"
#include "cctsyucasembms027.h"

// The server name: must be the same that the exe name
_LIT( KServerName, "te_ctsy_component" );

/**
 * ServerName
 *
 * @return The TEF server name.
 */
GLDEF_C const TTestName ServerName()

	{
	TTestName serverName(KServerName);
	return serverName;
	}

/**
 * @return The top level suite.
 */
GLDEF_C CTestSuite* CreateTestSuiteL()
	{
	START_SUITE

	ADD_TEST_SUITE(CCTsyLBSFU)
	ADD_TEST_SUITE(CCTsySmsStoreFU)
	ADD_TEST_SUITE(CCTsyDataCallControlFU)
	ADD_TEST_SUITE(CCTsyPhoneCapabilityFU)
	ADD_TEST_SUITE(CCTsyPhoneStoreFU)
	ADD_TEST_SUITE(CCTsySubscriberInfoFU)
	ADD_TEST_SUITE(CCTsyMailboxNumbersFU)
	ADD_TEST_SUITE(CCTsyAlternatingCallFU)
	ADD_TEST_SUITE(CCTsySmsMessagingFU)
	ADD_TEST_SUITE(CCTsyUSIMApplicationFU)
	ADD_TEST_SUITE(CCTsyPhoneIdentityFU)
	ADD_TEST_SUITE(CCTsyCallCompletionFU)
	ADD_TEST_SUITE(CCTsyLineControlFU)
	ADD_TEST_SUITE(CCTsyEAPAuthenticationFU)
	ADD_TEST_SUITE(CCTsyPhonePowerFU)
	ADD_TEST_SUITE(CCTsyCallBarringFU)
	ADD_TEST_SUITE(CCTsyMultimediaCallControlFU)
	ADD_TEST_SUITE(CCTsyPhoneBookStoreFU)
	ADD_TEST_SUITE(CCTsyCallCostFU)
	ADD_TEST_SUITE(CCTsyIMSFU)
	ADD_TEST_SUITE(CCTsyFixedDiallingFU)
	ADD_TEST_SUITE(CCTsyENStoreFU)
	ADD_TEST_SUITE(CCTsyMulticallCallControlFU)
	ADD_TEST_SUITE(CCTsyPhoneSecurityFU)
	ADD_TEST_SUITE(CCTsyUserToUserSignallingFU)
	ADD_TEST_SUITE(CCTsyDTMFFU)
	ADD_TEST_SUITE(CCTsyPacketQoSFU)
	ADD_TEST_SUITE(CCTsyONStoreFU)
	ADD_TEST_SUITE(CCTsySMSPStoreFU)
	ADD_TEST_SUITE(CCTsyCallOwnershipFU)
	ADD_TEST_SUITE(CCTsyMMSFU)
	ADD_TEST_SUITE(CCTsyCallEmergencyFU)
	ADD_TEST_SUITE(CCTsyCallWaitingFU)
	ADD_TEST_SUITE(CCTsyCallInformationFU)
	ADD_TEST_SUITE(CCTsyCallControlFU)
	ADD_TEST_SUITE(CCTsyTrafficChannelFU)
	ADD_TEST_SUITE(CCTsyAirTimeDurationFU)
	ADD_TEST_SUITE(CCTsyBroadcastMessagingFU)
	ADD_TEST_SUITE(CCTsyConferenceCallControlFU)
	ADD_TEST_SUITE(CCTsySupplementaryServiceCapsFU)
	ADD_TEST_SUITE(CCTsyUssdMessagingFU)
	ADD_TEST_SUITE(CCTsyMessageWaitingFU)
	ADD_TEST_SUITE(CCTsySessionManagementFU)
	ADD_TEST_SUITE(CCTsyIndicatorFU)
	ADD_TEST_SUITE(CCTsySmartCardApplicationFU)
	ADD_TEST_SUITE(CCTsyUserNetworkAccessFU)
	ADD_TEST_SUITE(CCTsyAlternateLineServiceFU)
	ADD_TEST_SUITE(CCTsyFaxFU)
	ADD_TEST_SUITE(CCTsyAuthorisationAndAuthenticationFU)
	ADD_TEST_SUITE(CCTsyNetworkControlFU)
	ADD_TEST_SUITE(CCTsySignalStrengthFU)
	ADD_TEST_SUITE(CCTsyNetworkSecurityFU)
	ADD_TEST_SUITE(CCTsyPacketContextFU)
	ADD_TEST_SUITE(CCTsyCallForwardingFU)
	ADD_TEST_SUITE(CCTsyPrivacyFU)
	ADD_TEST_SUITE(CCTsyPacketServiceFU)
	ADD_TEST_SUITE(CCTsyPersonalisationFU)
	ADD_TEST_SUITE(CCTsyAPNControlListFU)
	ADD_TEST_SUITE(CCTsyPhoneControlFU)
	ADD_TEST_SUITE(CCTsyCustomIPCFU)
	ADD_TEST_SUITE(CCTsyFeatMgr)
	ADD_TEST_SUITE(CCTsySystemState)
	ADD_TEST_SUITE(CCTsyTelephonyAudioControlFU)

	ADD_TEST_SUITE(CCTsyMbmsContextFU)
	ADD_TEST_SUITE(CCTsyUCaseMbms001)
	ADD_TEST_SUITE(CCTsyUCaseMbms002)
	ADD_TEST_SUITE(CCTsyUCaseMbms003)
	ADD_TEST_SUITE(CCTsyUCaseMbms004)
	ADD_TEST_SUITE(CCTsyUCaseMbms005)
	ADD_TEST_SUITE(CCTsyUCaseMbms006)
	ADD_TEST_SUITE(CCTsyUCaseMbms007)
	ADD_TEST_SUITE(CCTsyUCaseMbms008)
	ADD_TEST_SUITE(CCTsyUCaseMbms009)
	ADD_TEST_SUITE(CCTsyUCaseMbms010)
	ADD_TEST_SUITE(CCTsyUCaseMbms011)
	ADD_TEST_SUITE(CCTsyUCaseMbms012)
	ADD_TEST_SUITE(CCTsyUCaseMbms013)
	ADD_TEST_SUITE(CCTsyUCaseMbms014)
	ADD_TEST_SUITE(CCTsyUCaseMbms015)
	ADD_TEST_SUITE(CCTsyUCaseMbms016)
	ADD_TEST_SUITE(CCTsyUCaseMbms017)
	ADD_TEST_SUITE(CCTsyUCaseMbms018)
	ADD_TEST_SUITE(CCTsyUCaseMbms019)
	ADD_TEST_SUITE(CCTsyUCaseMbms020)	
	ADD_TEST_SUITE(CCTsyUCaseMbms021)
	ADD_TEST_SUITE(CCTsyUCaseMbms022)
	ADD_TEST_SUITE(CCTsyUCaseMbms023)
	ADD_TEST_SUITE(CCTsyUCaseMbms024)
	ADD_TEST_SUITE(CCTsyUCaseMbms025)
	ADD_TEST_SUITE(CCTsyUCaseMbms026)
	ADD_TEST_SUITE(CCTsyUCaseMbms027)
			
	END_SUITE
	}

// Require: create std TEF test case
GLDEF_D CTestStep* CreateTEFTestStep(const TDesC& /*aStepName*/, CTEFUnitServer& /*aServer*/)
	{
	// This allows std TEF test steps to be created and run inside TEFUnit
	return NULL;
	}
