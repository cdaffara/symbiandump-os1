/**
* Copyright (c) 2004-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* This file define all the common values thoughout your test project
* 
*
*/



/**
 @file te_lbsnrhsuite2defs.h
*/
#if (!defined TE_LBSNRHSUITE2DEFS_H__)
#define TE_LBSNRHSUITE2DEFS_H__



/* *************************************************** */

// Member Vairable: iSessionType
// Valid Options: 
//      EServiceNetworkLocation(1)
//      EServiceSelfLocation(2)
//      EServiceMobileTerminated(4)
//      EServiceTransmitThirdParty(8)
//      EServiceNetworkInduced(16)
_LIT(KSessionTypeName,                  "sessiontype");

// Member Variable: iRegistrationStatus
// Valid Options:
//      ERegisteredHomeNetwork(2)
//      ERegisteredRoamingNetwork(3)
_LIT(KRegistrationStatusName,           "registrationstatus");

// **********************************************************************************
// AGPS position sent to NRH from test (AGPS manager)

// Member Variable: iAgpsPosToNrhError
// Valid Options:  Any KErr??? code
const TInt KDefaultAgpsPosToNrhError=KErrNone;
_LIT(KAgpsPosToNrhErrorName,              "agpspostonrherror"); // 

// Member Variable: iAgpsPosToNrhLat
// Valid Options: Any Valid Lat
const TReal64 KDefaultAgpsPosToNrhLat=52.2455;
_LIT(KAgpsPosToNrhLatName,              "agpspostonrhlat");

// Member Variable: iAgpsPosToNrhLng
// Valid Options: Any Valid Lng
const TReal64 KDefaultAgpsPosToNrhLng=0.1665;
_LIT(KAgpsPosToNrhLngName,              "agpspostonrhlng");

// Member Variable: iAgpsPosToNrhHacc
// Valid Options: 
//      Any Valid Accuracy
const TReal32 KDefaultAgpsPosToNrhHacc=30.0;
_LIT(KAgpsPosToNrhHaccName,             "agpspostonrhhacc");

// **********************************************************************************
// reference position sent to NRH from test (network)

// Member Variable: iRefToNrhLat
// Valid Options: 
//      Any Valid Lat
const TReal32 KDefaultRefToNrhLat=52.2455;
_LIT(KRefToNrhLatName,              "reftonrhlat");

// Member Variable: iRefToNrhLng
// Valid Options: 
//      Any Valid Lng
const TReal32 KDefaultRefToNrhLng=0.1665;
_LIT(KRefToNrhLngName,              "reftonrhlng");

// Member Variable: iRefToNrhHacc
// Valid Options: 
//      Any Valid Accuracy
const TReal32 KDefaultRefToNrhHacc=500.0;
_LIT(KRefToNrhHaccName,             "reftonrhhacc");

// FNP sent to NRH from test (network)
// Member Variable: iNetworkPosToNrhError
// Valid Options:
//      Any KErr???.
_LIT(KNetworkPosToNrhErrorName,            "networkpostonrherror");
// Member Variable: iNetworkPosToNrhLat
// Valid Options: 
//      Any Valid Lat
_LIT(KNetworkPosToNrhLatName,              "networkpostonrhlat");
// Member Variable: iNetworkPosToNrhLng
// Valid Options: 
//      Any Valid Lng
_LIT(KNetworkPosToNrhLngName,              "networkpostonrhlng");
// Member Variable: iNetworkPosToNrhHacc
// Valid Options: 
//      Any Valid Accuracy
_LIT(KNetworkPosToNrhHaccName,             "networkpostonrhhacc");

// testexecute .ini file key names for admin settings.
// note that admin settings are set using ParseAdminSettingsFromIniL
// which is called by StartLbs step BEFORE we start LBS

// ADMIN SETTING = KLbsSettingHomeExternalLocate
// Options:
//      EExternalLocateOn 					(1)
//      EExternalLocateOnButAlwaysVerify 	(2)
//      EExternalLocateOff 					(3)
//      EExternalLocateOffButNotify 		(4)       
_LIT(KHomeExternalLocateKeyName,            "HomeExternalLocate");       

// ADMIN SETTING = KLbsSettingHomeNetworkInducedLocate
// Options:
//		EExternalLocateOn(0x01)
//		EExternalLocateOnButAlwaysVerify(0x02)
//		EExternalLocateOff(0x03)
//		EExternalLocateOffButNotify(0x04)
_LIT(KHomeNetworkInducedLocateKeyName,      "HomeNetworkInducedLocate"); 

