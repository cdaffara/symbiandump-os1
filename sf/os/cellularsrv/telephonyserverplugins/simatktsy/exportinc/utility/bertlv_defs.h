/*
* Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Name        : BerTlv_defs.h
* Part of     : Series 60 / utility
* Based on 3GPP TS 11.14 V8.8.0 3GPP TS 11.14 V8.8.0 
* Version     : 1.0
*
*/



//  INCLUDES
#ifndef BERTLV_DEFS_H
#define BERTLV_DEFS_H

#include <etelsat.h>    // Symbian Etel Sat related types

//  CONSTANTS 
const TUint8 KTlvLengthStartPosition                            = 0x01;
const TUint8 KTlvDataAreaStartPosition                          = 0x02;
const TUint8 KTlvHeaderLength                                   = 0x02;
const TUint8 KTwoByteLengthCoding                               = 0x81;
const TUint8 KMccAndMncLength                                   = 0x03;
const TUint8 KLocationAreaCodeLength                            = 0x02;
const TUint8 KCApduLengthWithoutLc                              = 0x06;
const TUint8 KCommonTlvHeaderLength                             = 0x02;
const TUint8 KDefaultCommandNumber                              = 0x01;
const TUint8 KDefaultGetInkeyCodingScheme                       = 0x04;
const TUint8 KTagValueMask                                      = 0x7f;
const TUint8 KTagCrMask                                         = 0x80;
const TUint8 KMaxOneByteLength                                  = 0x7f;
const TUint8 KReservedTonNpi1 									= 0x7F;
const TUint8 KReservedTonNpi2 									= 0xF7;


// In 242 octets, it is possible to pack (242x8)/7=277 characters.
// See ref. 3GPP TS 23.038.
const TInt16 KTextBufferMaxSize                                 = 277;
const TUint8 KCommandQualifier                                  = 4;
const TUint8 KTlvMaxSize                                        = 255;

// Proactive SIM commands
const TUint8 KRefresh                                           = 0x01;
const TUint8 KMoreTime                                          = 0x02;
const TUint8 KPollInterval                                      = 0x03;
const TUint8 KPollingOff                                        = 0x04;
const TUint8 KSetUpEventList                                    = 0x05;
const TUint8 KSetUpCall                                         = 0x10;
const TUint8 KSendSs                                            = 0x11;
const TUint8 KSendUssd                                          = 0x12;
const TUint8 KSendShortMessage                                  = 0x13;
const TUint8 KSendDtmf                                          = 0x14;
const TUint8 KLaunchBrowser                                     = 0x15;
const TUint8 KPlayTone                                          = 0x20;
const TUint8 KDisplayText                                       = 0x21;
const TUint8 KGetInkey                                          = 0x22;
const TUint8 KGetInput                                          = 0x23;
const TUint8 KSelectItem                                        = 0x24;
const TUint8 KSetUpMenu                                         = 0x25;
const TUint8 KProvideLocalInformation                           = 0x26;
const TUint8 KTimerManagement                                   = 0x27;
const TUint8 KSetUpIdleModeText                                 = 0x28;
const TUint8 KPerformCardApdu                                   = 0x30;
const TUint8 KPowerOnCard                                       = 0x31;
const TUint8 KPowerOffCard                                      = 0x32;
const TUint8 KGetReaderStatus                                   = 0x33;
const TUint8 KRunAtCommand                                      = 0x34;
const TUint8 KLanguageNotification                              = 0x35;
const TUint8 KOpenChannel                                       = 0x40;
const TUint8 KCloseChannel                                      = 0x41;
const TUint8 KReceiveData                                       = 0x42;
const TUint8 KSendData                                          = 0x43;
const TUint8 KGetChannelStatus                                  = 0x44;
const TUint8 KEndOfTheProactiveSession                          = 0x81;


// TAG Values
// BER-TLV tags in ME to SIM direction
const TUint8 KBerTlvSmsPpDownloadTag                            = 0xD1;
const TUint8 KBerTlvCellBroadcastTag                            = 0xD2;
const TUint8 KBerTlvMenuSelectionTag                            = 0xD3;
const TUint8 KBerTlvCallControlTag                              = 0xD4;
const TUint8 KBerTlvMoShortMessageControlTag                    = 0xD5;
const TUint8 KBerTlvEventDownloadTag                            = 0xD6;
const TUint8 KBerTlvTimerExpirationTag                          = 0xD7;

