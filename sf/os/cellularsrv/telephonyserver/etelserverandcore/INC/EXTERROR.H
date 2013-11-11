// Copyright (c) 1997-2009 Nokia Corporation and/or its subsidiary(-ies).
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
 @file
 @publishedAll
 @released
*/

#ifndef __EXTERROR_H__
#define __EXTERROR_H__


//Extended Error definitions
//@publishedAll
//@released

/** Base offset for the GSM errors */
const TInt KErrEtelGsmBase = -4000;

//Radio Resource Network Errors - defined by GSM 04.08
//@publishedAll
//@released

/** Base offset for the GSM radio resource errors. */
const TInt KErrGsmRadioResourceBase=KErrEtelGsmBase;
/** Radio Resource Network Errors. See 3GPP GSM 04.08 document. */
const TInt KErrGsmRRUnspecifedAbnormalRelease=KErrGsmRadioResourceBase-1;
/** Radio Resource Network Errors. See 3GPP GSM 04.08 document. */
const TInt KErrGsmRRChannelUnacceptable=KErrGsmRadioResourceBase-2;
/** Radio Resource Network Errors. See 3GPP GSM 04.08 document. */
const TInt KErrGsmRRTimerExpired=KErrGsmRadioResourceBase-3;
/** Radio Resource Network Errors. See 3GPP GSM 04.08 document. */
const TInt KErrGsmRRNoActivityOnRadioPath=KErrGsmRadioResourceBase-4;
/** Radio Resource Network Errors. See 3GPP GSM 04.08 document. */
const TInt KErrGsmRRPreEmptiveRelease=KErrGsmRadioResourceBase-5;
/** Radio Resource Network Errors. See 3GPP GSM 04.08 document. */
const TInt KErrGsmRRCallAlreadyCleared=KErrGsmRadioResourceBase-65;
/** Radio Resource Network Errors. See 3GPP GSM 04.08 document. */
const TInt KErrGsmRRInvalidMessage=KErrGsmRadioResourceBase-95;
/** Radio Resource Network Errors. See 3GPP GSM 04.08 document. */
const TInt KErrGsmRRNonExistentMessage=KErrGsmRadioResourceBase-97;
/** Radio Resource Network Errors. See 3GPP GSM 04.08 document. */
const TInt KErrGsmRRIncompatibleMessageWithCallState=KErrGsmRadioResourceBase-98;
/** Radio Resource Network Errors. See 3GPP GSM 04.08 document. */
const TInt KErrGsmRRInvalidInformationElement=KErrGsmRadioResourceBase-100;
/** Radio Resource Network Errors. See 3GPP GSM 04.08 document. */
const TInt KErrGsmRRUnspecifiedProtocolError=KErrGsmRadioResourceBase-111;


//Mobility Management Network Errors - defined by GSM 04.08
//@publishedAll
//@released

/** Offset for the GSM mobility management errors. */
const TInt KErrGsmMobilityManagementBase=KErrGsmRadioResourceBase-128;
/** Mobility Management Network Errors.  See 3GPP GSM 04.08 document. */
const TInt KErrGsmMMUnallocatedTmsi=KErrGsmMobilityManagementBase-1;
/** Mobility Management Network Errors.  See 3GPP GSM 04.08 document. */
const TInt KErrGsmMMImsiUnknownInHlr=KErrGsmMobilityManagementBase-2;
/** Mobility Management Network Errors.  See 3GPP GSM 04.08 document. */
const TInt KErrGsmMMIllegalMs=KErrGsmMobilityManagementBase-3;
/** Mobility Management Network Errors.  See 3GPP GSM 04.08 document. */
const TInt KErrGsmMMImsiUnknownInVlr=KErrGsmMobilityManagementBase-4;
/** Mobility Management Network Errors.  See 3GPP GSM 04.08 document. */
const TInt KErrGsmMMImeiNotAccepted=KErrGsmMobilityManagementBase-5;
/** Mobility Management Network Errors.  See 3GPP GSM 04.08 document. */
const TInt KErrGsmMMIllegalMe=KErrGsmMobilityManagementBase-6;
/** Mobility Management Network Errors.  See 3GPP GSM 04.08 document. */
const TInt KErrGsmMMPlmnNotAllowed=KErrGsmMobilityManagementBase-11;
/** Mobility Management Network Errors.  See 3GPP GSM 04.08 document. */
const TInt KErrGsmMMLocationAreaNotAllowed=KErrGsmMobilityManagementBase-12;
/** Mobility Management Network Errors.  See 3GPP GSM 04.08 document. */
const TInt KErrGsmMMRoamingNotAllowedInThisLocationArea=KErrGsmMobilityManagementBase-13;
/** Mobility Management Network Errors.  See 3GPP GSM 04.08 document. */
const TInt KErrGsmMMNetworkFailure=KErrGsmMobilityManagementBase-17;
/** Mobility Management Network Errors.  See 3GPP GSM 04.08 document. */
const TInt KErrGsmMMCongestion=KErrGsmMobilityManagementBase-22;
/** Mobility Management Network Errors.  See 3GPP GSM 04.08 document. */
const TInt KErrGsmMMServiceOptionNotSupported=KErrGsmMobilityManagementBase-32;
/** Mobility Management Network Errors.  See 3GPP GSM 04.08 document. */
const TInt KErrGsmMMServiceOptionNotSubscribed=KErrGsmMobilityManagementBase-33;
/** Mobility Management Network Errors.  See 3GPP GSM 04.08 document. */
const TInt KErrGsmMMServiceOptionTemporaryOutOfOrder=KErrGsmMobilityManagementBase-34;
/** Mobility Management Network Errors.  See 3GPP GSM 04.08 document. */
const TInt KErrGsmMMCallCanNotBeIdentified=KErrGsmMobilityManagementBase-38;
/** Mobility Management Network Errors.  See 3GPP GSM 04.08 document. */
const TInt KErrGsmMMSemanticErrorInMessage=KErrGsmMobilityManagementBase-95;
/** Mobility Management Network Errors.  See 3GPP GSM 04.08 document. */
const TInt KErrGsmMMMandatoryInformationElementError=KErrGsmMobilityManagementBase-96;
/** Mobility Management Network Errors.  See 3GPP GSM 04.08 document. */
const TInt KErrGsmMMNonExistentMessageType=KErrGsmMobilityManagementBase-97;
/** Mobility Management Network Errors.  See 3GPP GSM 04.08 document. */
const TInt KErrGsmMMIncompatibleMessageWithProtocolState=KErrGsmMobilityManagementBase-98;
/** Mobility Management Network Errors.  See 3GPP GSM 04.08 document. */
const TInt KErrGsmMMNonExistentInformationElement=KErrGsmMobilityManagementBase-99;
/** Mobility Management Network Errors.  See 3GPP GSM 04.08 document. */
const TInt KErrGsmMMConditionalIEError=KErrGsmMobilityManagementBase-100;
/** Mobility Management Network Errors.  See 3GPP GSM 04.08 document. */
const TInt KErrGsmMMIncompatibleMessageWithCallState=KErrGsmMobilityManagementBase-101;
const TInt KErrGsmMMNoSuitableCellsInArea=KErrGsmMobilityManagementBase-15;
const TInt KErrGsmMMMacFailure=KErrGsmMobilityManagementBase-20;
const TInt KErrGsmMMSynchFailure=KErrGsmMobilityManagementBase-21;
const TInt KErrGsmMMGsmAuthenticationUnacceptable=KErrGsmMobilityManagementBase-23;
const TInt KErrGsmMMUnspecifiedProtocolError=KErrGsmMobilityManagementBase-111;

