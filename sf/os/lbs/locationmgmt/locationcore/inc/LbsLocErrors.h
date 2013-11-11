// Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef LBSLOCERRORS_H
#define LBSLOCERRORS_H

#include <e32def.h>


//-------------------------------------------------------------------------------
// Transfer to third party API
//-------------------------------------------------------------------------------
/**
Panic string for Transfer to third party API
@publishedPartner
@released
 */
_LIT(KLbsLocFrameworkClientFault,"LbsTransferTPPanic");

/**
Panic codes for Lbs location framework
@publishedPartner
@released
 */
enum TTransferToThirdPartyClientPanic
	{
	/** ETransferToThirdPartySpecificAlreadyOpen */
	ETransferToThirdPartySpecificAlreadyOpen		= 0,
	/** ETransferToThirdPartySpecificDescriptorTooBig */
	ETransferToThirdPartySpecificDescriptorTooBig	= 1
	// others follow
	};

//-------------------------------------------------------------------------------
// Administration API
//-------------------------------------------------------------------------------
/**
Panic string for LBS Admin
@publishedPartner
@released
 */
_LIT(KLbsAdminFault,"LbsAdminPanic");

/**
Panic codes for Lbs Administration API
@publishedPartner
@released
 */
enum TAdminPanic
	{
	/** EAdminObserverNotDefined */
	EAdminObserverNotDefined = 0,
	/** EAdminActiveSchedulerNotInstalled */
	EAdminActiveSchedulerNotInstalled
	};

//-------------------------------------------------------------------------------
// GPS Clock Adjust API
//-------------------------------------------------------------------------------
/**
Panic string for the GPS Clock Adjust
@publishedPartner
@released
 */
_LIT(KGPSClockFault,"GPSClockPanic");

/**
Panic codes for the GPS Clock Adjust
@publishedPartner
@released
 */
enum TGPSClockPanic
	{
	/** EGPSClockUnexpectedLeave */
	EGPSClockUnexpectedLeave = 0,
	};

//-------------------------------------------------------------------------------
// Network Gateway
//-------------------------------------------------------------------------------
/**
Panic string for the Network Gateway
@publishedPartner
@released
 */
_LIT(KLbsNGFault,"LbsNGPanic");

/**
Panic codes for the Network Gateway
@publishedPartner
@released
 */
enum TNetowrkGatewayPanic
	{
	/** A channel Id of the received message does not match. */
	ENGUnexpectedNetChannelId		= 0,
	ENGUnexpectedMsgType			= 1,
	ENGUnexpectedSessionIdOrType	= 2
	};

//-------------------------------------------------------------------------------
// Network Request Handler
//-------------------------------------------------------------------------------
/**
Panic string for the Network Request Handler
@publishedPartner
@released
 */
_LIT(KLbsNRHFault,"LbsNRHPanic");

/**
Panic codes for the Network Request Handler
@publishedPartner
@released
 */
enum TNetowrkRequestHandlerPanic
	{
	/** A channel Id of the received message does not match. */
	ENRHUnexpectedNetChannelId = 0
	};

//-------------------------------------------------------------------------------
// AGPS Manager
//-------------------------------------------------------------------------------
/**
Panic string for the AGPS Manager
@publishedPartner
@released
 */
_LIT(KLbsAGPSManFault,"LbsAGPSManPanic");

/**
Panic codes for the AGPS Manager
@publishedPartner
@released
 */
enum TAGPSManagerPanic
	{
	/** A channel Id of the received message does not match. */
	EAGPSManUnexpectedNetChannelId = 0,
	EAGPSManUnknownFSMState		   = 1
	};
	
//-------------------------------------------------------------------------------

/**
Base error code for this component
@publishedPartner
@released
 */
const TInt KErrLbsLocationFrameworkBase		= -17950;

/**
End of code range for error codes for this component
@publishedPartner
@released
 */
const TInt KErrLbsLocationFrameworkLast		= -17999;

/**
Error code - X3P API stopped
@publishedPartner
@released
 */
const TInt KErrPositionX3PStopped	= KErrLbsLocationFrameworkBase;		//-17950

/**
Error code - The GPS hardware did not supply any response at all to 
a network location request. Note: if the GPS hardware supplied any
response, even an invalid on inaccurate one, this error will not be used.

@publishedPartner
@released
*/
const TInt KErrPositionNoGpsUpdate	= KErrLbsLocationFrameworkBase - 1;	//-17951

//-------------------------------------------------------------------------------
// Network Protocol Module API
//-------------------------------------------------------------------------------
/**
Panic string for the Network Protocol Module API
@publishedPartner
@released
*/
_LIT(KNetProtocolApiFault,"NetworkProtocolApiPanic");

/**
Panic codes for the Network Protocol Module API
@publishedPartner
@released
*/
enum TNetProtocolApiPanic
	{
	/** The panic may occur if due to a programming error the interface has been 
	misused. The most common misuse is an attempt to invoke a method that is no 
	longer supported by an extended version of the interface.*/
	ENetProtocolApiExtendedInterfaceMisuse = 0,
	};
	
//-------------------------------------------------------------------------------
// Location monitor server
//-------------------------------------------------------------------------------	
/**
Panic string for the Location monitor client API
@publishedPartner
@released
*/
_LIT(KLocationMonitorFault,"LocationMonitorClient");

/**
Panic codes for the Location monitor client API
@publishedPartner
@released
*/
enum TLocationMonitorClientPanic
	{
	/** 
   * A client has called an asynchronous request while it is still outstanding.
   */
  ELocMonitorDuplicateRequest = 0,
	};
#endif // LBSLOCERRORS_H
