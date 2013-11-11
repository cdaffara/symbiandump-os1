// Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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
 @internalAll 
*/

#ifndef SAT_DEFS_H_
#define SAT_DEFS_H_

//  INCLUDES
#include <etelsat.h>    // Symbian Etel Sat related types

#ifndef BERTLV_DEFS_H
#define BERTLV_DEFS_H

#include <e32base.h>				// for class CBase
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

#endif

#ifndef CTLV_H
#define CTLV_H

// DESCRIPTION
// TLV Base class. methods common for CTlv and CBerTlv

class CTlvBase : public CBase
    {
    public:

        /**
        * Constructor
        */
        CTlvBase();

        /**
        * Get pointer to TLV data
        * @return Pointer to TLV data
        */
        TPtrC8 Data() const;

        /**
        * Set TLV data
        * @param aData: Data to be inserted
        * @return None
        */
        void SetData( TPtrC8 aData );

        /**
        * Get total size, including tag
        * @return size
        */
        TInt GetSize() const;

        /**
        * Get tag part of TLV
        * @return tag value
        */
        TUint8 GetTag() const;
                        
        /**
        * Returns true if comprehension flag is on
        * @return boolean
        */
        TBool GetComprehensionRequired() const;

        /**
        * Get length part of TLV
        * @return length
        */
        TUint8 GetLength () const;

        /**
        * Get value part of TLV
        * @return value
        */
        TPtrC8 GetValue() const;

    protected: // Data

        TPtrC8 iData;
    };


// DESCRIPTION
// CTlv contains methods to read simple TLV data

class CTlv : public CTlvBase
    {
    public: // Constructor

        /**
        * Default constructor
        */
        IMPORT_C CTlv();

    public: // Methods

		/**
        * Returns short info defined by aType.
        * @param aType: wanted data type.
        * @return wanted data.
        */
        IMPORT_C TUint8 GetShortInfo( TTlvSpesificDataType aType );
		
        /**
        * Returns the Data defined by aType. Index and length of the 
        * data depends on data type.
        * @param aType: data type.
        * @return pointer to the data.
        */
        IMPORT_C TPtrC8 GetData( TTlvSpesificDataType aType );

		/**
        * Returns long info by aType.
        * @param aType: data type.
        * @return returned data
        */
        IMPORT_C TUint16 GetLongInfo( TTlvSpesificDataType aType );
    };

#endif 

#ifndef CBERTLV_H
#define CBERTLV_H

// DESCRIPTION
// CBerTlv contains methods to read BER-TLV data

class CBerTlv  : public CTlvBase
    {

    public: // Constructor

        /**
        * Default constructor
        */
        IMPORT_C CBerTlv();

    public: // Methods

        /**
        * Find TLV by tag value
        * @param aTlv: 
        * @param aTlvTag: TlvTag
        * @param aItemNbr: number of requested TLV, default is 0
        * @return TInt: KErrNone or KErrNotFound
        */
        IMPORT_C TInt TlvByTagValue( CTlv* aTlv, TUint8 aTlvTag, 
            TInt aItemNbr = 0 );
            
        /**
        * Find several TLVs by tag value
        * @param aSubBlock: retruns TLVs
        * @param aSubBlockId: TlvTag
        * @return TInt: KErrNone or KErrNotFound
        */
        IMPORT_C TInt TlvByTagValueMulti( CTlv* aTlv, TUint8 aTlvTag );	
    };

#endif

#ifndef TSATUTILITY_H
#define TSATUTILITY_H

// CONSTANTS
const TUint8 KZeroCharacterCode		        = 0x30;
const TUint8 KPacked7BitTextMask	        = 0x0C;
const TUint8 KCrShiftedOneBitToLeft 	    = 0x1A;
const TUint8 KUCS2TextCodingMask    	    = 0x08;
const TUint8 KUCS2ArabicCoding 	    	    = 0x80;
const TUint8 KUCS2GreekCoding	    	    = 0x81;
const TUint8 KUCS2TurkishCoding	    	    = 0x82;
// The card ID is always zero as long as only one supported
const TUint8 KCardId0                        = 0x00;