//Call Control Network Errors - defined by GSM 04.08
//@publishedAll
//@released

/** Base offset for the GSM call control errors. */
const TInt KErrGsmCallControlBase=KErrGsmMobilityManagementBase-128;
/** Call Control Network Errors.  See 3GPP GSM 04.08 document. */
const TInt KErrGsmCCUnassignedNumber=KErrGsmCallControlBase-1;
/** Call Control Network Errors.  See 3GPP GSM 04.08 document. */
const TInt KErrGsmCCNoRouteToTransitNetwork=KErrGsmCallControlBase-2;
/** Call Control Network Errors.  See 3GPP GSM 04.08 document. */
const TInt KErrGsmCCNoRouteToDestination=KErrGsmCallControlBase-3;
/** Call Control Network Errors.  See 3GPP GSM 04.08 document. */
const TInt KErrGsmCCChannelUnacceptable=KErrGsmCallControlBase-6;
/** Call Control Network Errors.  See 3GPP GSM 04.08 document. */
const TInt KErrGsmCCOperatorDeterminedBarring=KErrGsmCallControlBase-8;
/** Call Control Network Errors.  See 3GPP GSM 04.08 document. */
const TInt KErrGsmCCUserBusy=KErrGsmCallControlBase-17;
/** Call Control Network Errors.  See 3GPP GSM 04.08 document. */
const TInt KErrGsmCCUserNotResponding=KErrGsmCallControlBase-18;
/** Call Control Network Errors.  See 3GPP GSM 04.08 document. */
const TInt KErrGsmCCUserAlertingNoAnswer=KErrGsmCallControlBase-19;
/** Call Control Network Errors.  See 3GPP GSM 04.08 document. */
const TInt KErrGsmCCCallRejected=KErrGsmCallControlBase-21;
/** Call Control Network Errors.  See 3GPP GSM 04.08 document. */
const TInt KErrGsmCCNumberChanged=KErrGsmCallControlBase-22;
/** Call Control Network Errors.  See 3GPP GSM 04.08 document. */
const TInt KErrGsmCCNonSelectedUserClearing=KErrGsmCallControlBase-26;
/** Call Control Network Errors.  See 3GPP GSM 04.08 document. */
const TInt KErrGsmCCDestinationOutOfOrder=KErrGsmCallControlBase-27;
/** Call Control Network Errors.  See 3GPP GSM 04.08 document. */
const TInt KErrGsmCCInvalidNumberFormat=KErrGsmCallControlBase-28;
/** Call Control Network Errors.  See 3GPP GSM 04.08 document. */
const TInt KErrGsmCCFacilityRejected=KErrGsmCallControlBase-29;
/** Call Control Network Errors.  See 3GPP GSM 04.08 document. */
const TInt KErrGsmCCResponseToStatusEnquiry=KErrGsmCallControlBase-30;
/** Call Control Network Errors.  See 3GPP GSM 04.08 document. */
const TInt KErrGsmCCNormalUnspecified=KErrGsmCallControlBase-31;
/** Call Control Network Errors.  See 3GPP GSM 04.08 document. */
const TInt KErrGsmCCNoChannelAvailable=KErrGsmCallControlBase-34;
/** Call Control Network Errors.  See 3GPP GSM 04.08 document. */
const TInt KErrGsmCCNetworkOutOfOrder=KErrGsmCallControlBase-38;
/** Call Control Network Errors.  See 3GPP GSM 04.08 document. */
const TInt KErrGsmCCTemporaryFailure=KErrGsmCallControlBase-41;
/** Call Control Network Errors.  See 3GPP GSM 04.08 document. */
const TInt KErrGsmCCSwitchingEquipmentCongestion=KErrGsmCallControlBase-42;
/** Call Control Network Errors.  See 3GPP GSM 04.08 document. */
const TInt KErrGsmCCAccessInformationDiscarded=KErrGsmCallControlBase-43;
/** Call Control Network Errors.  See 3GPP GSM 04.08 document. */
const TInt KErrGsmCCRequestedChannelNotAvailable=KErrGsmCallControlBase-44;
/** Call Control Network Errors.  See 3GPP GSM 04.08 document. */
const TInt KErrGsmCCResourceNotAvailable=KErrGsmCallControlBase-47;
/** Call Control Network Errors.  See 3GPP GSM 04.08 document. */
const TInt KErrGsmCCQualityOfServiceNotAvailable=KErrGsmCallControlBase-49;
/** Call Control Network Errors.  See 3GPP GSM 04.08 document. */
const TInt KErrGsmCCRequestedFacilityNotSubscribed=KErrGsmCallControlBase-50;
/** Call Control Network Errors.  See 3GPP GSM 04.08 document. */
const TInt KErrGsmCCIncomingCallsBarredInCug=KErrGsmCallControlBase-55;
/** Call Control Network Errors.  See 3GPP GSM 04.08 document. */
const TInt KErrGsmCCBearerCapabilityNotAuthorised=KErrGsmCallControlBase-57;
/** Call Control Network Errors.  See 3GPP GSM 04.08 document. */
const TInt KErrGsmCCBearerCapabilityNotCurrentlyAvailable=KErrGsmCallControlBase-58;
/** Call Control Network Errors.  See 3GPP GSM 04.08 document. */
const TInt KErrGsmCCServiceNotAvailable=KErrGsmCallControlBase-63;
/** Call Control Network Errors.  See 3GPP GSM 04.08 document. */
const TInt KErrGsmCCBearerServiceNotImplemented=KErrGsmCallControlBase-65;
/** Call Control Network Errors.  See 3GPP GSM 04.08 document. */
const TInt KErrGsmCCChannelTypeNotImplemented=KErrGsmCallControlBase-66;
/** Call Control Network Errors.  See 3GPP GSM 04.08 document. */
const TInt KErrGsmCCAcmGreaterThanAcmMax=KErrGsmCallControlBase-68;
/** Call Control Network Errors.  See 3GPP GSM 04.08 document. */
const TInt KErrGsmCCRequestedFacilityNotImplemented=KErrGsmCallControlBase-69;
/** Call Control Network Errors.  See 3GPP GSM 04.08 document. */
const TInt KErrGsmCCOnlyRestrictedDigitalInformationBCAvailable=KErrGsmCallControlBase-70;
/** Call Control Network Errors.  See 3GPP GSM 04.08 document. */
const TInt KErrGsmCCServiceNotImplemented=KErrGsmCallControlBase-79;
/** Call Control Network Errors.  See 3GPP GSM 04.08 document. */
const TInt KErrGsmCCInvalidCallReferenceValue=KErrGsmCallControlBase-81;
/** Call Control Network Errors.  See 3GPP GSM 04.08 document. */
const TInt KErrGsmCCChannelDoesNotExist=KErrGsmCallControlBase-82;
/** Call Control Network Errors.  See 3GPP GSM 04.08 document. */
const TInt KErrGsmCCSuspendedCallExistsButCallIdentityDoesNotWork=KErrGsmCallControlBase-83;
/** Call Control Network Errors.  See 3GPP GSM 04.08 document. */
const TInt KErrGsmCCCallIdentityInUse=KErrGsmCallControlBase-84;
/** Call Control Network Errors.  See 3GPP GSM 04.08 document. */
const TInt KErrGsmCCNoCallSuspended=KErrGsmCallControlBase-85;
/** Call Control Network Errors.  See 3GPP GSM 04.08 document. */
const TInt KErrGsmCCRequestedCallIdentityAlreadyCleared=KErrGsmCallControlBase-86;
/** Call Control Network Errors.  See 3GPP GSM 04.08 document. */
const TInt KErrGsmCCUserNotInCug=KErrGsmCallControlBase-87;
/** Call Control Network Errors.  See 3GPP GSM 04.08 document. */
const TInt KErrGsmCCIncompatibleDestination=KErrGsmCallControlBase-88;
/** Call Control Network Errors.  See 3GPP GSM 04.08 document. */
const TInt KErrGsmCCInvalidTransitNetworkSelection=KErrGsmCallControlBase-91;
/** Call Control Network Errors.  See 3GPP GSM 04.08 document. */
const TInt KErrGsmCCIncompatibleSegmentedMessage=KErrGsmCallControlBase-94;
/** Call Control Network Errors.  See 3GPP GSM 04.08 document. */
const TInt KErrGsmCCSemanticallyIncorrectMessage=KErrGsmCallControlBase-95;
/** Call Control Network Errors.  See 3GPP GSM 04.08 document. */
const TInt KErrGsmCCInvalidMandatoryInformation=KErrGsmCallControlBase-96;
/** Call Control Network Errors.  See 3GPP GSM 04.08 document. */
const TInt KErrGsmCCNonExistentMessageType=KErrGsmCallControlBase-97;
/** Call Control Network Errors.  See 3GPP GSM 04.08 document. */
const TInt KErrGsmCCIncompatibleMessageInProtocolState=KErrGsmCallControlBase-98;
/** Call Control Network Errors.  See 3GPP GSM 04.08 document. */
const TInt KErrGsmCCNonExistentInformationElement=KErrGsmCallControlBase-99;
/** Call Control Network Errors.  See 3GPP GSM 04.08 document. */
const TInt KErrGsmCCConditionalIEError=KErrGsmCallControlBase-100;
/** Call Control Network Errors.  See 3GPP GSM 04.08 document. */
const TInt KErrGsmCCIncompatibleMessageInCallState=KErrGsmCallControlBase-101;
/** Call Control Network Errors.  See 3GPP GSM 04.08 document. */
const TInt KErrGsmCCRecoveryOnTimerExpiry=KErrGsmCallControlBase-102;
/** Call Control Network Errors.  See 3GPP GSM 04.08 document. */
const TInt KErrGsmCCUnspecifiedProtocolError=KErrGsmCallControlBase-111;
/** Call Control Network Errors.  See 3GPP GSM 04.08 document. */
const TInt KErrGsmCCUnspecifiedInterworkingError=KErrGsmCallControlBase-127;

