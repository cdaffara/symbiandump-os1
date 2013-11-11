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
* Name        : MSatTsy_IPCDefs.h
* Part of     : SIM ATK TSY / commonsimatktsy
* Interface   : Internal, Common TSY Plugin API
* Contains IPC definitions used by Multimode TSY Layering.
* Version     : 1.0
*
*/



#ifndef MSATTSY_IPCDEFS_H
#define MSATTSY_IPCDEFS_H

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


#endif // MSatTsy_IPCDefs_H
           
// End of File