// From 3GPP TS 51.11, 9.4.2 Responses to commands which are postponed
const TUint8 KSw1SatBusy 		    	    = 0x93;

// From 3GPP TS 03.40, 9.2.3.22	TP Failure Cause (TP FCS) 
// SMS-PP SIM DDL error
const TUint8 KSmsPpSimDdlError 	    	    = 0xD5;
// SMSP-PP SAT busy
const TUint8 KSmsPpSatBusy 		    	    = 0xD4;

// From 3GPP TS 11.11, 9.4	Status conditions returned by the card
const TUint8 KSw1NormalEnding               = 0x90;
const TUint8 KSw2NormalEnding               = 0x00;

// From 3GPP TS 11.10-4, 27.22.8 MO SHORT MESSAGE CONTROL BY SIM 
const TUint8 KMoSmCtrlResultAllowed 					= 0x00;
const TUint8 KMoSmCtrlResultNotAllowed 					= 0x01;
const TUint8 KMoSmCtrlResultAllowedWithModifications 	= 0x02;

// From 3GPP TS 22.030 7.0.0 Structure of the MMI (Annex B)
const TUint16 KSsAllForwardings             = 0x0002; 
const TUint16 KSsAllCondForwardings         = 0x0004; 
const TUint16 KSsForwUnconditional          = 0x0015; 
const TUint16 KSsForwBusy                   = 0x0043; 
const TUint16 KSsForwNoReply                = 0x003D; 
const TUint16 KSsForwNotReachable           = 0x003E;
 
// TON NPI
// This just indicates that the TON NPI value is not set, TSY specific def.
const TUint8 KTonNpiNotSet                  = 0xff;               
// From 3GPP TS 27.007-460, 8.33 Set Voice Call Number +CSVM
const TUint8 KTonNpiInternational           = 0x91;
const TUint8 KTonNpiUnknown                 = 0x81;

// General string length limitations
const TUint8 KMaxAddressLength          	= 255;
// From 3GPP TS 23.038-450, 6.1.2.3	USSD packing
// String max length 160b + additional info in TR (1b) and DCS in PCmd (1b)
const TUint8 KMaxUssdStringLengthInChars    = 182;
const TUint8 KMaxUssdStringLengthInBytes    = 161;
// Etel accepts SS strings with maximum length 242
const TUint8 KMaxSSStringLength             = 242;
const TUint8 KSizeOfConversionArray         = 63;
const TUint8 KNumberMaxLength 		        = 123;
const TUint8 KOperatorCodeLength            = 3;
const TUint8 KAsciiBcdTableLength           = 15;

// This applies for Parameters, DCS and Protocol ID (3GPP TS 23.038, 5 CBS Data
// Coding Scheme; 3GPP TS 23.040, 9.2.3.9 TP Protocol Identifier (TP PID);
// 3GPP TS 23.040, 9.2.3.1 - 9.2.3.5
const TUint8 KSmsTpduByteUnknownOrReserved  = 0xFF;

// Constants used in bit operations
const TUint8 KMaskF0                        = 0xF0;

// Bcd->ascii translation table
static const TUint8 KAscii[16] =
    {
     '0', '1', '2', '3', '4', '5', '6', '7',
     '8', '9', '*', '#', 'p', 'w', '.',  0
    };

// 3GPP TS 11.11, EFadn
static const TUint8 KAsciiToBCD[KAsciiBcdTableLength][2] =
    { 
    { '0', 0x00 }, { '1', 0x01 }, { '2', 0x02 }, { '3', 0x03 },
    { '4', 0x04 }, { '5', 0x05 }, { '6', 0x06 }, { '7', 0x07 },
    { '8', 0x08 }, { '9', 0x09 }, { '*', 0x0A }, { '#', 0x0B },
    { 'p', 0x0C }, /*DTMF*/ 
    { 'w', 0x0D }, /*Wild*/ 
    { '.', 0x0E }, /*RFU*/ };
    
