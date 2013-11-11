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
// ETel Multimode Packet Data API
// GPRS Rel97/98, CDMAOne, CDMA2000, Release 99 and Release 4.
// Client-Server IPC request definitions.
// This version of the file contains the reordered version of the IPC table for the purpose
// of having a more efficient policy table as part of the platform security framework.
// Previously IPCs were ordered by functional area and now they are ordered by the capability
// associated with them.
// 
//

/**
 @file
*/

#if !defined(__PCKTCS_H__)
#define __PCKTCS_H__

#include <e32def.h>
#include <et_clsvr.h>
#include <e32cmn.h>
/**
@publishedPartner
*/
_LIT(KPacketName,"PACKET_NAME");


/**
Client-Server IPC request definitions
@publishedPartner
@released
*/
#define IPC_PACKET_EXT (40000)
#define EEtelPacketIpcOffset0			(IPC_PACKET_EXT)
#define EEtelPacketIpcOffset1			(IPC_PACKET_EXT + 1000)
#define EEtelPacketIpcOffset2			(IPC_PACKET_EXT + 2000)
#define EEtelPacketIpcOffset3			(IPC_PACKET_EXT + 3000)
#define EEtelPacketIpcOffset4			(IPC_PACKET_EXT + 4000)
#define EEtelPacketIpcOffset5			(IPC_PACKET_EXT + 5000)
#define EEtelPacketIpcOffset6			(IPC_PACKET_EXT + 6000)




//
// None
//
enum
	{
	EPacketContextGetConnectionSpeed = EEtelPacketIpcOffset0,             // 40000
	EPacketContextGetLastErrorCause,                                      // 40001
	EPacketContextGetProfileName,                                         // 40002
	EPacketContextGetStatus,                                              // 40003
	EPacketContextNotifyConnectionSpeedChange,                            // 40004
	EPacketContextNotifyStatusChange,                                     // 40005
	EPacketEnumerateContexts,                                             // 40006
	EPacketEnumerateNifs,                                                 // 40007
	EPacketGetAttachMode,                                                 // 40008
	EPacketGetContextInfo,                                                // 40009
	EPacketGetCurrentReleaseMode,                                         // 40010
	EPacketGetDynamicCaps,                                                // 40011
	EPacketGetMSClass,                                                    // 40012
	EPacketGetNtwkRegStatus,                                              // 40013
	EPacketGetPrefBearer,                                                 // 40014
	EPacketGetStaticCaps,                                                 // 40015
	EPacketGetStatus,                                                     // 40016
	EPacketNotifyAttachModeChange,                                        // 40017
	EPacketNotifyChangeOfNtwkRegStatus,                                   // 40018
	EPacketNotifyContextActivationRequested,                              // 40019
	EPacketNotifyContextAdded,                                            // 40020
	EPacketNotifyDynamicCapsChange,                                       // 40021
	EPacketNotifyMSClassChange,                                           // 40022
	EPacketNotifyReleaseModeChange,                                       // 40023
	EPacketNotifyStatusChange,                                            // 40024
	EPacketPrepareOpenSecondary,                                          // 40025
	EPacketQoSGetProfileCaps,                                             // 40026
	EPacketNotifyMbmsServiceAvailabilityChange,						      // 40027
    EPacketContextGetConnectionInfo,                                      // 40028
    EPacketContextNotifyConnectionInfoChange,                             // 40029   
	};