/**
@publishedAll
*/
const TInt KErrGsmCCNormalCallClearing=KErrGsmCallControlBase-16; 
/**
@publishedAll
*/
const TInt KErrGsmCCPreemption=KErrGsmCallControlBase-25;

//Supplementary Services Network Errors - defined by GSM 04.80
//@publishedAll
//@released

/** Offset for the GSM supplementary services errors. */
const TInt KErrGsmSuppServiceBase=KErrGsmCallControlBase-128;
/** Supplementary Services Network Errors.  See 3GPP GSM 04.80 document. */
const TInt KErrGsmSSUnknownSubscriber=KErrGsmSuppServiceBase-1;
/** Supplementary Services Network Errors.  See 3GPP GSM 04.80 document. */
const TInt KErrGsmSSIllegalSubscriber=KErrGsmSuppServiceBase-9;
/** Supplementary Services Network Errors.  See 3GPP GSM 04.80 document. */
const TInt KErrGsmSSBearerServiceNotProvisioned=KErrGsmSuppServiceBase-10;
/** Supplementary Services Network Errors.  See 3GPP GSM 04.80 document. */
const TInt KErrGsmSSTeleserviceNotProvisioned=KErrGsmSuppServiceBase-11;
/** Supplementary Services Network Errors.  See 3GPP GSM 04.80 document. */
const TInt KErrGsmSSIllegalEquipment=KErrGsmSuppServiceBase-12;
/** Supplementary Services Network Errors.  See 3GPP GSM 04.80 document. */
const TInt KErrGsmSSCallBarred=KErrGsmSuppServiceBase-13;
/** Supplementary Services Network Errors.  See 3GPP GSM 04.80 document. */
const TInt KErrGsmSSIllegalOperation=KErrGsmSuppServiceBase-16;
/** Supplementary Services Network Errors.  See 3GPP GSM 04.80 document. */
const TInt KErrGsmSSErrorStatus=KErrGsmSuppServiceBase-17;
/** Supplementary Services Network Errors.  See 3GPP GSM 04.80 document. */
const TInt KErrGsmSSNotAvailable=KErrGsmSuppServiceBase-18;
/** Supplementary Services Network Errors.  See 3GPP GSM 04.80 document. */
const TInt KErrGsmSSSubscriptionViolation=KErrGsmSuppServiceBase-19;
/** Supplementary Services Network Errors.  See 3GPP GSM 04.80 document. */
const TInt KErrGsmSSIncompatibility=KErrGsmSuppServiceBase-20;
/** Supplementary Services Network Errors.  See 3GPP GSM 04.80 document. */
const TInt KErrGsmSSFacilityNotSupported=KErrGsmSuppServiceBase-21;
/** Supplementary Services Network Errors.  See 3GPP GSM 04.80 document. */
const TInt KErrGsmSSAbsentSubscriber=KErrGsmSuppServiceBase-27;
/** Supplementary Services Network Errors.  See 3GPP GSM 04.80 document. */
const TInt KErrGsmSSSystemFailure=KErrGsmSuppServiceBase-34;
/** Supplementary Services Network Errors.  See 3GPP GSM 04.80 document. */
const TInt KErrGsmSSDataMissing=KErrGsmSuppServiceBase-35;
/** Supplementary Services Network Errors.  See 3GPP GSM 04.80 document. */
const TInt KErrGsmSSUnexpectedDataValue=KErrGsmSuppServiceBase-36;
/** Supplementary Services Network Errors.  See 3GPP GSM 04.80 document. */
const TInt KErrGsmSSPasswordRegistrationFailure=KErrGsmSuppServiceBase-37;
/** Supplementary Services Network Errors.  See 3GPP GSM 04.80 document. */
const TInt KErrGsmSSNegativePasswordCheck=KErrGsmSuppServiceBase-38;
/** Supplementary Services Network Errors.  See 3GPP GSM 04.80 document. */
const TInt KErrGsmSSPasswordAttemptsViolation=KErrGsmSuppServiceBase-43;
/** Supplementary Services Network Errors.  See 3GPP GSM 04.80 document. */
const TInt KErrGsmSSUnknownAlphabet=KErrGsmSuppServiceBase-71;
/** Supplementary Services Network Errors.  See 3GPP GSM 04.80 document. */
const TInt KErrGsmSSUssdBusy=KErrGsmSuppServiceBase-72;
/** Supplementary Services Network Errors.  See 3GPP GSM 04.80 document. */
const TInt KErrGsmSSMaxNumMptyParticipants=KErrGsmSuppServiceBase-126;
/** Supplementary Services Network Errors.  See 3GPP GSM 04.80 document. */
const TInt KErrGsmSSResourcesUnavailable=KErrGsmSuppServiceBase-127;