// GSM 7-bit ->unicode translation table
// See 3GPP TS 23.038
static const TUint16 KUnicode[128] =
    {
    0x0040, 0x00A3, 0x0024, 0x00A5, 0x00E8, 0x00E9, 0x00F9, 0x00EC, 
    0x00F2, 0x00C7, 0x000A, 0x00D8, 0x00F8, 0x000D, 0x00C5, 0x00E5,
    0x0394, 0x005F, 0x03A6, 0x0393, 0x039B, 0x03A9, 0x03A0, 0x03A8, 
    0x03A3, 0x0398, 0x039E, 0x001B, 0x00C6, 0x00E6, 0x00DF, 0x00C9,
    0x0020, 0x0021, 0x0022, 0x0023, 0x00A4, 0x0025, 0x0026, 0x0027,
    0x0028, 0x0029, 0x002A, 0x002B, 0x002C, 0x002D, 0x002E, 0x002F,
    0x0030, 0x0031, 0x0032, 0x0033, 0x0034, 0x0035, 0x0036, 0x0037,
    0x0038, 0x0039, 0x003A, 0x003B, 0x003C, 0x003D, 0x003E, 0x003F,
    0x00A1, 0x0041, 0x0042, 0x0043, 0x0044, 0x0045, 0x0046, 0x0047,
    0x0048, 0x0049, 0x004A, 0x004B, 0x004C, 0x004D, 0x004E, 0x004F,
    0x0050, 0x0051, 0x0052, 0x0053, 0x0054, 0x0055, 0x0056, 0x0057, 
    0x0058, 0x0059, 0x005A, 0x00C4, 0x00D6, 0x00D1, 0x00DC, 0x00A7, 
    0x00BF, 0x0061, 0x0062, 0x0063, 0x0064, 0x0065, 0x0066, 0x0067, 
    0x0068, 0x0069, 0x006A, 0x006B, 0x006C, 0x006D, 0x006E, 0x006F,
    0x0070, 0x0071, 0x0072, 0x0073, 0x0074, 0x0075, 0x0076, 0x0077, 
    0x0078, 0x0079, 0x007A, 0x00E4, 0x00F6, 0x00F1, 0x00FC, 0x00E0  
    };


static const TUint16 KUnicode16ToSms7[63][2] =
    { 
    { 0x0040, 0x00 }, { 0x00A3, 0x01 }, { 0x0024, 0x02 }, { 0x00A5, 0x03 },
    { 0x00E8, 0x04 }, { 0x00E9, 0x05 }, { 0x00F9, 0x06 }, { 0x00EC, 0x07 },
    { 0x00F2, 0x08 }, { 0x00C7, 0x09 }, { 0x00D8, 0x0B }, { 0x00F8, 0x0C },
    { 0x21B5, 0x0D }, { 0x000D, 0x0D }, { 0x2029, 0x0D }, { 0x00C5, 0x0E },
    { 0x00E5, 0x0F }, { 0x0394, 0x10 }, { 0x005F, 0x11 }, { 0x03A6, 0x12 },
    { 0x0393, 0x13 }, { 0x039B, 0x14 }, { 0x03A9, 0x15 }, { 0x03A0, 0x16 },
    { 0x03A8, 0x17 }, { 0x03A3, 0x18 }, { 0x0398, 0x19 }, { 0x039E, 0x1A },
    { 0x00C6, 0x1C }, { 0x00E6, 0x1D }, { 0x00DF, 0x1E }, { 0x00C9, 0x1F },
    { 0x00A4, 0x24 }, { 0x00A1, 0x40 }, { 0x00C4, 0x5B }, { 0x00D6, 0x5C },
    { 0x00D1, 0x5D }, { 0x00DC, 0x5E }, { 0x00A7, 0x5F }, { 0x00BF, 0x60 },
    { 0x00E4, 0x7B }, { 0x00F6, 0x7C }, { 0x00F1, 0x7D }, { 0x00FC, 0x7E },
    { 0x00E0, 0x7F }, { 0x007B, 0x1B }, { 0x007B, 0x28 }, { 0x007D, 0x1B },
    { 0x007D, 0x29 }, { 0x005B, 0x1B }, { 0x005B, 0x3C }, { 0x007E, 0x1B },
    { 0x007E, 0x3D }, { 0x005D, 0x1B }, { 0x005D, 0x3E }, { 0x005C, 0x1B },
    { 0x005C, 0x2F }, { 0x005E, 0x1B }, { 0x005E, 0x14 }, { 0x007C, 0x1B },
    { 0x007C, 0x40 }, { 0x20AC, 0x1B }, { 0x20AC, 0x65 } 
    };

