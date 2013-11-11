/*
* Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
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
 @file
 @publishedPartner
 @released
*/
#ifndef _GSMERROR_H_
#define _GSMERROR_H_

//  INCLUDES
#include <e32std.h>

#ifdef USING_CTSY_DISPATCHER
#include <exterror.h>

/** 
 * Duplicates to errors which already exist in exterror.h.
 * 
 * Absolute value modified to be the same as the duplicate. 
 * 
 */
const TInt KErrGsmBusyUserRequest		= KErrGsmCCUserBusy;
const TInt KErrGsmReleaseByUser			= KErrGsmCCNormalCallClearing;
#endif // USING_CTSY_DISPATCHER

////////////////////////////////////////////////////////////////////////////////

//  CONSTANTS 
const TInt KErrPhoneSpecificGsmBase = -8000;
const TInt KErrPhoneSpecificGsmSmsBase = KErrPhoneSpecificGsmBase;

//phone specific

const TInt KErrGsmInvalidParameter      = KErrPhoneSpecificGsmSmsBase - 1;			// -8001
const TInt KErrGsmCbReserveFail         = KErrPhoneSpecificGsmSmsBase - 2;			// -8002

const TInt KErrPhoneSpecificGsmCallBase = KErrPhoneSpecificGsmSmsBase - 50;			// -8050

const TInt KErrGsmCallActive			= KErrPhoneSpecificGsmCallBase - 1;			// -8051
const TInt KErrGsmNoCallActive			= KErrPhoneSpecificGsmCallBase - 2;			// -8052
const TInt KErrGsmNoDualModeCall		= KErrPhoneSpecificGsmCallBase - 3;			// -8053
const TInt KErrGsmUnsupCallMode         = KErrPhoneSpecificGsmCallBase - 4;			// -8054
const TInt KErrGsmModeAlreadyActive		= KErrPhoneSpecificGsmCallBase - 5;			// -8055
const TInt KErrGsmSignallingFailure		= KErrPhoneSpecificGsmCallBase - 6;			// -8056
const TInt KErrGsmNumberDelayed			= KErrPhoneSpecificGsmCallBase - 7;			// -8057
const TInt KErrGsmNumberBlacklisted		= KErrPhoneSpecificGsmCallBase - 8;			// -8058
const TInt KErrGsmWrongTerminalType		= KErrPhoneSpecificGsmCallBase - 9;			// -8059
const TInt KErrGsmInvalidNumber			= KErrPhoneSpecificGsmCallBase - 10;			// -8060
const TInt KErrGsmUiFailure				= KErrPhoneSpecificGsmCallBase - 11;			// -8061
const TInt KErrGsmCostLimitExceeded		= KErrPhoneSpecificGsmCallBase - 12;			// -8062
const TInt KErrGsmNoHalfRateSupported	= KErrPhoneSpecificGsmCallBase - 13;			// -8063
const TInt KErrGsmAcmmaxReached			= KErrPhoneSpecificGsmCallBase - 14;			// -8064
const TInt KErrGsmInvalidFdn			= KErrPhoneSpecificGsmCallBase - 15;			// -8065
const TInt KErrGsmLowBattery			= KErrPhoneSpecificGsmCallBase - 16;			// -8066
const TInt KErrGsmUserPowerOff			= KErrPhoneSpecificGsmCallBase - 17;			// -8067
const TInt KErrGsmConnectionLost		= KErrPhoneSpecificGsmCallBase - 18;			// -8068
const TInt KErrGsmErrorRequest			= KErrPhoneSpecificGsmCallBase - 19;			// -8069
const TInt KErrGsmDtmfSendingBusy		= KErrPhoneSpecificGsmCallBase - 20;			// -8070
#ifndef USING_CTSY_DISPATCHER
const TInt KErrGsmReleaseByUser			= KErrPhoneSpecificGsmCallBase - 21;			// -8071
const TInt KErrGsmBusyUserRequest		= KErrPhoneSpecificGsmCallBase - 22;			// -8072
#endif // USING_CTSY_DISPATCHER
const TInt KErrGsmInvalidCallMode		= KErrPhoneSpecificGsmCallBase - 23;			// -8073
const TInt KErrGsmNoNumber              = KErrPhoneSpecificGsmCallBase - 24;			// -8074
const TInt KErrGsmNumberBarred          = KErrPhoneSpecificGsmCallBase - 26;			// -8076
const TInt KErrGsmNetworkBusy           = KErrPhoneSpecificGsmCallBase - 27;			// -8077
const TInt KErrGsmNoService             = KErrPhoneSpecificGsmCallBase - 28;			// -8078
const TInt KErrGsmCallInProgress        = KErrPhoneSpecificGsmCallBase - 29;			// -8079
const TInt KErrGsmCallServerFail        = KErrPhoneSpecificGsmCallBase - 30;			// -8080
const TInt KErrGsmGprsActive            = KErrPhoneSpecificGsmCallBase - 31;			// -8081
const TInt KErrGsmNotAllowed            = KErrPhoneSpecificGsmCallBase - 32;			// -8082
const TInt KErrGsmCallRejected          = KErrPhoneSpecificGsmCallBase - 40;			// -8090
const TInt KErrGsmCallCollision         = KErrPhoneSpecificGsmCallBase - 41;			// -8091
const TInt KErrGsmDriveMode             = KErrPhoneSpecificGsmCallBase - 42;			// -8092