//SMS Errors - defined by GSM 04.11, 03.40 and 07.05
//@publishedAll
//@released

/** Base offset for the GSM SMS errors. */
const TInt KErrGsmSmsBase=KErrGsmSuppServiceBase-128;

// 04.11
//@publishedAll
//@released

/** SMS Errors.  See GSM 04.11 document. */
const TInt KErrGsmSMSUnassignedNumber=KErrGsmSmsBase-1;
/** SMS Errors.  See GSM 04.11 document. */
const TInt KErrGsmSMSOperatorDeterminedBarring=KErrGsmSmsBase-8;
/** SMS Errors.  See GSM 04.11 document. */
const TInt KErrGsmSMSCallBarred=KErrGsmSmsBase-10;
/** SMS Errors.  See GSM 04.11 document. */
const TInt KErrGsmSMSReserved=KErrGsmSmsBase-11;
/** SMS Errors.  See GSM 04.11 document. */
const TInt KErrGsmSMSNetworkFailure=KErrGsmSmsBase-17;
/** SMS Errors.  See GSM 04.11 document. */
const TInt KErrGsmSMSShortMessageTransferRejected=KErrGsmSmsBase-21;
/** SMS Errors.  See GSM 04.11 document. */
const TInt KErrGsmSMSMemoryCapacityExceeded=KErrGsmSmsBase-22;
/** SMS Errors.  See GSM 04.11 document. */
const TInt KErrGsmSMSDestinationOutOfOrder=KErrGsmSmsBase-27;
/** SMS Errors.  See GSM 04.11 document. */
const TInt KErrGsmSMSUnidentifiedSubscriber=KErrGsmSmsBase-28;
/** SMS Errors.  See GSM 04.11 document. */
const TInt KErrGsmSMSFacilityRejected=KErrGsmSmsBase-29;
/** SMS Errors.  See GSM 04.11 document. */
const TInt KErrGsmSMSUnknownSubscriber=KErrGsmSmsBase-30;
/** SMS Errors.  See GSM 04.11 document. */
const TInt KErrGsmSMSNetworkOutOfOrder=KErrGsmSmsBase-38;
/** SMS Errors.  See GSM 04.11 document. */
const TInt KErrGsmSMSTemporaryFailure=KErrGsmSmsBase-41;
/** SMS Errors.  See GSM 04.11 document. */
const TInt KErrGsmSMSCongestion=KErrGsmSmsBase-42;
/** SMS Errors.  See GSM 04.11 document. */
const TInt KErrGsmSMSResourcesUnavailable=KErrGsmSmsBase-47;
/** SMS Errors.  See GSM 04.11 document. */
const TInt KErrGsmSMSRequestedFacilityNotSubscribed=KErrGsmSmsBase-50;
/** SMS Errors.  See GSM 04.11 document. */
const TInt KErrGsmSMSRequestedFacilityNotImplemented=KErrGsmSmsBase-69;
/** SMS Errors.  See GSM 04.11 document. */
const TInt KErrGsmSMSInvalidShortMessageTransferReferenceValue=KErrGsmSmsBase-81;
/** SMS Errors.  See GSM 04.11 document. */
const TInt KErrGsmSMSUnspecifiedInvalidMessage=KErrGsmSmsBase-95;
/** SMS Errors.  See GSM 04.11 document. */
const TInt KErrGsmSMSInvalidMandatoryInformation=KErrGsmSmsBase-96;
/** SMS Errors.  See GSM 04.11 document. */
const TInt KErrGsmSMSNonExistentMessageType=KErrGsmSmsBase-97;
/** SMS Errors.  See GSM 04.11 document. */
const TInt KErrGsmSMSIncompatibleMessageWithSmsProtocolState=KErrGsmSmsBase-98;
/** SMS Errors.  See GSM 04.11 document. */
const TInt KErrGsmSMSInformationElementNotImplemented=KErrGsmSmsBase-99;
/** SMS Errors.  See GSM 04.11 document. */
const TInt KErrGsmSMSUnspecifiedProtocolError=KErrGsmSmsBase-111;
/** SMS Errors.  See GSM 04.11 document. */
const TInt KErrGsmSMSUnspecifiedInterworkingError=KErrGsmSmsBase-127;