// BER-TLV tags in SIM to ME direction
const TUint8 KBerTlvProactiveSimCommandTag                      = 0xD0;

// SIMPLE-TLV tags in both directions
// TLV tags, Tag value, bits 1-7 (Range: '01' - '7E')
const TUint8 KTlvCommandDetailsTag                              = 0x01;
const TUint8 KTlvDeviceIdentityTag                              = 0x02;
const TUint8 KTlvResultTag                                      = 0x03;
const TUint8 KTlvDurationTag                                    = 0x04;
const TUint8 KTlvAlphaIdentifierTag                             = 0x05;
const TUint8 KTlvAddressTag                                     = 0x06;
const TUint8 KTlvCapabilityConfigurationParametersTag           = 0x07;
const TUint8 KTlvSubaddressTag                                  = 0x08;
const TUint8 KTlvSsStringTag                                    = 0x09;
const TUint8 KTlvUssdStringTag                                  = 0x0A;
const TUint8 KTlvSmsTpduTag                                     = 0x0B;
const TUint8 KTlvCellBroadcastPageTag                           = 0x0C;
const TUint8 KTlvTextStringTag                                  = 0x0D;
const TUint8 KTlvToneTag                                        = 0x0E;
const TUint8 KTlvItemTag                                        = 0x0F;
const TUint8 KTlvItemIdentifierTag                              = 0x10;
const TUint8 KTlvResponseLengthTag                              = 0x11;
const TUint8 KTlvFileListTag                                    = 0x12;
const TUint8 KTlvLocationInformationTag                         = 0x13;
const TUint8 KTlvImeiTag                                        = 0x14;
const TUint8 KTlvHelpRequestTag                                 = 0x15;
const TUint8 KTlvNetworkMeasurementResultsTag                   = 0x16;
const TUint8 KTlvDefaultTextTag                                 = 0x17;
const TUint8 KTlvItemsNextActionIndicatorTag                    = 0x18;
const TUint8 KTlvEventListTag                                   = 0x19;
const TUint8 KTlvCauseTag                                       = 0x1A;
const TUint8 KTlvLocationStatusTag                              = 0x1B;
const TUint8 KTlvTransactionIdentifierTag                       = 0x1C;
const TUint8 KTlvBccdChannelListTag                             = 0x1D;
const TUint8 KTlvIconIdentifierTag                              = 0x1E;
const TUint8 KTlvItemIconIdentifierListTag                      = 0x1F;
const TUint8 KTlvCardReaderStatusTag                            = 0x20;
const TUint8 KTlvCardAtrTag                                     = 0x21;
const TUint8 KTlvCApduTag                                       = 0x22;
const TUint8 KTlvRApduTag                                       = 0x23;
const TUint8 KTlvTimerIdentifierTag                             = 0x24;
const TUint8 KTlvTimerValueTag                                  = 0x25;
const TUint8 KTlvDateTimeAndTimeZoneTag                         = 0x26;
const TUint8 KTlvCallControlRequestedActionTag                  = 0x27;
const TUint8 KTlvAtCommandTag                                   = 0x28;
const TUint8 KTlvAtResponseTag                                  = 0x29;
const TUint8 KTlvBcRepeatIndicatorTag                           = 0x2A;
const TUint8 KTlvImmediateResponseTag                           = 0x2B;
const TUint8 KTlvDtmfStringTag                                  = 0x2C;
const TUint8 KTlvLanguageTag                                    = 0x2D;
const TUint8 KTlvTimingAdvanceTag                               = 0x2E;
const TUint8 KTlvAIDTag                                         = 0x2F;
const TUint8 KTlvBrowserIdentityTag                             = 0x30;
const TUint8 KTlvUrlTag                                         = 0x31;
const TUint8 KTlvBearerTag                                      = 0x32;
const TUint8 KTlvProvisioningReferenceFileTag                   = 0x33;
const TUint8 KTlvBrowserTerminationCauseTag                     = 0x34;
const TUint8 KTlvBearerDescriptionTag                           = 0x35;
const TUint8 KTlvChannelDataTag                                 = 0x36;
const TUint8 KTlvChannelDataLengthTag                           = 0x37;
const TUint8 KTlvChannelStatusTag                               = 0x38;
const TUint8 KTlvBufferSizeTag                                  = 0x39;
const TUint8 KTlvCardReaderIdentifierTag                        = 0x3A;
const TUint8 KTlvTextStringTagUserPassword                      = 0x3B;
const TUint8 KTlvSimMeTransportLevelTag                         = 0x3C;
const TUint8 KTlvURLTag                                         = 0x3D;
const TUint8 KTlvOtherAddressTag                                = 0x3E;
const TUint8 KTlvAccessTechnologyTag                            = 0x3F;
const TUint8 KTlvESNTag                                         = 0x46;
const TUint8 KTlvNetworkAccessNameTag                           = 0x47;
const TUint8 KTlvTimerIdentifierTagCRBitOn                      = 0xA4;