enum
	{
	EPacketContextGetConnectionSpeedCancel = EPacketContextGetConnectionSpeed + EMobileCancelOffset,                                           // 40500
	EPacketContextNotifyConnectionSpeedChangeCancel = EPacketContextNotifyConnectionSpeedChange + EMobileCancelOffset,                         // 40504
	EPacketContextNotifyStatusChangeCancel = EPacketContextNotifyStatusChange + EMobileCancelOffset,                                           // 40505
	EPacketEnumerateContextsCancel = EPacketEnumerateContexts + EMobileCancelOffset,                                                           // 40506
	EPacketEnumerateNifsCancel = EPacketEnumerateNifs + EMobileCancelOffset,                                                                   // 40507
	EPacketGetAttachModeCancel = EPacketGetAttachMode + EMobileCancelOffset,                                                                   // 40508
	EPacketGetContextInfoCancel = EPacketGetContextInfo + EMobileCancelOffset,                                                                 // 40509
	EPacketGetCurrentReleaseModeCancel = EPacketGetCurrentReleaseMode + EMobileCancelOffset,                                                   // 40510
	EPacketGetMSClassCancel = EPacketGetMSClass + EMobileCancelOffset,                                                                         // 40512
	EPacketGetNtwkRegStatusCancel = EPacketGetNtwkRegStatus + EMobileCancelOffset,                                                             // 40513
	EPacketGetPrefBearerCancel = EPacketGetPrefBearer + EMobileCancelOffset,                                                                   // 40514
	EPacketNotifyAttachModeChangeCancel = EPacketNotifyAttachModeChange + EMobileCancelOffset,                                                 // 40517
	EPacketNotifyChangeOfNtwkRegStatusCancel = EPacketNotifyChangeOfNtwkRegStatus + EMobileCancelOffset,                                       // 40518
	EPacketNotifyContextActivationRequestedCancel = EPacketNotifyContextActivationRequested + EMobileCancelOffset,                             // 40519
	EPacketNotifyContextAddedCancel = EPacketNotifyContextAdded + EMobileCancelOffset,                                                         // 40520
	EPacketNotifyDynamicCapsChangeCancel = EPacketNotifyDynamicCapsChange + EMobileCancelOffset,                                               // 40521
	EPacketNotifyMSClassChangeCancel = EPacketNotifyMSClassChange + EMobileCancelOffset,                                                       // 40522
	EPacketNotifyReleaseModeChangeCancel = EPacketNotifyReleaseModeChange + EMobileCancelOffset,                                               // 40523
	EPacketNotifyStatusChangeCancel = EPacketNotifyStatusChange + EMobileCancelOffset,                                                         // 40524
	EPacketQoSGetProfileCapsCancel = EPacketQoSGetProfileCaps + EMobileCancelOffset,                                                           // 40526
	EPacketNotifyMbmsServiceAvailabilityChangeCancel = EPacketNotifyMbmsServiceAvailabilityChange + EMobileCancelOffset,    		           // 40527	
	EPacketContextGetConnectionInfoCancel = EPacketContextGetConnectionInfo + EMobileCancelOffset,                                             // 40528
	EPacketContextNotifyConnectionInfoChangeCancel = EPacketContextNotifyConnectionInfoChange + EMobileCancelOffset,                           // 40529	
	};

//
// NetworkServices
//
enum
	{
	EPacketAttach = EEtelPacketIpcOffset1,                                // 41000
	EPacketContextActivate,                                               // 41001
	EPacketContextCreateNewTFT,                                           // 41002
	EPacketContextDeactivate,                                             // 41003
	EPacketContextDeleteTFT,                                              // 41004
	EPacketContextInitialiseContext,                                      // 41005
	EPacketContextLoanCommPort,                                           // 41006
	EPacketContextModifyActiveContext,                                    // 41007
	EPacketContextRecoverCommPort,                                        // 41008
	EPacketDeactivateNIF,                                                 // 41009
	EPacketDetach,                                                        // 41010
	EPacketRejectActivationRequest                                        // 41011

	};