const TInt KErrPhoneSpecificGsmSupplServBase=KErrPhoneSpecificGsmSmsBase-150;			// -8150

const TInt KErrGsmSsSpecificError =   KErrPhoneSpecificGsmSupplServBase-1;				// -8151
const TInt KErrGsmSsCugReject	  =   KErrPhoneSpecificGsmSupplServBase-2;				// -8152

//Ss indicate password error

const TInt KErrGsmSsEnterPassword	=  KErrPhoneSpecificGsmSupplServBase-3;					// -8153
const TInt KErrGsmSsEnterNewPassword = KErrPhoneSpecificGsmSupplServBase-4;					// -8154
const TInt KErrGsmSsEnterNewPasswordAgain	= KErrPhoneSpecificGsmSupplServBase-5;			// -8155
const TInt KErrGsmBadPassword	= KErrPhoneSpecificGsmSupplServBase-6;						// -8156
const TInt KErrGsmBadPasswordFormat	= KErrPhoneSpecificGsmSupplServBase-7;					// -8157

//Ss indicate problems

const TInt KErrGsmSsUnrecognizedComp = KErrPhoneSpecificGsmSupplServBase-8;			// -8158
const TInt KErrGsmSsMistypedComp = KErrPhoneSpecificGsmSupplServBase-9;				// -8159
const TInt KErrGsmSsBadlyStructurecComp	= KErrPhoneSpecificGsmSupplServBase-10;			// -8160
const TInt KErrGsmSsDublicateInvokeId	= KErrPhoneSpecificGsmSupplServBase-11;			// -8161
const TInt KErrGsmSsUnrecognizedOperation = KErrPhoneSpecificGsmSupplServBase-12;			// -8162
const TInt KErrGsmSsMistypedInvParameter = KErrPhoneSpecificGsmSupplServBase-13;			// -8163
const TInt KErrGsmSsResourceLimitation = KErrPhoneSpecificGsmSupplServBase-14;			// -8164
const TInt KErrGsmSsInitiatingRelease	= KErrPhoneSpecificGsmSupplServBase-15;			// -8165
const TInt KErrGsmSsUnrecognizedLinkedId = KErrPhoneSpecificGsmSupplServBase-16;			// -8166
const TInt KErrGsmSsLinkedRespUnexpected = KErrPhoneSpecificGsmSupplServBase-17;			// -8167
const TInt KErrGsmSsUnexpectedLinkedOper = KErrPhoneSpecificGsmSupplServBase-18;			// -8168
const TInt KErrGsmSsUnrecognizedInvokeId = KErrPhoneSpecificGsmSupplServBase-19;			// -8169
const TInt KErrGsmSsReturnResultUnexpected = KErrPhoneSpecificGsmSupplServBase-20;			// -8170
const TInt KErrGsmSsMistypedResParameter = KErrPhoneSpecificGsmSupplServBase-21;			// -8171
const TInt KErrGsmSsReturnErrorUnexpected	= KErrPhoneSpecificGsmSupplServBase-22;			// -8172
const TInt KErrGsmSsUnrecognizedError	= KErrPhoneSpecificGsmSupplServBase-23;			// -8173
const TInt KErrGsmSsUnexpectedError	= KErrPhoneSpecificGsmSupplServBase-24;			// -8174
const TInt KErrGsmSsMistypedErrParameter = KErrPhoneSpecificGsmSupplServBase-25;			// -8175
const TInt KErrGsmSsCauseSecurityCodeRequired	= KErrPhoneSpecificGsmSupplServBase-26;			// -8176
const TInt KErrSsActivationDataLost	= KErrPhoneSpecificGsmSupplServBase-27;			// -8177
const TInt KErrCPHSInUseBySIM = KErrPhoneSpecificGsmSupplServBase-28;				// -8178
const TInt KErrSSServiceBusy = KErrPhoneSpecificGsmSupplServBase-29;                            // -8179

const TInt KErrPhoneSpecificGsmServersBase=KErrPhoneSpecificGsmSmsBase-250;			// -8250

