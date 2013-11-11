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

#if !defined(__SATCS_H__)
#define __SATCS_H__

/**
@file

ETel SAT - Client-Server IPC request definition

This version of the file contains the reordered version of the IPC table for the purpose
of having a more efficient policy table as part of the platform security framework.
Previously IPCs were ordered by functional area and now they are ordered by the capability
associated with them.
*/

#include <et_clsvr.h>

/**
@publishedPartner
@released
*/
#define IPC_SAT_EXT (60000)
/**
@publishedPartner
@released
*/
#define EEtelSatIpcOffset0 (IPC_SAT_EXT)
#define EEtelSatIpcOffset1 (IPC_SAT_EXT + 1000)
#define EEtelSatIpcOffset2 (IPC_SAT_EXT + 2000)
#define EEtelSatIpcOffset3 (IPC_SAT_EXT + 3000)
#define EEtelSatIpcOffset4 (IPC_SAT_EXT + 4000)
#define EEtelSatIpcOffset5 (IPC_SAT_EXT + 5000)
#define EEtelSatIpcOffset6 (IPC_SAT_EXT + 6000)


/**
@publishedPartner
@released
*/
//
// None
//
enum
	{
	ESatGetClut = EEtelSatIpcOffset0,                            // 60000
	ESatGetIcon,                                                 // 60001
	ESatGetImageInstance,                                        // 60002
	ESatGetMeSideSatProfile,                                     // 60003
	ESatNotifyCloseChannelPCmd,                                  // 60004
	ESatNotifyGetChannelStatusPCmd,                              // 60005
	ESatNotifyGetInkeyPCmd,                                      // 60006
	ESatNotifyGetInputPCmd,                                      // 60007
	ESatNotifyGetReaderStatusPCmd,                               // 60008
	ESatNotifyLanguageNotificationPCmd,                          // 60009
	ESatNotifyPlayTonePCmd,                                      // 60010
	ESatNotifyPollingIntervalPCmd,                               // 60011
	ESatNotifyPollingOffPCmd,                                    // 60012
	ESatNotifyPowerOffCardPCmd,                                  // 60013
	ESatNotifyPowerOnCardPCmd,                                   // 60014
	ESatNotifyProactiveSimSessionEnd,                            // 60015
	ESatNotifyProactiveSimSessionStart,                          // 60016
	ESatNotifyReceiveDataPCmd,                                   // 60017
	ESatNotifyRefreshPCmd,                                       // 60018
	ESatNotifyRefreshRequired,                                   // 60019
	ESatNotifySelectItemPCmd,                                    // 60020
	ESatNotifySetUpEventListPCmd,                                // 60021
	ESatNotifySetUpMenuPCmd,                                     // 60022
	ESatNotifyTimerExpiration,                                   // 60023
	ESatNotifyTimerMgmtPCmd,                                     // 60024
	ESatNotifyTsyStateUpdated,                                   // 60025
	ESatNotifyRefreshRequiredParam,                              // 60026
	ESatNotifyGetFramesStatusPCmd								 // 60027
	};