// 03.40
//@publishedAll
//@released

/** Please see the GSM 03.40 document. */
const TInt KErrGsmSMSTelematicInterworkingNotSupported=KErrGsmSmsBase-128;
/** Please see the GSM 03.40 document. */
const TInt KErrGsmSMSShortMessageType0NotSupported=KErrGsmSmsBase-129;
/** Please see the GSM 03.40 document. */
const TInt KErrGsmSMSCannotReplaceShortMessage=KErrGsmSmsBase-130;
/** Please see the GSM 03.40 document. */
const TInt KErrGsmSMSUnspecifiedPIDError=KErrGsmSmsBase-143;
/** Please see the GSM 03.40 document. */
const TInt KErrGsmSMSDataCodingSchemeNotSupported=KErrGsmSmsBase-144;
/** Please see the GSM 03.40 document. */
const TInt KErrGsmSMSMessageClassNotSupported=KErrGsmSmsBase-145;
/** Please see the GSM 03.40 document. */
const TInt KErrGsmSMSUnspecifiedDCSError=KErrGsmSmsBase-159;
/** Please see the GSM 03.40 document. */
const TInt KErrGsmSMSCommandCannotBeActioned=KErrGsmSmsBase-160;
/** Please see the GSM 03.40 document. */
const TInt KErrGsmSMSCommandNotSupported=KErrGsmSmsBase-161;
/** Please see the GSM 03.40 document. */
const TInt KErrGsmSMSUnspecifiedCommandError=KErrGsmSmsBase-175;
/** Please see the GSM 03.40 document. */
const TInt KErrGsmSMSTpduNotSupported=KErrGsmSmsBase-176;
/** Please see the GSM 03.40 document. */
const TInt KErrGsmSMSServiceCentreBusy=KErrGsmSmsBase-192;
/** Please see the GSM 03.40 document. */
const TInt KErrGsmSMSNoSCSubscription=KErrGsmSmsBase-193;
/** Please see the GSM 03.40 document. */
const TInt KErrGsmSMSSCSystemFailure=KErrGsmSmsBase-194;
/** Please see the GSM 03.40 document. */
const TInt KErrGsmSMSInvalidSMEAddress=KErrGsmSmsBase-195;
/** Please see the GSM 03.40 document. */
const TInt KErrGsmSMSDestinationSMEBarred=KErrGsmSmsBase-196;
/** Please see the GSM 03.40 document. */
const TInt KErrGsmSMSDuplicateSM=KErrGsmSmsBase-197;
/** Please see the GSM 03.40 document. */
const TInt KErrGsmSMSTPVPFNotSupported=KErrGsmSmsBase-198;
/** Please see the GSM 03.40 document. */
const TInt KErrGsmSMSTPVPNotSupported=KErrGsmSmsBase-199;
/** Please see the GSM 03.40 document. */
const TInt KErrGsmSMSSimSMSStorageFull=KErrGsmSmsBase-208;
/** Please see the GSM 03.40 document. */
const TInt KErrGsmSMSNoSMSStorageCapabilityInSim=KErrGsmSmsBase-209;
/** Please see the GSM 03.40 document. */
const TInt KErrGsmSMSErrorInMS=KErrGsmSmsBase-210;
/** Please see the GSM 03.40 document. */
const TInt KErrGsmSMSMemCapacityExceeded=KErrGsmSmsBase-211;
/** Please see the GSM 03.40 document. */
const TInt KErrGsmSMSSimAppToolkitBusy=KErrGsmSmsBase-212;
/** Please see the GSM 03.40 document. */
const TInt KErrGsmSMSUnspecifiedErrorCause=KErrGsmSmsBase-255;

