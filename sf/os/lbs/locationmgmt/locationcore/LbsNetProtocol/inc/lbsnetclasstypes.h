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
// This file defines the major data type definitions for the LBS protocol module API.
// 
//

#include <e32std.h>

/**
@file
@publishedPartner
@released
*/

#ifndef LBSNETCLASSTYPES_H_
#define LBSNETCLASSTYPES_H_

/**
 * Class types for TLbsNetSessionId and its derived classes 
 *
 * @publishedPartner
 * @released
 */
enum TLbsNetSessionIdClassType
	{
	ELbsNetSessionIdUnknownClass = 0x00,
	ELbsNetSessionIdClass        = 0x01,
	ELbsNetSessionIdArrayClass       = 0x02,
	ELbsNetSessionIdLastClass    = 0xFFFFFFFF
	};

/**
 * Class types for ELbsNetPosRequestPrivacyClass and its derived classes 
 *
 * @publishedPartner
 * @released
 */
enum TLbsNetPosRequestPrivacyClassType
	{
	ELbsNetPosRequestPrivacyUnknownClass = 0x00,
	ELbsNetPosRequestPrivacyClass        = 0x01,
	ELbsNetPosRequestPrivacyLastClass    = 0xFFFFFFFF
	};

/**
 * Class types for TLbsNetRequestQuality and its derived classes 
 *
 * @publishedPartner
 * @released
 */
enum TLbsNetRequestQualityClassType
	{
	ELbsNetPosRequestQualityUnknownClass = 0x00,
	ELbsNetPosRequestQualityClass        = 0x01,
	ELbsNetPosRequestQualityLastClass    = 0xFFFFFFFF
	};

/**
 * Class types for TLbsNetPosRequestOptions and its derived classes 
 *
 * @publishedPartner
 * @released
 */
enum TLbsNetPosRequestOptionsClassType
	{
	ELbsNetPosRequestOptionsUnknownClass    = 0x00,
	ELbsNetPosRequestOptionsBaseClass       = 0x01,
	ELbsNetPosRequestOptionsClass           = 0x02,
	ELbsNetPosRequestOptionsAssistanceClass = 0x04,
	ELbsNetPosRequestOptionsTechnologyClass = 0x08,
	ELbsNetPosRequestOptionsLastClass       = 0xFFFFFFFF
	};

/**
 * Class types for TLbsNetPosMethod and its derived classes 
 *
 * @publishedPartner
 * @released
 */
enum TLbsNetPosMethodClassType
	{
	ELbsNetPosMethodUnknownClass = 0x00,
	ELbsNetPosMethodClass        = 0x01,
	ELbsNetPosMethodLastClass    = 0xFFFFFFFF
	};


/**
 * Class types for TLbsNetPosCapabilities and its derived classes 
 *
 * @publishedPartner
 * @released
 */
enum TLbsNetPosCapabilitiesClassType
	{
	ELbsNetPosCapabilitiesUnknownClass = 0x00,
	ELbsNetPosCapabilitiesClass        = 0x01,
	ELbsNetPosCapabilitiesLastClass    = 0xFFFFFFFF
	};


/**
 * Class types for TLbsNetPosRequestMethod and its derived classes 
 *
 * @publishedPartner
 * @released
 */
enum TLbsNetPosRequestMethodClassType
	{
	ELbsNetPosRequestMethodUnknownClass = 0x00,
	ELbsNetPosRequestMethodClass        = 0x01,
	ELbsNetPosRequestMethodLastClass    = 0xFFFFFFFF
	};


/**
 * Class types for TLbsNetProtocolModuleParams and its derived classes 
 *
 * @publishedPartner
 * @released
 */
enum TLbsNetProtocolModuleParamsClassType
	{
	ELbsNetProtocolModuleParamsUnknownClass = 0x00,
	ELbsNetProtocolModuleParamsClass        = 0x01,
	ELbsNetProtocolModuleParamsLastClass    = 0xFFFFFFFF
	};


#endif // LBSNETCLASSTYPES_H_