// SMS data coding schemes
enum TSmsDcs
	{
	ESmsUnknownOrReservedDcs = KSmsTpduByteUnknownOrReserved,
	ESms7BitDcs = 0x00,
	ESms8BitDcs = 0x04,
	ESms16BitDcs = 0x08
	};

// DESCRIPTION
/**
* This class includes small aid fuctions
* @since S60 3.1
*/
class TSatUtility
    {

    public: // New functions

        /**
        * Map TON and NPI to RSat TON and NPI values
        * @param aTonAndNpi: Source TON and NPI
        * @param aTon: Mapped Type of number
        * @param aNpi: Mapped Numbering plan
        */
		IMPORT_C static void TonAndNpi( TInt aTonAndNpi, 
            RSat::TTypeOfNumber* aTon,
            RSat::TNumberingPlan* aNpi );

        /**
        * Converts 7-bit packed string to 8-bit unpacked format
        * @param aSource: Pointer to the const data
        * @param aTarget converted 8-bit unpacked string
        * @return KErrNone or KErrOverflow if string was too long
        */
        IMPORT_C static TInt Packed7to8Unpacked( const TPtrC8 aSource, 
            TDes8& aTarget );

        /**
        * Converts UCS2 string to 7-bit packed format
        * @param aInput: is source
        * @param aOutput: converted 7-bit packed string
        */
        IMPORT_C static void UCSToPacked7( const TPtrC aInput, 
            TDes8& aOutput );
        
        /**
        * Converts Binary Coded Decimal to ASCII
        * @param aInput is source
        * @param aOutput converted 8-bit string
        * @return KErrNone or KErrOverflow if string was too long
        */
        IMPORT_C static TInt BCDToAscii( const TPtrC8 aInput, TDes8& aOutput );	

        /**
        * Converts Ascii string to Binary Coded Decimal
        * @param aInput is source
        * @param aOutput converted 8-bit string
        * @return KErrNone or KErrOverflow if string was too long
        */
        IMPORT_C static TInt AsciiToBCD( const TDesC8& aInput, 
            TDes8& aOutput );

        /**
        * Remove Wild 'w' and Expansion digit '.' from EFadn string.
        * @param aInput is source
        * @param aOutput converted 8-bit string
        */
        IMPORT_C static void RemoveWildAndExpansionDigit( const TPtrC8 aInput,
            TDes8& aOutput );

        /**
        * Set Alpha identifier as a Unicode text string
        * and according to the alphabet used
        * @param aRawData input data that was extracted with 
        * ETLV_AlphaIdentifier
        * @param aAlphaId output
        */
        IMPORT_C static void SetAlphaId( const TPtrC8 aRawData, TDes& aAlphaId );		

        /**
        * Convert integer to BCD format. If number is more than 100 only last
        * two digits is converted.
        * @param aTime time
        * @return converted BCD value
        */
        IMPORT_C static TUint8 ConvertToSemiOctet( const TInt aTime );

        /**
        * Browse a text in Unicode format, and for some specific characters 
        * replace with the correct byte value
        * @param aInput: GSM 7 bit default alphabet string
        * @param aOutput: Unicode 16 string
        */
        IMPORT_C static void Convert7BitToUnicode16( const TDesC8& aInput,
            TDes16& aOutput );

        /**
        * Converts unicode16 string to 7 bit character mode
        * @param aInput: Unicode 16 string
        * @param aOutput: Resulting GSM 7 bit default alphabet string
        * @return KErrNone or KErrOverflow (input string is too long) 
        */
        IMPORT_C static TInt ConvertUnicode16To7Bit( const TDesC16& aInput,
            TDes8& aOutput );

        /**
        * Fill in a TDuration structure
        * @param aBerTlv tlv containing duration data
        * @param aTDuration duration structure
        */
        IMPORT_C static void FillDurationStructure( CBerTlv& aBerTlv, 
            RSat::TDuration& aTDuration );

        /**
        * Fill in a TIconId structure
        * @param aBerTlv data containing icon address data
        * @param aTIconId icon id structure
        * @param aItemNmb item number
        */
        IMPORT_C static void FillIconStructure( CBerTlv& aBerTlv, 
            RSat::TIconId& aTIconId, const TInt aItemNmb = 0 );

        /**
        * Set Text string as a Unicode text string
        * @param aTextString text string
        * @param aText unicode output text string
        */
        IMPORT_C static void SetText( CTlv& aTextTlv, TDes& aUnicodeOutput );

        /**
        * Convert Alpha field text string to a Unicode text string. 
        * @param aSource text string
        * @param aText unicode output text string
        */
        IMPORT_C static void ConvertAlphaFieldsToUnicode( 
            const TDesC8& aSource, TDes& aTarget );
            
        /**
        * Copy from 16 bit descriptor to 8 bit descriptor with little endian.
        * Function uses Append method.
        * @param aSource: source
        * @param aTarget: target
        * @return KErrNone or KErrOverflow if string was too long
        */
		IMPORT_C static TInt Copy16to8LE( const TDesC16& aSource, 
            TDes8& aTarget );

        /**
        * Copy from 8 bit descriptor to 16 bit descriptor with little endian.
        * Function uses Append method.
        * @param aSource: source
        * @param aTarget: target
        * @return KErrNone or KErrOverflow if string was too long
        */
        IMPORT_C static TInt Copy8to16LE( const TDesC8& aSource, 
            TDes16& aTarget );

        /**
        * Finds whether the data coding scheme, coded in CBS format,
        * is 7-bit, 8-bit or 16-bit
        * @param aDcs Input data coding scheme
        * @return Converted DCS ( SMS format )
        */
        IMPORT_C static TSmsDcs DecodeCbsDcs( const TUint8 aDcs ); 
        
        /**
        * Gets 8 bit byte and copy it to the 16 bit byte with right endian.
        * @param aSource: source
        * @param aTarget: target
        * @param aIndex: index
        * @return KErrNone or KErrOverflow if string was too long
        */
        IMPORT_C static TInt CopyTwo8toOne16LE( const TDesC8& aSource, 
            TUint16& aTarget, const TInt aIndex ); 
	};