enum
	{
	EPacketAttachCancel = EPacketAttach + EMobileCancelOffset,                                                                                 // 41500
	EPacketContextActivateCancel = EPacketContextActivate + EMobileCancelOffset,                                                               // 41501
	EPacketContextCreateNewTFTCancel = EPacketContextCreateNewTFT + EMobileCancelOffset,                                                       // 41502
	EPacketContextDeactivateCancel = EPacketContextDeactivate + EMobileCancelOffset,                                                           // 41503
	EPacketContextDeleteTFTCancel = EPacketContextDeleteTFT + EMobileCancelOffset,                                                             // 41504
	EPacketContextInitialiseContextCancel = EPacketContextInitialiseContext + EMobileCancelOffset,                                             // 41505
	EPacketContextLoanCommPortCancel = EPacketContextLoanCommPort + EMobileCancelOffset,                                                       // 41506
	EPacketContextModifyActiveContextCancel = EPacketContextModifyActiveContext + EMobileCancelOffset,                                         // 41507
	EPacketContextRecoverCommPortCancel = EPacketContextRecoverCommPort + EMobileCancelOffset,                                                 // 41508
	EPacketDeactivateNIFCancel = EPacketDeactivateNIF + EMobileCancelOffset,                                                                   // 41509
	EPacketDetachCancel = EPacketDetach + EMobileCancelOffset,                                                                                 // 41510
	EPacketRejectActivationRequestCancel = EPacketRejectActivationRequest + EMobileCancelOffset                                                // 41511
	};

//
// ReadDeviceData
//
enum
	{
	EPacketContextEnumeratePacketFilters = EEtelPacketIpcOffset2,         // 42000
	EPacketContextGetConfig,                                              // 42001
	EPacketContextGetDNSInfo,                                             // 42002
	EPacketContextNotifyConfigChanged,                                    // 42003
	EPacketEnumerateContextsInNif,                                        // 42004
	EPacketGetContextNameInNif,                                           // 42005
	EPacketGetDefaultContextParams,                                       // 42006
	EPacketGetNifInfo,                                                    // 42007
	EPacketQoSGetProfileParams,                                           // 42008
	EPacketQoSNotifyProfileChanged,                                       // 42009
	EPacketNotifyMbmsNetworkServiceStatusChange,					      // 42010
	EPacketGetMbmsMonitoredServicesPhase1,					  			  // 42011
	EPacketGetMbmsMonitoredServicesPhase2,						          // 42012
	EPacketEnumerateMbmsMonitorServiceList,   							  // 42013
	EPacketEnumerateMbmsActiveServiceList, 								  // 42014
	EPacketGetMbmsSessionListPhase1,	 								  // 42015
	EPacketGetMbmsSessionListPhase2 									  // 42016
	};

enum
	{
	EPacketContextEnumeratePacketFiltersCancel = EPacketContextEnumeratePacketFilters + EMobileCancelOffset,                                   // 42500
	EPacketContextGetConfigCancel = EPacketContextGetConfig + EMobileCancelOffset,                                                             // 42501
	EPacketContextGetDNSInfoCancel = EPacketContextGetDNSInfo + EMobileCancelOffset,                                                           // 42502
	EPacketContextNotifyConfigChangedCancel = EPacketContextNotifyConfigChanged + EMobileCancelOffset,                                         // 42503
	EPacketEnumerateContextsInNifCancel = EPacketEnumerateContextsInNif + EMobileCancelOffset,                                                 // 42504
	EPacketGetContextNameInNifCancel = EPacketGetContextNameInNif + EMobileCancelOffset,                                                       // 42505
	EPacketGetDefaultContextParamsCancel = EPacketGetDefaultContextParams + EMobileCancelOffset,                                               // 42506
	EPacketGetNifInfoCancel = EPacketGetNifInfo + EMobileCancelOffset,                                                                         // 42507
	EPacketQoSGetProfileParamsCancel = EPacketQoSGetProfileParams + EMobileCancelOffset,                                                       // 42508
	EPacketQoSNotifyProfileChangedCancel = EPacketQoSNotifyProfileChanged + EMobileCancelOffset,                                               // 42509
	EPacketNotifyMbmsNetworkServiceStatusChangeCancel = EPacketNotifyMbmsNetworkServiceStatusChange + EMobileCancelOffset,					   // 42510
	EPacketGetMbmsMonitoredServicesCancel = EPacketGetMbmsMonitoredServicesPhase1 + EMobileCancelOffset,							 		   // 42511
	EPacketEnumerateMbmsMonitorServiceListCancel = EPacketEnumerateMbmsMonitorServiceList + EMobileCancelOffset,							   // 42513
	EPacketEnumerateMbmsActiveServiceListCancel = EPacketEnumerateMbmsActiveServiceList + EMobileCancelOffset,								   // 42514
	EPacketGetMbmsSessionListCancel = EPacketGetMbmsSessionListPhase1 + EMobileCancelOffset													   // 42515
	};

