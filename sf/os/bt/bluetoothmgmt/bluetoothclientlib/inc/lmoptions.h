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

#ifndef LMOPTIONS_H
#define LMOPTIONS_H

// Link Manager Ioctls

/** Link manager Ioctl codes.
WARNING: For internal use ONLY. Compatibility is not guaranteed in future releases.
*/
enum TBTLMIoctls
	{
	/** Disconnect ACL Ioctl code
	@deprecated
	*/
	KLMDisconnectACLIoctl,
	/** Set Packet type Ioctl code
	@deprecated
	*/
	KLMSetPacketTypeIoctl,
	/** Wait for SCO notification Ioctl code
	@internalComponent
	*/
	KLMWaitForSCONotificationIoctl,
	/** One-shot baseband notification Ioctl code
	@internalComponent
	*/	
	KLMBasebandEventOneShotNotificationIoctl,
	/** Baseband event notification Ioctl code
	@internalComponent
	*/	
	KLMBasebandEventNotificationIoctl,
	/** Read Failed Contact Counter Ioctl
	@internalComponent
	*/	
	KLMReadFailedContactCounterIoctl,
	/** Read Link Quality Ioctl
	@internalComponent
	*/	
	KLMReadLinkQualityIoctl,
	/** Read RSSI Ioctl
	@internalComponent
	*/	
	KLMReadRssiIoctl,
	/** Read Current Transmit Power Level Ioctl
	@internalComponent
	*/	
	KLMReadCurrentTransmitPowerLevelIoctl,
	};


/** Link manager options.
@internalComponent
WARNING: For internal use ONLY. Compatibility is not guaranteed in future releases.
*/
enum TBTLMOptions
	{
	ELMOutboundACLSize,						/*!< Outbound ACL size option */
	ELMInboundACLSize,						/*!< Inbound ACL size option */
	KLMGetACLHandle,						/*!< Get ACL Handle option */
	KLMGetACLLinkCount,						/*!< Get ACL link count option */
	KLMGetACLLinkArray,						/*!< Get ACL link array option */
	KLMSetBasebandConnectionPolicy,			/*!< Set baseband connection policy option */
	KLMGetBasebandHandle,					/*!< Get baseband handle option */
	EBBSubscribePhysicalLink,				/*!< Subscribe physical link option */
	EBBBeginRaw,							/*!< Enable raw broadcast option */
	EBBRequestRoleMaster,					/*!< Request switch to master option */
	EBBRequestRoleSlave,					/*!< Request switch to slave option */
	EBBCancelModeRequest,					/*!< Cancel mode request option */
	EBBRequestSniff,						/*!< Request sniff mode option */
	EBBRequestPark,							/*!< Request park mode option */
	EBBRequestPreventRoleChange,			/*!< Request to prevent a role (master / slave) switch option */
	EBBRequestAllowRoleChange,				/*!< Request to allow a role (master / slave) switchoption */
	EBBRequestChangeSupportedPacketTypes,	/*!< Request to cange the current supported packet types option */
	EBBEnumeratePhysicalLinks,				/*!< Enumerate physical links option */
	EBBGetPhysicalLinkState,				/*!< Get the physical link state option */
	EBBGetSniffInterval,					/*!< Get Sniff Interval option */
	EBBRequestLinkAuthentication,			/*!< Request authentication on the link */
	EBBRequestExplicitActiveMode,			/*!< Explicitly request the link to go into active mode */
	
	//Allow combination of options below...
	EBBRequestPreventSniff = 0x100,			/*!< Request to prevent entering sniff mode option */
	EBBRequestPreventHold = 0x200,			/*!< Request to prevent entering hold mode option */
	EBBRequestPreventPark = 0x400,			/*!< Request to prevent entering park mode option */
	EBBRequestPreventAllLowPowerModes = 
		(EBBRequestPreventSniff | 
		 EBBRequestPreventHold | 
		 EBBRequestPreventPark),			/*!< Request to prevent entering all modes option */
	EBBRequestAllowSniff = 0x800,			/*!< Request to allow entering sniff mode option */
	EBBRequestAllowHold = 0x1000,			/*!< Request to allow entering hold mode option */
	EBBRequestAllowPark = 0x2000,			/*!< Request to allow entering park mode option */
	EBBRequestAllowAllLowPowerModes = 
		(EBBRequestAllowSniff | 
		 EBBRequestAllowHold | 
		 EBBRequestAllowPark),				/*!< Request to allow entering-all-modes option. */
	};


#endif // LMOPTIONS_H