// 07.05
//@publishedAll
//@released

/** Please see the GSM 07.05 document. */
const TInt KErrGsmSMSFailureInME=KErrGsmSmsBase-300;
/** Please see the GSM 07.05 document. */
const TInt KErrGsmSMSServiceOfMSReserved=KErrGsmSmsBase-301;
/** Please see the GSM 07.05 document. */
const TInt KErrGsmSMSOperationNotAllowed=KErrGsmSmsBase-302;
/** Please see the GSM 07.05 document. */
const TInt KErrGsmSMSOperationNotSupported=KErrGsmSmsBase-303;
/** Please see the GSM 07.05 document. */
const TInt KErrGsmSMSInvalidPDUModeParameter=KErrGsmSmsBase-304;
/** Please see the GSM 07.05 document. */
const TInt KErrGsmSMSInvalidTextModeParameter=KErrGsmSmsBase-305;
/** Please see the GSM 07.05 document. */
const TInt KErrGsmSMSSimNotInserted=KErrGsmSmsBase-310;
/** Please see the GSM 07.05 document. */
const TInt KErrGsmSMSSimPin1Required=KErrGsmSmsBase-311;
/** Please see the GSM 07.05 document. */
const TInt KErrGsmSMSPhoneToSimLockRequired=KErrGsmSmsBase-312;
/** Please see the GSM 07.05 document. */
const TInt KErrGsmSMSSimFailure=KErrGsmSmsBase-313;
/** Please see the GSM 07.05 document. */
const TInt KErrGsmSMSSimBusy=KErrGsmSmsBase-314;
/** Please see the GSM 07.05 document. */
const TInt KErrGsmSMSSimWrong=KErrGsmSmsBase-315;
/** Please see the GSM 07.05 document. */
const TInt KErrGsmSMSSimPuk1Required=KErrGsmSmsBase-316;
/** Please see the GSM 07.05 document. */
const TInt KErrGsmSMSPin2Required=KErrGsmSmsBase-317;
/** Please see the GSM 07.05 document. */
const TInt KErrGsmSMSPuk2Required=KErrGsmSmsBase-318;
/** Please see the GSM 07.05 document. */
const TInt KErrGsmSMSMemoryFailure=KErrGsmSmsBase-320;
/** Please see the GSM 07.05 document. */
const TInt KErrGsmSMSInvalidMemoryIndex=KErrGsmSmsBase-321;
/** Please see the GSM 07.05 document. */
const TInt KErrGsmSMSMemoryFull=KErrGsmSmsBase-322;
/** Please see the GSM 07.05 document. */
const TInt KErrGsmSMSUnknownSCAddress=KErrGsmSmsBase-330;
/** Please see the GSM 07.05 document. */
const TInt KErrGsmSMSNoNetworkService=KErrGsmSmsBase-331;
/** Please see the GSM 07.05 document. */
const TInt KErrGsmSMSNetworkTimeout=KErrGsmSmsBase-332;
/** Please see the GSM 07.05 document. */
const TInt KErrGsmSMSCnmaAckNotExpected=KErrGsmSmsBase-340;
/** Please see the GSM 07.05 document. */
const TInt KErrGsmSMSUnknownError=KErrGsmSmsBase-500;