const TInt KErrGsmSimServCommunicationError	= KErrPhoneSpecificGsmServersBase-1;			// -8251
const TInt KErrGsmSimServDataNotAvail	= KErrPhoneSpecificGsmServersBase-2;			// -8252
const TInt KErrGsmSimServNumbOverflow	= KErrPhoneSpecificGsmServersBase-3;			// -8253
const TInt KErrGsmSimServInvalidLocation = KErrPhoneSpecificGsmServersBase-4;			// -8254
const TInt KErrGsmSimServLocationEmpty = KErrPhoneSpecificGsmServersBase-5;			// -8255
const TInt KErrGsmSimServUpdateImpossible	= KErrPhoneSpecificGsmServersBase-6;			// -8256
const TInt KErrGsmSimServNumbTooLong = KErrPhoneSpecificGsmServersBase-7;			// -8257
const TInt KErrGsmSimServIllegalNumber = KErrPhoneSpecificGsmServersBase-8;			// -8258
const TInt KErrGsmSimServEnableNotAllowed = KErrPhoneSpecificGsmServersBase-9;			// -8259
const TInt KErrGsmSimServDisableNotAllowed = KErrPhoneSpecificGsmServersBase-10;			// -8260

const TInt KErrGsmNetServCauseCommunicationError = KErrPhoneSpecificGsmServersBase-11;			// -8261
const TInt KErrGsmNetServCauseRequestCancelled = KErrPhoneSpecificGsmServersBase-12;			// -8262
const TInt KErrGsmNetServCauseNothingToCancel	= KErrPhoneSpecificGsmServersBase-13;			// -8263
const TInt KErrGsmNetCauseCallActive = KErrPhoneSpecificGsmServersBase-14;			// -8264
const TInt KErrGsmNetCauseRequestRejected = KErrPhoneSpecificGsmServersBase-15;			// -8265
const TInt KErrGsmNetCauseNoSelectedNetwork = KErrPhoneSpecificGsmServersBase-16;			// -8266
const TInt KErrGsmOfflineSimOpNotAllowed =	KErrPhoneSpecificGsmServersBase-17;			// -8267
const TInt KErrGsmOfflineOpNotAllowed	=	KErrPhoneSpecificGsmServersBase-18;			// -8268
const TInt KErrGsmSimServSneFull =   KErrPhoneSpecificGsmServersBase-19;			// -8269
const TInt KErrGsmSimServAnrFull =   KErrPhoneSpecificGsmServersBase-20;			// -8270
const TInt KErrGsmSimServEmailFull =   KErrPhoneSpecificGsmServersBase-21;			// -8271

const TInt KErrGsmCSConnectionBarred =   KErrPhoneSpecificGsmServersBase-22;        // -8272
const TInt KErrGsmPSConnectionBarred =   KErrPhoneSpecificGsmServersBase-23;        // -8273

const TInt KErrPacketDataTsyMaxPdpContextsReached = -6000;

// Sim Application Toolkit Mobile Originated Short Message Control
// etelsat.h only provides KErrSatControl, which is not specific enough
const TInt KErrSatSpecificBase=KErrPhoneSpecificGsmServersBase-50;			// -8300
const TInt KErrSatMoSmControlBarred   = KErrSatSpecificBase-1;			// -8301
const TInt KErrSatMoSmControlModified = KErrSatSpecificBase-2;			// -8302
// Diagnostic Info error values 
const TInt KErrDiagnosticInfoBase = KErrPhoneSpecificGsmBase-350; //-8350			// -8350
const TInt KErrDiagnosticInfoBarredWithCUG = KErrDiagnosticInfoBase-1;			// -8351
const TInt KErrDiagnosticInfoBarredNoCUG = KErrDiagnosticInfoBase-2;			// -8352
const TInt KErrDiagnosticInfoBarredUnknownCUG = KErrDiagnosticInfoBase-3;			// -8353
const TInt KErrDiagnosticInfoBarredIncompatibleCUG = KErrDiagnosticInfoBase-4;			// -8354
const TInt KErrDiagnosticInfoBarredFailureCUG = KErrDiagnosticInfoBase-5;			// -8355
const TInt KErrDiagnosticInfoBarredClirNotSubscribed = KErrDiagnosticInfoBase-6;			// -8356
const TInt KErrDiagnosticInfoBarredCCBSPossible = KErrDiagnosticInfoBase-7;			// -8357
const TInt KErrDiagnosticInfoBarredCCBSNotPossible = KErrDiagnosticInfoBase-8;			// -8358
 
// CustomAPI SmartCard error values
const TInt KErrCustomSCBase = KErrPhoneSpecificGsmBase - 400; //-8400
const TInt KErrCustomSCCondOfuseNotSatisfied = KErrCustomSCBase - 1;
const TInt KErrCustomSCRefDataNotFound = KErrCustomSCBase - 2;
const TInt KErrCustomSCIncorrectMACAuthError = KErrCustomSCBase - 3;
const TInt KErrCustomSCKeyRefreshFail = KErrCustomSCBase - 4;
const TInt KErrCustomSCNoMemSpaceAvailableAuthError = KErrCustomSCBase - 5;
const TInt KErrCustomSCNoMemSpaceAvailableMukAuthError = KErrCustomSCBase - 6;

#endif      //  _GSMERROR_H_
            
// End of File