// ADMIN SETTING = KLbsSettingHomeEmergencyLocate
// Options:
//		EExternalLocateOn(0x01)
//		EExternalLocateOnButAlwaysVerify(0x02)
//		EExternalLocateOff(0x03)
//		EExternalLocateOffButNotify(0x04)
_LIT(KHomeEmergencyLocateKeyName,           "HomeEmergencyLocate"); 

// ADMIN SETTING = KLbsSettingHomeSelfLocate
// Options:
//		ESelfLocateOn(0x01)
//		ESelfLocateOff(0x02)
_LIT(KHomeSelfLocateKeyName,                "HomeSelfLocate"); 

// ADMIN SETTING = KLbsSettingHomeTransmitLocate
// Options:
//		ETransmitLocateOn(0x01)
//		ETransmitLocateOff(0x02)
_LIT(KHomeTransmitLocateKeyName,            "HomeTransmitLocateLocate"); 

// ADMIN SETTING = KLbsSettingRoamingTransmitLocate
// Options:
//		ETransmitLocateOn(0x01)
//		ETransmitLocateOff(0x02)
_LIT(KRoamingTransmitLocateKeyName,         "RoamingTransmitLocateLocate"); 

// ADMIN SETTING = KLbsSettingRoamingExternalLocate
// Options:
//		EExternalLocateOn(0x01)
//		EExternalLocateOnButAlwaysVerify(0x02)
//		EExternalLocateOff(0x03)
//		EExternalLocateOffButNotify(0x04)
_LIT(KRoamingExternalLocateKeyName,         "RoamingExternalLocate"); 

// ADMIN SETTING = KLbsSettingRoamingEmergencyLocate
// Options:
//		EExternalLocateOn(0x01)
//		EExternalLocateOnButAlwaysVerify(0x02)
//		EExternalLocateOff(0x03)
//		EExternalLocateOffButNotify(0x04)
_LIT(KRoamingEmergencyLocateKeyName,        "RoamingEmergencyLocate");  

// ADMIN SETTING = KLbsSettingRoamingSelfLocate
// Options:
//		ESelfLocateOn(0x01)
//		ESelfLocateOff(0x02)
_LIT(KRoamingSelfLocateKeyName,             "RoamingSelfLocate"); 

// ADMIN SETTING = KLbsSettingQualityProfileSelfLocate
// Options:
//		???
_LIT(KQualityProfileSelfLocateKeyName,      "QualityProfileSelfLocate"); 

// ADMIN SETTING = KLbsSettingQualityProfileExternalLocate
// Options:
//		???
_LIT(KQualityProfileExternalLocateKeyName,  "QualityProfileExternalLocate"); 

// ADMIN SETTING = KLbsSettingQualityProfileTransmitLocate
// Options:
//		???
_LIT(KQualityProfileTransmitLocateKeyName,  "QualityProfileTransmitLocate");    // 

// ADMIN SETTING = KLbsSettingPrivacyTimeoutAction
// Options:
//		EPrivacyTimeoutNetworkDefined(0x01)
//		EPrivacyTimeoutReject(0x02)
//		EPrivacyTimeoutIgnore(0x03)
_LIT(KPrivacyTimeoutActionName,  "PrivacyTimeoutAction"); 

// ADMIN SETTING = KLbsSettingPrivacyHandler
// Options:
//		EPrivacyHandleByController(0x02) -> Only supported option
_LIT(KPrivacyMechanismName,                 "PrivacyMechanism"); 

// ADMIN SETTING = KLbsSettingMaximumExternalLocateRequests
// Options:
//		Any number
_LIT(KMaximumExternalLocateRequests,        "MaximumExternalLocateRequests");

// ADMIN SETTING = KLbsSettingHomeGpsMode
// Options:
//		EGpsPreferTerminalBased(0x01)
//		EGpsAlwaysTerminalBased(0x02) > NOT SUPPORTED
//		EGpsAutonomous(0x03)
//		EGpsPreferTerminalAssisted(0x04)
//		EGpsAlwaysTerminalAssisted(0x05)
_LIT(KAGPSModeName,                         "gpshomeadminmode");

// ADMIN SETTING = KLbsSettingRoamingGpsMode
// Options:
//		EGpsPreferTerminalBased(0x01)
//		EGpsAlwaysTerminalBased(0x02) > NOT SUPPORTED
//		EGpsAutonomous(0x03)
//		EGpsPreferTerminalAssisted(0x04)
//		EGpsAlwaysTerminalAssisted(0x05)
_LIT(KAGPSRoamingModeName,                  "gpsroamingadminmode");
// end if ADmin settings section

// Module Capabilities of the AGPS Integration Module
// 0 - Terminla Based
// 1 - Terminal Assisted
// 2 - Terminal Based OR Terminal Assisted
// 3 - Simultaneous TA and TB
// 4 - Autonomous
_LIT(KModuleCapsName, "modulecaps");