// TLV Coding

// Event download - Event list event coding
const TUint8 KEventMTCall                                       = 0x00;
const TUint8 KEventCallConnected                                = 0x01;
const TUint8 KEventCallDisconnected                             = 0x02;
const TUint8 KEventLocationStatus                               = 0x03;
const TUint8 KEventUserActivity                                 = 0x04;
const TUint8 KEventIdleScreenAvailable                          = 0x05;
const TUint8 KEventCardReaderStatus                             = 0x06;
const TUint8 KEventLanguageSelection                            = 0x07;
const TUint8 KEventBrowserTermination                           = 0x08;
const TUint8 KEventDataAvailable                                = 0x09;
const TUint8 KEventChannelStatus                                = 0x0A;
const TUint8 KEventAccessTechnologyChange                       = 0x0B;

// Event download - Location status coding
const TUint8 KLocationStatusNormalService                       = 0x00;
const TUint8 KLocationStatusLimitedService                      = 0x01;
const TUint8 KLocationStatusNoService                           = 0x02;

// Event download - Browser termination coding
const TUint8 KBrowserTerminationUser                            = 0x00;
const TUint8 KBrowserTerminationError                           = 0x01;

// REFRESH;
const TUint8 KSimInitFullFileChangeNotification                 = 0x00;
const TUint8 KFileChangeNotification                            = 0x01;
const TUint8 KSimInitFileChangeNotification                     = 0x02;
const TUint8 KSimInit                                           = 0x03;
const TUint8 KSimReset                                          = 0x04;
const TUint8 KUSIMApplicationReset                              = 0x05;
const TUint8 K3GSessionReset                                    = 0x06;
const TUint8 KMaxPathLength                                     = 16;

// SET UP CALL;
const TUint8 KSetUpCallIfNotCurrentlyBusy                       = 0x00;
const TUint8 KSetUpCallIfNotCurrentlyBusyWithRedial             = 0x01;
const TUint8 KSetUpCallPuttingOtherCallsOnHold                  = 0x02;
const TUint8 KSetUpCallPuttingOtherCallsOnHoldWithRedial        = 0x03;
const TUint8 KSetUpCallDisconnectingOtherCalls                  = 0x04;
const TUint8 KSetUpCallDisconnectingOtherCallsWithRedial        = 0x05;
const TUint8 KAuthorityFormatIdentifier                         = 0x50;
const TUint8 KMinutes                                           = 0x00;
const TUint8 KSeconds                                           = 0x01;
const TUint8 KTenthsOfSeconds                                   = 0x02;

// SEND SHORT MESSAGE;
const TUint8 KSmsPackingRequiredMask                            = 0x01;
const TUint8 KDCSAlphabetMask                                   = 0x06;
const TUint8 KSmsMaxSize                                        = 160;
const TUint8 KSmsMaxSizeWithoutPacking                          = 140;
const TUint8 KSMSCommandMaxSize                                 = 152;

// DISPLAY TEXT,
const TUint8 KPriorityMask                                      = 0x01;
const TUint8 KMessageClearMask                                  = 0x80;