//General TE-TA Errors - defined by GSM 07.07
//@publishedAll
//@released

/** Base offset for the general TE-TA Errors - defined by GSM 07.07. */
const TInt KErrGsm0707Base=KErrGsmSmsBase-512;
/** General TE-TA Errors. See GSM 07.07 document. */
const TInt KErrGsm0707PhoneFailure=KErrGsm0707Base;
/** General TE-TA Errors. See GSM 07.07 document. */
const TInt KErrGsm0707NoConnectionToPhone=KErrGsm0707Base-1;
/** General TE-TA Errors. See GSM 07.07 document. */
const TInt KErrGsm0707PhoneLinkReserved=KErrGsm0707Base-2;
/** General TE-TA Errors. See GSM 07.07 document. */
const TInt KErrGsm0707OperationNotAllowed=KErrGsm0707Base-3;
/** General TE-TA Errors. See GSM 07.07 document. */
const TInt KErrGsm0707OperationNotSupported=KErrGsm0707Base-4;
/** General TE-TA Errors. See GSM 07.07 document. */
const TInt KErrGsm0707PhoneToSimLockRequired=KErrGsm0707Base-5;
/** General TE-TA Errors. See GSM 07.07 document. */
const TInt KErrGsm0707SimNotInserted=KErrGsm0707Base-10;
/** General TE-TA Errors. See GSM 07.07 document. */
const TInt KErrGsm0707SimPin1Required=KErrGsm0707Base-11;
/** General TE-TA Errors. See GSM 07.07 document. */
const TInt KErrGsm0707SIMPuk1Required=KErrGsm0707Base-12;
/** General TE-TA Errors. See GSM 07.07 document. */
const TInt KErrGsm0707SimFailure=KErrGsm0707Base-13;
/** General TE-TA Errors. See GSM 07.07 document. */
const TInt KErrGsm0707SimBusy=KErrGsm0707Base-14;
/** General TE-TA Errors. See GSM 07.07 document. */
const TInt KErrGsm0707SimWrong=KErrGsm0707Base-15;
/** General TE-TA Errors. See GSM 07.07 document. */
const TInt KErrGsm0707IncorrectPassword=KErrGsm0707Base-16;
/** General TE-TA Errors. See GSM 07.07 document. */
const TInt KErrGsm0707SimPin2Required=KErrGsm0707Base-17;
/** General TE-TA Errors. See GSM 07.07 document. */
const TInt KErrGsm0707SIMPuk2Required=KErrGsm0707Base-18;
/** General TE-TA Errors. See GSM 07.07 document. */
const TInt KErrGsm0707MemoryFull=KErrGsm0707Base-20;
/** General TE-TA Errors. See GSM 07.07 document. */
const TInt KErrGsm0707InvalidIndex=KErrGsm0707Base-21;
/** General TE-TA Errors. See GSM 07.07 document. */
const TInt KErrGsm0707NotFound=KErrGsm0707Base-22;
/** General TE-TA Errors. See GSM 07.07 document. */
const TInt KErrGsm0707MemoryFailure=KErrGsm0707Base-23;
/** General TE-TA Errors. See GSM 07.07 document. */
const TInt KErrGsm0707TextStringTooLong=KErrGsm0707Base-24;
/** General TE-TA Errors. See GSM 07.07 document. */
const TInt KErrGsm0707InvalidCharsInTextString=KErrGsm0707Base-25;
/** General TE-TA Errors. See GSM 07.07 document. */
const TInt KErrGsm0707DialStringTooLong=KErrGsm0707Base-26;
/** General TE-TA Errors. See GSM 07.07 document. */
const TInt KErrGsm0707InvalidCharsInDialString=KErrGsm0707Base-27;
/** General TE-TA Errors. See GSM 07.07 document. */
const TInt KErrGsm0707NoNetworkService=KErrGsm0707Base-30;
/** General TE-TA Errors. See GSM 07.07 document. */
const TInt KErrGsm0707NetworkTimeout=KErrGsm0707Base-31;
/** General TE-TA Errors. See GSM 07.07 document. */
const TInt KErrGsm0707UnknownError=KErrGsm0707Base-100;