#endif

const TUint8 KLaunchBrowserCmdQualifierNotUsed    = 0x01;
const TUint8 KLaunchBrowserCmdQualifierReserved   = 0x04;
const TUint8 KCsdBearer                           = 0x01;
const TUint8 KGprsBearer                          = 0x03;

const TUint8 KOnlyIfNotBusy                       = 0x00;
const TUint8 KOnlyIfNotBusyWithRedial             = 0x01;
const TUint8 KHoldOtherCalls                      = 0x02;
const TUint8 KHoldOtherCallsWithRedial            = 0x03;
const TUint8 KDisconnectOtherCalls                = 0x04;
const TUint8 KDisconnectOtherCallsWithRedial      = 0x05;

const TUint8 KSpecificLanguage                    = 0x01;
const TUint8 KBipCsdBearer                        = 0x01;
const TUint8 KBipGprsBearer                       = 0x02;
const TUint8 KBipDefaultBearer                    = 0x03;
const TUint8 KBipLocalLinkBearer                  = 0x04;
const TUint8 KBipSendDataImmediately              = 0x01;

const TUint8 KNoCause                             = 0x00;
const TUint8 KSmsVPFRelative                      = 0x10;
const TUint8 KSmsVPFEnhanced                      = 0x08;
const TUint8 KSmsVPFAbsolute                      = 0x18;
//const TUint8 	KMaxAddressLength          	= 255;
const TUint8 	KMaxCommandDetailsLength	= 5;
const TInt 		KImeiLength=16; //the structure defined in CSatNotifyLocalInfo was 16 bytes however ETSI TS 123 003 V7.7.0 specifies 15 bytes for IMEIs.
//const TUint8 	KOperatorCodeLength            	= 3;
const TUint8 	KAdditionalInfoMaxSize		=	244;