// GET INKEY,
const TUint8 KAlphabetMask                                      = 0x01;
const TUint8 KUCS2AlphabetMask                                  = 0x02;
const TUint8 KYesNoMask                                         = 0x04;
const TUint8 KImmediateDigitResponse                            = 0x08;
const TUint8 KHelpAvailabilityMask                              = 0x80;

// GET INPUT,
const TUint8 KUserInputMask                                     = 0x04;
const TUint8 KGetInputSmsPackedMask                             = 0x08;
const TUint8 KUCS2DCS                                           = 0x08;
const TUint8 K7BitDefaultSmsDCS                                 = 0x00;
const TUint8 K8BitDCS                                           = 0x04;

// SELECT ITEM.
const TUint8 KSelectItemPresentationTypeMask                    = 0x03;
const TUint8 KSelectItemChoiceOfData                            = 0x01;
const TUint8 KSelectItemChoiceOfNavigation                      = 0x03;
const TUint8 KSelectItemSelectUsingSoftKeyMask                  = 0x04;

// SET UP MENU.
const TUint8 KSelectionUsingSoftKey                             = 0x01;

// TIMER MANAGEMENT
const TUint8 KDeactivateTimer                                   = 0x01;
const TUint8 KGetTimerValue                                     = 0x02;

// LAUNCH BROWSER
const TUint8 KLaunchBrowserIfNotYetLaunched                     = 0x00;
const TUint8 KUseExistingBrowser                                = 0x02;
const TUint8 KCloseAndLaunchNewBrowser                          = 0x03;

// PROVIDE LOCAL INFORMATION
const TUint8 KNetworkModeGsm                                    = 0x00;
const TUint8 KNetworkModeUtran                                  = 0x03;

// BEARER INDEPENDENT PROTOCOL
const TUint8 KIPV4                                              = 0x21;
const TUint8 KIPV6                                              = 0x57;

// MO SM CONTROL
const TUint8 KMoSmCtrlMaxLengthAddr1Addr2						= 249;

// General result
// All other values are reserved.
// Results '0X' and '1X' indicate that the command has been performed.
const TUint8 KCommandPerformedSuccessfully                              = 0x00;
const TUint8 KCommandPerformedWithPartialComprehension                  = 0x01;
const TUint8 KCommandPerformedWithMissingInformation                    = 0x02;
const TUint8 KRefreshPerformedWithAdditionalEfsRead                     = 0x03;
const TUint8 KCommandPerformedSuccessfullyIconCouldNotBeDisplayed       = 0x04;
const TUint8 KCommandPerformedButModifiedByCallControlBySim             = 0x05;
const TUint8 KCommandPerformedSuccessfullyLimitedService                = 0x06;
const TUint8 KCommandPerformedWithModification                          = 0x07;
const TUint8 KProactiveSimSessionTerminatedByTheUser                    = 0x10;
const TUint8 KBackwardMoveInTheProactiveSimSessionRequestedByTheUser    = 0x11;
const TUint8 KNoResponseFromUser                                        = 0x12;
const TUint8 KHelpInformationRequiredByTheUser                          = 0x13;
const TUint8 KUssdOrSsTransactionTerminatedByTheUser                    = 0x14;

// Results '2X' Indicate To The SIM That It May Be Worth Re-Trying The Command 
// At A Later Opportunity.
const TUint8 KMeCurrentlyUnableToProcessCommand                         = 0x20;
const TUint8 KNetworkCurrentlyUnableToProcessCommand                    = 0x21;
const TUint8 KUserDidNotAcceptTheProactiveCommand                       = 0x22;
const TUint8 KUserClearedDownCallBeforeConnectionOrNetworkRelease       = 0x23;
const TUint8 KActionInContradictionWithTheCurrentTimerState             = 0x24;
const TUint8 KInteractionWithCallControlBySimTemporaryProblem           = 0x25;
const TUint8 KLaunchBrowserGenericErrorCode                             = 0x26;