//
// WriteDeviceData
//
enum
	{
	EPacketContextDelete = EEtelPacketIpcOffset3,                         // 43000
	EPacketContextSetConfig,                                              // 43001
	EPacketSetAttachMode,                                                 // 43002
	EPacketSetDefaultContextParams,                                       // 43003
	EPacketContextUpdateMbmsSessionList,							  	  // 43004
	EPacketUpdateMbmsMonitorServiceList								  	  // 43005
	};

enum
	{
	EPacketContextDeleteCancel = EPacketContextDelete + EMobileCancelOffset,                                                                   // 43500
	EPacketContextSetConfigCancel = EPacketContextSetConfig + EMobileCancelOffset,                                                             // 43501
	EPacketSetAttachModeCancel = EPacketSetAttachMode + EMobileCancelOffset,                                                                   // 43502
	EPacketSetDefaultContextParamsCancel = EPacketSetDefaultContextParams + EMobileCancelOffset,                                               // 43503
	EPacketContextUpdateMbmsSessionListCancel = EPacketContextUpdateMbmsSessionList + EMobileCancelOffset,									   // 43504
	EPacketUpdateMbmsMonitorServiceListCancel = EPacketUpdateMbmsMonitorServiceList + EMobileCancelOffset									   // 43505
	};

//
// ReadUserData
//
enum
	{
	EPacketContextGetDataVolumeTransferred = EEtelPacketIpcOffset4,       // 44000
	EPacketContextNotifyDataTransferred                                   // 44001
	};

enum
	{
	EPacketContextGetDataVolumeTransferredCancel = EPacketContextGetDataVolumeTransferred + EMobileCancelOffset,                               // 44500
	EPacketContextNotifyDataTransferredCancel = EPacketContextNotifyDataTransferred + EMobileCancelOffset                                      // 44501
	};

//
// NetworkServices ReadDeviceData
//
enum
	{
	EPacketContextGetPacketFilterInfo = EEtelPacketIpcOffset5,            // 45000
	EPacketGetMbmsNetworkServiceStatus									  // 45001
	};

enum
	{
	EPacketContextGetPacketFilterInfoCancel = EPacketContextGetPacketFilterInfo + EMobileCancelOffset,                                         // 45500
	EPacketGetMbmsNetworkServiceStatusCancel = EPacketGetMbmsNetworkServiceStatus + EMobileCancelOffset                                        // 45501
	};

//
// NetworkServices WriteDeviceData
//
enum
	{
	EPacketContextAddPacketFilter = EEtelPacketIpcOffset6,                // 46000
	EPacketContextRemovePacketFilter,                                     // 46001
	EPacketQoSSetProfileParams,                                           // 46002
	EPacketSetMSClass,                                                    // 46003
	EPacketSetPrefBearer,                                                 // 46004
	EPacketAddMediaAuthorization,                                         // 46005
	EPacketRemoveMediaAuthorization,                                      // 46006
	};

enum
	{
	EPacketContextAddPacketFilterCancel = EPacketContextAddPacketFilter + EMobileCancelOffset,                                                 // 46500
	EPacketContextRemovePacketFilterCancel = EPacketContextRemovePacketFilter + EMobileCancelOffset,                                           // 46501
	EPacketQoSSetProfileParamsCancel = EPacketQoSSetProfileParams + EMobileCancelOffset,                                                       // 46502
	EPacketSetMSClassCancel = EPacketSetMSClass + EMobileCancelOffset,                                                                         // 46503
	EPacketSetPrefBearerCancel = EPacketSetPrefBearer + EMobileCancelOffset,                                                                   // 46504
	EPacketAddMediaAuthorizationCancel = EPacketAddMediaAuthorization + EMobileCancelOffset,                                                   // 46505
	EPacketRemoveMediaAuthorizationCancel = EPacketRemoveMediaAuthorization + EMobileCancelOffset                                              // 46506
	};
#endif
