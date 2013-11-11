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
#ifndef CMMUTILITY_H
#define CMMUTILITY_H

// INCLUDES 
#include <et_tsy.h>
#include <etelmm.h>
#include <etelqos.h>
#include <e32cmn_private.h>

//  CONSTANTS  
/** Number of SIM file types (TCacheFileTypes) to be 
 * registered for refresh
 */ 
const TUint8 KNrOfCacheFileTypes = 5;
/** Maximum number of filters */
const TInt KMaxNumberOfTftFilters = 8;

/**
 * Dial types for defining different dial requests
 */
const TInt KMultimodeCallTypeDialNone = 0;
const TInt KMultimodeCallTypeIDNoFdnCheck = 11;

/*  DATA TYPES */  

/** RF statuses */
enum TRfStatus
    {
    ERfsInActive = 0,
    ERfsActivated,
    EEmergencyCallMade
    };

/** RF state info*/
enum TRfStateInfo
    {
    ERfsStateInfoNormal = 3,
    ERfsStateInfoInactive = 4
    };

/** The file types for which the SIM refresh is supported */
enum TCacheFileTypes
    {
    KCacheSmsParams = 0x01,
    KCacheAdn = 0x02,
    KCacheFdn = 0x04,
    KCacheServiceTable = 0x08,
    KCacheALSline = 0x10
    };

/**  TFT filter oparations*/
enum TPacketFilterFilterOperation
    {
    ETftCreateNew,
    EDeleteExisting,
    EAddPacketFilters,
    EReplacePacketFilters,
    EDeletePacketFilters,
    ETftNoOperation
    };
  
/**  Dynamic transfer capabilities */
enum TDynamicTransferCaps
    {
    KCapsHSDPA = 0x00000001,
    KCapsHSUPA = 0x00000002,
    KCapsEGPRS = 0x00000004
    };

/** Transfer capabilities flag */
typedef TUint TDynamicTransferCapsFlags;

/** PDP contect configuration */
struct TContextConfiguration
    {
    TInfoName iContextName;
    RPacketContext::TProtocolType iConfiguration;
    RPacketQoS::TQoSGPRSRequested* iQosParams97;
    RPacketQoS::TQoSR99_R4Requested* iQosParams99 ;
	RPacketQoS::TQoSR5Requested* iQosParamsR5;
    CArrayFixFlat<RPacketContext::TPacketFilterV2>* iTftFilterArray;
    TInt iFilterIdArray[ KMaxNumberOfTftFilters ];
    TPacketFilterFilterOperation iTftOperation;
    };

/** PDP context parameters */
struct TContextParams
    {
	TInfoName iContextName;
	RPacketContext::TProtocolType iPdpType;
	TBool iContextType;
	TInfoName iPrimaryContextName;
	RPacketContext::TContextStatus iContextStatus;
    };

/** Lock settings */
struct TLockAndSetting
    {
    RMobilePhone::TMobilePhoneLock* iLock;
    RMobilePhone::TMobilePhoneLockSetting* iSetting;
    };

/** structure to hold context statuses */ 
struct TContextMisc
    {
    RPacketContext::TContextStatus iStatus;
    TBuf8<1> iRejectionCause;
    };

/** Update MBMS Monitor service list parameters*/
struct TMmMbmsMonitorServiceListParams
    {
    TMbmsAction iActionType;
   	CPcktMbmsMonitoredServiceList* iServiceList;
    CPcktMbmsMonitoredServiceList* iListOfChanges;	
    };
  
/** Update MBMS active service list parameters*/
struct TMmMbmsActiveServiceListParams
    {
    TMbmsAction iActionType;
   	RPacketMbmsContext::CMbmsSession* iServiceList;
    TMbmsSessionId iChangedItem;	
    };         

#endif // CMMUTILITY_H

// End of File