// Results '3X' Indicate That It Is Not Worth The SIM Re-Trying With An 
// Identical Command, As It Will Only Get The Same Response. However, The 
// Decision To Retry Lies With The SIM Application.
const TUint8 KCommandBeyondMeCapabilities                               = 0x30;
const TUint8 KCommandTypeNotUnderstoodByMe                              = 0x31;
const TUint8 KCommandDataNotUnderstoodByMe                              = 0x32;
const TUint8 KCommandNumberNotKnownByMe                                 = 0x33;
const TUint8 KSsReturnError                                             = 0x34;
const TUint8 KSmsRpError                                                = 0x35;
const TUint8 KErrorRequiredValuesAreMissing                             = 0x36;
const TUint8 KUssdReturnError                                           = 0x37;
const TUint8 KMultiplecardCommandsError                                 = 0x38;
const TUint8 KSimPermanentProblem                                       = 0x39;
const TUint8 KBearerIndependentProtocolError                            = 0x3a;

// Additional information
// Contents: For the general result "Command performed successfully", some 
// proactive commands require additional information in the command result. 
// This is defined in the subclauses below. For the general results '20', '21', 
// '26', '34', '35', '37', '38' and '39' and '3A', it is mandatory for the ME 
// to provide a specific cause value as additional information, as defined in 
// the subclauses below. For the other general results, the ME may optionally 
// supply additional information. If additional information is not supplied, 
// then the length of the value part of the data object need only contain the 
// general result. See 3GPP TS 11.14 V8.8.0, chapters 12.12.1 - 12.12.11 for 
// more details.

// Additional information for ME problem
// Additional information for network problem
// Additional information for SS problem
const TUint8 KNoSpecificCauseCanBeGiven                         = 0x00;
const TUint8 KScreenIsBusy                                      = 0x01;
const TUint8 KMeCurrentlyBusyOnCall                             = 0x02;
const TUint8 KMeCurrentlyBusyOnSsTransaction                    = 0x03;
const TUint8 KNoService                                         = 0x04;
const TUint8 KAccessControlClassBar                             = 0x05;
const TUint8 KRadioResourceNotGranted                           = 0x06;
const TUint8 KNotInSpeechCall                                   = 0x07;
const TUint8 KMeCurrentlyBusyOnUssdTransaction                  = 0x08;
const TUint8 KMeCurrentlyBusyOnSendDtmfCommand                  = 0x09;

// Additional information for interaction with call control or MO SM control
const TUint8 KActionNotAllowed                                  = 0x01;
const TUint8 KTypeOfRequestHasChanged                           = 0x02;

// Additional information for MultipleCard commands
const TUint8 KCardReaderRemovedOrNotPresent                     = 0x01;
const TUint8 KCardRemovedOrNotPresent                           = 0x02;
const TUint8 KCardReaderBusy                                    = 0x03;
const TUint8 KCardPoweredOff                                    = 0x04;
const TUint8 KCApduFormatError                                  = 0x05;
const TUint8 KMuteCard                                          = 0x06;
const TUint8 KTransmissionError                                 = 0x07;
const TUint8 KProtocolNotSupported                              = 0x08;
const TUint8 KSpecifiedReaderNotValid                           = 0x09;

// Additional information for Launch Browser problem
const TUint8 KBearerUnavailable                                 = 0x01;
const TUint8 KBrowserUnavailable                                = 0x02;
const TUint8 KMeUnableToReadTheProvisioningData                 = 0x03;

// Additional information for Bearer Independent Protocol
const TUint8 KNoChannelAvailable                                = 0x01;
const TUint8 KChannelClosed                                     = 0x02;
const TUint8 KChannelIdentifierNotValid                         = 0x03;
const TUint8 KRequestedBufferSizeNotAvailable                   = 0x04;
const TUint8 KSecurityError                                     = 0x05;
const TUint8 KRequestedSimMeInterfaceTransportLevelNotAvailable = 0x06;

// Source and Destination device identities are coded as follows:
const TUint8 KKeypad				                            = 0x01;
const TUint8 KDisplay				                            = 0x02;
const TUint8 KEarpiece				                            = 0x03;
const TUint8 KAdditionalCardReader0                             = 0x10;
const TUint8 KAdditionalCardReader1                             = 0x11;
const TUint8 KAdditionalCardReader2                             = 0x12;
const TUint8 KAdditionalCardReader3                             = 0x13;
const TUint8 KAdditionalCardReader4                             = 0x14;
const TUint8 KAdditionalCardReader5                             = 0x15;
const TUint8 KAdditionalCardReader6                             = 0x16;
const TUint8 KAdditionalCardReader7                             = 0x17;
const TUint8 KChannel1				                            = 0x21;
const TUint8 KChannel2				                            = 0x22;
const TUint8 KChannel3				                            = 0x23;
const TUint8 KChannel4				                            = 0x24;
const TUint8 KChannel5				                            = 0x25;
const TUint8 KChannel6				                            = 0x26;
const TUint8 KChannel7				                            = 0x27;
const TUint8 KSim					                            = 0x81;
const TUint8 KMe					                            = 0x82;
const TUint8 KNetwork				                            = 0x83;