// NOTE: each mode spcification can be one or two bytes
// when its one byte then the second byte is left undefined in the .ini file

// Member Variable: iFirstRequestAgpsMode0, iFirstRequestAgpsMode1
// Valid Options: Bitmask of the following 
//		ETechnologyTerminal	= 0x01,
//		ETechnologyNetwork	= 0x02,
//		ETechnologyAssisted	= 0x04
_LIT(KModeFirstRequest0,            "modefirstrequest0");
_LIT(KModeFirstRequest1,            "modefirstrequest1");

// Member Variable: iSecondRequestAgpsMode0, iSecondRequestAgpsMode1
// Valid Options: Bitmask of the following 
//		ETechnologyTerminal	= 0x01,
//		ETechnologyNetwork	= 0x02,
//		ETechnologyAssisted	= 0x04
_LIT(KModeSecondRequest0,           "modesecondrequest0");
_LIT(KModeSecondRequest1,           "modesecondrequest1");

// Member Variable: iFirstAgpsModeToAgpsMan0, iFirstAgpsModeToAgpsMan1
// Valid Options: 
//		ETechnologyTerminal	= 0x01,
//		ETechnologyNetwork	= 0x02,
//		ETechnologyAssisted	= 0x04
_LIT(KFirstModeToAgpsMan0,          "firstmodetoagpsman0");
_LIT(KFirstModeToAgpsMan1,          "firstmodetoagpsman1");


// Member Variable: iSecondAgpsModeToAgpsMan0, iSecondAgpsModeToAgpsMan1
// Valid Options: 
//		ETechnologyTerminal	= 0x01,
//		ETechnologyNetwork	= 0x02,
//		ETechnologyAssisted	= 0x04
_LIT(KSecondModeToAgpsMan0,         "secondmodetoagpsman0");
_LIT(KSecondModeToAgpsMan1,         "secondmodetoagpsman1");

// Member Variable: iAccuracyFirstRequest, iAccuracySecondRequest
// Valid Options: 
//		Any valid accuracy
_LIT(KAccuracyFirstRequestName,     "accuracyfirstrequest");
_LIT(KAccuracySecondRequestName,    "accuracysecondrequest");

// Member Variable: iMaxFixFirstReq, iMaxFixSecondReq
// Valid Options: 
//		Any valid max fix time
_LIT(KMaxFixFirstReqName,           "maxfixfirstreq");
_LIT(KMaxFixSecondReqName,          "maxfixsecondtreq");

// Member Variable: iReasonInRespLocReq
// Valid Options: 
//		Any KErr???
_LIT(KReasonInRespLocRequestName,   "reasoninresplocreq");

// Member Variable: iAgpsPosToNrhError
// Valid Options: 
//		Any KErr???
_LIT(KErrorInAgpsPositionName,      "agpsposerror");

// Member Variable: iAccuracyFirstRequest, iAccuracySecondRequest
// Valid Options: 
//		Any valid accuracy
_LIT(KFirstAccToAgpsManName,        "firstacctoagpsman");
_LIT(KSecondAccToAgpsManName,       "secondacctoagpsman");

// Member Variable: iFirstMaxFixToAgpsMan, iSecondMaxFixToAgpsMan 
// Valid Options: 
//		Any valid max fix time
_LIT(KFirstMaxFixToAgpsManName,     "firstmaxfixtoagpsman");
_LIT(KSecondMaxFixToAgpsManName,    "secondmaxfixtotoagpsman");

// Member Variable: iEmergency 
// Valid Options: 
//		0 = Not an Emergency
//		1 = Emergency
_LIT(KEmergencyName,                 "emergency");

// Member Variable: iSequenceNumber 
// Valid Options: unsigned interger 

_LIT(KSequenceNumberName,                 "sequencenumber");


// Member Variable: iSessionCompleteError
// Valid Options: Any KErr code
const TInt KDefaultSessionCompleteErrorCode=KErrNone;
_LIT(KSessionCompleteErrorCodeName, "sessioncompleterrorcode");

// Member Variable: iRefPosBeforeLocationRequest
// Valid Options: 
//		If set then before

// If not defined in .ini file then the default is
// to send in ref postion before location request
const TUint KRefBeforeLocationrequest=1;

_LIT(KRefPosBeforeLocationRequestName,       "refposbeforelocationrequest"); 

