// Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
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
 @internalTechnology
 @released
*/

#ifndef LBSPROPERTYKEYDEFS_H
#define LBSPROPERTYKEYDEFS_H

#include <e32def.h>




//This header splits up the key space for the process that defines the LBS P&S properties.
//It then goes on to map ModuleId-Property type pairs to individual key vaules.


//We offset from 0 - this is to make debugging easier (0 means a lot of things, 1000 is a better clue).
//Also, several places in the implementation use the fact that 0 is NEVER a valid key value.

const TUint KLbsPowerModeAdviceKeyBase			= 0x00001000;		//Base value for RLbsManagerPowerModeAdvice keys
const TUint KLbsModuleInfoKeyBase				= 0x01001000;		//Base value for LbsModuleInfo keys
const TUint KLbsModuleStatusKayBase				= 0x02001000;		//Base value for RLbsModuleStatus keys
const TUint KLbsPositionUpdatesKeyBase			= 0x03001000;		//Base value for RLbsPositionUpdates keys
const TUint	KLbsGpsMeasurementUpdatesKeyBase	= 0x03001100;		//Base value for RLbsGpsMeasurementUpdates keys
const TUint KLbsPositionUpdateRequestsKeyBase	= 0x04001000;		//Base value for RLbsPositionUpdateRequests keys
const TUint KLbsSystemModuleEventKeyBase		= 0x05001000;		//Base value for RLbsSystemModuleEvent keys

// The following section lists the keys used by lbsnetinternalapi

/* Key value used to publish message data */
const TUint KNetGatewayMessageKey = 0x06001000;

/* Key value used to publish the 'message read' acknowledgement */
const TUint KNetGatewayMessageReadAckKey = 0x06001010;

/* Key value used to publish message data */
const TUint KNetRequestMessageKey = 0x06001020;

/* Key value used to publish the 'message read' acknowledgement */
const TUint KNetRequestMessageReadAckKey = 0x06001030;

/* Key value used to publish message data */
const TUint KNetworkLocationRequestMessageKey = 0x06001040;

/* Key value used to publish message data */
const TUint KNetworkLocationResponseMessageKey = 0x06001060;

/* Key value used to publish network reference position data */
const TUint KNetworkReferencePositionUpdateKey = 0x06001080;

/* Key value used to publish network final position data */
const TUint KNetworkFinalPositionUpdateKey = 0x06001088;

/* Key value used to publish messages from AGPS Manager to Network Gateway  
   and the corresponding acknowledgement */
const TUint KNetAGPS2NGMessageKey = 0x06001090;
const TUint KNetAGPS2NGMessageReadAckKey = 0x060010a0;

/* Key value used to publish messages from the Network Gateway to the AGPS Manager  
   and the corresponding acknowledgement */
const TUint KNetNG2AGPSMessageKey = 0x060010b0;
const TUint KNetNG2AGPSMessageReadAckKey = 0x060010c0;

/* Key value used to publish messages from Supl Psy to Network Gateway  
   and the corresponding acknowledgement */
const TUint KNetSUPLPSY2NGMessageKey = 0x06001091;
const TUint KNetSUPLPSY2NGMessageReadAckKey = 0x06001092;

/* Key value used to publish messages from the Network Gateway to Supl Psy   
   and the corresponding acknowledgement */
const TUint KNetNG2SUPLPSYMessageKey = 0x06001093;
const TUint KNetNG2SUPLPSYMessageReadAckKey = 0x06001094;

/* Key value used to publish network registration status */
const TUint KNetworkRegistrationStatusKey = 0x060010d0;


// End of section keys used by lbsnetinternalapi


const TUint KLbsRootProcessKeyBase      		= 0x07001000;		//Base value for LBS Root Api keys
const TUint KLbsAssistDataApiBase       		= 0x08001000;		//Base value for Assistance Data Api keys
const TUint KLbsQualityProfileKeyBase      		= 0x09001000;		//Base value for Quality Profile keys
const TUint KLbsSuplPushAPIBase		      		= 0x0A001000;		//Base value for SUPL Push API keys
const TUint KLbsPositioningStatusAPIBase   		= 0x0B001000;		//Base value for Positioning Status API keys

#endif	//LBSPROPERTYKEYDEFS_H