typedef TBuf8<KImeiLength> TImeiBuf;

const TUint8  KMaxBccLength                       = 16;
const TUint16 KMaxLengthDefaultBearerData         = 258;


// Max address length
const TUint8 KAddrMaxLength = 255;

/*
* This structure contains local information.
* Used in event handling and by CSatNotifyLocalInfo
*/
struct TLocalInformation
    {
    TUint16 iLocationAreaCode;      // Location area code 
    TUint16 iCellId;                // Cell Id
    TBuf8<KOperatorCodeLength> iOperatorCode;   // Operator code
    };


struct TTerminalRespData 
    {
    // Proactive command number
    TUint8 iPCmdNumber;   
    // Command details tlv     
    TBuf8<KMaxCommandDetailsLength>iCommandDetails;
    // General result for NotifySetUpEventList    
    TUint8 iGeneralResult;  
    // Additional info   
    TBuf<KAdditionalInfoMaxSize> iAdditionalInfo; 
    };

//icon defs
const TUint8 KGetIconDataPckgLength     = 2;
const TUint8 KIconInfoLength            = 6;
const TUint8 KBasicIconDataLength		= 2;
const TUint8 KColourIconDataLength		= KIconInfoLength;
const TUint8 KFilePathLength            	= 8;		
/**
 * Used for Location Status
 */
const TUint8 KCountryCodeLength = 3;


/*
* The data in this structure contains information 
* needed in Get Icon, Get image instance and get clut.
*/
struct TIconInfo
    {
    TUint16 iOffset;                      // Offset
    TUint16 iLength;                      // Length
	TBuf8<KFilePathLength> iSimFilePath;  // Sim file path
    };


enum TSsStatus
	{
	ENotBusy,
	ESsBusy,
	EUssdBusy
	};

enum TCachedSimAtkFiles
	{
	ECacheEFSST = 0x01,  // EF-SST, (U)SIM Service table
	ECacheEFCBMID = 0x02,// EF-CBMID, CB DataDownload message IDs
	// Next 0x04, 0x08 etc..
	};

/**
* This Structure provides Refresh specific information
* Used: CSatNotifyRefresh::RefreshAllowed()
*/

struct TRefreshInfo
    {
    TBuf8<RSat::KRefreshedFileListMaxSize> iFileList;
    TBuf8<RSat::KAidMaxSize> iAid;
    TUint16 iInternalCache;
    };

/*
* This structure contains data needed for LocationStatus
* envelope. (used by EventDownload and MoSmContol)
*/
struct TLocationStatusEnvelope 
    {
    TUint8 iLocationStatus;  // Location satus
    TBuf8<KCountryCodeLength> iCountryCode;     // Country code
    TUint16 iLocationAreaCode;// Location area code
    TUint16 iCellId;          // Cell id
	};


const TUint8  KMaxCauseLength	   = 248;
	
/**
* This structure contains data needed for CallDisconnected
* envelope.
*/
struct TCallDisconnectedEnvelope 
    {
    TUint8 iCmdNumber;      		// Command number
    TBool iNearEnd;     			// MO flag
    TBool iCauseGiven;     	    	// Cause given
    TBuf8<KMaxCauseLength>  iCause; // Cause
    };

/**
 * This structure contains data needed for MTCall
 * envelope.
 */
 struct TMTCallEnvelope 
	 {
	 TUint8 iCmdNumber;               // Command number
	 TUint8 iTONandNPI;               // Ton and Npi
	 TBuf8<KNumberMaxLength> iDiallingNumber;// Calling Party BCD number
	 TPtrC8 iCalledPartySubAddress;   // Calling Party Subaddress
	 };
 
 /*
 * This structure contains the address data of the service center
 * needed for MO-SMS Control envelope.
 */
 struct TAddressData 
     {
     TPtrC8 iRPDestinationAddr;	// RP Destination Address
     TPtrC8 iTPDestinationAddr;	// TP Destination Address 
     };

 /**
 * The data in this structure is used for a response to the NAA for a
 * MO SM Control notification request. The possible values of the
 * result are defined in TSatUtility.h
 */
 struct TMoSmCtrlData 
     { 
     TAddressData iAddressData;  // RP and TP Destination addresses 
     RSat::TAlphaId*	iAlphaId;	// Pointer to the Alpha ID object
     TBool iIsAlphaIdEmpty;		// Declares id Alpha ID is empty
     TUint8 iResult;				// MO SM Ctrl Result, 
     };


