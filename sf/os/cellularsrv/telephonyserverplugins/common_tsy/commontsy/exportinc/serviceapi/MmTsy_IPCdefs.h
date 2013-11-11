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
#ifndef MMTSY_IPCDEFS_H
#define MMTSY_IPCDEFS_H

//  INCLUDES

// CONSTANTS
#define IPC_TSY_PRIVATE_EXT ( 1000300 )

/** TMmTsyLayerIPC enumerates IPC values used by CommonTSY Plug-in API. */
enum TMmTsyLayerIPC
    {
    // PhoneBook Storage requests
    EMmTsyPhoneBookStoreReadIPC = IPC_TSY_PRIVATE_EXT,			//1000300
    EMmTsyPhoneBookStoreReadCancelIPC,							//1000301
    EMmTsyPhoneBookStoreWriteIPC,								//1000302
    EMmTsyPhoneBookStoreWriteCancelIPC,							//1000303
    EMmTsyPhoneBookStoreGetInfoIPC,								//1000304
    EMmTsyPhoneBookStoreGetInfoCancelIPC,						//1000305
    EMmTsyPhoneBookStoreDeleteIPC,								//1000306
    EMmTsyPhoneBookStoreDeleteCancelIPC,						//1000307
    EMmTsyPhoneBookStoreDeleteAllIPC,							//1000308
    EMmTsyPhoneBookStoreDeleteAllCancelIPC,						//1000309
    EMmTsyPhoneBookStoreNotifyStoreEventIPC,					//1000310
    EMmTsyPhoneBookStoreNotifyStoreEventCancelIPC,				//1000311
    EMmTsyPhoneBookStoreReadAllPhase1IPC,						//1000312
    EMmTsyPhoneBookStoreReadAllPhase2IPC,						//1000313
    EMmTsyPhoneBookStoreReadAllCancelIPC,						//1000314
    EMmTsyPhoneBookStoreCacheIPC,								//1000315
    EMmTsyPhoneBookStoreInitIPC,								//1000316
    EMmTsyPhoneBookStoreResetCacheIPC,							//1000317
    EMmTsyPhoneBookStoreFdnInfoIPC,								//1000318
    EMmTsyPhoneBookStoreMaxIpcLimit,							//1000319

    // Emergency Number Storage requests
    EMmTsyENStoreReadIPC,							//1000320
    EMmTsyENStoreReadCancelIPC,						//1000321
    EMmTsyENStoreWriteIPC,							//1000322
    EMmTsyENStoreWriteCancelIPC,					//1000323
    EMmTsyENStoreGetInfoIPC,						//1000324
    EMmTsyENStoreGetInfoCancelIPC,					//1000325
    EMmTsyENStoreDeleteIPC,							//1000326
    EMmTsyENStoreDeleteCancelIPC,					//1000327
    EMmTsyENStoreDeleteAllIPC,						//1000328
    EMmTsyENStoreDeleteAllCancelIPC,				//1000329
    EMmTsyENStoreNotifyStoreEventIPC,				//1000330
    EMmTsyENStoreNotifyStoreEventCancelIPC,			//1000331
    EMmTsyENStoreReadAllPhase1IPC,					//1000332
    EMmTsyENStoreReadAllPhase2IPC,					//1000333
    EMmTsyENStoreReadAllCancelIPC,					//1000334
    EMmTsyENStoreMaxIpcLimit,						//1000335
    
    // Own Number Storage requests
    EMmTsyONStoreReadIPC,						//1000336
    EMmTsyONStoreReadCancelIPC,					//1000337
    EMmTsyONStoreWriteIPC,						//1000338
    EMmTsyONStoreWriteCancelIPC,				//1000339
    EMmTsyONStoreGetInfoIPC,					//1000340
    EMmTsyONStoreGetInfoCancelIPC,				//1000341
    EMmTsyONStoreDeleteIPC,						//1000342
    EMmTsyONStoreDeleteCancelIPC,				//1000343
    EMmTsyONStoreDeleteAllIPC,					//1000344
    EMmTsyONStoreDeleteAllCancelIPC,			//1000345
    EMmTsyONStoreNotifyStoreEventIPC,			//1000346
    EMmTsyONStoreNotifyStoreEventCancelIPC,		//1000347
    EMmTsyONStoreReadAllPhase1IPC,				//1000348
    EMmTsyONStoreReadAllPhase2IPC,				//1000349
    EMmTsyONStoreReadAllCancelIPC,				//1000350
    EMmTsyONStoreStoreAllIPC,					//1000351
    EMmTsyONStoreStoreAllCancelIPC,				//1000352
    EMmTsyONStoreCountEntriesIPC,				//1000353
    EMmTsyONStoreMaxIpcLimit,					//1000354

    // Packet Request
    EPacketInitProxiesIPC,						//1000355

    // Broadcast Messaging Request
	//GSM Mode
    EMmTsyGsmBroadcastNotifyMessageReceived,	//1000356

    //WCDMA Mode
    EMmTsyWcdmaBroadcastNotifyMessageReceived,	//1000357

    //Boot Sequence Request
    EMmTsyBootNotifyModemStatusReadyIPC,		//1000358
    EMmTsyBootNotifySimStatusReadyIPC,			//1000359
    EMmTsyBootNotifySecurityReadyIPC,			//1000360
    EMmTsyBootGetRFStatusIPC,					//1000361

    // SMS Messaging Request
    EMmTsySmsSendSatMessage,			//1000362
    EMmTsyActivateSmsRouting,			//1000363
    EMmTsyDeactivateSmsRouting,			//1000364

    //Sim request
    EMmTsySimGetICCType,			//1000365
    EMmTsySimRefreshRegisterIPC,	//1000366
    EMmTsySimRefreshNowIPC,			//1000367
    EMmTsySimRefreshDoneIPC,		//1000368

    // Security requests (Custom Tsy)
    ECustomCheckSecurityCodeCancelIPC,		//1000369
    EMmTsySecurityGetSimActivePinStateIPC,	//1000370
    ECustomSecurityDeliverCodeIPC,			//1000371

    // Security requests (Phone Tsy)
    EMmTsyPhoneGetPin1DisableSupportedIPC,	//1000372

    EMmTsyNotifyEGprsInfoChangeIPC,			//1000373

    EMmTsyGetCustomVendorExtPtrIPC,			//1000374 (@deprecated. There should be no references to this.)
    
    EMmTsyONStoreReadEntryIPC,			//1000375
    EMmTsyONStoreReadSizeIPC,			//1000376
    EMmTsyONStoreWriteEntryIPC,			//1000377
    EMmTsyONStoreWriteSizeIPC,			//1000378
    //Get call forwarding number (Phonetsy)
    EMmTsyGetCallForwardingNumberIPC,		//1000379
    EPacketNotifyTransferCapsIPC,			//1000380
    EMmTsyPhoneBookStoreCacheCancelIPC,		//1000381
    
    //Update LIFE_TIMER value
    EMmTsyUpdateLifeTimeIPC    
    }; // end enum TMmTsyLayerIPC

