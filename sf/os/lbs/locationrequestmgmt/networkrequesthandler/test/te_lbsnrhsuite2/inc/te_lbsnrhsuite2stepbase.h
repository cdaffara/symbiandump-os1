/**
* Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description:
*
*/



/**
 @file te_lbsnrhsuite2stepbase.h
*/

#if (!defined TE_LBSNNRH_STEP_BASE__)
#define TE_LBSNNRH_STEP_BASE__
#include <test/testexecutestepbase.h>
// Please add your include here if you have 
#include <lbs/lbsadmin.h>
#include "lbsnrhngchannel.h"
#include "lbsnrhngmsgs.h"
#include "lbsnrhprivctrlchannel.h"
#include "lbsprocesssupervisor.h"
#include "lbsextendmoduleinfo.h"		

/****************************************************************************
* The reason to have a new step base is that it is very much possible
* that the all individual test steps have project related common variables 
* and members 
* and this is the place to define these common variable and members.
* 
****************************************************************************/
class CTe_LbsNrhSuiteStepBase : public CTestStep
	{
public:
	virtual ~CTe_LbsNrhSuiteStepBase();
	CTe_LbsNrhSuiteStepBase();
	virtual TVerdict doTestStepPreambleL(); 
	virtual TVerdict doTestStepPostambleL();
	
	// Set admin settings from a .ini file section
	void ParseAdminSettingsFromIniL();
	
	void GetDeviceGpsModeCapabilitiesFromIniL(TPositionModuleInfoExtended::TDeviceGpsModeCapabilities& aIntegrationModuleCaps);
		
	TInt ConvertStringToPrivacyMechanism(
				const TDesC& aString, 
				CLbsAdmin::TPrivacyHandler& aPrivacyMechanism);
	TInt ConvertStringToExternalLocateService(
				const TDesC& aString, 
				CLbsAdmin::TExternalLocateService& aExternalLocate);
	
	// Helper functions to create various data classes
	TLbsNetMtLrRequestMsg* CreatePrivacyMsgLC(
	        const TLbsNetSessionIdInt& aSessionId,
	        TLbsNetPosRequestPrivacyInt::TLbsRequestAdviceInt aAdvice=TLbsNetPosRequestPrivacyInt::ERequestAdviceVerify,
	        TLbsNetPosRequestPrivacyInt::TLbsRequestActionInt aAction=TLbsNetPosRequestPrivacyInt::ERequestActionAllow,
	        TBool aEmergency=EFalse);
	TLbsNetMtLrRequestMsg* CreatePrivacyVerificationMsgLC(
			const TLbsNetSessionIdInt& aSessionId,TBool aEmergency=EFalse);
	TLbsNetMtLrRequestMsg* CreatePrivacyVerificationMsgAdviceNotifyLC(
			const TLbsNetSessionIdInt& aSessionId,TBool aEmergency=EFalse);

	TLbsNetMtLrRequestMsg* CreatePrivacyNotificationMsgLC(
			const TLbsNetSessionIdInt& aSessionId);
	
	TLbsNetLocationRequestMsg* CreateLocationRequestMsgLC(
			const TLbsNetSessionIdInt& aSessionId,
			TLbsNetworkEnumInt::TLbsNetProtocolServiceInt aService);
	TLbsNetLocationRequestMsg* CreateEmergencyLocationRequestMsgLC(
			const TLbsNetSessionIdInt& aSessionId,
			TLbsNetworkEnumInt::TLbsNetProtocolServiceInt aService);
	

	TLbsNetLocationRequestMsg* CreateLocationRequestMsgLC(
			const TLbsNetSessionIdInt& aSessionId,
			TLbsNetworkEnumInt::TLbsNetProtocolServiceInt aService,
			const TLbsNetPosRequestMethodInt& aPosMethodArray);
	TLbsNetLocationRequestMsg* CreateEmergencyLocationRequestMsgLC(
			const TLbsNetSessionIdInt& aSessionId,
			TLbsNetworkEnumInt::TLbsNetProtocolServiceInt aService,
			const TLbsNetPosRequestMethodInt& aPosMethodArray);
	
	TLbsNetLocationRequestMsg* CreateLocationRequestMsgLC(
			const TLbsNetSessionIdInt& aSessionId,
			TLbsNetworkEnumInt::TLbsNetProtocolServiceInt aService,
			const TLbsNetPosRequestQualityInt& aPosQuality);
	
	TLbsNetLocationRequestMsg* CreateEmergencyLocationRequestMsgLC(
			const TLbsNetSessionIdInt& aSessionId,
			TLbsNetworkEnumInt::TLbsNetProtocolServiceInt aService,
			const TLbsNetPosRequestQualityInt& aPosQuality);
	
	TLbsNetLocationRequestMsg* CreateLocationRequestMsgLC(
			const TLbsNetSessionIdInt& aSessionId,
			TLbsNetworkEnumInt::TLbsNetProtocolServiceInt aService,
			const TLbsNetPosRequestMethodInt& aPosMethodArray,
			const TLbsNetPosRequestQualityInt& aPosQuality);
	TLbsNetLocationRequestMsg* CreateEmergencyLocationRequestMsgLC(
			const TLbsNetSessionIdInt& aSessionId,
			TLbsNetworkEnumInt::TLbsNetProtocolServiceInt aService,
			const TLbsNetPosRequestMethodInt& aPosMethodArray,
			const TLbsNetPosRequestQualityInt& aPosQuality);
	
	TLbsNetLocationUpdateMsg* CreateNetworkReferencePositionMsgLC(
			const TLbsNetSessionIdInt& aSessionId);
	TLbsNetLocationUpdateMsg* CreateNetworkFinalPositionMsgLC(
			const TLbsNetSessionIdInt& aSessionId);
	TLbsNetSessionCompleteMsg* CreateSessionCompeleteMsgLC(
			const TLbsNetSessionIdInt& aSessionId,
			TInt aReason);

//Please add your class members which will be common to all individual test steps:
protected:
	HBufC8*		iReadData;
	HBufC8*		iWriteData;
	
	CActiveScheduler* iScheduler;
	CLbsAdmin* iAdmin;
	CNgChannel* iNgChannel;
	CPrivCtrlChannel* iPrivCtrlChannel;
	RLbsNetworkPositionUpdates iReferencePositionUpdates;
	RLbsNetworkPositionUpdates iFinalPositionUpdates;
	RLbsProcessSupervisor iProcessSupervisor;
	
    // ******** inputs to NRH in Privacyrequest and LocationReqiest *********

	TInt iEmergency;
	
	// when set to 1 privacy reqiest will be cancelled by test code
	TInt iPrivacyrequestCancel;
	
 
	
	TInt iPrivacyRequestTimeoutEnable; // when set to 1 then test will NOT repsond to privacy request
	
	TInt iLocationyRequestTimeoutEnable; // when set to 1 then test will NOT inject a AGPS positon from AGPS manager
	        
	
	// ******** inputs to NRH in Location request *********
	
	// AGPS mode in first location request sent to NRH
	// When Mode1 is undefined then only 1 entry in mode array 
	TInt iFirstRequestAgpsMode0;
	TInt iFirstRequestAgpsMode1;
	
    // AGPS mode in second location request sent to NRH
	// only used when testing multi sessions
    // When Mode1 is undefined then only 1 entry in mode array
  	TInt iSecondRequestAgpsMode0;
	TInt iSecondRequestAgpsMode1;
	
	// AGPS mode in sent to AGPS manager from NRH as a result of 
	// first location request sent to NRH
    // When Mode1 is undefined then only 1 entry in mode array 
	TInt iFirstAgpsModeToAgpsMan0;
	TInt iFirstAgpsModeToAgpsMan1;
	
    // AGPS mode in sent to AGPS manager (test) from NRH as a result of
	// Only used when tesing multiple sessions
    // second location request sent to NRH
    // When Mode1 is undefined then only 1 entry in mode array 
	TInt iSecondAgpsModeToAgpsMan0;
	TInt iSecondAgpsModeToAgpsMan1;
	
	// Error code in RespondLocationRequest sent from NRH to NG (test)
	TInt iReasonInRespLocReq;
	
  	
	// Acc of first request set from NG(test) to NRH
	TInt iAccuracyFirstRequest;
    // MaxFix (timeout) of first request set from NG(test) to NRH
	TInt iMaxFixFirstReq ;
    // Acc of location request sent to AGPS manager (test) as a result of first loc req from NG (test)
    TInt iFirstAccToAgpsMan ;
    // Max fix of location request sent to AGPS manager (test) as a result of first loc req from NG (test)
    TInt iFirstMaxFixToAgpsMan ;
    // power advice sent to AGPSman as a result if first request
    TInt iPowerAdviceAfterFirsRequestName;
    
    // Same for second request (only used when etsting multiple sessions)
	TInt iAccuracySecondRequest ;
	TInt iMaxFixSecondReq ;
	TInt iSecondAccToAgpsMan ;
	TInt iSecondMaxFixToAgpsMan;
	TInt iPowerAdviceAfterSecondRequestName ;

    // used in priv requests and location rerquests sent from NG to NRH (i.e. MTLR, NI etc)
	TInt iSessionType; // KSessionTypeName
	
	// ProcessLocationUpdate(Refpos)
	// sent to NRH - when not defined in  .ini fiel then deafault values are used
	TInt iRefToNrhLat; // KRefToNrhLatName
	TInt iRefToNrhLng; // KRefToNrhLngName
	TInt iRefToNrhHacc; // KRefToNrhHaccName
	
	// when defined in the .ini file these define the contents of the RespondLocationrequestMessage
	// sent out by the NRH
	// if iErrorCodeFromNrh is defined then the message is expected to contain a Position 
	TInt iErrorCodeFromNrh;
	TInt iPosFromNrhType;
	TInt iPosFromNrhLat;
	TInt iPosFromNrhLng;
	TInt iPosFromNrhAcc;
	
	// when defined these are the settings for the Agps position injected into the Nrh
	// when not defined in the .ini file then default values are used when forming the agps positon
	// This is relevant when the agps mode is TB or Hybrid but not when TAP
	// when this is defined then the test will inject an agps position
	// Error code in AGPS pos sent from AGPS manager (test) to NRH
	
	TInt iAgpsPosToNrhError; // KAgpsPosToNrhErrorName
    TInt iAgpsPosToNrhLat; // KAgpsPosToNrhLatName
    TInt iAgpsPosToNrhLng; // KAgpsPosToNrhLngName
    TInt iAgpsPosToNrhHacc; // KAgpsPosToNrhHaccName
    
    // in TAP or Hybrid mode measurements may be  sent into nrh from agps manager (test)
    // when this is defined then the test will inject a measurement
    TInt  iAgpsMeasToNrhErr;
    
    // when defined these are the settings for the position injected into the Nrh from the network
    // in the ProcessLocationUpdate message. For example, inTAP/HYBRID mode it may be a network
    // caclulated position. In other modes it may be the same position as sent to the Network!
    // when not defined in the .ini file then default values are used when forming the agps positon
    TInt iNetworkPosToNrhError;
    TInt iNetworkPosToNrhLat;
    TInt iNetworkPosToNrhLng;
    TInt iNetworkPosToNrhHacc;
    
    // and position sent to network should be measurements
    // when this is defined then the test will check that the ProcessLocationUpdate message
    // contains measurements and has the correct error code
    TInt iMeasFromNrhError;

    // when set to 1 (true) in .ini file then test will inject Ref Pos Before injecting location request
    // when set to 0 (false)
    // if left undefined then the default action for the test is same as when set to  true
    TInt iRefPosBeforeLocationRequest;

    // X3P specific inputs

    // ******** inputs to NRH in TransmitPosition request *********

    TInt iX3pTransmitPositionRequestID;
    TInt iX3pTransmitPositionTransmitPriority;
    TInt iX3pTransmitPositionAgpsAndRefPos;
    TInt iX3pTransmitPositionTimeout;
  
    // when not defined then cancel is sent
    TInt iCancelToAgpsManagerAfterSessionComplete;
    
    
    // MTLR specific inputs

    // ******** inputs to NRH in PrivacyRequest  *********
    TInt iPrivacyRequestEmergency;     // True,False
    TInt iPrivacyRequestAdvice;        // ERequestAdviceNotUsed(0),ERequestAdviceNotify(1),ERequestAdviceVerify(2),ERequestAdviceSilent(3)
    TInt iPrivacyRequestDefaultAction; // ERequestActionNotUsed(0),ERequestActionAllow(1),ERequestActionReject(2)
    
 
    // ******** input from privacy controller  *********
    TInt iPrivacySendResponseAction; // ERequestActionNotUsed(0),ERequestActionAllow(1),ERequestActionReject(2)
    
    // ******** output to NRH in PrivacyResponse  *********
    TInt iPrivacyRcvResponseAction; // ERequestActionNotUsed(0),ERequestActionAllow(1),ERequestActionReject(2)
    
    // Completion code from privacy
    TInt iPrivacyCompleteError;
    
    // sessioncomplete error - KErrNotFound not supported!
    TInt iSessionCompleteError;
    
    // Registration status
    TInt iRegistrationStatus; // KRegistrationStatusName
    
    // sequence number of session
    TInt iSequenceNumber;
	};

#endif