// Member Variable: iPowerAdviceAfterFirsRequestName, iPowerAdviceAfterSecondRequestName
// Note: Sent to AGPS Manager from NRH
// Valid Options: 
// 		20 - Off
// 		21 - On
// 		22 - Standby
_LIT(KPowerAdviceAfterFirstRequestName,  "poweradviceafterfirstrequest");
_LIT(KPowerAdviceAfterSecondRequestName, "poweradviceaftersecondrequest");

// Member Variable: iErrorCodeFromNrh
// Valid Options:
//		KErr???
_LIT(KErrorCodeFromNrhTypeName,       "posfromnrherror");

// Member Variable: iPosFromNrhType
// Valid Options:
// 		??? (one of agps, ref or fnp)
_LIT(KPosFromNrhTypeName,             "posfromnrhtype"); // i.e agps,ref or fnp

// Member Variable: iPosFromNrhLat
// Valid Options:
//		Any valid lat
_LIT(KPosFromNrhLatName,              "posfromnrhlat");

// Member Variable: iPosFromNrhLng
// Valid Options:
//		Any valid long
_LIT(KPosFromNrhLngName,              "posfromnrhlng");

// Member Variable: iPosFromNrhAcc
// Valid Options:
//		Any valid acc
_LIT(KPosFromNrhHaccName,             "posfromnrhacc");

// Member Variable: iCancelToAgpsManagerAfterSessionComplete
// Valid Options:
//
_LIT(KCancelToAgpsmanAfterSessCompleteName,       "canceltoagpsmanaftersesscomplete");

// Member Variable: iX3pTransmitPositionRequestID
// Valid Options: unsigned integer
//
_LIT(KX3pTransmitPositionRequestIDName,  "x3ptransmitpositionrequestid");


// Member Variable: iX3pTransmitPositionTransmitPriority
// Valid Options: unsigned integer
//
_LIT(KX3pTransmitPositionTransmitPriorityName,  "x3ptransmitpositiontransmitpriority");


// Member Variable: iX3pTransmitPositionAgpsAndRefPos
// Valid Options: 0 then just agps pos, 1 then both agps and ref
//
_LIT(KX3pTransmitPositionAgpsAndRefPosName,  "x3ptransmitpositionagpsrefpos");

// Member Variable: iX3pTransmitPositionTimeout
// Valid Options: unsigned integer
//
_LIT(KX3pTransmitPositionTimeoutName,  "x3ptransmitpositiontimeout");


// Member Variable: iPrivacyRequestEmergency
// Valid Options: 0 Not Emergency, 1 Emergency
//
_LIT(KPrivacyRequestEmergencyName,  "privacyrequestemergency");


// Member Variable: iPrivacyRequestAdvice
// Valid Options: 0 = ERequestAdviceNotUsed, 
//                1 = ERequestAdviceNotify,
//                2 = ERequestAdviceVerify, 
//                3 = ERequestAdviceSilent, 
//                Default = ERequestAdviceVerify
//
_LIT(KPrivacyRequestAdviceName,  "privacyrequestadvice");


// Member Variable: iPrivacyRequestDefaultAction
// Valid Options: 0 = ERequestActionNotUsed,
//                1 = ERequestActionAllow,
//                2 = ERequestActionReject,
//                Default = ERequestActionAllow
//
_LIT(KPrivacyRequestDefaultActionName,  "privacyrequestdefaultaction");


// Member Variable: iPrivacySendResponseAction
// Valid Options: 0 = ERequestUnknown,
//                1 = ERequestAccepted,
//                2 = ERequestRejected,
//                3 = ERequestIgnored
//                Default = ERequestAccepted
//
_LIT(KPrivacySendResponseActionName,  "privacysendresponseaction");


// Member Variable: iPrivacyRcvResponseAction
// Valid Options: 0 = ERequestUnknown,
//                1 = ERequestAccepted,
//                2 = ERequestRejected,
//                3 = ERequestIgnored
//                Default = ERequestAccepted
//
_LIT(KPrivacyRcvResponseActionName,  "privacyrcvresponseaction");


// Member Variable: iPrivacyCompleteError
// Valid Options: Error Code
//                Default = KErrNone
//
_LIT(KPrivacyCompleteErrorName,  "privacycompleteerror");

// Ini file key names for test cases
_LIT(KTestCaseIdKeyName,            "tc_id");

// For test step panics
_LIT(Kte_lbsnrhprivacysuitePanic,   "te_lbsnrhprivacysuite");

// Test server Uid
const TUint Kte_lbsnrhprivacysuiteUidValue = 0x1028224E;
const TUid Kte_lbsnrhprivacysuiteUid = { Kte_lbsnrhprivacysuiteUidValue };

// Default timeout while waiting for a response
const TTimeIntervalSeconds KDefaultTimeout(1);
const TTimeIntervalSeconds KDefaultDoubleTimeout(2);


#endif