#define IPC_CTSY_BASE ( 2000300 ) // ipc base number
#ifndef USING_CTSY_DISPATCHER
enum TCtsyIpc
	{	
	ECtsyPhoneCellInfoSync = IPC_CTSY_BASE,
	ECtsyPhoneCellInfoReq,
	ECtsyPhoneCellInfoComp,
	ECtsyPhoneCellInfoIndReq,
	ECtsyPhoneCellInfoIndComp,
	ECtsyPhoneCellInfoInd,
	ECtsyUpdateLifeTimeReq,
	ECtsyUpdateLifeTimeComp,
	ECtsyPacketMbmsInitialiseContextReq,
	ECtsyPacketMbmsInitialiseContextComp,
	ECtsyPacketMbmsContextNotifyConfigChanged,
	ECtsyPhoneTerminateAllCallsReq,
	ECtsyPhoneTerminateAllCallsComp,
	// RMobilePhone::NotifySendNetworkServiceRequest
	ECtsyPhoneSendNetworkServiceRequestReq,
	ECtsyPhoneSendNetworkServiceRequestIndReq,
	ECtsyPhoneSendNetworkServiceRequestInd,
	ECtsyPhoneTerminateActiveCallsReq,
    ECtsyPhoneTerminateActiveCallsComp
	};
#else
enum TCtsyIpc
	{	
	ECtsyPhoneCellInfoReq = IPC_CTSY_BASE,
	ECtsyPhoneCellInfoComp,
	ECtsyPhoneCellInfoIndReq,
	ECtsyPhoneCellInfoIndComp,
	ECtsyUpdateLifeTimeReq,
	ECtsyUpdateLifeTimeComp,
	ECtsyPacketMbmsInitialiseContextReq,
	ECtsyPacketMbmsInitialiseContextComp,
	ECtsyPacketMbmsContextNotifyConfigChanged,
	ECtsyPhoneTerminateAllCallsReq,
	ECtsyPhoneTerminateAllCallsComp,
	ECtsyPhoneSendNetworkServiceRequestInd,
	ECtsyGetPhoneIdV2Comp,
    ECtsyPhoneTerminateActiveCallsReq,
    ECtsyPhoneTerminateActiveCallsComp
	};
#endif //USING_CTSY_DISPATCHER

#endif // MMTSY_IPCDEFS_H
     
// End of File