/*
* This structure contains information needed in constructing Call 
* Control (call/SS/USSD) Envelope and handling the call control events.
*/
struct TCCInfo
    {
    TUint8 iTag;                // Tag of envelope
    TBuf8<KMaxBccLength> iBcc1; // For storing bearer capability 1            
    TBuf8<KMaxBccLength> iBcc2; // For storing bearer capability 2
    TBuf8<KMaxAddressLength> iAddress; // For storing address
    TUint8 iTonNpi;             // For storing ton&npi
    TUint8 iDcs;                // For storing DCS (only USSD)
    };


typedef TBuf8<16> TNmrBuf;
typedef TBuf<32> TBcchListBuf;

// Cell Broadcast isi msg length
const TUint8 KCbsMsgMaxLength           = 92;


// --------------

//  INCLUDES
#include <e32base.h>				// Etel base

// CONSTANTS

// The internal SAT IPC values are agreed to be between 1001600 - 1002000
// See ETel_Telephony_Server_Design.doc, chapter 6.3 Policing Custom APIs
const TInt KSatInternalIpcOffset = 1001600;
const TInt KSatInternalIpcMaxOffset = 1002000;

// Contains IPC definitions used by SAT TSY Layering.

// TSatTsyLayerIPC enumerates IPC values used by SAT API.
enum TSatTsyLayerIPC
    {
    ESatTsyReady = KSatInternalIpcOffset,
    ESatTsyPCmd,
    ESatTsyTimerExpiredIPC,	
    ESatTsyGetIfPendingNotification,
	ESatTsyGetBearerCapability,
    ESatTsyGetIMEI,              	// 1001605   
    ESatTsyTimeZone,
    ESatTsyTimingAdvance,  
    ESatTsyCreateEnvelope,                  
    ESatTsySendEnvelope,   
    ESatTsyEnvelopeError,        	// 1001610	
	ESatTsyMoSmControlEnvelope,                   	
    ESatTsyCallConnected,			
    ESatTsyCallDisconnected,     	  
    ESatTsyLocationStatus,
    ESatTsyMTCall,   				// 1001615 
    ESatTsyAccessTechnologyChange,    
    ESatTsyCellBroadcast,
    ESatTsySmsPpDdl,                
    ESatTsyReadCbmids,				
    ESatTsyNetStatus,              	// 1001620 
    ESatTsyProvideLocalInfo,        
    ESatTsyLocalInformationNmr,     
    ESatTsyAccTech,
    ESatTsyNotifyLocalInfo,       
    ESatTsyServiceRequest,			// 1001625	
    ESatTsyRefresh,		            		
    ESatTsyCompleteRefresh,      
    ESatTsySetPolling,
    ESatTsySetProactiveCommandOnGoingStatus, 
	ESatTsyGetIconData,				// 1001630
	ESatTsyGetImageInstance,
	ESatTsyGetClut,		
	ESatTsyCallControlActivation,   
	ESatTsyMoSmsControlActivation,
	ESatTsyUssdControlActivation,	// 1001635
	ESatTsySmsPpDdlStatus,
	ESatTsySsStatus,
	ESatTsyUssdStatus,				
	ESatTsyImsiChanged,             
	ESatTsyCheckAtkDataForAlphaId,  // 1001640
	ESatTsyCallEvent,				
	ESatTsySSEvent,
	ESatTsyUSSDEvent,				
	ESatTsyEmergencyNbrCheck,       
	ESatTsyFdnStatusCheck,          // 1001645
	ESatTsyRemoveEventList,
	ESatTsySmsDeliverReport
    }; // end enum TSatTsyLayerIPC


#endif /*SAT_DEFS_H_*/