// TLV Specific Data Types
enum TTlvSpesificDataType
    {
    ETLV_TonAndNpi,
    ETLV_DiallingNumberString,
    ETLV_AlphaIdentifier,
    ETLV_SubAddress,
    ETLV_CapabilityConfigurationParameters,
    ETLV_CellBroadcastPage,
    ETLV_CommandNumber,
    ETLV_TypeOfCommand,
    ETLV_CommandQualifier,
    ETLV_SourceDeviceIdentity,
    ETLV_DestinationDeviceIdentity,
    ETLV_TimeUnit,
    ETLV_TimeInterval,
    ETLV_IdentifierOfItem,
    ETLV_TextStringOfItem,
    ETLV_IdentifierOfItemChosen,
    ETLV_MinimumLengthOfResponse,
    ETLV_MaximumLengthOfResponse,
    ETLV_GeneralResult,
    ETLV_AdditionalInformationOnResult,
    ETLV_SmsTPdu,
    ETLV_DataCodingScheme,
    ETLV_SsOrUssdString,
    ETLV_TextString,
    ETLV_Tone,
    ETLV_UssdString,
    ETLV_NumberOfFiles,
    ETLV_Files,
    ETLV_LocationAreaCode,
    ETLV_CellIdentityValue,
    ETLV_MccAndMnc,
    ETLV_Imei,
    ETLV_NetworkMeasurementResults,
    ETLV_DefaultText,
    ETLV_ItemsNextActionIndicator,
    ETLV_EventList,
    ETLV_Cause,
    ETLV_LocationStatus,
    ETLV_TransactionIdentifier,
    ETLV_BcchChannelList,
    ETLV_IconIdentifier,
    ETLV_IconQualifier,
    ETLV_IconListQualifier,
    ETLV_IconIdentifierList,
    ETLV_CardReaderStatus,
    ETLV_CommandClassCLA,
    ETLV_CommandInstructionCodeIns,
    ETLV_P1Parameter,
    ETLV_P2Parameter,
    ETLV_Lc,
    ETLV_Data,
    ETLV_Le,
    ETLV_RApduData,
    ETLV_StatusWordSW1,
    ETLV_StatusWordSW2,
    ETLV_TimerIdentifier,
    ETLV_TimerValue,
    ETLV_DateTimeAndTimeZone,
    ETLV_AtCommand,
    ETLV_AtResponse,
    ETLV_BcRepeatIndicatorValues,
    ETLV_DtmfString,
    ETLV_Language,
    ETLV_MeStatus,
    ETLV_TimingAdvance,
    ETLV_BrowserIdentity,
    ETLV_Url,
    ETLV_ListOfBearers,
    ETLV_PathToProvisioningFile,
    ETLV_BrowserTerminationCause,
    ETLV_BearerType,
    ETLV_BearerParameters,
    ETLV_ChannelDataString,
    ETLV_ChannelDataLength,
    ETLV_BufferSize,
    ETLV_ChannelStatus,
    ETLV_IdentifierOfCardReader,
    ETLV_TypeOfAddress,
    ETLV_Address,
    ETLV_TransportProtocolType,
    ETLV_PortNumber,
    ETLV_NetworkAccessName,
    ETLV_CallControlRequestedAction,
    ETLV_Atr,
    ETLV_AID
    };

//  MACROS  

    //none

//  DATA TYPES  

    //none

//  EXTERNAL DATA STRUCTURES  

    //none

//  FUNCTION PROTOTYPES  

    //none

//  FORWARD DECLARATIONS

    //none

#endif // BERTLV_DEFS_H

// End of File