enum
	{
	ESatGetClutCancel = ESatGetClut + EMobileCancelOffset,                                                                                     // 60500
	ESatGetIconCancel = ESatGetIcon + EMobileCancelOffset,                                                                                     // 60501
	ESatGetImageInstanceCancel = ESatGetImageInstance + EMobileCancelOffset,                                                                   // 60502
	ESatGetMeSideSatProfileCancel = ESatGetMeSideSatProfile + EMobileCancelOffset,                                                             // 60503
	ESatNotifyCloseChannelPCmdCancel = ESatNotifyCloseChannelPCmd + EMobileCancelOffset,                                                       // 60504
	ESatNotifyGetChannelStatusPCmdCancel = ESatNotifyGetChannelStatusPCmd + EMobileCancelOffset,                                               // 60505
	ESatNotifyGetInkeyPCmdCancel = ESatNotifyGetInkeyPCmd + EMobileCancelOffset,                                                               // 60506
	ESatNotifyGetInputPCmdCancel = ESatNotifyGetInputPCmd + EMobileCancelOffset,                                                               // 60507
	ESatNotifyGetReaderStatusPCmdCancel = ESatNotifyGetReaderStatusPCmd + EMobileCancelOffset,                                                 // 60508
	ESatNotifyLanguageNotificationPCmdCancel = ESatNotifyLanguageNotificationPCmd + EMobileCancelOffset,                                       // 60509
	ESatNotifyPlayTonePCmdCancel = ESatNotifyPlayTonePCmd + EMobileCancelOffset,                                                               // 60510
	ESatNotifyPollingIntervalPCmdCancel = ESatNotifyPollingIntervalPCmd + EMobileCancelOffset,                                                 // 60511
	ESatNotifyPollingOffPCmdCancel = ESatNotifyPollingOffPCmd + EMobileCancelOffset,                                                           // 60512
	ESatNotifyPowerOffCardPCmdCancel = ESatNotifyPowerOffCardPCmd + EMobileCancelOffset,                                                       // 60513
	ESatNotifyPowerOnCardPCmdCancel = ESatNotifyPowerOnCardPCmd + EMobileCancelOffset,                                                         // 60514
	ESatNotifyProactiveSimSessionEndCancel = ESatNotifyProactiveSimSessionEnd + EMobileCancelOffset,                                           // 60515
	ESatNotifyProactiveSimSessionStartCancel = ESatNotifyProactiveSimSessionStart + EMobileCancelOffset,                                       // 60516
	ESatNotifyReceiveDataPCmdCancel = ESatNotifyReceiveDataPCmd + EMobileCancelOffset,                                                         // 60517
	ESatNotifyRefreshPCmdCancel = ESatNotifyRefreshPCmd + EMobileCancelOffset,                                                                 // 60518
	ESatNotifyRefreshRequiredCancel = ESatNotifyRefreshRequired + EMobileCancelOffset,                                                         // 60519
	ESatNotifySelectItemPCmdCancel = ESatNotifySelectItemPCmd + EMobileCancelOffset,                                                           // 60520
	ESatNotifySetUpEventListPCmdCancel = ESatNotifySetUpEventListPCmd + EMobileCancelOffset,                                                   // 60521
	ESatNotifySetUpMenuPCmdCancel = ESatNotifySetUpMenuPCmd + EMobileCancelOffset,                                                             // 60522
	ESatNotifyTimerExpirationCancel = ESatNotifyTimerExpiration + EMobileCancelOffset,                                                         // 60523
	ESatNotifyTimerMgmtPCmdCancel = ESatNotifyTimerMgmtPCmd + EMobileCancelOffset,                                                             // 60524
	ESatNotifyTsyStateUpdatedCancel = ESatNotifyTsyStateUpdated + EMobileCancelOffset,                                                         // 60525
	ESatNotifyRefreshRequiredParamCancel = ESatNotifyRefreshRequiredParam + EMobileCancelOffset,                                               // 60526
	ESatNotifyGetFramesStatusPCmdCancel = ESatNotifyGetFramesStatusPCmd + EMobileCancelOffset
	};

//
// ReadDeviceData
//
enum
	{
	ESatGetProvisioningRefFile = EEtelSatIpcOffset1,             // 61000
	ESatNotifyDeclareServicePCmd,                                // 61001
	ESatNotifyDisplayTextPCmd,                                   // 61002
	ESatNotifyGetServiceInfoPCmd,                                // 61003
	ESatNotifyLaunchBrowserPCmd,                                 // 61004
	ESatNotifyLocalInfoPCmd,                                     // 61005
	ESatNotifyOpenChannelPCmd,                                   // 61006
	ESatNotifyPerformCardApduPCmd,                               // 61007
	ESatNotifyRunAtCommandPCmd,                                  // 61008
	ESatNotifySendDataPCmd,                                      // 61009
	ESatNotifySendDtmfPCmd,                                      // 61010
	ESatNotifySendSmPCmd,                                        // 61011
	ESatNotifySendSsPCmd,                                        // 61012
	ESatNotifySendUssdPCmd,                                      // 61013
	ESatNotifyServiceSearchPCmd,                                 // 61014
	ESatNotifySetUpCallPCmd,                                     // 61015
	ESatNotifySetUpIdleModeTextPCmd,                             // 61016
	ESatGetOpenChannelPCmd,										 // 61017
	ESatNotifyRetrieveMultimediaMsgPCmd,					 // 61018
	ESatNotifySubmitMultimediaMsgPCmd,						 // 61019
	ESatNotifyDisplayMultimediaMsgPCmd,						 // 61020
	ESatNotifySetFramesPCmd										 // 61021
	};