//
// General ICC phonebook write errors.
// @publishedAll
// @released
//
/** Base offset for the general ICC phonebook write errors. */
const TInt KErrPhonebookBase=-3551; // 55 values are available for use!
/** Main number is too long and no free extension records are available. */
const TInt KErrPhonebookNumberOverflow=KErrPhonebookBase;                                           // -3551
/** Main number is too long and no extension file exists. */
const TInt KErrPhonebookNumberNoMemory=KErrPhonebookBase-1;                                         // -3552
/** Main number is corrupt or not valid. */
const TInt KErrPhonebookNumberCorrupt=KErrPhonebookBase-2;                                          // -3553
/** Main alpha string too long. */
const TInt KErrPhonebookTextOverflow=KErrPhonebookBase-3;                                           // -3554
/** Main alpha string is corrupt or not valid. */
const TInt KErrPhonebookTextCorrupt=KErrPhonebookBase-4;                                            // -3555
/** Capability entry for does not fit. */
const TInt KErrPhonebookCapabilityOverflow=KErrPhonebookBase-5;                                     // -3556
/** No more capability entries. */
const TInt KErrPhonebookCapabilityNoMemory=KErrPhonebookBase-6;                                     // -3557
/** Capability file is corrupt. */
const TInt KErrPhonebookCapabilityCorrupt=KErrPhonebookBase-7;                                      // -3558
/** No space in the phonebook to store the entry. */
const TInt KErrPhonebookNoMemory=KErrPhonebookBase-8;                                               // -3559
/** Phonebook is corrupt. */
const TInt KErrPhonebookCorrupt=KErrPhonebookBase-9;                                                // -3560
/** Second name alpha string too long. */
const TInt KErrPhonebookSecondNameTextOverflow=KErrPhonebookBase-10;                                // -3561
/** The Second Name file has no more free entries or is missing. */
const TInt KErrPhonebookSecondNameNoMemory=KErrPhonebookBase-11;                                    // -3562
/** Second Name text is corrupt. */
const TInt KErrPhonebookSecondNameCorrupt=KErrPhonebookBase-12;                                     // -3563
/** Phone Book Control is missing. */
const TInt KErrPhonebookControlNoMemory=KErrPhonebookBase-13;                                       // -3564
/** Phone Book Control is corrupt. */
const TInt KErrPhonebookControlCorrupt=KErrPhonebookBase-14;                                        // -3565
/** Group name string too long. */
const TInt KErrPhonebookGroupAlphaTextOverflow=KErrPhonebookBase-15;                                // -3566
/** Group file has no more free entries or is missing. */
const TInt KErrPhonebookGroupAlphaTextNoMemory=KErrPhonebookBase-16;                                // -3567
/** Group name text is corrupt/not valid. */
const TInt KErrPhonebookGroupAlphaTextCorrupt=KErrPhonebookBase-17;                                 // -3568
/** Group file does not exist. */
const TInt KErrPhonebookGroupNoMemory=KErrPhonebookBase-18;                                         // -3569
/** Group file cannot store any more identifiers. */
const TInt KErrPhonebookGroupOverflow=KErrPhonebookBase-19;                                         // -3570
/** Group file is missing or corrupt. */
const TInt KErrPhonebookGroupCorrupt=KErrPhonebookBase-20;                                          // -3571
/** Addition number alpha string too long. */
const TInt KErrPhonebookAdditionalNumberAlphaTextOverflow=KErrPhonebookBase-21;                     // -3572
/** Addition Alpha String file has no more free entries or is missing. */
const TInt KErrPhonebookAdditionalNumberAlphaTextNoMemory=KErrPhonebookBase-22;                     // -3573
/** Additional number text is corrupt or not valid. */
const TInt KErrPhonebookAdditionalNumberAlphaTextCorrupt=KErrPhonebookBase-23;                      // -3574
/** Additional number is too long and there are no free extension records. */
const TInt KErrPhonebookAdditionalNumberNumberOverflow=KErrPhonebookBase-24;                        // -3575
/** Additional number is too long and no extension file exists. */
const TInt KErrPhonebookAdditionalNumberNumberNoMemory=KErrPhonebookBase-25;                        // -3576
/** Additional number is corrupt or not valid. */
const TInt KErrPhonebookAdditionalNumberNumberCorrupt=KErrPhonebookBase-26;                         // -3577
/** Capability entry for the additional number does not fit. */
const TInt KErrPhonebookAdditionalNumberCapabilityOverflow=KErrPhonebookBase-27;                    // -3578
/** No more capability entries exist for the addition number. */
const TInt KErrPhonebookAdditionalNumberCapabilityNoMemory=KErrPhonebookBase-28;                    // -3579
/** The capability file is corrupt. */
const TInt KErrPhonebookAdditionalNumberCapabilityCorrupt=KErrPhonebookBase-29;                     // -3580
/** No more entries exist to store additional number. */
const TInt KErrPhonebookAdditionalNumberNoMemory=KErrPhonebookBase-30;                              // -3581
/** Additional number is corrupt. */
const TInt KErrPhonebookAdditionalNumberCorrupt=KErrPhonebookBase-31;                               // -3582
/** Email alpha string too long. */
const TInt KErrPhonebookEmailTextOverflow=KErrPhonebookBase-32;                                     // -3583
/** Email file has no more free entries or is missing. */
const TInt KErrPhonebookEmailNoMemory=KErrPhonebookBase-33;                                         // -3584
/** Email file text is corrupt or not valid. */
const TInt KErrPhonebookEmailCorrupt=KErrPhonebookBase-34;                                          // -3585
/** Phone Book Reference (PBR) file is corrupt. */
const TInt KErrPhonebookPBRCorrupt=KErrPhonebookBase-35;                                            // -3586


#endif
