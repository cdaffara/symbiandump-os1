/**
* Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
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
 @file lbsnetprotocolproxydefs.h
 @internalComponent
 @test
*/
#if (!defined LBSNETPROTOCOLPROXYDEFS_H)
#define LBSNETPROTOCOLPROXYDEFS_H

#include <e32base.h>
#include <e32std.h>
#include <e32debug.h>

#include <lbs/test/proppipe.h>

const TUint32 KSecond	= 1000 * 1000;

const TUint32 KResponseChannelPipeKey = 0x07897651;
const TUint32 KResponseChannelFlowKey = 0x07897652;
const TUint32 KResponseChannelWriteTimeout = 60 * KSecond;

const TUint32 KActionChannelPipeKey = 0x07897653;
const TUint32 KActionChannelFlowKey = 0x07897654;
const TUint32 KActionChannelWriteTimeout = 60 * KSecond;

const TUint32 KProtocolResponseTypeBase = 2000;

_LIT8(KEoSMarker, "!!");

enum TNetProtocolActionType
	{
	ENetMsgNullAction = 0,
	ENetMsgProcessPrivacyRequest = 1000,
	ENetMsgProcessLocationRequest,
	ENetMsgProcessSessionComplete,
	ENetMsgProcessAssistanceData,
	ENetMsgProcessLocationUpdate,
	ENetMsgGetCurrentCapabilitiesRequest,
	ENetMsgProcessStatusUpdate,
	ENetMsgActionExtendedInterface,
	};
	
enum TNetProtocolResponseType
	{
	ENetMsgTimeoutExpired = -1000,
	ENetMsgNullResponse = 0,
	ENetMsgRespondPrivacyRequest = KProtocolResponseTypeBase,
	ENetMsgRespondLocationRequest,
	ENetMsgRequestTransmitLocation,
	ENetMsgCancelTransmitLocation,
	ENetMsgRequestAssistanceData,
	ENetMsgRequestSelfLocation,
	ENetMsgCancelSelfLocation,
	ENetMsgRequestNetworkLocation,
	ENetMsgCancelNetworkLocation,
	ENetMsgGetCurrentCapabilitiesResponse,	//assumed to map onto 	ENetMsgAdviceSystemStatus
	ENetMsgResponseExtendedInterface,	
	};


enum TNetProtocolResponseFilter
	{
	EResponseFilterNone = 0,
	EResponseFilterMsgRespondPrivacyRequest				= 1 << 0,
	EResponseFilterMsgRespondLocationRequest			= 1 << 1,
	EResponseFilterMsgRequestTransmitLocation			= 1 << 2,
	EResponseFilterMsgCancelTransmitLocation			= 1 << 3,
	EResponseFilterMsgRequestAssistanceData				= 1 << 4,
	EResponseFilterMsgRequestSelfLocation				= 1 << 5,
	EResponseFilterMsgCancelSelfLocation				= 1 << 6,
	EResponseFilterMsgRequestNetworkLocation			= 1 << 7,
	EResponseFilterMsgCancelNetworkLocation				= 1 << 8,
	EResponseFilterMsgGetCurrentCapabilitiesResponse	= 1 << 9,	
	EResponseFilterMsgResponseExtendedInterface			= 1 << 10	
	};

#endif // LBSNETPROTOCOLPROXYDEFS_H