enum
	{
	ESatGetProvisioningRefFileCancel = ESatGetProvisioningRefFile + EMobileCancelOffset,                                                       // 61500
	ESatNotifyDeclareServicePCmdCancel = ESatNotifyDeclareServicePCmd + EMobileCancelOffset,                                                   // 61501
	ESatNotifyDisplayTextPCmdCancel = ESatNotifyDisplayTextPCmd + EMobileCancelOffset,                                                         // 61502
	ESatNotifyGetServiceInfoPCmdCancel = ESatNotifyGetServiceInfoPCmd + EMobileCancelOffset,                                                   // 61503
	ESatNotifyLaunchBrowserPCmdCancel = ESatNotifyLaunchBrowserPCmd + EMobileCancelOffset,                                                     // 61504
	ESatNotifyLocalInfoPCmdCancel = ESatNotifyLocalInfoPCmd + EMobileCancelOffset,                                                             // 61505
	ESatNotifyOpenChannelPCmdCancel = ESatNotifyOpenChannelPCmd + EMobileCancelOffset,                                                         // 61506
	ESatNotifyPerformCardApduPCmdCancel = ESatNotifyPerformCardApduPCmd + EMobileCancelOffset,                                                 // 61507
	ESatNotifyRunAtCommandPCmdCancel = ESatNotifyRunAtCommandPCmd + EMobileCancelOffset,                                                       // 61508
	ESatNotifySendDataPCmdCancel = ESatNotifySendDataPCmd + EMobileCancelOffset,                                                               // 61509
	ESatNotifySendDtmfPCmdCancel = ESatNotifySendDtmfPCmd + EMobileCancelOffset,                                                               // 61510
	ESatNotifySendSmPCmdCancel = ESatNotifySendSmPCmd + EMobileCancelOffset,                                                                   // 61511
	ESatNotifySendSsPCmdCancel = ESatNotifySendSsPCmd + EMobileCancelOffset,                                                                   // 61512
	ESatNotifySendUssdPCmdCancel = ESatNotifySendUssdPCmd + EMobileCancelOffset,                                                               // 61513
	ESatNotifyServiceSearchPCmdCancel = ESatNotifyServiceSearchPCmd + EMobileCancelOffset,                                                     // 61514
	ESatNotifySetUpCallPCmdCancel = ESatNotifySetUpCallPCmd + EMobileCancelOffset,                                                             // 61515
	ESatNotifySetUpIdleModeTextPCmdCancel = ESatNotifySetUpIdleModeTextPCmd + EMobileCancelOffset,											   // 61516
	ESatGetOpenChannelPCmdCancel = ESatGetOpenChannelPCmd + EMobileCancelOffset,                                                               // 61517
	ESatNotifyRetrieveMultimediaMsgPCmdCancel = ESatNotifyRetrieveMultimediaMsgPCmd + EMobileCancelOffset,                                     // 61518
	ESatNotifySubmitMultimediaMsgPCmdCancel = ESatNotifySubmitMultimediaMsgPCmd + EMobileCancelOffset,                                         // 61519
	ESatNotifyDisplayMultimediaMsgPCmdCancel = ESatNotifyDisplayMultimediaMsgPCmd + EMobileCancelOffset,                                       // 61520
	ESatNotifySetFramesPCmdCancel = ESatNotifySetFramesPCmd + EMobileCancelOffset                                                              // 61521
	};

//
// WriteDeviceData
//
enum
	{
	ESatRefreshAllowed = EEtelSatIpcOffset2,                     // 62000
	};

enum
	{
	ESatRefreshAllowedCancel = ESatRefreshAllowed + EMobileCancelOffset                                                                        // 62500
	};

//
// NetworkControl
//
enum
	{
	ESatTerminalRsp = EEtelSatIpcOffset3,                        // 63000
	ESatUsatClientReadyIndication                                // 63001
	};

enum
	{
	ESatTerminalRspCancel = ESatTerminalRsp + EMobileCancelOffset                                                                              // 63500
	};

//
// NetworkControl ReadDeviceData
//
enum
	{
	ESatMenuSelection = EEtelSatIpcOffset4,                      // 64000
	ESatNotifyCallControlRequest,                                // 64001
	ESatNotifyCbDownload,                                        // 64002
	ESatNotifyMoSmControlRequest,                                // 64003
	ESatNotifySmsPpDownload,                                     // 64004
	};

enum
	{
	ESatMenuSelectionCancel = ESatMenuSelection + EMobileCancelOffset,                                                                         // 64500
	ESatNotifyCallControlRequestCancel = ESatNotifyCallControlRequest + EMobileCancelOffset,                                                   // 64501
	ESatNotifyCbDownloadCancel = ESatNotifyCbDownload + EMobileCancelOffset,                                                                   // 64502
	ESatNotifyMoSmControlRequestCancel = ESatNotifyMoSmControlRequest + EMobileCancelOffset,                                                   // 64503
	ESatNotifySmsPpDownloadCancel = ESatNotifySmsPpDownload + EMobileCancelOffset                                                             // 64504
	};

//
// NetworkControl WriteDeviceData
//
enum
	{
	ESatClientSatProfileIndication = EEtelSatIpcOffset5,          // 65000
	ESatEventDownload,                                            // 65001
	ESatMmsNotificationDownload,								  // 65002
	ESatMmsTransferStatus,								          // 65003
	ESatUssdDataDownload                                          // 65004
	};

enum
	{
	ESatEventDownloadCancel = ESatEventDownload + EMobileCancelOffset,                             // 65501
	ESatMmsNotificationDownloadCancel = ESatMmsNotificationDownload + EMobileCancelOffset,         // 65502
	ESatMmsTransferStatusCancel = ESatMmsTransferStatus + EMobileCancelOffset,                     // 65503
	ESatUssdDataDownloadCancel = ESatUssdDataDownload + EMobileCancelOffset                        // 65504
	};

//
// NetworkServices WriteDeviceData
//
enum
	{
	ESatSendMessageNoLogging = EEtelSatIpcOffset6,                            // 66000
	};

enum
	{
	ESatSendMessageNoLoggingCancel = ESatSendMessageNoLogging + EMobileCancelOffset                                                            // 66500
	};

#endif
